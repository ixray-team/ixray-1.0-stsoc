//=============================================================================
//  Filename:   UIAnimationFlicker.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Анимация мигания
//=============================================================================

#ifndef UI_ANIMATION_FLICKER_H_
#define UI_ANIMATION_FLICKER_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIAnimationBase.h"

//////////////////////////////////////////////////////////////////////////

class CUIAnimationFlicker: public CUIAnimationBase
{
private:
	typedef CUIAnimationBase inherited;
public:
	CUIAnimationFlicker() {}
	virtual				~CUIAnimationFlicker() {}

	// Update
	virtual void		Update();
};

#endif	// UI_ANIMATION_FLICKER_H_