#include "StdAfx.h"

#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "../level.h"
#include "../game_base_space.h"
#include "../ai_space.h"
#include "../script_engine.h"

CUIWpnParams::CUIWpnParams(){
	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textHandling);
	AttachChild(&m_textRPM);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressHandling);
	AttachChild(&m_progressRPM);
}

CUIWpnParams::~CUIWpnParams(){

}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc){
	if (!xml_doc.NavigateToNode("wpn_params", 0))	return;
	CUIXmlInit::InitWindow			(xml_doc, "wpn_params", 0, this);

	CUIXmlInit::InitStatic			(xml_doc, "wpn_params:cap_accuracy",		0, &m_textAccuracy);
	CUIXmlInit::InitStatic			(xml_doc, "wpn_params:cap_damage",			0, &m_textDamage);
	CUIXmlInit::InitStatic			(xml_doc, "wpn_params:cap_handling",		0, &m_textHandling);
	CUIXmlInit::InitStatic			(xml_doc, "wpn_params:cap_rpm",				0, &m_textRPM);

	CUIXmlInit::InitProgressBar		(xml_doc, "wpn_params:progress_accuracy",	0, &m_progressAccuracy);
	CUIXmlInit::InitProgressBar		(xml_doc, "wpn_params:progress_damage",		0, &m_progressDamage);
	CUIXmlInit::InitProgressBar		(xml_doc, "wpn_params:progress_handling",	0, &m_progressHandling);
	CUIXmlInit::InitProgressBar		(xml_doc, "wpn_params:progress_rpm",		0, &m_progressRPM);

	m_progressAccuracy.SetRange		(0, 100);
	m_progressDamage.SetRange		(0, 100);
	m_progressHandling.SetRange		(0, 100);
	m_progressRPM.SetRange			(0, 100);

	bool	functor_exists;
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetRPM" ,m_functorRPM);			VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamage" ,m_functorDamage);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamageMP" ,m_functorDamageMP);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetHandling" ,m_functorHandling);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetAccuracy" ,m_functorAccuracy);	VERIFY(functor_exists);
}

void CUIWpnParams::SetInfo(const shared_str& wpn_section){

	m_progressRPM.SetProgressPos(m_functorRPM(*wpn_section));
	m_progressAccuracy.SetProgressPos(m_functorAccuracy(*wpn_section));
	if (GameID() == GAME_SINGLE)
        m_progressDamage.SetProgressPos(m_functorDamage(*wpn_section));
	else
		m_progressDamage.SetProgressPos(m_functorDamageMP(*wpn_section));
	m_progressHandling.SetProgressPos(m_functorHandling(*wpn_section));
}

bool CUIWpnParams::Check(const shared_str& wpn_section){
	if (pSettings->line_exist(wpn_section, "fire_dispersion_base"))
	{
        if (0==xr_strcmp(wpn_section, "wpn_addon_silencer"))
            return false;
        if (0==xr_strcmp(wpn_section, "wpn_binoc"))
            return false;
        if (0==xr_strcmp(wpn_section, "mp_wpn_binoc"))
            return false;

        return true;		
	}
	else
		return false;
}