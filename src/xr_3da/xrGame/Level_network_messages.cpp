#include "stdafx.h"
#include "entity.h"
#include "xrserver_objects.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "Physics.h"
#include "xrServer.h"
#include "Actor.h"
#include "game_cl_base_weapon_usage_statistic.h"

void CLevel::ClientReceive()
{

	Demo_StartFrame();

	Demo_Update();

	m_dwRPC = 0;
	m_dwRPS = 0;

	for (NET_Packet* P = net_msg_Retreive(); P; P=net_msg_Retreive())
	{
		//-----------------------------------------------------
		m_dwRPC++;
		m_dwRPS += P->B.count;
		//-----------------------------------------------------
		u16			m_type;
		u16			ID;
		P->r_begin	(m_type);
		switch (m_type)
		{
		case M_SPAWN:			
			{
				/*/
				cl_Process_Spawn(*P);
				/*/
				game_events->insert		(*P);
				if (g_bDebugEvents)		ProcessGameEvents();
				//*/
			}
			break;
		case M_EVENT:
			game_events->insert		(*P);
			if (g_bDebugEvents)		ProcessGameEvents();
			break;
		case M_EVENT_PACK:
			NET_Packet	tmpP;
			while (!P->r_eof())
			{
				tmpP.B.count = P->r_u8();
				P->r(&tmpP.B.data, tmpP.B.count);
				tmpP.timeReceive = P->timeReceive;

				game_events->insert		(tmpP);
				if (g_bDebugEvents)		ProcessGameEvents();
			};			
			break;
		case M_UPDATE:
			{
//				if (m_dwPingLastSendTime == 0) m_dwPingLastSendTime = timeServer_Async();

				game->net_import_update	(*P);
				//-------------------------------------------
				if (OnServer()) break;
				//-------------------------------------------
				Objects.net_Import		(P);

				if (OnClient()) UpdateDeltaUpd(timeServer());
				IClientStatistic pStat = Level().GetStatistic();
				u32 dTime = 0;
				
				if ((Level().timeServer() + pStat.getPing()) < P->timeReceive)
				{
#ifdef DEBUG
//					Msg("! TimeServer[%d] < TimeReceive[%d]", Level().timeServer(), P->timeReceive);
#endif
					dTime = pStat.getPing();
				}
				else
					dTime = Level().timeServer() - P->timeReceive + pStat.getPing();

				u32 NumSteps = ph_world->CalcNumSteps(dTime);
				SetNumCrSteps(NumSteps);
			}break;
		case M_UPDATE_OBJECTS:
			{
				Objects.net_Import		(P);
			}break;
		//----------- for E3 -----------------------------
		case M_CL_UPDATE:
			{
				if (OnClient()) break;
				P->r_u16		(ID);
				u32 Ping = P->r_u32();
				CGameObject*	O	= smart_cast<CGameObject*>(Objects.net_Find		(ID));
				if (0 == O)		break;
				O->net_Import(*P);
		//---------------------------------------------------
				UpdateDeltaUpd(timeServer());
				if (pObjects4CrPr.empty() && pActors4CrPr.empty())
					break;

				u32 dTime = 0;
				if ((Level().timeServer() + Ping) < P->timeReceive)
				{
#ifdef DEBUG
//					Msg("! TimeServer[%d] < TimeReceive[%d]", Level().timeServer(), P->timeReceive);
#endif
					dTime = Ping;
				}
				else					
					dTime = Level().timeServer() - P->timeReceive + Ping;
				u32 NumSteps = ph_world->CalcNumSteps(dTime);
				SetNumCrSteps(NumSteps);

				O->CrPr_SetActivationStep(u32(ph_world->m_steps_num) - NumSteps);
				AddActor_To_Actors4CrPr(O);

			}break;
		case M_MOVE_PLAYERS:
			{
				u8 Count = P->r_u8();
				for (u8 i=0; i<Count; i++)
				{
					u16 ID = P->r_u16();					
					Fvector NewPos, NewDir;
					P->r_vec3(NewPos);
					P->r_vec3(NewDir);

					CActor*	OActor	= smart_cast<CActor*>(Objects.net_Find		(ID));
					if (0 == OActor)		break;
					OActor->MoveActor(NewPos, NewDir);
				};

				NET_Packet PRespond;
				PRespond.w_begin(M_MOVE_PLAYERS_RESPOND);
				Send(PRespond, net_flags(TRUE, TRUE));
			}break;
		//------------------------------------------------
		case M_CL_INPUT:
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_ImportInput(*P);
			}break;
		//---------------------------------------------------
		case M_CL_PING_CHALLENGE:
			{
				NET_Packet NewP;
				NewP.w_begin(m_type+1);//M_CL_PING_CHALLENGE_RESPOND);
				NewP.w_u32(P->r_u32());
				NewP.w_u32(P->r_u32());
				NewP.w_u32(P->r_u32());
				Send(NewP, net_flags(TRUE, TRUE));
			}break;
		case M_CL_PING_CHALLENGE_RESPOND:
			{
				u32 Time = P->r_u32();
				u32 Ping = timeServer_Async() - Time;
				m_dwRealPing = (m_dwRealPing*9+Ping)/10;
			}break;
		//---------------------------------------------------
		case 	M_SV_CONFIG_NEW_CLIENT:
			InitializeClientGame(*P);
			break;
		case M_SV_CONFIG_GAME:
			game->net_import_state	(*P);
			break;
		case M_SV_CONFIG_FINISHED:
			game_configured			= TRUE;
			break;		
		case M_MIGRATE_DEACTIVATE:	// TO:   Changing server, just deactivate
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_MigrateInactive	(*P);
				if (bDebug)		Log("! MIGRATE_DEACTIVATE",*O->cName());
			}
			break;
		case M_MIGRATE_ACTIVATE:	// TO:   Changing server, full state
			{
				P->r_u16		(ID);
				CObject*	O	= Objects.net_Find		(ID);
				if (0 == O)		break;
				O->net_MigrateActive	(*P);
				if (bDebug)		Log("! MIGRATE_ACTIVATE",*O->cName());
			}
			break;
		case M_CHAT:
			{
				char	buffer[256];
				P->r_stringZ(buffer);
				Msg		("- %s",buffer);
			}
			break;
		case M_GAMEMESSAGE:
			{
				if (!game) break;
				Game().OnGameMessage(*P);
			}break;
		case M_RELOAD_GAME:
		case M_LOAD_GAME:
		case M_CHANGE_LEVEL:
			{
				Engine.Event.Defer	("KERNEL:disconnect");
				Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(*m_caServerOptions)),size_t(xr_strdup(*m_caClientOptions)));
			}break;
		case M_SAVE_GAME:
			{
				ClientSave			();
			}break;
		case M_GAMESPY_CDKEY_VALIDATION_CHALLENGE:
			{
				OnGameSpyChallenge(P);
			}break;
		case M_AUTH_CHALLENGE:
			{
				OnBuildVersionChallenge();
			}break;
		case M_CLIENT_CONNECT_RESULT:
			{
				OnConnectResult(P);
			}break;
		case M_CHAT_MESSAGE:
			{
				if (!game) break;
				Game().OnChatMessage(P);
			}break;
		case M_CHANGE_LEVEL_GAME:
			{
				if (OnClient())
				{
					Engine.Event.Defer	("KERNEL:disconnect");
					Engine.Event.Defer	("KERNEL:start",m_caServerOptions.size() ? size_t( xr_strdup(*m_caServerOptions)) : 0,m_caClientOptions.size() ? size_t(xr_strdup(*m_caClientOptions)) : 0);
				}
				else
				{
					const char* m_SO = m_caServerOptions.c_str();
//					const char* m_CO = m_caClientOptions.c_str();

					m_SO = strchr(m_SO, '/'); if (m_SO) m_SO++;
					m_SO = strchr(m_SO, '/'); 

					string128 LevelName = "";
					string128 GameType = "";

					P->r_stringZ(LevelName);
					P->r_stringZ(GameType);

					string4096 NewServerOptions = "";
					sprintf(NewServerOptions, "%s/%s", LevelName, GameType);

					if (m_SO) strcat(NewServerOptions, m_SO);
					m_caServerOptions = NewServerOptions;

					Engine.Event.Defer	("KERNEL:disconnect");
					Engine.Event.Defer	("KERNEL:start",size_t(xr_strdup(*m_caServerOptions)),size_t(xr_strdup(*m_caClientOptions)));
				};
			}break;
		case M_CHANGE_SELF_NAME:
			{
				net_OnChangeSelfName(P);
			}break;
		case M_PAUSE_GAME:
			{
				if (!net_IsSyncronised()) break;
				u8 Pause = P->r_u8();
				Device.Pause(!(Pause == 0));
				Sound->pause_emitters(!!Device.Pause());
			}break;
		case M_BULLET_CHECK_RESPOND:
			{
				if (!game) break;
				if (GameID() != GAME_SINGLE)
					Game().m_WeaponUsageStatistic->On_Check_Respond(P);
			}break;
		case M_STATISTIC_UPDATE:
			{
				if (!game) break;
				if (GameID() != GAME_SINGLE)
					Game().m_WeaponUsageStatistic->OnUpdateRequest(P);
			}break;
		case M_STATISTIC_UPDATE_RESPOND:
			{
				if (!game) break;
				if (GameID() != GAME_SINGLE)
					Game().m_WeaponUsageStatistic->OnUpdateRespond(P);
			}break;
		}

		net_msg_Release();
	}	

