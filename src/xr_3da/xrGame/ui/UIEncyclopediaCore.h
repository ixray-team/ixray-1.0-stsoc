//=============================================================================
//  Filename:   CUIEncyclopediaCore.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс для представления ядра функциональности энциклопедии
//=============================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIWindow.h"
#include "UITreeViewItem.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CEncyclopediaArticle;
class CUIScrollView;
class CUIFrameWindow;

class CUIEncyclopediaCore: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
					CUIEncyclopediaCore();
	void			Init			(CUIScrollView *infoList, CUIListWnd *idxList);
	// Контролы и алгоритмы которые используются для вывода информации
	// Добавляем статью и возвращаем путь к ней
	shared_str		SetCurrentArtice(CUITreeViewItem *pTVItem);
	// Добавлем 1 энциклопедиционную статью
	void			AddArticle		(ARTICLE_ID, bool bReaded);
	void			DeleteArticles	();
	bool			HasArticle		(ARTICLE_ID article_id);
	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void	Show			(bool status);
	// Opens tree at item with specified value
//.	void			OpenTree		(ARTICLE_ID value);


	// Хранилище статей
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;

	CGameFont		*m_pTreeRootFont;
	u32				m_uTreeRootColor;
	CGameFont		*m_pTreeItemFont;
	u32				m_uTreeItemColor;

private:
//.	void			AdjustImagePos	(CUIStatic &s);
//.	void			OpenTree		(CUITreeViewItem *pItem);

	CUIListWnd*				pIdxList;
	CUIScrollView*			pInfoList;
	ArticlesDB				m_ArticlesDB;
	// Маска для изображения предмета текущей статьи
	CUIFrameWindow*			UIImgMask;
	
	// Текущая выбранная статья
	CEncyclopediaArticle*	m_pCurrArticle;
	CUIStatic*				pItemImage;

	// Текущая позиция листа с информацией
    int						m_iCurrentInfoListPos;
};
