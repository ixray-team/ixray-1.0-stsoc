#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "HUDmanager.h"
#include "xrserver_objects_alife_monsters.h"
#include "xrserver.h"
#include "Level.h"
#include "game_cl_base.h"

#include "ui\UIBuyWndShared.h"

//-------------------------------------------------------
extern	s32		g_sv_dm_dwFragLimit;
//-------------------------------------------------------
BOOL		g_sv_tdm_bAutoTeamBalance		= FALSE;
BOOL		g_sv_tdm_bAutoTeamSwap			= TRUE;
BOOL		g_sv_tdm_bFriendlyIndicators	= FALSE;
BOOL		g_sv_tdm_bFriendlyNames			= FALSE;
float		g_sv_tdm_fFriendlyFireModifier	= 1.0f;
//-------------------------------------------------------
BOOL	game_sv_TeamDeathmatch::isFriendlyFireEnabled	()	{return (g_sv_tdm_fFriendlyFireModifier > 0.1f);};
float	game_sv_TeamDeathmatch::GetFriendlyFire			()	{ return (g_sv_tdm_fFriendlyFireModifier > 0.1f) ? g_sv_tdm_fFriendlyFireModifier : 0.0f;};
BOOL	game_sv_TeamDeathmatch::Get_AutoTeamBalance		()	{return g_sv_tdm_bAutoTeamBalance		; };
BOOL	game_sv_TeamDeathmatch::Get_AutoTeamSwap		()	{return g_sv_tdm_bAutoTeamSwap			; };
BOOL	game_sv_TeamDeathmatch::Get_FriendlyIndicators	()	{return g_sv_tdm_bFriendlyIndicators	; };
BOOL	game_sv_TeamDeathmatch::Get_FriendlyNames		()	{return g_sv_tdm_bFriendlyNames			; };
//-------------------------------------------------------
void	game_sv_TeamDeathmatch::Create					(shared_str& options)
{
	inherited::Create					(options);
	
	switch_Phase(GAME_PHASE_PENDING);
	
	game_TeamState td;
	td.score		= 0;
	td.num_targets	= 0;
	teams.push_back(td);
	teams.push_back(td);
	//-----------------------------------------------------------
}

void game_sv_TeamDeathmatch::net_Export_State						(NET_Packet& P, ClientID to)
{
	inherited::net_Export_State(P, to);
	P.w_u8			(u8(g_sv_tdm_bFriendlyIndicators));
	P.w_u8			(u8(g_sv_tdm_bFriendlyNames));
}

u8 game_sv_TeamDeathmatch::AutoTeam() 
{
	u32	cnt = get_players_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip || ps->team == 0) continue;
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	}
	return (l_teams[0]>l_teams[1])?2:1;
}

void	game_sv_TeamDeathmatch::AutoBalanceTeams()
{
	if (!g_sv_tdm_bAutoTeamBalance) return;
	//calc team count
	s16 MinTeam, MaxTeam;
	u32 NumToMove;
	u32	cnt = get_players_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; it++)	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC->net_Ready) continue;
		if (ps->Skip) continue;
		if(ps->team>=1) ++(l_teams[ps->team-1]);
	};

	if (l_teams[0] == l_teams[1]) return;

	if (l_teams[0] > l_teams[1]) 
	{
		MinTeam = 1;
		MaxTeam = 0;
	}
	else
	{
		MinTeam = 0;
		MaxTeam = 1;
	};

	NumToMove = (l_teams[MaxTeam] - l_teams[MinTeam]) / 2;
	if (!NumToMove) return;
	///////////////////////////////////////////////////////////////////////
	while (NumToMove)
	{
		///////// get lowest score player from MaxTeam
		u32 LowestPlayer = 0;
		s16 LowestScore = 32767;
		for(it=0; it<cnt; it++)	{
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* ps	= l_pC->ps;
			if (!l_pC->net_Ready) continue;
			if (ps->Skip) continue;
			if (ps->team-1 != MaxTeam) continue;

			if (ps->kills < LowestScore)
			{
				LowestScore = ps->kills;
				LowestPlayer = it;
			};
		};
		///////// move player to opposite team
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(LowestPlayer);
		game_PlayerState* ps	= l_pC->ps;
		ps->team = MinTeam+1;
		NumToMove--;
	}
};

