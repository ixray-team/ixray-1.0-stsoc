#include "stdafx.h"
#include "CustomLift.h"

#define LIFT_ACCEL 10.f
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomLift::CCustomLift()
{
	state			= elsStop;
	cur_floor		= 0;
	tgt_floor		= 0;
	floor_count		= 1;
}

CCustomLift::~CCustomLift()
{	
}

void CCustomLift::Load(LPCSTR section)
{
	inherited::Load	(section);
	floor_count		= pSettings->r_s32(section,"floor_count");
}

void CCustomLift::OnMove()
{
	switch (state){
	case elsMove:
		{
			cur_floor	= 0; // calculate current floor
			if (fsimilar(cur_floor,float(tgt_floor))) Stop();
		}
		break;
	case elsStop: break;
	}
}

void CCustomLift::Move(int tgt){
	R_ASSERT((tgt<floor_count)&&(tgt>=0));
	tgt_floor		= tgt;
	state			= elsMove;
}

void CCustomLift::Stop(){
	state			= elsStop;
}

void CCustomLift::OnNear( CObject* O )
{
	return;
}
