// FProgressiveFixedVisual.cpp: implementation of the FProgressiveFixedVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "FProgressiveFixedVisual.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FProgressiveFixedVisual::FProgressiveFixedVisual() : Fvisual()
{
	iRefresh = rand()%PM_REFRESH;
}

FProgressiveFixedVisual::~FProgressiveFixedVisual()
{
	_RELEASE	(pIndices);
}

void FProgressiveFixedVisual::Load(const char* N, IReader *data, u32 dwFlags)
{
	// load base visual data
	Fvisual::Load(N,data,dwFlags);

	IReader* fs = data->open_chunk(OGF_P_MAP);

	// PMAP_HEADER
	R_ASSERT(fs->find_chunk(0x1));
	V_Current	= V_Minimal = fs->r_u32();
	I_Current	= fs->r_u32();
	FIX_Current	= 0;

	// PMAP_VSPLIT
	R_ASSERT(vCount>V_Minimal);
	vsplit = xr_alloc<Vsplit>(vCount-V_Minimal);
	R_ASSERT(fs->r_chunk_safe(0x2,vsplit,(vCount-V_Minimal)*sizeof(Vsplit)));

	// PMAP_FACES
	R_ASSERT(fs->find_chunk(0x3));
	u32 fCount = fs->r_u32();
	faces_affected = xr_alloc<u16>(fCount);
	fs->r(faces_affected,fCount*sizeof(u16));

	fs->close();
}

void FProgressiveFixedVisual::Release()
{
	Fvisual::Release();

	xr_free(faces_affected);
	xr_free(vsplit);
}

void FProgressiveFixedVisual::SetLOD(float LOD)
{
	if (LOD<0)			return;
	VERIFY				(LOD<=1.0f);
	
	u32 dwCount = V_Minimal + iFloor(float(vCount-V_Minimal)*LOD+.4999f);
	
	VERIFY(dwCount>=V_Minimal);
	VERIFY(dwCount<=vCount);
	
	// First cycle - try to improve quality
	if (V_Current!=dwCount) {
		u16*					Indices = 0;
		R_CHK					(pIndices->Lock	(0,0,(void**)&Indices,0));
		
		while (V_Current<dwCount) {
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			u32	dwEnd = u32(S.numFixFaces)+FIX_Current;
			u16	V_Cur = u16(V_Current);
			for (u32 I=FIX_Current; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==S.vsplitVert);
				Indices[faces_affected[I]]=V_Cur;
			};
			
			// correct numbers
			I_Current	+=	3*u32(S.numNewTriangles);
			V_Current	+=	1;
			FIX_Current	+=	S.numFixFaces;
		};
		
		// Second cycle - try to decrease quality
		while (V_Current>dwCount) {
			V_Current	-=	1;
			
			Vsplit&	S	=	vsplit[V_Current-V_Minimal];
			
			// fixup faces
			u16 V_New		= u16(S.vsplitVert);
			u32 dwEnd		= FIX_Current;
			u32 dwStart	= dwEnd-S.numFixFaces;
			for (u32 I=dwStart; I<dwEnd; I++) {
				//				VERIFY(Indices[faces_affected[I]]==V_Current);
				Indices[faces_affected[I]]=V_New;
			};
			
			// correct numbers
			I_Current	-=	3*u32(S.numNewTriangles);
			FIX_Current	-=	S.numFixFaces;
		};

		R_CHK					(pIndices->Unlock());
	}
}

void FProgressiveFixedVisual::Render(float LOD)
{
	if (++iRefresh >= PM_REFRESH) {
		iRefresh = 0;
		SetLOD(LOD);
	}
	if (V_Current && I_Current) {
		RCache.set_Geometry		(hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vBase,0,V_Current,iBase,I_Current/3);
	}
}

#define PCOPY(a)	a = pFrom->a

void	FProgressiveFixedVisual::pm_copy(IRender_Visual *pSrc)
{
	FProgressiveFixedVisual	*pFrom = dynamic_cast<FProgressiveFixedVisual*>(pSrc);
	PCOPY(V_Minimal);
	PCOPY(V_Current);
	PCOPY(I_Current);
	PCOPY(FIX_Current);

	PCOPY(faces_affected);
	PCOPY(vsplit);

}
void	FProgressiveFixedVisual::Copy(IRender_Visual *pFrom)
{
	Fvisual::Copy			(pFrom);
	pm_copy					(pFrom);

	FProgressiveFixedVisual* pSrc	= dynamic_cast<FProgressiveFixedVisual*>(pFrom);

	// note::	index buffer bust be explicitly duplicated 
	BYTE *d_src=0, *d_dst=0;
	D3DINDEXBUFFER_DESC		desc;
	R_CHK					(pSrc->pIndices->GetDesc	(&desc));
	R_CHK					(pSrc->pIndices->Lock		(0,0,(void**)&d_src,D3DLOCK_READONLY));
	R_CHK					(HW.pDevice->CreateIndexBuffer(iCount*2,desc.Usage,D3DFMT_INDEX16,desc.Pool,&pIndices,0));
	R_CHK					(pIndices->Lock				(0,0,(void**)&d_dst,0));
	Memory.mem_copy			(d_dst,d_src,desc.Size);
	R_CHK					(pIndices->Unlock());
	R_CHK					(pSrc->pIndices->Unlock());
}
