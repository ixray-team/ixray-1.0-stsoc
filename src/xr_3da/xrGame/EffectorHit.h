#pragma once


#include "../effector.h"
class CEffectorHit :
	public CEffector
{
	float	m_x, m_y, fTime, fLifeTime;
public:
	CEffectorHit(float x, float y, float time, float force);
	virtual ~CEffectorHit(void);
	virtual	BOOL	Process		(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
};
