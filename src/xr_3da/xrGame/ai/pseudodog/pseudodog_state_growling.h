#pragma once 

class CPseudodogGrowling : public IState {
	CAI_PseudoDog		*pMonster;

	typedef IState inherited;

	enum {
		ACTION_THREATEN, 
		ACTION_PSI_ATTACK,
		ACTION_ANGRY_IDLE,
	} m_tAction;

	const CEntityAlive *enemy;

	TTime				m_dwFaceEnemyLastTime;
	TTime				time_next_psi_attack;

public:
					CPseudodogGrowling	(CAI_PseudoDog *p);

private:
	virtual void	Init			();
	virtual void	Run				();

			bool	CheckPsiAttack	();
};
