
//	Module 		: script_motivation_action_wrapper.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation action wrapper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action.h"
#include "script_space.h"

class CScriptMotivationActionWrapper : public CScriptMotivationAction, public luabind::wrap_base {
public:
	typedef	CScriptMotivationAction	inherited;

protected:
	typedef CScriptGameObject			_object_type;

public:
	IC						CScriptMotivationActionWrapper	(const GraphEngineSpace::CWorldState &goal);
	virtual					~CScriptMotivationActionWrapper	();
	virtual void			setup							(_object_type *object);
	static	void			setup_static					(inherited *motivation, CScriptGameObject *object);
	virtual float			evaluate						(u32 sub_motivation_id);
	static	float			evaluate_static					(inherited *motivation, u32 sub_motivation_id);
};

#include "script_motivation_action_wrapper_inline.h"