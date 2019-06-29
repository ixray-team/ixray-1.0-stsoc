#pragma once

#include "net_shared.h"


class XRNETSERVER_API IClient
{
	struct Flags
	{
		u32		bLocal		: 1;
		u32		bConnected	: 1;
		u32		bReconnect	: 1;
		u32		bVerified	: 1;
	};
public:
	IClientStatistic	stats;

	ClientID			ID;
	shared_str				Name;
	Flags				flags;	// local/host/normal
	u32					dwTime_LastUpdate;

	char				m_cAddress[4];
	DWORD				m_dwPort;

	IClient(CTimer* timer):stats(timer)	{
		dwTime_LastUpdate	= 0;
		flags.bLocal = FALSE;
		flags.bConnected = FALSE;
		flags.bReconnect = FALSE;
		flags.bVerified = TRUE;
	}
	virtual ~IClient(){}
};

class XRNETSERVER_API IServerStatistic
{
public:
	u32					bytes_out,bytes_out_real;
	u32					bytes_in, bytes_in_real;

	void					clear()
	{
		bytes_out	= bytes_out_real=0;
		bytes_in	= bytes_in_real	=0;

		dwBytesSended = 0;
		dwSendTime = 0;
		dwBytesPerSec = 0;
	}

	u32					dwBytesSended;
	u32					dwSendTime;
	u32					dwBytesPerSec;
};

class XRNETSERVER_API IBannedClient
{
public:
	char				HAddr[4];
	u32					BanTime;
	
	IBannedClient ()
	{
		HAddr[0] = 0;
		HAddr[1] = 0;
		HAddr[2] = 0;
		HAddr[3] = 0;

		BanTime = 0;
	};

	IBannedClient (const char* Address, const u32 Time)
	{
		HAddr[0] = Address[0];
		HAddr[1] = Address[1];
		HAddr[2] = Address[2];
		HAddr[3] = Address[3];

		BanTime = Time;
	};

	bool	operator == 		(const char* Addr)
	{
		for (int i=0; i<4; i++)
		{
			if (i == 3 && HAddr[i] == 0) return true;
			if (HAddr[i] != Addr[i]) return false;
		};
		return true;
	};	
};

class XRNETSERVER_API IPureServer
{
protected:
	IDirectPlay8Server*		NET;
	IDirectPlay8Address*	net_Address_device;
	
	NET_Compressor			net_Compressor;
	NET_Compressor_FREQ		traffic_out;
	NET_Compressor_FREQ		traffic_in;

	xrCriticalSection		csPlayers;
	xr_vector<IClient*>		net_Players;
	xr_vector<IClient*>		net_Players_disconnected;
	IClient*				SV_Client;

	int						psNET_Port;	

	xr_vector<IBannedClient*>		BannedAddresses;

	// Compressor configuration
	MSYS_CONFIG				msgConfig;
	void					config_Load			();
	void					config_Save			();

	// 
	xrCriticalSection		csMessage;

	// Utilities
	void					pCompress			(NET_Packet& D, NET_Packet& S);
	void					pDecompress			(NET_Packet& D, void* data, u32 size);
	void					client_link_aborted	(ClientID/*DPNID*/ ID	);
	void					client_link_aborted	(IClient* C	);
	
	// Statistic
	IServerStatistic		stats;
	CTimer*					device_timer;

	virtual void			new_client			(ClientID clientID, LPCSTR name, bool bLocal)   =0;
			bool			GetClientAddress	(IDirectPlay8Address* pClientAddress, char* Address, DWORD* pPort = NULL);

			IBannedClient*	GetBannedClient		(const char* Address);			
			void			BannedAddress_Save	(u32 it, IWriter* fs);
			void			BannedList_Save		();
	virtual	void			BannedList_Load		()	{};
			LPCSTR			GetBannedListName	();

public:
	IPureServer				(CTimer* timer);
	virtual ~IPureServer	();
	HRESULT					net_Handler			(u32 dwMessageType, PVOID pMessage);
	
	virtual BOOL			Connect				(LPCSTR session_name);
	virtual void			Disconnect			();
	virtual void			Reparse				();									// Reparse configuration and freq data

	// send
	virtual void			SendTo_LL			(ClientID/*DPNID*/ ID, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	void					SendTo				(ClientID/*DPNID*/ ID, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	void					SendBroadcast_LL	(ClientID/*DPNID*/ exclude, void* data, u32 size, u32 dwFlags=DPNSEND_GUARANTEED);
	void					SendBroadcast		(ClientID/*DPNID*/ exclude, NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED);

	// statistic
	const IServerStatistic*	GetStatistic		() { return &stats; }
	void					ClearStatistic		();
	void					UpdateClientStatistic		(IClient* C);

	// extended functionality
	virtual u32				OnMessage			(NET_Packet& P, ClientID/*DPNID*/ sender);	// Non-Zero means broadcasting with "flags" as returned
	virtual void			OnCL_Connected		(IClient* C);
	virtual void			OnCL_Disconnected	(IClient* C);
	virtual bool			OnCL_QueryHost		()		{ return true; };

	virtual IClient*		client_Create		()				= 0;			// create client info
	virtual void			client_Replicate	()				= 0;			// replicate current state to client
	virtual void			client_Destroy		(IClient* C)	= 0;			// destroy client info

	IC u32					client_Count		()			{ return net_Players.size(); }
	IC IClient*				client_Get			(u32 num)	{ return net_Players[num]; }

	IC u32					disconnected_client_Count		()			{ return net_Players_disconnected.size(); }
	IC IClient*				disconnected_client_Get			(u32 num)	{ return net_Players_disconnected[num]; }
	
	BOOL					HasBandwidth		(IClient* C);

	IC int					GetPort				()			{ return psNET_Port; };
			bool			GetClientAddress	(ClientID ID, char* Address, DWORD* pPort = NULL);
	virtual bool			DisconnectClient	(IClient* C);
	virtual bool			DisconnectAddress	(char* Address);
	virtual void			BanClient			(IClient* C, u32 BanTime);
	virtual void			BanAddress			(char* Address, u32 BanTime);
	IClient*				GetServerClient		()			{ return SV_Client; };


};
