#pragma once
#include "../fhierrarhyvisual.h"

class	FLOD	:	public FHierrarhyVisual
{
	typedef FHierrarhyVisual inherited;
public:
	struct _vertex
	{
		Fvector		v;
		Fvector2	t;
		u32			c_rgb_hemi;	// rgb,hemi
		u8			c_sun;
	};
	struct _face
	{
		_vertex		v[4];
		Fvector		N;
	};
	struct _hw 
	{
		Fvector		p;
		Fvector		n;
		u32			color;
		Fvector2	t;
		IC void		set(const Fvector& _p, const Fvector& _n, u32 C, float u, float v) { p.set(_p); n.set(_n); color=C; t.set(u,v);}
	};
	static const u32	F_HW	= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;

	ref_geom		geom		;
	_face			facets		[8];
	float			lod_factor	;
public:
	virtual void Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void Copy			(IRender_Visual *pFrom	);
};
