#include "StdAfx.h"

#include "UIKickPlayer.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "../level.h"
#include "../game_cl_base.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"

CUIKickPlayer::CUIKickPlayer(){
	m_prev_upd_time = 0;
	bkgrnd = xr_new<CUIStatic>(); bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>(); header->SetAutoDelete(true);
	AttachChild(header);

	lst_back = xr_new<CUIFrameWindow>(); lst_back->SetAutoDelete(true);
	AttachChild(lst_back);

	lst = xr_new<CUIListBox>(); lst->SetAutoDelete(true);
	AttachChild(lst);

	btn_ok = xr_new<CUI3tButton>(); btn_ok->SetAutoDelete(true);
	AttachChild(btn_ok);

	btn_cancel = xr_new<CUI3tButton>(); btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	selected_item_id		= u32(-1);
	mode					= MODE_KICK;
}

void CUIKickPlayer::Init(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "kick_ban", 0, this);
	CUIXmlInit::InitStatic(xml_doc, "kick_ban:background", 0, bkgrnd);
	CUIXmlInit::InitFrameWindow(xml_doc, "kick_ban:list_back", 0, lst_back);
	CUIXmlInit::InitListBox(xml_doc, "kick_ban:list", 0, lst);
	CUIXmlInit::Init3tButton(xml_doc, "kick_ban:btn_ok", 0, btn_ok);
	CUIXmlInit::Init3tButton(xml_doc, "kick_ban:btn_cancel", 0, btn_cancel);
}

void CUIKickPlayer::InitBan(CUIXml& xml_doc){
	CUIXmlInit::InitStatic(xml_doc, "kick_ban:header_ban", 0, header);
    Init(xml_doc);
	mode = MODE_BAN;
}

void CUIKickPlayer::InitKick(CUIXml& xml_doc){
	CUIXmlInit::InitStatic(xml_doc, "kick_ban:header_kick", 0, header);
	Init(xml_doc);
	mode = MODE_KICK;
}

#include <dinput.h>
bool CUIKickPlayer::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (dik == DIK_ESCAPE)
	{
		OnBtnCancel();
		return true;
	}
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIKickPlayer::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (LIST_ITEM_SELECT == msg && pWnd == lst)
	{		
		CUIListBox* lst			= smart_cast<CUIListBox*>	(pWnd);
		CUIListBoxItem* itm		= lst->GetItemByID			( *((u32*)(pData)) );
		selected_item_id		= itm->GetID				();
		selected_item_text		= itm->GetText				();
	}
	else if (BUTTON_CLICKED == msg)
	{
		if (pWnd == btn_ok)
			OnBtnOk();
		else if (pWnd == btn_cancel)
			OnBtnCancel();
	}
}

void CUIKickPlayer::OnBtnOk(){
	CUIListBoxItem* item = smart_cast<CUIListBoxItem*>(lst->GetSelected());
	if (item)
	{
		string512 command;	
		switch (mode){
			case MODE_KICK:
                sprintf(command, "cl_votestart kick %s", item->GetText());
				break;
			case MODE_BAN:
				sprintf(command, "cl_votestart ban %s", item->GetText());
				break;
		}
		Console->Execute			(command);
		game_cl_mp* game			= smart_cast<game_cl_mp*>(&Game());
		game->StartStopMenu			(this, true);
	}
	else
		return;
}

void CUIKickPlayer::OnBtnCancel()
{
    game_cl_mp* game				= smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu				(this, true);
}

IC bool	DM_Compare_Players		(game_PlayerState* v1, game_PlayerState* v2);

DEFINE_VECTOR	(game_PlayerState*,ItemVec,ItemIt);

void CUIKickPlayer::Update()
{
	CUIDialogWnd::Update();

	static string512 teaminfo;
	if (m_prev_upd_time > Device.dwTimeContinual - 1000)
		return;
	m_prev_upd_time = Device.dwTimeContinual;
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

//.	items.clear			();
	ItemVec			items;
	for (;I!=E;++I)		
		items.push_back(I->second);

    std::sort(items.begin(), items.end(), DM_Compare_Players);

	lst->Clear();
	
	if( selected_item_id+1 > items.size() ) 
			selected_item_id = u32(-1);

	for (u32 i = 0; i<items.size(); i++){
		game_PlayerState* p			= items[i];
		CUIListBoxItem* item		= lst->AddItem(p->name);
		item->SetID					(i);
		if(selected_item_id == i)
		{
			if(p->name != selected_item_text)
			{
				selected_item_id	= u32(-1);
				selected_item_text	= "";
			}
		}
	}

	if (selected_item_id != u32(-1))
        lst->SetSelected	(selected_item_id);
}