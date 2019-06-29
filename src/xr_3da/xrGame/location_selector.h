////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_selector.h"
#include "gameobject.h"

class CLocationSelector : virtual public CGameObject {
protected:
	struct SEnemySelected {
		CEntity*			m_enemy;
		bool				m_visible;
		float				m_cost;
	};

	Fvector					m_hit_direction;
	u32						m_hit_time;
	u32						m_current_update;
	SEnemySelected			m_selected_enemy;
	u32						m_locate_query_time;
	u32						m_location_query_interval;
	bool					m_selector_failed;

public:
					CLocationSelector		();
	virtual			~CLocationSelector		();
	virtual void	init_evaluator			(PathManagers::CAbstractNodeEvaluator *node_evaluator);
	template <u64 flags>
			void	select_location			(PathManagers::CNodeEvaluator<flags> *node_evaluator);
};