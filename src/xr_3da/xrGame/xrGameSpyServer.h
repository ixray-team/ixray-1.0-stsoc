#pragma once

#include "xrServer.h"
#include "GameSpy/GameSpy_GCD_Server.h"
#include "GameSpy/GameSpy_QR2.h"


class xrGameSpyClientData	: public xrClientData
{
private:
	typedef xrClientData inherited;
public:
	string64			m_pChallengeString;
	bool				m_bCDKeyAuth;

	xrGameSpyClientData			();
	virtual void				Clear();
	virtual ~xrGameSpyClientData	();
};

class xrGameSpyServer	: public xrServer
{
private:
	typedef xrServer inherited;
private:
	int								m_iReportToMasterServer;

	BOOL							m_bQR2_Initialized;
	void							QR2_Init						();
	void							QR2_ShutDown					();
	
	BOOL							m_bCDKey_Initialized;
	void							CDKey_Init						();
	void							CDKey_ShutDown					();
	void							SendChallengeString_2_Client	(IClient* C);
	
	CGameSpy_GCD_Server				m_GCDServer;
	CGameSpy_QR2					m_QR2;
protected:
	virtual bool					NeedToCheckClient_GameSpy_CDKey	(IClient* CL);
public:
	shared_str						HostName;
	shared_str						MapName;
	shared_str						Password;
	int								m_iMaxPlayers;
	bool							m_bCheckCDKey;
	BOOL							m_bDedicated;

	int								GetPlayersCount					();
	void							OnCDKey_Validation				(int LocalID, int res, char* errormsg);
	CGameSpy_QR2*					QR2() {return &m_QR2;} ;
public:
	xrGameSpyServer					();
	virtual ~xrGameSpyServer		();

	virtual BOOL			Connect				(shared_str& session_name);
	virtual void			Update				();

//	virtual void			OnCL_Connected		(IClient* C);
	virtual void			OnCL_Disconnected	(IClient* C);
	virtual IClient*		client_Create		();

	virtual u32				OnMessage			(NET_Packet& P, ClientID/*DPNID*/ sender);	// Non-Zero means broadcasting with "flags" as returned
};

