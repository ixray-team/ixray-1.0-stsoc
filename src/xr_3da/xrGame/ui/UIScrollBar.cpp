//////////////////////////////////////////////////////////////////////
// UIScrollBar.cpp: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiscrollbar.h"
#include "UI3tButton.h"
#include "UIScrollBox.h"
#include "UIXmlInit.h"
#include "UITextureMaster.h"

CUIScrollBar::CUIScrollBar(void)
{
	m_iMinPos		= 1;
	m_iMaxPos		= 1;
	m_iPageSize		= 0;
	m_iStepSize		= 1;
	m_iScrollPos	= 0;
	m_b_enabled		= true;
	m_DecButton		= xr_new<CUI3tButton>();		AttachChild(m_DecButton);
	m_IncButton		= xr_new<CUI3tButton>();		AttachChild(m_IncButton);
	m_ScrollBox		= xr_new<CUIScrollBox>();		AttachChild(m_ScrollBox);
	m_StaticBackground = xr_new<CUIStaticItem>();

	
	
	
}

CUIScrollBar::~CUIScrollBar(void)
{
	xr_delete(m_StaticBackground);
	xr_delete(m_DecButton);
	xr_delete(m_IncButton);
	xr_delete(m_ScrollBox);
}

void CUIScrollBar::Init(float x, float y, float length, bool bIsHorizontal, LPCSTR profile)
{
	string256 _path;
	CUIXml xml_doc;
	R_ASSERT(xml_doc.Init	(CONFIG_PATH, UI_PATH, "scroll_bar.xml"));

	float height = xml_doc.ReadAttribFlt(profile, 0, "height", 16);
    	
	m_bIsHorizontal = bIsHorizontal;
	if(m_bIsHorizontal){
		CUIWindow::Init			(x,y, length, height);

        strconcat(_path, profile, ":left_arrow");
		CUIXmlInit::Init3tButton(xml_doc, _path, 0, m_DecButton);
		m_DecButton->SetWndPos(0.0f, 0.0f);

		strconcat(_path, profile, ":right_arrow");
		CUIXmlInit::Init3tButton(xml_doc, _path, 0, m_IncButton);
		m_IncButton->SetWndPos(length - m_IncButton->GetWidth(), 0.0f);

		m_ScrollBox->SetHorizontal();

		strconcat(_path, profile, ":box");
		CUIXmlInit::InitButton(xml_doc, _path, 0, m_ScrollBox);
		m_IncButton->SetWndPos(0.0f, length/2);

//		m_DecButton->Init		(SCROLLBAR_LEFT_ARROW, 0.0f, 0.0f, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
//		m_IncButton->Init		(SCROLLBAR_RIGHT_ARROW,length-SCROLLBAR_WIDTH, 0.0f, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
//		m_ScrollBox->Init		(SCROLLBAR_WIDTH, 0.0f, length/2, SCROLLBAR_HEIGHT, m_bIsHorizontal);
		strconcat(_path, profile, ":back:texture");
		LPCSTR texture = xml_doc.Read(_path, 0, "");
		R_ASSERT(texture);
		CUITextureMaster::InitTexture(texture, m_StaticBackground);
		//m_StaticBackground->Init(texture ,"hud\\default", 0.0f, 0.0f,alNone);
		m_ScrollWorkArea		= _max(0,iFloor(GetWidth()-2*height));
	}else{
		CUIWindow::Init			(x,y, height, length);

		strconcat(_path, profile, ":up_arrow");
		CUIXmlInit::Init3tButton(xml_doc, _path, 0, m_DecButton);
		m_DecButton->SetWndPos(0.0f, 0.0f);

		strconcat(_path, profile, ":down_arrow");
 		CUIXmlInit::Init3tButton(xml_doc, _path, 0, m_IncButton);
		m_IncButton->SetWndPos(0.0f, length - height);

		m_ScrollBox->SetVertical();

		strconcat(_path, profile, ":box_v");
		CUIXmlInit::InitButton(xml_doc, _path, 0, m_ScrollBox);		
		//m_ScrollBox->SetWndPos(0.0f, length/2);

//		m_DecButton->Init		(SCROLLBAR_UP_ARROW,0.0f, 0.0f ,SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
//		m_IncButton->Init		(SCROLLBAR_DOWN_ARROW, 0.0f, length-SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, SCROLLBAR_HEIGHT);
//		m_ScrollBox->Init		(0, SCROLLBAR_HEIGHT, SCROLLBAR_WIDTH, length/2, m_bIsHorizontal);
		strconcat(_path, profile, ":back_v:texture");
		LPCSTR texture = xml_doc.Read(_path, 0, "");
		R_ASSERT(texture);

		CUITextureMaster::InitTexture(texture, m_StaticBackground);
//		m_StaticBackground->Init(texture ,"hud\\default", 0.0f, 0.0f ,alNone);
		m_ScrollWorkArea		= _max(0,iFloor(GetHeight()-2*height));
	}	

	m_ScrollBox->SetPushOffset(Fvector2().set(0.0f,0.0f) );
	UpdateScrollBar();
}


