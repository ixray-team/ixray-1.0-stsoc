#include "stdafx.h"

using namespace SceneGraph;

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;
extern float		g_fLOD, g_fFarSq;

IC float calcLOD	(float fDistSq, float R)
{
	float dist	= g_fFarSq - fDistSq + R*R;
	float lod	= g_fLOD*dist/g_fFarSq;
	clamp		(lod,0.001f,0.999f);
	return		lod;
}

// NORMAL
void __fastcall normal_L2			(FixedMAP<float,IRender_Visual*>::TNode *N)
{
	IRender_Visual *V = N->val;
	V->Render(calcLOD(N->key,V->vis.sphere.R));
}

void __fastcall mapNormal_Render	(mapNormalItems& N)
{
	// DIRECT:SORTED
	N.sorted.traverseLR		(normal_L2);
	N.sorted.clear			();

	// DIRECT:UNSORTED
	xr_vector<IRender_Visual*>&	L			= N.unsorted;
	IRender_Visual **I=&*L.begin(), **E = &*L.end();
	for (; I!=E; I++)
	{
		IRender_Visual *V = *I;
		V->Render	(0);	// zero lod 'cause it is too small onscreen
	}
	L.clear	();
}

// Matrix
void __fastcall matrix_L2			(FixedMAP<float,_MatrixItem>::TNode *N)
{
	_MatrixItem&	I		= N->val;
	IRender_Visual			*V		= I.pVisual;
	RCache.set_xform_world	(I.Matrix);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

void __fastcall mapMatrix_Render	(mapMatrixItems& N)
{
	// DIRECT:SORTED
	N.sorted.traverseLR		(matrix_L2);
	N.sorted.clear			();

	// DIRECT:UNSORTED
	xr_vector<_MatrixItem>&	L			= N.unsorted;
	_MatrixItem *it =&*L.begin(), *end	= &*L.end();
	for (; it!=end; it++)
	{
		_MatrixItem&	I		= *it;
		IRender_Visual			*V		= I.pVisual;
		RCache.set_xform_world	(I.Matrix);
		V->Render				(0);	// zero lod 'cause it is too small onscreen
	}
	L.clear					();
}

// ALPHA
void __fastcall sorted_L1	(mapSorted_Node *N)
{
	IRender_Visual *V = N->val.pVisual;
	RCache.set_Shader		(V->hShader);
	RCache.set_xform_world	(N->val.Matrix);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

IC	bool	cmp_vs_nrm			(mapNormalVS::TNode* N1, mapNormalVS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_vs_mat			(mapMatrixVS::TNode* N1, mapMatrixVS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_ps_nrm			(mapNormalPS::TNode* N1, mapNormalPS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_ps_mat			(mapMatrixPS::TNode* N1, mapMatrixPS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_cs_nrm			(mapNormalCS::TNode* N1, mapNormalCS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_cs_mat			(mapMatrixCS::TNode* N1, mapMatrixCS::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_states_nrm		(mapNormalStates::TNode* N1, mapNormalStates::TNode* N2)	{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_states_mat		(mapMatrixStates::TNode* N1, mapMatrixStates::TNode* N2)	{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_vb_nrm			(mapNormalVB::TNode* N1, mapNormalVB::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}
IC	bool	cmp_vb_mat			(mapMatrixVB::TNode* N1, mapMatrixVB::TNode* N2)			{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures_lex2_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex2_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lexN_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return std::lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_lexN_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return std::lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_ssa_nrm	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2){	
	return (N1->val.ssa > N2->val.ssa);		
}
IC	bool	cmp_textures_ssa_mat	(mapMatrixTextures::TNode* N1, mapMatrixTextures::TNode* N2){	
	return (N1->val.ssa > N2->val.ssa);		
}

void		sort_tlist_nrm			
(
 xr_vector<mapNormalTextures::TNode*>& lst, 
 xr_vector<mapNormalTextures::TNode*>& temp, 
 mapNormalTextures&					textures, 
 BOOL	bSSA
 )
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa_nrm);
		} 
		else 
		{
			// Split into 2 parts
			mapNormalTextures::TNode* _it	= textures.begin	();
			mapNormalTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa_nrm);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2_nrm);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3_nrm);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN_nrm);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2_nrm);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3_nrm);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN_nrm);
	}
}

void		sort_tlist_mat			
(
 xr_vector<mapMatrixTextures::TNode*>& lst,
 xr_vector<mapMatrixTextures::TNode*>& temp,
 mapMatrixTextures&					textures,
 BOOL	bSSA
 )
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa_mat);
		} 
		else 
		{
			// Split into 2 parts
			mapMatrixTextures::TNode* _it	= textures.begin	();
			mapMatrixTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa_mat);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2_mat);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3_mat);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN_mat);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2_mat);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3_mat);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN_mat);
	}
}

