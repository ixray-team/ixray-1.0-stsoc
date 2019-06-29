#ifndef _EFFECTOR_PP_HIT_H
#define _EFFECTOR_PP_HIT_H
#pragma once

#include "../effectorPP.h"

class CEffectorPPHit : public CEffectorPP
{
	float	m_x, m_y, fTime, fLifeTime;
public:
			CEffectorPPHit		(float x, float y, float time, float force);
	virtual ~CEffectorPPHit		();
	virtual	BOOL	Process		(SPPInfo& pp);
};

#endif //_EFFECTOR_PP_HIT_H
