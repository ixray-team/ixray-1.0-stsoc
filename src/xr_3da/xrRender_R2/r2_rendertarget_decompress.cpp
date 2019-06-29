#include "stdafx.h"

void	CRenderTarget::phase_decompress		()
{
	if (RImplementation.b_fp16)	return;

	// Common
	RCache.set_CullMode			(CULL_NONE);
	RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);

	Fvector2					p0,p1;
	u32		Offset;
	u32		C					= D3DCOLOR_RGBA	(255,255,255,255);
	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	d_Z					= EPS_S;
	float	d_W					= 1.f;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	struct v_dc		{
		Fvector4	p;
		Fvector2	uv0;
		Fvector4	uv1;
		void		set			(Fmatrix& mUnwarp, float x, float y, float u0, float v0, float u1, float v1)	{
			p.set	(x,y,EPS_S,1);
			uv0.set	(u0,v0);
			uv1.set	(u1*mUnwarp._11,	v1*mUnwarp._22,	1,	0);
			// Msg		("[%+1.3f]-[%+1.3f] : dx(%f),dy(%f),dz(%f)",u1,v1,uv1.x,uv1.y,uv1.z);
		}                                                                                                                                                                                                           
	};
	VERIFY						(g_decompress->vb_stride == sizeof(v_dc));
	float			w1			= -1.f/float(Device.dwWidth);
	float			h1			= -1.f/float(Device.dwHeight);
	float			ws			= 1.0f * float(Device.dwWidth-1)	/float(Device.dwWidth);
	float			hs			= 1.0f * float(Device.dwHeight-1)	/float(Device.dwHeight);

	// Fill vertex buffer
	Fmatrix						mUnwarp;
	D3DXMatrixInverse			((D3DXMATRIX*)&mUnwarp,0,(D3DXMATRIX*)&Device.mProject);
	// Msg						("w=%f,h=%f",mUnwarp._11*VIEWPORT_NEAR,mUnwarp._22*VIEWPORT_NEAR);
	v_dc* pv					= (v_dc*) RCache.Vertex.Lock	(4,g_decompress->vb_stride,Offset);
	pv->set						(mUnwarp,	0,				float(_h+0),	p0.x, p1.y, -1*ws+w1,	-1*hs+h1);	pv++;
	pv->set						(mUnwarp,	0,				0,				p0.x, p0.y, -1*ws+w1,	+1*hs+h1);	pv++;
	pv->set						(mUnwarp,	float(_w+0),	float(_h+0),	p1.x, p1.y, +1*ws+w1,	-1*hs+h1);	pv++;
	pv->set						(mUnwarp,	float(_w+0),	0,				p1.x, p0.y, +1*ws+w1,	+1*hs+h1);	pv++;
	RCache.Vertex.Unlock		(4,g_decompress->vb_stride);
	RCache.set_Geometry			(g_decompress);

	// Decompress
	if (RImplementation.b_mrt)
	{
		u_setrt						(rt_Position,rt_Normal,NULL,HW.pBaseZB);
		RCache.set_Shader			(s_decompress);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	} else {
		u_setrt						(rt_Position,	NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_decompress->E[1]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
		u_setrt						(rt_Normal,		NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_decompress->E[2]);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}
}
