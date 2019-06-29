#include "StdAfx.h"
#include "..\game_base_space.h"
#include "..\Spectator.h"
#include "GameSpy_Browser.h"
#include "GameSpy_Base_Defs.h"
#include "..\ui\ServerList.h"


#include "GameSpy_Available.h"

void __cdecl SBCallback(void* sb, SBCallbackReason reason, void* server, void *instance);

CGameSpy_Browser::CGameSpy_Browser()
{
	m_SecretKey[0] = 't';
	m_SecretKey[1] = '9';
	m_SecretKey[2] = 'F';
	m_SecretKey[3] = 'j';
	m_SecretKey[4] = '3';
	m_SecretKey[5] = 'M';
	m_SecretKey[6] = 'x';
	m_SecretKey[7] = '\0';
	//-------------------------
	hGameSpyDLL = NULL;

	LoadGameSpy();
	//-------------------------
	m_pGSBrowser = NULL;
	//-------------------------
	m_pServerList = NULL;
	
};

CGameSpy_Browser::~CGameSpy_Browser()
{
	if (m_pGSBrowser)
	{		
		xrGS_ServerBrowserFree(m_pGSBrowser);
		m_pGSBrowser = NULL;
	}
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_Browser::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");
	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserNew);	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserFree);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserClear);
	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserThink);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserState);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserHalt);	
	GAMESPY_LOAD_FN(xrGS_ServerBrowserUpdate);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserLANUpdate);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserCount);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserGetServer);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserGetServerByIP);

	GAMESPY_LOAD_FN(xrGS_SBServerGetPublicAddress);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPublicQueryPort);
	GAMESPY_LOAD_FN(xrGS_SBServerGetStringValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetIntValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetFloatValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetBoolValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPing);

	GAMESPY_LOAD_FN(xrGS_ServerBrowserAuxUpdateServer);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserAuxUpdateIP);
	
	GAMESPY_LOAD_FN(xrGS_SBServerGetPlayerStringValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPlayerIntValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetPlayerFloatValue);

	GAMESPY_LOAD_FN(xrGS_SBServerGetTeamStringValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetTeamIntValue);
	GAMESPY_LOAD_FN(xrGS_SBServerGetTeamFloatValue);

	GAMESPY_LOAD_FN(xrGS_ServerBrowserRemoveIP);
	GAMESPY_LOAD_FN(xrGS_ServerBrowserRemoveServer);

}

static services_checked = false;

bool	CGameSpy_Browser::Init(CServerList* pServerList)
{
	//-------------------------------------
	CGameSpy_Available GSA;
	if (!services_checked)
	{
        if (!GSA.CheckAvailableServices()) 
			return false;
		else
			services_checked = true;
	}
	//-------------------------------------
	m_QR2.RegisterAdditionalKeys();

	m_pGSBrowser = xrGS_ServerBrowserNew(GAMESPY_GAMENAME, GAMESPY_GAMENAME, m_SecretKey, 0, GAMESPY_MAX_UPDATES, GSA.xrGS_GetQueryVersion(), SBFalse, SBCallback, this);
	if (!m_pGSBrowser)
	{
		Msg("! Unable to init GameSpy Server Browser!");
	}
	else
		Msg("- GameSpy Server Browser Inited!");

	m_pServerList = pServerList;

	return true;
};

void			CGameSpy_Browser::RefreshList_Full(bool Local)
{
	SBState state = xrGS_ServerBrowserState(m_pGSBrowser);               
	if((state != sb_connected) && (state != sb_disconnected))
	{
		xrGS_ServerBrowserHalt(m_pGSBrowser);
		Msg("GameSpy Refresh Stopped\n");		
	};
	xrGS_ServerBrowserClear(m_pGSBrowser);

	// fields we're interested in	
	int numFields = sizeof(Fields_Of_Interest) / sizeof(Fields_Of_Interest[0]);

	// do an update
	SBError error;
	if(!Local)
		error = xrGS_ServerBrowserUpdate(m_pGSBrowser, m_pServerList ? SBTrue : SBFalse, SBFalse, Fields_Of_Interest, numFields, (char *)(const char *)"");
	else
		error = xrGS_ServerBrowserLANUpdate(m_pGSBrowser, m_pServerList ? SBTrue : SBFalse, START_PORT, END_PORT);

	if (error != sbe_noerror)
	{
		Msg("! GameSpy Update Error!");	
	}
};