void	game_sv_TeamDeathmatch::OnRoundStart			()
{
	if (!m_bFastRestart) AutoBalanceTeams();
	
	if (!m_bFastRestart) AutoSwapTeams();

	inherited::OnRoundStart	();
};

void game_sv_TeamDeathmatch::OnPlayerConnect	(ClientID id_who)
{
	inherited::OnPlayerConnect	(id_who);

	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	game_PlayerState*	ps_who	=	get_id	(id_who);
	LPCSTR	options				=	get_name_id	(id_who);
	ps_who->team				=	u8(get_option_i(options,"team",AutoTeam()));


	if (ps_who->Skip) return;

	if (!xrCData->flags.bReconnect) Money_SetStart(id_who);
	SetPlayersDefItems(ps_who);
}

void	game_sv_TeamDeathmatch::OnPlayerConnectFinished	(ClientID id_who)
{
	inherited::OnPlayerConnectFinished(id_who);

	xrClientData* xrCData		=	m_server->ID_to_client(id_who);
	// Send Message About Client join Team
	if (xrCData)
	{
		NET_Packet			P;
		//		P.w_begin			(M_GAMEMESSAGE);
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_PLAYER_JOIN_TEAM);
		P.w_stringZ			(get_option_s(*xrCData->Name,"name",*xrCData->Name));
		P.w_u16				(xrCData->ps->team);
		u_EventSend(P);
	};
};

void game_sv_TeamDeathmatch::OnPlayerSelectTeam		(NET_Packet& P, ClientID sender)
{
	xrClientData *l_pC = m_server->ID_to_client(sender);
	s16 l_team; 
	P.r_s16(l_team);
	OnPlayerChangeTeam(l_pC->ID, l_team);
	//-------------------------------------------------
};

void game_sv_TeamDeathmatch::OnPlayerChangeTeam(ClientID id_who, s16 team) 
{
	game_PlayerState*	ps_who	=	get_id	(id_who);
	if (!ps_who) return;
	if (!team) 
	{
		team				= AutoTeam();
	};
	//-----------------------------------------------------	
	NET_Packet Px;
	GenerateGameMessage(Px);
	Px.w_u32(GAME_EVENT_PLAYER_GAME_MENU_RESPOND);
	Px.w_u8(PLAYER_CHANGE_TEAM);
	Px.w_s16(team);
	m_server->SendTo(id_who,Px,net_flags(TRUE,TRUE));
	//-----------------------------------------------------
	if (ps_who->team == team) return;
	//-----------------------------------------------------
	KillPlayer(id_who, ps_who->GameID);
	//-----------------------------------------------------	
	ps_who->setFlag(GAME_PLAYER_FLAG_SPECTATOR);
	//-----------------------------------------------------
	s16 OldTeam = ps_who->team;
	ps_who->team = team;
	TeamStruct* pTS = GetTeamData(team);
	if (pTS)
	{
		if ((ps_who->money_for_round < pTS->m_iM_Start) || (OldTeam == 0))
			Money_SetStart(id_who);
	}

/////////////////////////////////////////////////////////
	//Send Switch team message
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(PLAYER_CHANGE_TEAM);
	P.w_u16				(ps_who->GameID);
	P.w_u16				(ps_who->team);
	P.w_u16				(team);
	u_EventSend(P);
/////////////////////////////////////////////////////////
	

	
	SetPlayersDefItems(ps_who);
	
}

