#include "stdafx.h"
#include "net_client.h"
#include "net_messages.h"

void	dump_URL	(LPCSTR p, IDirectPlay8Address* A)
{
	string256	aaaa;
	DWORD		aaaa_s			= sizeof(aaaa);
	R_CHK		(A->GetURLA(aaaa,&aaaa_s));
	Log			(p,aaaa);
}

// 
INetQueue::INetQueue()		
{
	unused.reserve	(128);
	for (int i=0; i<16; i++)
		unused.push_back	(xr_new<NET_Packet>());
}
INetQueue::~INetQueue()
{
	cs.Enter		();
	u32				it;
	for				(it=0; it<unused.size(); it++)	xr_delete(unused[it]);
	for				(it=0; it<ready.size(); it++)	xr_delete(ready[it]);
	cs.Leave		();
}
NET_Packet*		INetQueue::Create	()
{
	NET_Packet*	P			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<NET_Packet> ());
		P					= ready.back	();
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		P					= ready.back	();
	}
	cs.Leave		();
	return	P;
}
NET_Packet*		INetQueue::Create	(const NET_Packet& _other)
{
	NET_Packet*	P			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<NET_Packet> ());
		P					= ready.back	();
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		P					= ready.back	();
	}
	Memory.mem_copy	(P,&_other,sizeof(NET_Packet));
	cs.Leave		();
	return			P;
}
NET_Packet*		INetQueue::Retreive	()
{
	NET_Packet*	P			= 0;
	cs.Enter		();
	if (!ready.empty())		P = ready.front();
	cs.Leave		();
	return	P;
}
void			INetQueue::Release	()
{
	cs.Enter		();
	VERIFY			(!ready.empty());
	unused.push_back(ready.front());
	ready.pop_front	();
	cs.Leave		();
}

//
const u32 syncQueueSize		= 512;
const int syncSamples		= 256;
class XRNETSERVER_API syncQueue
{
	u32				table	[syncQueueSize];
	u32				write;
	u32				count;
public:
	syncQueue()			{ clear(); }

	IC void			push	(u32 value)
	{
		table[write++]	= value;
		if (write == syncQueueSize)	write = 0;

		if (count <= syncQueueSize)	count++;
	}
	IC u32*		begin	()	{ return table;			}
	IC u32*		end		()	{ return table+count;	}
	IC u32		size	()	{ return count;			}
	IC void     clear	()	{ write=0; count=0;		}
} net_DeltaArray;

//-------
XRNETSERVER_API Flags32	psNET_Flags			= {0};
XRNETSERVER_API int		psNET_ClientUpdate	= 30;		// FPS
XRNETSERVER_API int		psNET_ClientPending	= 2;
XRNETSERVER_API char	psNET_Name[32]		= "Player";

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID NET_GUID = 
{ 0x218fa8b,  0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };

// {8D3F9E5E-A3BD-475b-9E49-B0E77139143C}
static const GUID CLSID_NETWORKSIMULATOR_DP8SP_TCPIP =
{ 0x8d3f9e5e, 0xa3bd, 0x475b, { 0x9e, 0x49, 0xb0, 0xe7, 0x71, 0x39, 0x14, 0x3c } };

static HRESULT WINAPI Handler (PVOID pvUserContext, DWORD dwMessageType, PVOID pMessage)
{
	IPureClient* C = (IPureClient*)pvUserContext;
	return C->net_Handler(dwMessageType,pMessage);
}

IPureClient::IPureClient	(CTimer* timer): net_Statistic(timer)
{
	device_timer			= timer;
	net_TimeDelta_User		= 0;
	net_Time_LastUpdate		= 0;
	net_TimeDelta			= 0;
	net_TimeDelta_Calculated = 0;
}

IPureClient::~IPureClient	()
{
}

