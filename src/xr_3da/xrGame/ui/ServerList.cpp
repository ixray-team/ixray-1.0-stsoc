#include "StdAfx.h"
#include "ServerList.h"
#include "UIXmlInit.h"
#include "../string_table.h"
#include "../../xr_ioconsole.h"
#include "UIEditBox.h"
#include "UIColorAnimatorWrapper.h"
#include "UIListItemAdv.h"
#include "UIMessageBoxEx.h"
#include "TeamInfo.h"
#include "../MainMenu.h"


CGameSpy_Browser* g_gs_browser = NULL;



CServerList::CServerList(){
	m_GSBrowser.Init(this);
//	m_list[LST_SERVER].ShowSelectedItem();
//	AttachChild(&m_list[LST_SERVER]);

	
	for (int i = 0; i<6; i++)
		AttachChild(&m_header[i]);
	for (int i = 0; i<4; i++)
		AttachChild(&m_header2[i]);
	for (int i = 0; i<5; i++)
		AttachChild(&m_separator[i]);


    m_pAnimation = xr_new<CUIColorAnimatorWrapper>("ui_mm_mp_srvinfo");
	m_pAnimation->Cyclic(false);

	AttachChild(&m_edit_gs_filter);


	for (int i = 0; i<3; i++)
	{
		m_list[i].ShowSelectedItem();
		AttachChild(&m_frame[i]);
		AttachChild(&m_list[i]);
	}

	m_bShowServerInfo = false;
	m_bAnimation = false;

	m_sort_func = "none";
	m_message_box = xr_new<CUIMessageBoxEx>();
	m_message_box->Init("message_box_password");
	m_message_box->SetMessageTarget(this);
}

CServerList::~CServerList()
{
	xr_delete(m_pAnimation);
	xr_delete(m_message_box);
};

void CServerList::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
}

void CServerList::Update(){
	
	m_GSBrowser.Update();

	if (m_bAnimation)
	{
		m_pAnimation->Update();

		m_frame[LST_SRV_PROP].SetColor(subst_alpha(0xffffffff, color_get_A(m_pAnimation->GetColor())));
		m_frame[LST_PLAYERS].SetColor(subst_alpha(0xffffffff, color_get_A(m_pAnimation->GetColor())));

		for (int i = 0; i<4; i++)
		{
			m_header2[i].SetColor(subst_alpha(0xffffffff, color_get_A(m_pAnimation->GetColor())));
			m_header2[i].SetTextColor(subst_alpha(m_header2[i].GetTextColor(), color_get_A(m_pAnimation->GetColor())));
		}

		if (m_pAnimation->Done())
		{
			m_bAnimation = false;
			if (m_bShowServerInfo)
				AfterAppear();
			else
				AfterDisappear();
		}
	}
	CUIWindow::Update();
}

void CServerList::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (m_bShowServerInfo && LIST_ITEM_CLICKED == msg && &m_list[LST_SERVER] == pWnd){
        ClearDetailedServerInfo();
		FillUpDetailedServerInfo();
	}
	else if (BUTTON_CLICKED == msg){
		if (pWnd == &m_header[1]){
			SetSortFunc("server_name",true);
		}
		else if (pWnd == &m_header[2]){
			SetSortFunc("map",true);
		}
		else if (pWnd == &m_header[3]){
			SetSortFunc("game_type",true);
		}
		else if (pWnd == &m_header[4]){
			SetSortFunc("player",true);
		}
		else if (pWnd == &m_header[5]){
			SetSortFunc("ping",true);
		}
	}
	else if (MESSAGE_BOX_YES_CLICKED == msg){

		int sel = m_list[LST_SERVER].GetSelectedItem();
		if (-1 == sel)
			return;

		CUIListItemServer* item = smart_cast<CUIListItemServer*>(m_list[LST_SERVER].GetItem(sel));
		xr_string command;
		xr_string name_and_pass = m_playerName;
		name_and_pass += "/psw=";
		name_and_pass += m_message_box->GetPassword();
		item->CreateConsoleCommand(command, name_and_pass.c_str());
//		Console->Execute("main_menu off");
		Console->Execute(command.c_str());
	}
}

void CServerList::BeforeAppear(){
    UpdateSizes();
	UpdateVisibility();
}
void CServerList::AfterAppear(){
	FillUpDetailedServerInfo();
	UpdateVisibility();
}
void CServerList::BeforeDisapear(){
	ClearDetailedServerInfo();
	UpdateVisibility();
}
void CServerList::AfterDisappear(){
	UpdateSizes();
	UpdateVisibility();
}

