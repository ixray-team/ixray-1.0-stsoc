#include "stdafx.h"

#include "../xr_ioconsole.h"

#include "uiinventory.h"
#include "inventory.h"
#include "actor.h"
#include "HUDManager.h"


#include "weapon.h"

#define INV_OFFS_ROW	20
#define INV_SKIP_ROW	35
#define INV_OFFS_COL1	50
#define INV_OFFS_COL2	200
#define INV_OFFS_COL3	400

static
u32 DILetters[] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z };
static
xr_map<u32, char> gs_DIK2CHR;

CUIInventory::CUIInventory(void) {
	m_active = m_shift = m_deleteNext = false;
	CHUDManager* l_pHUD	= /*(CHUDManager*)Level().*/&HUD();
	m_rowOffs = (float)l_pHUD->ClientToScreenY(INV_OFFS_ROW, alLeft|alTop);
	m_rowSkip = (float)l_pHUD->ClientToScreenY(INV_SKIP_ROW, alLeft|alTop);
	m_colOffs[0] = (float)l_pHUD->ClientToScreenX(INV_OFFS_COL1, alLeft|alTop);
	m_colOffs[1] = (float)l_pHUD->ClientToScreenX(INV_OFFS_COL2, alLeft|alTop);
	m_colOffs[2] = (float)l_pHUD->ClientToScreenX(INV_OFFS_COL3, alLeft|alTop);
	for(char l_c = 'a'; l_c <= 'z'; ++l_c) gs_DIK2CHR[DILetters[l_c-'a']] = l_c;



	//by Dandy
	//init inventory window
	UIInventoryWnd.Init();

	m_pUICursor = HUD().GetUI()->GetCursor();
	m_pUI = HUD().GetUI();
}

CUIInventory::~CUIInventory(void) {
}

void CUIInventory::OnFrame() {
	if(m_active)
	{
/*
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		if(l_pA) {
			CGameFont* l_pF = HUD().pFontMedium;
			l_pF->SetColor(0xffffffff);
			CInventory &l_inv = l_pA->m_inventory;
			for(u32 i = 0; i < l_inv.m_slots.size(); ++i) {
				l_pF->OutSet(m_colOffs[1], m_rowOffs + m_rowSkip * i);
				l_pF->OutNext("%d. %s", i+1, l_inv.m_slots[i].m_name);
				if(l_inv.m_slots[i].m_pIItem) {
					l_pF->OutNext("          %s", l_inv.m_slots[i].m_pIItem->NameComplex());
				}
			}
			l_pF->OutSet(m_colOffs[0], m_rowOffs + m_rowSkip * l_inv.m_slots.size());
			l_pF->OutNext("Belt: %d/%d", l_inv.m_belt.size(), l_inv.m_maxBelt);
			char l_l = 'a';
			char l_name[4][255];
			TIItemList l_tmpSet; l_tmpSet.insert(l_tmpSet.end(), l_inv.m_belt.begin(), l_inv.m_belt.end());
			for(PPIItem l_it = l_tmpSet.begin(); l_tmpSet.end() != l_it;) {
				for(u32 i = 0; i < 4; ++i) l_name[i][0] = 0;
				for(u32 i = 0; i < 4; ++i) {
					PIItem l_pItem = *l_it;
					l_name[i][0] = l_l++; l_name[i][1] = '.'; l_name[i][2] = ' '; strcpy(&l_name[i][3], l_pItem->Name());
					u32 l_cnt = 1;
					for(PPIItem l_it2 = l_it; true;) {
						PPIItem l_it3 = l_it2; ++l_it2; if(l_it2 == l_tmpSet.end()) break;
						if(!strcmp((*l_it2)->Name(), &l_name[i][3])) { ++l_cnt; l_tmpSet.erase(l_it2); l_it2 = l_it3; }
					}
					if(l_cnt>1) {
						u32 l_len = (u32)xr_strlen(l_name[i]);
						l_name[i][l_len] = ' '; l_name[i][l_len+1] = 'x';
						if(l_cnt>=10) { l_name[i][l_len+2] = '0'+u8(l_cnt/10); l_name[i][l_len+3] = '0'+u8(l_cnt%10); l_name[i][l_len+4] = 0; }
						else { l_name[i][l_len+2] = '0'+u8(l_cnt%10); l_name[i][l_len+3] = 0; }
					}
					++l_it; if(l_it == l_tmpSet.end()) break;
				}
				l_pF->OutNext("          %-30s %-30s %-30s %-30s", l_name[0], l_name[1], l_name[2], l_name[3]);
			}
			l_pF->OutSet(m_colOffs[0], m_rowOffs + m_rowSkip * 8.5f);
			l_pF->OutNext("______________________________________________________________________________________");
			l_pF->OutNext("Bag: %d/%d  %.1fkg/%.0fkg", l_inv.m_ruck.size(), l_inv.m_maxRuck ,l_inv.TotalWeight(), l_inv.m_maxWeight);
			l_l = 'a'; l_tmpSet.clear(); l_tmpSet.insert(l_tmpSet.end(), l_inv.m_ruck.begin(), l_inv.m_ruck.end());
			for(PPIItem l_it = l_tmpSet.begin(); l_tmpSet.end() != l_it;) {
				for(u32 i = 0; i < 4; ++i) l_name[i][0] = 0;
				for(u32 i = 0; i < 4; ++i) {
					PIItem l_pItem = *l_it;
					l_name[i][0] = l_l++; l_name[i][1] = '.'; l_name[i][2] = ' '; strcpy(&l_name[i][3], l_pItem->NameComplex());
					u32 l_cnt = 1;
					for(PPIItem l_it2 = l_it; true;) {
						PPIItem l_it3 = l_it2; ++l_it2; if(l_it2 == l_tmpSet.end()) break;
						if(!strcmp((*l_it2)->NameComplex(), &l_name[i][3])) { ++l_cnt; l_tmpSet.erase(l_it2); l_it2 = l_it3; }
					}
					if(l_cnt>1) {
						u32 l_len = (u32)xr_strlen(l_name[i]);
						l_name[i][l_len] = ' '; l_name[i][l_len+1] = 'x';
						if(l_cnt>=10) { l_name[i][l_len+2] = '0'+u8(l_cnt/10); l_name[i][l_len+3] = '0'+u8(l_cnt%10); l_name[i][l_len+4] = 0; }
						else { l_name[i][l_len+2] = '0'+u8(l_cnt%10); l_name[i][l_len+3] = 0; }
					}
					++l_it; if(l_it == l_tmpSet.end()) break;
				}
				l_pF->OutNext("          %-30s %-30s %-30s %-30s", l_name[0], l_name[1], l_name[2], l_name[3]);
			}
			l_pF->OutSet(m_colOffs[0], m_rowOffs + m_rowSkip * 11);
			l_pF->OutNext("0. Exit");
		}

//*/

		//by Dandy
		//draw the windows text
		UIInventoryWnd.SetFont(HUD().pFontMedium);
		UIInventoryWnd.Update();
	}
}

