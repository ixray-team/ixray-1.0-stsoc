//=============================================================================
//  Filename:   UIAnimationFlicker.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Анимация мигания
//=============================================================================

#include "stdafx.h"
#include "UIAnimationFlicker.h"

//////////////////////////////////////////////////////////////////////////

void CUIAnimationFlicker::Update()
{
	if (easPlayed == m_eAnimationState)
	{
		int tmpTime = m_iCurrAnimationTime;
		tmpTime		+= static_cast<int>(Device.dwTimeGlobal - m_uPrevGlobalTime);

		if ((tmpTime >= m_iAnimationPeriod / 2 && m_iCurrAnimationTime < m_iAnimationPeriod / 2) ||
			(tmpTime >= (m_iAnimationPeriod) && m_iCurrAnimationTime < m_iAnimationPeriod))
		{
			m_iPhase = (m_iPhase + 1) % 2;
		}

		m_iCurrAnimationTime = tmpTime % m_iAnimationPeriod;
	}

	inherited::Update();
}