//=============================================================================
//  Filename:   UIAnimationBase.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Абстрактный класс анимаций для различных потребностей интерфейса
//=============================================================================

#include "stdafx.h"
#include "UIAnimationBase.h"

//////////////////////////////////////////////////////////////////////////

CUIAnimationBase::CUIAnimationBase()
	:	m_iAnimationPeriod		(0),
		m_iPhase				(0),
		m_uPrevGlobalTime		(0),
		m_eAnimationState		(easStopped),
		m_iCurrAnimationTime	(0),
		m_uTotalTime			(0),
		m_iAnimationDuration	(-1),
		m_pMessageTarget		(NULL)
{
}

//////////////////////////////////////////////////////////////////////////

CUIAnimationBase::~CUIAnimationBase()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationBase::Update()
{
	// Update time
	if (m_eAnimationState == easPlayed)
	{
		if (0 != m_uPrevGlobalTime)
		{
			m_uTotalTime += Device.dwTimeGlobal - m_uPrevGlobalTime;
		}

		if (-1 != m_iAnimationDuration && static_cast<int>(m_uTotalTime) > m_iAnimationDuration)
		{
			Stop();
			Reset();
		}
	}
	m_uPrevGlobalTime = Device.dwTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationBase::Play()
{
	if (0 == m_uPrevGlobalTime) m_uPrevGlobalTime = Device.dwTimeGlobal;
	m_eAnimationState = easPlayed;
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationBase::Stop()
{
	m_eAnimationState = easStopped;
	if (m_pMessageTarget)
	{
		m_pMessageTarget->SendMessage(NULL, ANIMATION_STOPPED, reinterpret_cast<void*>(this));
	}
}