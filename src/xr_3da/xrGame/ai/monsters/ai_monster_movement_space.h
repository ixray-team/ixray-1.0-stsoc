#pragma once

namespace MonsterMovement {

	enum EMovementParameters {
		eVelocityParameterIdle			= u32(1) <<	 1,
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,
		eVelocityParameterInvisible		= u32(1) <<	 9,
		eVelocityParameterRunAttack		= u32(1) <<	 10,

		eVelocityParamsWalk				= eVelocityParameterStand		| eVelocityParameterWalkNormal,
		eVelocityParamsWalkDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged,
		eVelocityParamsRun				= eVelocityParameterStand		| eVelocityParameterWalkNormal	| eVelocityParameterRunNormal,
		eVelocityParamsRunDamaged		= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsAttackNorm		= eVelocityParameterStand		| eVelocityParameterWalkNormal	| eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand		| eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand		| eVelocityParameterSteal,
		eVelocityParamsDrag				= eVelocityParameterStand		| eVelocityParameterDrag,
		eVelocityParamsInvisible		= eVelocityParameterInvisible	| eVelocityParameterStand,
		eVelocityParamsRunAttack		= eVelocityParameterRunAttack	| eVelocityParameterStand, 


		eVelocityParameterCustom		= u32(1) <<	 12,
	};

	enum EMovementParametersChimera {
		eChimeraVelocityParameterUpperWalkFwd		= eVelocityParameterCustom << 1,
		eChimeraVelocityParameterJumpOne			= eVelocityParameterCustom << 2,
		eChimeraVelocityParameterJumpTwo			= eVelocityParameterCustom << 3,

		eChimeraVelocityParamsJump					= eChimeraVelocityParameterJumpOne | eChimeraVelocityParameterJumpTwo,
		eChimeraVelocityParamsUpperWalkFwd			= eVelocityParameterStand | eChimeraVelocityParameterUpperWalkFwd,
	};

	enum EMovementParametersSnork {
		eSnorkVelocityParameterJumpOne		= eVelocityParameterCustom << 1,
		eSnorkVelocityParameterJumpTwo		= eVelocityParameterCustom << 2,

		eSnorkVelocityParamsJump			= eSnorkVelocityParameterJumpOne | eSnorkVelocityParameterJumpTwo,
	};

};


