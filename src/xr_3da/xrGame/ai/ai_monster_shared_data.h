#pragma once 
#include "../shared_data.h"
#include "ai_monster_defs.h"

class _biting_shared : public CSharedResource {
public:
	// float speed factors
	SVelocityParam			m_fsVelocityNone;
	SVelocityParam			m_fsVelocityStandTurn;
	SVelocityParam			m_fsVelocityWalkFwdNormal;
	SVelocityParam			m_fsVelocityWalkFwdDamaged;
	SVelocityParam			m_fsVelocityRunFwdNormal;
	SVelocityParam			m_fsVelocityRunFwdDamaged;
	SVelocityParam 			m_fsVelocityDrag;
	SVelocityParam 			m_fsVelocitySteal;

	float					m_fDistToCorpse;
	float					m_fMinAttackDist;
	float					m_fMaxAttackDist;

	float					m_fDamagedThreshold;		// порог здоровья, ниже которого устанавливается флаг m_bDamaged

	// -------------------------------------------------------

	TTime					m_dwIdleSndDelay;
	TTime					m_dwEatSndDelay;
	TTime					m_dwAttackSndDelay;

	// -------------------------------------------------------

	// Мораль 
	float					m_fMoraleSuccessAttackQuant;		// увеличение морали при успешной атаке
	float					m_fMoraleDeathQuant;				// уменьшение морали при смерти монстра из одной команды
	float					m_fMoraleFearQuant;					// уменьшение морали в панике
	float					m_fMoraleRestoreQuant;				// квант восстановления морали ? 
	float					m_fMoraleBroadcastDistance;			// мораль уменьшается, если в данном радиусе умер монстр из команды

	// ----------------------------------------------------------- 

	u32						m_dwDayTimeBegin;
	u32						m_dwDayTimeEnd;
	float					m_fMinSatiety;
	float					m_fMaxSatiety;
	// ----------------------------------------------------------- 

	float					m_fSoundThreshold;
	float					m_fHitPower;

	float					m_fEatFreq;
	float					m_fEatSlice;
	float					m_fEatSliceWeight;

	u8						m_legs_number;
	SAttackEffector			m_attack_effector;

	float					m_max_hear_dist;

};

class _motion_shared : public CSharedResource {
public:
	ANIM_ITEM_MAP			m_tAnims;			// карта анимаций
	MOTION_ITEM_MAP			m_tMotions;			// карта соответсвий EAction к SMotionItem
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// вектор переходов из одной анимации в другую
	ATTACK_ANIM				aa_all;				// список атак

	t_fx_index				default_fx_indexes;
	FX_MAP_STRING			fx_map_string;
	FX_MAP_U16				fx_map_u16;
	bool					map_converted;
	
	AA_MAP					aa_map;
	//STEPS_MAP				steps_map;
};


