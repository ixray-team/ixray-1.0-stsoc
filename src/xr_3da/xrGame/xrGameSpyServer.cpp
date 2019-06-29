#include "stdafx.h"
#include "xrMessages.h"
#include "xrGameSpyServer.h"
#include "../igame_persistent.h"

#include "GameSpy/GameSpy_Available.h"

xrGameSpyServer::xrGameSpyServer()
{
	m_iReportToMasterServer = 0;
	m_bQR2_Initialized = FALSE;
	m_bCDKey_Initialized = FALSE;
	m_bCheckCDKey = false;
	m_bDedicated = g_pGamePersistent->bDedicatedServer;
}

xrGameSpyServer::~xrGameSpyServer()
{
	CDKey_ShutDown();
	QR2_ShutDown();
}

//----------- xrGameSpyClientData -----------------------
IClient*		xrGameSpyServer::client_Create		()
{
	return xr_new<xrGameSpyClientData> ();
}
xrGameSpyClientData::xrGameSpyClientData	():xrClientData()
{
}
void	xrGameSpyClientData::Clear()
{
	inherited::Clear();

	m_pChallengeString[0] = 0;
	m_bCDKeyAuth = false;
};

xrGameSpyClientData::~xrGameSpyClientData()
{
	m_pChallengeString[0] = 0;
	m_bCDKeyAuth = false;
}
//-------------------------------------------------------
BOOL xrGameSpyServer::Connect(shared_str &session_name)
{
	BOOL res = inherited::Connect(session_name);
	if (!res) return res;

	if ( 0 == *(game->get_option_s		(*session_name,"hname",NULL)))
	{
		string1024	CompName;
		DWORD		CompNameSize = 1024;
		if (GetComputerName(CompName, &CompNameSize)) HostName._set(CompName);
	}
	else
		HostName._set(game->get_option_s		(*session_name,"hname",NULL));
	
	if (0 != *(game->get_option_s		(*session_name,"psw",NULL)))
		Password._set(game->get_option_s		(*session_name,"psw",NULL));

	string4096	tMapName = "";
	const char* SName = *session_name;
	strncpy(tMapName, *session_name, strchr(SName, '/') - SName);
	MapName._set(tMapName);// = (session_name);
	

	m_iReportToMasterServer = game->get_option_i		(*session_name,"public",0);
	m_iMaxPlayers	= game->get_option_i		(*session_name,"maxplayers",32);
	m_bCheckCDKey = game->get_option_i		(*session_name,"cdkey",0) != 0;
	//--------------------------------------------//
	if (game->Type() != GAME_SINGLE) 
	{
		//----- Check for Backend Services ---
		CGameSpy_Available GSA;
		if (!GSA.CheckAvailableServices()) return FALSE;

		//------ Init of QR2 SDK -------------
		QR2_Init();

		//------ Init of CDKey SDK -----------
		if(m_bCheckCDKey) CDKey_Init();
	};

	return res;
}

void			xrGameSpyServer::Update				()
{
	inherited::Update();

	if (m_bQR2_Initialized)
	{
		m_QR2.Think(NULL);
	};

	if (m_bCDKey_Initialized)
	{
		m_GCDServer.Think();
	};
}

int				xrGameSpyServer::GetPlayersCount()
{
	int NumPlayers = client_Count();
	if (!g_pGamePersistent->bDedicatedServer || NumPlayers < 1) return NumPlayers;
	return NumPlayers - 1;
};

/*
void			xrGameSpyServer::OnCL_Connected		(IClient* _CL)
{
//	Server_Client_Check(_CL); 

	xrGameSpyClientData* CL		= (xrGameSpyClientData*)_CL;

	if (!m_bCDKey_Initialized || CL == GetServer_client()) 
	{
		CL->m_bCDKeyAuth = true;
		inherited::OnCL_Connected(_CL);
		return;
	};
	
	SendChallengeString_2_Client(_CL);
};
*/

bool			xrGameSpyServer::NeedToCheckClient_GameSpy_CDKey	(IClient* CL)
{
	if (!m_bCDKey_Initialized)// || CL == GetServer_client())
	{
		return false;
	};

	SendChallengeString_2_Client(CL);

	return true;
};

void			xrGameSpyServer::OnCL_Disconnected	(IClient* _CL)
{
	inherited::OnCL_Disconnected(_CL);

	csPlayers.Enter			();

	if (m_bCDKey_Initialized)
	{
		Msg("GameSpy::CDKey::Server : Disconnecting Client");
		m_GCDServer.DisconnectUser(int(_CL->ID.value()));
	};

	csPlayers.Leave			();
}

u32				xrGameSpyServer::OnMessage(NET_Packet& P, ClientID sender)			// Non-Zero means broadcasting with "flags" as returned
{
	u16			type;
	P.r_begin	(type);

	xrGameSpyClientData* CL		= (xrGameSpyClientData*)ID_to_client(sender);

	switch (type)
	{
	case M_GAMESPY_CDKEY_VALIDATION_CHALLENGE_RESPOND:
		{
			string128 ResponseStr;
			P.r_stringZ(ResponseStr);

			Msg("GameSpy::CDKey::Server : Respond accepted, authenticate client.");

			m_GCDServer.AuthUser(int(CL->ID.value()), 0, CL->m_pChallengeString, ResponseStr, this);
			return (0);
		}break;
		/*
	case M_CLIENTREADY:
		{
			if (CL->m_bCDKeyAuth) break;
//			SendChallengeString_2_Client(CL);
		}break;
		*/
	}

	return	inherited::OnMessage(P, sender);
};