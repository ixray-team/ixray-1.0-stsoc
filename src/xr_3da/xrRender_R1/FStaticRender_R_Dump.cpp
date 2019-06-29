#include "stdafx.h"

using	namespace SceneGraph;

void	CRender::r_dump		(u32	_priority)
{
	if (0==mapNormal[_priority][0].size())	return;

	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	RCache.set_xform_world			(Fidentity);

	// Sorting by SSA and changes minimizations
	for (u32 pass_id=0; pass_id<SHADER_PASSES_MAX; pass_id++)	
	{
		mapNormalCodes&		codes		= mapNormal	[_priority][pass_id];
		if (0==codes.size())			break;
		BOOL sort						= (pass_id==0);

		codes.getANY_P		(lstCodes);
		if (sort) std::sort	(lstCodes.begin(), lstCodes.end(), cmp_codes);
		for (u32 code_id=0; code_id<lstCodes.size(); code_id++)
		{
			mapNormalCodes::TNode*	Ncode	= lstCodes[code_id];
			RCache.set_States				(Ncode->key);	

			mapNormalVS&	vs				= Ncode->val;	vs.ssa	= 0;
			vs.getANY_P		(lstVS);	if (sort)		std::sort	(lstVS.begin(),lstVS.end(),cmp_vs);
			for (u32 vs_id=0; vs_id<lstVS.size(); vs_id++)
			{
				mapNormalVS::TNode*		Nvs		= lstVS[vs_id];
				RCache.set_VS						(Nvs->key);	

				mapNormalPS&		ps			= Nvs->val;		ps.ssa	= 0;
				ps.getANY_P						(lstPS);
				std::sort						(lstPS.begin(), lstPS.end(), cmp_ps_nrm);

				for (u32 ps_id=0; ps_id<lstPS.size(); ps_id++)
				{
					mapNormalPS::TNode*	Nps			= lstPS[ps_id];
					RCache.set_PS					(Nps->key);	

					mapNormalCS&	cs			= Nps->val;		cs.ssa	= 0;
					cs.getANY_P		(lstCS);	if (sort)		std::sort	(lstCS.begin(),lstCS.end(),cmp_cs);
					for (u32 cs_id=0; cs_id<lstCS.size(); cs_id++)
					{
						mapNormalCS::TNode*	Ncs	= lstCS[cs_id];
						RCache.set_xform_world					(Fidentity);
						RCache.set_Constants					(Ncs->key);

						mapNormalTextures&	tex			= Ncs->val;		tex.ssa	= 0;
						sort_tlist			(lstTextures,lstTexturesTemp,tex,sort);
						for (u32 tex_id=0; tex_id<lstTextures.size(); tex_id++)
						{
							mapNormalTextures::TNode*	Ntex = lstTextures[tex_id];
							RCache.set_Textures						(Ntex->key);	

							mapNormalVB&		vb		= Ntex->val;		vb.ssa	= 0;
							vb.getANY_P		(lstVB);	if (sort)	std::sort	(lstVB.begin(),lstVB.end(),cmp_vb);
							for (u32 vb_id=0; vb_id<lstVB.size(); vb_id++)
							{
								mapNormalVB::TNode*		Nvb = lstVB[vb_id];
								// RCache.set_Vertices					(Nvb->key);	

								mapNormalMatrices&	mat		= Nvb->val;			mat.ssa	= 0;
								mat.getANY_P	(lstMatrices);	if (sort)	std::sort	(lstMatrices.begin(),lstMatrices.end(),cmp_matrices);
								for (u32 mat_id=0; mat_id<lstMatrices.size(); mat_id++)
								{
									mapNormalMatrices::TNode*	Nmat	= lstMatrices[mat_id];
									RCache.set_Matrices						(Nmat->key);	

									mapNormalItems&				items	= Nmat->val;		items.ssa	= 0;
									mapNormal_Render						(items);
								}
								lstMatrices.clear		();
								mat.clear				();
							}
							lstVB.clear				();
							vb.clear				();
						}
						lstTextures.clear		();
						lstTexturesTemp.clear	();
						tex.clear				();
					}
					lstCS.clear				();
					cs.clear				();
				}
				lstPS.clear				();
				ps.clear				();
			}
			lstVS.clear				();
			vs.clear				();
		}
		lstCodes.clear			();
		codes.clear				();
	}

	// NORMAL-matrix		*** actors and dyn. objects
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();
}
