#include "StdAfx.h"

#include "../xrGameSpyServer.h"
#include "GameSpy_Keys.h"

#include "../game_sv_artefacthunt.h"
//--------------------------- QR2 callbacks ---------------------------------------
void __cdecl callback_serverkey(int keyid, void* outbuf, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	game_sv_mp* gmMP = smart_cast<game_sv_mp*>(pServer->game);
	game_sv_Deathmatch* gmDM = smart_cast<game_sv_Deathmatch*>(pServer->game);
	game_sv_TeamDeathmatch* gmTDM = smart_cast<game_sv_TeamDeathmatch*>(pServer->game);
	game_sv_ArtefactHunt* gmAhunt = smart_cast<game_sv_ArtefactHunt*>(pServer->game);

	switch (keyid)
	{
	case HOSTNAME_KEY: pQR2->BufferAdd(outbuf, pServer->HostName.c_str()); break;
	case MAPNAME_KEY: pQR2->BufferAdd(outbuf, pServer->MapName.c_str()); break;
	case GAMEVER_KEY: pQR2->BufferAdd(outbuf, "0.1"); break;
	case NUMPLAYERS_KEY: pQR2->BufferAdd_Int(outbuf, pServer->GetPlayersCount()); break;
	case MAXPLAYERS_KEY: pQR2->BufferAdd_Int(outbuf, pServer->m_iMaxPlayers); break;
	case GAMETYPE_KEY:	pQR2->BufferAdd_Int(outbuf, pServer->game->Type());		break;
	case PASSWORD_KEY:
		{
			if ( 0 == *(pServer->Password))
				pQR2->BufferAdd_Int(outbuf, 0);
			else
				pQR2->BufferAdd_Int(outbuf, 1);
		}break;
	case HOSTPORT_KEY: pQR2->BufferAdd_Int(outbuf, pServer->GetPort()); break;

	case DEDICATED_KEY:	pQR2->BufferAdd_Int(outbuf, pServer->m_bDedicated);		break;
	case GAMETYPE_NAME_KEY: pQR2->BufferAdd(outbuf, pServer->game->type_name()); break;
	case NUMTEAMS_KEY: pQR2->BufferAdd_Int(outbuf, gmMP->GetNumTeams()); break;		
		//------- game ---------//	
	case G_MAP_ROTATION_KEY:		if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->HasMapRotation());	break;
	case G_VOTING_ENABLED_KEY:		if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->IsVotingEnabled());	break;
	case G_SPECTATOR_MODES_KEY:		if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->GetSpectatorModes()); break;		
		//------- deathmatch -------//
	case G_FRAG_LIMIT_KEY:				if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetFragLimit());				break;
	case G_TIME_LIMIT_KEY:				if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetTimeLimit());				break;
	case G_DAMAGE_BLOCK_TIME_KEY:		if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetDMBLimit());				break;
	case G_DAMAGE_BLOCK_INDICATOR_KEY:	if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->IsDamageBlockIndEnabled());	break;
	case G_ANOMALIES_ENABLED_KEY:		if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->IsAnomaliesEnabled());		break;
	case G_ANOMALIES_TIME_KEY:			if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetAnomaliesTime());			break;
	case G_WARM_UP_TIME_KEY:			if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetWarmUpTime());			break;
	case G_FORCE_RESPAWN_KEY:			if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetForceRespawn());			break;
		//---- game_sv_teamdeathmatch ----
	case G_AUTO_TEAM_BALANCE_KEY:	if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_AutoTeamBalance	());			break;
	case G_AUTO_TEAM_SWAP_KEY:		if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_AutoTeamSwap		());			break;
	case G_FRIENDLY_INDICATORS_KEY: if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_FriendlyIndicators	());			break;
	case G_FRIENDLY_NAMES_KEY:		if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_FriendlyNames		());			break;
	case G_FRIENDLY_FIRE_KEY:		if (gmTDM)pQR2->BufferAdd_Int(outbuf, int(gmTDM->GetFriendlyFire()*100.0f));		break;
		//---- game_sv_artefacthunt ----	
	case G_ARTEFACTS_COUNT_KEY:			if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsCount		());			break;
	case G_ARTEFACT_STAY_TIME_KEY:		if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsStayTime		());			break;
	case G_ARTEFACT_RESPAWN_TIME_KEY:	if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsRespawnDelta	());			break;
	case G_REINFORCEMENT_KEY:			if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ReinforcementTime		());			break;
	case G_SHIELDED_BASES_KEY:			if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ShieldedBases			());			break;
	case G_RETURN_PLAYERS_KEY:			if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ReturnPlayers			());			break;
	case G_BEARER_CANT_SPRINT_KEY:		if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_BearerCantSprint		());			break;
	default:
		{
			R_ASSERT2(0, "Unknown GameSpy Server key ");
		}break;
	}
	//GSI_UNUSED(userdata);
};

