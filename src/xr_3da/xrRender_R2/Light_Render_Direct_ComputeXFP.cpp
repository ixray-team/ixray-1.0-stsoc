#include "stdafx.h"
#include "light_render_direct.h"
#include "..\igame_persistent.h"
#include "..\environment.h"

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static	Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static	Fvector cmDir[6]	= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};

extern	void ComputeFrustum	(Fvector* _F, float p_FOV, float p_A, float p_FAR, Fvector& camD, Fvector& camN, Fvector& camR, Fvector& camP);

BOOL CLight_Compute_XFORM_and_VIS::compute_xf_omni	(u32 m_phase, light* L)
{
	Fmatrix						m_ViewROT, m_invViewROT,m_invLET;
	Fvector						t;
	m_ViewROT.build_camera_dir	(t.set(0,0,0),Device.vCameraDirection,Device.vCameraTop);
	m_invViewROT.invert			(m_ViewROT);
	Device.mView.transform_tiny	(t,L->position);
	m_invLET.translate			(t.invert());
	L->X.P.world.mul			(m_invViewROT,m_invLET);

	// Build EYE-space xform
	Fvector						L_dir,L_up,L_right,L_pos;
	L_dir=cmDir	[m_phase];							L_dir.normalize		();
	L_up=cmNorm	[m_phase];							L_up.normalize		();
	L_right.crossproduct			(L_up,L_dir);	L_right.normalize	();
	L_pos.set						(L->position);
	L->X.P.view.build_camera_dir	(L_pos,L_dir,L_up);
	L->X.P.project.build_projection	(deg2rad(90.f),1.f,SMAP_near_plane,L->range+EPS_S);
	L->X.P.combine.mul				(L->X.P.project,L->X.P.view);

	// Quick exit for frustum-frustum intersection
	// If the center of light lies inside, all frustums will cause intersection
	if (RImplementation.ViewBase.testSphere_dirty(L->position,EPS_S))	return			TRUE;

	// Test for visibility of result
	{
		Fvector		points			[6];
		ComputeFrustum				(points,deg2rad(90.f),1.f,L->range+EPS_S,L_dir,L_up,L_right,L_pos);
		CFrustum	fTest;			fTest.CreateFromMatrix			(L->X.P.combine,FRUSTUM_P_LRTB|FRUSTUM_P_NEAR);
		sPoly	sSrc(points,4), sDest;
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[0]);sSrc.push_back(points[1]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[1]);sSrc.push_back(points[2]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[2]);sSrc.push_back(points[3]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[3]);sSrc.push_back(points[0]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
	}

	// The last possibility - our main frustum is totally inside test frustum
	{
		Fvector		points			[6];
		float		p_FOV			= Device.fFOV;
		float		p_DIST			= g_pGamePersistent->Environment.CurrentEnv.far_plane;
		ComputeFrustum				(points,p_FOV,1.f,p_DIST,Device.vCameraDirection,Device.vCameraTop,Device.vCameraRight,Device.vCameraPosition);
		CFrustum	fTest;			fTest.CreateFromMatrix			(L->X.P.combine,FRUSTUM_P_LRTB|FRUSTUM_P_NEAR);
		sPoly		sSrc(points,4), sDest;
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[0]);sSrc.push_back(points[1]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[1]);sSrc.push_back(points[2]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[2]);sSrc.push_back(points[3]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
		sSrc.clear();sDest.clear();sSrc.push_back(L_pos);sSrc.push_back(points[3]);sSrc.push_back(points[0]);
		if (0!=fTest.ClipPoly(sSrc, sDest))		return TRUE;
	}

	return FALSE;
}
