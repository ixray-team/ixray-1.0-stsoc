//////////////////////////////////////////////////////////////////////
// UIListWnd.cpp: окно со списком
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"
#include ".\uilistwnd.h"
//#include "UIInteractiveListItem.h"
#include "uiscrollbar.h"

//////////////////////////////////////////////////////////////////////////

#define				ACTIVE_BACKGROUND			"ui\\ui_pop_up_active_back"
#define				ACTIVE_BACKGROUND_WIDTH		16
#define				ACTIVE_BACKGROUND_HEIGHT	16

// разделитель для интерактивных строк в листе
static const char	cSeparatorChar				= '%';

//////////////////////////////////////////////////////////////////////////

CUIListWnd::CUIListWnd()
{
	m_bActiveBackgroundEnable	= false;
	m_bListActivity				= true;
	m_iFocusedItem				= -1;
	m_iSelectedItem             = -1;
	m_iFocusedItemGroupID		= -1;
	m_iSelectedItemGroupID      = -1;
	m_bShowSelectedItem			= false;
	m_dwFontColor				= 0xFFFFFFFF;
	SetItemHeight				(DEFAULT_ITEM_HEIGHT);
	m_bVertFlip					= false;
	m_bUpdateMouseMove			= false;
	m_bForceFocusedItem			= false;
	m_iLastUniqueID				= 0;
	m_bNewRenderMethod			= false;
	m_bAlwaysShowScroll			= false;
	m_bAlwaysShowScroll_enable	= false;
	
}

//////////////////////////////////////////////////////////////////////////

CUIListWnd::~CUIListWnd()
{
	//очистить список и удалить все элементы
//	for(LIST_ITEM_LIST_it it=m_ItemList.begin(); m_ItemList.end() != it; ++it)
//		DetachChild(*it);

	while(!m_ItemList.empty())
		DetachChild(m_ItemList.front());

	m_ItemList.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::Init(float x, float y, float width, float height)
{
	Init(x, y, width, height, m_iItemHeight);
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::Init(float x, float y, float width, float height, float item_height)
{
	CUIWindow::Init(x, y, width, height);

	//добавить полосу прокрутки
	m_ScrollBar = xr_new<CUIScrollBar>(); m_ScrollBar->SetAutoDelete(true);
	AttachChild(m_ScrollBar);

	if (!!m_scrollbar_profile)
		m_ScrollBar->Init(width,0,height, false, *m_scrollbar_profile);
	else
		m_ScrollBar->Init(width,0,height, false);


	m_ScrollBar->SetWndPos(m_ScrollBar->GetWndPos().x - m_ScrollBar->GetWidth(), m_ScrollBar->GetWndPos().y);

	SetItemWidth(width - m_ScrollBar->GetWidth());
	
	m_iFirstShownIndex = 0;

	SetItemHeight(item_height);
	m_iRowNum = iFloor(height/m_iItemHeight);


	

	

	m_ScrollBar->SetRange(0,0);
	m_ScrollBar->SetPageSize(s16(0));
	m_ScrollBar->SetScrollPos(s16(m_iFirstShownIndex));

	m_ScrollBar->Show(false);
	m_ScrollBar->Enable(false);


	m_StaticActiveBackground.Init(ACTIVE_BACKGROUND,"hud\\default", 0,0,alNone);
	m_StaticActiveBackground.SetTile(iFloor(m_iItemWidth/ACTIVE_BACKGROUND_WIDTH), 
									 iFloor(m_iItemHeight/ACTIVE_BACKGROUND_HEIGHT),
									 fmod(m_iItemWidth,float(ACTIVE_BACKGROUND_WIDTH)), 
									 fmod(m_iItemHeight,float(ACTIVE_BACKGROUND_HEIGHT)));

	UpdateList();
}
//////////////////////////////////////////////////////////////////////////

/*was made within plan of dinamic changing of appea*/
void CUIListWnd::SetHeight(float height){
	CUIWindow::SetHeight(height);
	m_iRowNum = iFloor(height/m_iItemHeight);
	m_ScrollBar->SetHeight(height);
	this->UpdateList();
	this->UpdateScrollBar();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::SetWidth(float width)
{
	inherited::SetWidth(width);
	m_StaticActiveBackground.SetTile(iFloor(GetWidth()/ACTIVE_BACKGROUND_WIDTH), 
									 iFloor(m_iItemHeight/ACTIVE_BACKGROUND_HEIGHT),
									 fmod(GetWidth(),float(ACTIVE_BACKGROUND_WIDTH)), 
									 fmod(float(m_iItemHeight),float(ACTIVE_BACKGROUND_HEIGHT))
									 );
}

//////////////////////////////////////////////////////////////////////////
//добавляет элемент созданный извне
//////////////////////////////////////////////////////////////////////////

void CUIListWnd::RemoveItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return;

	LIST_ITEM_LIST_it it;

	//выбрать нужный элемент
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);
	
	DetachChild(*it);


	UpdateList();

	//обновить полосу прокрутки
	if(m_ItemList.size()>0)
		m_ScrollBar->SetRange(0,s16(m_ItemList.size()-1));
	else
		m_ScrollBar->SetRange(0,0);

	m_ScrollBar->SetPageSize(s16((u32)m_iRowNum<m_ItemList.size()?
									 m_iRowNum:m_ItemList.size()));
	m_ScrollBar->SetScrollPos(s16(m_iFirstShownIndex));
	m_ScrollBar->Refresh();

	//перенумеровать индексы заново
	i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it,i++)
	{
		(*it)->SetIndex(i);
	}

}

