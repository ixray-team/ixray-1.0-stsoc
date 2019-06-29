////////////////////////////////////////////////////////////////////////////
//	Module 		: script_monster_space.h
//	Created 	: 06.10.2003
//  Modified 	: 06.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script monster space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ScriptMonster {
	enum EActionType {
		eActionTypeMovement = u32(0),
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,
		eActionTypeCount
	};
};
