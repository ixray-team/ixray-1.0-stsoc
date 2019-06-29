#include "stdafx.h"
#include "DemoActor.h"
#include "../CameraBase.h"

void CDemoActor::UpdateCamera()
{
	camera->Set	(Position(),XFORM().k,XFORM().j);
	g_pGameLevel->Cameras().Update	(camera);
}
