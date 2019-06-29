#pragma once

class CBaseMonsterRunAway : public IState {
	typedef IState inherited;
	CBaseMonster		*pMonster;
	
	enum {
		ACTION_RUN,
		ACTION_LOOK_AROUND
	} m_tAction;


	Fvector danger_pos;

public:

					CBaseMonsterRunAway	(CBaseMonster *p);

private:
	virtual void	Init				();
	virtual void	Run					();
};
