#include "stdafx.h"

void	CRenderTarget::phase_scene		()
{
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(rt_Position->pRT,		0);
	RCache.set_RT				(rt_Normal->pRT,		1);
	RCache.set_RT				(rt_Color->pRT,			2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();

	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		(psDeviceFlags.test(rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,0,0),1,0
		));
	m_pd3dDevice->Clear			(0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00, 1.0f, 0L);

	m_pd3dDevice->SetRenderState			( D3DRS_CULLMODE,			D3DCULL_CCW			);

	//
	m_pd3dDevice->SetRenderState			( D3DRS_TWOSIDEDSTENCILMODE,FALSE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		TRUE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS		);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILREF,			0x1					);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILMASK,		0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILWRITEMASK,	0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILPASS,		D3DSTENCILOP_REPLACE);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	);
}

