//=============================================================================
//  Filename:   UIActorDiary.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Диалог дневника актера
//=============================================================================

#ifndef UI_ACTOR_DIARY_H_
#define UI_ACTOR_DIARY_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIWindow.h"
#include "../encyclopedia_article_defs.h"
//////////////////////////////////////////////////////////////////////////
class CUIListWnd;
class CUIEncyclopediaCore;
class CUITreeViewItem;

class CUIActorDiaryWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
					CUIActorDiaryWnd	();
	virtual			~CUIActorDiaryWnd	();
	void			Init				(CUIListWnd *idxList);

	void			AddArticle			(ARTICLE_ID id, bool bReaded);
	void			DeleteArticles		(CUITreeViewItem *pRoot);
	void			ShowArticle			(CUITreeViewItem *pItem);
	virtual void	Show				(bool status);

	void			ReloadArticles		();

	CUIEncyclopediaCore *m_pCore;
private:
	CUIListWnd*		UIInfoList;
};

//////////////////////////////////////////////////////////////////////////

#endif	// UI_ACTOR_DIARY_H_