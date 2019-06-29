#include "stdafx.h"
#include "dxerr9.h"
#include "net_server.h"

#include "NET_Log.h"
static	INetLog* pSvNetLog = NULL; 

#define		BASE_PORT		5445

void	dump_URL	(LPCSTR p, IDirectPlay8Address* A);

LPCSTR nameTraffic	= "traffic.net";
#define	BANNED_LIST		"banned.ltx"

XRNETSERVER_API int		psNET_ServerUpdate	= 30;		// FPS
XRNETSERVER_API int		psNET_ServerPending	= 2;


void IClientStatistic::Update(DPN_CONNECTION_INFO& CI)
{
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global-dwBaseTime >= 999)
	{
		dwBaseTime		= time_global;
		
		mps_recive		= CI.dwMessagesReceived - mps_receive_base;
		mps_receive_base= CI.dwMessagesReceived;

		u32	cur_msend	= CI.dwMessagesTransmittedHighPriority+CI.dwMessagesTransmittedNormalPriority+CI.dwMessagesTransmittedLowPriority;
		mps_send		= cur_msend - mps_send_base;
		mps_send_base	= cur_msend;

		dwBytesPerSec = (dwBytesPerSec*9 + dwBytesSended)/10;
		dwBytesSended = 0;
	}
	ci_last	= CI;
}

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b, 0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };
// {8D3F9E5E-A3BD-475b-9E49-B0E77139143C}
static const GUID CLSID_NETWORKSIMULATOR_DP8SP_TCPIP =
{ 0x8d3f9e5e, 0xa3bd, 0x475b, { 0x9e, 0x49, 0xb0, 0xe7, 0x71, 0x39, 0x14, 0x3c } };

static HRESULT WINAPI Handler (PVOID pvUserContext, DWORD dwMessageType, PVOID pMessage)
{
	IPureServer* C			= (IPureServer*)pvUserContext;
	return C->net_Handler	(dwMessageType,pMessage);
}

IPureServer::IPureServer	(CTimer* timer)
#ifdef PROFILE_CRITICAL_SECTIONS
	:csPlayers(MUTEX_PROFILE_ID(IPureServer::csPlayers))
	,csMessage(MUTEX_PROFILE_ID(IPureServer::csMessage))
#endif // PROFILE_CRITICAL_SECTIONS
{
	device_timer			= timer;
	stats.clear				();
	stats.dwSendTime		= TimeGlobal(device_timer);
	SV_Client = NULL;

	pSvNetLog = NULL;//xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
}

IPureServer::~IPureServer	()
{
	for	(u32 it=0; it<BannedAddresses.size(); it++)	xr_delete(BannedAddresses[it]);
	BannedAddresses.clear();
	SV_Client = NULL;

	xr_delete(pSvNetLog); pSvNetLog = NULL;
}

void IPureServer::pCompress	(NET_Packet& D, NET_Packet& S)
{
	// Update freq
	BYTE*	it		= S.B.data;
	BYTE*	end		= it+S.B.count;
	for (; it!=end; )	traffic_out[*it++]++;

	// Compress data
	D.B.count		= net_Compressor.Compress(D.B.data,S.B.data,S.B.count);

	// Update statistic
	stats.bytes_out			+= S.B.count;
	stats.bytes_out_real	+= D.B.count;
}
void IPureServer::pDecompress(NET_Packet& D, void* m_data, u32 m_size)
{
	// Decompress data
	D.B.count		=  net_Compressor.Decompress(D.B.data,LPBYTE(m_data),m_size);

	// Update statistic
	stats.bytes_in		+= D.B.count;
	stats.bytes_in_real	+= m_size;

	// Update freq
	BYTE*	it		= D.B.data;
	BYTE*	end		= it+D.B.count;
	for (; it!=end; )	traffic_in[*it++]++;
}

void IPureServer::config_Load()
{
	//************ Build sys_config
	
	// traffic in

	string_path			pth;
	FS.update_path		(pth,"$app_data_root$", nameTraffic);

	IReader*		F	= FS.r_open(pth);
	if (F && F->length()) {
		F->r	(&traffic_in,sizeof(traffic_in));
		F->r	(&traffic_out,sizeof(traffic_out));
		F->close();
		traffic_in.Normalize	();
		traffic_out.Normalize	();
	} else {
		traffic_in.setIdentity	();
		traffic_out.setIdentity	();
	}

	// build message
	msgConfig.sign1				= 0x12071980;
	msgConfig.sign2				= 0x26111975;
	u32	I;
	for (I=0; I<256; I++)		msgConfig.send[I]		= u16(traffic_in[I]);
	for (I=0; I<256; I++)		msgConfig.receive[I]	= u16(traffic_out[I]);

	// initialize compressors
	net_Compressor.Initialize	(traffic_out,traffic_in);
}