void __cdecl callback_playerkey(int keyid, int index, void* outbuf, void *userdata)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	if (u32(index) >= pServer->client_Count()) return;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	xrGameSpyClientData* pCD = NULL;
	
	if (pServer->m_bDedicated)
	{
		if (u32(index+1) >= pServer->client_Count()) return;
		pCD = (xrGameSpyClientData*)pServer->client_Get(index+1);
	}
	else
		pCD = (xrGameSpyClientData*)pServer->client_Get(index);
	if (!pCD || !pCD->ps) return;

	switch (keyid)
	{
	case PLAYER__KEY: pQR2->BufferAdd(outbuf, pCD->ps->getName()); break;
	case P_NAME__KEY: pQR2->BufferAdd(outbuf, pCD->ps->getName()); break;
	case P_FRAGS__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->kills); break;
	case P_DEATH__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->deaths); break;
	case P_RANK__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->rank); break;
	case P_TEAM__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->team); break;
	case P_SPECTATOR__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->testFlag(GAME_PLAYER_FLAG_SPECTATOR)); break;
	case P_ARTEFACTS__KEY: 
		if (pServer->game->Type() == GAME_ARTEFACTHUNT) 
			pQR2->BufferAdd_Int(outbuf, pCD->ps->af_count); break;
			break;
	default:
		{
			R_ASSERT2(0, "Unknown GameSpy Player key ");
		}break;
	}
};

void __cdecl callback_teamkey(int keyid, int index, void* outbuf, void *userdata)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	game_sv_Deathmatch* gmDM = smart_cast<game_sv_Deathmatch*>(pServer->game);
	if (!gmDM || u32(index) >= gmDM->GetNumTeams()) return;

	switch (keyid)
	{
	case T_SCORE_T_KEY:	if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->GetTeamScore(index)); break;
	default:
		{
			R_ASSERT2(0, "Unknown GameSpy Team key ");
		}break;
	};
};

void __cdecl callback_keylist(qr2_key_type keytype, void* keybuffer, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;
	
	switch (keytype)
	{
	case key_server: 
		{
			pQR2->KeyBufferAdd(keybuffer, HOSTNAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, MAPNAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, GAMEVER_KEY);
			pQR2->KeyBufferAdd(keybuffer, NUMPLAYERS_KEY);		
			pQR2->KeyBufferAdd(keybuffer, MAXPLAYERS_KEY);

			pQR2->KeyBufferAdd(keybuffer, GAMETYPE_KEY);
			pQR2->KeyBufferAdd(keybuffer, PASSWORD_KEY);

			pQR2->KeyBufferAdd(keybuffer, HOSTPORT_KEY);
			
			//---- Game Keys
			pQR2->KeyBufferAdd(keybuffer, DEDICATED_KEY);
			pQR2->KeyBufferAdd(keybuffer, GAMETYPE_NAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, NUMTEAMS_KEY);
			//---- game_sv_base ---
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
		} break;
	case key_player:
		{
			pQR2->KeyBufferAdd(keybuffer, P_NAME__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_FRAGS__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_DEATH__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_RANK__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_TEAM__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_SPECTATOR__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_ARTEFACTS__KEY);		
		}break;
	case key_team:
		{
			pQR2->KeyBufferAdd(keybuffer, T_SCORE_T_KEY);
		}break;
	};

	//GSI_UNUSED(userdata);
};

int __cdecl callback_count(qr2_key_type keytype, void *userdata)
{
	if (!userdata) return 0;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	switch (keytype)
	{
	case key_player:
		{
			return pServer->client_Count();
		}break;
	case key_team:
		{
			if (!pServer->game) return 0;
			switch (pServer->game->Type())
			{
			case GAME_DEATHMATCH:
				return 1;
			case GAME_TEAMDEATHMATCH:
				return 2;
			case GAME_ARTEFACTHUNT:
				return 2;
			}
		}break;
	default:
		return 0;
	}

	//GSI_UNUSED(userdata);

	return 0;
};

void __cdecl callback_adderror(qr2_error_t error, char *errmsg, void *userdata)
{
	int x=0;
	x=x;
};

void __cdecl callback_nn(int cookie, void *userdata)
{
};

void __cdecl callback_cm(char *data, int len, void *userdata)
{
};
