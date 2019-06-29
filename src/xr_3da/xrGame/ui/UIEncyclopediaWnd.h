//=============================================================================
//  Filename:   UIEncyclopediaWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#pragma once

#include "UIWindow.h"
#include "../encyclopedia_article_defs.h"

class CEncyclopediaArticle;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUIListWnd;
class CUIEncyclopediaCore;
class CUIScrollView;
class CUITreeViewItem;

class CUIEncyclopediaWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	enum {eNeedReload=(1<<0),};
	Flags16				m_flags;
	shared_str			m_InfosHeaderStr;
public:
						CUIEncyclopediaWnd			();
	virtual				~CUIEncyclopediaWnd			();

	virtual void		Init						();
	virtual void		Show						(bool status);
	virtual void		SendMessage					(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void		Draw						();

	void				AddArticle					(ARTICLE_ID, bool bReaded);
	void				DeleteArticles				();
	bool				HasArticle					(ARTICLE_ID);

	void				ReloadArticles				();
	virtual void		Reset						();
protected:
	u32					prevArticlesCount;
	// Элементы графического оформления
	CUIFrameWindow*		UIEncyclopediaIdxBkg;
	CUIFrameWindow*		UIEncyclopediaInfoBkg;
	CUIFrameLineWnd*	UIEncyclopediaIdxHeader;
	CUIFrameLineWnd*	UIEncyclopediaInfoHeader;
	CUIAnimatedStatic*	UIAnimation;
	CUIStatic*			UIArticleHeader;

	// Хранилище статей
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;
	ArticlesDB				m_ArticlesDB;
	CGameFont*				m_pTreeRootFont;
	u32						m_uTreeRootColor;
	CGameFont*				m_pTreeItemFont;
	u32						m_uTreeItemColor;

	CUIListWnd*				UIIdxList;
	CUIScrollView*			UIInfoList;

	shared_str				SetCurrentArtice(CUITreeViewItem *pTVItem);
};
