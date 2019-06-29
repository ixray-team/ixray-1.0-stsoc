#include "stdafx.h"
#include "xrserver.h"
#include "xrserver_objects.h"

bool xrServer::Process_event_reject	(NET_Packet& P, ClientID sender, u32 time, u16 ID, u16 id_entity, bool send_message)
{
	DWORD		MODE		= net_flags(TRUE,TRUE);

	// Parse message
	u16					id_parent = ID;
	CSE_Abstract*		e_parent	= game->get_entity_from_eid	(id_parent);
	CSE_Abstract*		e_entity	= game->get_entity_from_eid	(id_entity);
	if (!e_parent || !e_entity)
		return			(false);

	xrClientData*		c_parent	= e_parent->owner;
	xrClientData*		c_entity	= e_entity->owner;
	// xrClientData*	c_from		= ID_to_client	(sender);
	// R_ASSERT			(c_entity == c_parent);
	// R_ASSERT			(c_parent == c_from);		// assure client only send request for local units

	if (!game->OnDetach(id_parent,id_entity))
		return			(false);

	// Perform migration if needed
	xrClientData*		c_dest		= SelectBestClientToMigrateTo		(e_entity);
	if (c_dest	!= c_entity)		PerformMigration					(e_entity,c_parent,c_dest);

	// Rebuild parentness
	if (0xffff == e_entity->ID_Parent) {
		Msg				("! ERROR: can't detach independant object. entity[%s:%d], parent[%s:%d], section[%s]",
			e_entity->name_replace(),id_entity,e_parent->name_replace(),id_parent, *e_entity->s_name);
		return			(false);
	}

	R_ASSERT			(0xffff != e_entity->ID_Parent);
	VERIFY3				(e_entity->ID_Parent == id_parent,e_entity->name_replace(),e_parent->name_replace());
	e_entity->ID_Parent	= 0xffff;
	xr_vector<u16>& C	= e_parent->children;
	xr_vector<u16>::iterator c	= std::find	(C.begin(),C.end(),id_entity);
	VERIFY3				(C.end()!=c,e_entity->name_replace(),e_parent->name_replace());
	C.erase				(c);

	// Signal to everyone (including sender)
	if (send_message){
		ClientID clientID;clientID.setBroadcast();
		SendBroadcast	(clientID,P,MODE);
	}
	
	return				(true);
}
