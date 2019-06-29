////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_processor.cpp
//	Created 	: 19.09.2003
//  Modified 	: 22.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script processor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_engine.h"
#include "ai_script_processor.h"
#include "ai_script.h"
#include "ai_space.h"

#include "script_debugger.h"

DEFINE_VECTOR(LPSTR,LPSTR_VECTOR,LPSTR_IT);

CScriptProcessor::CScriptProcessor(LPCSTR caCaption, LPCSTR caScriptString)
{
	Msg				("* Initializing %s script processor",caCaption);
	m_name[0]		= 0;
	strcat(m_name,caCaption);
	u32				N = _GetItemCount(caScriptString);
	string256		I;
	for (u32 i=0; i<N; ++i)
		add_script	(_GetItem(caScriptString,i,I));
}

CScriptProcessor::~CScriptProcessor()
{
	SCRIPT_IT		I = m_tpScripts.begin();
	SCRIPT_IT		E = m_tpScripts.end();
	for ( ; I != E; ++I)
		xr_delete	(*I);
}

void CScriptProcessor::run_scripts()
{
	LPSTR			S;
	for ( ; !m_scripts_to_run.empty(); ) {
		LPSTR		I = m_scripts_to_run.back();
		S			= xr_strdup(I);
		xr_free		(I);
		m_scripts_to_run.pop_back();

		CScript		*l_tpScript = xr_new<CScript>(S);
		xr_free		(S);

		if (l_tpScript->m_bActive)
			m_tpScripts.push_back(l_tpScript);
		else
			xr_delete(l_tpScript);
	}
}

void CScriptProcessor::run_strings()
{
	for ( ; !m_strings_to_run.empty(); ) {
		LPSTR		I = m_strings_to_run.back();
		int			err_code = lua_dostring(ai().script_engine().lua(),I);

		if (err_code) {
			if (!ai().script_engine().print_output(ai().script_engine().lua(),"console_string",err_code))
				ai().script_engine().print_error(ai().script_engine().lua(),err_code);
		}
		xr_free		(I);
		m_strings_to_run.pop_back();
	}
}

void CScriptProcessor::update()
{
	run_scripts			();
	run_strings			();
	LPSTR	S = g_ca_stdout;
	for (int i=0, n=(int)m_tpScripts.size(); i<n; ++i) {
		strcpy	(m_caOutput,"");
		g_ca_stdout = m_caOutput;
		if (!m_tpScripts[i]->Update()) {
			xr_delete	(m_tpScripts[i]);
			m_tpScripts.erase(m_tpScripts.begin() + i);
			--i;
			--n;
			continue;
		}
		if (xr_strlen(m_caOutput))
			ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeInfo,"%s",m_caOutput);
	}
#ifdef _DEBUG
	lua_setgcthreshold	(ai().script_engine().lua(),0);
#endif
	g_ca_stdout			= S;
}

void CScriptProcessor::add_script	(LPCSTR	script_name)
{
	m_scripts_to_run.push_back(xr_strdup(script_name));
}

void CScriptProcessor::add_string	(LPCSTR	string_to_run)
{
	m_strings_to_run.push_back(xr_strdup(string_to_run));
}