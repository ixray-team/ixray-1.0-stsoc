//=============================================================================
//  Filename:   UINotesWnd.cpp
//	---------------------------------------------------------------------------
//  Notes subwindow in Diary window
//=============================================================================

#include "StdAfx.h"
#include "UINotesWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

void CUINotesWnd::Init()
{
	CUIXmlInit xml_init;

	//	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	// Заголовок
//	AttachChild(&UIStaticCaption);
//	xml_init.InitStatic(uiXml, "static", 0, &UIStaticCaption);
}

//////////////////////////////////////////////////////////////////////////

ref_str CUINotesWnd::DialogName()
{
	return "Notes";
}