#include "stdafx.h"
#include "..\fhierrarhyvisual.h"
#include "..\SkeletonCustom.h"
#include "..\fmesh.h"

void CRender::RenderBox			(IRender_Sector* _S, Fbox& BB, int sh)
{
	CSector*	S			= (CSector*)_S;
	lstVisuals.clear		();
	lstVisuals.push_back	(S->root());
	
	for (u32 test=0; test<lstVisuals.size(); test++)
	{
		IRender_Visual*	V		= 	lstVisuals[test];
		
		// Visual is 100% visible - simply add it
		xr_vector<IRender_Visual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals
		
		switch (V->Type) {
		case MT_HIERRARHY:
			{
				// Add all children
				FHierrarhyVisual* pV = (FHierrarhyVisual*)V;
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IRender_Visual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
			case MT_SKELETON_ANIM:
			case MT_SKELETON_RIGID:
			{
				// Add all children
				CKinematics * pV = (CKinematics*)V;
				pV->Calculate			();
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IRender_Visual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
		default:
			{
				// Renderable visual
				ShaderElement* E	= V->hShader->E[sh]._get();
				for (u32 pass=0; pass<E->Passes.size(); pass++)
				{
					RCache.set_Element			(E,pass);
					V->Render					(-1.f);
				}
			}
			break;
		}
	}
}
