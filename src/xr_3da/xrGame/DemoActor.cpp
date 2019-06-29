#include "stdafx.h"
#include "CameraFirstEye.h"
#include "../environment.h"

#include "DemoActor.h"
#include "../ObjectAnimator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDemoActor::CDemoActor()
{
	camera			= xr_new<CCameraFirstEye> (this, pSettings, "actor_firsteye_cam", false);
	animator		= 0;
	attached		= 0;
	music			= 0;
}

CDemoActor::~CDemoActor()
{	
	xr_delete		(animator);
	xr_delete		(camera);
	xr_free			(attached);
}

void CDemoActor::Load	(LPCSTR section)
{
	inherited::Load		(section);
}

void CDemoActor::shedule_Update(u32 DT)
{
	inherited::shedule_Update	(DT);
	if (time2start>0)
	{
		time2start-=float(DT)/1000.f;
		if (time2start<0) PlayDemo("demo");
	} else {
		animator->OnMove();
		if (animator->IsMotionActive()) {
			XFORM().set		(animator->GetRotate());
			Position().set	(animator->GetPosition());
			Position().add	(start_position);
		} else {
			// animation stops
			time2start = time2start_ltx;
		}
	}
	if (IsMyCamera()) UpdateCamera();
}

void CDemoActor::PlayDemo(const char* /**name/**/)
{
/*

	animator->PlayMotion		(name,false);
	animator->OnMove			();
	if (music)	music->Play		(false);
	if (attached){
		CObject* O		= g_pGameLevel->Objects.FindObjectByName(attached);
		R_ASSERT		(O);
		R_ASSERT		(O->CLS_ID==CLSID_OBJECT_DUMMY);
		smart_cast<CDummyObject*>(O)->PlayDemo(name);
	}
*/
}

void CDemoActor::StopDemo(){
	animator->StopMotion	();
}

void CDemoActor::g_fireParams(Fvector& P, Fvector& D){
	P.set			(Position());
	D.set			(XFORM().k);
}