BOOL IPureClient::Connect	(LPCSTR options)
{
	R_ASSERT						(options);

	//
	string64						server_name;
	strcpy							(server_name,options);
	if (strchr(server_name,'/'))	*strchr(server_name,'/') = 0;

	string64				password_str = "";
	if (strstr(options, "psw="))
	{
		char* PSW = strstr(options, "psw=") + 4;
		if (strchr(PSW, '/')) 
			strncpy(password_str, PSW, strchr(PSW, '/') - PSW);
		else
			strcpy(password_str, PSW);
	}

	
	int				psNET_Port	= 5445;
	if (strstr(options, "port="))
	{
		string64	portstr;
        strcpy(portstr, strstr(options, "port=")+5);
		if (strchr(portstr,'/'))	*strchr(portstr,'/') = 0;
		psNET_Port = atol(portstr);
	};
	Msg("* Client connect on port %d\n",psNET_Port);
/*
	string4096				session_name;
	string4096				session_options = "";

	strcpy					(session_name,options);
	if (strchr(session_name,'/'))	*strchr(session_name,'/')=0;
	if (strchr(options,'/'))	strcpy(session_options, strchr(options,'/')+1);
*/


	//
	net_Connected	= FALSE;
	net_Syncronised	= FALSE;
	net_Disconnected= FALSE;

    // Create the IDirectPlay8Client object.
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Client, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Client, (LPVOID*) &NET));
	
    // Initialize IDirectPlay8Client object.
#ifdef DEBUG
    R_CHK(NET->Initialize	(this, Handler, 0));
#else 
	R_CHK(NET->Initialize	(this, Handler, DPNINITIALIZE_DISABLEPARAMVAL ));
#endif
	BOOL	bSimulator		= FALSE;
	if (strstr(Core.Params,"-netsim"))		bSimulator = TRUE;
	
    // Create our IDirectPlay8Address Device Address, --- Set the SP for our Device Address
	net_Address_device	= NULL;
//	u32 c_port			= psNET_Port+1;
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_device )); 
    R_CHK(net_Address_device->SetSP(bSimulator? &CLSID_NETWORKSIMULATOR_DP8SP_TCPIP : &CLSID_DP8SP_TCPIP ));
