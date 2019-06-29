////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_planner.h
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat planner
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_planner_action_script.h"

class CAI_Stalker;

class CStalkerCombatPlannerNew : public CActionPlannerActionScript<CAI_Stalker> {
public:
	enum {
		POST_COMBAT_WAIT_INTERVAL = 3000,
	};

private:
	typedef CActionPlannerActionScript<CAI_Stalker> inherited;

private:
	u32					m_last_level_time;
	u16					m_last_enemy_id;

protected:
			void		add_evaluators				();
			void		add_actions					();

public:
			void xr_stdcall	on_best_cover_changed	(const CCoverPoint *new_cover, const CCoverPoint *old_cover);

public:
						CStalkerCombatPlannerNew	(CAI_Stalker *object = 0, LPCSTR action_name = "");
	virtual				~CStalkerCombatPlannerNew	();
	virtual	void		setup						(CAI_Stalker *object, CPropertyStorage *storage);
	virtual void		update						();
	virtual void		initialize					();
	virtual void		execute						();
	virtual void		finalize					();
};
