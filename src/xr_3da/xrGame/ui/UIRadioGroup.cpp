/////////////////////////////////////////////////////////////////////
// UIRadioGroup.cpp: класс для работы с группой радио кнопок
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiogroup.h"

CUIRadioGroup::CUIRadioGroup(void)
{
}

CUIRadioGroup::~CUIRadioGroup(void)
{
}


//присоединить радио кнопку
void CUIRadioGroup::AttachChild(CUIRadioButton* pChild)
{
	CUIWindow::AttachChild(pChild);

	pChild->ResetCheck();
}




//отсоединить радио кнопку
void CUIRadioGroup::DetachChild(CUIRadioButton* pChild)
{
	CUIWindow::DetachChild(pChild);
//	m_ChildWndList.remove(pChild);
}


//обработка сообщений от радио кнопок
void CUIRadioGroup::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	//обрабатываем сообщения только от дочерних окон
	if(this != pWnd->GetParent()) return;

	//переустановить флажки на кнопках в группе
	if(msg == RADIOBUTTON_SET)
	{
		//сбросить флажки на всех кнопках
		for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
		{
			((CUIRadioButton*)(*it))->ResetCheck();
		}

		//установить флажок, той кнопке, что вызывала сообщение
		((CUIRadioButton*)pWnd)->SetCheck();
	}
}


int CUIRadioGroup::GetCheckedPos()
{
	int res = -1;
	int i = 0;
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it,++i){
		if(((CUIRadioButton*)(*it))->GetCheck() == true){
			res=i;
			return res;
		}
	}

	return res;
}
	

void CUIRadioGroup::SetCheckedPos(int new_pos)
{
		//сбросить флажки на всех кнопках
		WINDOW_LIST_it it = m_ChildWndList.begin();
		for(; it != m_ChildWndList.end() ; ++it)
			((CUIRadioButton*)(*it))->ResetCheck();

		if(new_pos>=(int)m_ChildWndList.size()) return;
		//установить флажок, той кнопке, что вызывала сообщение
		it = m_ChildWndList.begin();
		std::advance(it,new_pos);
		((CUIRadioButton*)(*it))->SetCheck();
}