//	R_CHK(net_Address_device->AddComponent	(DPNA_KEY_PORT, &c_port, sizeof(c_port), DPNA_DATATYPE_DWORD ));
	
    // Create our IDirectPlay8Address Server Address, --- Set the SP for our Server Address
	WCHAR	ServerNameUNICODE	[256];
	R_CHK(MultiByteToWideChar(CP_ACP, 0, server_name, -1, ServerNameUNICODE, 256 ));

	net_Address_server = NULL;
    R_CHK(CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_server )); 
    R_CHK(net_Address_server->SetSP			(bSimulator? &CLSID_NETWORKSIMULATOR_DP8SP_TCPIP : &CLSID_DP8SP_TCPIP  ));
	R_CHK(net_Address_server->AddComponent	(DPNA_KEY_PORT,	&psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));
    R_CHK(net_Address_server->AddComponent	(DPNA_KEY_HOSTNAME, ServerNameUNICODE, 2*u32(wcslen(ServerNameUNICODE) + 1), DPNA_DATATYPE_STRING ));

	// Debug
	// dump_URL		("! cl ",	net_Address_device);
	// dump_URL		("! en ",	net_Address_server);
	
    // Now set up the Application Description
    DPN_APPLICATION_DESC        dpAppDesc;
    ZeroMemory					(&dpAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
    dpAppDesc.guidApplication	= NET_GUID;
	
	// Setup client info
	WCHAR	ClientNameUNICODE	[256];
	R_CHK(MultiByteToWideChar	(CP_ACP, 0, options, -1, ClientNameUNICODE, 256 ));

	DPN_PLAYER_INFO				Pinfo;
	ZeroMemory					(&Pinfo,sizeof(Pinfo));
	Pinfo.dwSize				= sizeof(Pinfo);
	Pinfo.dwInfoFlags			= DPNINFO_NAME;
	Pinfo.pwszName				= ClientNameUNICODE;
	R_CHK(NET->SetClientInfo	(&Pinfo,0,0,DPNSETCLIENTINFO_SYNC));

	if (stricmp(server_name,"localhost")==0)	
	{
		WCHAR	SessionPasswordUNICODE[4096];
		if (xr_strlen(password_str))
		{
			CHK_DX(MultiByteToWideChar	(CP_ACP, 0, password_str, -1, SessionPasswordUNICODE, 4096 ));
			dpAppDesc.dwFlags		|=	DPNSESSION_REQUIREPASSWORD;
			dpAppDesc.pwszPassword	=	SessionPasswordUNICODE;
		};

		HRESULT res = NET->Connect(
			&dpAppDesc,				// pdnAppDesc
			net_Address_server,		// pHostAddr
			net_Address_device,		// pDeviceInfo
			NULL,					// pdnSecurity
			NULL,					// pdnCredentials
			NULL, 0,				// pvUserConnectData/Size
			NULL,					// pvAsyncContext
			NULL,					// pvAsyncHandle
			DPNCONNECT_SYNC);		// dwFlags

//		R_CHK(res);
		if (res != S_OK) return FALSE;

		// Create ONE node
		HOST_NODE	NODE;
		ZeroMemory	(&NODE, sizeof(HOST_NODE));
		
		// Copy the Host Address
		R_CHK		(net_Address_server->Duplicate(&NODE.pHostAddress ) );
		
		// Retreive session name
		char					desc[4096];
		ZeroMemory				(desc,sizeof(desc));
		DPN_APPLICATION_DESC*	dpServerDesc=(DPN_APPLICATION_DESC*)desc;
		DWORD					dpServerDescSize=sizeof(desc);
		dpServerDesc->dwSize	= sizeof(DPN_APPLICATION_DESC);
		R_CHK					(NET->GetApplicationDesc(dpServerDesc,&dpServerDescSize,0));
		if( dpServerDesc->pwszSessionName)	{
			string4096				dpSessionName;
			R_CHK(WideCharToMultiByte(CP_ACP,0,dpServerDesc->pwszSessionName,-1,dpSessionName,sizeof(dpSessionName),0,0));
			NODE.dpSessionName	= (char*)(&dpSessionName[0]);
		}
		net_Hosts.push_back		(NODE);
	} else {
		string64						EnumData;
		EnumData[0] = 0;
		strcat	(EnumData, "ToConnect");
		DWORD	EnumSize = xr_strlen(EnumData) + 1;
		// We now have the host address so lets enum
		R_CHK(NET->EnumHosts(
			&dpAppDesc,				// pApplicationDesc
			net_Address_server,		// pdpaddrHost
			net_Address_device,		// pdpaddrDeviceInfo
			EnumData, EnumSize,		// pvUserEnumData, size
			2,						// dwEnumCount
			0,						// dwRetryInterval
			1500,					// dwTimeOut
			NULL,					// pvUserContext
			NULL,					// pAsyncHandle
			DPNENUMHOSTS_SYNC)		// dwFlags
			);
		
		// ****** Connection
		IDirectPlay8Address*        pHostAddress = NULL;
		if (net_Hosts.empty())		return FALSE;

		WCHAR	SessionPasswordUNICODE[4096];
		if (xr_strlen(password_str))
		{
			CHK_DX(MultiByteToWideChar(CP_ACP, 0, password_str, -1, SessionPasswordUNICODE, 4096 ));
			dpAppDesc.dwFlags |= DPNSESSION_REQUIREPASSWORD;
			dpAppDesc.pwszPassword = SessionPasswordUNICODE;
		};
		
		net_csEnumeration.Enter		();
		// real connect
		for (u32 I=0; I<net_Hosts.size(); I++) 
			Msg("* HOST #%d: %s\n",I+1,net_Hosts[I].dpSessionName);
		
		R_CHK(net_Hosts.front().pHostAddress->Duplicate(&pHostAddress ) );
		// dump_URL		("! c2s ",	pHostAddress);
		HRESULT res = NET->Connect(
			&dpAppDesc,				// pdnAppDesc
			pHostAddress,			// pHostAddr
			net_Address_device,		// pDeviceInfo
			NULL,					// pdnSecurity
			NULL,					// pdnCredentials
			NULL, 0,				// pvUserConnectData/Size
			NULL,					// pvAsyncContext
			NULL,					// pvAsyncHandle
			DPNCONNECT_SYNC);		// dwFlags
//		R_CHK(res);		
		net_csEnumeration.Leave		();
		_RELEASE					(pHostAddress);
		if (res != S_OK) return FALSE;
	}

	// Caps
	/*
	GUID			sp_guid;
	DPN_SP_CAPS		sp_caps;

	net_Address_device->GetSP(&sp_guid);
	ZeroMemory		(&sp_caps,sizeof(sp_caps));
	sp_caps.dwSize	= sizeof(sp_caps);
	R_CHK			(NET->GetSPCaps(&sp_guid,&sp_caps,0));
	sp_caps.dwSystemBufferSize	= 0;
	R_CHK			(NET->SetSPCaps(&sp_guid,&sp_caps,0));
	R_CHK			(NET->GetSPCaps(&sp_guid,&sp_caps,0));
	*/

	// Sync
	net_TimeDelta	= 0;
	net_Syncronize	();
	return			TRUE;
}

