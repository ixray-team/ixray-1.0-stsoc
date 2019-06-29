// UIDragDropItem.h: класс перетаскиваемой картинки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIDragDropItem.h"
#include "../HUDManager.h"
#include "uidragdroplist.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CUIDragDropList;

CUIDragDropItem:: CUIDragDropItem()
{
	m_eButtonState = BUTTON_NORMAL;
	m_ePressMode = NORMAL_PRESS;

	//m_str = "";

	m_bButtonClicked = false;
	m_bCursorOverWindow = false;


	m_bDDEnabled = true;


	m_iGridWidth = 0;
	m_iGridHeight = 0;

	m_iGridCol = 0;
	m_iGridRow = 0;

	m_pData = NULL;
	m_pCustomDrawProc = NULL;

	m_bClipper = false;

	m_bInFloat = false;

	m_bMoveOnNextDrop = false;

	m_previousPos.x = 0;
	m_previousPos.y = 0;

	m_bNeedOldMousePosRecalc = false;
}

CUIDragDropItem::~ CUIDragDropItem()
{

}

void CUIDragDropItem::Init(LPCSTR tex_name, float x, float y, float width, float height)
{
	m_pParentWnd = NULL;
	m_pMouseCapturer = NULL;

	m_pData = NULL;
	m_pCustomDrawProc = NULL;

	m_iOldMouseX = x;
	m_iOldMouseY = y;

	m_previousPos.x = x;
	m_previousPos.y = y;

	CUIButton::Init		(tex_name, x, y, width, height);

	ClipperOn			();
	SetStretchTexture	(true);
}

bool  CUIDragDropItem::OnMouse(float x, float y, EUIMessages mouse_action)
{
	float deltaX = 0;
	float deltaY = 0;


	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_button = true;
	}
	else
	{
		cursor_on_button = false;
	}

	
	m_bCursorOverWindow = cursor_on_button;


	m_bButtonClicked = false;


	if(mouse_action == WINDOW_MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
			GetParent()->SetCapture(this, cursor_on_button);

	if(mouse_action == WINDOW_LBUTTON_DB_CLICK && m_bCursorOverWindow)
	{
		GetTop()->SendMessage(this, DRAG_DROP_ITEM_DB_CLICK);
		GetParent()->SetCapture(this, false);
	}
	else if(mouse_action == WINDOW_RBUTTON_DOWN && m_bCursorOverWindow)
	{
		GetTop()->SendMessage(this, DRAG_DROP_ITEM_RBUTTON_CLICK);
	}


	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == WINDOW_LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
			
			GetTop()->SendMessage(this, DRAG_DROP_ITEM_DRAG);
			GetParent()->SetCapture(this, true);

			ClipperOff();
			m_bNeedOldMousePosRecalc = true;

			m_previousPos.x = GetWndRect().left;
			m_previousPos.y = GetWndRect().top;
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == WINDOW_LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				GetTop()->SendMessage(this, DRAG_DROP_ITEM_DROP);
				
				m_bButtonClicked = true;
				ClipperOn();
			}
		
			m_eButtonState = BUTTON_NORMAL;
			
			//освободить мышь
			GetParent()->SetCapture(this, false);
		}
		else if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(m_bDDEnabled) 
			{
				deltaX = x - m_iOldMouseX;
				deltaY = y - m_iOldMouseY;
				if (m_bNeedOldMousePosRecalc)
				{
					deltaX = 0;
					deltaY = 0;
					m_bNeedOldMousePosRecalc = false;
				}
				
				SetWndPos(GetWndRect().left+deltaX,  GetWndRect().top+deltaY);

				GetTop()->SendMessage(this, DRAG_DROP_ITEM_MOVE);
			}
		}
	}
	else if(m_eButtonState == BUTTON_UP)
	{
		if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(cursor_on_button)
				m_eButtonState = BUTTON_PUSHED;
		}
		else if(mouse_action == WINDOW_LBUTTON_UP)
		{
			m_eButtonState = BUTTON_NORMAL;
			GetParent()->SetCapture(this, false);


			GetTop()->SendMessage(this, DRAG_DROP_ITEM_DROP);
			ClipperOn();
		}
	}


	//запомнить положение мыши, с учетом
	//возможного перемещения картинки
	m_iOldMouseX = x - deltaX;
	m_iOldMouseY = y - deltaY;

	return true;
}



void CUIDragDropItem::Draw()
{
	Frect rect = GetAbsoluteRect();
	m_UIStaticItem.SetPos(rect.left , rect.top);

	if (m_bInFloat) 
		UI()->PushScissor(UI()->ScreenRect(),true);

	m_UIStaticItem.Render();
	//
	if(m_pCustomDrawProc) (*m_pCustomDrawProc)(this);

	if (m_bInFloat) 
		UI()->PopScissor();
}

void CUIDragDropItem::Update()
{
	inherited::Update();
//вызвать дополнительную функцию обновления
//	if(m_pCustomUpdateProc) (*m_pCustomUpdateProc)(this);
}
//////////////////////////////////////////////////////////////////////////

void CUIDragDropItem::Rescale(float scale_x, float scale_y)
{
	SetWidth	(GetGridWidth() * scale_x * INV_GRID_WIDTH);
	SetHeight	(GetGridHeight() * scale_y * INV_GRID_HEIGHT);
	cur_scale_x = scale_x;
	cur_scale_y = scale_y;
}
//////////////////////////////////////////////////////////////////////////

void CUIDragDropItem::Highlight(bool on)
{
	CUIDragDropList *pDDList = smart_cast<CUIDragDropList*>(GetParent());
	if (!pDDList || !IsShown()) return;

	int place_row = GetGridRow();
	int place_col = GetGridCol();

	for(int k = 0; k < GetGridHeight(); ++k)
	{
		for(int m = 0; m < GetGridWidth(); ++m)
		{
			pDDList->HighlightCell(k + place_row - pDDList->GetCurrentFirstRow(), m + place_col, on);
		}
	}
}