//////////////////////////////////////////////////////////////////////////

CUIListItem* CUIListWnd::GetItem(int index)
{
	if(index<0 || index>=(int)m_ItemList.size()) return NULL;

	LIST_ITEM_LIST_it it;

	//выбрать нужный элемент
	it = m_ItemList.begin();
	for(int i=0; i<index;++i, ++it);

	R_ASSERT(m_ItemList.end() != it);

	return (*it);
}


void CUIListWnd::DetachChild(CUIWindow* pChild)
{
	LIST_ITEM_LIST_it it = std::find(m_ItemList.begin(),m_ItemList.end(),pChild);
	if(it!=m_ItemList.end())
		m_ItemList.erase(it);

	inherited::DetachChild	(pChild);
}

void CUIListWnd::RemoveAll()
{
	if(m_ItemList.empty()) return;

	LIST_ITEM_LIST_it it;

		
	while(!m_ItemList.empty())
	{
//		it = m_ItemList.begin();
		DetachChild(m_ItemList.front());
//		m_ItemList.erase(it);
	}

	m_iFirstShownIndex = 0;
	
	
	UpdateList();
	Reset();

	//обновить полосу прокрутки
	m_ScrollBar->SetRange(0,0);
	m_ScrollBar->SetPageSize(0);
	m_ScrollBar->SetScrollPos(s16(m_iFirstShownIndex));

	UpdateScrollBar();
}

