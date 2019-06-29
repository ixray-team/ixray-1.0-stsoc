#ifndef __XR_UICSPLAYERLIST_H__
#define __XR_UICSPLAYERLIST_H__
#pragma once

#include "uiListBox.h"

class CUICSPlayerList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUICSPlayerList	();
	void			Init			();
	virtual void	OnFrame			();
};

#endif //__XR_UIDMPLAYERLIST_H__
