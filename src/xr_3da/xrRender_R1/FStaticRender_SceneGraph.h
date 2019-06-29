#pragma once

#include "FixedMap.h"

class ENGINE_API		IRender_Visual;
class ENGINE_API		CObject;

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		IRenderable*	pObject;
		IRender_Visual*	pVisual;
		Fmatrix			Matrix;				// matrix (copy)
		Fvector			vCenter;
		float			lod;
	};
	struct _LodItem		{
		float				ssa;
		IRender_Visual*		pVisual;
	};
	struct _NormalItem	{
		float				ssa;
		IRender_Visual*		pVisual;
	};

	// Higher level	- NORMAL
	typedef xr_vector<_NormalItem>	mapNormalDirect;
	struct mapNormalItems		: public	mapNormalDirect											{	float	ssa;	};
	struct mapNormalMatrices	: public	FixedMAP<SMatrixList*,mapNormalItems>					{	float	ssa;	};
	struct mapNormalVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapNormalMatrices>		{	float	ssa;	};
	struct mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalVB>						{	float	ssa;	};
	struct mapNormalCS			: public	FixedMAP<R_constant_table*,mapNormalTextures>			{	float	ssa;	};
	struct mapNormalPS			: public	FixedMAP<IDirect3DPixelShader9*, mapNormalCS>			{	float	ssa;	};
	struct mapNormalVS			: public	FixedMAP<IDirect3DVertexShader9*, mapNormalPS>			{	float	ssa;	};
	struct mapNormalCodes		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalVS>				{ };

	// Higher level - MATRIX
	typedef FixedMAP<float,_MatrixItem>				mapMatrixItem;

	// Top level
	typedef mapNormalCodes							mapNormal_T		[SHADER_PASSES_MAX];

	typedef FixedMAP<ShaderElement*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode						mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>				mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItem>				mapHUD_T;
	typedef mapSorted_T::TNode						mapHUD_Node;

	typedef FixedMAP<float,_LodItem>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