void CServerList::FillUpDetailedServerInfo(){
	int si = m_list[LST_SERVER].GetSelectedItem();

	bool t1 = false;
	bool t2 = false;
	bool spect = false;
		
    if (-1 != si)
	{
		CUIListItemServer* pItem = (CUIListItemServer*)m_list[LST_SERVER].GetItem(si);
		R_ASSERT(pItem);

		ServerInfo srvInfo;
		m_GSBrowser.GetServerInfoByIndex(&srvInfo, pItem->GetInfo()->info.Index);
		u32 teams = srvInfo.m_aTeams.size();

//		R_ASSERT(teams>0 && teams<3);
		if (2 == teams){
			string256 _buff;
//			LPCSTR tm_col = pSettings->r_string("team1","color");
//			u32 team1_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
//			tm_col = pSettings->r_string("team2","color");
//			u32 team2_color = color_argb(155,atoi(_GetItem(tm_col, 0, _buff)),atoi(_GetItem(tm_col, 1, _buff)),atoi(_GetItem(tm_col, 2, _buff)));
			
//			LPCSTR team1_name = pSettings->r_string("team1","name");
//			LPCSTR team2_name = pSettings->r_string("team2","name");
			CUIListItemAdv* pItemAdv;

			// TEAM 1
			xr_vector<PlayerInfo>::iterator it;
			for (it = srvInfo.m_aPlayers.begin(); it != srvInfo.m_aPlayers.end(); it++)
			{
				PlayerInfo pf = *it;
				if (1 != pf.Team)
					continue;
				if (pf.Spectator)
					continue;

				if (!t1)		// add header
				{
					sprintf(_buff, "team \"%s\"", *CTeamInfo::GetTeam1_name());
					pItemAdv = xr_new<CUIListItemAdv>();
					pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
					pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
					pItemAdv->AddField(_buff, m_list[LST_PLAYERS].GetItemWidth());
					m_list[LST_PLAYERS].AddItem(pItemAdv);
					t1 = true;
				}


 				pItemAdv = xr_new<CUIListItemAdv>();				

				char buf[16];
				pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
				pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
				pItemAdv->AddField(pf.Name, m_header2[1].GetWidth());
				pItemAdv->AddField(itoa(pf.Frags, buf,10), m_header2[2].GetWidth());
				pItemAdv->AddField(itoa(pf.Deaths, buf,10), m_header2[3].GetWidth());
				m_list[LST_PLAYERS].AddItem(pItemAdv);
			}

			
			// TEAM 2
			for (it = srvInfo.m_aPlayers.begin(); it != srvInfo.m_aPlayers.end(); it++)
			{
				PlayerInfo pf = *it;
				if (2 != pf.Team)
					continue;
				if (pf.Spectator)
					continue;

				if (!t2)
				{
					sprintf(_buff, "team \"%s\"", *CTeamInfo::GetTeam2_name());
					pItemAdv = xr_new<CUIListItemAdv>();
					pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
					pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
					pItemAdv->AddField(_buff, m_list[LST_PLAYERS].GetItemWidth());
					m_list[LST_PLAYERS].AddItem(pItemAdv);

					t2 = true;
				}

				pItemAdv = xr_new<CUIListItemAdv>();				
				char buf[16];
				pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
				pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
				pItemAdv->AddField(pf.Name, m_header2[1].GetWidth());
				pItemAdv->AddField(itoa(pf.Frags, buf,10), m_header2[2].GetWidth());
				pItemAdv->AddField(itoa(pf.Deaths, buf,10), m_header2[3].GetWidth());
				m_list[LST_PLAYERS].AddItem(pItemAdv);
			}

			// SPECTATORS
			for (it = srvInfo.m_aPlayers.begin(); it != srvInfo.m_aPlayers.end(); it++)
			{
				PlayerInfo pf = *it;
				if (!pf.Spectator)
					continue;

				if (!spect)
				{
					sprintf(_buff, "spectator");
					pItemAdv = xr_new<CUIListItemAdv>();
					pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
					pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
					pItemAdv->AddField(_buff, m_list[LST_PLAYERS].GetItemWidth());
					m_list[LST_PLAYERS].AddItem(pItemAdv);

					spect = true;
				}

				pItemAdv = xr_new<CUIListItemAdv>();				
				char buf[16];
				pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
				pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
				pItemAdv->AddField(pf.Name, m_header2[1].GetWidth());
				pItemAdv->AddField(itoa(pf.Frags, buf,10), m_header2[2].GetWidth());
				pItemAdv->AddField(itoa(pf.Deaths, buf,10), m_header2[3].GetWidth());
				m_list[LST_PLAYERS].AddItem(pItemAdv);
			}

		}
		else
		{
			xr_vector<PlayerInfo>::iterator it;
			for (it = srvInfo.m_aPlayers.begin(); it != srvInfo.m_aPlayers.end(); it++)
			{
				PlayerInfo pf = *it;
				CUIListItemAdv* pItemAdv = xr_new<CUIListItemAdv>();

				char buf[16];

				pItemAdv->SetFont(m_list[LST_PLAYERS].GetFont());
				pItemAdv->SetTextColor(m_list[LST_PLAYERS].GetTextColor());
				pItemAdv->AddField(pf.Name, m_header2[1].GetWidth());
				pItemAdv->AddField(itoa(pf.Frags, buf,10), m_header2[2].GetWidth());
				pItemAdv->AddField(itoa(pf.Deaths, buf,10), m_header2[3].GetWidth());
				m_list[LST_PLAYERS].AddItem(pItemAdv);
			}
		}

		xr_vector<GameInfo>::iterator it;
		for (it = srvInfo.m_aInfos.begin(); it != srvInfo.m_aInfos.end(); it++){
			GameInfo gi = *it;
			CUIListItemAdv* pItemAdv = xr_new<CUIListItemAdv>();

			pItemAdv->AddField(*gi.InfoName, m_list[LST_SRV_PROP].GetWidth()/2);
			pItemAdv->AddField(*gi.InfoData, m_list[LST_SRV_PROP].GetWidth()/2);

			m_list[LST_SRV_PROP].AddItem(pItemAdv);
		}
	}
	else
		ClearDetailedServerInfo();
}

