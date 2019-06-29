////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.h
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CScript;

DEFINE_VECTOR(CScript*,SCRIPT_VECTOR,SCRIPT_IT);

class CScriptProcessor {
protected:
	SCRIPT_VECTOR			m_tpScripts;
	string4096				m_caOutput;
	xr_vector<LPSTR>		m_scripts_to_run;
	xr_vector<LPSTR>		m_strings_to_run;
	string256				m_name;
protected:
			void		run_scripts					();
			void		run_strings					();

public:
						CScriptProcessor			(LPCSTR					caCaption, LPCSTR caScriptString);
	virtual				~CScriptProcessor			();
			void		update						();
			void		add_script					(LPCSTR					script_name);
			void		add_string					(LPCSTR					string_to_run);
			const SCRIPT_VECTOR&	scripts			() const {return m_tpScripts;};
			LPCSTR		name						() const {return m_name;};
};