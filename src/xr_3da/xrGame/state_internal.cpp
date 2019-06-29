////////////////////////////////////////////////////////////////////////////
//	Module 		: state_internal.cpp
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Internal stalker state
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_internal.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/ai_monsters_misc.h"

CStateInternal::CStateInternal		()
{
	Init				();
}

CStateInternal::~CStateInternal		()
{
}

void CStateInternal::Init			()
{
}

void CStateInternal::Load			(LPCSTR section)
{
}

void CStateInternal::reinit			(CAI_Stalker *object)
{
	m_object			= object;
	VERIFY				(m_object);
	m_sound_info		= eSoundInfoNoSound;
	m_enemy_info		= eEnemyInfoNoEnemy;
	m_condition_info	= eConditionInfoNoWishes;
	m_object_info		= eObjectInfoNoObjects;
}

void CStateInternal::reload			(LPCSTR section)
{
	m_attack_success_probability[0]	= 0.2f;
	m_attack_success_probability[1]	= 0.4f;
	m_attack_success_probability[2]	= 0.6f;
	m_attack_success_probability[3]	= 0.8f;
}

void CStateInternal::update			(u32 time_delta)
{
	update_sound_info		();
	update_enemy_info		();
	update_condition_info	();
	update_object_info		();
}

void CStateInternal::update_sound_info		()
{
	VERIFY				(m_object);
	if (!m_object->sound()) {
		m_sound_info	= eSoundInfoNoSound;
		return;
	}
	if (
		((m_object->sound()->m_sound_type & SOUND_TYPE_WEAPON)				== SOUND_TYPE_WEAPON) || 
		((m_object->sound()->m_sound_type & SOUND_TYPE_MONSTER_DYING)		== SOUND_TYPE_MONSTER_DYING) || 
		((m_object->sound()->m_sound_type & SOUND_TYPE_MONSTER_INJURING)	== SOUND_TYPE_MONSTER_INJURING) || 
		((m_object->sound()->m_sound_type & SOUND_TYPE_MONSTER_ATTACKING)	== SOUND_TYPE_MONSTER_ATTACKING) 
		)
		m_sound_info	= eSoundInfoDangerous;
	else
		m_sound_info	= eSoundInfoNonDangerous;
}

void CStateInternal::update_enemy_info		()
{
	VERIFY				(m_object);
	if (m_object->enemies().empty()) {
		m_enemy_info	= eEnemyInfoNoEnemy;
		return;
	}
	
	switch (
		dwfChooseAction(
			0,
			m_attack_success_probability[0],
			m_attack_success_probability[1],
			m_attack_success_probability[2],
			m_attack_success_probability[3],
			m_object->g_Team(),
			m_object->g_Squad(),
			m_object->g_Group(),
			0,
			1,
			2,
			3,
			4,
			m_object,
			30.f
		)) {
		case 4 : 
			m_enemy_info	= eEnemyInfoVeryDangerous;
			break;
		case 3 : 
			m_enemy_info	= eEnemyInfoDangerous;
			break;
		case 2 : 
			m_enemy_info	= eEnemyInfoStrong;
			break;
		case 1 : 
			m_enemy_info	= eEnemyInfoWeak;
			break;
		case 0 : 
			m_enemy_info	= eEnemyInfoVeryWeak;
			break;
	}

	if (m_object->visible_now(m_object->enemy()) && m_object->see(m_object->enemy(),m_object))
		m_enemy_info		= EEnemyInfo(m_enemy_info | eEnemyInfoSeeMe);

	if (m_object->expedient(m_object->enemy()))
		m_enemy_info		= EEnemyInfo(m_enemy_info | eEnemyInfoExpedient);
}

void CStateInternal::update_condition_info	()
{
	m_condition_info	= eConditionInfoNoWishes;
}

void CStateInternal::update_object_info		()
{
	VERIFY				(m_object);
	if (m_object->items().empty())
		m_object_info	= eObjectInfoNoObjects;
	else
		m_object_info	= eObjectInfoObjects;
}