/* new_code_here
CUIListItem* CUIListWnd::GetClickedItem(){
	return m_pClickedListItem;
}
*/

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::UpdateList()
{
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	
	//спрятать все элементы до участка 
	//отображающейся в данный момент
	for(int i=0; i<_min(m_ItemList.size(),m_iFirstShownIndex);
					++i, ++it)
	{
		(*it)->Show(false);
//		(*it)->Enable(false);
	}
	   

	//показать текущий список
	for(i=m_iFirstShownIndex; 
			i<_min(m_ItemList.size(),m_iFirstShownIndex + m_iRowNum+1);
			++i, ++it)
	{
		(*it)->SetWndRect((*it)->GetWndRect().left, m_bVertFlip?GetHeight()-(i-m_iFirstShownIndex)* m_iItemHeight-m_iItemHeight:(i-m_iFirstShownIndex)* m_iItemHeight, 
							m_iItemWidth, m_iItemHeight);
		(*it)->Show(true);
		
		if(m_bListActivity) 
			(*it)->Enable(true);
		else
			(*it)->Enable(false);
	}

	--it;

	//спрятать все после
	for(u32 k=m_iFirstShownIndex + m_iRowNum; 
			k<m_ItemList.size(); ++k, ++it)
	{
		(*it)->Show(false);
//		(*it)->Enable(false);
	}


	UpdateScrollBar();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
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
		//если сообщение пришло от одного из элементов списка

/*		WINDOW_LIST_it it = std::find(m_ChildWndList.begin(), 
									  m_ChildWndList.end(), 
									  pWnd);
	
		if( m_ChildWndList.end() != it)
*/
		if( IsChild(pWnd) )
		{
			CUIListItem* pListItem2;
			CUIListItem* pListItem = smart_cast<CUIListItem*>(pWnd);
			R_ASSERT(pListItem);

			if(BUTTON_CLICKED == msg)
			{				
				for (WINDOW_LIST_it it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
				{
					pListItem2 = smart_cast<CUIListItem*>(*it);
					if (!pListItem2) 
						continue;
					if (pListItem2->GetGroupID() == -1) 
						continue;
					if (pListItem2->GetGroupID() == 
						pListItem->GetGroupID())
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
				GetMessageTarget()->SendMessage(this, LIST_ITEM_CLICKED, pListItem);
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

				/*for (it = m_ChildWndList.begin(); (it != m_ChildWndList.end()) && (m_iFocusedItem == -1); ++it)
				{
					pListItem2 = smart_cast<CUIListItem*>(*it);
					if (!pListItem2) continue;
					if (pWnd != pListItem2)
						pListItem2->OnMouse(cursor_pos.x - pListItem2->GetWndRect().left, cursor_pos.y - pListItem2->GetWndRect().top, MOUSE_MOVE);
				}*/
			}
		}
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::Draw()
{
	WINDOW_LIST_it it;

	if(m_iFocusedItem != -1 && m_bActiveBackgroundEnable)
	{
		Frect rect;
		GetAbsoluteRect(rect);
		for (it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
		{
			CUIListItem *pListItem2 = smart_cast<CUIListItem*>(*it);
			if (!pListItem2) continue;
			if (pListItem2->GetGroupID() == -1) continue;
			if ((pListItem2->GetGroupID() == m_iFocusedItemGroupID) && 
				((pListItem2->GetIndex() >= m_iFirstShownIndex) && 
				(pListItem2->GetIndex() <= m_iRowNum + m_iFirstShownIndex - 1)))
			{
				m_StaticActiveBackground.SetPos(rect.left, rect.top + 
								(pListItem2->GetIndex() - m_iFirstShownIndex)*GetItemHeight());
				m_StaticActiveBackground.Render();
			}
		}
	}
	
	if(m_iSelectedItem != -1 && m_bShowSelectedItem)
	{
		Frect rect;
		GetAbsoluteRect	(rect);
		for (it = m_ChildWndList.begin(); it != m_ChildWndList.end(); ++it)
		{
			CUIListItem *pListItem2 = smart_cast<CUIListItem*>(*it);
			if (!pListItem2) continue;
			if (pListItem2->GetGroupID() == -1) continue;
			if (pListItem2->GetIndex() == m_iSelectedItem) 
			{
				UI()->PushScissor(rect);
				m_StaticActiveBackground.SetPos(rect.left, rect.top + 
					(pListItem2->GetIndex() - m_iFirstShownIndex)*GetItemHeight());
				m_StaticActiveBackground.Render();
				UI()->PopScissor();
			}
		}
	}

	CUIWindow::Draw();
}

//////////////////////////////////////////////////////////////////////////

int CUIListWnd::GetSize()
{
	return (int)m_ItemList.size();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::SetItemWidth(float iItemWidth)
{
	m_iItemWidth = iItemWidth;
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::SetItemHeight(float iItemHeight)
{
	m_iItemHeight = iItemHeight;
	m_iRowNum = iFloor(GetHeight()/iItemHeight);
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::Reset()
{
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it)
	{
		(*it)->Reset();
	}

	ResetAll();

	inherited::Reset();
}

//////////////////////////////////////////////////////////////////////////
//находит первый элемент с заданной pData, иначе -1
//////////////////////////////////////////////////////////////////////////

int CUIListWnd::FindItem(void* pData)
{
	int i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it,++i)
	{
		if((*it)->GetData()==pData) return i;
	}
	return -1;
}

int CUIListWnd::FindItemWithValue(int iValue)
{
	int i=0;
	for(LIST_ITEM_LIST_it it=m_ItemList.begin();  m_ItemList.end() != it; ++it,++i)
	{
		if((*it)->GetValue()==iValue) return i;
	}
	return -1;
}


bool CUIListWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	switch(mouse_action){
	case WINDOW_LBUTTON_DB_CLICK:
//		mouse_action = WINDOW_LBUTTON_DOWN;
		break;
	case WINDOW_MOUSE_WHEEL_DOWN:
			m_ScrollBar->TryScrollInc	();
			return						true;
			break;
	case WINDOW_MOUSE_WHEEL_UP:
			m_ScrollBar->TryScrollDec();
			return						true;
			break;
	}

	return inherited::OnMouse(x, y, mouse_action);
}

bool CUIListWnd::OnKeyboard	(int dik, EUIMessages keyboard_action)
{
	return inherited::OnKeyboard(dik, keyboard_action);
}
//////////////////////////////////////////////////////////////////////////

int CUIListWnd::GetLongestSignWidth()
{
	int max_width = m_ItemList.front()->GetSignWidht();
	
	LIST_ITEM_LIST_it it=m_ItemList.begin();
	++it;
	for(;  m_ItemList.end() != it; ++it)
	{
		if((*it)->GetSignWidht()>max_width) max_width = (*it)->GetSignWidht();
	}

	return max_width;
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::UpdateScrollBar()
{
//	m_ScrollBar->UpdateScrollBar();
	//спрятать скорлинг, если он не нужен
	if (m_bAlwaysShowScroll_enable)
	{
		m_ScrollBar->Show(m_bAlwaysShowScroll);
		return;
	}

	if ((int)m_ItemList.size()<=m_ScrollBar->GetPageSize())
		m_ScrollBar->Show(false);
	else
		m_ScrollBar->Show(true);
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::EnableScrollBar(bool enable)
{
	m_ScrollBar->SetEnabled(enable);
	UpdateScrollBar();
}

void CUIListWnd::ActivateList(bool activity)
{
	m_bListActivity = activity;
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::ScrollToBegin()
{
	m_ScrollBar->SetScrollPos((s16)m_ScrollBar->GetMinRange());
	m_iFirstShownIndex = m_ScrollBar->GetScrollPos();
	UpdateList();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::ScrollToEnd()
{
	u32 pos = m_ScrollBar->GetMaxRange()- m_ScrollBar->GetPageSize() + 1;

	if ((int)pos > m_ScrollBar->GetMinRange())
		m_ScrollBar->SetScrollPos(pos);
	else
		m_ScrollBar->SetScrollPos(m_ScrollBar->GetMinRange());

	m_iFirstShownIndex = m_ScrollBar->GetScrollPos();
	UpdateList();
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::ScrollToPos(int position)
{
	if (IsScrollBarEnabled())
	{
		int pos = position;
		clamp(pos, m_ScrollBar->GetMinRange(), (m_ScrollBar->GetMaxRange() - m_ScrollBar->GetPageSize() + 1));
		m_ScrollBar->SetScrollPos(pos);
		m_iFirstShownIndex = m_ScrollBar->GetScrollPos();
		UpdateList();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIListWnd::Update()
{
	if(m_bUpdateMouseMove)
	{
		OnMouse(cursor_pos.x, cursor_pos.y, WINDOW_MOUSE_MOVE);
		m_bUpdateMouseMove = false;
	}

	inherited::Update();
}

void CUIListWnd::SetFocusedItem(int iNewFocusedItem)
{ 
	m_iFocusedItem = iNewFocusedItem; 
	m_bForceFocusedItem = true;
	EnableActiveBackground(true);
	if (m_iFocusedItem >= 0)
		m_iFocusedItemGroupID = GetItem(m_iFocusedItem)->GetGroupID();
}

//=============================================================================
//  Interactive element insertion
//=============================================================================

//xr_vector<int> CUIListWnd::AddInteractiveItem(const char *str2, const float shift,
//											  const u32 &MsgColor, CGameFont* pFont, int pushAfter)
//{
//	string1024	str = {0};
//	// скопируем строку для возможности записи
//	std::strcpy(str, str2);
//	// Распарсиваем строку, для определения принадлежности типа строки к обычным или
//	// интерактивным элементам
//	char				*pSep = str;
//	int					count = 0;
//	xr_vector<char *>	vSubItems;
//	xr_vector<int>		IDs;
//
//	vSubItems.clear();
//
//	// Считаем количество знаков-разделителей
//	while (pSep < str + xr_strlen(str) + 1)
//	{
//		pSep = std::find(pSep, str + xr_strlen(str) + 1, cSeparatorChar);
//		if (pSep < str + xr_strlen(str) + 1)
//		{
//			++count;
//			vSubItems.push_back(pSep - count + 1 );
//			pSep++;
//		}
//	}
//
//	// Завершить строку нулем
//	if (count != 0)
//	{
//		char *b = std::remove(str, str + xr_strlen(str) + 1, cSeparatorChar);
//		*b = '\n';
//	}
//
//	// Если нет ни одного знака-разделителя, или их количество нечетно, то считаем, что строка 
//	// не интерактивна
//	if (count % 2 == 1 || count == 0)
//	{
//		// Возвращаем NULL если строка не интерактивна.
//		// В этом случае ListWnd должен сам добавить строку обычным способом
//		CUIString A;
//		A.SetText(str);
//		AddParsedItem<CUIListItem>(A, shift, MsgColor, pFont);
//		return IDs;
//	}
//	int k = 0;
//	while (k < count / 2)
//	{
//		IDs.push_back(m_iLastUniqueID++);
//		++k;
//	}
//
//	// Если строка таки интерактивна, то конструируем интерактивную структуру - член листа
//	CUIInteractiveListItem *pILItem = xr_new<CUIInteractiveListItem>();
//	AddItem<CUIListItem>(pILItem, pushAfter);
//	pILItem->Init(str, vSubItems, IDs, GetItemHeight());
//	pILItem->SetFont(pFont);
//	pILItem->SetTextColor(MsgColor);
//	pILItem->SetMessageTarget(GetMessageTarget());
//	return IDs;
//}

//////////////////////////////////////////////////////////////////////////

int CUIListWnd::GetItemPos(CUIListItem *pItem)
{
	LIST_ITEM_LIST_it it = m_ItemList.begin();
	for (u32 i = 0; i < m_ItemList.size(); ++i)
	{
		if (*it == pItem) return i;
		++it;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////

LPSTR CUIListWnd::FindNextWord(LPSTR currPos) const
{
	if (!currPos) return NULL;
	bool delimPass	= false;
	while (0 != *currPos && (!delimPass || IsEmptyDelimiter(*currPos)))
	{
		if (IsEmptyDelimiter(*currPos))
		{
			delimPass = true;
		}
		currPos++;
	}

	return 0 == *currPos ? NULL : currPos;
}

//////////////////////////////////////////////////////////////////////////

float CUIListWnd::WordTailSize(LPCSTR currPos, CGameFont *font, int &charsCount) const
{
	VERIFY(font);
	static string256 str;
	ZeroMemory(str, 256);
	charsCount = 0;
	LPCSTR memorizedPos = currPos;
	while (currPos && 0 != *currPos && 0 == IsEmptyDelimiter(*currPos))
	{
		charsCount++;
		currPos++;
	}

	clamp(charsCount, 0, 256);

	if (currPos)
	{
		strncpy(str, memorizedPos, charsCount);
		return (font->SizeOf_(str));
	}
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////

bool CUIListWnd::IsEmptyDelimiter(const char c) const
{
	return ' ' == c;
}

bool CUIListWnd::IsScrollBarEnabled() 
{
	return m_ScrollBar->GetEnabled();
}
