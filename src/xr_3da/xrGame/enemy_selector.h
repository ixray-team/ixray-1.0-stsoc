////////////////////////////////////////////////////////////////////////////
//	Module 		: weapon_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Weapon manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CEnemySelector {
public:
	struct SEnemySelected {
		CEntity*							m_enemy;
		bool								m_visible;
		float								m_cost;
	};

	Fvector									m_hit_direction;
	u32										m_hit_time;
	SEnemySelected							m_selected_enemy;

					CEnemySelector			();
	virtual			~CEnemySelector			();
					
};