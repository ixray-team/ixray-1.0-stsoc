////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_script.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation script
////////////////////////////////////////////////////////////////////////////

#pragma once

class CStalkerAnimationScript {
private:
	bool					m_hand_usage;
	MotionID				m_animation;

public:
	IC						CStalkerAnimationScript	(bool hand_usage, const MotionID &animation);
	IC	bool				hand_usage				() const;
	IC	const MotionID		&animation				() const;
};

#include "stalker_animation_script_inline.h"