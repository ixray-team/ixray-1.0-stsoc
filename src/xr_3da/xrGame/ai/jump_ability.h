#pragma once

class CAnimTriple;

class CJumpingAbility {
	CAnimTriple		*m_animation_holder;

public:
					CJumpingAbility		();
	virtual			~CJumpingAbility	();

	virtual void	init_external		(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3);

	virtual void	jump				();
	virtual void	update_frame		();
	virtual void	update_scheduled	();
};

