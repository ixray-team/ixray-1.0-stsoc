// UIWpnDragDropItem.cpp: специальный класс перетаскиваемой картинки
//						  переделанный для оружия с апгрейцдами
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIWpnDragDropItem.h"
#include "UIInventoryUtilities.h"
#include "../HUDManager.h"
#include "UIDragDropList.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIWpnDragDropItem:: CUIWpnDragDropItem()
{
	m_pWeapon = NULL;
}

CUIWpnDragDropItem::~ CUIWpnDragDropItem()
{
}


void CUIWpnDragDropItem::Draw()
{
/*	float bk_w = GetWidth();
	if(m_pWeapon && m_pWeapon->IsSilencerAttached()){
		
		float d = cur_scale_x*iSilencerGridWidth*INV_GRID_WIDTH;
		SetWidth	(bk_w-d);

		Frect out_rect;

		out_rect.top =   0;
		out_rect.bottom = GetHeight();
		out_rect.left =  0;
		out_rect.right = GetWidth();

		m_UIStaticItem.SetRect(out_rect);
	}
*/
	inherited::Draw();
	
//	SetWidth(bk_w);

	if(!m_pWeapon)	return;

	Frect rect = GetAbsoluteRect();

	if (m_bInFloat) 
		UI()->PushScissor(UI()->ScreenRect(),true);


	if(m_pWeapon->ScopeAttachable() && m_pWeapon->IsScopeAttached())
	{
		int scope_x = m_pWeapon->GetScopeX();
		int scope_y = m_pWeapon->GetScopeY();

		int w = iFloor(0.5f+(float)scope_x*cur_scale_x);
		int h = iFloor(0.5f+(float)scope_y*cur_scale_y);

		m_UIStaticScope.SetPos(	rect.left + w,
								rect.top + h);
		m_UIStaticScope.SetRect(	0.0f, 
									0.0f, 
									(cur_scale_x*iScopeGridWidth*INV_GRID_WIDTH), 
									(cur_scale_y*iScopeGridHeight*INV_GRID_HEIGHT));

		m_UIStaticScope.Render();
	}
	if(m_pWeapon->SilencerAttachable() && m_pWeapon->IsSilencerAttached())
	{

		int silencer_x = m_pWeapon->GetSilencerX();
		int silencer_y = m_pWeapon->GetSilencerY();

		int w = iFloor(0.5f+(float)silencer_x*cur_scale_x);
		int h = iFloor(0.5f+(float)silencer_y*cur_scale_y);
		
		m_UIStaticSilencer.SetPos(	rect.left + w,
									rect.top  + h);

		m_UIStaticSilencer.SetRect(	0.0f, 
									0.0f, 
									(cur_scale_x*iSilencerGridWidth*INV_GRID_WIDTH), 
									(cur_scale_y*iSilencerGridHeight*INV_GRID_HEIGHT));

		m_UIStaticSilencer.Render();
	}
	if(m_pWeapon->GrenadeLauncherAttachable() && m_pWeapon->IsGrenadeLauncherAttached())
	{
		int launcher_x = m_pWeapon->GetGrenadeLauncherX();
		int launcher_y = m_pWeapon->GetGrenadeLauncherY();

		int w = iFloor(0.5f+(float)launcher_x*cur_scale_x);
		int h = iFloor(0.5f+(float)launcher_y*cur_scale_y);


		m_UIStaticGrenadeLauncher.SetPos(	rect.left + w,
											rect.top + h);

		m_UIStaticGrenadeLauncher.SetRect(	
									0.0f, 
									0.0f, 
									(cur_scale_x*iLauncherGridWidth*INV_GRID_WIDTH), 
									(cur_scale_y*iLauncherGridHeight*INV_GRID_HEIGHT));

		m_UIStaticGrenadeLauncher.Render();

	}

	if (m_bInFloat) 
		UI()->PopScissor();

}