void CServerList::ClearDetailedServerInfo(){
	m_list[LST_SRV_PROP].RemoveAll();
	m_list[LST_PLAYERS].RemoveAll();
}

void CServerList::ShowServerInfo(){
	m_bShowServerInfo = !m_bShowServerInfo;
	m_bAnimation = true;
	m_pAnimation->Reverese(!m_bShowServerInfo);
	m_pAnimation->Reset();
	if (m_bShowServerInfo)
		BeforeAppear();
	else
		BeforeDisapear();
}

void CServerList::UpdateSizes(){
	m_list[LST_SERVER].SetHeight(m_bShowServerInfo? m_fListH[1]:m_fListH[0]);
	m_frame[LST_SERVER].SetHeight(m_bShowServerInfo? m_fListH[1]:m_fListH[0]);
	Fvector2 pos = m_edit_gs_filter.GetWndPos();
	pos.y = m_bShowServerInfo?m_fEditPos[1]:m_fEditPos[0];
	m_edit_gs_filter.SetWndPos(pos);

	for (int i = 0; i< 5; i++)
		m_separator[i].SetHeight(m_bShowServerInfo? m_fListH[1]:m_fListH[0]);	
}

void CServerList::UpdateVisibility(){
	m_list[LST_SRV_PROP].Show(m_bShowServerInfo ? !m_bAnimation : false);
	m_list[LST_PLAYERS].Show(m_bShowServerInfo ? !m_bAnimation: false);
	m_frame[LST_SRV_PROP].Show(m_bShowServerInfo ? true : m_bAnimation);
	m_frame[LST_PLAYERS].Show(m_bShowServerInfo ? true : m_bAnimation);

	for (int i = 0; i< 5; i++)
		m_header2[i].Show(m_bShowServerInfo ? true : m_bAnimation);
}

void CServerList::SetFilters(SServerFilters& sf){
	m_sf = sf;
	RefreshList();
}

void CServerList::SetPlayerName(const char* name){
	m_playerName = name;
}

bool CServerList::IsValidItem(ServerInfo& item){
	bool result = true;

	result &= !m_sf.empty ? (m_sf.empty == (item.m_ServerNumPlayers == 0))						: true;
	result &= !m_sf.full ? (m_sf.full == (item.m_ServerNumPlayers == item.m_ServerMaxPlayers))	: true;
	result &= !m_sf.with_pass ? (m_sf.with_pass == item.m_bPassword)							: true;
	result &= !m_sf.without_pass ? (m_sf.without_pass != item.m_bPassword)						: true;
	result &= !m_sf.without_ff ? (m_sf.without_ff != item.m_bFFire)								: true;
	//result &= m_sf.without_pb ? (m_sf.without_pb = item.m)
	result &= !m_sf.listen_servers ? (m_sf.listen_servers != item.m_bDedicated)					: true;

	return result;
}

