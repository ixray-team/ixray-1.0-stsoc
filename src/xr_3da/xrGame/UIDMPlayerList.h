#error 111
#pragma once

//#include "uiListBox.h"
#include "UIDMFragList.h"

class CUIDMPlayerList:public CUIDMFragList
{
	typedef CUIDMFragList inherited;
public:
	CUIDMPlayerList					();
	~CUIDMPlayerList				();

	virtual bool					SetItemData		(u32 ItemID, CUIStatsListItem *pItem);
};
