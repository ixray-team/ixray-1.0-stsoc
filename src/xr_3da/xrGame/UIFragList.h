#ifndef __XR_UIFRAGLIST_H__
#define __XR_UIFRAGLIST_H__
#pragma once

#include "uilistbox.h"

class CUIFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUIFragList		();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UIFRAGLIST_H__
