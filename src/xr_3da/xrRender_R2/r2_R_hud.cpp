#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
using namespace SceneGraph;

void __fastcall hud_L1		(mapSorted_Node *N)
{
	IRender_Visual *V = N->val.pVisual;
	RCache.set_Shader		(V->hShader);
	RCache.set_xform_world	(N->val.Matrix);
	V->Render				(1);
}

void CRender::render_hud()
{
	ENGINE_API extern float		psHUD_FOV;

	// Change projection
	Fmatrix Pold				= Device.mProject;
	Fmatrix FTold				= Device.mFullTransform;
	Device.mProject.build_projection(
		deg2rad(psHUD_FOV*Device.fFOV*Device.fASPECT), 
		Device.fASPECT, VIEWPORT_NEAR, 
		g_pGamePersistent->Environment.CurrentEnv.far_plane);
	Device.mFullTransform.mul	(Device.mProject, Device.mView);
	RCache.set_xform_project	(Device.mProject);

	// Rendering
	rmNear						();
	mapHUD.traverseLR			(hud_L1);
	mapHUD.clear				();
	rmNormal					();

	// Restore projection
	Device.mProject				= Pold;
	Device.mFullTransform		= FTold;
	RCache.set_xform_project	(Device.mProject);
}
