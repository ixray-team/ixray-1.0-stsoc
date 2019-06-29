////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_movement_manager.h
//	Created 	: 31.10.2005
//  Modified 	: 31.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human movement manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

class CSE_ALifeHumanAbstract;
class CALifeHumanDetailPathManager;
class CALifeHumanPatrolPathManager;

namespace MovementManager {
	enum EPathType;
};

class CALifeHumanMovementManager {
public:
	typedef CSE_ALifeHumanAbstract							object_type;
	typedef CALifeHumanDetailPathManager					detail_path_type;
	typedef CALifeHumanPatrolPathManager					patrol_path_type;
	typedef MovementManager::EPathType						EPathType;

private:
	object_type					*m_object;
	detail_path_type			*m_detail;
	patrol_path_type			*m_patrol;
	EPathType					m_path_type;

public:
								CALifeHumanMovementManager	(object_type *object);
								~CALifeHumanMovementManager	();
	IC		object_type			&object						() const;
	IC		detail_path_type	&detail						() const;
	IC		patrol_path_type	&patrol						() const;
	IC		const EPathType		&path_type					() const;

public:
			void				update						();
	IC		void				path_type					(const EPathType &path_type);

public:
			bool				completed					() const;
			bool				actual						() const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CALifeHumanMovementManager)
#undef script_type_list
#define script_type_list save_type_list(CALifeHumanMovementManager)

#include "alife_human_movement_manager_inline.h"