void CServerList::InitFromXml(CUIXml& xml_doc, const char* path){
	CUIXmlInit::InitWindow		(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf,path,":list"),							0, &m_list[LST_SERVER]);
	m_fListH[0] =				m_list[LST_SERVER].GetHeight();
	m_fListH[1] =				xml_doc.ReadAttribFlt(buf,0,"height2");
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf,path,":list_server_properties"),		0, &m_list[LST_SRV_PROP]);
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf,path,":list_players_list"),				0, &m_list[LST_PLAYERS]);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf,path,":frame"),							0, &m_frame[LST_SERVER]);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf,path,":list_server_properties:frame"),	0, &m_frame[LST_SRV_PROP]);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf,path,":list_players_list:frame"),		0, &m_frame[LST_PLAYERS]);
	CUIXmlInit::InitFont		(xml_doc, strconcat(buf,path,":list_item:text"),				0, m_itemInfo.color, m_itemInfo.font);
	CUIXmlInit::InitEditBox		(xml_doc, strconcat(buf,path,":edit_gs_filter"),				0, &m_edit_gs_filter);
	m_fEditPos[0] =				m_edit_gs_filter.GetWndPos().y;
	m_fEditPos[1] =				xml_doc.ReadAttribFlt(buf,0,"y2");
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":cap_server_properties"),			0, &m_header2[0]);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":cap_players_list"),				0, &m_header2[1]);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":cap_frags"),						0, &m_header2[2]);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":cap_death"),						0, &m_header2[3]);
	
	m_itemInfo.size.icon	= xml_doc.ReadAttribFlt( strconcat(buf, path, ":sizes"), 0, "icon");
	m_itemInfo.size.server	= xml_doc.ReadAttribFlt( buf, 0, "server");
	m_itemInfo.size.map		= xml_doc.ReadAttribFlt( buf, 0, "map");
	m_itemInfo.size.game	= xml_doc.ReadAttribFlt( buf, 0, "game");
	m_itemInfo.size.players	= xml_doc.ReadAttribFlt( buf, 0, "players");
	m_itemInfo.size.ping	= xml_doc.ReadAttribFlt( buf, 0, "ping");

	// init header elements
	for (int i = 0; i<6; i++)
		CUIXmlInit::Init3tButton(xml_doc,strconcat(buf,path,":header"), 0, &m_header[i]);
	m_header[0].Enable(false);
	for (int i = 0; i<5; i++)
	{
		CUIXmlInit::InitFrameLine(xml_doc, strconcat(buf,path,":separator"), 0, &m_separator[i]);
		m_separator[i].SetHeight(m_list[LST_SERVER].GetHeight());
	}
	InitHeader();
	UpdateSizes();
	UpdateVisibility();
}

void CServerList::ConnectToSelected(){
	int sel = m_list[LST_SERVER].GetSelectedItem();
	if (-1 == sel)
		return;

	CUIListItemServer* item = smart_cast<CUIListItemServer*>(m_list[LST_SERVER].GetItem(sel));

//	params.info.icons.pass

	if (item->GetInfo()->info.icons.pass)
	{
		MainMenu()->StartStopMenu(m_message_box,true);
	}
	else
	{
		xr_string command;

		item->CreateConsoleCommand(command, m_playerName.c_str());

//		Console->Execute("main_menu off");
		Console->Execute(command.c_str());
	}
}

