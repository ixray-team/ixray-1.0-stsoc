//=============================================================================
//  Filename:   UIActorDiary.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Диалог дневника актера
//=============================================================================

#include "stdafx.h"
#include "UIActorDiary.h"
#include "UIEncyclopediaCore.h"
#include "UIXmlInit.h"
#include "../UI.h"

//////////////////////////////////////////////////////////////////////////

const char * const		ACTOR_DIARY_XML		= "actor_diary.xml";

//////////////////////////////////////////////////////////////////////////

CUIActorDiaryWnd::CUIActorDiaryWnd()
	:	m_pCore		(NULL)
{
	SetWindowName("Actor Diary");
}

//////////////////////////////////////////////////////////////////////////

CUIActorDiaryWnd::~CUIActorDiaryWnd()
{
	xr_delete(m_pCore);
}

//////////////////////////////////////////////////////////////////////////

void CUIActorDiaryWnd::Init(CUIListWnd *idxList)
{
	R_ASSERT(idxList);

	m_pCore		= xr_new<CUIEncyclopediaCore>();

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, ACTOR_DIARY_XML);
	R_ASSERT3(xml_result, "xml file not found", ACTOR_DIARY_XML);

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	CUIXmlInit xml_init;

	UIInfoList = xr_new<CUIListWnd>(); UIInfoList->SetAutoDelete(true);
	AttachChild(UIInfoList);
	xml_init.InitListWnd(uiXml, "list", 0, UIInfoList);
	UIInfoList->ActivateList(false);
	UIInfoList->EnableScrollBar(true);
	UIInfoList->EnableActiveBackground(false);
	UIInfoList->SetNewRenderMethod(true);
	UIInfoList->SetMessageTarget(m_pCore);

	m_pCore->Init(UIInfoList, idxList);
}

//////////////////////////////////////////////////////////////////////////

void CUIActorDiaryWnd::ShowArticle(CUITreeViewItem *pItem)
{
	m_pCore->SetCurrentArtice(pItem);
}

//////////////////////////////////////////////////////////////////////////

void CUIActorDiaryWnd::DeleteArticles(CUITreeViewItem *pRoot)
{
	R_ASSERT(pRoot);
	pRoot->Close();
	m_pCore->DeleteArticles();
	UIInfoList->RemoveAll();
	pRoot->DeleteAllSubItems();
}
//////////////////////////////////////////////////////////////////////////

void CUIActorDiaryWnd::ReloadArticles		()
{

}
//////////////////////////////////////////////////////////////////////////

void CUIActorDiaryWnd::Show(bool status)
{
	inherited::Show(status);
	UIInfoList->Show(status);
	m_pCore->Show(status);
}
void CUIActorDiaryWnd::AddArticle(ARTICLE_ID id, bool bReaded)	
{ 
	m_pCore->AddArticle(id, bReaded); 
}
