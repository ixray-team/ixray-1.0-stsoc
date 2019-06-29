#pragma once

class CBurer;

class CBurerAttackGravi : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;

	enum {
		ACTION_GRAVI_STARTED,
		ACTION_GRAVI_CONTINUE,
		ACTION_GRAVI_FIRE,
		ACTION_WAIT_TRIPLE_END,
		ACTION_COMPLETED,
	} m_tAction;

	TTime			time_gravi_started;

public:

					CBurerAttackGravi	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition ();
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	();
	virtual void	UpdateExternal		();

private:
			// выполнять состояние
			void	ExecuteGraviStart		();
			void	ExecuteGraviContinue	();
			void	ExecuteGraviFire		();
};

