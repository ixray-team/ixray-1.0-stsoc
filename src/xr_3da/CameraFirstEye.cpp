#include "stdafx.h"
#pragma hdrstop

#include "CameraFirstEye.h"
#include "xr_level_controller.h"
#include "../xr_object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraFirstEye::CCameraFirstEye(CObject* p, CInifile* ini, LPCSTR section, u32 flags ) : CCameraBase(p, flags)
{
	style				= csFirstEye;

	lim_pitch			= ini->r_fvector2	(section,"lim_pitch");
	lim_yaw				= ini->r_fvector2	(section,"lim_yaw");
	rot_speed			= ini->r_fvector3	(section,"rot_speed");
	bClampPitch			= (0!=lim_pitch[0])||(0!=lim_pitch[1]);
	bClampYaw			= (0!=lim_yaw[0])||(0!=lim_yaw[1]);
	if (bClampPitch)	pitch = (lim_pitch[0]+lim_pitch[1])*0.5f;
	if (bClampYaw)		yaw	  = (lim_yaw[0]+lim_yaw[1])*0.5f;
}

CCameraFirstEye::~CCameraFirstEye()
{

}

void CCameraFirstEye::Update(Fvector& point, Fvector& noise_dangle)
{
	Fmatrix			mR, R;
	Fmatrix			rX, rY, rZ;
	rX.rotateX		(noise_dangle.x);
	rY.rotateY		(-noise_dangle.y);
	rZ.rotateZ		(noise_dangle.z);
	R.mul_43		(rY,rX);
	R.mulB_43		(rZ);
	
	mR.identity		();
	Fquaternion		Q;
	Q.rotationYawPitchRoll(roll,yaw,pitch);
	mR.rotation		(Q);
	mR.transpose	();
	mR.mulB			(R);
	
	vDirection.set	(mR.k);
	vNormal.set		(mR.j);

	if (m_Flags.is(flRelativeLink))	{
		parent->XFORM().transform_dir	(vDirection);
		parent->XFORM().transform_dir	(vNormal);
	}

	vPosition.set	(point);
}

void CCameraFirstEye::Move( int cmd, float val )
{
	if (bClampPitch)
	{
		while (pitch < lim_pitch[0])
			pitch += PI_MUL_2;
		while (pitch > lim_pitch[1])
			pitch -= PI_MUL_2;
	};
	switch (cmd){
	case kDOWN:		pitch	-= val?val:(rot_speed.y*Device.fTimeDelta);	break;
	case kUP:		pitch	+= val?val:(rot_speed.y*Device.fTimeDelta);	break;
	case kLEFT:		yaw		-= val?val:(rot_speed.x*Device.fTimeDelta);	break;
	case kRIGHT:	yaw		+= val?val:(rot_speed.x*Device.fTimeDelta);	break;
	}
	if (bClampYaw)		clamp(yaw,lim_yaw[0],lim_yaw[1]);
	if (bClampPitch)	clamp(pitch,lim_pitch[0],lim_pitch[1]);
}

void CCameraFirstEye::OnActivate( CCameraBase* old_cam )
{
	if (old_cam&&(m_Flags.is(flRelativeLink)==old_cam->m_Flags.is(flRelativeLink)))
		yaw = (old_cam)->yaw;
}
