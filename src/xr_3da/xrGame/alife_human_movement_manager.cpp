////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_movement_manager.cpp
//	Created 	: 31.10.2005
//  Modified 	: 31.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human movement manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_movement_manager.h"
#include "alife_human_detail_path_manager.h"
#include "alife_human_patrol_path_manager.h"
#include "object_broker.h"
#include "movement_manager_space.h"

CALifeHumanMovementManager::CALifeHumanMovementManager	(object_type *object)
{
	VERIFY			(object);
	m_object		= object;
	m_detail		= xr_new<detail_path_type>(object);
	m_patrol		= xr_new<patrol_path_type>(object);
	m_path_type		= MovementManager::ePathTypeNoPath;
}

CALifeHumanMovementManager::~CALifeHumanMovementManager	()
{
	delete_data		(m_detail);
	delete_data		(m_patrol);
}

bool CALifeHumanMovementManager::completed				() const
{
	return			(true);
}

bool CALifeHumanMovementManager::actual					() const
{
	return			(true);
}

void CALifeHumanMovementManager::update					()
{
	switch (path_type()) {
		case MovementManager::ePathTypeGamePath : {
			detail().update	();
			break;
		};
		case MovementManager::ePathTypePatrolPath : {
			patrol().update	();

			detail().target	(
				patrol().target_game_vertex_id(),
				patrol().target_level_vertex_id(),
				patrol().target_position()
			);

			detail().update	();

			break;
		};
		case MovementManager::ePathTypeNoPath : {
			break;
		};
		default : NODEFAULT;
	};
}
