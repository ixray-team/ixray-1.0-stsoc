#pragma once

class CBurer;

class CBurerAttackRunAround : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;

	Fvector						selected_point;
	TTime						time_started;

	Fvector						dest_direction;

public:

					CBurerAttackRunAround	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition () {return true;}
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	() {}
	virtual void	UpdateExternal		();

};

class CBurerAttackFaceTarget : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;
	Fvector						point;

public:

					CBurerAttackFaceTarget	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition () {return true;}
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	() {}
	virtual void	UpdateExternal		();

};