void IPureClient::Disconnect()
{
    if( NET )	NET->Close(0);

    // Clean up Host _list_
	net_csEnumeration.Enter			();
	for (u32 i=0; i<net_Hosts.size(); i++) {
		HOST_NODE&	N = net_Hosts[i];
		_RELEASE	(N.pHostAddress);
	}
	net_Hosts.clear					();
	net_csEnumeration.Leave			();

	// Release interfaces
	_SHOW_REF	("cl_netADR_Server",net_Address_server);
	_RELEASE	(net_Address_server);
	_SHOW_REF	("cl_netADR_Device",net_Address_device);
	_RELEASE	(net_Address_device);
	_SHOW_REF	("cl_netCORE",NET);
	_RELEASE	(NET);
}

HRESULT	IPureClient::net_Handler(u32 dwMessageType, PVOID pMessage)
{
	// HRESULT     hr = S_OK;

	switch (dwMessageType)
	{
	case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			PDPNMSG_ENUM_HOSTS_RESPONSE     pEnumHostsResponseMsg;
			const DPN_APPLICATION_DESC*     pDesc;
			// HOST_NODE*                      pHostNode = NULL;
			// WCHAR*                          pwszSession = NULL;

			pEnumHostsResponseMsg			= (PDPNMSG_ENUM_HOSTS_RESPONSE) pMessage;
			pDesc							= pEnumHostsResponseMsg->pApplicationDescription;

			// Insert each host response if it isn't already present
			net_csEnumeration.Enter			();
			BOOL	bHostRegistered			= FALSE;
			for (u32 I=0; I<net_Hosts.size(); I++)
			{
				HOST_NODE&	N = net_Hosts	[I];
				if	( pDesc->guidInstance == N.dpAppDesc.guidInstance)
				{
					// This host is already in the list
					bHostRegistered = TRUE;
					break;
				}
			}

			if (!bHostRegistered) 
			{
				// This host session is not in the list then so insert it.
				HOST_NODE	NODE;
				ZeroMemory	(&NODE, sizeof(HOST_NODE));

				// Copy the Host Address
				R_CHK		(pEnumHostsResponseMsg->pAddressSender->Duplicate(&NODE.pHostAddress ) );
				Memory.mem_copy(&NODE.dpAppDesc,pDesc,sizeof(DPN_APPLICATION_DESC));

				// Null out all the pointers we aren't copying
				NODE.dpAppDesc.pwszSessionName					= NULL;
				NODE.dpAppDesc.pwszPassword						= NULL;
				NODE.dpAppDesc.pvReservedData					= NULL;
				NODE.dpAppDesc.dwReservedDataSize				= 0;
				NODE.dpAppDesc.pvApplicationReservedData		= NULL;
				NODE.dpAppDesc.dwApplicationReservedDataSize	= 0;

				if( pDesc->pwszSessionName)	{
					string4096			dpSessionName;
					R_CHK				(WideCharToMultiByte(CP_ACP,0,pDesc->pwszSessionName,-1,dpSessionName,sizeof(dpSessionName),0,0));
					NODE.dpSessionName	= (char*)(&dpSessionName[0]);
				}

				net_Hosts.push_back			(NODE);
			}
			net_csEnumeration.Leave			();
		}
		break;

	case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE	pMsg	= (PDPNMSG_RECEIVE) pMessage;
			void*			m_data	= pMsg->pReceiveData;
			u32				m_size	= pMsg->dwReceiveDataSize;
			MSYS_CONFIG*	cfg		= (MSYS_CONFIG*)m_data;

			if ((m_size>2*sizeof(u32)) && (cfg->sign1==0x12071980) && (cfg->sign2==0x26111975))
			{
				// Internal system message
				if ((m_size == sizeof(MSYS_PING)))
				{
					// It is reverted(server) ping
					u32		time	= TimerAsync(device_timer);
					MSYS_PING*	msg	= (MSYS_PING*)m_data;
					u32		ping	= time - (msg->dwTime_ClientSend);
					u32		delta	= msg->dwTime_Server + ping/2 - time;
					net_DeltaArray.push	(delta);
					Sync_Average		();
				} else if ((m_size == sizeof(MSYS_CONFIG)))
				{
					// It is configuration message
					NET_Compressor_FREQ			send,receive;
					send.setFromWORDS			(cfg->send);
					receive.setFromWORDS		(cfg->receive);
					net_Compressor.Initialize	(send,receive);
					net_Connected				= TRUE;
					// Msg("* New reparse point received");
				} else {
					Msg("! Unknown system message");
				}
			} else if (net_Connected)	{
				// One of the messages - decompress it
				OnMessage				(m_data,m_size);
			}
		}
		break;
	case DPN_MSGID_TERMINATE_SESSION:
		{
			PDPNMSG_TERMINATE_SESSION 	pMsg	= (PDPNMSG_TERMINATE_SESSION ) pMessage;
			char*			m_data	= (char*)pMsg->pvTerminateData;
			u32				m_size	= pMsg->dwTerminateDataSize;
			HRESULT			m_hResultCode = pMsg->hResultCode;

			net_Disconnected	= TRUE;

			if (m_size != 0)
			{
				Msg("- Session terminated : %s", m_data);
			}
			else
			{
				Msg("- Session terminated : %s", (::Debug.error2string(m_hResultCode)));
			}
		};
		break;
	default:
		{
#if	1
			LPSTR	msg = "";	
			switch (dwMessageType)
			{
			case DPN_MSGID_ADD_PLAYER_TO_GROUP:			msg = "DPN_MSGID_ADD_PLAYER_TO_GROUP"; break;
			case DPN_MSGID_ASYNC_OP_COMPLETE:			msg = "DPN_MSGID_ASYNC_OP_COMPLETE"; break;
			case DPN_MSGID_CLIENT_INFO:					msg	= "DPN_MSGID_CLIENT_INFO"; break;
			case DPN_MSGID_CONNECT_COMPLETE:			
				{
					PDPNMSG_CONNECT_COMPLETE pMsg = (PDPNMSG_CONNECT_COMPLETE)pMessage;
					if (pMsg->dwApplicationReplyDataSize)
					{
						string256 ResStr = "";
						strncpy(ResStr, (char*)(pMsg->pvApplicationReplyData), pMsg->dwApplicationReplyDataSize);
						Msg("Connection result : %s", ResStr);
					}
					else
						msg	= "DPN_MSGID_CONNECT_COMPLETE"; 
				}break;
			case DPN_MSGID_CREATE_GROUP:				msg	= "DPN_MSGID_CREATE_GROUP"; break;
			case DPN_MSGID_CREATE_PLAYER:				msg = "DPN_MSGID_CREATE_PLAYER"; break;
			case DPN_MSGID_DESTROY_GROUP: 				msg = "DPN_MSGID_DESTROY_GROUP"; break;
			case DPN_MSGID_DESTROY_PLAYER: 				msg = "DPN_MSGID_DESTROY_PLAYER"; break;
			case DPN_MSGID_ENUM_HOSTS_QUERY:			msg = "DPN_MSGID_ENUM_HOSTS_QUERY"; break;
			case DPN_MSGID_GROUP_INFO:					msg = "DPN_MSGID_GROUP_INFO"; break;
			case DPN_MSGID_HOST_MIGRATE:				msg = "DPN_MSGID_HOST_MIGRATE"; break;
			case DPN_MSGID_INDICATE_CONNECT:			msg = "DPN_MSGID_INDICATE_CONNECT"; break;
			case DPN_MSGID_INDICATED_CONNECT_ABORTED:	msg = "DPN_MSGID_INDICATED_CONNECT_ABORTED"; break;
			case DPN_MSGID_PEER_INFO:					msg = "DPN_MSGID_PEER_INFO"; break;
			case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:	msg = "DPN_MSGID_REMOVE_PLAYER_FROM_GROUP"; break;
			case DPN_MSGID_RETURN_BUFFER:				msg = "DPN_MSGID_RETURN_BUFFER"; break;
			case DPN_MSGID_SEND_COMPLETE:				msg = "DPN_MSGID_SEND_COMPLETE"; break;
			case DPN_MSGID_SERVER_INFO:					msg = "DPN_MSGID_SERVER_INFO"; break;
			case DPN_MSGID_TERMINATE_SESSION:			msg = "DPN_MSGID_TERMINATE_SESSION"; break;
			default:									msg = "???"; break;
			}
//			Msg("! ************************************ : %s",msg);
#endif
		}
		break;
	}

	return S_OK;
}

