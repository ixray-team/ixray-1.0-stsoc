#pragma once


//Satan 2 ANDY: there is no necessity to use forward declaration ;)
#include "UIWindow.h"
#include "UILabel.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"
#include "UIEditBox.h"

class CUIXml;

class CUIKeyBinding : public CUIWindow {
public:
	CUIKeyBinding();
	void InitFromXml(CUIXml& xml_doc, LPCSTR path);
#ifdef DEBUG
	void CheckStructure(CUIXml& xml_doc);
	bool IsActionExist(LPCSTR action, CUIXml& xml_doc);
#endif
protected:
	void FillUpList();
	CUILabel		m_header[2];
	CUIFrameWindow	m_frame;
	CUIListWnd		m_list;
	u32				m_dwGroupColor;
	u32				m_dwItemColor;
};