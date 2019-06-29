#pragma once

#include "../biting/ai_biting.h"
#include "../ai_monster_bones.h"
#include "../ai_monster_jump.h"
#include "../controlled_entity.h"


class CAI_Dog : public CAI_Biting, 
				public CJumping,
				public CControlledEntity<CAI_Dog> {
	
	typedef		CAI_Biting					inherited;
	typedef		CControlledEntity<CAI_Dog>	controlled;

	bool					strike_in_jump;
	bonesManipulation		Bones;

public:
					CAI_Dog				();
	virtual			~CAI_Dog			();	

	virtual void	Load				(LPCSTR section);
	virtual void	UpdateCL			();

	virtual void	reinit				();
	virtual void	StateSelector		();
	virtual void	CheckSpecParams		(u32 spec_params);
	virtual	void	OnSoundPlay			();

	virtual void	OnJumpStop			();
	virtual bool	CanJump				() {return true;}
	virtual bool	CanExecRotationJump	() {return true;}

	virtual bool	ability_can_drag	() {return true;}
	virtual bool	ability_can_jump	() {return true;}


};