bool CUIInventory::IR_OnKeyboardPress(int dik) 
{
	if(!m_active) return false;
	
//	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	CInventoryOwner *l_pA = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(l_pA)
	{
/*	
		CInventory &l_inv = l_pA->m_inventory;
		if(dik >= DIK_1 && dik <= DIK_9) 
		{
			if(m_deleteNext) 
			{
				m_deleteNext = false;
				if(!l_inv.m_slots[dik-DIK_1].m_pIItem) return true;
				l_inv.m_slots[dik-DIK_1].m_pIItem->Drop();
				//NET_Packet P;
				//l_pA->u_EventGen(P,GE_OWNERSHIP_REJECT,l_pA->ID());
				//P.w_u16(u16(l_inv.m_slots[dik-DIK_1].m_pIItem->ID()));
				//l_pA->u_EventSend(P);
			} 
			else if(dik-DIK_1 < (int)l_inv.m_slots.size()) 
					l_inv.Ruck(l_inv.m_slots[dik-DIK_1].m_pIItem);
			return true;
		} else if(gs_DIK2CHR[dik] >= 'a' && gs_DIK2CHR[dik] <= 'z') {
			char l_c = 'a'; TIItemList l_tmpSet;
			if(m_shift) l_tmpSet.insert(l_tmpSet.end(), l_inv.m_belt.begin(), l_inv.m_belt.end());
			else l_tmpSet.insert(l_tmpSet.end(), l_inv.m_ruck.begin(), l_inv.m_ruck.end());
			for(PPIItem l_it = l_tmpSet.begin(); l_tmpSet.end() != l_it; ++l_it, ++l_c) {
				PIItem l_pItem = *l_it;
				if(gs_DIK2CHR[dik] == l_c) {
					if(m_deleteNext) {
						m_deleteNext = false;
						l_pItem->Drop();
						//NET_Packet P;
						//l_pA->u_EventGen(P,GE_OWNERSHIP_REJECT,l_pA->ID());
						//P.w_u16(u16(l_pItem->ID()));
						//l_pA->u_EventSend(P);
					} else {
						if(m_shift) l_inv.Ruck(l_pItem);
						else if(l_inv.Slot(l_pItem)) {
							if(l_pItem->m_slot < 0xffffffff) l_inv.Activate(l_pItem->m_slot);
							CWeapon *l_pW = dynamic_cast<CWeapon*>(l_pItem);
							if(l_pW) l_pW->SpawnAmmo();
						} else l_inv.Belt(l_pItem);
					}
					return true;
				}
				for(PPIItem l_it2 = l_it; true;) {
					PPIItem l_it3 = l_it2; ++l_it2; if(l_it2 == l_tmpSet.end()) break;
					if(!strcmp((*l_it2)->NameComplex(), l_pItem->NameComplex())) { l_tmpSet.erase(l_it2); l_it2 = l_it3; }
				}
			}
			return true;
		} else
//				*/
		switch(dik) 
		{
			/*case DIK_I : 
                //Exit from menu
				Hide();
				m_pUICursor->Hide();
				m_pUI->ShowIndicators();
				if(m_bCrosshair)
					Console->Execute("hud_crosshair 1");
				return true;*/

			case DIK_RSHIFT :
			case DIK_LSHIFT :
				m_shift = true;
				return true;
		}
	}

	//mouse click
	if(dik==MOUSE_1)
	{
		UIInventoryWnd.OnMouse(m_pUICursor->GetPos().x,m_pUICursor->GetPos().y,
								CUIWindow::LBUTTON_DOWN);
		return true;
	}
	else if(dik==MOUSE_2)
	{
			UIInventoryWnd.OnMouse(m_pUICursor->GetPos().x,m_pUICursor->GetPos().y,
									CUIWindow::RBUTTON_DOWN);
		return true;
	}

	return false;
}

