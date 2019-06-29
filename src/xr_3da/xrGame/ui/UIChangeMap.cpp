#include "StdAfx.h"
#include "UIChangeMap.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "../level.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"
#include "UIMapList.h"

xr_token	game_types		[];

CUIChangeMap::CUIChangeMap()
{
	m_prev_upd_time = 0;

	bkgrnd = xr_new<CUIStatic>(); bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>(); header->SetAutoDelete(true);
	AttachChild(header);

	map_pic = xr_new<CUIStatic>(); map_pic->SetAutoDelete(true);
	AttachChild(map_pic);

	map_frame = xr_new<CUIStatic>(); map_frame->SetAutoDelete(true);
	AttachChild(map_frame);

	frame = xr_new<CUIFrameWindow>(); frame->SetAutoDelete(true);
	AttachChild(frame);

	lst_back = xr_new<CUIFrameWindow>(); lst_back->SetAutoDelete(true);
	AttachChild(lst_back);

	lst = xr_new<CUIListBox>(); lst->SetAutoDelete(true);
	AttachChild(lst);

	btn_ok = xr_new<CUI3tButton>(); btn_ok->SetAutoDelete(true);
	AttachChild(btn_ok);

	btn_cancel = xr_new<CUI3tButton>(); btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	selected_item = u32(-1);
}

void CUIChangeMap::Init(CUIXml& xml_doc)
{
	CUIXmlInit::InitWindow				(xml_doc,			"change_map", 0, this);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:header", 0, header);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:background", 0, bkgrnd);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:map_frame", 0, map_frame);
	CUIXmlInit::InitStatic				(xml_doc,			"change_map:map_pic", 0, map_pic); map_pic->ClipperOn();
	CUIXmlInit::InitFrameWindow			(xml_doc,			"change_map:list_back", 0, lst_back);
	CUIXmlInit::InitFrameWindow			(xml_doc,			"change_map:frame", 0, frame);
	CUIXmlInit::InitListBox				(xml_doc,			"change_map:list", 0, lst);
	CUIXmlInit::Init3tButton			(xml_doc,			"change_map:btn_ok", 0, btn_ok);
	CUIXmlInit::Init3tButton			(xml_doc,			"change_map:btn_cancel", 0, btn_cancel);

	map_pic->InitTexture				("ui\\ui_noise");

	FillUpList							();
}

#include <dinput.h>
bool CUIChangeMap::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik == DIK_ESCAPE)
	{
		OnBtnCancel();
		return true;
	}
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIChangeMap::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (LIST_ITEM_SELECT == msg && pWnd == lst)
	{		
		selected_item = *static_cast<u32*>(pData);
		OnItemSelect();
	}
	else if (BUTTON_CLICKED == msg)
	{
		if (pWnd == btn_ok)
			OnBtnOk();
		else if (pWnd == btn_cancel)
			OnBtnCancel();
	}
}

void CUIChangeMap::OnItemSelect()
{
	u32 idx					= lst->GetSelectedIDX();
	if(idx==u32(-1))		return;

	const shared_str& name	= maps[idx];

	xr_string map_name		= "intro\\intro_map_pic_";
	map_name				+=	name.c_str();
	xr_string full_name		= map_name + ".dds";

	if (FS.exist("$game_textures$",full_name.c_str()))
		map_pic->InitTexture(map_name.c_str());
	else
		map_pic->InitTexture("ui\\ui_noise");
}

void CUIChangeMap::OnBtnOk()
{
	u32 idx					= lst->GetSelectedIDX();
	const shared_str& name	= maps[idx];

	{
		string512					command;
        sprintf						(command, "cl_votestart changemap %s", name.c_str());
		Console->Execute			(command);
		GetHolder()->StartStopMenu	(this, true);
	}
}
#include "../string_table.h"
void CUIChangeMap::FillUpList()
{
	lst->Clear				();
	maps.clear_not_free		();
	string_path				fn;
	FS.update_path			(fn, "$game_config$", MAP_LIST);
	CInifile map_list_cfg	(fn);

	LPCSTR sect_name		= get_token_name( game_types, GameID() );

	CInifile::Sect& S		= map_list_cfg.r_section(sect_name);
	CInifile::SectIt it		= S.begin(), end = S.end();
	
	for (;it!=end; ++it)
	{
		shared_str _map_name = it->first;
		maps.push_back		(_map_name);
		lst->AddItem		( CStringTable().translate(_map_name).c_str() );
	}
}

void CUIChangeMap::OnBtnCancel()
{
		GetHolder()->StartStopMenu	(this, true);
}