void CServerList::InitHeader(){
	Fvector2 pos;
	pos.set(0,0);

	float y = m_header[0].GetHeight();

	CStringTable st;

	m_header[0].SetWidth(m_itemInfo.size.icon);
//	m_header[0].SetText(*st.translate("icon"));
	pos.x += m_itemInfo.size.icon;
	m_separator[0].SetWndPos(pos.x,y + 3);
	m_header[1].SetWidth(m_itemInfo.size.server);
	m_header[1].SetWndPos(pos);
	m_header[1].SetText(*st.translate("server name"));
	pos.x += m_itemInfo.size.server;
	m_separator[1].SetWndPos(pos.x,y + 3);
	m_header[2].SetWidth(m_itemInfo.size.map);
	m_header[2].SetWndPos(pos);
	m_header[2].SetText(*st.translate("map"));
	pos.x += m_itemInfo.size.map;
	m_separator[2].SetWndPos(pos.x,y + 3);
	m_header[3].SetWidth(m_itemInfo.size.game);
	m_header[3].SetWndPos(pos);
	m_header[3].SetText(*st.translate("game type"));
	pos.x += m_itemInfo.size.game;
	m_separator[3].SetWndPos(pos.x,y + 3);
	m_header[4].SetWidth(m_itemInfo.size.players);
	m_header[4].SetWndPos(pos);
	m_header[4].SetText(*st.translate("players"));
	pos.x += m_itemInfo.size.players;
	m_separator[4].SetWndPos(pos.x,y + 3);
	m_header[5].SetWidth(m_itemInfo.size.ping);
	m_header[5].SetWndPos(pos);
	m_header[5].SetText(*st.translate("ping"));
}

void	CServerList::RefreshGameSpyList	(bool Local){
	if (Local)
		Msg("Refresh Local List");
	else
		Msg("Refresh MasterServer List");
	
	m_GSBrowser.RefreshList_Full(Local);
	ResetCurItem();
	RefreshList();
}


void CServerList::AddServerToList	(ServerInfo* pServerInfo)
{
	if (!IsValidItem(*pServerInfo)) return;

	CUIListItemServer* item = xr_new<CUIListItemServer>();
	float w = m_list[LST_SERVER].GetItemWidth();
	float h = m_list[LST_SERVER].GetItemHeight();
	SrvInfo2LstSrvInfo(pServerInfo);
	item->Init(m_itemInfo, 0, 0, w, h);
	m_list[LST_SERVER].AddItem<CUIListItemServer>(item);
};

void	CServerList::UpdateServerInList(ServerInfo* pServerInfo, int index){
	int sz = m_list[LST_SERVER].GetItemsCount();

	for (int i = 0; i< sz; i++)
	{
		CUIListItemServer* pItem = static_cast<CUIListItemServer*>(m_list[LST_SERVER].GetItem(i));
		if (pItem->Get_gs_index() == index)
		{
			UpdateServerInList(pServerInfo, pItem);
			return;
		}
	}

    R_ASSERT2(false, "CServerList::UpdateServerInList - invalid index");

};

void	CServerList::UpdateServerInList(ServerInfo* pServerInfo, CUIListItemServer* pItem){
	SrvInfo2LstSrvInfo(pServerInfo);
	pItem->SetParams(m_itemInfo);
};

void	CServerList::RefreshList()
{
	SaveCurItem();
	m_list[LST_SERVER].RemoveAll();
	//-------------------------------
	u32 NumServersFound = m_GSBrowser.GetServersCount();
	g_gs_browser = &m_GSBrowser;
	m_tmp_srv_lst.resize(NumServersFound);

	
	for (u32 i=0; i<NumServersFound; i++)
		m_tmp_srv_lst[i] = i;

	if (0 == xr_strcmp(m_sort_func, "server_name"))
		sort(m_tmp_srv_lst.begin(), m_tmp_srv_lst.end(), sort_by_ServerName);
	else if (0 == xr_strcmp(m_sort_func, "map"))
		sort(m_tmp_srv_lst.begin(), m_tmp_srv_lst.end(), sort_by_Map);
	else if (0 == xr_strcmp(m_sort_func, "game_type"))
		sort(m_tmp_srv_lst.begin(), m_tmp_srv_lst.end(), sort_by_GameType);
	else if (0 == xr_strcmp(m_sort_func, "player"))
		sort(m_tmp_srv_lst.begin(), m_tmp_srv_lst.end(), sort_by_GameType);
	else if (0 == xr_strcmp(m_sort_func, "ping"))
		sort(m_tmp_srv_lst.begin(), m_tmp_srv_lst.end(), sort_by_GameType);

	for (u32 i=0; i<NumServersFound; i++)
	{
		ServerInfo NewServerInfo;
		m_GSBrowser.GetServerInfoByIndex(&NewServerInfo, m_tmp_srv_lst[i]);
		AddServerToList(&NewServerInfo);
	}
	RestoreCurItem();
};