//	if (!g_bDebugEvents) ProcessGameSpawns();
}

void				CLevel::OnMessage				(void* data, u32 size)
{	
	DemoCS.Enter();

	if (IsDemoPlay() ) 
	{
		if (m_bDemoStarted) 
		{
			DemoCS.Leave();
			return;
		}
		
		if (!m_aDemoData.empty() && net_IsSyncronised())
		{
//			NET_Packet *P = &(m_aDemoData.front());
			DemoDataStruct *P = &(m_aDemoData.front());
			u32 CurTime = timeServer_Async();
			timeServer_UserDelta(P->m_dwTimeReceive - CurTime);
			m_bDemoStarted = TRUE;
			Msg("! ------------- Demo Started ------------");
			m_dwCurDemoFrame = P->m_dwFrame;
			DemoCS.Leave();
			return;
		}
	};	

	if (IsDemoSave() && net_IsSyncronised()) 
	{
		Demo_StoreData(data, size, DATA_CLIENT_PACKET);
	}	

	IPureClient::OnMessage(data, size);	

	DemoCS.Leave();
};

NET_Packet*				CLevel::net_msg_Retreive		()
{
	NET_Packet* P = NULL;

	DemoCS.Enter();

	P = IPureClient::net_msg_Retreive();
	if (!P) Demo_EndFrame();

	DemoCS.Leave();

	return P;
}