void	IPureClient::OnMessage(void* data, u32 size)
{
	// One of the messages - decompress it
	NET_Packet* P	= net_Queue.Create			();
	P->B.count		= net_Compressor.Decompress	(P->B.data,LPBYTE(data),size);
	P->timeReceive	= timeServer_Async();//TimerAsync				(device_timer);
}

void	IPureClient::timeServer_Correct(u32 sv_time, u32 cl_time)
{
	u32		ping	= net_Statistic.getPing();
	u32		delta	= sv_time + ping/2 - cl_time;
	net_DeltaArray.push	(delta);
	Sync_Average		();
}

void	IPureClient::Send(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
	if (net_Disconnected)	return;

	// first - compress message and setup buffer
	NET_Buffer	Compressed;
	Compressed.count	= net_Compressor.Compress	(Compressed.data,P.B.data,P.B.count);

	net_Statistic.dwBytesSended += Compressed.count;
	// send it
	DPN_BUFFER_DESC		desc;
	desc.dwBufferSize	= Compressed.count;
	desc.pBufferData	= Compressed.data;
	
	// verify
	VERIFY				(desc.dwBufferSize);
	VERIFY				(desc.pBufferData);

    DPNHANDLE	hAsync=0;
	VERIFY		(NET);
	R_CHK(NET->Send(
		&desc,1,
		dwTimeout,
		0,&hAsync,
		dwFlags
		));
}