void IPureServer::config_Save	()
{
	string_path			pth;
	FS.update_path		(pth,"$app_data_root$", nameTraffic);
	// traffic in
	IWriter*		fs	= FS.w_open	(pth);
	if(fs==NULL) return;
	fs->w				(&traffic_in,sizeof(traffic_in));
	fs->w				(&traffic_out,sizeof(traffic_out));
	FS.w_close		(fs);
}

void IPureServer::Reparse	()
{
	config_Save			();
	config_Load			();
	ClientID ID;ID.set(0);
	SendBroadcast_LL	(ID,&msgConfig,sizeof(msgConfig));
}

BOOL IPureServer::Connect(LPCSTR options)
{
	// Parse options
	string4096				session_name;
	string4096				session_options = "";
	string64				password_str = "";
	u32						dwMaxPlayers = 0;

	strcpy					(session_name,options);
	if (strchr(session_name,'/'))	*strchr(session_name,'/')=0;
	if (strchr(options,'/'))		strcpy(session_options, strchr(options,'/')+1);
	if (strstr(options, "psw="))
	{
		char* PSW = strstr(options, "psw=") + 4;
		if (strchr(PSW, '/')) 
			strncpy(password_str, PSW, strchr(PSW, '/') - PSW);
		else
			strncpy(password_str, PSW, 63);
	}
	if (strstr(options, "maxplayers="))
	{
		char* sMaxPlayers = strstr(options, "maxplayers=") + 11;
		string64 tmpStr = "";
		if (strchr(sMaxPlayers, '/')) 
			strncpy(tmpStr, sMaxPlayers, strchr(sMaxPlayers, '/') - sMaxPlayers);
		else
			strncpy(tmpStr, sMaxPlayers, 63);
		dwMaxPlayers = atol(tmpStr);
	}
	Msg("MaxPlayers = %d", dwMaxPlayers);

	//---------------------------
#ifdef DEBUG
	string1024 tmp;
#endif // DEBUG
//	HRESULT CoInitializeExRes = CoInitializeEx(NULL, 0);	
//	if (CoInitializeExRes != S_OK && CoInitializeExRes != S_FALSE)
//	{
//		DXTRACE_ERR(tmp, CoInitializeExRes);
//		CHK_DX(CoInitializeExRes);
//	};	
	//---------------------------
    // Create the IDirectPlay8Client object.
	HRESULT CoCreateInstanceRes = CoCreateInstance	(CLSID_DirectPlay8Server, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server, (LPVOID*) &NET);
	//---------------------------	
	if (CoCreateInstanceRes != S_OK)
	{
		DXTRACE_ERR(tmp, CoCreateInstanceRes );
		CHK_DX(CoCreateInstanceRes );
	}	
	//---------------------------
	
    // Initialize IDirectPlay8Client object.
#ifdef DEBUG
    CHK_DX(NET->Initialize	(this, Handler, 0));
#else
	CHK_DX(NET->Initialize	(this, Handler, DPNINITIALIZE_DISABLEPARAMVAL));
#endif

	BOOL	bSimulator		= FALSE;
	if (strstr(Core.Params,"-netsim"))		bSimulator = TRUE;
	
	
	// dump_URL		("! sv ",	net_Address_device);

	// Set server-player info
    DPN_APPLICATION_DESC		dpAppDesc;
    DPN_PLAYER_INFO				dpPlayerInfo;
    WCHAR						wszName		[] = L"XRay Server";
	
    ZeroMemory					(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize			= sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags	= DPNINFO_NAME;
    dpPlayerInfo.pwszName		= wszName;
    dpPlayerInfo.pvData			= NULL;
    dpPlayerInfo.dwDataSize		= NULL;
    dpPlayerInfo.dwPlayerFlags	= 0;
	
	CHK_DX(NET->SetServerInfo( &dpPlayerInfo, NULL, NULL, DPNSETSERVERINFO_SYNC ) );
	
    // Set server/session description
	WCHAR	SessionNameUNICODE[4096];
	CHK_DX(MultiByteToWideChar(CP_ACP, 0, session_name, -1, SessionNameUNICODE, 4096 ));
    // Set server/session description
	
    // Now set up the Application Description
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.dwFlags			= DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
    dpAppDesc.guidApplication	= NET_GUID;
    dpAppDesc.pwszSessionName	= SessionNameUNICODE;
	dpAppDesc.dwMaxPlayers		= (dwMaxPlayers == 0) ? 0 : (dwMaxPlayers+1);
	dpAppDesc.pvApplicationReservedData	= session_options;
	dpAppDesc.dwApplicationReservedDataSize = xr_strlen(session_options)+1;

	WCHAR	SessionPasswordUNICODE[4096];
	if (xr_strlen(password_str))
	{
		CHK_DX(MultiByteToWideChar(CP_ACP, 0, password_str, -1, SessionPasswordUNICODE, 4096 ));
		dpAppDesc.dwFlags |= DPNSESSION_REQUIREPASSWORD;
		dpAppDesc.pwszPassword = SessionPasswordUNICODE;
	};
	
	// Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device = NULL;
	CHK_DX(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
	CHK_DX(net_Address_device->SetSP		(bSimulator? &CLSID_NETWORKSIMULATOR_DP8SP_TCPIP : &CLSID_DP8SP_TCPIP ));
	
	HRESULT HostSuccess = S_FALSE;
	// We are now ready to host the app and will try different ports
	psNET_Port = BASE_PORT;
	while (HostSuccess != S_OK && psNET_Port <=BASE_PORT + 100)
	{
		CHK_DX(net_Address_device->AddComponent	(DPNA_KEY_PORT, &psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));

		HostSuccess = NET->Host			
			(
			&dpAppDesc,				// AppDesc
			&net_Address_device, 1, // Device Address
			NULL, NULL,             // Reserved
			NULL,                   // Player Context
			0 );					// dwFlags
		if (HostSuccess != S_OK)
		{
//			xr_string res = Debug.error2string(HostSuccess);
#ifdef DEBUG
				Msg("! IPureServer : port %d is BUSY!", psNET_Port);
#endif	
				psNET_Port++;
		};
	};
	
	CHK_DX(HostSuccess);
	
	config_Load		();

	BannedList_Load	();

	return	TRUE;
}

void IPureServer::Disconnect	()
{
	config_Save		();

	BannedList_Save	();

    if( NET )	NET->Close(0);
	
	// Release interfaces
    _RELEASE	(net_Address_device);
    _RELEASE	(NET);
}

#define BANNED_STR	"Player Banned by Server!"
HRESULT	IPureServer::net_Handler(u32 dwMessageType, PVOID pMessage)
{
    // HRESULT     hr = S_OK;
	
    switch (dwMessageType)
    {
	case DPN_MSGID_ENUM_HOSTS_QUERY :
		{
			PDPNMSG_ENUM_HOSTS_QUERY	msg = PDPNMSG_ENUM_HOSTS_QUERY(pMessage);
			if (0 == msg->dwReceivedDataSize) return S_FALSE;
			if (!stricmp((const char*)msg->pvReceivedData, "ToConnect")) return S_OK;
			if (*((const GUID*) msg->pvReceivedData) != NET_GUID) return S_FALSE;
			if (!OnCL_QueryHost()) return S_FALSE;
			return S_OK;
		}break;
	case DPN_MSGID_CREATE_PLAYER :
        {
			PDPNMSG_CREATE_PLAYER	msg = PDPNMSG_CREATE_PLAYER(pMessage);
			const	u32				max_size = 1024;
			char	bufferData		[max_size];
            DWORD	bufferSize		= max_size;
			ZeroMemory				(bufferData,bufferSize);

			// retreive info
			DPN_PLAYER_INFO*		Pinfo = (DPN_PLAYER_INFO*) bufferData;
			Pinfo->dwSize			= sizeof(DPN_PLAYER_INFO);
			HRESULT _hr				= NET->GetClientInfo(msg->dpnidPlayer,Pinfo,&bufferSize,0);
			if (_hr==DPNERR_INVALIDPLAYER)	break;	// server player
			CHK_DX					(_hr);
			
			string256				cname;
			CHK_DX(WideCharToMultiByte(CP_ACP,0,Pinfo->pwszName,-1,cname,sizeof(cname),0,0));
			
			bool bLocal				= (Pinfo->dwPlayerFlags&DPNPLAYER_LOCAL) != 0;
			ClientID clientID; clientID.set(msg->dpnidPlayer);
			new_client				(clientID, cname, bLocal);
        }
		break;
	case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER	msg = PDPNMSG_DESTROY_PLAYER(pMessage);

			csPlayers.Enter			();
			for (u32 I=0; I<net_Players.size(); I++)
//				if (net_Players[I]->ID==msg->dpnidPlayer)
				if (net_Players[I]->ID.compare(msg->dpnidPlayer) )
				{
					// gen message
					net_Players[I]->flags.bConnected	= FALSE;
					net_Players[I]->flags.bReconnect	= FALSE;
					OnCL_Disconnected	(net_Players[I]);

//					// real destroy
					client_Destroy		(net_Players[I]);
//					net_Players.erase	(net_Players.begin()+I);
					break;
				}
			csPlayers.Leave			();
		}
		break;
	case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE	msg = PDPNMSG_RECEIVE(pMessage);
			void*	m_data		= msg->pReceiveData;
			u32		m_size		= msg->dwReceiveDataSize;
			DPNID   m_sender	= msg->dpnidSender;

			MSYS_PING*	m_ping	= (MSYS_PING*)m_data;
			if ((m_size>2*sizeof(u32)) && (m_ping->sign1==0x12071980) && (m_ping->sign2==0x26111975))
			{
				// this is system message
				if (m_size==sizeof(MSYS_PING))
				{
					// ping - save server time and reply
					m_ping->dwTime_Server	= TimerAsync(device_timer);
					ClientID ID; ID.set(m_sender);
					IPureServer::SendTo_LL	(ID,m_data,m_size,net_flags(FALSE,FALSE,TRUE));
				}
			} else {
				// Decompress message
				NET_Packet P;
				pDecompress		(P,m_data,m_size);
				csMessage.Enter	();
				ClientID ID; ID.set(m_sender);
				//---------------------------------------
				if (psNET_Flags.test(NETFLAG_LOG_SV_PACKETS)) 
				{
					if (!pSvNetLog) pSvNetLog = xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
					if (pSvNetLog) pSvNetLog->LogPacket(TimeGlobal(device_timer), &P, TRUE);
				}
				//---------------------------------------
				u32	result		= OnMessage(P,ID);
				csMessage.Leave	();
				if (result)		SendBroadcast(ID,P,result);
			}
        }
        break;
	case DPN_MSGID_INDICATE_CONNECT :
		{
			PDPNMSG_INDICATE_CONNECT msg = (PDPNMSG_INDICATE_CONNECT)pMessage;

			char HAddr[4] = {0, 0, 0, 0};
			GetClientAddress(msg->pAddressPlayer, HAddr);

			if (GetBannedClient(HAddr)) 
			{
				msg->dwReplyDataSize = xr_strlen(BANNED_STR);
				msg->pvReplyData = BANNED_STR;
				return S_FALSE;
			};
		}break;
    }
	
    return S_OK;
}

