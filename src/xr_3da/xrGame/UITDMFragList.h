#error 111
#pragma once

//#include "uilistbox.h"
#include "UIDMFragList.h"

class CUITDMFragList:public CUIDMFragList
{
	typedef CUIDMFragList inherited;
private:
	u8				m_CurTeam;
public:
	void				Init			(u8 Team);
	virtual void		UpdateItemsList	();
//	virtual void		Update();
};
