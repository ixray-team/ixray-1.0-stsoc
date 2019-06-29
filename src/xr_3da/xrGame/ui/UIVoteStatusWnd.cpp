#include "stdafx.h"
#include "UIVoteStatusWnd.h"
#include "UIXmlInit.h"



void UIVoteStatusWnd::InitFromXML(CUIXml& xml_doc)
{
	AttachChild					(&m_str_message);
	AttachChild					(&m_hint);
	AttachChild					(&m_time_message);
	CUIXmlInit::InitFrameWindow	(xml_doc, "vote_wnd",						0,	this);
	CUIXmlInit::InitStatic		(xml_doc, "vote_wnd:static_str_message",	0, &m_str_message);
	CUIXmlInit::InitStatic		(xml_doc, "vote_wnd:static_hint",			0, &m_hint);
	CUIXmlInit::InitStatic		(xml_doc, "vote_wnd:static_time_message",	0, &m_time_message);
}

void UIVoteStatusWnd::SetVoteTimeResultMsg(LPCSTR s)
{
	m_time_message.SetText	(s);
}

void UIVoteStatusWnd::SetVoteMsg(LPCSTR s)
{
	m_str_message.SetText	(s);
}
