#pragma once

class CBurer;

class CBurerAttackMelee : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;
	
	TTime time_path_last_rebuild;

	enum {
		ACTION_RUN,
		ACTION_MELEE,
	} m_tAction;

public:

					CBurerAttackMelee	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition ();
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	() {}
	virtual void	UpdateExternal		();

};

