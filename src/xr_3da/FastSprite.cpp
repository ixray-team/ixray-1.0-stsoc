// FastSprite.cpp: implementation of the CFastSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FastSprite.h"
#include "xr_creator.h"
#include "render.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFastSprite::CFastSprite(char *tex_name) : IRender_Visual()
{
	Type			= MT_SPRITE;
	Stream			= RCache.Create(FVF::F_TL,MAX_SPRITES*4);
	hShader.create	("sprite",tex_name);

	fTTL			= 0;

	// POS,RADIUS,BBOX & mTransform --- setted up by derivates
}

CFastSprite::~CFastSprite()
{
}

void CFastSprite::Render(float L)
{
	// build transform matrix
	Fmatrix mSpriteTransform;
	mSpriteTransform.mul(Device.mFullTransform, mTransform);

	// actual rendering
	FVF::TL			PT;
	VSprites::iterator i	= sprites.begin();
	VSprites::iterator end	= sprites.end();
	u32			vOffset;
	FVF::TL*		pv_start= (FVF::TL*)Stream->Lock(sprites.size()*4,vOffset);
	FVF::TL*		pv		= pv_start;

	for (;i!=end; i++) {
		// No sense to render sprite if it is almost invisible
		if (i->alpha < (3.f/255.f) ) continue;

		BYTE C			= BYTE(i->alpha*255.f);
		u32 clr			= color_rgba(C,C,C,C);
		PT.transform	( i->pos, mSpriteTransform );
		float size		= Device.dwWidth * i->size/PT.p.w;

		// 'Cause D3D clipping have to clip Four points
		// We can help him :)
		if (size<1.5f)	continue;
		if (PT.p.x< -1)	continue;
		if (PT.p.x>  1)	continue;
		if (PT.p.y< -1)	continue;
		if (PT.p.y>  1)	continue;
		if (PT.p.z<  0) continue;

		// Convert to screen coords
		float cx        = Device._x2real(PT.p.x);
		float cy        = Device._y2real(PT.p.y);

		// Rotation
		float	_sin1,_cos1,_sin2,_cos2;
		float	da		= i->angle;
		_sin1			= _sin(da);
		_cos1			= _cos(da);
		da				+= PI_DIV_2;
		_sin2			= _sin(da);
		_cos2			= _cos(da);

		// 1
		pv->set			(	cx + size * _sin1,	// sx
							cy + size * _cos1,	// sy
							PT.p.z, PT.p.w, clr, 0,1 );
		pv++;

		// 2
		pv->set			(	cx - size * _sin2,	// sx
							cy - size * _cos2,	// sy
							PT.p.z, PT.p.w, clr, 0,0 );
		pv++;

		// 3
		pv->set			(	cx + size * _sin2,	// sx
							cy + size * _cos2,	// sy
							PT.p.z, PT.p.w, clr, 1,1 );
		pv++;

		// 4
		pv->set			(	cx - size * _sin1,	// sx
							cy - size * _cos1,	// sy
							PT.p.z, PT.p.w, clr, 1,0 );
		pv++;
	}

	// unlock VB and Render it as triangle list
	u32 dwNumVerts = pv-pv_start;
	Stream->Unlock(dwNumVerts);
	if (dwNumVerts)
		RCache.Draw(Stream,dwNumVerts,dwNumVerts/2,vOffset,Device.Streams_QuadIB);
}

void CFastSprite::Update()
{
	::Render.add_leafs_Static(this);
}

#define PCOPY(a)	a = pFrom->a
void CFastSprite::Copy(IRender_Visual* pSrc)
{
	IRender_Visual::Copy	(pSrc);

	CFastSprite* pFrom = (CFastSprite*) pSrc;
	PCOPY	(Stream		);
	PCOPY	(sprites	);
	PCOPY	(mTransform	);
	PCOPY	(fTTL		);
}
