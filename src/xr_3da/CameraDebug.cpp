// CameraDebug.cpp: implementation of the CCameraDebug class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "CameraDebug.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "cameramanager.h"

#define         g_fSpeed        5.0f
#define         g_fAngularSpeed 1.0f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraDebug::CCameraDebug() : CCameraBase(0,0)
{
	// properties
	style = csCamDebug;

	g_vecVelocity = g_vecAngularVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXMatrixTranslation(&g_matPosition, 0.0f, 0.0f, 0.0f);
}

CCameraDebug::~CCameraDebug()
{

}

void CCameraDebug::OnMove()
{
    D3DXVECTOR3 vecT(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vecR(0.0f, 0.0f, 0.0f);

    if(Console.iGetKeyState(DIK_A) || Console.iGetKeyState(DIK_NUMPAD1) || Console.iGetKeyState(DIK_LEFT))  vecT.x -= 1.0f; // Slide Left
    if(Console.iGetKeyState(DIK_D) || Console.iGetKeyState(DIK_NUMPAD3) || Console.iGetKeyState(DIK_RIGHT)) vecT.x += 1.0f; // Slide Right
    if(Console.iGetKeyState(DIK_DOWN))											vecT.y -= 1.0f; // Slide Down
    if(Console.iGetKeyState(DIK_UP))											vecT.y += 1.0f; // Slide Up
    if(Console.iGetKeyState(DIK_S))												vecT.z -= 2.0f; // Move Forward
    if(Console.iGetKeyState(DIK_W))												vecT.z += 2.0f; // Move Backward
    if(Console.iGetKeyState(DIK_NUMPAD2))										vecR.x -= 1.0f; // Pitch Down
    if(Console.iGetKeyState(DIK_NUMPAD8))										vecR.x += 1.0f; // Pitch Up
    if(Console.iGetKeyState(DIK_E) || Console.iGetKeyState(DIK_NUMPAD6))		vecR.y += 1.0f; // Turn Left
    if(Console.iGetKeyState(DIK_Q) || Console.iGetKeyState(DIK_NUMPAD4))		vecR.y -= 1.0f; // Turn Right
    if(Console.iGetKeyState(DIK_NUMPAD9))										vecR.z -= 2.0f; // Roll CW
    if(Console.iGetKeyState(DIK_NUMPAD7))										vecR.z += 2.0f; // Roll CCW

    g_vecVelocity = g_vecVelocity * 0.5f + vecT * 0.5f;
    g_vecAngularVelocity = g_vecAngularVelocity * 0.5f + vecR * 0.5f;

    D3DXMATRIX      matT, matR;
    D3DXQUATERNION  qR;

	float acc;
	if (Console.iGetKeyState(DIK_LSHIFT)) acc=.1f; else acc=1.f;
    vecT = g_vecVelocity * Device.fTimeDelta * g_fSpeed * acc;
    vecR = g_vecAngularVelocity * Device.fTimeDelta * g_fAngularSpeed * acc;

    D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);
    D3DXMatrixMultiply(&g_matPosition, &matT, &g_matPosition);

    D3DXQuaternionRotationYawPitchRoll(&qR, vecR.y, vecR.x, vecR.z);
    D3DXMatrixRotationQuaternion(&matR, &qR);
    D3DXMatrixMultiply(&g_matPosition, &matR, &g_matPosition);

//	if(g_matPosition.m31 < 1.0f)    g_matPosition.m31 = 1.0f;

	Position().set	( g_matPosition._41, g_matPosition._42, g_matPosition._43 );
	vDirection.set	( g_matPosition._31, g_matPosition._32, g_matPosition._33 );
	vNormal.set		( g_matPosition._21, g_matPosition._22, g_matPosition._23 );

	/*
	// Export saved data to camera managers
	Position().set	( savedP );
	vDirection.set	( savedD );
	vNormal.set		( savedN );
	*/
}

void CCameraDebug::OnCameraActivate(CCameraBase* old_cam)
{
	((Fmatrix *)&g_matPosition)->invert(Device.mView);
	savedP.set(Device.vCameraPosition);
	savedD.set(Device.vCameraDirection);
	savedN.set(Device.vCameraTop);
	savedStyle				= old_cam->style;
	savedDF.set				(psDeviceFlags);
	psDeviceFlags.set		(rsClearBB,TRUE);
	Device.seqRender.Add	(this,REG_PRIORITY_HIGH+1111);
	IR_Capture				();
	bDebug					= TRUE;
}
void CCameraDebug::OnCameraDeactivate()
{
	bDebug					= FALSE;
	IR_Release				();
	Device.seqRender.Remove	(this);

	psDeviceFlags.set		(savedDF);
}

void CCameraDebug::OnRender()
{
	Fmatrix mView;
	mView.invert			(*((Fmatrix *)&g_matPosition));
	RCache.set_xform_view	(mView);
//	Device.Frustum.DrawFrustum();
}

void CCameraDebug::IR_OnKeyboardPress(int dik)
{
//	switch (dik)
//	{
//	case DIK_RETURN: g_pGameLevel->Cameras.Activate(savedStyle); break;
//	}
}
