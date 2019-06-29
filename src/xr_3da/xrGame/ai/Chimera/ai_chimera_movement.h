#pragma once

#include "../../movement_manager.h"
#include "../ai_monster_share.h"
#include "../ai_monster_defs.h"


struct _chimera_movement_shared : public CSharedResource {
	// float speed factors
	SVelocityParam			m_fsVelocityNone;
	SVelocityParam			m_fsVelocityStandTurn;
	SVelocityParam			m_fsVelocityWalkFwdNormal;
	SVelocityParam			m_fsVelocityWalkFwdDamaged;
	SVelocityParam			m_fsVelocityRunFwdNormal;
	SVelocityParam			m_fsVelocityRunFwdDamaged;
	SVelocityParam 			m_fsVelocityDrag;
	SVelocityParam 			m_fsVelocitySteal;
};

enum EMovementParameters {
	eVelocityParameterStand			= u32(1) <<  0,
	eVelocityParameterWalk			= u32(1) <<  1,
	eVelocityParameterRun			= u32(1) <<  2,

	eVelocityParameterDamaged		= u32(1) <<  3,

	eVelocityParameterStandDamaged	= eVelocityParameterStand	| eVelocityParameterDamaged,
	eVelocityParameterWalkDamaged	= eVelocityParameterWalk	| eVelocityParameterDamaged,
	eVelocityParameterRunDamaged	= eVelocityParameterRun		| eVelocityParameterDamaged,

	eVelocityParamsStand			= eVelocityParameterStand, 
	eVelocityParamsWalk				= eVelocityParameterStand			| eVelocityParameterWalk,
	eVelocityParamsRun				= eVelocityParameterStand			| eVelocityParameterWalk			| eVelocityParameterRun,
	eVelocityParamsWalkDamaged		= eVelocityParameterStandDamaged	| eVelocityParameterWalkDamaged,
	eVelocityParamsRunDamaged		= eVelocityParamsWalkDamaged		| eVelocityParameterRunDamaged,
};

enum EPathEventType{
	PE_StatePathBuilt,
	PE_StatePathBuildFailed,
	PE_StatePathNeedRebuild,
	PE_StatePathRebuildInProgress,
	PE_PathDisabled
};

class CChimeraMovementManager : virtual	public CMovementManager,
								public CSharedClass<_chimera_movement_shared> {
		
		typedef CMovementManager inherited;
		typedef CSharedClass<_chimera_movement_shared> inherited_shared;

public:
					CChimeraMovementManager	();
	virtual			~CChimeraMovementManager();
	
			void	Init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);

	virtual void	load_shared				(LPCSTR section);

			void	update					();
};

