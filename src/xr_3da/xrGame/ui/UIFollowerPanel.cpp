#include "stdafx.h"
#include "UIFollowerPanel.h"
#include "UIFollowerIcon.h"
#include "xrxmlparser.h"

CUIFollowerPanel::~CUIFollowerPanel()
{}

void CUIFollowerPanel::Init(CUIXml* doc, LPCSTR path, int index)
{
	R_ASSERT2		(doc->NavigateToNode(path,index),path);
	Fvector2		pos;
	pos.x			= doc->ReadAttribFlt(path, index, "x");
	pos.y			= doc->ReadAttribFlt(path, index, "y");

	inherited::SetWndPos(pos);
	
	m_vIdent		= doc->ReadAttribFlt(path, index, "v_ident");
	m_hIdent		= doc->ReadAttribFlt(path, index, "h_ident");
	m_iconSize.x	= doc->ReadAttribFlt(path, index, "icon_width");
	m_iconSize.y	= doc->ReadAttribFlt(path, index, "icon_height");
}

void CUIFollowerPanel::AddFollower(u16 id)
{
	int cnt = inherited::GetChildNum();				
	Fvector2 new_size;
	new_size.y = m_iconSize.y+m_vIdent;
	new_size.x = (cnt+1)*(m_iconSize.x+m_hIdent);
	inherited::SetWndSize(new_size);

	UIFollowerIcon* icon = xr_new<UIFollowerIcon>	();

	icon->SetAutoDelete		(true);
	Frect					icon_rect;
	icon_rect.x1			= cnt*(m_iconSize.x+m_hIdent);
	icon_rect.y1			= m_vIdent;
	icon_rect.x2			= m_iconSize.x+icon_rect.x1;
	icon_rect.y2			= m_iconSize.y+icon_rect.y1;
	icon->Init				(icon_rect, "follower_character.xml");
	AttachChild				(icon);


	icon->InitCharacter		(id);
}

void CUIFollowerPanel::RemoveFollower(u16 id)
{
	WINDOW_LIST& wl = GetChildWndList();
	WINDOW_LIST_it it;
	for(it=wl.begin();it!=wl.end();++it){
		UIFollowerIcon* i = smart_cast<UIFollowerIcon*>(*it);
		if(i->OwnerID() == id){
			DetachChild		(*it);
			break;
		}
	}
	RecalcSize				();
}

void CUIFollowerPanel::RecalcSize()
{
	int cnt = inherited::GetChildNum();				
	Fvector2 new_size;
	new_size.y = m_iconSize.y+m_vIdent;
	new_size.x = (cnt+1)*(m_iconSize.x+m_hIdent);
	inherited::SetWndSize(new_size);
	
	Fvector2		pos;
	WINDOW_LIST&	wl = GetChildWndList();
	WINDOW_LIST_it	it;
	int				i;
	for(i=0,it=wl.begin();it!=wl.end();++it,++i){
		pos.x			= i*(m_iconSize.x+m_hIdent);
		pos.y			= m_vIdent;
		(*it)->SetWndPos		(pos);
	}
}
