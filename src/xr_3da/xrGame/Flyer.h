// CustomFlyer.h: interface for the CCustomFlyer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_)
#define AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_
#pragma once

#include "entity.h"
#include "actor_flags.h"

// refs
class CCameraBase;

class CFlyer : public CEntity
{
	enum EMoveCommand{
		mcFwd		= 0x0001,
		mcBack		= 0x0002,
		mcLStrafe	= 0x0004,
		mcRStrafe	= 0x0008,
		mcUp		= 0x0010,
		mcDown		= 0x0020,
		mcAccel		= 0x0100,
		mcLeft		= 0x0200,
		mcRight		= 0x0400
	};
	enum EFlyerCameras {
		efcFrontView	= 0,
		efcLookAt,
		efcFreeLook,
		efcMaxCam
	};
	enum EFlyerState{
		fsWork,
		fsDead
	};

private:
	typedef CEntity		inherited;
private:
	
	// State machine
	EFlyerState			eState;
	u32					new_mstate, cur_mstate;

	float				mYaw;
	float				mYawVelocity;
	float				mMaxYawVelocity;

	float				mMaxAirVelocity;
	IC BOOL				isAccelerated	()	{
		if (cur_mstate&mcAccel) return psActorFlags.test(AF_ALWAYSRUN)?FALSE:TRUE ;
		else					return psActorFlags.test(AF_ALWAYSRUN)?TRUE :FALSE;
	}

	// Cameras
	CCameraBase*		cameras[efcMaxCam];
	EFlyerCameras		cam_active;
	void				ChangeCamStyle		(EFlyerCameras style);

	void				UpdateState			();
	void				UpdateCamera		();
	void				SetAnimation		();
	void				CheckControls		(Fvector &vControlAccel, float& fYawAccel);
public:
						CFlyer				();
	virtual				~CFlyer				();

	virtual void		Load				(LPCSTR section);

	// Fire control
	virtual void		g_fireParams		(const CHudItem* /**pHudItem/**/, Fvector& /**P/**/, Fvector& /**D/**/) {};

	virtual void		HitSignal			( int /**iHitAmount/**/, Fvector& /**vLocalDir/**/, CEntity* /**E/**/, s16 /**element/**/){;}

	virtual void		Update				( u32 DT );

	virtual void		IR_OnMouseMove			(int x, int y);
	virtual void		IR_OnKeyboardPress		(int dik);
	virtual void		IR_OnKeyboardRelease	(int dik);
	virtual void		IR_OnKeyboardHold		(int dik);

#ifdef DEBUG
	virtual void		OnRender			();
#endif
};

#endif // !defined(AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_)
