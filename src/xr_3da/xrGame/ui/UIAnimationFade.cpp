//=============================================================================
//  Filename:   UIAnimationFade.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ƒл€ анимаци€ плавного спадани€/увеличени€ значени€
//=============================================================================

#include "stdafx.h"
#include "UIAnimationFade.h"

//////////////////////////////////////////////////////////////////////////

CUIAnimationFade::CUIAnimationFade()
	:	m_eCurrFadeDirection	(efdFadeIn),
		m_FadeBounds			(0, 255)		// Default values for alpha channel
{
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationFade::Update()
{
	if (easPlayed == m_eAnimationState)
	{
		int tmpTime	= m_iCurrAnimationTime + static_cast<int>(Device.dwTimeGlobal - m_uPrevGlobalTime);

		if (tmpTime >= m_iAnimationPeriod / 2)
		{
			SwitchFadeDirection();
			tmpTime %= m_iAnimationPeriod / 2;
			if (0 == tmpTime) tmpTime = 1;
		}

		switch (m_eCurrFadeDirection)
		{
		case efdFadeIn:
			m_iPhase = static_cast<int>((m_FadeBounds.second - m_FadeBounds.first) * (tmpTime * 2.0f / m_iAnimationPeriod) + m_FadeBounds.first);
			break;
		case efdFadeOut:
			m_iPhase = static_cast<int>((m_FadeBounds.second - m_FadeBounds.first) * (1 - tmpTime * 2.0f / m_iAnimationPeriod) + m_FadeBounds.first) + 1;
			break;
		default:
			NODEFAULT;
		}

		m_iCurrAnimationTime += static_cast<int>(Device.dwTimeGlobal - m_uPrevGlobalTime);
		m_iCurrAnimationTime %= m_iAnimationPeriod / 2;

	}

	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationFade::SwitchFadeDirection()
{
	if (efdFadeIn == m_eCurrFadeDirection) m_eCurrFadeDirection = efdFadeOut;
	else m_eCurrFadeDirection = efdFadeIn;
}