void __cdecl SBCallback(void* sb, SBCallbackReason reason, void* server, void *instance)
{
	CGameSpy_Browser* pGSBrowser = (CGameSpy_Browser*) instance;
	if (!pGSBrowser) return;
	switch (reason)
	{
	case sbc_serveradded : //a server was added to the list, may just have an IP & port at this point
		{
#ifdef _DEBUG
			Msg("sbc_serveradded");
#endif
		}break;
	case sbc_serverupdated : //server information has been updated - either basic or full information is now available about this server
		{
#ifdef _DEBUG
			Msg("sbc_serverupdated");
#endif
			pGSBrowser->UpdateServerList();
		}break;
	case sbc_serverupdatefailed : //an attempt to retrieve information about this server, either directly or from the master, failed
		{
#ifdef _DEBUG
			Msg("sbc_serverupdatefailed");
#endif
			pGSBrowser->OnUpdateFailed(server);
		}break;
	case sbc_serverdeleted : //a server was removed from the list
		{
#ifdef _DEBUG
			Msg("sbc_serverdeleted");
#endif
		}break;
	case sbc_updatecomplete : //the server query engine is now idle 
		{
#ifdef _DEBUG
			Msg("sbc_updatecomplete");
#endif
		}break;
	case sbc_queryerror		://the master returned an error string for the provided query
		{
#ifdef _DEBUG
			Msg("sbc_queryerror");
#endif
		}break;
	case sbc_serverchallengereceived:
		{
#ifdef _DEBUG
			Msg("sbc_serverchallengereceived");
#endif
		}break;
	default:
		{
			R_ASSERT2(0, "Unknown Callback Reason");
		}break;
	};
};

void CGameSpy_Browser::CallBack_OnUpdateCompleted()
{
	int NumServers = xrGS_ServerBrowserCount(m_pGSBrowser);

	ServerInfo NewServerInfo;
	for (int i=0; i<NumServers; i++)
	{
		void* pServer = xrGS_ServerBrowserGetServer(m_pGSBrowser, i);
		ReadServerInfo(&NewServerInfo, pServer);
	}
};

int	CGameSpy_Browser::GetServersCount()
{
	return xrGS_ServerBrowserCount(m_pGSBrowser);
};

void CGameSpy_Browser::GetServerInfoByIndex(ServerInfo* pServerInfo, int idx)
{
	void* pServer = xrGS_ServerBrowserGetServer(m_pGSBrowser, idx);
	ReadServerInfo(pServerInfo, pServer);
	pServerInfo->Index = idx;
}

#define ADD_BOOL_INFO(i, s, t, k)	i->m_aInfos.push_back(GameInfo(t, ((xrGS_SBServerGetBoolValue(s, m_QR2.xrGS_RegisteredKey(k), SBFalse)) == SBTrue)? "Yes" : "No"))
#define ADD_INT_INFO(i, s, t, k)	{string256 tmp; sprintf(tmp, "%d", xrGS_SBServerGetIntValue(s, m_QR2.xrGS_RegisteredKey(k), 0));\
	i->m_aInfos.push_back(GameInfo(t, tmp));}

#define ADD_INT_INFO_N(i, s, t1, t2, k)	{if (xrGS_SBServerGetIntValue(s, m_QR2.xrGS_RegisteredKey(k), 0))\
{string256 tmp; sprintf(tmp, "%d" t2, xrGS_SBServerGetIntValue(s, m_QR2.xrGS_RegisteredKey(k), 0));\
	i->m_aInfos.push_back(GameInfo(t1, tmp));}\
	else {i->m_aInfos.push_back(GameInfo(t1, "No"));}}

#define ADD_TIME_INFO(i, s, m, t1, t2, k)	{if (xrGS_SBServerGetIntValue(s, m_QR2.xrGS_RegisteredKey(k), 0))\
{string256 tmp; sprintf(tmp,t2, xrGS_SBServerGetFloatValue(s, m_QR2.xrGS_RegisteredKey(k), 0)*m);\
	i->m_aInfos.push_back(GameInfo(t1, tmp));}\
	else {i->m_aInfos.push_back(GameInfo(t1, "No"));}}

