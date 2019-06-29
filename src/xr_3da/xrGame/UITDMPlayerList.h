#error 111
#pragma once

//#include "uiListBox.h"
#include "UIDMPlayerList.h"

class CUITDMPlayerList:public CUIDMPlayerList
{
	typedef CUIDMPlayerList inherited;
private:
	u8				m_CurTeam;
public:
	void			Init			(u8 Team);
	virtual void	UpdateItemsList	();
};
