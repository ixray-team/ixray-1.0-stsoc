// FShadowForm.cpp: implementation of the FShadowForm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ffileops.h"
#include "FShadowForm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FShadowForm::FShadowForm()
{
	pVB				= NULL;
	pProjected		= NULL;
	Indices.clear	();
	g_bDrawShadowVolCaps = false;
}

FShadowForm::~FShadowForm()
{
	_RELEASE(pProjected);
	_RELEASE(pVB);
	Indices.clear();
	_DELETEARRAY(S.pwShadVolIndices);
	_DELETEARRAY(pwCHI);
}

void FShadowForm::Load(IReader *data)
{
	IRender_Visual::Load(data);

	// read vertices
	if (data->find_chunk(OGF_VERTICES)) {
		u32 dwVertType;

		dwVertType		= data->ReadDWORD();
		VERIFY(dwVertType==(D3DFVF_XYZ|D3DFVF_DIFFUSE));
		dwNumVertices	= data->ReadDWORD();
		pVB	= Device.VB.Create(dwVertType,dwNumVertices*2,true);
		void *v;
		pVB->Lock(
			DDLOCK_NOSYSLOCK|DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY,
            &v, NULL );
		CopyMemory(v,data->Pointer(),dwNumVertices*sizeof(COLORVERTEX));
		pVB->Unlock();
	} else {
		THROW;
	}

	// indices
	if (data->find_chunk(OGF_INDICES)) {
		u32 dwCount = data->ReadDWORD();
		VERIFY(dwCount%3 == 0);
		Indices.resize(dwCount);
		data->Read(Indices.begin(), sizeof(u16)*dwCount);
	} else {
		THROW;
	}

	ZeroMemory(&S,sizeof(S));

    // Create pProjected vertex buffer as a target for the vertex-projection
    // operation used to compute the silhouette.
    // Note: even though RHW not used, we must specify it or ProcessVerticess()
    // will not consider this as a valid target to xform verts into
	pProjected = Device.VB.Create(D3DFVF_XYZRHW,dwNumVertices,true);

    // Allocate enough indices to hold the largest-case shadowvolume (max #
    // of vertices in c-hull is dwNumVertices).
    // (dwNumVertices+1)*2 for triangle mesh to hold shadowvolume sides +
    // dwNumVertices to hold triangle-fan
	S.pwShadVolIndices	= xr_alloc<u16>((dwNumVertices+1)*3);

	// Allocate memory for the hull (max size is dwNumInVertices+1 times the
	// the storage space for a u32 and a ptr to a vertex
	u32 dwElementSize	= sizeof(u32) + sizeof(COLORVERTEX*)+4;
	pwCHI				= (u16*)(xr_alloc<u8>((dwNumVertices+1)*dwElementSize));
}

//-----------------------------------------------------------------------------
// Name: MakeShadowVolume()
// Desc: Build the polygons of the shadow volume for a directional light
//       pInVertices are copied into a vertex buffer created within S
//-----------------------------------------------------------------------------
extern VOID Find2DConvexHull(	u32 dwNumInVertices, COLORVERTEX* pInVertices,
								u32* pdwNumOutIndices, u16* ppOutIndices );

