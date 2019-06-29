// CUIDiaryWnd.cpp:  дневник и все что с ним связано
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIInteractiveListItem.h"
#include "UIPdaWnd.h"
#include "../UI.h"
#include "UITreeViewItem.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIMainIngameWnd.h"

#include "../InfoPortionDefs.h"
#include "../Actor.h"
#include "../character_info.h"
#include "../ai/trader/ai_trader.h"

#include "../xrserver.h"
#include "../xrServer_Objects_ALife_Monsters.h"

#include "../script_space.h"
#include "../script_engine.h"

#include "../string_table.h"
#include "../hudmanager.h"
#include "../string_table.h"
#include "../alife_registry_wrappers.h"
#include "../level.h"
#include "UIEncyclopediaCore.h"
//////////////////////////////////////////////////////////////////////////

const char * const	DIARY_XML			= "events_new.xml";
const int			contractsOffset		= 100;

//////////////////////////////////////////////////////////////////////////

// ID for tree view items
enum EDiaryIDs
{
	idJobsCurrent,
	idJobsAccomplished,
	idJobsFailed,
	idContracts,
	idNews,
	idActorDiary,
	idMax
};

//-----------------------------------------------------------------------------/
//	Default constructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::CUIDiaryWnd()
	:	m_pActiveSubdialog		(NULL),
		m_pTreeItemFont			(NULL),
		m_pTreeRootFont			(NULL),
//		m_pContractsTreeItem	(NULL),
		m_uTreeRootColor		(0xffffffff),
		m_uTreeItemColor		(0xffffffff),
		m_pLeftHorisontalLine	(NULL),
//		m_pActorDiaryRoot		(NULL),
		m_pActiveJobs			(NULL),
		m_pJobsRoot				(NULL),
		m_pNews					(NULL)
{
//	Show(false);
}

//-----------------------------------------------------------------------------/
//  Destructor
//-----------------------------------------------------------------------------/
CUIDiaryWnd::~CUIDiaryWnd()
{

}

void CUIDiaryWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, DIARY_XML);
	R_ASSERT3(xml_result, "xml file not found", DIARY_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UITreeViewBg);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, &UITreeViewBg);

	UITreeViewBg.AttachChild(&UITreeViewHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, &UITreeViewHeader);

	xml_init.InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT(m_pTreeItemFont);
	xml_init.InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT(m_pTreeRootFont);

	UITreeViewHeader.AttachChild(&UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimation);

	AttachChild(&UIFrameWnd);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, &UIFrameWnd);

	UIFrameWnd.AttachChild(&UIFrameWndHeader);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, &UIFrameWndHeader);
	UIFrameWndHeader.UITitleText.SetElipsis(CUIStatic::eepBegin, 20);

	UIFrameWnd.AttachChild(&UIArticleCaption);
	xml_init.InitStatic(uiXml, "article_header_static", 0, &UIArticleCaption);
	
	UITreeViewBg.AttachChild(&UITreeView);
	xml_init.InitListWnd(uiXml, "idx_list", 0, &UITreeView);
	UITreeView.SetMessageTarget(this);
	UITreeView.EnableScrollBar(true);

	// Поддиалоги
	UIJobsWnd.Init();
	UIJobsWnd.Show(false);
	UINewsWnd.Init();
	UINewsWnd.Show(false);
	UINewsWnd.SetMessageTarget(this);
	UIContractsWnd.Init();
	UIActorDiaryWnd.Init(&UITreeView);
	UIActorDiaryWnd.Show(false);
	UIActorDiaryWnd.m_pCore->m_pTreeRootFont	= m_pTreeRootFont;
	UIActorDiaryWnd.m_pCore->m_pTreeItemFont	= m_pTreeItemFont;
	UIActorDiaryWnd.m_pCore->m_uTreeRootColor	= m_uTreeRootColor;
	UIActorDiaryWnd.m_pCore->m_uTreeItemColor	= m_uTreeItemColor;

	m_pActiveSubdialog = NULL;
	
	InitTreeView();

	// Autostatics
	m_pLeftHorisontalLine = xml_init.InitAutoStatic(uiXml, "left_auto_static", &UIFrameWnd)[2];
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UITreeViewBg);
}
//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	static CUITreeViewItem *pPrtevTVItem = NULL;

	if (&UITreeView == pWnd && LIST_ITEM_CLICKED == msg)
	{
		// для начала проверим, что нажатый элемент не рутовый
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT(pTVItem);

		if (!pTVItem->IsRoot() && pPrtevTVItem != pTVItem)
		{
			pPrtevTVItem = pTVItem;

			xr_string caption = static_cast<xr_string>(ALL_PDA_HEADER_PREFIX) + pTVItem->GetHierarchyAsText();
			UIFrameWndHeader.UITitleText.SetText(caption.c_str());


			if (m_pActiveSubdialog)
			{
				m_pActiveSubdialog->Show(false);
				UIFrameWnd.DetachChild(m_pActiveSubdialog);
			}

			if (m_pActiveSubdialog == &UIContractsWnd)
			{
				VERIFY(m_pLeftHorisontalLine);
				Frect r = m_pLeftHorisontalLine->GetWndRect();
				m_pLeftHorisontalLine->SetWndPos(r.left, r.top - contractsOffset);
			}

//			Frect r;

			EDiaryIDs id;
			if (false/*m_pActorDiaryRoot->Find(pTVItem)*/)
			{
				id = idActorDiary;
			}
			else
			{
				id = static_cast<EDiaryIDs>(pTVItem->GetValue());
			}

			CStringTable stbl;

			switch (id)
			{

			case idJobsFailed:
				UIJobsWnd.SetFilter(eTaskStateFail);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*stbl(m_pActiveSubdialog->WindowName()));
				break;
			case idJobsAccomplished:
				UIJobsWnd.SetFilter(eTaskStateCompleted);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*stbl(m_pActiveSubdialog->WindowName()));
				break;
			case idJobsCurrent:
				//сбростить флажок необходимости прочтения PDA
				HUD().GetUI()->UIMainIngameWnd->SetFlashIconState(CUIMainIngameWnd::efiPdaTask, false);
				UIJobsWnd.SetFilter(eTaskStateInProgress);
				m_pActiveSubdialog = &UIJobsWnd;
				ArticleCaption(*stbl(m_pActiveSubdialog->WindowName()));
				break;
/*			case idContracts:
				VERIFY(m_pLeftHorisontalLine);
				r = m_pLeftHorisontalLine->GetWndRect();
				m_pLeftHorisontalLine->SetWndPos(r.left, r.top + contractsOffset);
				m_pActiveSubdialog = &UIContractsWnd;
				SetContractTrader();
				ArticleCaption(*stbl(m_pActiveSubdialog->WindowName()));
				InventoryUtilities::SendInfoToActor("ui_pda_contracts");
				break;
*/
			case idNews:
				
				m_pActiveSubdialog = &UINewsWnd;
				ArticleCaption(*stbl(m_pActiveSubdialog->WindowName()));
				InventoryUtilities::SendInfoToActor("ui_pda_news");
				break;

			case idActorDiary:
				m_pActiveSubdialog = &UIActorDiaryWnd;
				InventoryUtilities::SendInfoToActor("ui_pda_diary");
				break;
			default:
				NODEFAULT;
			}


			if (m_pActiveSubdialog)
			{
				UIFrameWnd.AttachChild(m_pActiveSubdialog);
				m_pActiveSubdialog->Show(true);

				/*if (idActorDiary == id)
				{
					caption = static_cast<xr_string>(ALL_PDA_HEADER_PREFIX) +
						m_pActorDiaryRoot->GetHierarchyAsText() +
						*UIActorDiaryWnd.m_pCore->SetCurrentArtice(pTVItem);
					UIFrameWndHeader.UITitleText.SetText(caption.c_str());
					caption.erase(0, caption.find_last_of("/") + 1);
					ArticleCaption(caption.c_str());
				}*/
			}
		}
	}
	else if (pWnd == this && DIARY_RESET_PREV_ACTIVE_ITEM == msg)
	{
		pPrtevTVItem = NULL;
	}
	else if (pWnd == &UINewsWnd && DIARY_SET_NEWS_AS_UNREAD == msg)
	{
		MarkNewsAsRead(true);
	}

	inherited::SendMessage(pWnd, msg, pData);
}
//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::MarkNewsAsRead (bool status)
{
	R_ASSERT(m_pNews);
	m_pNews->MarkArticleAsRead(status);
	m_pNews->SetItemColor();
}
//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::SetContractTrader()
{
	CSE_Abstract* E = Level().Server->game->get_entity_from_eid(m_TraderID);
	CSE_ALifeTrader* pTrader = NULL;
	if(E) pTrader = smart_cast<CSE_ALifeTrader*>(E);

	if(pTrader)
	{
		CCharacterInfo character_info;
		if(pTrader->character_profile().size() )
		{				
			character_info.Load(pTrader->character_profile());
			character_info.InitSpecificCharacter(pTrader->specific_character());
			UIContractsWnd.UICharInfo.InitCharacter(&character_info);

			LPCSTR artefact_list_func	= READ_IF_EXISTS(pSettings,r_string,"artefacts_tasks", "script_func", "");
			if (!artefact_list_func || !xr_strlen(artefact_list_func)) {
				UIContractsWnd.UIListWnd.RemoveAll();
				return;
			}

			luabind::functor<LPCSTR> lua_function;
			bool functor_exists = ai().script_engine().functor(artefact_list_func ,lua_function);
			if (!functor_exists) {
				UIContractsWnd.UIListWnd.RemoveAll();
				return;
			}

			R_ASSERT3(functor_exists, "Cannot find artefacts tasks func", artefact_list_func);
			LPCSTR artefact_list = lua_function	(NULL, m_TraderID);
			UIContractsWnd.UIListWnd.RemoveAll();
			if (artefact_list) {
				CUIString str;
				str.SetText(artefact_list);
//				CUIStatic::PreprocessText(str.m_str, UIContractsWnd.UIListWnd.GetItemWidth() - 5, UIContractsWnd.UIListWnd.GetFont());
				UIContractsWnd.UIListWnd.AddParsedItem<CUIListItem>(&str.m_str.front(), 0, UIContractsWnd.UIListWnd.GetTextColor());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::InitTreeView()
{
	CUITreeViewItem *pTVItem = NULL, *pTVItemSub = NULL;
	CStringTable stbl;

	// Jobs section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Jobs"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetReadedColor(m_uTreeRootColor);
//	pTVItem->SetUnreadedColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pJobsRoot = pTVItem;

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Current"));
	pTVItemSub->SetValue(idJobsCurrent);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetReadedColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);
	m_pActiveJobs = pTVItemSub;

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Accomplished"));
	pTVItemSub->SetValue(idJobsAccomplished);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetReadedColor(m_uTreeItemColor);
	pTVItemSub->SetUnreadedColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItemSub = xr_new<CUITreeViewItem>();
	pTVItemSub->SetText(*stbl("Failed"));
	pTVItemSub->SetValue(idJobsFailed);
	pTVItemSub->SetFont(m_pTreeItemFont);
	pTVItemSub->SetReadedColor(m_uTreeItemColor);
	pTVItem->AddItem(pTVItemSub);

	pTVItem->MarkArticleAsRead(true);
/*
	// Contracts section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Contracts"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetReadedColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pContractsTreeItem = pTVItem;
	pTVItem->MarkArticleAsRead(true);

	// Actor diary section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("Diary"));
	pTVItem->SetRoot(true);
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetReadedColor(m_uTreeRootColor);
	UITreeView.AddItem(pTVItem);
	m_pActorDiaryRoot = pTVItem;
	pTVItem->MarkArticleAsRead(true);
*/
	// News section
	pTVItem = xr_new<CUITreeViewItem>();
	pTVItem->SetText(*stbl("News"));
	pTVItem->SetFont(m_pTreeRootFont);
	pTVItem->SetReadedColor(m_uTreeRootColor);
	pTVItem->SetValue(idNews);
	UITreeView.AddItem(pTVItem);
	pTVItem->MarkArticleAsRead(true);
	pTVItem->SetManualSetColor(true);
	m_pNews = pTVItem;
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::Show(bool status)
{
	inherited::Show(status);
	if (status)
	{
		InitDiary();
	}
	else
		InventoryUtilities::SendInfoToActor("ui_pda_diary_hide");

	if (m_pActiveSubdialog) m_pActiveSubdialog->Show(status);
}

void CUIDiaryWnd::InitDiary()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;
/*
	m_pContractsTreeItem->Close();
	m_pContractsTreeItem->DeleteAllSubItems();

	if(pActor->contacts_registry->registry().objects_ptr())
	{
		const TALK_CONTACT_VECTOR& contacts = *pActor->contacts_registry->registry().objects_ptr();
		for(TALK_CONTACT_VECTOR::const_iterator it = contacts.begin();
			contacts.end() != it; it++)
		{
			CSE_Abstract* E = Level().Server->game->get_entity_from_eid((*it).id);
			CSE_ALifeTrader* pTrader = NULL;
			if(E) pTrader = smart_cast<CSE_ALifeTrader*>(E);

			if(pTrader)
			{
				CCharacterInfo character_info;
				character_info.Load(pTrader->character_profile());
				character_info.InitSpecificCharacter(pTrader->specific_character());

				CUITreeViewItem* pTVItemSub = xr_new<CUITreeViewItem>();
				pTVItemSub->SetText(character_info.Name());
				m_TraderID = (*it).id;
				pTVItemSub->SetValue(idContracts);
				pTVItemSub->SetFont(m_pTreeItemFont);
				pTVItemSub->SetTextColor(m_uTreeItemColor);
				m_pContractsTreeItem->AddItem(pTVItemSub);
			}
		}
	}
*/
	ReloadArticles();
}

//////////////////////////////////////////////////////////////////////////
void  CUIDiaryWnd::ReloadArticles()
{
	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	// Тут добавляются записи в дневник игрока
//	UIActorDiaryWnd.DeleteArticles(m_pActorDiaryRoot);

	if(pActor && pActor->encyclopedia_registry->registry().objects_ptr())
	{
		for(ARTICLE_VECTOR::const_iterator it = pActor->encyclopedia_registry->registry().objects_ptr()->begin();
			it != pActor->encyclopedia_registry->registry().objects_ptr()->end(); it++)
		{
			if (ARTICLE_DATA::eDiaryArticle == it->article_type)
			{
				UIActorDiaryWnd.AddArticle((*it).article_id, true);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::ArticleCaption(LPCSTR caption)
{
	UIArticleCaption.SetText(caption);
}

//////////////////////////////////////////////////////////////////////////

void CUIDiaryWnd::SetActiveSubdialog(EPdaSections section)
{
	switch (section)
	{
	case epsActiveJobs:
		if (!m_pJobsRoot->IsOpened())
			UITreeView.SendMessage(m_pJobsRoot, BUTTON_CLICKED, NULL);
		SendMessage(this, DIARY_RESET_PREV_ACTIVE_ITEM, NULL);
		UITreeView.SendMessage(m_pActiveJobs, BUTTON_CLICKED, NULL);
		break;
	case epsDiaryArticle:
//		if (!m_pJobsRoot->IsOpened())
//			UITreeView.SendMessage(m_pJobsRoot, BUTTON_CLICKED, NULL);
//		SendMessage(this, DIARY_RESET_PREV_ACTIVE_ITEM, NULL);


//.		UITreeView.SendMessage(m_pActorDiaryRoot, BUTTON_CLICKED, NULL);
		break;
	default:
		NODEFAULT;
	}
}

void CUIDiaryWnd::OpenDiaryTree(ARTICLE_ID id) 
{ 
	UIActorDiaryWnd.m_pCore->OpenTree(id); 
}
