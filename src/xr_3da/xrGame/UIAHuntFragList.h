#error 111
#pragma once

//#include "uilistbox.h"
#include "UITDMFragList.h"

class CUIAHuntFragList:public CUITDMFragList
{
	typedef CUITDMFragList inherited;
private:
//	u8				m_CurTeam;
public:
//	void				Init			(u8 Team);
//	virtual void		UpdateItemsList	();
//	virtual void		Update();
	virtual bool					SetItemData		(u32 ItemID, CUIStatsListItem *pItem);
};