//корректировка размеров скроллера
void CUIScrollBar::SetWidth(float width)
{
	if(width<=0.0f) width = 1.0f;
	inherited::SetWidth(width);
}

void CUIScrollBar::SetHeight(float height)
{
	if(height<=0.0f) height = 1.0f;
	inherited::SetHeight(height);
	UpdateScrollBar();
}

void CUIScrollBar::SetStepSize(int step)
{
	m_iStepSize				= step;
	UpdateScrollBar			();
}


void CUIScrollBar::SetRange(int iMin, int iMax) 
{
	m_iMinPos				= iMin;  
	m_iMaxPos				= iMax;
	VERIFY					(iMax>=iMin);
	if(iMax<iMin)			iMax=iMin;
	UpdateScrollBar			();
}
void CUIScrollBar::Show(bool b)
{
	if(!m_b_enabled)return;
	inherited::Show(b);
}

void CUIScrollBar::Enable(bool b)
{
	if(!m_b_enabled)return;
	inherited::Enable(b);
}

void CUIScrollBar::UpdateScrollBar()
{
	// next line disabled because of problems with appearance after size changing in listwnd
	//	Show						( !!(0!=ScrollSize()) );

	if (IsShown()){
		//уcтановить размер и положение каретки
		if(m_iMaxPos==m_iMinPos)	m_iMaxPos++;
		float box_sz				= float(m_ScrollWorkArea)*float(m_iPageSize ? m_iPageSize : 1)/float(m_iMaxPos-m_iMinPos);
		if(m_bIsHorizontal){	
			// set width
			clamp					(box_sz,_min(GetHeight(),GetWidth() - m_IncButton->GetWidth() - m_DecButton->GetWidth()),GetWidth() - m_IncButton->GetWidth() - m_DecButton->GetWidth());
			m_ScrollBox->SetWidth	(box_sz);
			m_ScrollBox->SetHeight	(GetHeight());
			// set pos
			int pos					= PosViewFromScroll(iFloor(m_ScrollBox->GetWidth()),iFloor(GetHeight()));
			m_ScrollBox->SetWndPos	(float(pos), m_ScrollBox->GetWndRect().top);
			m_IncButton->SetWndPos	(GetWidth() - m_IncButton->GetWidth(), 0.0f);
		}else{
			// set height
			clamp					(box_sz,_min(GetWidth(),GetHeight()-m_IncButton->GetHeight() - m_DecButton->GetHeight()),GetHeight()-m_IncButton->GetHeight() - m_DecButton->GetHeight());
			m_ScrollBox->SetHeight	(box_sz);
			m_ScrollBox->SetWidth	(GetWidth());
			// set pos
			int pos				= PosViewFromScroll(iFloor(m_ScrollBox->GetHeight()),iFloor(GetWidth()));
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left, float(pos));
			m_IncButton->SetWndPos	(0.0f, GetHeight() - m_IncButton->GetHeight());
		}
	}

	ClampByViewRect();
}

bool CUIScrollBar::OnMouse(float x, float y, EUIMessages mouse_action)
{
	switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
			TryScrollInc();
			return true;
			break;
		case WINDOW_MOUSE_WHEEL_UP:
			TryScrollDec();
			return true;
			break;
	};
	return inherited::OnMouse(x, y, mouse_action);
}


void CUIScrollBar::ClampByViewRect()
{
	if(m_bIsHorizontal){
		if(m_ScrollBox->GetWndRect().left <= m_DecButton->GetWidth())
			m_ScrollBox->SetWndPos	(m_DecButton->GetWidth(), m_ScrollBox->GetWndRect().top);
		else if(m_ScrollBox->GetWndRect().right >= m_IncButton->GetWndPos().x)
			m_ScrollBox->SetWndPos	(m_IncButton->GetWndRect().left - m_ScrollBox->GetWidth(), 
									m_ScrollBox->GetWndRect().top);
	}else{
		// limit vertical position (TOP) by position of button	
		if(m_ScrollBox->GetWndRect().top <= m_DecButton->GetHeight())
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left, 
									m_DecButton->GetHeight());
		// limit vertical position (BOTTOM) by position of button
		else if(m_ScrollBox->GetWndRect().bottom >= m_IncButton->GetWndPos().y)
			m_ScrollBox->SetWndPos	(m_ScrollBox->GetWndRect().left,
									m_IncButton->GetWndPos().y - m_ScrollBox->GetHeight());
	}
}

