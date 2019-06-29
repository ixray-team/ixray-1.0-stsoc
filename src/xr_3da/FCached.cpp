// FCached.cpp: implementation of the FCached class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FCached.h"
#include "fmesh.h"

#define GeomBytes		24	// pos+norm

void TransferGeometry	(LPVOID vDest, LPVOID vSrc, u32 vCount, u32 vStride,
						 LPWORD iDest, LPWORD iSrc, u32 iCount, u32 iOffset,
						 Fmatrix* xform)
{
	// Transfer vertices
	if (xform) 
	{
		LPBYTE	sit		= LPBYTE(vSrc);
		LPBYTE	send	= sit+vCount*vStride;
		LPBYTE	dit		= LPBYTE(vDest);
		u32	remain	= vStride-GeomBytes;		// 2fvector of 3 floats

		switch (remain) {
		case 8:		// 32 byte vertex	(pos(12)+norm(12)+uv1(8))
			for (; sit!=send; sit+=vStride, dit+=vStride)
			{
				Fvector*	sP	= (Fvector*)sit;
				Fvector*	dP	= (Fvector*)dit;
				Fvector*	sN	= (Fvector*)(sit+3*4);
				Fvector*	dN	= (Fvector*)(dit+3*4);
				xform->transform_tiny	(*dP,*sP);
				xform->transform_dir	(*dN,*sN);
				CopyMemory	(dit+GeomBytes,sit+GeomBytes,8);
			}
			break;
		case 16:	// 40 byte vertex	(pos(12)+norm(12)+uv1(8)+uv2(8))
			for (; sit!=send; sit+=vStride, dit+=vStride)
			{
				Fvector*	sP	= (Fvector*)sit;
				Fvector*	dP	= (Fvector*)dit;
				Fvector*	sN	= (Fvector*)(sit+3*4);
				Fvector*	dN	= (Fvector*)(dit+3*4);
				xform->transform_tiny	(*dP,*sP);
				xform->transform_dir	(*dN,*sN);
				CopyMemory	(dit+GeomBytes,sit+GeomBytes,16);
			}
			break;
		default:	// any size
			for (; sit!=send; sit+=vStride, dit+=vStride)
			{
				Fvector*	sP	= (Fvector*)sit;
				Fvector*	dP	= (Fvector*)dit;
				Fvector*	sN	= (Fvector*)(sit+3*4);
				Fvector*	dN	= (Fvector*)(dit+3*4);
				xform->transform_tiny	(*dP,*sP);
				xform->transform_dir	(*dN,*sN);
				CopyMemory	(dit+GeomBytes,sit+GeomBytes,remain);
			}
			break;
		}
	} else {
		Memory.mem_copy	(vDest,vSrc,vCount*vStride);
	}

	// Transfer indices (in 32bit lines)
	{
		VERIFY	(iOffset<65535);
		u32	item	= (iOffset<<16) | iOffset;
		u32	count	= iCount/2;
		LPDWORD	sit		= LPDWORD(iSrc);
		LPDWORD	send	= sit+count;
		LPDWORD	dit		= LPDWORD(iDest);
		for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
		if		(iCount&1)	iDest[iCount-1]=iSrc[iCount-1]+u16(iOffset);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FCached::FCached()
{
	pVertices	= NULL;
	vCount		= 0;
	pIndices	= NULL;
	iCount		= 0;
}

FCached::~FCached()
{

}

void FCached::Release()
{
	IRender_Visual::Release();
	xr_free			(pVertices);
	xr_free			(pIndices);
}

void FCached::Load(const char* N, IReader *data, u32 dwFlags)
{
	IRender_Visual::Load(N,data,dwFlags);

	// read vertices
	if ((dwFlags&VLOAD_NOVERTICES)==0) {
		R_ASSERT(data->find_chunk(OGF_VERTICES));
		
		u32			dwVertType;
		
		dwVertType		= data->r_u32();
		vCount			= data->r_u32();
		hGeom.create	(dwVertType, RCache.Vertex.Buffer(), RCache.Index.Buffer());
		
		u32	mem_size	= vCount*hGeom->vb_stride;
		pVertices		= xr_malloc		(mem_size);
		data->r			(pVertices,mem_size);
	}

	// indices
	if ((dwFlags&VLOAD_NOINDICES)==0) 
	{
		R_ASSERT		(data->find_chunk(OGF_INDICES));
		iCount			= data->r_u32();
		R_ASSERT		(iCount%3 == 0);

		u32	mem_size	= iCount*2;
		pIndices		= (u16*)xr_malloc(mem_size);
		data->r			(pIndices,mem_size);
	}
	
	// checking indices range
#ifdef DEBUG
	if (pVertices && pIndices)	R_ASSERT(ValidateIndices(vCount,iCount,pIndices));
#endif
}

#define PCOPY(a)	a = pFrom->a
void FCached::Copy	(IRender_Visual *pSrc)
{
	IRender_Visual::Copy(pSrc);

	FCached	*pFrom = (FCached *)pSrc;
	PCOPY	(vCount);
	PCOPY	(pIndices);
	PCOPY	(iCount);
}
