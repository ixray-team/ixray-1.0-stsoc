// FShotMarkVisual.cpp: implementation of the FShotMarkVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FShotMarkVisual.h"

#define TIME_TO_HIDE_TOTAL	.8f
#define TIME_TO_HIDE		.4f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FShotMarkVisual::FShotMarkVisual()
{
	fTime = TIME_TO_HIDE_TOTAL;
//	mmx.diffuse.a = 1.f;
	mSelfTransform.identity();
}

FShotMarkVisual::~FShotMarkVisual()
{

}

void FShotMarkVisual::Render(float L)
{
	fTime-=Device.fTimeDelta;
	if (fTime>0 && fTime<TIME_TO_HIDE) {
//		mmx.diffuse.a = fTime/TIME_TO_HIDE;
	}
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE));
	Fvisual::Render(L);
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW));
}
