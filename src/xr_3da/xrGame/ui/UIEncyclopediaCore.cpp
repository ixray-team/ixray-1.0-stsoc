//=============================================================================
//  Filename:   CUIEncyclopediaCore.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс для представления ядра функциональности энциклопедии
//=============================================================================

#include "stdafx.h"
#include "UIEncyclopediaCore.h"
#include "UIIconedListItem.h"
#include "../UI.h"
#include "../encyclopedia_article.h"
#include "../string_table.h"
#include "../level.h"
#include "../HUDManager.h"
#include "../alife_registry_wrappers.h"
#include "../actor.h"
#include "UIXmlInit.h"
//////////////////////////////////////////////////////////////////////////

const char * const		ENCYCLOPEDIA_CORE_XML		= "encyclopedia.xml";
static int				MAX_PICTURE_WIDTH			= 0;
static const int		MIN_PICTURE_FRAME_WIDTH		= 64;
static const int		MIN_PICTURE_FRAME_HEIGHT	= 64;

//////////////////////////////////////////////////////////////////////////

CUIEncyclopediaCore::CUIEncyclopediaCore()
	:	m_pTreeItemFont			(NULL),
		m_pTreeRootFont			(NULL),
		m_uTreeRootColor		(0xffffffff),
		m_uTreeItemColor		(0xffffffff),
		m_pCurrArticle			(NULL),
		m_iCurrentInfoListPos	(0)
{
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::Init(CUIScrollView *infoList, CUIListWnd *idxList)
{
	R_ASSERT(infoList);
	R_ASSERT(idxList);

	pIdxList	= idxList;
	pInfoList	= infoList;
//.	pInfoList->SetRightIndention(static_cast<int>(10 * UI()->GetScaleX()));

	CUIXml		uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, ENCYCLOPEDIA_CORE_XML);
	R_ASSERT3(xml_result, "xml file not found", ENCYCLOPEDIA_CORE_XML);

	CUIXmlInit	xml_init;

	inherited::Init(0,0, UI_BASE_WIDTH,	UI_BASE_HEIGHT);

	// mask
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIImgMask);
	m_iCurrentInfoListPos = 0;
}

//////////////////////////////////////////////////////////////////////////

shared_str CUIEncyclopediaCore::SetCurrentArtice(CUITreeViewItem *pTVItem)
{
	R_ASSERT(pTVItem);

	pInfoList->ScrollToBegin();
	pInfoList->Clear();
	m_iCurrentInfoListPos = 0;

	// Удаляем текущую картинку
//.	if (m_pCurrArticle && m_pCurrArticle->data())
//.		pInfoList->DetachChild(&m_pCurrArticle->data()->image);

	// для начала проверим, что нажатый элемент не рутовый
	if (!pTVItem->IsRoot())
	{

		CUIEncyclopediaArticleWnd*	article_info = xr_new<CUIEncyclopediaArticleWnd>();article_info->SetAutoDelete(true);
		article_info->Init			("pda_events.xml","main_wnd:right_frame:task_descr_view:main_frame:scroll_view:objective_item");
		article_info->SetArticle	(m_ArticlesDB[pTVItem->GetValue()]->data());
		m_UITaskInfoWnd->AddWindow	(article_info);

/*
		// Image
		CUIStatic &img = m_ArticlesDB[pTVItem->GetValue()]->data()->image;
		img.SetWndPos(0, 0);
		Frect r = pInfoList->GetAbsoluteRect();

		img.SetClipRect(r);
		AdjustImagePos(img);
		pInfoList->AttachChild(&img);
		
		if( UIImgMask.GetParent() )
			UIImgMask.GetParent()->DetachChild(&UIImgMask);

		img.SetMask(&UIImgMask);

		// Добавляем текст
		CUIString str;
		str.SetText(*CStringTable()(m_ArticlesDB[pTVItem->GetValue()]->data()->text.c_str()));
		pInfoList->AddParsedItem<CUIListItem>(str, 0, 0xffffffff);

*/
		// Запоминаем текущий эдемент
		m_pCurrArticle = m_ArticlesDB[pTVItem->GetValue()];

		// Пометим как прочитанную
		if (!pTVItem->IsArticleReaded())
		{
			CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

			if(pActor && pActor->encyclopedia_registry->registry().objects_ptr())
			{
				for(ARTICLE_VECTOR::iterator it = pActor->encyclopedia_registry->registry().objects().begin();
					it != pActor->encyclopedia_registry->registry().objects().end(); it++)
				{
					if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type &&
						m_pCurrArticle->Id() == it->article_id)
					{
						it->readed = true;
						break;
					}
				}
			}
		}
	}

	return pTVItem->GetHierarchyAsText().c_str();
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::DeleteArticles()
{
	for(size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		xr_delete(m_ArticlesDB[i]);
	}
	m_ArticlesDB.clear();

//	m_pCurrArticle = NULL;
}

