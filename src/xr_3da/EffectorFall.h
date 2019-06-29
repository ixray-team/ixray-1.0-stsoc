// EffectorFall.h: interface for the CEffectorFall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORFALL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)
#define AFX_EFFECTORFALL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_
#pragma once

#include "Effector.h"

// приседание после падения
class ENGINE_API CEffectorFall : public CEffector
{
	float	fPower;
	float	fPhase;
public:
	virtual	BOOL	Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);

	CEffectorFall(float power, float life_time=1);
	virtual ~CEffectorFall();
};

#endif // !defined(AFX_EFFECTORFALL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)
