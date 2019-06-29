////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;

template <typename _item_type>
class CObjectActionBase : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker> inherited;
	_item_type			*m_item;

public:
	IC					CObjectActionBase	(_item_type *item, CAI_Stalker *owner, LPCSTR action_name = "");
	virtual void		initialize			();
};

typedef CObjectActionBase<CGameObject> CSObjectActionBase;

#include "object_action_base_inline.h"