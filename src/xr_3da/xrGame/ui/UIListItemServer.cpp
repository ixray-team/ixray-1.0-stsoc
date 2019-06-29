#include "StdAfx.h"

#include "UIListItemServer.h"
#include "UITextureMaster.h"

CUIListItemServer::CUIListItemServer(){
	AttachChild(&m_icon);
	m_icon.AttachChild(&m_iconPass);
	m_icon.AttachChild(&m_iconDedicated);
	m_icon.AttachChild(&m_iconPunkBuster);
	AttachChild(&m_server);
	AttachChild(&m_map);
	AttachChild(&m_game);
	AttachChild(&m_players);
	AttachChild(&m_ping);
}

void CUIListItemServer::Init(LIST_SRV_ITEM& params, float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);

	SetTextColor(params.color);
	SetFont(params.font);

	float offset = 0.0f;

	m_icon.Init(offset, 0, params.size.icon, height);
	offset += params.size.icon;

	m_server.Init(offset, 0, params.size.server, height);
//	m_server.SetText(*params.info.server);
	offset += params.size.server;

	m_map.Init(offset, 0, params.size.map, height);
//	m_map.SetText(*params.info.map);
	offset += params.size.map;

	m_game.Init(offset, 0, params.size.game, height);
//	m_game.SetText(*params.info.game);
	offset += params.size.game;

	m_players.Init(offset, 0, params.size.game, height);
//	m_players.SetText(*params.info.players);
	offset += params.size.players;

	m_ping.Init(offset, 0, params.size.game, height);
//	m_ping.SetText(*params.info.ping);

	float icon_size = CUITextureMaster::GetTextureHeight("ui_icon_password");
//	if (params.info.icons.pass)
//	{
	m_iconPass.Init(0,0,icon_size,icon_size);
	m_iconPass.InitTexture("ui_icon_password");
//	}
//	if (params.info.icons.dedicated)
//	{
	m_iconDedicated.Init(icon_size,0,icon_size,icon_size);
	m_iconDedicated.InitTexture("ui_icon_dedicated");
//	}
	
//	if (params.info.icons.punkbuster)
//	{
	m_iconPunkBuster.Init(icon_size*2,0,icon_size,icon_size);
	m_iconPunkBuster.InitTexture("ui_icon_punkbuster");
//	}

	SetParams(params);



	m_srv_info = params;
}

void CUIListItemServer::SetParams(LIST_SRV_ITEM& params){
	m_server.SetText(*params.info.server);
	m_map.SetText(*params.info.map);
	m_game.SetText(*params.info.game);
	m_players.SetText(*params.info.players);
	m_ping.SetText(*params.info.ping);
	m_iconPass.Show(params.info.icons.pass);
	m_iconDedicated.Show(params.info.icons.dedicated);
	m_iconPunkBuster.Show(params.info.icons.punkbuster);

	SetValue(params.info.Index);
}

void CUIListItemServer::Draw(){
	CUIStatic::Draw();
	CUIWindow::Draw();
}


void CUIListItemServer::SetTextColor(u32 color){
	//m_icon.SetTextColor(color);
	m_server.SetTextColor(color);
	m_map.SetTextColor(color);
	m_game.SetTextColor(color);
	m_players.SetTextColor(color);
	m_ping.SetTextColor(color);
}

void CUIListItemServer::SetFont(CGameFont* pFont){
	m_icon.SetFont(pFont);
	m_server.SetFont(pFont);
	m_map.SetFont(pFont);
	m_game.SetFont(pFont);
	m_players.SetFont(pFont);
	m_ping.SetFont(pFont);
}

void CUIListItemServer::CreateConsoleCommand(xr_string& command, LPCSTR player_name){
	command = "start client(";
	command+= *m_srv_info.info.address;
	command+= "/name=";
	command+= player_name;
	command+= ")";
}