BOOL	IPureClient::net_HasBandwidth	()
{
	u32		dwTime				= TimeGlobal(device_timer);
	u32		dwInterval			= 1000/psNET_ClientUpdate;
	if		(net_Disconnected) return FALSE;
	
	if		(psNET_Flags.test(NETFLAG_MINIMIZEUPDATES))	dwInterval	= 333;	// approx 3 times per second

	HRESULT hr;
	if ((dwTime-net_Time_LastUpdate)>dwInterval)	
	{
		R_ASSERT			(NET);
		
		// check queue for "empty" state
		DWORD				dwPending=0;
		hr					= NET->GetSendQueueInfo(&dwPending,0,0);
		if (FAILED(hr)) return FALSE;

		if (dwPending > u32(psNET_ClientPending))	
		{
			net_Statistic.dwTimesBlocked++;
			return FALSE;
		};

		// Query network statistic for this client
		DPN_CONNECTION_INFO	CI;
		ZeroMemory			(&CI,sizeof(CI));
		CI.dwSize			= sizeof(CI);
		hr					= NET->GetConnectionInfo(&CI,0);
		if (FAILED(hr)) return FALSE;

		net_Statistic.Update(CI);

		// ok
		net_Time_LastUpdate	= dwTime;
		return TRUE;
	}
	return FALSE;
}

