////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Object action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CObjectAction::CObjectAction	()
{
	m_item_action	= MonsterSpace::eObjectActionIdle;
	m_item			= 0;
}

IC	CObjectAction::CObjectAction	(const MonsterSpace::EObjectAction &item_action, CGameObject *item)
{
	m_item_action	= item_action;
	m_item			= item;
}