void CUIWpnDragDropItem::SetData(void* pData)
{
	inherited::SetData(pData);

	CInventoryItem* pItem = (CInventoryItem*)pData;
	m_pWeapon = smart_cast<CWeapon*>(pItem);

	if(m_pWeapon)
	{
		if(m_pWeapon->GrenadeLauncherAttachable())
		{
			m_UIStaticGrenadeLauncher.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetGrenadeLauncherName();
			iLauncherGridWidth = pSettings->r_u32(section, "inv_grid_width");
			iLauncherGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticGrenadeLauncher.SetOriginalRect(
				float(iXPos*INV_GRID_WIDTH), float(iYPos*INV_GRID_HEIGHT),
				float(iLauncherGridWidth*INV_GRID_WIDTH), float(iLauncherGridHeight*INV_GRID_HEIGHT));
		}
		if(m_pWeapon->ScopeAttachable())
		{
			m_UIStaticScope.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetScopeName();
			iScopeGridWidth = pSettings->r_u32(section, "inv_grid_width");
			iScopeGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticScope.SetOriginalRect(
				float(iXPos*INV_GRID_WIDTH), float(iYPos*INV_GRID_HEIGHT),
				float(iScopeGridWidth*INV_GRID_WIDTH), float(iScopeGridHeight*INV_GRID_HEIGHT));
			
		}
		if(m_pWeapon->SilencerAttachable())
		{
			m_UIStaticSilencer.SetShader(InventoryUtilities::GetEquipmentIconsShader());

			const char* section  = *m_pWeapon->GetSilencerName();
			 iSilencerGridWidth = pSettings->r_u32(section, "inv_grid_width");
			 iSilencerGridHeight = pSettings->r_u32(section, "inv_grid_height");
			u32 iXPos = pSettings->r_u32(section, "inv_grid_x");
			u32 iYPos = pSettings->r_u32(section, "inv_grid_y");

			m_UIStaticSilencer.SetOriginalRect(
				float(iXPos*INV_GRID_WIDTH), float(iYPos*INV_GRID_HEIGHT),
				float(iSilencerGridWidth*INV_GRID_WIDTH), float(iSilencerGridHeight*INV_GRID_HEIGHT));


		}
	}
}

void CUIWpnDragDropItem::ClipperOff()
{
	inherited::ClipperOff();

	if(!m_pWeapon) return;


	if(m_pWeapon->IsScopeAttached())inherited::ClipperOff(m_UIStaticScope);
	if(m_pWeapon->IsSilencerAttached()) inherited::ClipperOff(m_UIStaticSilencer);
	if(m_pWeapon->IsGrenadeLauncherAttached()) inherited::ClipperOff(m_UIStaticGrenadeLauncher);
}

void CUIWpnDragDropItem::ClipperOn()
{
	inherited::ClipperOn();

	if(!m_pWeapon) return;

	if(m_pWeapon->IsScopeAttached()) this->TextureClipper(0,0,NULL, m_UIStaticScope);
	if(m_pWeapon->IsSilencerAttached())  this->TextureClipper(0,0,NULL, m_UIStaticSilencer);
	if(m_pWeapon->IsGrenadeLauncherAttached())  this->TextureClipper(0,0,NULL, m_UIStaticGrenadeLauncher);

}
/*
///.
void CUIWpnDragDropItem::SetTextureScaleXY(float x, float y)
{
	inherited::SetTextureScaleXY(x,y);

	if(!m_pWeapon) return;

	m_UIStaticGrenadeLauncher.SetScaleXY(x, y);
	m_UIStaticScope.SetScaleXY(x, y);
	m_UIStaticSilencer.SetScaleXY(x, y);
}
*/
void CUIWpnDragDropItem::UpdateGridWidth(bool b)
{
	if(m_pWeapon){
		int old_width = GetGridWidth();
		int new_width = 0;

		if(m_pWeapon->SilencerAttachable() && m_pWeapon->IsSilencerAttached())
			new_width = m_pWeapon->GetGridWidth();//+1;
		else
			new_width = m_pWeapon->GetGridWidth();

		if(old_width!=new_width){
			SetGridWidth(new_width);
			if(b)
				Rescale(cur_scale_x, cur_scale_y);
		}

	}

}

void CUIWpnDragDropItem::Update()
{
	UpdateGridWidth(true);
	inherited::Update();
}

void CUIWpnDragDropItem::Rescale(float scale_x, float scale_y)
{
	UpdateGridWidth(false);
	inherited::Rescale(scale_x, scale_y);
}
