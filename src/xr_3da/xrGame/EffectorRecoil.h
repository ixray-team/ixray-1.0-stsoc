// EffectorRecoil.h: interface for the CEffectorRecoil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORRECOIL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)
#define AFX_EFFECTORRECOIL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_
#pragma once

#include "../Effector.h"

#define Recoil_TIME .5f

// Отдача оружия или попадание
class CEffectorRecoil : public CEffector
{
	float	fTime;
	float	fPower;
public:
	virtual	BOOL	Process(Fvector &p, Fvector &d, Fvector &n);

	CEffectorRecoil(float power, float life_time);
	virtual ~CEffectorRecoil();
};

#endif // !defined(AFX_EFFECTORRECOIL_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)
