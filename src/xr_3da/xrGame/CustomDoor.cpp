#include "stdafx.h"
#include "CustomDoor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomDoor::CCustomDoor()
{
	state		= edsClose;
}

CCustomDoor::~CCustomDoor()
{	
	::Sound->destroy(sndOpenClose);
}

void CCustomDoor::Load			(LPCSTR section)
{
	inherited::Load				(section);

	//R_ASSERT	(Visual()->Type==MT_SKELETON_ANIM);
	PSkeletonAnimated(Visual())->PlayCycle("close");

	if (pSettings->line_exist(section,"sound"))
	{
		LPCSTR N	= pSettings->r_string(section,"sound");
		::Sound->create(sndOpenClose,TRUE,N);
	}
}

void CCustomDoor::Open(){
	PSkeletonAnimated(Visual())->PlayCycle("open");
	const Fsphere& S = CFORM()->getSphere();
	::Sound->play_at_pos(sndOpenClose,this,S.P);
}

void CCustomDoor::Close(){
	PSkeletonAnimated(Visual())->PlayCycle("close");
	const Fsphere& S = CFORM()->getSphere();
	::Sound->play_at_pos(sndOpenClose,this,S.P);
}

void CCustomDoor::OnMove()
{
//	PKinematics(Visual())->Calculate();
//	PKinematics(Visual())->Update();
	switch (state){
	case edsOpen:		break;
	case edsClose:		break;
	case edsOpening:	break;
	case edsClosing:	break;
	};
}

void CCustomDoor::OnNear( CObject* O )
{
/*
	// check if target
	if (clsid_Target != O->SUB_CLS_ID) return;
	
	// check if not contacted before
	if (find(Contacted.begin(),Contacted.end(),O)!=Contacted.end()) return;
	
	// check if it is actually contacted
	CCF_EventBox* M = (CCF_EventBox*)CFORM(); R_ASSERT	(M);
	if (M->Contact(O)) {
		OnEnter.Signal((u32)O);
		Contacted.push_back(O);
		return;
	}
*/	
	return;
}
