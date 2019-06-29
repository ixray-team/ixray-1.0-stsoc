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
	if (pVisual->vis.frame == RImplementation.marker)	return;
	pVisual->vis.frame			= RImplementation.marker;

	float distSQ;
	float SSA    = CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Select List and add to it
	ShaderElement*		sh		= pVisual->hShader->E[RImplementation.phase]._get();
	if (val_bHUD)	{
		// HUD
		mapHUD_Node* N			= mapHUD.insertInAnyWay(distSQ);
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
	} else if (sh->Flags.bStrictB2F) {
		// Strict
		mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
	} else {
		// Normal
		SPass&						pass	= *sh->Passes.front();
		mapMatrix_T&				map		= mapMatrix;
		mapMatrixVS::TNode*			Nvs		= map.insert		(pass.vs->vs);
		mapMatrixPS::TNode*			Nps		= Nvs->val.insert	(pass.ps->ps);
		mapMatrixCS::TNode*			Ncs		= Nps->val.insert	(pass.constants._get());
		mapMatrixStates::TNode*		Nstate	= Ncs->val.insert	(pass.state->state);
		mapMatrixTextures::TNode*	Ntex	= Nstate->val.insert(pass.T._get());
		mapMatrixVB::TNode*			Nvb		= Ntex->val.insert	(pVisual->hGeom->vb);
		mapMatrixItems&				item	= Nvb->val;

		// Need to sort for HZB efficient use
		if (SSA>Nvb->val.ssa) {
			Nvb->val.ssa = SSA;
			if (SSA>Ntex->val.ssa) {
				Ntex->val.ssa = SSA;
				if (SSA>Nstate->val.ssa) {
					Nstate->val.ssa = SSA;
					if (SSA>Ncs->val.ssa)	{
						Ncs->val.ssa = SSA;
						if (SSA>Nps->val.ssa) {
							Nps->val.ssa = SSA;
							if (SSA>Nvs->val.ssa)	Nvs->val.ssa = SSA; 
						}
					}
				}
			}
		}

		if (SSA<r_ssaDONTSORT)	
		{
			item.unsorted.push_back			(_MatrixItem());
			item.unsorted.back().pVisual	= pVisual;
			item.unsorted.back().Matrix		= *val_pTransform;
			item.unsorted.back().vCenter.set(Center);
		}
		else					
		{
			FixedMAP<float,_MatrixItem>::TNode*	N	= item.sorted.insertInAnyWay	(distSQ);
			N->val.pVisual							= pVisual;
			N->val.Matrix							= *val_pTransform;
			N->val.vCenter.set						(Center);
		}
	}
}

void CRender::InsertSG_Static	(IRender_Visual *pVisual)
{
	if (pVisual->vis.frame == RImplementation.marker)	return;
	pVisual->vis.frame = RImplementation.marker;

	float distSQ;
	float SSA    = CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);
	if (SSA<=r_ssaDISCARD)		return;

	// Select List and add to it
	ShaderElement*		sh		= (pVisual->hShader->E[RImplementation.phase])._get();
	if (sh->Flags.bStrictB2F) {
		mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
		N->val.pVisual			= pVisual;
		N->val.Matrix			= Fidentity;
		N->val.vCenter.set		(pVisual->vis.sphere.P);
	} else {
		SPass&						pass	= *sh->Passes.front();
		mapNormal_T&				map		= mapNormal;		//	[sh->Flags.iPriority];
		mapNormalVS::TNode*			Nvs		= map.insert		(pass.vs->vs);
		mapNormalPS::TNode*			Nps		= Nvs->val.insert	(pass.ps->ps);
		mapNormalCS::TNode*			Ncs		= Nps->val.insert	(pass.constants._get());
		mapNormalStates::TNode*		Nstate	= Ncs->val.insert	(pass.state->state);
		mapNormalTextures::TNode*	Ntex	= Nstate->val.insert(pass.T._get());
		mapNormalVB::TNode*			Nvb		= Ntex->val.insert	(pVisual->hGeom->vb);
		mapNormalItems&				item	= Nvb->val;

		// Need to sort for HZB efficient use
		if (SSA>Nvb->val.ssa) {
			Nvb->val.ssa = SSA;
			if (SSA>Ntex->val.ssa) {
				Ntex->val.ssa = SSA;
				if (SSA>Nstate->val.ssa) {
					Nstate->val.ssa = SSA;
					if (SSA>Ncs->val.ssa)	{
						Ncs->val.ssa = SSA;
						if (SSA>Nps->val.ssa) {
							Nps->val.ssa = SSA;
							if (SSA>Nvs->val.ssa)	Nvs->val.ssa = SSA; 
						}
					}
				}
			}
		}

		if (SSA<r_ssaDONTSORT)		item.unsorted.push_back		(pVisual);
		else						item.sorted.insertInAnyWay	(distSQ,pVisual);
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
			E = pV->children.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_SKELETON:
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
	case MT_SKELETON:
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
			/*
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
			*/
				// Add all children, doesn't perform any tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static (*I);
			/*
			}
			*/
		}
		break;
	case MT_TREE:
		{
			FTreeVisual*	pV	= (FTreeVisual*) pVisual;
			val_pTransform		= &pV->xform;
			InsertSG_Dynamic	(pV,pV->vis.sphere.P);
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
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON:
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
	case MT_SKELETON:
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
			/*
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
			*/
				// Add all children, perform tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static	(*I);
			/*
			}
			*/
		}
		break;
	case MT_TREE:
		{
			FTreeVisual*	pV	= (FTreeVisual*) pVisual;
			val_pTransform		= &pV->xform;
			InsertSG_Dynamic	(pV,pV->vis.sphere.P);
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Static		(pVisual);
		}
		break;
	}
}
