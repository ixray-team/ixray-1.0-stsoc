#pragma once

class CBurer;
class CGameObject;
class CEntityAlive;
class CPhysicsShellHolder;
class CBurerAttackTele : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;
	xr_vector<CPhysicsShellHolder *>	tele_objects;
	
	CPhysicsShellHolder					*selected_object;

	enum {
		ACTION_TELE_STARTED,
		ACTION_TELE_CONTINUE,
		ACTION_TELE_FIRE,
		ACTION_WAIT_TRIPLE_END,
		ACTION_COMPLETED,
	} m_tAction;

	TTime						time_started;

public:

					CBurerAttackTele	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition ();
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	();
	virtual void	UpdateExternal		();

private:
			// Поиск объектов для телекинеза	
			void	FindObjects			();

			// выполнять состояние
			void	ExecuteTeleStart	();
			void	ExecuteTeleContinue ();
			void	ExecuteTeleFire		();

			// Проверка, есть ли хоть один объект под контролем
			bool	IsActiveObjects		();

			// Проверить, может ли стартовать телекинез
			bool	CheckTeleStart		();
			// Выбор подходящих объектов для телекинеза
			void	SelectObjects		();
};