void CServerList::RefreshQuick(){
	int SvId = m_list[LST_SERVER].GetSelectedItem();
	if (-1 == SvId)
		return;
	CUIListItemServer* pItem = (CUIListItemServer*)m_list[LST_SERVER].GetItem(SvId);
	m_GSBrowser.RefreshQuick(pItem->GetInfo()->info.Index);
	
	RefreshList();

	ClearDetailedServerInfo();
	FillUpDetailedServerInfo();
}

void CServerList::SetSortFunc(const char* func_name, bool make_sort){
	m_sort_func = func_name;

	if (make_sort)
		RefreshList();
}

void CServerList::SrvInfo2LstSrvInfo(const ServerInfo* pServerInfo){
	m_itemInfo.info.server	= pServerInfo->m_ServerName;
	xr_string address = pServerInfo->m_HostName;
	char port[8];
	address+= "/port=";	
	address+= itoa(pServerInfo->m_Port, port, 10);
	m_itemInfo.info.address = address.c_str();
	m_itemInfo.info.map		= pServerInfo->m_SessionName;
	m_itemInfo.info.game	= pServerInfo->m_ServerGameType;
	m_itemInfo.info.players.sprintf("%d/%d", pServerInfo->m_ServerNumPlayers, pServerInfo->m_ServerMaxPlayers);
	m_itemInfo.info.ping.sprintf("%d", pServerInfo->m_Ping);
	m_itemInfo.info.icons.pass	= pServerInfo->m_bPassword;
	m_itemInfo.info.icons.dedicated	= pServerInfo->m_bDedicated;
	m_itemInfo.info.icons.punkbuster = false;//	= pServerInfo->m_bPunkBuster;
	m_itemInfo.info.Index	= pServerInfo->Index;   
}

bool CServerList::sort_by_ServerName(int p1, int p2){
	CGameSpy_Browser& gs_browser = *g_gs_browser;
	ServerInfo info1,info2;

	gs_browser.GetServerInfoByIndex(&info1, p1);
    gs_browser.GetServerInfoByIndex(&info2, p2);

	return -1 == xr_strcmp(info1.m_ServerName, info2.m_ServerName);
}

bool CServerList::sort_by_Map(int p1, int p2){
	CGameSpy_Browser& gs_browser = *g_gs_browser;
	ServerInfo info1,info2;

	gs_browser.GetServerInfoByIndex(&info1, p1);
    gs_browser.GetServerInfoByIndex(&info2, p2);

	return -1 == xr_strcmp(info1.m_SessionName, info2.m_SessionName);
}

bool CServerList::sort_by_GameType(int p1, int p2){
	CGameSpy_Browser& gs_browser = *g_gs_browser;
	ServerInfo info1,info2;

	gs_browser.GetServerInfoByIndex(&info1, p1);
    gs_browser.GetServerInfoByIndex(&info2, p2);

	return -1 == xr_strcmp(info1.m_ServerGameType, info2.m_ServerGameType);
}

bool CServerList::sort_by_Players(int p1, int p2){
	CGameSpy_Browser& gs_browser = *g_gs_browser;
	ServerInfo info1,info2;

	gs_browser.GetServerInfoByIndex(&info1, p1);
    gs_browser.GetServerInfoByIndex(&info2, p2);

	return info1.m_ServerNumPlayers < info2.m_ServerNumPlayers;
}
bool CServerList::sort_by_Ping(int p1, int p2){
	CGameSpy_Browser& gs_browser = *g_gs_browser;
	ServerInfo info1,info2;

	gs_browser.GetServerInfoByIndex(&info1, p1);
    gs_browser.GetServerInfoByIndex(&info2, p2);

	return info1.m_Ping < info2.m_Ping;
}

void CServerList::SaveCurItem(){
	int SvId = m_list[LST_SERVER].GetSelectedItem();
	if (-1 == SvId){
		m_cur_item = -1;
		return;
	}

	CUIListItemServer* pItem = (CUIListItemServer*)m_list[LST_SERVER].GetItem(SvId);
	R_ASSERT(pItem);

	m_cur_item = pItem->GetInfo()->info.Index;
}

void CServerList::RestoreCurItem(){
	if (-1 == m_cur_item)
		return;
	int index = m_list[LST_SERVER].FindItemWithValue(m_cur_item);

	m_list[LST_SERVER].SetSelectedItem(index);	
	m_list[LST_SERVER].ScrollToPos(index);
}

void CServerList::ResetCurItem(){
	m_list[LST_SERVER].SetSelectedItem(-1);
	m_list[LST_SERVER].ScrollToBegin();
}