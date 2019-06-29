////////////////////////////////////////////////////////////////////////////
//	Module 		: action_dead.cpp
//	Created 	: 25.03.2004
//  Modified 	: 25.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action dead class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_action_free_no_alife.h"
#include "ai/stalker/ai_stalker.h"

CStalkerActionFreeNoALife::CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionFreeNoALife::execute		()
{
	inherited::execute		();
	m_object->play			(eStalkerSoundHumming,60000,10000);
	CStalkerMovementManager	*cast = dynamic_cast<CStalkerMovementManager*>(m_object);
	VERIFY					(cast);
	cast->update			(
		0,
		0,
		0,
		0,
		CMovementManager::ePathTypeGamePath,
		CMovementManager::eDetailPathTypeSmooth,
		eBodyStateStand,
		eMovementTypeWalk,
		eMentalStateFree
		);
	m_object->CSightManager::update				(eLookTypeSearch);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}
