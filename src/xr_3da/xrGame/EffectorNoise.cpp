// EffectorNoise.cpp: implementation of the CEffectorNoise class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorNoise.h"

static __forceinline float rnd_noise(void) { return float(rand())/float(RAND_MAX) - .5f; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorNoise::CEffectorNoise(float _fForce,float life_time) : CEffector(cefNoise,life_time,TRUE)
{
	fForce		= _fForce;
	fStepTime	= -1.f;
}

CEffectorNoise::~CEffectorNoise()
{
}

BOOL CEffectorNoise::Process(Fvector &p, Fvector &d, Fvector &n)
{
	fStepTime-=Device.fTimeDelta;
	if (fStepTime<0) {
		// select _new_ step
		vStep.set(rnd_noise(),rnd_noise(),rnd_noise());
		vStep.mul(rnd_noise()*fForce);
		fStepTime=_abs(rnd_noise())*.03f;
	}
	p.add(vStep);
	fLifeTime-=Device.fTimeDelta;
	return TRUE;
}
