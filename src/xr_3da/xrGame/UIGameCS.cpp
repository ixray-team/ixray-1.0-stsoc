#include "stdafx.h"
#include "UIGameCS.h"

#include "UI.h"
#include "entity_alive.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

static const int money_x_offs = 785; 
static const int money_y_offs = 485;
static const int time_x_offs = 400; 
static const int time_y_offs = 550;

//--------------------------------------------------------------------
CUIGameCS::CUIGameCS()
{
	BuyMenu.Load			("game_cs.ltx","cs_buy_menu",this,CUIGameCS::BuyItem);
	CTMenu.Load				("game_cs.ltx","cs_change_team",this);
	FragList.Init			();
	PlayerList.Init			();
	OwnBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); OwnBase.SetColor		(0x8000FF00);
	EnemyBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); EnemyBase.SetColor	(0x80FF0000);
	BuyZone.Init			("ui\\ui_cs_buyzone",	"font", 5,210,	alLeft|alTop); BuyZone.SetColor		(0x8000FF00);
	Artifact.Init			("ui\\ui_cs_artefact",	"font",	5,245,	alLeft|alTop); Artifact.SetColor	(0x80FF00A2);
	HUD().ClientToScreen(vMoneyPlace, money_x_offs, money_y_offs, alRight|alBottom);
	HUD().ClientToScreen(vTimePlace, time_x_offs, time_y_offs, alCenter);
}
//--------------------------------------------------------------------

CUIGameCS::~CUIGameCS()
{
}
//--------------------------------------------------------------------

BOOL CUIGameCS::CanBuy()
{
	game_cl_GameState::Player* P = m_cl_game->local_player;
	return (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&((Level().timeServer()-m_cl_game->start_time)<m_cl_game->buy_time);
}
//--------------------------------------------------------------------

void CUIGameCS::BuyItem(CUICustomMenuItem* sender)
{
	CUICustomMenu* M= (CUICustomMenu*)sender->Owner();
	M->Hide			();
	CUIGameCS* G	= (CUIGameCS*)M->m_Owner; R_ASSERT(G);
	if (G&&G->CanBuy()&&sender->value1){
		string64 buf;
		sprintf(buf,"%s/cost=%s",sender->value1,sender->value0);
		// buy item
		CEntityAlive* E	= dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
		if (E)		E->BuyItem(buf);
	}
}
//--------------------------------------------------------------------

LPCSTR make_time(string16 buf, DWORD sec)
{
	sprintf(buf,"%2.0d:%2.0d",(sec%3600)/60,sec%60);
	int len = (int)xr_strlen(buf);
	for (int i=0; i<len; ++i) if (buf[i]==' ') buf[i]='0';
	return buf;
}

void CUIGameCS::OnFrame()
{
	switch (m_cl_game->phase){
	case GAME_PHASE_PENDING: {
		PlayerList.OnFrame();
		if (CTMenu.Visible())		CTMenu.OnFrame	();
	} break;
	case GAME_PHASE_INPROGRESS: {
		if(!CanBuy()) BuyMenu.Hide	();
		if (BuyMenu.Visible())		BuyMenu.OnFrame	();
		if (CTMenu.Visible())		CTMenu.OnFrame	();
//		if (InventoryMenu.Visible()) InventoryMenu.OnFrame();
		if (uFlags&flShowFragList) FragList.OnFrame	();
		string16 buf;
		game_cl_GameState::Player* P = m_cl_game->local_player;
		CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
		if (m_Actor){
			HUD().pFontBigDigit->SetColor		(0xA0969678);
			HUD().pFontBigDigit->SetAligment	(CGameFont::alRight);
			HUD().pFontBigDigit->Out			((float)vMoneyPlace.x,(float)vMoneyPlace.y,"$%d",P->money_total);
			HUD().pFontBigDigit->SetAligment	(CGameFont::alCenter);
			HUD().pFontBigDigit->Out			((float)vTimePlace.x,(float)vTimePlace.y,"\x60%s",make_time(buf,(m_cl_game->timelimit-(Level().timeServer()-Game().start_time))/1000));
		}
	} break;
	case GAME_PHASE_TEAM1_SCORES: {
		HUD().pFontDI->SetAligment(CGameFont::alCenter);
		HUD().pFontDI->SetColor(0xA0969678);
		HUD().pFontDI->SetSize(0.05f);
		HUD().pFontDI->Out(0,-0.5f,"TEAM 1 SCORES");
	} break;
	case GAME_PHASE_TEAM2_SCORES: {
		HUD().pFontDI->SetAligment(CGameFont::alCenter);
		HUD().pFontDI->SetColor(0xA0969678);
		HUD().pFontDI->SetSize(0.05f);
		HUD().pFontDI->Out(0,-0.5f,"TEAM 2 SCORES");
	} break;
	case GAME_PHASE_TEAMS_IN_A_DRAW: {
		HUD().pFontDI->SetAligment(CGameFont::alCenter);
		HUD().pFontDI->SetColor(0xA0969678);
		HUD().pFontDI->SetSize(0.05f);
		HUD().pFontDI->Out(0,-0.5f,"TEAMS IN A DRAW");
	} break;
/*	case ÒÐÅÒÜÅ_ÑÎÑÒÎßÍÈÅ:
		// åñëè èãðà çàêîí÷åíà
		HUD().pFontDI->SetAligment(CGameFont::alCenter);
		HUD().pFontDI->SetColor();
		HUD().pFontDI->SetSize(0.2f);
		HUD().pFontDI->Out(0,0.5f,"ÓÐÀ...../dgdfgdg");
	break;
*/	}
}
//--------------------------------------------------------------------

void CUIGameCS::Render()
{
	switch (m_cl_game->phase){
	case GAME_PHASE_PENDING: 
		PlayerList.Render();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) FragList.Render		();
		CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
		if (m_Actor){
			game_cl_GameState::Player* P = m_cl_game->local_player;
			if (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)				OwnBase.Render();
			else if (P->flags&GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE)	EnemyBase.Render();
			if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.Render();
			if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.Render();
		}
		break;
	}
}
//--------------------------------------------------------------------

