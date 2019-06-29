////////////////////////////////////////////////////////////////////////////
//	Module 		: animation_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Animation action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "control_action.h"
#include "ai_monster_space.h"

class CAnimationAction : public CControlAction {
protected:
	MonsterSpace::EMentalState	m_mental_state;

public:
	IC					CAnimationAction	();
	IC					CAnimationAction	(const MonsterSpace::EMentalState &mental_state);
			void		execute				();
};

#include "animation_action_inline.h"

