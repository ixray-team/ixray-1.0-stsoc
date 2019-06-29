#include "stdafx.h"
#include "flyer.h"
#include "../xr_input.h"
#include "../CameraBase.h"


void CFlyer::IR_OnKeyboardPress(int cmd){
	if (eState==fsDead) return;
	switch(cmd){
	case kACCEL:	new_mstate |= mcAccel;		break;
	case kR_STRAFE:	new_mstate |= mcRStrafe;	break;
	case kL_STRAFE:	new_mstate |= mcLStrafe;	break;
	case kFWD:		new_mstate |= mcFwd;		break;
	case kBACK:		new_mstate |= mcBack;		break;
	case kJUMP:		new_mstate |= mcUp;			break;
	case kCROUCH:	new_mstate |= mcDown;		break;
	case kLEFT:		new_mstate |= mcLeft;		break;
	case kRIGHT:	new_mstate |= mcRight;		break;
//	case kFIRE:		FireStart();				break;
	case kCAM_1:	ChangeCamStyle(efcFrontView);break;
	case kCAM_2:	ChangeCamStyle(efcLookAt);	break;
	case kCAM_3:	ChangeCamStyle(efcFreeLook);break;
	}
}

void CFlyer::IR_OnKeyboardRelease(int cmd){
	switch(cmd){
	case kACCEL:	new_mstate &=~mcAccel;		break;
	case kR_STRAFE:	new_mstate &=~mcRStrafe;	break;
	case kL_STRAFE:	new_mstate &=~mcLStrafe;	break;
	case kFWD:		new_mstate &=~mcFwd;		break;
	case kBACK:		new_mstate &=~mcBack;		break;
	case kJUMP:		new_mstate &=~mcUp;			break;
	case kCROUCH:	new_mstate &=~mcDown;		break;
	case kLEFT:		new_mstate &=~mcLeft;		break;
	case kRIGHT:	new_mstate &=~mcRight;		break;
//	case kFIRE:		FireEnd();					break;
	}
}

void CFlyer::IR_OnKeyboardHold(int cmd){
	if (eState==fsDead) return;
	switch(cmd){
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
	case kUP:
	case kDOWN: 
		cameras[cam_active]->Move(cmd); 
		break;
	case kLEFT:
	case kRIGHT: 
//		if (cam_active==efcFreeLook) 
//			cameras[cam_active]->Move(cmd); 
		break;

	}
}

void CFlyer::IR_OnMouseMove(int dx, int dy)
{
	float scale		= psMouseSens * psMouseSensScale/50.f;
	if (efcLookAt!=cam_active)
		if (dx){
			float d = float(dx)*scale;
			cameras[cam_active]->Move((d<0)?kLEFT:kRIGHT, _abs(d));
//			fMouseYawMoving = 1.f;
		}
	if (dy){
		float d = ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*MouseHWScale;
		cameras[cam_active]->Move((d>0)?kUP:kDOWN, _abs(d));
	}
}
