#include "stdafx.h"
#include "uiweapon.h"
#include "hudmanager.h"
#include "weapon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
CUIWeapon::CUIWeapon()
{
	cur_shader	= 0;
}
//--------------------------------------------------------------------

void CUIWeapon::Init()
{
	HUD().ClientToScreen(position, 694, 525, alRight|alBottom);
	// back frame with tex
	back.Init		("ui\\hud_weapon_back","font",694,525,alRight|alBottom);
	back.SetRect	(0,0,90,61);
	// weapon frame
	weapon.SetPos	(697,540);
	weapon.SetAlign	(alRight|alBottom);
	weapon.SetRect	(0,0,90,32);
}
//--------------------------------------------------------------------

CUIWeapon::~CUIWeapon()
{
}
//--------------------------------------------------------------------
void CUIWeapon::Render()
{
	back.Render		();
	if (cur_shader) 
		weapon.Render(cur_shader);
}
//--------------------------------------------------------------------
void CUIWeapon::Out(CWeapon* W)
{
	if (W){
		CGameFont* H= HUD().pFontSmall;
		H->SetColor	(0x80ffffff);
		H->Out		(float(position.x+6),	float(position.y+15),"%s",W->GetName());
		int	AE		= W->GetAmmoElapsed();
		int	AC		= W->GetAmmoCurrent();
		//int	AC		= W->GetAmmoMagSize();
		if((AE>=0)&&(AC>0))
			H->Out	(float(position.x+3),	float(position.y+2),"%d/%d %s",AE,AC, *W->m_ammoName?*W->m_ammoName:"");
		cur_shader	= W->GetUIIcon();
	}else{
		cur_shader	= 0;
	}
}
