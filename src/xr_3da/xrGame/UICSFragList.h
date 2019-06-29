#ifndef __XR_UICSFRAGLIST_H__
#define __XR_UICSFRAGLIST_H__
#pragma once

#include "uilistbox.h"

class CUICSFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUICSFragList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UICSFRAGLIST_H__