void FShadowForm::MakeShadowVolume( Fvector& vLDir, Fmatrix &matWorld )
{
    Fmatrix matNewView,mInvWorld;
	Fvector vL;
    u32 i;


	// Transform LightDir into Local Coordinates
	mInvWorld.invert(matWorld);
	vL.transform_dir(vLDir,mInvWorld);
	vL.normalize();
//	vL.set(vLDir);

    // For view matrix, all we want is anything that projects the verts onto a
    // plane perpendicular to light direction. So any eyepoint is OK (try to
    // make object near origin though, so look at one of the verticess). Don't
    // care what direction is view up _vector_ (y).
    Fvector at,from,up;
	at.set		(Position());
	from.mad	(at,vL,-fRadius*2);

	Fvector	right, y;
	y.set(0,1,0);
	right.crossproduct(y,vL);
	up.crossproduct(vL,right);
	up.normalize();

    matNewView.build_camera	(from, at, up	);
    RCache.set_xform_view	(matNewView		);

    // Do the planar projection
    pProjected->ProcessVertices( D3DVOP_TRANSFORM, 0, dwNumVertices, pVB, 0, HW.pDevice, 0 );

    COLORVERTEX* pProjVertices;
    u16*        pHullIndices = pwCHI;
    u32        dwNumHullIndices;

    if( SUCCEEDED( pProjected->Lock(
		DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
        (VOID**)&pProjVertices, NULL ) ) )
    {
        Find2DConvexHull( dwNumVertices, pProjVertices, &dwNumHullIndices, pHullIndices );

        pProjected->Unlock();
    }

	COLORVERTEX* pVBVertices;
    if( SUCCEEDED( pVB->Lock( DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
                                      (VOID**)&pVBVertices, NULL ) ) )
    {
        // Make shadow volume by taking hull verts and project them along the light
        // direction far enough to be offscreen

        // Add verts to end of VB
        for( i=0; i<dwNumHullIndices; i++ )
        {
            pVBVertices[dwNumVertices+i].p.mad(pVBVertices[pHullIndices[i]].p,vL,40.0f);
            pVBVertices[dwNumVertices+i].c = 0x7f000000;
        }

        S.dwNumVertices=dwNumVertices+dwNumHullIndices;

        // Now form array of indices that will make the triangles. The ShadowVolume
        // will have dwNumHullIndices square sides.

        S.dwNumSideIndices = (dwNumHullIndices+1)*2;

        // If shadvol is not capped, shadow may be drawn in place where a
        // backfacing cap is missing even though no geometry is there
        if( g_bDrawShadowVolCaps )
           S.dwNumCapIndices = dwNumHullIndices;
        else
           S.dwNumCapIndices = 0;

        u16* pIndexPtr = S.pwShadVolSideIndices = S.pwShadVolIndices;

        // Triangles for all facets but final one
        for( i=0; i<dwNumHullIndices; i++ )
        {
            // pHullIndices[i] is the index of the ith vertex of the
            // dwNumHullIndices'th convex hull vertices. dwNumVertices+i is the
            // index of the projected vertex corresponding to the pHullIndices[i]
            // vertex.
            *pIndexPtr++ = pHullIndices[i];
            *pIndexPtr++ = (u16)( dwNumVertices+i );
        }

        // Add tris for final facet (i==dwNumHullIndices)
        *pIndexPtr++ = pHullIndices[0];
        *pIndexPtr++ = (u16)( dwNumVertices+0 );

        S.pwShadVolCapIndices = pIndexPtr;

        if( g_bDrawShadowVolCaps )
        {
            for( i=(dwNumVertices+dwNumHullIndices-1); i>=dwNumVertices; i-- )
            {
                // Draw a fan over the shadvolume cap. Note: only the back-facing
                // cap is done here (which is why we're count backwards).
                *pIndexPtr++ = (u16)i;
            }
        }

        pVB->Unlock();
    }
}

void FShadowForm::Render(float LOD)
{
    HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS,  Device.Caps.soInc );

    // draw front-side of shadow volume in stencil/z only

    HW.pDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, /*pVB*/pVB,
                                        0, S.dwNumVertices,
                                        S.pwShadVolSideIndices,
                                        S.dwNumSideIndices, 0x0);
    if(g_bDrawShadowVolCaps)
    {
        HW.pDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLEFAN,/*pVB*/pVB,
                                            0, S.dwNumVertices,
                                            S.pwShadVolCapIndices,
                                            S.dwNumCapIndices, 0x0);
    }

    // Now reverse cull order so back sides of shadow volume are written.

    // increment stencil buffer value
    HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS,		Device.Caps.soDec );
    HW.pDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,		D3DCULL_CW );

    // Draw back-side of shadow volume in stencil/z only
    HW.pDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLESTRIP, pVB,
                                            0, S.dwNumVertices,
                                            S.pwShadVolSideIndices,
                                            S.dwNumSideIndices, 0x0);

    if(g_bDrawShadowVolCaps)
    {
        HW.pDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLEFAN, pVB,
                                            0, S.dwNumVertices,
                                            S.pwShadVolCapIndices,
                                            S.dwNumCapIndices, 0x0);
    }

    // Restore render states
    HW.pDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
}