bool CUIGameCS::IR_OnKeyboardPress(int dik)
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		if (CTMenu.Visible()&&CTMenu.IR_OnKeyboardPress(dik))		return true;
		// switch pressed keys
		switch (dik){
		case DIK_M:		BuyMenu.Hide(); CTMenu.Show();	return true;
		}
		break;
	case GAME_PHASE_INPROGRESS:
		if (BuyMenu.Visible()&&BuyMenu.IR_OnKeyboardPress(dik))	return true;
		if (CTMenu.Visible()&&CTMenu.IR_OnKeyboardPress(dik))		return true;
//		if (InventoryMenu.Visible()&&InventoryMenu.IR_OnKeyboardPress(dik))		return true;
		// switch pressed keys
		switch (dik){
//		case DIK_M:		CTMenu.Show(); BuyMenu.Hide(); InventoryMenu.Hide(); return true;
//		case DIK_B:		CTMenu.Hide(); BuyMenu.Show(); InventoryMenu.Hide(); return true;
//		case DIK_I:		CTMenu.Hide(); BuyMenu.Hide(); InventoryMenu.Show(); return true;
		case DIK_TAB:	SetFlag(flShowFragList,TRUE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameCS::IR_OnKeyboardRelease(int dik)
{
	if (m_cl_game->phase==GAME_PHASE_INPROGRESS){
		if (BuyMenu.Visible()&&BuyMenu.IR_OnKeyboardRelease(dik))	return true;
		if (CTMenu.Visible()&&CTMenu.IR_OnKeyboardRelease(dik))	return true;
//		if (InventoryMenu.Visible()&&InventoryMenu.IR_OnKeyboardRelease(dik))		return true;
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
