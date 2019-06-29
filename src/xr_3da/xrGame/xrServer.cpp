// xrServer.cpp: implementation of the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Objects_ALife_All.h"
#include "level.h"
#include "game_cl_base.h"
#include "ai_space.h"

#include "script_space.h"
#include "../XR_IOConsole.h"
//#include "script_engine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
xrClientData::xrClientData	():IClient(Device.GetTimerGlobal())
{
	ps			= Level().Server->game->createPlayerState();
	ps->clear();

	Clear();
}

void	xrClientData::Clear()
{
	owner		= NULL;
	net_Ready	= FALSE;
	net_Accepted = FALSE;
	net_PassUpdates = TRUE;
};

xrClientData::~xrClientData()
{
	xr_delete(ps);
}


xrServer::xrServer():IPureServer(Device.GetTimerGlobal())
{
}

xrServer::~xrServer()
{
	for (u32 it=0;it<net_Players.size(); it++) client_Destroy(net_Players[it]);
	for (it=0;it<net_Players_disconnected.size(); it++) client_Destroy(net_Players_disconnected[it]);
}

//--------------------------------------------------------------------
xrClientData*	xrServer::ID_to_client		(ClientID ID)
{
	if (0==ID.value())			return NULL;
	csPlayers.Enter		();
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		if (net_Players[client]->ID==ID)	{
			csPlayers.Leave		();
			return (xrClientData*)net_Players[client];
		}
	}
	for (u32 client=0; client<net_Players_disconnected.size(); ++client)
	{
		if (net_Players_disconnected[client]->ID==ID)	{
			csPlayers.Leave		();
			return (xrClientData*)net_Players_disconnected[client];
		}
	}
	csPlayers.Leave		();
	return NULL;
}

CSE_Abstract*	xrServer::ID_to_entity		(u16 ID)
{
	// #pragma todo("??? to all : ID_to_entity - must be replaced to 'game->entity_from_eid()'")	
	if (0xffff==ID)				return 0;
	xrS_entities::iterator	I	= entities.find	(ID);
	if (entities.end()!=I)		return I->second;
	else						return 0;
}

//--------------------------------------------------------------------
IClient*	xrServer::client_Create		()
{
	return xr_new<xrClientData> ();
}
void		xrServer::client_Replicate	()
{
}

IClient*	xrServer::client_Find_Get			(ClientID ID)
{
	char	cAddress[4];
	DWORD	dwPort;
	if (GetClientAddress(ID, cAddress, &dwPort))
	{		
		for (u32 i=0; i<net_Players_disconnected.size(); i++)
		{
			IClient* CLX = net_Players_disconnected[i];
			if ((CLX->m_cAddress[0] == cAddress[0]) &&
				(CLX->m_cAddress[1] == cAddress[1]) &&
				(CLX->m_cAddress[2] == cAddress[2]) &&
				(CLX->m_cAddress[3] == cAddress[3])
				)
			{				
				net_Players_disconnected.erase(net_Players_disconnected.begin()+i);

				*((DWORD*)CLX->m_cAddress) = *((DWORD*)cAddress);	
				CLX->m_dwPort = dwPort;
				CLX->flags.bReconnect = TRUE;
				Msg("Player found");
				return CLX;
			};
		};
	};

	IClient* res = client_Create();
	*((DWORD*)res->m_cAddress) = *((DWORD*)cAddress);	
	res->m_dwPort = dwPort;
	Msg("Player not found");
	return res;
};

INT	g_sv_Client_Reconnect_Time = 0;
void		xrServer::client_Destroy	(IClient* C)
{
	csPlayers.Enter	();

	// Delete assosiated entity
	// xrClientData*	D = (xrClientData*)C;
	// CSE_Abstract* E = D->owner;
	for (u32 DI=0; DI<net_Players_disconnected.size(); DI++)
	{
		if (net_Players_disconnected[DI] == C)
		{
			xr_delete(C);
			net_Players_disconnected.erase(net_Players_disconnected.begin()+DI);
			break;
		};
	};

	for (u32 I=0; I<net_Players.size(); I++)
	{
		if (net_Players[I] == C)
		{
			if (!g_sv_Client_Reconnect_Time || !C->flags.bVerified)
			{
				xr_delete(C);				
			}
			else
			{
				C->dwTime_LastUpdate = Device.dwTimeGlobal;
				net_Players_disconnected.push_back(C);				
				((xrClientData*)C)->Clear();
			};
			net_Players.erase	(net_Players.begin()+I);
			break;
		};
	}

	csPlayers.Leave();
}

