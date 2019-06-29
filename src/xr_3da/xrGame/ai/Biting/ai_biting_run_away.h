#pragma once

class CBitingRunAway : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;
	
	enum {
		ACTION_RUN,
		ACTION_LOOK_AROUND
	} m_tAction;


	Fvector danger_pos;

public:

					CBitingRunAway	(CAI_Biting *p);

private:
	virtual void	Init				();
	virtual void	Run					();
};