void	CGameSpy_Browser::ReadServerInfo	(ServerInfo* pServerInfo, void* pServer)
{
	if (!pServer || !pServerInfo) return;
	sprintf(pServerInfo->m_Address, "%s:%d", xrGS_SBServerGetPublicAddress(pServer), xrGS_SBServerGetPublicQueryPort(pServer));
	sprintf(pServerInfo->m_HostName, "%s", xrGS_SBServerGetPublicAddress(pServer));
	sprintf(pServerInfo->m_ServerName, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(HOSTNAME_KEY), pServerInfo->m_HostName));

	sprintf(pServerInfo->m_SessionName, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(MAPNAME_KEY), "Unknown"));	
	sprintf(pServerInfo->m_ServerGameType, "%s", xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(GAMETYPE_NAME_KEY), "UNKNOWN"));
	pServerInfo->m_bPassword	= xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(PASSWORD_KEY), SBFalse) == SBTrue;
	pServerInfo->m_Ping = (s16)(xrGS_SBServerGetPing(pServer) & 0xffff);
	pServerInfo->m_ServerNumPlayers = (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(NUMPLAYERS_KEY), 0);
	pServerInfo->m_ServerMaxPlayers = (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(MAXPLAYERS_KEY), 32);
	pServerInfo->m_ServerNumTeams = (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(NUMTEAMS_KEY), 0);
	pServerInfo->m_Port		= (s16)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(HOSTPORT_KEY), 0);
	pServerInfo->m_HPort	= (s16)xrGS_SBServerGetPublicQueryPort(pServer);
	pServerInfo->m_bDedicated	= (xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(DEDICATED_KEY), SBFalse)) == SBTrue;
	pServerInfo->m_GameType = (u8)xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(GAMETYPE_KEY), 0);

	//--------- Read Game Infos ---------------------------//
	pServerInfo->m_aInfos.clear();

	ADD_BOOL_INFO(pServerInfo, pServer, "Map Rotation", G_MAP_ROTATION_KEY);
	ADD_BOOL_INFO(pServerInfo, pServer, "Voting", G_VOTING_ENABLED_KEY);
	//-----------------------------------------------------------------------
	pServerInfo->m_aInfos.push_back(GameInfo("Spectator Modes:", ""));
	int SpectrModes = xrGS_SBServerGetIntValue(pServer, m_QR2.xrGS_RegisteredKey(G_SPECTATOR_MODES_KEY), 0);
	
	pServerInfo->m_aInfos.push_back(GameInfo("  Free Fly", ((SpectrModes & (1<<CSpectator::eacFreeFly	)) != 0) ? "Yes" : "No"));
	pServerInfo->m_aInfos.push_back(GameInfo("  First Eye", ((SpectrModes & (1<<CSpectator::eacFirstEye	)) != 0) ? "Yes" : "No"));
	pServerInfo->m_aInfos.push_back(GameInfo("  Look At", ((SpectrModes & (1<<CSpectator::eacLookAt	)) != 0) ? "Yes" : "No"));
	pServerInfo->m_aInfos.push_back(GameInfo("  Free Look", ((SpectrModes & (1<<CSpectator::eacFreeLook	)) != 0) ? "Yes" : "No"));
	if (pServerInfo->m_GameType != GAME_DEATHMATCH)
		pServerInfo->m_aInfos.push_back(GameInfo("  Team Only", ((SpectrModes & (1<<CSpectator::eacMaxCam	)) != 0) ? "Yes" : "No"));
	//-----------------------------------------------------------------------
	
	if (pServerInfo->m_GameType == GAME_DEATHMATCH || pServerInfo->m_GameType == GAME_TEAMDEATHMATCH) 
		ADD_INT_INFO_N (pServerInfo, pServer, "Frag Limit", "", G_FRAG_LIMIT_KEY);	
	ADD_TIME_INFO(pServerInfo, pServer, 1/60000.0f, "Time Limit", "%.0f min", G_TIME_LIMIT_KEY);

	ADD_BOOL_INFO(pServerInfo, pServer, "Invinsibility Indicators", G_DAMAGE_BLOCK_INDICATOR_KEY);
	ADD_TIME_INFO(pServerInfo, pServer, 1/1000.0f, "Invinsibility Tyme", "%.f sec", G_DAMAGE_BLOCK_TIME_KEY);

	ADD_TIME_INFO(pServerInfo, pServer, 1/1000.0f, "Force Respawn", "%.f sec", G_FORCE_RESPAWN_KEY);
	ADD_BOOL_INFO(pServerInfo, pServer, "Anomalies", G_ANOMALIES_ENABLED_KEY);
	if ((xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(G_ANOMALIES_ENABLED_KEY), SBFalse)) == SBTrue)
		ADD_TIME_INFO(pServerInfo, pServer, 1/60000.0f, "Anomalies Period", "%.1f min", G_ANOMALIES_TIME_KEY);

	ADD_TIME_INFO(pServerInfo, pServer, 1/1000.0f, "Warm Up Time", "%.0f sec", G_WARM_UP_TIME_KEY);

	if (pServerInfo->m_GameType == GAME_TEAMDEATHMATCH || pServerInfo->m_GameType == GAME_ARTEFACTHUNT)
	{
		ADD_BOOL_INFO(pServerInfo, pServer, "Auto Team Balance", G_AUTO_TEAM_BALANCE_KEY);
		ADD_BOOL_INFO(pServerInfo, pServer, "Auto Team Swap", G_AUTO_TEAM_SWAP_KEY);
		ADD_BOOL_INFO(pServerInfo, pServer, "Friendly Indicators", G_FRIENDLY_INDICATORS_KEY);
		ADD_BOOL_INFO(pServerInfo, pServer, "Friendly Names", G_FRIENDLY_NAMES_KEY);

		ADD_INT_INFO_N (pServerInfo, pServer, "Friendly Fire", " %%", G_FRIENDLY_FIRE_KEY);
	};

	if (pServerInfo->m_GameType == GAME_ARTEFACTHUNT)
	{
		pServerInfo->m_aInfos.push_back(GameInfo("Artefacts:", ""));
		ADD_INT_INFO(pServerInfo, pServer, "  Count",					G_ARTEFACTS_COUNT_KEY	);

		ADD_TIME_INFO(pServerInfo, pServer, 1/60000.0f, "  Stay Time", "%.2f min", G_ARTEFACT_STAY_TIME_KEY);
		ADD_TIME_INFO(pServerInfo, pServer, 1/1000, "  Respawn Time", "%.0f sec", G_ARTEFACT_RESPAWN_TIME_KEY);

		int Reinforcement = atoi(xrGS_SBServerGetStringValue(pServer, m_QR2.xrGS_RegisteredKey(G_REINFORCEMENT_KEY), "0"));		
		switch (Reinforcement)
		{
		case -1:
			pServerInfo->m_aInfos.push_back(GameInfo("Players Respawn", "Artefact Captured"));
			break;
		case 0:
			pServerInfo->m_aInfos.push_back(GameInfo("Players Respawn", "Any Time"));
			break;
		default:
			ADD_TIME_INFO(pServerInfo, pServer, 1/1000, "Players Respawn", "each %.0f sec", G_REINFORCEMENT_KEY);
			break;
		}

		ADD_BOOL_INFO(pServerInfo, pServer, "Shielded Bases",					G_SHIELDED_BASES_KEY	);
		ADD_BOOL_INFO(pServerInfo, pServer, "Return Players",					G_RETURN_PLAYERS_KEY	);
		ADD_BOOL_INFO(pServerInfo, pServer, "AfBearer Can't Sprint",			G_BEARER_CANT_SPRINT_KEY);
	}