void CRender::render_scenegraph	()
{
	Device.Statistic.RenderDUMP.Begin		();

	// **************************************************** NORMAL
	// Perform sorting based on ScreenSpaceArea
	// Sorting by SSA and changes minimizations
	{
		RCache.set_xform_world			(Fidentity);
		mapNormalVS&	vs				= mapNormal;
		vs.getANY_P						(nrmVS);
		std::sort						(nrmVS.begin(), nrmVS.end(), cmp_vs_nrm);
		for (u32 vs_id=0; vs_id<nrmVS.size(); vs_id++)
		{
			mapNormalVS::TNode*	Nvs			= nrmVS[vs_id];
			RCache.set_VS					(Nvs->key);	

			mapNormalPS&		ps			= Nvs->val;		ps.ssa	= 0;
			ps.getANY_P						(nrmPS);
			std::sort						(nrmPS.begin(), nrmPS.end(), cmp_ps_nrm);
			for (u32 ps_id=0; ps_id<nrmPS.size(); ps_id++)
			{
				mapNormalPS::TNode*	Nps			= nrmPS[ps_id];
				RCache.set_PS					(Nps->key);	

				mapNormalCS&		cs			= Nps->val;		cs.ssa	= 0;
				cs.getANY_P						(nrmCS);
				std::sort						(nrmCS.begin(), nrmCS.end(), cmp_cs_nrm);
				for (u32 cs_id=0; cs_id<nrmCS.size(); cs_id++)
				{
					mapNormalCS::TNode*	Ncs			= nrmCS[cs_id];
					RCache.set_Constants			(Ncs->key);

					mapNormalStates&	states		= Ncs->val;		states.ssa	= 0;
					states.getANY_P					(nrmStates);
					std::sort						(nrmStates.begin(), nrmStates.end(), cmp_states_nrm);
					for (u32 state_id=0; state_id<nrmStates.size(); state_id++)
					{
						mapNormalStates::TNode*	Nstate		= nrmStates[state_id];
						RCache.set_States					(Nstate->key);

						mapNormalTextures&		tex			= Nstate->val;	tex.ssa =	0;
						sort_tlist_nrm						(nrmTextures,nrmTexturesTemp,tex,true);
						for (u32 tex_id=0; tex_id<nrmTextures.size(); tex_id++)
						{
							mapNormalTextures::TNode*	Ntex		= nrmTextures[tex_id];
							RCache.set_Textures						(Ntex->key);

							mapNormalVB&				vb			= Ntex->val;	vb.ssa	=	0;
							vb.getANY_P								(nrmVB);
							std::sort								(nrmVB.begin(), nrmVB.end(), cmp_vb_nrm);
							for (u32 vb_id=0; vb_id<nrmVB.size(); vb_id++)
							{
								mapNormalVB::TNode*			Nvb		= nrmVB[vb_id];
								// no need to setup that shit - visual defined

								mapNormalItems&				items	= Nvb->val;		items.ssa	= 0;
								mapNormal_Render					(items);
							}
							nrmVB.clear				();
							vb.clear				();
						}
						nrmTextures.clear		();
						nrmTexturesTemp.clear	();
						tex.clear				();
					}
					nrmStates.clear			();
					states.clear			();
				}
				nrmCS.clear				();
				cs.clear				();
			}
			nrmPS.clear				();
			ps.clear				();
		}
		nrmVS.clear				();
		vs.clear				();
	}

	// **************************************************** MATRIX
	// Perform sorting based on ScreenSpaceArea
	// Sorting by SSA and changes minimizations
	{
		mapMatrixVS&	vs				= mapMatrix;
		vs.getANY_P						(matVS);
		std::sort						(matVS.begin(), matVS.end(), cmp_vs_mat);
		for (u32 vs_id=0; vs_id<matVS.size(); vs_id++)
		{
			mapMatrixVS::TNode*	Nvs			= matVS[vs_id];
			RCache.set_VS					(Nvs->key);	

			mapMatrixPS&		ps			= Nvs->val;		ps.ssa	= 0;
			ps.getANY_P						(matPS);
			std::sort						(matPS.begin(), matPS.end(), cmp_ps_mat);
			for (u32 ps_id=0; ps_id<matPS.size(); ps_id++)
			{
				mapMatrixPS::TNode*	Nps			= matPS[ps_id];
				RCache.set_PS					(Nps->key);	

				mapMatrixCS&		cs			= Nps->val;		cs.ssa	= 0;
				cs.getANY_P						(matCS);
				std::sort						(matCS.begin(), matCS.end(), cmp_cs_mat);
				for (u32 cs_id=0; cs_id<matCS.size(); cs_id++)
				{
					mapMatrixCS::TNode*	Ncs			= matCS[cs_id];
					RCache.set_Constants			(Ncs->key);

					mapMatrixStates&	states		= Ncs->val;		states.ssa	= 0;
					states.getANY_P					(matStates);
					std::sort						(matStates.begin(), matStates.end(), cmp_states_mat);
					for (u32 state_id=0; state_id<matStates.size(); state_id++)
					{
						mapMatrixStates::TNode*	Nstate		= matStates[state_id];
						RCache.set_States					(Nstate->key);

						mapMatrixTextures&		tex			= Nstate->val;	tex.ssa =	0;
						sort_tlist_mat						(matTextures,matTexturesTemp,tex,true);
						for (u32 tex_id=0; tex_id<matTextures.size(); tex_id++)
						{
							mapMatrixTextures::TNode*	Ntex		= matTextures[tex_id];
							RCache.set_Textures						(Ntex->key);

							mapMatrixVB&				vb			= Ntex->val;	vb.ssa	=	0;
							vb.getANY_P								(matVB);
							std::sort								(matVB.begin(), matVB.end(), cmp_vb_mat);
							for (u32 vb_id=0; vb_id<matVB.size(); vb_id++)
							{
								mapMatrixVB::TNode*			Nvb		= matVB[vb_id];
								// no need to setup that shit - visual defined

								mapMatrixItems&				items	= Nvb->val;		items.ssa	= 0;
								mapMatrix_Render					(items);
							}
							matVB.clear				();
							vb.clear				();
						}
						matTextures.clear		();
						matTexturesTemp.clear	();
						tex.clear				();
					}
					matStates.clear			();
					states.clear			();
				}
				matCS.clear				();
				cs.clear				();
			}
			matPS.clear				();
			ps.clear				();
		}
		matVS.clear				();
		vs.clear				();
	}

	Device.Statistic.RenderDUMP.End	();
}
