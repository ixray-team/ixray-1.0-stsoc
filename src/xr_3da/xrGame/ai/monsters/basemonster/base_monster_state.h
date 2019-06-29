////////////////////////////////////////////////////////////////////////////
//	Module 		: base_monster_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "../ai_monster_state.h"

class CBaseMonster;

#define IS_NEED_REBUILD() pMonster->movement().IsPathEnd(2,0.5f)

#include "base_monster_state_attack.h"
#include "base_monster_state_exploreNDE.h"
#include "base_monster_state_exploreDNE.h"
#include "base_monster_state_panic.h"
#include "base_monster_run_away.h"
#include "base_monster_state_controlled.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterRest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBaseMonsterRest : public IState {
	CBaseMonster	*pMonster;
	
	enum {
			ACTION_WALK,
			ACTION_SATIETY_GOOD,
			ACTION_SLEEP,
			ACTION_WALK_CIRCUMSPECTION,
			ACTION_WALK_PATH_END
	} m_tAction;
	
	TTime			m_dwLastPlanTime;					//!< последнее время планирования

	bool			m_bFollowPath;

	typedef IState inherited;

public:
					CBaseMonsterRest		(CBaseMonster *p);

	virtual TTime	UnlockState		(TTime cur_time);
private:
	virtual void	Run();
	virtual	void	Init();
			void	Replanning();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBaseMonsterEat : public IState {
	typedef IState inherited;
	CBaseMonster		*pMonster;

	enum {
		ACTION_CORPSE_APPROACH_RUN,
		ACTION_CORPSE_APPROACH_WALK,
		ACTION_PREPARE_DRAG,
		ACTION_DRAG,
		ACTION_WALK_LITTLE_AWAY,
		ACTION_LOOK_AROUND,
		ACTION_WALK,
		ACTION_EAT,
		ACTION_GET_HIDE,
		ACTION_LITTLE_REST,
	} m_tAction;

	CEntityAlive	*pCorpse;
	float			m_fDistToCorpse;			//!< дистанция до трупа

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

	Fvector			SavedPos;					// сохранённая позиция до трупа
	float			m_fDistToDrag;				// на какое расстояние тащить
	bool			bDragging;
	bool			bEatRat;

	bool			flag_once_1;
	TTime			m_dwStandStart;
	TTime			m_dwPrepareDrag;
	bool			bEating;

	bool			bRestAfterLunch;
	bool			bHideAfterLunch;
	TTime			m_dwTimeStartRest;
	TTime			rebuild_path;

	bool			Captured;
	TTime			m_dwLastImpulse;

	Fvector			cover_position;
	u32				cover_vertex_id;

public:
					CBaseMonsterEat		(CBaseMonster *p);

	virtual	void	Reset			();
	virtual	bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
	virtual void	Done			();
};



//////////////////////////////////////////////////////////////////////////
// Тестовое состояние CBaseMonsterNull
//////////////////////////////////////////////////////////////////////////
class CBaseMonsterNull : public IState {
public:
	void Run() {}
};

//////////////////////////////////////////////////////////////////////////
// Тестовое состояние CBaseMonsterTest
//////////////////////////////////////////////////////////////////////////

class CBaseMonsterTest : public IState {
	typedef IState inherited;
	CBaseMonster		*pMonster;

	u32		vertex_id;
	TTime	last_time;
	u32		start_vertex_id;	

public:
	CBaseMonsterTest(CBaseMonster *p);
	
	void Init();
	void Run();
};
