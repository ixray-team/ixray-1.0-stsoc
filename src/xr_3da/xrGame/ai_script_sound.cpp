////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_sound.h
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_sound.h"
#include "ai_script_classes.h"

void CLuaSound::Play			(CLuaGameObject *object, float delay, int flags)
{
	VERIFY				(m_tpSound);
	m_tpSound->play		(object->object(),flags,delay);
}

void CLuaSound::PlayUnlimited	(CLuaGameObject *object, float delay, int flags)
{
	VERIFY				(m_tpSound);
	VERIFY				((flags & sm_Looped) != sm_Looped);
	m_tpSound->play_unlimited(object->object(),flags,delay);
}

void CLuaSound::PlayAtPos		(CLuaGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_tpSound);
	m_tpSound->play_at_pos(object->object(), position,flags,delay);
}

void CLuaSound::PlayAtPosUnlimited(CLuaGameObject *object, const Fvector &position, float delay, int flags)
{
	VERIFY				(m_tpSound);
	VERIFY				((flags & sm_Looped) != sm_Looped);
	m_tpSound->play_at_pos_unlimited(object->object(), position,flags,delay);
}