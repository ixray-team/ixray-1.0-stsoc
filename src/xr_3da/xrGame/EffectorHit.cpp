#include "stdafx.h"
#include "effectorhit.h"

CEffectorHit::CEffectorHit(float x, float y, float time, float force) : CEffector(cefHit, time, FALSE) {
	fTime = fLifeTime = time;
	m_x = x*force; m_y = y*force;
}

CEffectorHit::~CEffectorHit(void) {
}

BOOL CEffectorHit::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect) {
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	Fmatrix		M;
	M.identity	();
	M.j.set		(n);
	M.k.set		(d);
	M.i.crossproduct(n,d);
	M.c.set		(p);

	Fvector dangle; float k = fLifeTime/fTime; k = k*k*k*k;
	dangle.x = m_x * k;
	dangle.y = m_y * k;
	dangle.z = dangle.x /*+ dangle.y*/;

	Fmatrix		R;
	R.setHPB	(dangle.x,dangle.y,dangle.z);

	Fmatrix		mR;
	mR.mul		(M,R);
	
	d.set		(mR.k);
	n.set		(mR.j);

	return TRUE;
}
