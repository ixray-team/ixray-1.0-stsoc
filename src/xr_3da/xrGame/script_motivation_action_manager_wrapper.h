////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager action manager wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"
#include "script_space.h"
class CScriptMotivationActionManagerWrapper : public CScriptMotivationActionManager, public luabind::wrap_base {
protected:
	typedef CScriptGameObject	_object_type;
	typedef	CScriptMotivationActionManager	inherited;

public:
	virtual					~CScriptMotivationActionManagerWrapper	();
	virtual void			setup									(_object_type *object);
	static	void			setup_static							(CScriptMotivationActionManager *manager, CScriptGameObject *object);
	virtual void			update									();
	static	void			update_static							(CScriptMotivationActionManager *manager);
};

#include "script_motivation_action_manager_wrapper_inline.h"