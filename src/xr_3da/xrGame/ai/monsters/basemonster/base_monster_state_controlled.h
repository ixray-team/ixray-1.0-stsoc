#pragma once 

class CBaseMonsterControlled : public IState {
	typedef IState inherited;
	CBaseMonster		*pMonster;

	const CEntity	*enemy;

	enum {
		ACTION_RUN,
		ACTION_MELEE,
		ACTION_STAND_IDLE,
		ACTION_FOLLOW
	} m_tAction, m_tPrevAction;

	TTime	LastTimeRebuild;
public:
					CBaseMonsterControlled	(CBaseMonster *p);
	virtual void	Init				();
	virtual void	Run					();

			void	ExecuteAttack		();
			void	ExecuteFollow		();
};
