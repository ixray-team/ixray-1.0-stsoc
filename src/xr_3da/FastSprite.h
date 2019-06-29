// FastSprite.h: interface for the CFastSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTSPRITE_H__ECCE8A89_05B6_4966_AE66_7C8649B3BDF9__INCLUDED_)
#define AFX_FASTSPRITE_H__ECCE8A89_05B6_4966_AE66_7C8649B3BDF9__INCLUDED_
#pragma once

#include "fBasicVisual.h"

#define MAX_SPRITES		256

struct ENGINE_API FSprite 
{
	Fvector pos;
	float	size;
	float	alpha;
	float	angle;
};

typedef xr_vector<FSprite>	VSprites;

class ENGINE_API CFastSprite : public IRender_Visual
{
protected:
	CVertexStream*			Stream;
	VSprites				sprites;

	Fmatrix					mTransform;		// applyed automatically during render
	float					fTTL;			// time to live
public:
	// Visual
	virtual void			Render			(float LOD);
	virtual void			Copy			(IRender_Visual* pFrom);

	// Functionality
	BOOL					isReadyForDestroy()	{ return fTTL<=0; }
	virtual void			Update			();

	CFastSprite				(char *tex_name);
	virtual ~CFastSprite	();

};

#endif // !defined(AFX_FASTSPRITE_H__ECCE8A89_05B6_4966_AE66_7C8649B3BDF9__INCLUDED_)
