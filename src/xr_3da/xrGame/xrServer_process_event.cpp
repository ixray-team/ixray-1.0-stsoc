#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"
#include "game_base.h"
#include "game_cl_base.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_Objects_ALife_Monsters.h"

void xrServer::Process_event	(NET_Packet& P, ClientID sender)
{
	u32			timestamp;
	u16			type;
	u16			destination;
	u32			MODE			= net_flags(TRUE,TRUE);

	// correct timestamp with server-unique-time (note: direct message correction)
	P.r_u32		(timestamp	);

	// read generic info
	P.r_u16		(type		);
	P.r_u16		(destination);

	CSE_Abstract*	receiver	= game->get_entity_from_eid	(destination);
	if (receiver)	
	{
		R_ASSERT(receiver->owner);
		receiver->OnEvent						(P,type,timestamp,sender);

	};

	switch		(type)
	{
	case GE_GAME_EVENT:
		{
			u16		game_event_type;
			P.r_u16(game_event_type);
			game->AddDelayedEvent(P,game_event_type,timestamp,sender);
		}break;
	case GE_INFO_TRANSFER:{
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast			(clientID,P,MODE);
#	ifdef SLOW_VERIFY_ENTITIES
		VERIFY					(verify_entities());
#	endif
		}break;
	case GE_INV_ACTION:
		{
			xrClientData* CL		= ID_to_client(sender);
			if (CL)	CL->net_Ready	= TRUE;
			if (SV_Client) SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY					(verify_entities());
#	endif
		}break;
	case GE_RESPAWN:
		{
			CSE_Abstract*		E	= game->get_entity_from_eid	(destination);
			if (E) 
			{
				R_ASSERT			(E->s_flags.is(M_SPAWN_OBJECT_PHANTOM));

				svs_respawn			R;
				R.timestamp			= timestamp	+ E->RespawnTime*1000;
				R.phantom			= destination;
				q_respawn.insert	(R);
			}
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY					(verify_entities());
#	endif
		}
		break;
	case GE_WPN_STATE_CHANGE:
	case GE_ZONE_STATE_CHANGE:{
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast			(clientID,P,MODE);
#	ifdef SLOW_VERIFY_ENTITIES
		VERIFY					(verify_entities());
#	endif
		}break;
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
		{
			Process_event_ownership	(P,sender,timestamp,destination);
			VERIFY					(verify_entities());
		}break;
	case GE_OWNERSHIP_TAKE_MP_FORCED:
		{
			Process_event_ownership	(P,sender,timestamp,destination,TRUE);
			VERIFY					(verify_entities());
		}break;
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
		{
			Process_event_reject	(P,sender,timestamp,destination,P.r_u16());
			VERIFY					(verify_entities());
		}break;
	case GE_DESTROY:
		{
			Process_event_destroy	(P,sender,timestamp,destination, NULL);
			VERIFY					(verify_entities());
		}
		break;
	case GE_TRANSFER_AMMO:
		{
			u16					id_parent=destination,id_entity;
			P.r_u16				(id_entity);
			CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);	// кто забирает (для своих нужд)
			CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);	// кто отдает
			if (!e_entity)		break;
			if (0xffff != e_entity->ID_Parent)	break;						// this item already taken
			xrClientData*		c_parent	= e_parent->owner;
			xrClientData*		c_from		= ID_to_client	(sender);
			R_ASSERT			(c_from == c_parent);						// assure client ownership of event

			// Signal to everyone (including sender)
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast		(clientID,P,MODE);

			// Perfrom real destroy
			entity_Destroy		(e_entity	);
			VERIFY				(verify_entities());
		}
		break;
	case GE_HIT:
	case GE_HIT_STATISTIC:
		{
			P.r_pos -=2;
			if (type == GE_HIT_STATISTIC) 
			{
				P.B.count -= 4;
				P.w_u32(sender.value());
			};
			game->AddDelayedEvent(P,GAME_EVENT_ON_HIT, 0, ClientID() );
		} break;
	case GE_ASSIGN_KILLER: {
		u16							id_dest	= destination, id_src;
		P.r_u16						(id_src);
		
		CSE_Abstract				*e_dest = game->get_entity_from_eid	(id_dest);	// кто умер
		VERIFY						(e_dest);

		CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
		if (creature)
			creature->m_killer_id	= id_src;

//		Msg							("[%d][%s] killed [%d][%s]",id_src,id_src==u16(-1) ? "UNKNOWN" : game->get_entity_from_eid(id_src)->name_replace(),id_dest,e_dest->name_replace());

		break;
	}
	case GE_CHANGE_VISUAL:
		{
			CSE_Visual* visual		= smart_cast<CSE_Visual*>(receiver); VERIFY(visual);
			string256 tmp;
			P.r_stringZ				(tmp);
			visual->set_visual		(tmp);
		}break;
	case GE_DIE:
		{
			// Parse message
			u16					id_dest		=	destination, id_src;
			P.r_u16				(id_src);


			xrClientData *l_pC	= ID_to_client(sender);
			VERIFY				(game && l_pC);
			if ((game->Type() != GAME_SINGLE) && l_pC && l_pC->owner)
			{
				Msg					("* [%2d] killed by [%2d] - sended by [%s:%2d]", id_dest, id_src, game->get_option_s(*l_pC->Name,"name","Player"), l_pC->owner->ID);
			}

			CSE_Abstract*		e_dest		= game->get_entity_from_eid	(id_dest);	// кто умер
			VERIFY				(e_dest);
			if (game->Type() != GAME_SINGLE)
				Msg				("* [%2d] is [%s:%s]", id_dest, *e_dest->s_name, e_dest->name_replace());
			CSE_Abstract*		e_src		= game->get_entity_from_eid	(id_src	);	// кто убил
			if (!e_src) 
			{
				xrClientData*	C = (xrClientData*)	game->get_client(id_src);
				if (C) e_src = C->owner;
			};
			VERIFY				(e_src);
			R_ASSERT2			(e_dest && e_src, "Killer or/and being killed are offline or not exist at all :(");
			if (game->Type() != GAME_SINGLE)
				Msg				("* [%2d] is [%s:%s]", id_src, *e_src->s_name, e_src->name_replace());

			game->on_death		(e_dest,e_src);

			xrClientData*		c_src		= e_src->owner;				// клиент, чей юнит убил

			if (c_src->owner->ID == id_src) {
				// Main unit
				P.w_begin			(M_EVENT);
				P.w_u32				(timestamp);
				P.w_u16				(type);
				P.w_u16				(destination);
				P.w_u16				(id_src);
				P.w_clientID		(c_src->ID);
			}

			ClientID clientID;clientID.setBroadcast();
			SendBroadcast			(clientID,P,MODE);

			//////////////////////////////////////////////////////////////////////////
			// 
			if (game->Type() == GAME_SINGLE) {
				P.w_begin			(M_EVENT);
				P.w_u32				(timestamp);
				P.w_u16				(GE_KILL_SOMEONE);
				P.w_u16				(id_src);
				P.w_u16				(destination);
				SendTo				(c_src->ID, P, net_flags(TRUE, TRUE));
			}
			//////////////////////////////////////////////////////////////////////////

			VERIFY					(verify_entities());
		}
		break;
	case GE_GRENADE_EXPLODE:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast		(clientID,P,MODE);
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY				(verify_entities());
#	endif
		}break;
	case GE_ADDON_ATTACH:
	case GE_ADDON_DETACH:
	case GE_CHANGE_POS:
		{			
			SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY					(verify_entities());
#	endif
		}break;
	case GEG_PLAYER_ITEM2SLOT:
	case GEG_PLAYER_ITEM2BELT:
	case GEG_PLAYER_ITEM2RUCK:
		{
			/*
			u16 id = P.r_u16();
			CSE_Abstract*		E	= game->get_entity_from_eid	(id);
			if (E)
			{
				CSE_ALifeItemCustomOutfit* pOutfit = smart_cast<CSE_ALifeItemCustomOutfit*>(E);
				if (pOutfit)
				{
					SendBroadcast		(sender,P,MODE);
					break;
				};
			};
			SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
			*/
			SendBroadcast		(sender,P,MODE);
		}break;
	case GEG_PLAYER_WEAPON_HIDE_STATE:
		{
			SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));

