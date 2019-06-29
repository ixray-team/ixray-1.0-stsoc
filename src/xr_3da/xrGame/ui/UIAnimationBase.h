//=============================================================================
//  Filename:   UIAnimationBase.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Абстрактный класс анимаций для различных потребностей интерфейса
//=============================================================================

#ifndef UI_ANIMATION_BASE_H_
#define UI_ANIMATION_BASE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIWindow.h"

//////////////////////////////////////////////////////////////////////////

class CUIAnimationBase
{
public:
	CUIAnimationBase();
	virtual ~CUIAnimationBase() = 0;

	// Функция которую необходимо обязательно поместить в Update диалога, для обновления анимации
	virtual void		Update();
	// Возвращаем текущее состояние анимации
	int					GetCurrentPhase() const			{ return m_iPhase; }
	// Установить параметры анимации
	// Params:
	// 1. period - период в милисекундах
	// 2. initialState - начальное состояние анимации
	void				SetAnimationPeriod(int period)	{ m_iAnimationPeriod = period; }
	void				SetCurrentPhase(int phase)		{ m_iPhase = phase; }

	// Состояние анимации
	enum EAnimationState
	{
		easPlayed,
		easStopped
	};
	// Функции управления анимацией
	void				Play();
	void				Stop();
	void				Reset()							{ m_uPrevGlobalTime = 0; m_uTotalTime = 0; m_iCurrAnimationTime = 0; }
	EAnimationState		GetState() const				{ return m_eAnimationState; }

	void				SetPlayDuration(int duration)	{ m_iAnimationDuration = duration; }
	void				SetMessageTarget(CUIWindow *pWnd) { m_pMessageTarget = pWnd; }

protected:
	// Период анимации
	int					m_iAnimationPeriod;
	// Предыдущее запомненное значение глобального времени
	int					m_uPrevGlobalTime;
	// Текущее время анимации
	int					m_iCurrAnimationTime;
	// Текущее фаза анимации
	int					m_iPhase;
	// Общее время проигрывания анимации для поддержки выключения по времени
	// Если -1 то играем зацикленно
	u32					m_uTotalTime;
	int					m_iAnimationDuration;
	// Состояние анимации
	EAnimationState		m_eAnimationState;

	CUIWindow			*m_pMessageTarget;
};

#endif