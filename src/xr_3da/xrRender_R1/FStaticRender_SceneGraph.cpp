#include "stdafx.h"

#include "..\fhierrarhyvisual.h"
#include "..\SkeletonCustom.h"
#include "..\fmesh.h"
#include "..\fcached.h"
#include "..\flod.h"
#include "..\irenderable.h"

#include "particlegroup.h"

using	namespace SceneGraph;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
float r_ssaDISCARD;
float r_ssaDONTSORT;
float r_ssaLOD_A;
float r_ssaLOD_B;
float r_ssaHZBvsTEX;

IC	float	CalcSSA				(float& distSQ, Fvector& C, IRender_Visual* V)
{
	float R	= V->vis.sphere.R;
	distSQ	= Device.vCameraPosition.distance_to_sqr(C);
	return	R*R/distSQ;
}

void CRender::InsertSG_Dynamic	(IRender_Visual *pVisual, Fvector& Center)
{
	float distSQ;	if (CalcSSA(distSQ,Center,pVisual)<=r_ssaDISCARD)	return;

	// Select List and add to it
	ShaderElement*		sh		= (L_Projector->shadowing()?pVisual->hShader->E[0]:pVisual->hShader->E[1])._get();
	if (val_bInvisible)	{
		mapMatrixItem::TNode C;
		C.val.pObject			= val_pObject;
		C.val.pVisual			= pVisual;
		C.val.Matrix			= *val_pTransform;
		C.val.vCenter.set		(Center);
		L_Shadows->add_element	(&C);
	}else if (val_bHUD)	{
		mapHUD_Node* N			= mapHUD.insertInAnyWay(distSQ);
		N->val.pObject			= val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
	} else if (sh->Flags.bStrictB2F) {
		mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
		if (val_pObject)		VERIFY	(val_pObject->renderable.ROS);
		N->val.pObject			= val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
		L_Shadows->add_element	(N);
	} else {
		mapMatrix_Node* N		= mapMatrix.insert		(sh		);
		mapMatrixItem::TNode* C	= N->val.insertInAnyWay	(distSQ	);
		C->val.pObject			= val_pObject;
		C->val.pVisual			= pVisual;
		C->val.Matrix			= *val_pTransform;
		C->val.vCenter.set		(Center);
		L_Shadows->add_element	(C);
	}
}

