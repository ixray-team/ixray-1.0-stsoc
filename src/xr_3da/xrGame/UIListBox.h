#ifndef __XR_UILISTBOX_H__
#define __XR_UILISTBOX_H__
#pragma once

#include "uistaticitem.h"
#include "uiframerect.h"

class CUIListBox: public CUIFrameRect
{
protected:
	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
	ItemVec			items;
public:
					CUIListBox		();
	virtual void	Render			();
	virtual void	OnFrame			();
};

#endif //__XR_UILISTBOX_H__
