//=============================================================================
//  Filename:   UINotesWnd.h
//	---------------------------------------------------------------------------
//  Notes subwindow in Diary window
//=============================================================================

#ifndef UI_NOTES_WND_H_
#define UI_NOTES_WND_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "xrXMLParser.h"

class CUINotesWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	CUINotesWnd() {}
	virtual ~CUINotesWnd() {}
	virtual void Init();
	// Нет смысла скрывать листбокс, так как все равно пока это практически все, что есть, и
	// придется писать обертки над всеми управляющими функциями листбокса
	CUIListWnd		UIListWnd;

	// Получить заголовок раздела
	virtual ref_str	DialogName();
protected:
	// Название окна
	CUIStatic		UIStaticCaption;
};

#endif