void	game_sv_TeamDeathmatch::OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA)
{
	inherited::OnPlayerKillPlayer(ps_killer, ps_killed, KillType, SpecialKillType, pWeaponA);

	if (!ps_killed || !ps_killer) return;
	
	UpdateTeamScore(ps_killer);	
}

void	game_sv_TeamDeathmatch::UpdateTeamScore			(game_PlayerState* ps_killer)
{
	SetTeamScore(ps_killer->team-1, 0);
	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		game_PlayerState*	ps	=	get_it	(it);
		if (ps->team != ps_killer->team) continue;

		SetTeamScore(ps_killer->team-1, GetTeamScore(ps_killer->team-1)+ps->kills );
	};
}

KILL_RES	game_sv_TeamDeathmatch::GetKillResult			(game_PlayerState* pKiller, game_PlayerState* pVictim)
{
	KILL_RES Res = inherited::GetKillResult(pKiller, pVictim);
	switch (Res)
	{
	case KR_RIVAL:
		{
			if (pKiller->team == pVictim->team)
				Res = KR_TEAMMATE;
		}break;
	default:
		{
		}break;
	};
	return Res;
};

bool	game_sv_TeamDeathmatch::OnKillResult			(KILL_RES KillResult, game_PlayerState* pKiller, game_PlayerState* pVictim)
{	
	bool res = true;
	TeamStruct* pTeam		= GetTeamData(u8(pKiller->team));
	switch (KillResult)
	{
	case KR_TEAMMATE:
		{
			pKiller->kills -= 1;
			if (pTeam) Player_AddMoney(pKiller, pTeam->m_iM_KillTeam);
			res = false;
		}break;
	default:
		{
			res = inherited::OnKillResult(KillResult, pKiller, pVictim);
		}break;
	}
	return res;
};


bool game_sv_TeamDeathmatch::checkForFragLimit()
{
	if (g_sv_dm_dwFragLimit && ((teams[0].score >= g_sv_dm_dwFragLimit )||(teams[1].score >= g_sv_dm_dwFragLimit ) ) ){
		OnFraglimitExceed();
		return true;
	};
	return false;
}

u32		game_sv_TeamDeathmatch::RP_2_Use				(CSE_Abstract* E)
{
	return 0;
};

void	game_sv_TeamDeathmatch::OnPlayerHitPlayer_Case	(game_PlayerState* ps_hitter, game_PlayerState* ps_hitted, SHit* pHitS)
{
	inherited::OnPlayerHitPlayer_Case(ps_hitter, ps_hitted, pHitS);
};

void	game_sv_TeamDeathmatch::OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	inherited::OnPlayerHitPlayer(id_hitter, id_hitted, P);	
};

void	game_sv_TeamDeathmatch::LoadTeams			()
{
	m_sBaseWeaponCostSection._set("teamdeathmatch_base_cost");
	if (!pSettings->section_exist(m_sBaseWeaponCostSection))
	{
		R_ASSERT2(0, "No section for base weapon cost for this type of the Game!");
		return;
	};
	m_strWeaponsData->Load(m_sBaseWeaponCostSection);

	LoadTeamData("teamdeathmatch_team0");
	LoadTeamData("teamdeathmatch_team1");
	LoadTeamData("teamdeathmatch_team2");
};

void	game_sv_TeamDeathmatch::Update					()
{
	inherited::Update	();
	switch(Phase()) 	
	{
	case GAME_PHASE_TEAM1_SCORES :
	case GAME_PHASE_TEAM2_SCORES :
	case GAME_PHASE_TEAMS_IN_A_DRAW :
		{
			if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync())
				OnRoundEnd("Finish");
		} break;
	};
}
extern INT g_sv_Skip_Winner_Waiting;
bool	game_sv_TeamDeathmatch::HasChampion				()
{
	return (GetTeamScore(0)!=GetTeamScore(1) || g_sv_Skip_Winner_Waiting);
}

