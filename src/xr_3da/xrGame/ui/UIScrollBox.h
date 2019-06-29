//////////////////////////////////////////////////////////////////////
// UIScrollBox.h: класс перемещаемой каретки в CScrollBar
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLL_BOX_H_
#define _UI_SCROLL_BOX_H_

#pragma once
#include "uibutton.h"

class CUIScrollBox :
	public CUIButton
{
public:
	CUIScrollBox(void);
	virtual ~CUIScrollBox(void);

	virtual void Init(float x, float y, float length, float broad);
			void SetHorizontal();
			void SetVertical();

	//сообщения, отправляемые родительскому окну
//	typedef enum{SCROLLBOX_MOVE, SCROLLBOX_STOP} E_MESSAGE;


	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void Draw();
						

protected:
	
	//горизонтальный или вертикальный 
	bool m_bIsHorizontal;

	float m_iOldMouseX;
	float m_iOldMouseY;

};

#endif