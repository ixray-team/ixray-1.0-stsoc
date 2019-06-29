////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai/stalker/ai_stalker.h"

template <typename _item_type>
IC	CObjectActionBase<_item_type>::CObjectActionBase(_item_type *item, CAI_Stalker *owner, LPCSTR action_name) :
	inherited			(owner,action_name)
{
	m_item					= item;
}

template <typename _item_type>
void CObjectActionBase<_item_type>::initialize	()
{
	inherited::initialize	();
#ifndef OLD_OBJECT_HANDLER
	m_object->set_aimed		(0,false);
	m_object->set_aimed		(1,false);
#endif
}
