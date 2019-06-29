#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"
#include "UIFrameWindow.h"
#include "UIEditBox.h"
#include "UI3tButton.h"


#include "../GameSpy/GameSpy_Browser.h"

class CUIXml;
class CGameSpy_Browser;
class CUIColorAnimatorWrapper;
class CUIMessageBoxEx;

#define LST_SERVER		0 
#define LST_SRV_PROP	1
#define LST_PLAYERS		2
#define DIF_HEIGHT		180

class SServerFilters{
public:
	bool	empty;
	bool	full;
	bool	with_pass;
	bool	without_pass;
	bool	without_ff;
	bool	without_pb;
	bool	listen_servers;
};

class CServerList : public CUIWindow{
public:
	CServerList();
	virtual ~CServerList();

	virtual void Init(float x, float y, float width, float height);
	virtual void Update();
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void InitFromXml(CUIXml& xml_doc, const char* path);
			void InitHeader();
			void InitSeparator();
			void AddServerToList	(ServerInfo* pServerInfo);
			void UpdateServerInList	(ServerInfo* pServerInfo, int index);
			void UpdateServerInList	(ServerInfo* pServerInfo, CUIListItemServer* pItem);
			void ConnectToSelected();
			void SetFilters(SServerFilters& sf);
			void SetPlayerName(const char* name);
			void SetSortFunc(const char* func_name, bool make_sort);

	virtual void	RefreshGameSpyList	(bool Local);
			void	RefreshQuick();
			void	ShowServerInfo();			
	virtual	void	RefreshList();

protected:
			bool IsValidItem(ServerInfo& item);
			void SrvInfo2LstSrvInfo(const ServerInfo* pSrvInfo);
			void UpdateSizes();
			void UpdateVisibility();
			void BeforeAppear();
			void AfterAppear();
			void BeforeDisapear();
			void AfterDisappear();
			void FillUpDetailedServerInfo();
			void ClearDetailedServerInfo();

			void SaveCurItem();
			void RestoreCurItem();
			void ResetCurItem();

	static	bool sort_by_ServerName(int p1, int p2);
	static	bool sort_by_Map(int p1, int p2);
	static	bool sort_by_GameType(int p1, int p2);
	static	bool sort_by_Players(int p1, int p2);
	static	bool sort_by_Ping(int p1, int p2);

	CUIColorAnimatorWrapper* m_pAnimation;

	LIST_SRV_ITEM	m_itemInfo;
	SServerFilters	m_sf;
	CUIListWnd		m_list[3];
	CUIFrameWindow	m_frame[3];
	CUI3tButton		m_header[6];
	CUILabel		m_header2[4];
	CUIFrameLineWnd	m_separator[5];
	CUIEditBox		m_edit_gs_filter;
	xr_string		m_playerName;
	bool			m_bShowServerInfo;
	bool			m_bAnimation;
	float			m_fListH[2];
	float			m_fEditPos[2];
	int				m_cur_item;

	CUIMessageBoxEx* m_message_box;

	CGameSpy_Browser	m_GSBrowser;	

	shared_str		m_sort_func;
	xr_vector<int>	m_tmp_srv_lst;

	
};