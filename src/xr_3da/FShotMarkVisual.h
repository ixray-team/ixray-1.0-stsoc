// FShotMarkVisual.h: interface for the FShotMarkVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSHOTMARKVISUAL_H__D3672A8D_8BAF_4925_A7E4_787CDD87BC6F__INCLUDED_)
#define AFX_FSHOTMARKVISUAL_H__D3672A8D_8BAF_4925_A7E4_787CDD87BC6F__INCLUDED_
#pragma once

#include "FVisual.h"

class FShotMarkVisual : public Fvisual
{
	float		fTime;
public:
	Fmatrix		mSelfTransform;

	virtual void			Render				(float LOD);
	BOOL					isReadyForDestroy	(void)	{ return fTime<0; }

	FShotMarkVisual			();
	virtual ~FShotMarkVisual();
};

#endif // !defined(AFX_FSHOTMARKVISUAL_H__D3672A8D_8BAF_4925_A7E4_787CDD87BC6F__INCLUDED_)