//	pServerInfo->m_aInfos.push_back(GameInfo("Map Rotation", ((xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(G_MAP_ROTATION_KEY), SBFalse)) == SBTrue)? "Yes" : "No"));
//	pServerInfo->m_aInfos.push_back(GameInfo("Voting", ((xrGS_SBServerGetBoolValue(pServer, m_QR2.xrGS_RegisteredKey(G_VOTING_ENABLED_KEY), SBFalse)) == SBTrue)? "Yes" : "No"));
	/*
	pQR2->KeyBufferAdd(keybuffer, G_MAP_ROTATION_KEY);
	pQR2->KeyBufferAdd(keybuffer, G_VOTING_ENABLED_KEY);
	//---- game sv mp ----
	pQR2->KeyBufferAdd(keybuffer, G_SPECTATOR_MODES_KEY);
	//---- game_sv_deathmatch ----
	pQR2->KeyBufferAdd(keybuffer, G_FRAG_LIMIT_KEY);				
	pQR2->KeyBufferAdd(keybuffer, G_TIME_LIMIT_KEY);				
	pQR2->KeyBufferAdd(keybuffer, G_DAMAGE_BLOCK_TIME_KEY);			
	pQR2->KeyBufferAdd(keybuffer, G_DAMAGE_BLOCK_INDICATOR_KEY);
	pQR2->KeyBufferAdd(keybuffer, G_ANOMALIES_ENABLED_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_ANOMALIES_TIME_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_WARM_UP_TIME_KEY);			
	pQR2->KeyBufferAdd(keybuffer, G_FORCE_RESPAWN_KEY);			
	//---- game_sv_teamdeathmatch ----
	pQR2->KeyBufferAdd(keybuffer, G_AUTO_TEAM_BALANCE_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_AUTO_TEAM_SWAP_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_INDICATORS_KEY);	
	pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_NAMES_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_FIRE_KEY);			
	//---- game_sv_artefacthunt ----	
	pQR2->KeyBufferAdd(keybuffer, G_ARTEFACTS_COUNT_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_ARTEFACT_STAY_TIME_KEY);	
	pQR2->KeyBufferAdd(keybuffer, G_ARTEFACT_RESPAWN_TIME_KEY);	
	pQR2->KeyBufferAdd(keybuffer, G_REINFORCEMENT_KEY);			
	pQR2->KeyBufferAdd(keybuffer, G_SHIELDED_BASES_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_RETURN_PLAYERS_KEY);		
	pQR2->KeyBufferAdd(keybuffer, G_BEARER_CANT_SPRINT_KEY);	
	*/
	//--------- Read Players Info -------------------------//
	pServerInfo->m_aPlayers.clear();
	for (int i=0; i<pServerInfo->m_ServerNumPlayers; i++)
	{
		PlayerInfo PI;
		sprintf(PI.Name, "%s", xrGS_SBServerGetPlayerStringValue(pServer, i,	"name", "Unknown"));
		PI.Frags =		s16(xrGS_SBServerGetPlayerIntValue(pServer, i,			"frags", 0));
		PI.Deaths =		u16(xrGS_SBServerGetPlayerIntValue(pServer, i,			"death", 0));
		PI.Rank =		u8(xrGS_SBServerGetPlayerIntValue(pServer, i,			"rank", 0));
		PI.Team =		u8(xrGS_SBServerGetPlayerIntValue(pServer, i,			"p_team", 0));
		PI.Spectator =	(xrGS_SBServerGetPlayerIntValue(pServer, i,				"spectator", 1)) != 0;
		PI.Artefacts =	u8(xrGS_SBServerGetPlayerIntValue(pServer, i,			"artefacts", 0));

		pServerInfo->m_aPlayers.push_back(PI);
	};
	//----------- Read Team Info ---------------------------//
	pServerInfo->m_aTeams.clear();
	if (pServerInfo->m_GameType == GAME_TEAMDEATHMATCH || pServerInfo->m_GameType == GAME_ARTEFACTHUNT)
	{
		for (int i=0; i<pServerInfo->m_ServerNumTeams; i++)
		{
			TeamInfo TI;
			TI.Score = u8(xrGS_SBServerGetTeamIntValue(pServer, i, "t_score", 0));
			pServerInfo->m_aTeams.push_back(TI);
		}		
	}
};

void			CGameSpy_Browser::RefreshQuick(int Index)
{
	void* pServer = xrGS_ServerBrowserGetServer(m_pGSBrowser, Index);
	if (!pServer) return;
	ServerInfo xServerInfo;
	ReadServerInfo(&xServerInfo, pServer);
	xrGS_ServerBrowserAuxUpdateServer(m_pGSBrowser, pServer, SBFalse, SBTrue);
//	xrGS_ServerBrowserAuxUpdateIP(m_pGSBrowser, xServerInfo.m_HostName, xServerInfo.m_Port, SBFalse, SBFalse, SBTrue);
};

void			CGameSpy_Browser::OnUpdateFailed		(void* server)
{
	xrGS_ServerBrowserRemoveServer(m_pGSBrowser, server);
}

void			CGameSpy_Browser::Update()
{
	xrGS_ServerBrowserThink(m_pGSBrowser);
};

void			CGameSpy_Browser::UpdateServerList()
{
	if (m_pServerList) m_pServerList->RefreshList();
}