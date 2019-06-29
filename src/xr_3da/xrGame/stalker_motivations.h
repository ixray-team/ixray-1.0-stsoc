////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wrapper_abstract.h"
#include "motivation.h"
#include "motivation_action.h"
//#include "script_game_object.h"

class CAI_Stalker;
class CScriptGameObject;

typedef CWrapperAbstract<CAI_Stalker,CMotivation>		CStalkerMotivation;
typedef CWrapperAbstract<CAI_Stalker,CMotivationAction>	CStalkerMotivationAction;

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

class CMotivationGlobal : public CStalkerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

//////////////////////////////////////////////////////////////////////////
// CMotivationAlive
//////////////////////////////////////////////////////////////////////////

class CMotivationAlive : public CStalkerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

//////////////////////////////////////////////////////////////////////////
// CMotivationSquad
//////////////////////////////////////////////////////////////////////////

class CMotivationSquad : public CStalkerMotivation {
public:
	virtual float	evaluate		(u32 sub_motivation_id);
};

//////////////////////////////////////////////////////////////////////////
// CMotivationSquadGoal
//////////////////////////////////////////////////////////////////////////

class CMotivationSquadGoal : public CStalkerMotivationAction {
protected:
	typedef CStalkerMotivationAction inherited;

public:
	IC				CMotivationSquadGoal	(const CWorldState &state);
	virtual void	update					();
};

#include "stalker_motivations_inline.h"