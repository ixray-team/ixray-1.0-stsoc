// File:        UIListWndEx.cpp
// Description: This List Box compatible with ListItemEx.
//              Requiered to use feature "Selected Item"
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "StdAfx.h"
#include "UIListWndEx.h"

CUIListWndEx::CUIListWndEx(){
	
}

CUIListWndEx::~CUIListWndEx(){

}

void CUIListWndEx::SendMessage(CUIWindow *pWnd, s16 msg, void* pData){
	if(pWnd == m_ScrollBar)
	{
		if(msg == SCROLLBAR_VSCROLL)
		{
			m_iFirstShownIndex = m_ScrollBar->GetScrollPos();
			UpdateList();
			GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL, NULL);
		}
	}
	else 
	{

		// if message has come from one of child
/*		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
			m_ChildWndList.end(), 
			pWnd);

		if( m_ChildWndList.end() != it)
*/
		if( IsChild(pWnd) )
		{
			CUIListItem* pListItem2, *pListItem = smart_cast<CUIListItem*>(pWnd);
			R_ASSERT(pListItem);

			if(BUTTON_CLICKED == msg)
			{
				GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED, pListItem);
				// 
				for (WINDOW_LIST_it it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = smart_cast<CUIListItem*>(*it);
					if (!pListItem2) 
						continue;
					if (pListItem2->GetGroupID() == -1) 
						continue;
					if (pListItem2->GetGroupID() == pListItem->GetGroupID())
					{
						pListItem2->SetHighlightText(true);
						pListItem2->SendMessage(this, LIST_ITEM_SELECT, pData);
						m_iSelectedItem = pListItem2->GetIndex();
						m_iSelectedItemGroupID = pListItem2->GetGroupID();
					}					
					else
					{
						pListItem2->SetHighlightText(false);
						pListItem2->SendMessage(this, LIST_ITEM_UNSELECT, pData);
					}
				}
			}

			else if(STATIC_FOCUS_RECEIVED == msg)
			{
				if (!m_bForceFocusedItem)
				{
					m_iFocusedItem = pListItem->GetIndex();
					m_iFocusedItemGroupID = pListItem->GetGroupID();
				}
				else if (m_iFocusedItem >= 0)
					m_iFocusedItemGroupID = GetItem(m_iFocusedItem)->GetGroupID();


				// prototype code

				for (WINDOW_LIST_it it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = smart_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					if (pListItem2->GetGroupID() == -1) continue;
					if (pListItem2->GetGroupID() == pListItem->GetGroupID())
					{
						pListItem2->SetHighlightText(true);
						pListItem2->SendMessage(this, STATIC_FOCUS_RECEIVED, pData);
					}					
					else
					{
						pListItem2->SetHighlightText(false);
						pListItem2->SendMessage(this, STATIC_FOCUS_LOST, pData);
					}
				}
				// end prototype code
			}
			else if(STATIC_FOCUS_LOST == msg)
			{
				if(pListItem->GetIndex() == m_iFocusedItem && !m_bForceFocusedItem) m_iFocusedItem = -1;

				for (WINDOW_LIST_it it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = smart_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					pListItem2->SetHighlightText(false);
					pListItem2->SendMessage(this, STATIC_FOCUS_LOST, pData);
				}
				m_bUpdateMouseMove = true;
			}
		}
	}
}

CUIListItemEx* CUIListWndEx::GetExItem(int index){
	return static_cast<CUIListItemEx*>(this->GetItem(index));
}