void CRender::InsertSG_Static(IRender_Visual *pVisual)
{
	if (pVisual->vis.frame != Device.dwFrame) 
	{
		pVisual->vis.frame = Device.dwFrame;
		
		float distSQ;
		float SSA    = CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);

		if (SSA<=r_ssaDISCARD)		return;

		// Select List and add to it
		ShaderElement*		sh		= (((_sqrt(distSQ)-pVisual->vis.sphere.R)<20)?pVisual->hShader->E[0]:pVisual->hShader->E[1])._get();
		if (sh->Flags.bStrictB2F) {
			mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
			N->val.pObject			= NULL;
			N->val.pVisual			= pVisual;
			N->val.Matrix			= Fidentity;
			N->val.vCenter.set		(pVisual->vis.sphere.P);
		} else {
			for (u32 pass_id=0; pass_id<sh->Passes.size(); pass_id++)
			{
				SPass&						pass	= *(sh->Passes[pass_id]);
				mapNormalCodes&				codes	= mapNormal			[sh->Flags.iPriority/2][pass_id];
				mapNormalCodes::TNode*		Ncode	= codes.insert		(pass.state->state);
				mapNormalVS::TNode*			Nvs		= Ncode->val.insert	(pass.vs->vs);
				mapNormalPS::TNode*			Nps		= Nvs->val.insert	(pass.ps->ps);
				mapNormalCS::TNode*			Ncs		= Nps->val.insert	(pass.constants._get());
				mapNormalTextures::TNode*	Ntex	= Ncs->val.insert	(pass.T._get());
				mapNormalVB::TNode*			Nvb		= Ntex->val.insert	(pVisual->hGeom->vb);
				mapNormalMatrices::TNode*	Nmat	= Nvb->val.insert	(pass.M._get());
				mapNormalItems&				items	= Nmat->val;
				_NormalItem					item	= {SSA,pVisual};
				items.push_back						(item);
				if (0==pass_id) {
					// Need to sort for HZB efficient use
					if (SSA>Nmat->val.ssa) {
						Nmat->val.ssa = SSA;
						if (SSA>Nvb->val.ssa) {
							Nvb->val.ssa = SSA;
							if (SSA>Ntex->val.ssa) {
								Ntex->val.ssa = SSA;
								if (SSA>Ncs->val.ssa) {
									Ncs->val.ssa = SSA;
									if (SSA>Nps->val.ssa)	{
										Nps->val.ssa = SSA; 
										if (SSA>Nvs->val.ssa)	{
											Nvs->val.ssa = SSA; 
											if (SSA>Ncode->val.ssa) Ncode->val.ssa = SSA;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Dynamic(IRender_Visual *pVisual)
{
	// Visual is 100% visible - simply add it
	xr_vector<IRender_Visual*>::iterator I,E;	// it may be useful for 'hierrarhy' visual

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pV = (PS::CParticleGroup*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	default:
		{
			// General type of visual
			// Calculate distance to it's center
			Fvector		Tpos;
			val_pTransform->transform_tiny(Tpos, pVisual->vis.sphere.P);
			InsertSG_Dynamic(pVisual,Tpos);
		}
		return;
	}
}

void CRender::add_leafs_Static(IRender_Visual *pVisual)
{
	if (!HOM.visible(pVisual->vis))		return;

	// Visual is 100% visible - simply add it
	xr_vector<IRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pV = (PS::CParticleGroup*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Static	(*I);
		}
		return;
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Static (*I);
		}
		return;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Static	(*I);
		}
		return;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			if (ssa<r_ssaLOD_A)
			{
				mapLOD_Node*	N	= mapLOD.insertInAnyWay(D);
				N->val.ssa						= ssa;
				N->val.pVisual					= pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, doesn't perform any tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static (*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Static(pVisual);
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRender::add_Dynamic(IRender_Visual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	Fvector		Tpos;	// transformed position
	EFC_Visible	VIS;

	val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
	VIS = View->testSphere			(Tpos, pVisual->vis.sphere.R,planes);
	if (fcvNone==VIS) return FALSE;

	// If we get here visual is visible or partially visible
	xr_vector<IRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pV = (PS::CParticleGroup*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Dynamic(pVisual,Tpos);
		}
		break;
	}
	return TRUE;
}

void CRender::add_Static(IRender_Visual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
//	VIS = View->testSphere(pVisual->bv_Position,pVisual->bv_Radius,planes);
//	VIS = (View->testSphere_dirty(pVisual->bv_Position,pVisual->bv_Radius))?fcvFully:fcvNone;
//	VIS = View->testAABB(pVisual->bv_BBox.min,pVisual->bv_BBox.max,planes);
	vis_data&	vis			= pVisual->vis;
	VIS = View->testSAABB	(vis.sphere.P,vis.sphere.R,vis.box.min,vis.box.max,planes);
	if (fcvNone==VIS){
		return;
	}
	if (!HOM.visible(vis))	return;
	
	// If we get here visual is visible or partially visible
	xr_vector<IRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_PARTICLE_GROUP:
		{
			// Add all children, doesn't perform any tests
			PS::CParticleGroup* pV = (PS::CParticleGroup*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_SKELETON_ANIM:
	case MT_SKELETON_RIGID:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV	= (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			if (ssa<r_ssaLOD_A)	
			{
				mapLOD_Node*	N	= mapLOD.insertInAnyWay(D);
				N->val.ssa						= ssa;
				N->val.pVisual					= pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, perform tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Static(pVisual);
		}
		break;
	}
}