//--------------------------------------------------------------------
#ifdef DEBUG
INT	g_Dump_Update_Write = 0;
INT g_sv_SendUpdate = 0;
#endif
void xrServer::Update	()
{
	NET_Packet		Packet;
	u32				position;

	csPlayers.Enter	();

	VERIFY						(verify_entities());

	// game update
	game->ProcessDelayedEvent();
	game->Update	();

	// spawn queue
	u32 svT				= Device.TimerAsync();
	while (!(q_respawn.empty() || (svT<q_respawn.begin()->timestamp)))
	{
		// get
		svs_respawn	R		= *q_respawn.begin();
		q_respawn.erase		(q_respawn.begin());

		// 
		CSE_Abstract* E	= ID_to_entity(R.phantom);
		E->Spawn_Write		(Packet,FALSE);
		u16					ID;
		Packet.r_begin		(ID);	R_ASSERT(M_SPAWN==ID);
		ClientID clientID; clientID.set(0xffff);
		Process_spawn		(Packet,clientID);
	}

	// 
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		// Initialize process and check for available bandwidth
		xrClientData*	Client		= (xrClientData*) net_Players	[client];
		if (!Client->net_Ready)		continue;		
		if (!HasBandwidth(Client) 
#ifdef DEBUG
			&& !g_sv_SendUpdate
#endif
			)	continue;		

		// Send relevant entities to client
		// CSE_Abstract*	Base	= Client->owner;
		u16 PacketType = M_UPDATE;
		Packet.w_begin	(PacketType);

		// GameUpdate
		++(Client->game_replicate_id);
		u32		g_it				= (Client->game_replicate_id % client_Count());
//		u32		g_id				= net_Players[g_it]->ID;
		ClientID g_id 				= net_Players[g_it]->ID;
		game->net_Export_Update		(Packet,Client->ID,g_id);

//		if (!Client->flags.bLocal)	
			game->net_Export_GameTime(Packet);

		if (!Client->flags.bLocal || client_Count() == 1)
		{
#ifdef DEBUG
			if (g_Dump_Update_Write) Msg("---- UPDATE_Write to %s --- ", *(Client->Name));
#endif					// Entities
			
			NET_Packet tmpPacket;
			xrS_entities::iterator	I=entities.begin(),E=entities.end();
			for (; I!=E; ++I)
			{
				CSE_Abstract&	Test = *(I->second);

				if (0==Test.owner)							continue;	// Phantom(?)
				if (!Test.net_Ready)						continue;
				if (Test.owner == Client && Client->flags.bLocal)					continue;	// Can't be relevant

				if (Test.s_flags.is(M_SPAWN_OBJECT_PHANTOM))	continue;	// Surely: phantom
				
				tmpPacket.B.count = 0;
				// write specific data
				{
					tmpPacket.w_u16			(Test.ID		);
					tmpPacket.w_chunk_open8	(position		);
					Test.UPDATE_Write		(tmpPacket			);
#ifdef DEBUG
					if (g_Dump_Update_Write) Msg("* %s : %d", Test.name(), u32(tmpPacket.w_tell()-position)-sizeof(u8));
#endif
					tmpPacket.w_chunk_close8	(position		);
					if (Packet.B.count + tmpPacket.B.count < NET_PacketSizeLimit)
					{
						Packet.w(tmpPacket.B.data, tmpPacket.B.count);
					}
					else
					{
						if (Packet.B.count > 2)	
						{
//							if (!Client->flags.bLocal)
//								Msg ("- Server Update[%d] to Client  : %d", PacketType, Packet.B.count);
							SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
						}
						PacketType = M_UPDATE_OBJECTS;
						Packet.w_begin(PacketType);
					}
				}
			}
#ifdef DEBUG
			if (g_Dump_Update_Write) Msg("----------------------- ");
#endif			
		};

		if (Packet.B.count > 2)	
		{
//			if (!Client->flags.bLocal)
//				Msg ("- Server Update[%d] to Client  : %d", PacketType, Packet.B.count);
			SendTo			(Client->ID,Packet,net_flags(FALSE,TRUE));
		}
	}
