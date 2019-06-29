#include "stdafx.h"
#include "xrserver.h"
#include "xrserver_objects.h"

void ReplaceOwnershipHeader	(NET_Packet& P)
{
	//способ очень грубый, но на данный момент иного выбора нет. Заранее приношу извинения
	u16 NewType = GE_OWNERSHIP_TAKE;
	CopyMemory(&P.B.data[6],&NewType,2);
};

void xrServer::Process_event_ownership(NET_Packet& P, ClientID sender, u32 time, u16 ID, BOOL bForced)
{
	u32				MODE		= net_flags		(TRUE,TRUE);

	// Parse message
	u16					id_parent=ID,id_entity;
	P.r_u16				(id_entity);
	CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);
	CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);
	if (0		==	e_parent)				return;
	if (0		==	e_entity)				return;
	if (0xffff	!=	e_entity->ID_Parent)	return;
	xrClientData*		c_parent	= e_parent->owner;
	xrClientData*		c_entity	= e_entity->owner;
	xrClientData*		c_from		= ID_to_client	(sender);
	if ((GetServerClient() != c_from) && (c_parent != c_from))					return;	//. hack
	//R_ASSERT			(c_parent == c_from);		// assure client only send request for local units
	

	// Game allows ownership of entity
	if (game->OnTouch	(id_parent,id_entity, bForced))
	{

		// Perform migration if needed
		if (c_parent != c_entity)		PerformMigration(e_entity,c_entity,c_parent);

		// Rebuild parentness
		e_entity->ID_Parent			= id_parent;
		e_parent->children.push_back(id_entity);

		if (bForced)
		{
			ReplaceOwnershipHeader(P);
		}
		// Signal to everyone (including sender)
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast		(clientID,P,MODE);
	}

}