void	IPureServer::SendTo_LL(ClientID ID/*DPNID ID*/, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
//	if (psNET_Flags.test(NETFLAG_LOG_SV_PACKETS)) pSvNetLog->LogData(TimeGlobal(device_timer), data, size);
	if (psNET_Flags.test(NETFLAG_LOG_SV_PACKETS)) 
	{
		if (!pSvNetLog) pSvNetLog = xr_new<INetLog>("logs\\net_sv_log.log", TimeGlobal(device_timer));
		if (pSvNetLog) pSvNetLog->LogData(TimeGlobal(device_timer), data, size);
	}
	// send it
	DPN_BUFFER_DESC		desc;
	desc.dwBufferSize	= size;
	desc.pBufferData	= LPBYTE(data);

#ifdef _DEBUG
	u32 time_global		= TimeGlobal(device_timer);
	if (time_global - stats.dwSendTime >= 999)
	{
		stats.dwBytesPerSec = (stats.dwBytesPerSec*9 + stats.dwBytesSended)/10;
		stats.dwBytesSended = 0;
		stats.dwSendTime = time_global;
	};
	if ( ID.value() )
		stats.dwBytesSended += size;
#endif

	// verify
	VERIFY		(desc.dwBufferSize);
	VERIFY		(desc.pBufferData);

    DPNHANDLE	hAsync	= 0;
	HRESULT		_hr		= NET->SendTo(
		ID.value(),
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags | DPNSEND_COALESCE 
		);
	if (SUCCEEDED(_hr) || (DPNERR_CONNECTIONLOST==_hr))	return;

	R_CHK		(_hr);
}

