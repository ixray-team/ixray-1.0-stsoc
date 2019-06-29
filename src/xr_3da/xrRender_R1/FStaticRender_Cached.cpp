// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 12:14:16 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "..\fcached.h"
#include "..\fmesh.h"

const u32	v_limit			= 2048;

// Render of cached meshes
void __fastcall render_Cached(xr_vector<FCached*>& cache)
{
/*
	_VertexStream*			vs	= RCache.Vertex;
	_IndexStream*			is	= RCache.Index;

	RCache.Reset		();
	for (u32 Start=0; Start<cache.size(); )
	{
		// Calculate lock parameters
		u32	v_count=0,i_count=0,t;
		for (u32 End=Start; End<cache.size(); End++)
		{
			FCached& V	=	*(cache[End]);
			if (V.VS!=vs)		break;
			t			=	v_count+V.vCount;
			if (t>v_limit)		break;
			v_count		=	t;
			i_count		+=	V.iCount;
		}
		vs				=	cache[Start]->VS;

		// Transfer geometry
		u32	vBase,	iBase;
		u32	Stride	= vs->Stride();
		BYTE*	verts	= LPBYTE(vs->Lock(v_count,vBase));
		u16*	indices	= LPWORD(is->Lock(i_count,iBase));
		u32	iOffset	= 0;
		for (u32 I=Start; I!=End; I++)
		{
			FCached& V	=	*(cache[I]);
			
			// Transfer vertices
			Memory.mem_copy	(verts,V.pVertices,V.vCount*Stride);
			
			// Transfer indices (in 32bit lines)
			VERIFY	(iOffset<65535);
			{
				u32	iCount	= V.iCount;
				u32	item	= (iOffset<<16) | iOffset;
				u32	count	= iCount/2;
				LPDWORD	sit		= LPDWORD(V.pIndices);
				LPDWORD	send	= sit+count;
				LPDWORD	dit		= LPDWORD(indices);
				for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
				if		(iCount&1)	indices[iCount-1] = V.pIndices[iCount-1]+u16(iOffset);
			}

			// Increment counters
			verts		+=	V.vCount*Stride;
			indices		+=	V.iCount;
			iOffset		+=	V.vCount;
		}
		vs->Unlock		(v_count);
		is->Unlock		(i_count);

		// Render
		u32	dwNumPrimitives			= i_count/3;
		CHK_DX(HW.pDevice->SetVertexShader		(vs->getFVF()));
		CHK_DX(HW.pDevice->SetStreamSource		(0,vs->getBuffer(),vs->Stride()));
		CHK_DX(HW.pDevice->SetIndices			(is->getBuffer(),vBase));
		CHK_DX(HW.pDevice->DrawIndexedPrimitive	(D3DPT_TRIANGLELIST,0,v_count,iBase,dwNumPrimitives));
		UPDATEC(v_count,dwNumPrimitives,1);

		Start = End;
	}
*/
}