#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY					(verify_entities());
#	endif
		}break;
	case GEG_PLAYER_ACTIVATE_SLOT:
	case GEG_PLAYER_ITEMDROP:
	case GEG_PLAYER_ITEM_EAT:
		{
			SendTo(SV_Client->ID, P, net_flags(TRUE, TRUE));
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY					(verify_entities());
#	endif
		}break;	
	case GEG_PLAYER_ITEM_SELL:
		{
			game->OnPlayer_Sell_Item(sender, P);
		}break;
	case GE_TELEPORT_OBJECT:
		{
			game->teleport_object	(P,destination);
		}break;
	case GE_ADD_RESTRICTION:
		{
			game->add_restriction	(P,destination);
		}break;
	case GE_REMOVE_RESTRICTION:
		{
			game->remove_restriction(P,destination);
		}break;
	case GE_REMOVE_ALL_RESTRICTIONS:
		{
			game->remove_all_restrictions(P,destination);
		}break;

	case GEG_PLAYER_ATTACH_HOLDER:
	case GEG_PLAYER_DETACH_HOLDER:
	case GEG_PLAYER_ACTIVATEARTEFACT:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast		(clientID,P,MODE);
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY				(verify_entities());
#	endif
		}break;
	case GEG_PLAYER_PLAY_HEADSHOT_PARTICLE:
		{
			ClientID clientID;clientID.setBroadcast();
			SendBroadcast		(clientID,P,MODE);
#	ifdef SLOW_VERIFY_ENTITIES
			VERIFY				(verify_entities());
#	endif
		}break;
	case GE_ACTOR_JUMPING:
		{
			SendBroadcast(sender, P, MODE);
		}break;
	case GE_MONEY:
		{
			CSE_Abstract				*e_dest = game->get_entity_from_eid	(destination);
			CSE_ALifeTraderAbstract*	pTa = smart_cast<CSE_ALifeTraderAbstract*>(e_dest);
			pTa->m_dwMoney				= P.r_u32();
						
		}break;
	default:
		R_ASSERT2	(0,"Game Event not implemented!!!");
		break;
	}
}
