// EffectorNoise.h: interface for the CEffectorNoise class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORNOISE_H__E8300C01_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)
#define AFX_EFFECTORNOISE_H__E8300C01_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_

#pragma once

#include "../Effector.h"

class CEffectorNoise : public CEffector
{
	float	fForce;
	float	fStepTime;
	Fvector	vStep;
public:
	virtual	BOOL	Process(Fvector &p, Fvector &d, Fvector &n);

	CEffectorNoise(float _fForce, float life_time);
	virtual ~CEffectorNoise();
};

#endif // !defined(AFX_EFFECTORNOISE_H__E8300C01_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)
