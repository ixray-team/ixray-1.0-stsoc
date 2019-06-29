#include "stdafx.h"

void CRender::calc_DetailTexturing()
{
	// Get axis of cylinder
	//Fvector		vAxis	= Device.vCameraTop;
	//vAxis.invert		();
	// matDetailTexturing->xform.build_camera_dir(Device.vCameraPosition,vAxis,Device.vCameraDirection);
	Fmatrix		M;
	matDetailTexturing->xform.invert	(Device.mView);
	Fvector c_pos,c_dir,c_up;
	c_pos.set							(Device.vCameraPosition);
	c_dir.set							(Device.vCameraTop);
	c_up.set							(Device.vCameraDirection);
	M.build_camera_dir					(c_pos,c_dir,c_up);
	matDetailTexturing->xform.mulA_43	(M);
	M.scale								(.05f,	.05f,	.05f);
	matDetailTexturing->xform.mulA_43	(M);
	M.translate							(+1.f,	-1.f,	0	);
	matDetailTexturing->xform.mulA_43	(M);
	M.scale								(.5f,	-.5f,	0	);
	matDetailTexturing->xform.mulA_43	(M);

	/*
	Fvector&	V	= Device.vCameraPosition;
	M.translate	(-V.x,-V.y,-V.z);
	matDetailTexturing->xform.mulA_43	(M);
	M.mapZXY	();
	matDetailTexturing->xform.mulA_43	(M);
	*/

	/*
	Fmatrix		M,m;
	M.mapZXY	();				// ZXY, in camera space, so X=distance from plane
	m.identity	();

//	matDetailTexturing->xform.mul_43	(m,M);
	*/
}
