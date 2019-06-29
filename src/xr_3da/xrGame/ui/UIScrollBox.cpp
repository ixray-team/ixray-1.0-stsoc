//////////////////////////////////////////////////////////////////////
// UIScrollBox.h: класс перемещаемой каретки в CScrollBar
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbox.h"



//#define SCROLLBOX_HORZ	"ui\\ui_scb_scroll_box"
//#define SCROLLBOX_VERT	"ui\\ui_scb_scroll_box_v"




CUIScrollBox::CUIScrollBox(void)
{
	m_bAvailableTexture = true;
	m_bIsHorizontal = true;
	
	SetPushOffset( Fvector2().set(0.0f,0.0f));
}

CUIScrollBox::~CUIScrollBox(void)
{
}

void CUIScrollBox::SetHorizontal(){
	m_bIsHorizontal = true;
}

void CUIScrollBox::SetVertical(){
	m_bIsHorizontal = false;
}

void CUIScrollBox::Init(float x, float y, float length, float broad)
{
	CUIButton::Init(x,y,length, broad);
}

//////////////////////////////////////////////////////////////////////////


bool CUIScrollBox::OnMouse(float x, float y, EUIMessages mouse_action)
{
	float deltaX = 0;
	float deltaY = 0;

	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_button = true;
	else
		cursor_on_button = false;


	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == WINDOW_LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
		
			GetParent()->SetCapture(this, true);
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == WINDOW_LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
				m_bButtonClicked = true;
			}
		
			m_eButtonState = BUTTON_NORMAL;
		
			//освободить мышь
			GetParent()->SetCapture(this, false);

			GetMessageTarget()->SendMessage(this, SCROLLBOX_STOP);
		}
		else if(mouse_action == WINDOW_MOUSE_MOVE)
		{
//			if(!cursor_on_button)
//				m_eButtonState = BUTTON_UP;
//			else
			{
				deltaX = x - m_iOldMouseX;
				deltaY = y - m_iOldMouseY;

				if(m_bIsHorizontal)
				{
					SetWndPos(GetWndRect().left+deltaX, GetWndRect().top);
				}
				else
					SetWndPos(GetWndRect().left, GetWndRect().top+deltaY);

				GetMessageTarget()->SendMessage(this, SCROLLBOX_MOVE);
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
		}
	}


	//запомнить положение мыши, с учетом
	//возможного перемещения каретки
	m_iOldMouseX = x - deltaX;
	m_iOldMouseY = y - deltaY;

	return true;
}

void CUIScrollBox::Draw()
{
	if(m_bIsHorizontal){
		if (m_UIStaticItem.GetOriginalRect().width()){
			int tile		= iFloor(GetWidth()/m_UIStaticItem.GetOriginalRect().width());
			float rem		= GetWidth()-tile*m_UIStaticItem.GetOriginalRect().width();
			m_UIStaticItem.SetTile(tile,1,rem,0);
		}
	}else{
		if (m_UIStaticItem.GetOriginalRect().height()){
			int tile		= iFloor(GetHeight()/m_UIStaticItem.GetOriginalRect().height());
			float rem		= GetHeight()-tile*m_UIStaticItem.GetOriginalRect().height();
			m_UIStaticItem.SetTile(1,tile,0,rem);
		}
	}
	 CUIButton::Draw();
}