bool CUIInventory::IR_OnKeyboardRelease(int dik) 
{
	if(!m_active) return false;
/*
		CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
		if(l_pA) {
			//CInventory &l_inv = l_pA->m_inventory;
			if(dik >= DIK_1 && dik <= DIK_9) {
				return true;
			} else if(gs_DIK2CHR[dik] >= 'a' && gs_DIK2CHR[dik] <= 'z') {
				return true;
			} else switch(dik) {
				case DIK_RSHIFT :
				case DIK_LSHIFT : {
					m_shift = false;
				} return true;
				case DIK_DELETE : {
					m_deleteNext = !m_deleteNext;
				} return true;
			}
		}
//		*/

	//mouse click
	if(dik==MOUSE_1)
	{
		UIInventoryWnd.OnMouse(m_pUICursor->GetPos().x,m_pUICursor->GetPos().y,
			CUIWindow::LBUTTON_UP);
		return true;
	}
	else if(dik==MOUSE_2)
	{
			UIInventoryWnd.OnMouse(m_pUICursor->GetPos().x,m_pUICursor->GetPos().y,
									CUIWindow::RBUTTON_UP);
		return true;
	}

	return false;
}

bool CUIInventory::IR_OnMouseMove(int dx, int dy)
{
	if(!m_active) return false;
	
	if (m_pUICursor->IsVisible())
	{ 
		m_pUICursor->MoveBy(dx, dy);
		UIInventoryWnd.OnMouse(m_pUICursor->GetPos().x, m_pUICursor->GetPos().y, 
								CUIWindow::MOUSE_MOVE);
	}

	return true;
}


void CUIInventory::Render()
{
	if(!m_active) return;

	UIInventoryWnd.Draw();
}


void CUIInventory::Init()
{
	if(!m_active) return;

//	CActor *l_pA = dynamic_cast<CActor*>(Level().CurrentEntity());
	CInventoryOwner *l_pA = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());

	if(l_pA) 
	{
	//UIInventoryWnd.InitInventory(&l_pA->m_inventory);
		UIInventoryWnd.InitInventory();
	}


}
void CUIInventory::Show() 
{ 
	m_active = true; 
	UIInventoryWnd.Show(); 

	//remove crosshair if it was shown
/*	if(Console->GetBool("hud_crosshair"))
		m_bCrosshair = true;
	else
		m_bCrosshair = false;
*/
	m_bCrosshair = true;

	if(m_bCrosshair)
		Console->Execute("hud_crosshair 0");

}
void CUIInventory::Hide() 
{ 
	m_active = false;
	UIInventoryWnd.Hide(); 

	if(m_bCrosshair)
			Console->Execute("hud_crosshair 1");

}