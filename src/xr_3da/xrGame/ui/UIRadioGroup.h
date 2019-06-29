/////////////////////////////////////////////////////////////////////
// CUIRadioGroup.h: класс для работы с группой радио кнопок
//////////////////////////////////////////////////////////////////////

#ifndef _UI_RADIO_GROUP_H_
#define _UI_RADIO_GROUP_H_





#pragma once

#include "uiwindow.h"
#include "uiradiobutton.h"


class CUIRadioGroup :
	public CUIWindow
{
public:
	CUIRadioGroup(void);
	virtual ~CUIRadioGroup(void);

	////////////////////////////////////
	//работа с дочерними и родительскими окнами
	void AttachChild(CUIRadioButton* pChild);
	void DetachChild(CUIRadioButton* pChild);

	
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	//получение/установка позиции в группе
	int GetCheckedPos();	
	void SetCheckedPos(int new_pos);

};


#endif