void	IPureClient::Sync_Thread	()
{
	MSYS_PING			clPing;

	//***** Ping server
	net_DeltaArray.clear();
	R_ASSERT			(NET);
	for (; NET && !net_Disconnected; )
	{
		// Waiting for queue empty state
		if (net_Syncronised)	break; // Sleep(2000);
		else {
			DWORD			dwPending=0;
			do {
				Sleep			(1);
				R_CHK			(NET->GetSendQueueInfo(&dwPending,0,0));
			} while (dwPending);
		}

		// Construct message
		clPing.sign1				= 0x12071980;
		clPing.sign2				= 0x26111975;
		clPing.dwTime_ClientSend	= TimerAsync(device_timer);

		// Send it
		__try {
			DPN_BUFFER_DESC					desc;
			DPNHANDLE						hAsync=0;
			desc.dwBufferSize				= sizeof(clPing);
			desc.pBufferData				= LPBYTE(&clPing);
			if (0==NET || net_Disconnected)	break;

			if (FAILED(NET->Send(&desc,1,0,0,&hAsync,net_flags(FALSE,FALSE,TRUE))))	{
				Msg("* CLIENT: SyncThread: EXIT. (failed to send - disconnected?)");
				break;
			}
		} 
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			Msg("* CLIENT: SyncThread: EXIT. (failed to send - disconnected?)");
			break;
		}
		
		// Waiting for reply-packet to arrive
		if (!net_Syncronised)	{
			u32	old_size	= net_DeltaArray.size();
			u32	timeBegin	= TimerAsync(device_timer);
			while ((net_DeltaArray.size()==old_size)&&(TimerAsync(device_timer)-timeBegin<5000))		Sleep(1);
			
			if (net_DeltaArray.size()>=syncSamples)	{
				net_Syncronised	= TRUE;
				net_TimeDelta	= net_TimeDelta_Calculated;
				Msg				("* CL_TimeSync: DELTA: %d",net_TimeDelta);
			}
		}
	}
}

void	IPureClient::Sync_Average	()
{
	//***** Analyze results
	s64		summary_delta	= 0;
	s32		size			= net_DeltaArray.size();
	u32*	I				= net_DeltaArray.begin();
	u32*  E					= I+size;
	for (; I!=E; I++)		summary_delta	+= *((int*)I);

	s64 frac				=	s64(summary_delta) % s64(size);
	if (frac<0)				frac=-frac;
	summary_delta			/=	s64(size);
	if (frac>s64(size/2))	summary_delta += (summary_delta<0)?-1:1;
	net_TimeDelta_Calculated=	s32(summary_delta);
	net_TimeDelta			=	(net_TimeDelta*5+net_TimeDelta_Calculated)/6;
//	Msg("* CLIENT: d(%d), dc(%d), s(%d)",net_TimeDelta,net_TimeDelta_Calculated,size);
}

void 				sync_thread(void* P)
{
	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	IPureClient*	C	= (IPureClient*)P;
	C->Sync_Thread		();
}
void	IPureClient::net_Syncronize	()
{
	net_Syncronised		= FALSE;
	net_DeltaArray.clear();
	thread_spawn		(sync_thread,"network-time-sync",0,this);
}

void	IPureClient::ClearStatistic()
{
	net_Statistic.Clear();
}