void CUIScrollBar::SetPosScrollFromView(float view_pos, float view_size, float view_offs)
{
	int scroll_size	= ScrollSize();
	float pos			= view_pos-view_offs;
	float work_size	= m_ScrollWorkArea-view_size;
	SetScrollPosClamped	(work_size?iFloor(((pos/work_size)*(scroll_size) + m_iMinPos)):0);
}

int CUIScrollBar::PosViewFromScroll(int view_size, int view_offs)
{
	int work_size	= m_ScrollWorkArea-view_size;
	int scroll_size	= ScrollSize();
	return			scroll_size?(m_iScrollPos*work_size+scroll_size*view_offs-m_iMinPos*work_size)/scroll_size:0;
}

void CUIScrollBar::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == m_DecButton){
		if(msg == BUTTON_CLICKED){
			TryScrollDec();
		}
	}else if(pWnd == m_IncButton){
		if(msg == BUTTON_CLICKED){
			TryScrollInc();
		}
	}else if(pWnd == m_ScrollBox){
		if(msg == SCROLLBOX_MOVE){
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal){
				//горизонтальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox->GetWndPos().x,m_ScrollBox->GetWidth(),GetHeight());
				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			}else{
				//вертикальный
				ClampByViewRect		();
				SetPosScrollFromView(m_ScrollBox->GetWndPos().y,m_ScrollBox->GetHeight(),GetWidth());
				if (GetMessageTarget())
					GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
			}
		}else if(msg == SCROLLBOX_STOP){
			//вычислить новое положение прокрутки
			if(m_bIsHorizontal && GetMessageTarget())
				GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
			else
				GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
		}
	}
	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIScrollBar::TryScrollInc()
{
	if(ScrollInc())
		if(m_bIsHorizontal)
			GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
		else
			GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);
}

void CUIScrollBar::TryScrollDec()
{
	if(ScrollDec())
		if(m_bIsHorizontal)
			GetMessageTarget()->SendMessage(this, SCROLLBAR_HSCROLL);
		else
			GetMessageTarget()->SendMessage(this, SCROLLBAR_VSCROLL);

}


bool CUIScrollBar::ScrollDec()
{
	if(m_iScrollPos>m_iMinPos){
		if(m_iScrollPos>m_iStepSize)
			SetScrollPos	(m_iScrollPos-m_iStepSize);
		else
			SetScrollPos	(0);

		return true;
	}

	return false;
}


bool CUIScrollBar::ScrollInc()
{
	if(m_iScrollPos<=(m_iMaxPos-m_iPageSize)){
		SetScrollPos	(m_iScrollPos+m_iStepSize);
		return true;
	}

	return false;	
}

void CUIScrollBar::Reset()
{
	ResetAll();
	inherited::Reset();
}


void CUIScrollBar::Draw()
{
	//нарисовать фоновую подложку
	Frect rect;
	GetAbsoluteRect(rect);
	if(m_bIsHorizontal){
		if (m_StaticBackground->GetOriginalRect().width()){
			float size	= GetWidth() - m_DecButton->GetWidth() - m_IncButton->GetWidth();
			float w		= m_StaticBackground->GetOriginalRect().width();

			int tile	= iFloor(size/w);
			float rem	= size - tile*w;

			m_StaticBackground->SetTile(tile,1,rem,0.0f);
			m_StaticBackground->SetPos(rect.left + m_DecButton->GetWidth(),rect.top);
		}
	}else{
		if (m_StaticBackground->GetOriginalRect().height()){
			float size	= GetHeight()- m_IncButton->GetHeight() - m_DecButton->GetHeight();
			float h		= m_StaticBackground->GetOriginalRect().height();

			int tile	= iFloor(size/h);
			float rem	= size - tile*h;

			m_StaticBackground->SetTile(1,tile,0.0f,rem);
			m_StaticBackground->SetPos(rect.left,rect.top + m_DecButton->GetHeight());
		}
	}
	
//	m_StaticBackground->SetPos(rect.left,rect.top);
	m_StaticBackground->Render();

	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIScrollBar::Refresh()
{
	SendMessage(m_ScrollBox, SCROLLBOX_MOVE, NULL);
}

