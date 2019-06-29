////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Object action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "control_action.h"
#include "ai_monster_space.h"

class CGameObject;

class CObjectAction : public CControlAction {
protected:
	MonsterSpace::EObjectAction	m_item_action;
	CGameObject					*m_item;

public:
	IC							CObjectAction		();
	IC							CObjectAction		(const MonsterSpace::EObjectAction &item_action, CGameObject *item = 0);
			void				execute				();
};

#include "object_action_inline.h"