void	IPureServer::SendTo		(ClientID ID/*DPNID ID*/, NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	// first - compress message and setup buffer
	NET_Packet	Compressed;
	pCompress	(Compressed,P);

	SendTo_LL	(ID,Compressed.B.data,Compressed.B.count,dwFlags,dwTimeout);
}

void	IPureServer::SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags)
{
	csPlayers.Enter	();
	for (u32 I=0; I<net_Players.size(); I++)
	{
		IClient* PLAYER		= net_Players[I];
		if (PLAYER->ID==exclude)		continue;
		if (!PLAYER->flags.bConnected)	continue;
		SendTo_LL	(PLAYER->ID, data, size, dwFlags);
	}
	csPlayers.Leave	();
}

void	IPureServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
	// Perform broadcasting
	NET_Packet			BCast;
	pCompress			(BCast,P);
	SendBroadcast_LL	(exclude, BCast.B.data, BCast.B.count, dwFlags);
}

u32	IPureServer::OnMessage	(NET_Packet& P, ClientID sender)	// Non-Zero means broadcasting with "flags" as returned
{
	/*
	u16 m_type;
	P.r_begin	(m_type);
	switch (m_type)
	{
	case M_CHAT:
		{
			char	buffer[256];
			P.r_string(buffer);
			printf	("RECEIVE: %s\n",buffer);
		}
		break;
	}
	*/
	
	return 0;
}