void	game_sv_TeamDeathmatch::OnTimelimitExceed		()
{
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team, false);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd	("TIME_limit");
}
void	game_sv_TeamDeathmatch::OnFraglimitExceed		()
{
	u8 winning_team = (GetTeamScore(0) < GetTeamScore(1))? 1 : 0;
	OnTeamScore(winning_team, false);
	phase = u16((winning_team)?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
	switch_Phase		(phase);
	OnDelayedRoundEnd	("FRAG_limit");
}
//-----------------------------------------------
void game_sv_TeamDeathmatch::ReadOptions				(shared_str &options)
{
	inherited::ReadOptions(options);
	//-------------------------------
	g_sv_tdm_bAutoTeamBalance		= get_option_i(*options, "abalance",	(g_sv_tdm_bAutoTeamBalance		 ? 1 : 0)) != 0;
	g_sv_tdm_bAutoTeamSwap			= get_option_i(*options,"aswap",		(g_sv_tdm_bAutoTeamSwap		 ? 1 : 0)) != 0;
	g_sv_tdm_bFriendlyIndicators	= get_option_i(*options,"fi",			(g_sv_tdm_bFriendlyIndicators	 ? 1 : 0)) != 0;
	g_sv_tdm_bFriendlyNames		= get_option_i(*options,"fn",			(g_sv_tdm_bFriendlyNames		 ? 1 : 0)) != 0;

	float fFF = get_option_f				(*options,"ffire",g_sv_tdm_fFriendlyFireModifier);
	g_sv_tdm_fFriendlyFireModifier			= fFF;
}

static bool g_bConsoleCommandsCreated_TDM = false;
void game_sv_TeamDeathmatch::ConsoleCommands_Create	()
{
//	inherited::ConsoleCommands_Create();
	//-------------------------------------
//	string1024 Cmnd;
	//-------------------------------------	
//	CMD_ADD(CCC_SV_Int,"sv_auto_team_balance", (int*)&g_sv_tdm_bAutoTeamBalance,0,1,g_bConsoleCommandsCreated_TDM,Cmnd);
//	CMD_ADD(CCC_SV_Int,"sv_auto_team_swap", (int*)&g_sv_tdm_bAutoTeamSwap,0,1,g_bConsoleCommandsCreated_TDM,Cmnd);
//	CMD_ADD(CCC_SV_Int,"sv_friendly_indicators", (int*)&g_sv_tdm_bFriendlyIndicators, 0,1,g_bConsoleCommandsCreated_TDM,Cmnd);
//	CMD_ADD(CCC_SV_Int,"sv_friendly_names", (int*)&g_sv_tdm_bFriendlyNames, 0,1,g_bConsoleCommandsCreated_TDM,Cmnd);
//	CMD_ADD(CCC_SV_Float,"sv_friendlyfire", &m_fFriendlyFireModifier, 0.0f,2.0f,g_bConsoleCommandsCreated_TDM,Cmnd);
	//-------------------------------------
//	g_bConsoleCommandsCreated_TDM = true;
};

void game_sv_TeamDeathmatch::ConsoleCommands_Clear	()
{
	inherited::ConsoleCommands_Clear();
	//-----------------------------------
	CMD_CLEAR("sv_auto_team_balance");
	CMD_CLEAR("sv_auto_team_swap");
	CMD_CLEAR("sv_friendly_indicators");
	CMD_CLEAR("sv_friendly_names");
	CMD_CLEAR("sv_friendlyfire");
};

void	game_sv_TeamDeathmatch::AutoSwapTeams			()
{
	if (!g_sv_tdm_bAutoTeamSwap) return;

	u32		cnt = get_players_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		// init
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		if (!l_pC || !l_pC->net_Ready || !l_pC->ps) continue;
		game_PlayerState* ps	= l_pC->ps;
		if (ps->Skip) continue;		

		if (ps->team != 0)
			ps->team	=	(ps->team == 1) ? 2 : 1;
	}
}