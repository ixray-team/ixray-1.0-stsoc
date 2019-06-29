////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_manager_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager manager wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_manager.h"
#include "script_space.h"

class CScriptMotivationManagerWrapper : public CScriptMotivationManager, public luabind::wrap_base {
protected:
	typedef CScriptGameObject				_object_type;
	typedef	CScriptMotivationManager	inherited;

public:
	virtual					~CScriptMotivationManagerWrapper();
	virtual void			setup							(_object_type *object);
	static	void			setup_static					(CScriptMotivationManager *manager, CScriptGameObject *object);
	virtual void			update							();
	static	void			update_static					(CScriptMotivationManager *manager);
};

#include "script_motivation_manager_wrapper_inline.h"