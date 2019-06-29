////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_script.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation action manager class with script support
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "motivation_action_manager.h"
//#include "script_game_object.h"

class CScriptGameObject;

template <
	typename _object_type,
	template <typename _object_type> class _motivation_type = CMotivation,
	template <typename _object_type> class _motivation_action_type = CMotivationAction
>
class CMotivationActionManagerScript : 
	public CMotivationActionManager<
		CScriptGameObject,
		_motivation_type,
		_motivation_action_type
	> 
{
protected:
	typedef CMotivationActionManager<
		CScriptGameObject,
		_motivation_type,
		_motivation_action_type
	> inherited;

protected:
	_object_type	*m_object;

public:
	IC				CMotivationActionManagerScript	();
	virtual			~CMotivationActionManagerScript	();
	virtual void	setup							(_object_type *object);
};

#include "motivation_action_manager_script_inline.h"