void IPureServer::OnCL_Connected		(IClient* CL)
{
	Msg("* Player '%s' connected.\n",	CL->Name);
}
void IPureServer::OnCL_Disconnected		(IClient* CL)
{
	Msg("* Player '%s' disconnected.\n",CL->Name);
}

/*
void IPureServer::client_link_aborted	(IClient* CL)
{
}
void IPureServer::client_link_aborted	(DPNID ID)
{
}
*/

BOOL IPureServer::HasBandwidth			(IClient* C)
{
	u32	dwTime			= TimeGlobal(device_timer);
	u32	dwInterval		= 0;
	
	if (psNET_ServerUpdate != 0) dwInterval = 1000/psNET_ServerUpdate; 
	if	(psNET_Flags.test(NETFLAG_MINIMIZEUPDATES))	dwInterval	= 1000;	// approx 2 times per second

	HRESULT hr;
	if (psNET_ServerUpdate != 0 && (dwTime-C->dwTime_LastUpdate)>dwInterval)
	{
		// check queue for "empty" state
		DWORD				dwPending;
		hr					= NET->GetSendQueueInfo(C->ID.value(),&dwPending,0,0);
		if (FAILED(hr))		return FALSE;

		if (dwPending > u32(psNET_ServerPending))	
		{
			C->stats.dwTimesBlocked++;
			return FALSE;
		};

		UpdateClientStatistic(C);
		// ok
		C->dwTime_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}

void	IPureServer::UpdateClientStatistic		(IClient* C)
{
	// Query network statistic for this client
	DPN_CONNECTION_INFO	CI;
	ZeroMemory			(&CI,sizeof(CI));
	CI.dwSize			= sizeof(CI);
	HRESULT hr					= NET->GetConnectionInfo(C->ID.value(),&CI,0);
	if (FAILED(hr))		return;
	C->stats.Update		(CI);
}

void	IPureServer::ClearStatistic	()
{
	stats.clear();
	for (u32 I=0; I<net_Players.size(); I++)
	{
		net_Players[I]->stats.Clear();
	}
};

bool			IPureServer::DisconnectClient	(IClient* C)
{
	if (!C) return false;

	string64 Reason = "Kicked by server";
	HRESULT res = NET->DestroyClient(C->ID.value(), Reason, xr_strlen(Reason)+1, 0);
	CHK_DX(res);
	return true;
};

bool			IPureServer::DisconnectAddress	(char* Address)
{
	IClient* PlayersToDisconnect[256];
	u32 NumPlayers = 0;
	IBannedClient	tmpBanCl(Address, 0);
	for (u32 it = 0; it<net_Players.size(); it++)
	{
		char ClAddress[4];
		GetClientAddress(net_Players[it]->ID, ClAddress);
		if (tmpBanCl == ClAddress)
		{
			PlayersToDisconnect[NumPlayers++] = net_Players[it];
		};
	};

	if (!NumPlayers) return false;

	for (it = 0; it<NumPlayers; it++)
	{
		DisconnectClient(PlayersToDisconnect[it]);
	};
	return true;
};

bool			IPureServer::GetClientAddress	(IDirectPlay8Address* pClientAddress, char* Address, DWORD* pPort)
{
	if (!Address) return false;
	Address[0] = 0; Address[1] = 0; Address[2] = 0; Address[3] = 0;
	if (!pClientAddress) return false;

	WCHAR wstrHostname[ 256 ] = {0};
	DWORD dwSize = sizeof(wstrHostname);
	DWORD dwDataType = 0;
	CHK_DX(pClientAddress->GetComponentByName( DPNA_KEY_HOSTNAME, wstrHostname, &dwSize, &dwDataType ));

	string256				HostName;
	CHK_DX(WideCharToMultiByte(CP_ACP,0,wstrHostname,-1,HostName,sizeof(HostName),0,0));
	char a[4][4] = {0, 0, 0, 0};
	sscanf(HostName, "%[^'.'].%[^'.'].%[^'.'].%s", a[0], a[1], a[2], a[3]);

	for (int i=0; i<4; i++)
	{
		Address[i] = char(atol(a[i]));
	};

	if (pPort != NULL)
	{
		DWORD dwPort = 0;
		DWORD dwPortSize = sizeof(dwPort);
		DWORD dwPortDataType = DPNA_DATATYPE_DWORD;
		CHK_DX(pClientAddress->GetComponentByName( DPNA_KEY_PORT, &dwPort, &dwPortSize, &dwPortDataType ));
		*pPort = dwPort;
	};

	return true;
};

bool			IPureServer::GetClientAddress	(ClientID ID, char* Address, DWORD* pPort)
{
	if (!Address) return false;
	Address[0] = 0; Address[1] = 0; Address[2] = 0; Address[3] = 0;

	IDirectPlay8Address* pClAddr = NULL;
	CHK_DX(NET->GetClientAddress(ID.value(), &pClAddr, 0));

	return GetClientAddress(pClAddr, Address, pPort);
};

IBannedClient*			IPureServer::GetBannedClient	(const char* Address)
{
	for	(u32 it=0; it<BannedAddresses.size(); it++)	
	{
		IBannedClient* pBClient = BannedAddresses[it];
		if ((*pBClient) == Address ) return pBClient;
	}
	return NULL;
};

void			IPureServer::BanClient			(IClient* C, u32 BanTime)
{
	char ClAddress[4];
	GetClientAddress(C->ID, ClAddress);
	BanAddress(ClAddress, BanTime);
};

void			IPureServer::BanAddress			(char* Address, u32 BanTime)
{
	if (!Address) return;
	if (GetBannedClient(Address)) 
	{
		Msg("Already banned\n");
		return;
	};

	IBannedClient* pNewClient = xr_new<IBannedClient>(Address, BanTime);
	if (pNewClient) BannedAddresses.push_back(pNewClient);
};

void			IPureServer::BannedAddress_Save	(u32 it, IWriter* fs)
{
	if (!fs) return;
	if (it >= BannedAddresses.size()) return;

	char* HAddr = BannedAddresses[it]->HAddr;
//	string1024 WriteStr = "";
	fs->w_printf("sv_banplayer ip %i.%i.%i.%i\n", 
		unsigned char(HAddr[0]), 
		unsigned char(HAddr[1]), 
		unsigned char(HAddr[2]), 
		unsigned char(HAddr[3]));
};

void			IPureServer::BannedList_Save	()
{
	string256					temp;
	FS.update_path				(temp,"$app_data_root$", BANNED_LIST);
	IWriter*		fs	= FS.w_open(temp);
	for	(u32 it=0; it<BannedAddresses.size(); it++)
	{
		BannedAddress_Save(it, fs);
	};
	FS.w_close		(fs);
}

LPCSTR			IPureServer::GetBannedListName	()
{
//	string256					temp;
//	FS.update_path				(temp,"$app_data_root$", BANNED_LIST);
	return (LPCSTR)BANNED_LIST;
}