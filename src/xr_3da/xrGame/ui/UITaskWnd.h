//=============================================================================
//  Filename:   UITaskWnd.h
//  Task subwindow in PDA dialog
//=============================================================================

#ifndef UITASKWND_H_ 
#define UITASKWND_H_

// #pragma once

#include "UIDialogWnd.h"

class CUITaskWnd: public CUIDialogWnd
{
public:
	// Ctor and Dtor
	CUITaskWnd();
	virtual ~CUITaskWnd();

	virtual void Init();
};

#endif