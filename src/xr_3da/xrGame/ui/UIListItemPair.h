// File:        UIListItemPair.h
// Description: This list item designed special for CUIListWndEx to represent
//				two properties of save-file - name and date.
//              ______________________________
//             | file_name       |  date/time |
//             |_________________|____________|
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#pragma once

#include "UIListItemEx.h"

class CUIListItemPair : public CUIListItemEx {
public:
	// construction/destruction
	CUIListItemPair();
	virtual ~CUIListItemPair();
//	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void SetHighlightText(bool Highlight); 
	virtual void SetText(LPCSTR file_name, LPCSTR data_time);
	       void SetTextFileName(LPCSTR file_name);
	LPCSTR		 GetTextFileName();
	        void SetTextDateTime(LPCSTR data_time);
	LPCSTR		 GetTextDateTime();
	        void SetFontFileName(CGameFont* pFont);
	        void SetFontDateTime(CGameFont* pFont);
			int  GetBorder();
			void SetBorder(int iBorder);
	virtual LPCSTR GetText();
	virtual void SetTextColor(u32 color);

	virtual void Draw();

protected:
	

	CUIStatic m_staticFileName;
	CUIStatic m_staticDateTime;
	int       m_iBorder;
};