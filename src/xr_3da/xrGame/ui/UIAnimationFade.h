//=============================================================================
//  Filename:   UIAnimationFade.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Для анимация плавного спадания/увеличения значения
//=============================================================================

#ifndef UI_ANIMATION_FADE_H_
#define UI_ANIMATION_FADE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <utility>
#include "UIAnimationBase.h"

//////////////////////////////////////////////////////////////////////////

class CUIAnimationFade: public CUIAnimationBase
{
	typedef	CUIAnimationBase inherited;
public:
	CUIAnimationFade();
	virtual				~CUIAnimationFade() {}
	// Update
	virtual void		Update();

	typedef				std::pair<int, int> FadeBounds;
	// Установить границы фейда
	void				SetFadeBounds(FadeBounds bounds) { VERIFY(bounds.first < bounds.second); m_FadeBounds = bounds; }

	// Направление фейда
	enum EFadeDirection
	{
		efdFadeIn = 0,
		efdFadeOut
	};

	// Настройка направления анимации
	void				SetAnimationDirection(EFadeDirection dir) { m_eCurrFadeDirection = dir; }

protected:
	// Границы фейда
	FadeBounds			m_FadeBounds;
	// Текущее направление фейда
	EFadeDirection		m_eCurrFadeDirection;
	// Сменить направление фейда
	void				SwitchFadeDirection();
};

#endif	//UI_ANIMATION_FADE_H_