//////////////////////////////////////////////////////////////////////////
bool CUIEncyclopediaCore::HasArticle		(ARTICLE_ID article_id)
{
	for(std::size_t i = 0; i<m_ArticlesDB.size(); ++i)
	{
		if(m_ArticlesDB[i]->Id() == article_id) return true;
	}
	return false;
}

void CUIEncyclopediaCore::AddArticle(ARTICLE_ID article_id, bool bReaded)
{
	for(std::size_t i = 0; i<m_ArticlesDB.size(); i++)
	{
		if(m_ArticlesDB[i]->Id() == article_id) return;
	}

	// Добавляем элемент
	m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
	CEncyclopediaArticle*& a = m_ArticlesDB.back();
	a = xr_new<CEncyclopediaArticle>();
	a->Load(article_id);

	CUIStatic &img = a->data()->image;

	img.ClipperOn();
	img.Enable(false);

	// Теперь создаем иерархию вещи по заданному пути

	CreateTreeBranch(a->data()->group, a->data()->name, pIdxList, m_ArticlesDB.size() - 1, 
		m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, bReaded);
}

//////////////////////////////////////////////////////////////////////////
/*
void CUIEncyclopediaCore::AdjustImagePos(CUIStatic &s)
{
	// Выравниваем текстурку по центру листа
	s.SetWndPos(((pInfoList->GetWndRect().right - pInfoList->GetWndRect().left) - (s.GetWndRect().right - s.GetWndRect().left) - 15) / 2, s.GetWndRect().top);

	// Теперь добавляем в лист столько пустых айтемов, чтобы они полностью перекрыли нашу картинку
	int emptyItemsCnt = iCeil(s.GetWndRect().bottom / pInfoList->GetItemHeight());

	for (int i = 0; i < emptyItemsCnt; ++i)
	{
		pInfoList->AddItem<CUIListItem>("");
	}
}
*/
//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (pInfoList == pWnd && SCROLLBAR_VSCROLL == msg && m_pCurrArticle)
	{
		Frect r = m_pCurrArticle->data()->image.GetWndRect();
		m_pCurrArticle->data()->image.SetWndPos(r.left,
			r.top + (m_iCurrentInfoListPos - pInfoList->GetListPosition()) * pInfoList->GetItemHeight());
		m_iCurrentInfoListPos = pInfoList->GetListPosition();
//		Irect r = pItemImage->GetWndRect();
//		pItemImage->SetWndPos(r.left,
//			r.top + (m_iCurrentInfoListPos - pInfoList->GetListPosition()) * pInfoList->GetItemHeight());
//		m_iCurrentInfoListPos = pInfoList->GetListPosition();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIEncyclopediaCore::Show(bool status)
{
	if (!status)
	{
		if (m_pCurrArticle)
		{
			pInfoList->DetachChild(&m_pCurrArticle->data()->image);
			m_pCurrArticle->data()->image.SetWndPos(0, 0);

			m_pCurrArticle = NULL;
		}
		pInfoList->RemoveAll();
	}

	m_iCurrentInfoListPos = 0;
}

//////////////////////////////////////////////////////////////////////////
/*
void CUIEncyclopediaCore::OpenTree(ARTICLE_ID value)
{
	int itemVal = -1;
	for(std::size_t k = 0; k<m_ArticlesDB.size(); ++k)
	{
		if(m_ArticlesDB[k]->Id() == value)
		itemVal = (int)k;
	}

	R_ASSERT(pIdxList);
	// Find tree item

	CUITreeViewItem *pItem	= NULL;
	CUITreeViewItem *result	= NULL;

	for (int i = 0; i < pIdxList->GetSize(); ++i)
	{
		pItem = smart_cast<CUITreeViewItem*>(pIdxList->GetItem(i));
		if (pItem)
		{
			result = pItem->Find(itemVal);
//			result = pItem->Find(value);
			if (result) break;
		}
	}

	if (result)
	{
		OpenTree(result);
		pIdxList->SendMessage(result, BUTTON_CLICKED, NULL);
		result->SendMessage(result, BUTTON_CLICKED, NULL);
		pIdxList->ScrollToPos(result->GetIndex());
	}
}
*/
/*
void CUIEncyclopediaCore::OpenTree(CUITreeViewItem *pItem)
{
	R_ASSERT(pItem);
	if (pItem->GetOwner())
	{
		OpenTree(pItem->GetOwner());
	}

	if (pItem->IsRoot())
		pItem->Open();
}
*/