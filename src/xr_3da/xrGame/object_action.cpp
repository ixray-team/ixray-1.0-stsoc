////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action.cpp
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Object action
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_action.h"
#include "ai/stalker/ai_stalker.h"

void CObjectAction::execute	()
{
	m_object->set_goal		(m_item_action,m_item);
}