#ifdef DEBUG
	g_sv_SendUpdate = 0;
#endif			

	if (game->sv_force_sync)	Perform_game_export();

	VERIFY						(verify_entities());
	//-----------------------------------------------------
	//Remove any of long time disconnected players
	for (u32 DI = 0; DI<net_Players_disconnected.size(); DI++)
	{
		IClient* CL = net_Players_disconnected[DI];
		if (CL->dwTime_LastUpdate+g_sv_Client_Reconnect_Time*60000<Device.dwTimeGlobal)
			client_Destroy(CL);
	}
	//-----------------------------------------------------
	csPlayers.Leave	();
}

u32 xrServer::OnMessage	(NET_Packet& P, ClientID sender)			// Non-Zero means broadcasting with "flags" as returned
{
	if (g_pGameLevel && Level().IsDemoSave()) Level().Demo_StoreServerData(P.B.data, P.B.count);
	u16			type;
	P.r_begin	(type);

	csPlayers.Enter			();

	VERIFY							(verify_entities());
	xrClientData* CL				= ID_to_client(sender);

	switch (type)
	{
	case M_UPDATE:	
		{
			Process_update			(P,sender);						// No broadcast
			VERIFY					(verify_entities());
		}break;
	case M_SPAWN:	
		{
//			R_ASSERT(CL->flags.bLocal);
			if (CL->flags.bLocal)
				Process_spawn		(P,sender);	

			VERIFY					(verify_entities());
		}break;
	case M_EVENT:	
		{
			Process_event			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_EVENT_PACK:
		{
			NET_Packet	tmpP;
			while (!P.r_eof())
			{
				tmpP.B.count = P.r_u8();
				P.r(&tmpP.B.data, tmpP.B.count);

				OnMessage (tmpP, sender);
			};			
		}break;
	case M_CL_UPDATE:
		{
			xrClientData* CL		= ID_to_client	(sender);
			if (!CL) break;
			CL->net_Ready	= TRUE;
			if (!CL->net_PassUpdates) break;
			//-------------------------------------------------------------------
			u32 ClientPing = CL->stats.getPing();
			P.w_seek(P.r_tell()+2, &ClientPing, 4);
			//-------------------------------------------------------------------
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(TRUE, TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_MOVE_PLAYERS_RESPOND:
		{
			xrClientData* CL		= ID_to_client	(sender);
			if (!CL) break;
			CL->net_Ready	= TRUE;
			CL->net_PassUpdates = TRUE;
		}break;
	//-------------------------------------------------------------------
	case M_CL_PING_CHALLENGE:
		{
			P.r_u32();
			u32 id = sender.value();
			P.w_seek( P.r_tell(), &(id), 4);
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(FALSE));
		}break;
	case M_CL_PING_CHALLENGE_RESPOND:
		{
			P.r_u32();
			u32 id = P.r_u32();
			ClientID clientID; clientID.set(id);
			u32 clLastPing = P.r_u32();
			xrClientData* pCL = ID_to_client(clientID);
			pCL->ps->Rping = u16(clLastPing);
			SendTo	(clientID, P, net_flags(FALSE));
		}break;
		//-------------------------------------------------------------------
	case M_CL_INPUT:
		{
			xrClientData* CL		= ID_to_client	(sender);
			if (CL)	CL->net_Ready	= TRUE;
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(TRUE, TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_GAMEMESSAGE:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_CLIENTREADY:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	
			{
				CL->net_Ready	= TRUE;
				CL->ps->DeathTime = Device.dwTimeGlobal;
				game->OnPlayerConnectFinished(sender);
			};
			game->signal_Syncronize	();
			VERIFY					(verify_entities());
		}break;
	case M_SWITCH_DISTANCE:
		{
			game->switch_distance	(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_CHANGE_LEVEL:
		{
			if (game->change_level(P,sender)){
				ClientID clientID;clientID.setBroadcast();
				SendBroadcast		(clientID,P,net_flags(TRUE,TRUE));
				}
			VERIFY					(verify_entities());
		}break;
	case M_SAVE_GAME:
		{
			game->save_game			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_LOAD_GAME:
		{
			game->load_game			(P,sender);
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_RELOAD_GAME:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
			VERIFY					(verify_entities());
		}break;
	case M_SAVE_PACKET:
		{
			Process_save			(P,sender);
			VERIFY					(verify_entities());
		}break;
	case M_CLIENT_REQUEST_CONNECTION_DATA:
		{
			xrClientData* CL			= ID_to_client	(sender);
			OnCL_Connected				(CL);
		}break;
	case M_CHAT_MESSAGE:
		{
			xrClientData *l_pC			= ID_to_client(sender);
			OnChatMessage				(&P, l_pC);
		}break;
	case M_CHANGE_LEVEL_GAME:
		{
			ClientID CID; CID.set		(0xffffffff);
			SendBroadcast				(CID,P,net_flags(TRUE,TRUE));
		}break;
	case M_CL_AUTH:
		{
			game->AddDelayedEvent		(P,GAME_EVENT_PLAYER_AUTH, 0, sender);
		}break;
	case M_PAUSE_GAME:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
		}break;
	case M_STATISTIC_UPDATE:
		{
			ClientID clientID;clientID.setBroadcast();
			if (SV_Client)
				SendBroadcast			(SV_Client->ID,P,net_flags(TRUE,TRUE));
			else
				SendBroadcast			(clientID,P,net_flags(TRUE,TRUE));
		}break;
	case M_STATISTIC_UPDATE_RESPOND:
		{
			if (SV_Client) SendTo	(SV_Client->ID, P, net_flags(TRUE, TRUE));
		}break;
	case M_PLAYER_FIRE:
		{
			if (game)
				game->OnPlayerFire(sender, P);
		}break;
	}

	VERIFY							(verify_entities());

	csPlayers.Leave					();

	return							IPureServer::OnMessage(P, sender);
}

void xrServer::SendTo_LL			(ClientID ID, void* data, u32 size, u32 dwFlags, u32 dwTimeout)
{
	if (SV_Client && SV_Client->ID==ID)
	{
		// optimize local traffic
		Level().OnMessage		(data,size);
	} else {
		IPureServer::SendTo_LL	(ID,data,size,dwFlags,dwTimeout);
	}
}

//--------------------------------------------------------------------
CSE_Abstract*	xrServer::entity_Create		(LPCSTR name)
{
	return F_entity_Create(name);
}

void			xrServer::entity_Destroy	(CSE_Abstract *&P)
{
//	Msg							("xrServer::entity_Destroy : [%d][%s][%s]",P->ID,P->name(),P->name_replace());
	R_ASSERT					(P);
	P->owner = NULL;
	entities.erase				(P->ID);
	m_tID_Generator.vfFreeID	(P->ID,Device.TimerAsync());
	if (!ai().get_alife() || !P->m_bALifeControl)	
		F_entity_Destroy		(P);
}

//--------------------------------------------------------------------
void			xrServer::Server_Client_Check	( IClient* CL )
{
	clients_Lock	();
	
	if (SV_Client && SV_Client->ID == CL->ID)
	{
		if (!CL->flags.bConnected)
		{
			SV_Client = NULL;
		};
		clients_Unlock	();
		return;
	};

	if (SV_Client && SV_Client->ID != CL->ID)
	{
		clients_Unlock	();
		return;
	};


	if (!CL->flags.bConnected) 
	{
		clients_Unlock();
		return;
	};

	IDirectPlay8Address* pAddr = NULL;
	CHK_DX(NET->GetClientAddress(CL->ID.value(), &pAddr, 0));

	if (pAddr) 
	{
		IBannedClient xClient;
		bool res = GetClientAddress(pAddr, xClient.HAddr);
		if (res)
		{
			char HostIP[4] = {0, 0, 0, 0};
			DWORD	NumAdresses = 0;
			NET->GetLocalHostAddresses(NULL, &NumAdresses, 0);

			IDirectPlay8Address* p_pAddr[256];
			Memory.mem_fill(p_pAddr, 0, sizeof(p_pAddr));

			NumAdresses = 256;
			R_CHK(NET->GetLocalHostAddresses(p_pAddr, &NumAdresses, 0));

			for (DWORD i=0; i<NumAdresses; i++)
			{
				if (!p_pAddr[i]) continue;

				if (!GetClientAddress(p_pAddr[i], HostIP)) continue;
				if (xClient == HostIP)
				{
					CL->flags.bLocal = 1;
					SV_Client = (xrClientData*)(CL);
					break;
				}
				else
				{
					CL->flags.bLocal = 0;
				};				
			};	
		};
	};

	clients_Unlock();
};

bool		xrServer::OnCL_QueryHost		() 
{
	if (game->Type() == GAME_SINGLE) return false;
	return (client_Count() != 0); 
};

CSE_Abstract*	xrServer::GetEntity			(u32 Num)
{
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (u32 C=0; I!=E; ++I,++C)
	{
		if (C == Num) return I->second;
	};
	return NULL;
};


void		xrServer::OnChatMessage(NET_Packet* P, xrClientData* CL)
{
//	string256 ChatMsg;
//	u16 PlayerID = P->r_u16();
	s16 team = P->r_s16();
//	P->r_stringZ(ChatMsg);
	if (!CL->net_Ready) return;
	game_PlayerState* Cps = CL->ps;
	for (u32 client=0; client<net_Players.size(); ++client)
	{
		// Initialize process and check for available bandwidth
		xrClientData*	Client		= (xrClientData*) net_Players	[client];
		game_PlayerState* ps = Client->ps;
		if (!Client->net_Ready) continue;
		if (team != 0 && ps->team != team) continue;
		if (Cps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) && !ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
			continue;
		SendTo(Client->ID, *P);
	};
};

void		xrServer::BannedList_Load		()
{
	string256					temp = "";
	FS.update_path				(temp,"$app_data_root$", GetBannedListName());
	Console->ExecuteScript(temp);
}

#ifdef DEBUG

static	BOOL	_ve_initialized			= FALSE;
static	BOOL	_ve_use					= TRUE;

bool xrServer::verify_entities				() const
{
	if (!_ve_initialized)	{
		_ve_initialized					= TRUE;
		if (strstr(Core.Params,"-~ve"))	_ve_use=FALSE;
	}
	if (!_ve_use)						return true;

	xrS_entities::const_iterator		I = entities.begin();
	xrS_entities::const_iterator		E = entities.end();
	for ( ; I != E; ++I) {
		VERIFY2							((*I).first != 0xffff,"SERVER : Invalid entity id as a map key - 0xffff");
		VERIFY2							((*I).second,"SERVER : Null entity object in the map");
		VERIFY3							((*I).first == (*I).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*I).second->name_replace());
		verify_entity					((*I).second);
	}
	return								(true);
}

void xrServer::verify_entity				(const CSE_Abstract *entity) const
{
	VERIFY(entity->m_wVersion!=0);
	if (entity->ID_Parent != 0xffff) {
		xrS_entities::const_iterator	J = entities.find(entity->ID_Parent);
		VERIFY3							(J != entities.end(),"SERVER : Cannot find parent in the map",entity->name_replace());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							(std::find((*J).second->children.begin(),(*J).second->children.end(),entity->ID) != (*J).second->children.end(),"SERVER : Parent/Children relationship mismatch - Object has parent, but corresponding parent doesn't have children",(*J).second->name_replace());
	}

	xr_vector<u16>::const_iterator		I = entity->children.begin();
	xr_vector<u16>::const_iterator		E = entity->children.end();
	for ( ; I != E; ++I) {
		VERIFY3							(*I != 0xffff,"SERVER : Invalid entity children id - 0xffff",entity->name_replace());
		xrS_entities::const_iterator	J = entities.find(*I);
		VERIFY3							(J != entities.end(),"SERVER : Cannot find children in the map",entity->name_replace());
		VERIFY3							((*J).second,"SERVER : Null entity object in the map",entity->name_replace());
		VERIFY3							((*J).first == (*J).second->ID,"SERVER : ID mismatch - map key doesn't correspond to the real entity ID",(*J).second->name_replace());
		VERIFY3							((*J).second->ID_Parent == entity->ID,"SERVER : Parent/Children relationship mismatch - Object has children, but children doesn't have parent",(*J).second->name_replace());
	}
}

#endif // DEBUG

shared_str xrServer::level_name				(const shared_str &server_options) const
{
	return								(game->level_name(server_options));
}
