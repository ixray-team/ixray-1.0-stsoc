// FProgressiveFixedVisual.h: interface for the FProgressiveFixedVisual class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FProgressiveFixedVisualH
#define FProgressiveFixedVisualH
#pragma once

#include "FVisual.h"

#pragma pack(push,1)
struct ENGINE_API Vsplit
{
	u16		vsplitVert;		// the vert which I'm splitting off of;
							// (the vert I create is just the current end of the VB)
	BYTE	numNewTriangles;
	BYTE	numFixFaces;
							// the index here is an offset in the FixFaces _list
							// which contains offsets in the triangle _list
};
#pragma pack(pop)

#define PM_REFRESH	6

class ENGINE_API FProgressiveFixedVisual : public Fvisual
{
protected:
	u32			V_Minimal;
	u32			V_Current;
	u32			I_Current;
	u32			FIX_Current;
	u32			iRefresh;		// 0..PM_REFRESH
	u16*		faces_affected;	// shared
	Vsplit*		vsplit;			// shared

	void		pm_copy		(IRender_Visual *pFrom);
public:
	virtual void Render		(float LOD);	// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Used
	virtual void Load		(const char* N, IReader *data,u32 dwFlags);
	virtual void Copy		(IRender_Visual *pFrom);
	virtual void Release	();

	// functionality
	void	SetLOD			(float LOD);

	FProgressiveFixedVisual();
	virtual ~FProgressiveFixedVisual();
};

#endif // !defined(AFX_FPROGRESSIVEFIXEDVISUAL_H__778ADCC0_9140_11D3_B4E3_4854E82A090D__INCLUDED_)
