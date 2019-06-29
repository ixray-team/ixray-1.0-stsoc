#pragma once

#include "../\biting/ai_biting.h"
#include "../\ai_monster_jump.h"

class CAI_PseudoDog : public CAI_Biting, public CJumping {
	typedef		CAI_Biting	inherited;

	bool			strike_in_jump;

	SAttackEffector m_psi_effector;

	float			m_anger_hunger_threshold;
	float			m_anger_loud_threshold;

	TTime			m_time_became_angry;


	IState			*stateGrowling;
	TTime			time_growling;			// время нахождения в состоянии пугания

	ref_sound		psy_effect_sound;		// звук, который играется в голове у актера
	float			psy_effect_turn_angle;

public:
					CAI_PseudoDog		();
	virtual			~CAI_PseudoDog		();	

	virtual void	Load				(LPCSTR section);

	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);

	virtual void	StateSelector		();
	virtual void	UpdateCL			();

	virtual void	OnJumpStop			();
	virtual bool	CanJump				() {return true;}
	
	virtual bool	ability_can_drag	() {return true;}
	virtual bool	ability_psi_attack	() {return true;}
	virtual bool	ability_can_jump	() {return true;}


	virtual void	CheckSpecParams		(u32 spec_params);
	virtual void	play_effect_sound	();

};
