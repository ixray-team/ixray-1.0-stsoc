// FShadowForm.h: interface for the FShadowForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_)
#define AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_
#pragma once

#include "fBasicVisual.h"

// These are the shadow volumes
struct SHADOW_V
{
	u16*   pwShadVolIndices;      // tri indices into vertex buffer VB for DrawPrim
	u16*   pwShadVolSideIndices;  // ptrs into main index array pwShadVolIndices for Side tris of shadow volume
	u16*   pwShadVolCapIndices;   // ptrs into main index array pwShadVolIndices for cap tris of shadow volume
	u32   dwNumVertices;
	u32   dwNumSideIndices;
	u32   dwNumCapIndices;
};
#pragma pack(push)
#pragma pack(1)
struct COLORVERTEX {
	Fvector p;
	u32	c;
};
#pragma pack(pop)

class FShadowForm : public IRender_Visual
{
	// Data
	LPDIRECT3DVERTEXBUFFER7	pVB;
	xr_vector<u16>			Indices;
	u32					dwNumVertices;

	BOOL					g_bDrawShadowVolCaps;

	// Resulting shadow volume
	LPDIRECT3DVERTEXBUFFER7	pProjected;
	SHADOW_V				S;
	u16*					pwCHI;

	// Private methods
public:
	void					RenderShadow		(LPDIRECT3DVERTEXBUFFER7 pv);
	void					MakeShadowVolume	(Fvector &L, Fmatrix &mWorld );

	virtual	void			Load(IReader *data, u32 dwFlags);
	virtual void			Render(float LOD);

	FShadowForm();
	virtual ~FShadowForm();

};

#endif // !defined(AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_)
