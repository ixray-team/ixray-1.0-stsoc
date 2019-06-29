////////////////////////////////////////////////////////////////////////////
//	Module 		: state_internal.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Internal state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_management_config.h"

template <
	typename _Object
>
class CStateInternal {
public:
	enum ESoundInfo {
		eSoundInfoNoSound		= u32(0),
		eSoundInfoNonDangerous	= u32(1) << 0,
		eSoundInfoDangerous		= u32(1) << 1,
		eSoundInfoSound			= eSoundInfoNonDangerous | eSoundInfoDangerous,
		eSoundInfoDummy			= u32(-1),
	};

	enum EEnemyInfo {
		eEnemyInfoNoEnemy		= u32(0),
		eEnemyInfoVeryDangerous	= u32(1) << 0,
		eEnemyInfoDangerous		= u32(1) << 1,
		eEnemyInfoStrong		= u32(1) << 2,
		eEnemyInfoWeak			= u32(1) << 3,
		eEnemyInfoVeryWeak		= u32(1) << 4,
		eEnemyInfoEnemy			= eEnemyInfoVeryDangerous | eEnemyInfoDangerous	| eEnemyInfoStrong | eEnemyInfoWeak | eEnemyInfoVeryWeak,
		eEnemyInfoExpedient		= u32(1) << 5,
		eEnemyInfoSeeMe			= u32(1) << 6,
		eEnemyInfoDummy			= u32(-1),
	};

	enum EConditionInfo {
		eConditionInfoNoWishes = u32(0),
		eConditionInfoEat,
		eConditionInfoSleep,
		eConditionInfoWound,
		eConditionInfoDummy = u32(-1),
	};

	enum EObjectInfo {
		eObjectInfoNoObjects = u32(0),
		eObjectInfoObjects,
		eObjectInfoDummy = u32(-1),
	};

private:
	ESoundInfo			m_sound_info;
	EEnemyInfo			m_enemy_info;
	EConditionInfo		m_condition_info;
	EObjectInfo			m_object_info;
	_Object				*m_object;
	float				m_attack_success_probability[4];
	
			void		update_sound_info		();
			void		update_enemy_info		();
			void		update_condition_info	();
			void		update_object_info		();
public:
						CStateInternal			();
	virtual				~CStateInternal			();
			void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					();
};

#include "state_internal_inline.h"