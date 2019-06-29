#pragma once 

class CBitingControlled : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;

	const CEntity	*enemy;

	enum {
		ACTION_RUN,
		ACTION_MELEE,
		ACTION_STAND_IDLE,
		ACTION_FOLLOW
	} m_tAction, m_tPrevAction;

	TTime	LastTimeRebuild;
	TTime	FaceEnemyLastTime;
public:
					CBitingControlled	(CAI_Biting *p);
	virtual void	Init				();
	virtual void	Run					();

			void	ExecuteAttack		();
			void	ExecuteFollow		();
};
