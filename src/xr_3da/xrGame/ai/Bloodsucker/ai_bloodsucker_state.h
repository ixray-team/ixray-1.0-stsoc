#pragma once

#include "..\\ai_monster_state.h"

class CAI_Bloodsucker;

//////////////////////////////////////////////////////////////////////////
// State REST
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerRest : public IState {
	CAI_Bloodsucker	*pMonster;
	typedef	IState inherited;

	enum {
		ACTION_SIT,
		ACTION_WALK,
		ACTION_SLEEP,
		ACTION_WALK_CIRCUMSPECTION,
	} m_tAction;

	TTime			m_dwReplanTime;						//!< время через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее время планирования

public:
					CBloodsuckerRest(CAI_Bloodsucker *p);
	virtual void	Reset			();
	virtual TTime	UnlockState		(TTime cur_time);
private:
	virtual	void	Init();
	virtual	void	Run();
			void	Replanning();
};

//////////////////////////////////////////////////////////////////////////
// State EAT
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerEat : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;
	
	enum {
		ACTION_CORPSE_APPROACH_RUN,
		ACTION_CORPSE_APPROACH_WALK,
		ACTION_EAT
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< дистанция до трупа
	float			m_fWalkDistToCorpse;		//!< расстояние до трупа, на котором нужно переходить на ходьбу
	
	bool			bTastyCorpse;

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

public:
					CBloodsuckerEat	(CAI_Bloodsucker *p);
	virtual void	Reset			();
private:
	virtual	void	Init			();
	virtual	void	Run				();
	virtual void	Done			();
};


//////////////////////////////////////////////////////////////////////////
// State 'Hear dangerous non-expedient sound' just panic or smth of the kind
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerHearDNE : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;

	enum {
		ACTION_RUN_AWAY,
		ACTION_LOOK_BACK_POSITION,
		ACTION_LOOK_AROUND,
	} m_tAction;

	SoundElem		m_tSound;

	bool			flag_once_1, flag_once_2;
	float			m_fRunAwayDist;
	Fvector			StartPosition, SavedPosition, LastPosition;
	TTime			m_dwLastPosSavedTime;
	TTime			m_dwStayLastTimeCheck;

public:
					CBloodsuckerHearDNE	(CAI_Bloodsucker *p);
private:
	virtual	void	Init			();
	virtual	void	Run				();
};

////////////////////////////////////////////////////////////////////////////////////////////
// State 'Hear non-dangerous expedient sound' just go and see what is going on there
////////////////////////////////////////////////////////////////////////////////////////////
class CBloodsuckerHearNDE : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;

	enum {
		ACTION_LOOK_DESTINATION,
		ACTION_GOTO_SOUND_SOURCE,
	} m_tAction;

	SoundElem		m_tSound;
	bool			flag_once_1, flag_once_2;

public:
					CBloodsuckerHearNDE	(CAI_Bloodsucker *p);
private:
	virtual	void	Init			();
	virtual	void	Run				();
};


////////////////////////////////////////////////////////////////////////////////////////////
// State 'Panic' run away from the place!
////////////////////////////////////////////////////////////////////////////////////////////
class CBloodsuckerPanic : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;

	SEnemy		m_tEnemy;
	
	// implementation of 'face the most open area'
	bool			bFacedOpenArea;
	Fvector			cur_pos;			
	Fvector			prev_pos;
	TTime			m_dwStayTime;

public:
					CBloodsuckerPanic	(CAI_Bloodsucker *p);
	virtual	void	Reset				();
private:
	virtual	void	Init				();
	virtual	void	Run					();

};
