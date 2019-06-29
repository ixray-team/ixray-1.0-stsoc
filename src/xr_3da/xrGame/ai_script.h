////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script.h
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Scripting system with Lua as a scripting language usage
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"
#include "script_stack_tracker.h"

class CScript : public CScriptStackTracker {
public:
	LPSTR	m_script_name;
	int		m_thread_reference;
	bool	m_bActive;

					CScript		(LPCSTR caNamespaceName);
	virtual			~CScript	();
			bool	Update		();
};