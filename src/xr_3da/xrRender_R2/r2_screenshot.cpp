#include "stdafx.h"
#include "r2.h"
#include "..\tga.h"
#include "..\xrImage_Resampler.h"

#define RGBA_GETALPHA(rgb)      u32((rgb) >> 24)
#define RGBA_GETRED(rgb)        u32(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      u32(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       u32((rgb) & 0xff)

IC u32 convert(float c)
{
	u32 C=iFloor(c);
	if (C>255) C=255;
	return C;
}
IC void MouseRayFromPoint	( Fvector& direction, int x, int y, Fmatrix& m_CamMat )
{
	int halfwidth  = Device.dwWidth/2;
	int halfheight = Device.dwHeight/2;

	Ivector2 point2;
	point2.set(x-halfwidth, halfheight-y);

	float size_y = VIEWPORT_NEAR * tanf( deg2rad(60.f) * 0.5f );
	float size_x = size_y / (Device.fHeight_2/Device.fWidth_2);

	float r_pt	= float(point2.x) * size_x / (float) halfwidth;
	float u_pt	= float(point2.y) * size_y / (float) halfheight;

	direction.mul( m_CamMat.k, VIEWPORT_NEAR );
	direction.mad( direction, m_CamMat.j, u_pt );
	direction.mad( direction, m_CamMat.i, r_pt );
	direction.normalize();
}

void CRender::Screenshot		(LPCSTR postfix, BOOL bSquare)
{
	if (!Device.bReady) return;
	if ((psDeviceFlags.test(rsFullscreen)) == 0) {
		Log("! Can't capture screen while in windowed mode...");
		return;
	}

	// Create temp-surface
	IDirect3DSurface9*	pFB;
	R_CHK(HW.pDevice->CreateOffscreenPlainSurface(
		Device.dwWidth,Device.dwHeight,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,NULL));
	R_CHK(HW.pDevice->GetFrontBufferData(0,pFB));

	D3DLOCKED_RECT	D;
	R_CHK(pFB->LockRect(&D,0,D3DLOCK_NOSYSLOCK));

	// Image processing
	u32* pPixel	= (u32*)D.pBits;
	u32* pEnd		= pPixel+(Device.dwWidth*Device.dwHeight);

	D3DGAMMARAMP G;
	Device.Gamma.GenLUT(G);
	for (int i=0; i<256; i++) {
		G.red	[i]	/= 256;
		G.green	[i]	/= 256;
		G.blue	[i]	/= 256;
	}

	for (;pPixel!=pEnd; pPixel++)
	{
		u32 p = *pPixel;
		*pPixel = D3DCOLOR_XRGB(
			G.red	[RGBA_GETRED(p)],
			G.green	[RGBA_GETGREEN(p)],
			G.blue	[RGBA_GETBLUE(p)]
			);
	}

	string64		buf,t_stemp;
	timestamp		(t_stemp);
	strconcat		(buf,"ss_",Core.UserName,"_",t_stemp);
	if (postfix)	strconcat	(buf,"_#",postfix,".tga");
	else			strcat		(buf,".tga");
	IWriter*		fs  = FS.w_open("$screenshots$",buf); 
	if (0!=fs)		
	{
		TGAdesc			p;
		p.format		= IMG_24B;

		if (bSquare){
			u32* data	= (u32*)xr_malloc(Device.dwHeight*Device.dwHeight*4);
			imf_Process	(data,Device.dwHeight,Device.dwHeight,(u32*)D.pBits,Device.dwWidth,Device.dwHeight,imf_lanczos3);
			p.scanlenght= Device.dwHeight*4;
			p.width		= Device.dwHeight;
			p.height	= Device.dwHeight;
			p.data		= data;
			p.maketga	(*fs);
			xr_free		(data);
		}else{
			// 
			p.scanlenght	= D.Pitch;
			p.width			= Device.dwWidth;
			p.height		= Device.dwHeight;
			p.data			= D.pBits;
			p.maketga	(*fs);
		}

		FS.w_close		(fs);

		R_CHK(pFB->UnlockRect());
		pFB->Release	();
	} else {
		Msg		("! failed to write screenshot to '%s'",buf);
	}
}
