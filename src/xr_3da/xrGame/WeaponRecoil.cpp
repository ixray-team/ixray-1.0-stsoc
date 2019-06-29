// WeaponRecoil.cpp: класс, который реализует отдачу оружия
//					 (типа внешнего эффектора на камеру)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponRecoil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponRecoil::CWeaponRecoil(float max_angle, float relax_time)
{
	fRelaxSpeed		= _abs(relax_time);
	fAngleCurrent	= -EPS_S;
	fMaxAngle		= _abs(max_angle);
	bActive			= FALSE;
}

CWeaponRecoil::~CWeaponRecoil	()
{
}

void CWeaponRecoil::Shot(float angle)
{
	fAngleCurrent	+= (angle*.75f+::Random.randF(-1,1)*angle*.25f);
	clamp			(fAngleCurrent,-fMaxAngle,fMaxAngle);
//	float r			= (::Random.randF()>0.75f)?
//					  (fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
//	vDispersionDir.set(r,1.f,r); 
//	vDispersionDir.normalize_safe();

	fAngleNoiseH	= (::Random.randF()>0.75f)?
					  fAngleCurrent*(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;
	fAngleNoiseP	= (::Random.randF()>0.75f)?
					  fAngleCurrent*(fAngleCurrent/fMaxAngle)*::Random.randF(-1,1):0.f;


	bActive			= TRUE;
}

void CWeaponRecoil::Process(Fvector &dangle)
{
	if (bActive)
	{
		Fvector delta_d;
		//delta_d.mad					(vDispersionDir,tanf(fAngleCurrent));
//		delta_d.set(vDispersionDir);
//		delta_d.mul(tanf(fAngleCurrent));
		//delta_d.getHP(dangle.y,dangle.x); dangle.z=0.f;
		dangle.x= -fAngleCurrent + fAngleNoiseP;
		dangle.y= fAngleNoiseH;
		dangle.z= 0.f;

		if(fAngleNoiseH>0.f)
			fAngleNoiseH -= fRelaxSpeed*Device.fTimeDelta;
		if(fAngleNoiseH<0.f)
			fAngleNoiseH = 0.f;

		if(fAngleNoiseP>0.f)
			fAngleNoiseP -= fRelaxSpeed*Device.fTimeDelta;
		if(fAngleNoiseP<0.f)
			fAngleNoiseP = 0.f;

		if (fAngleCurrent>=0.f)
		{
			fAngleCurrent		-= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleCurrent<0.f)
			{
				fAngleCurrent	= 0.f;
				bActive			= FALSE;
			}
		}
		else
		{
			fAngleCurrent		+= fRelaxSpeed*Device.fTimeDelta;
			if (fAngleCurrent>0.f)
			{
				fAngleCurrent	= 0.f;
				bActive			= FALSE;
			}
		}
	}
	else
		dangle.set(0,0,0);
}