#error 111

#pragma once

#include "ui/UIStatsWnd.h"

struct	game_PlayerState;
class CUIDMFragList:public CUIStatsWnd
{
	typedef CUIStatsWnd inherited;

protected:
	DEFINE_VECTOR	(game_PlayerState*,ItemVec,ItemIt);
	ItemVec			items;

public:
	CUIDMFragList					();
	~CUIDMFragList					();

//	virtual void Show				();

	virtual void Update				();

	virtual u32						GetItemCount	()	{return UIStatsList.GetSize();};
	virtual	CUIStatsListItem*		GetItem			(int index);
	virtual bool					SetItemData		(u32 ItemID, CUIStatsListItem *pItem);


	virtual void					UpdateItemsList ();

//	void			Init			();
//	virtual void	OnFrame			();
};


/*

//#include "uilistbox.h"

class CUIDMFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUIDMFragList	();
	void			Init			();
	virtual void	OnFrame			();
};
*/
