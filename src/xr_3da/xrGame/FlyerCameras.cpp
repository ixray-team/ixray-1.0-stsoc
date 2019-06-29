#include "stdafx.h"
#include "Flyer.h"
#include "../CameraBase.h"

void CFlyer::UpdateCamera()
{
	Fvector point, dangle;
	
	// get calc eye point 90% from player height
	Fvector						R;
	m_PhysicMovementControl->Box().getsize		(R);
	point.set					(0.f,R.y*0.55f,0.9f);
	XFORM().transform_tiny	(point);

	dangle.set(0,0,0);
	switch (cam_active){
	case efcFrontView:	break;
	case efcLookAt:		break;
	case efcFreeLook:	break;
	default: NODEFAULT;
	}
	CCameraBase* C				= cameras[cam_active];
	C->Update					(point,dangle);
	g_pGameLevel->Cameras().Update	(C);
}
 
void CFlyer::ChangeCamStyle	(EFlyerCameras style){
	CCameraBase* old_cam = cameras[cam_active];
	cam_active = style;
	CCameraBase* new_cam = cameras[cam_active];
	old_cam->OnDeactivate();
	new_cam->OnActivate(old_cam);
	switch(cam_active){
	case efcFrontView:	bVisible = true; 	new_cam->yaw=0;	break;
	case efcLookAt:		bVisible = true;	break;
	case efcFreeLook:	bVisible = true;	break;
	}
}
