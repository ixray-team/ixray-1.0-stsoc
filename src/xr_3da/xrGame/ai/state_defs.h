#pragma once


enum EStateType {
	ST_Rest						= u32(0),
	ST_WalkGraphPoint,
	ST_HideFromPoint,
	ST_MoveAroundPoint,
	ST_MoveToPoint,
	ST_LookPoint,
	ST_LookUnprotectedArea,
	ST_CustomAction,
	ST_Unknown					= u32(-1),
};