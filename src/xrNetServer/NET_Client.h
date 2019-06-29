#pragma once

#include "net_shared.h"

class XRNETSERVER_API INetQueue
{
	xrCriticalSection		cs;
	xr_deque<NET_Packet*>	ready;
	xr_vector<NET_Packet*>	unused;
public:
	INetQueue();
	~INetQueue();

	NET_Packet*			Create	();
	NET_Packet*			Create	(const NET_Packet& _other);
	NET_Packet*			Retreive();
	void				Release	();
};

class XRNETSERVER_API IPureClient
{
	friend void 				sync_thread(void*);
protected:
	struct HOST_NODE
	{
		DPN_APPLICATION_DESC	dpAppDesc;
		IDirectPlay8Address*	pHostAddress;
		shared_str				dpSessionName;
	};
	CTimer*					device_timer;
protected:
	IDirectPlay8Client*		NET;
	IDirectPlay8Address*	net_Address_device;
	IDirectPlay8Address*	net_Address_server;
	
	xrCriticalSection		net_csEnumeration;
	xr_vector<HOST_NODE>	net_Hosts;

	NET_Compressor			net_Compressor;

	BOOL					net_Connected;
	BOOL					net_Syncronised;
	BOOL					net_Disconnected;

	INetQueue				net_Queue;
	IClientStatistic		net_Statistic;
	
	u32						net_Time_LastUpdate;
	s32						net_TimeDelta;
	s32						net_TimeDelta_Calculated;
	s32						net_TimeDelta_User;
	
	void					Sync_Thread		();
	void					Sync_Average	();
public:
	IPureClient				(CTimer* tm);
	virtual ~IPureClient	();
	HRESULT					net_Handler				(u32 dwMessageType, PVOID pMessage);
	
	BOOL					Connect					(LPCSTR server_name);
	void					Disconnect				();

	void					net_Syncronize			();
	BOOL					net_isCompleted_Connect	()	{ return net_Connected;		}
	BOOL					net_isCompleted_Sync	()	{ return net_Syncronised;	}
	BOOL					net_isDisconnected		()	{ return net_Disconnected;	}
	LPCSTR					net_SessionName			()	{ return *(net_Hosts.front().dpSessionName); }

	// receive
	IC virtual	NET_Packet*			net_msg_Retreive		()	{ return net_Queue.Retreive();	}
	IC void					net_msg_Release			()	{ net_Queue.Release();			}

	// send
	virtual	void			Send					(NET_Packet& P, u32 dwFlags=DPNSEND_GUARANTEED, u32 dwTimeout=0);
	virtual void			OnMessage				(void* data, u32 size);
	virtual void			OnInvalidHost			()	{};
	virtual void			OnInvalidPassword		()	{};
	virtual void			OnSessionFull			()	{};
	virtual void			OnConnectRejected		()	{};
	BOOL					net_HasBandwidth		();
	void					ClearStatistic			();
	IClientStatistic		GetStatistic			() const {return  net_Statistic; }
	void					UpdateStatistic			();

			bool			GetServerAddress		(char* pAddress, DWORD* pPort);
	
	// time management
	IC u32					timeServer				()	{ return TimeGlobal(device_timer) + net_TimeDelta + net_TimeDelta_User; }
	IC u32					timeServer_Async		()	{ return TimerAsync(device_timer) + net_TimeDelta + net_TimeDelta_User; }
	IC u32					timeServer_Delta		()	{ return net_TimeDelta; }
	IC void					timeServer_UserDelta	(s32 d)						{ net_TimeDelta_User=d;	}
	IC void					timeServer_Correct		(u32 sv_time, u32 cl_time);

	virtual	BOOL			net_IsSyncronised		();

	virtual	LPCSTR			GetMsgId2Name			(u16 ID) {return "";};
};
