#include "stdafx.h"
#include "EffectorPPHit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorPPHit::CEffectorPPHit(float x, float y, float time, float force) : CEffectorPP(cefppHit,time)
{
 	fTime = fLifeTime = time;
	m_x = x*force; m_y = y*force;


}

CEffectorPPHit::~CEffectorPPHit	()
{
}

BOOL CEffectorPPHit::Process	(SPPInfo& pp)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;
	float k = fLifeTime/fTime; k = k*k*k*k;
	pp.duality.h	= m_x * k;
	pp.duality.v	= m_y * k;
	pp.noise.fps=10;
	pp.noise.grain=0.01f;
	pp.color_base.r = pp.color_base.g = pp.color_base.b = .5f;
	return TRUE;
}
