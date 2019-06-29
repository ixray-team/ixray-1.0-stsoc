#include "stdafx.h"
#include <dinput.h>
#include "UICHTMenu.h"
#include "HUDManager.h"
#include "UICustomMenu.h"
#include "level.h"

#define BUY_MENU_OFFS_ROW	200
#define BUY_MENU_OFFS_COL	0
//--------------------------------------------------------------------
CUIChangeTeamMenu::CUIChangeTeamMenu	()
{
	menu_offs_row		= (float)HUD().ClientToScreenY(BUY_MENU_OFFS_ROW,alLeft|alTop);
	menu_offs_col		= (float)HUD().ClientToScreenX(BUY_MENU_OFFS_COL,alLeft|alTop);
}
//--------------------------------------------------------------------

void CUIChangeTeamMenu::OnFrame()
{
	if (menu_active){
		CGameFont* F	= HUD().pFontMedium;
		F->SetColor		(0xFFFFFFFF);
		if (menu_active->Parent()){
			F->OutSet	(menu_offs_col,menu_offs_row);
			F->OutNext	("$ Cost");
		}
		if (menu_active->title){
			F->OutSet	(menu_offs_col,menu_offs_row);
			F->OutNext	(menu_active->title);
		}
		F->OutSet	(menu_offs_col,menu_offs_row);
		F->OutSkip	(1.5f);
		int k=1;
		for (MIIt it=menu_active->FirstItem(); menu_active->LastItem() != it; ++it,++k)
			(*it)->DrawItem(F,k,0);
		F->OutSkip(0.5f);
		F->OutNext("%-2d %-20s",0,"Exit");
	}
}
//--------------------------------------------------------------------

void CUIChangeTeamMenu::Render	()
{
}
//--------------------------------------------------------------------
bool CUIChangeTeamMenu::IR_OnKeyboardPress(int dik)
{
	int id = -1;
	switch (dik){
	case DIK_0:	Hide();	break;
	case DIK_1: id=1;	break;
	case DIK_2: id=2;	break;
	case DIK_3: id=3;	break;
	case DIK_4: id=4;	break;
	case DIK_5: id=5;	break;
	case DIK_6: id=6;	break;
	case DIK_7: id=7;	break;
	case DIK_8: id=8;	break;
	case DIK_9: id=9;	break;
	}
	if (menu_active&&(id>0)){
		CUICustomMenuItem* menu	= menu_active->GetItem(id);
		if (menu){
			if (menu->HasChildren())menu_active = menu;
			else { menu->Execute(); Hide(); }
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
