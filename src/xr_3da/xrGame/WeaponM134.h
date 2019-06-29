#ifndef __XR_WEAPON_M134_H__
#define __XR_WEAPON_M134_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;

#define SND_RIC_COUNT 5

class CWeaponM134 : public CWeapon
{
	enum EM134State
	{
		eM134Idle,
		eM134Spinup,
		eM134Brake,
		eM134Fire
	};
	typedef CWeapon inherited;

	static void __stdcall	RotateCallback_norm	(CBoneInstance* B);
	static void __stdcall	RotateCallback_hud	(CBoneInstance* B);
private:
	// General
	float					fTime;
	float					fRotateSpeed;
	float					fRotateAngle;

	float					fRotateMaxSpeed;
	float					fRotateSpinupAccel;
	float					fRotateBreakAccel;

	u32						dwServoMaxFreq;
	u32						dwServoMinFreq;

	ref_sound				sndFireStart;
	ref_sound				sndFireEnd;
	ref_sound				sndFireLoop;
	ref_sound				sndServo;


	int						iWpnRotBone;
	int						iHUDRotBone;
	int						iFireBone;

	u32						dwFP_Frame;
	u32						dwXF_Frame;

	BYTE					bRotAxisHUD;
	BYTE					bRotAxisWpn;

	EM134State				st_current, st_target;

	CMotionDef*				mhud_idle;	
	CMotionDef*				mhud_fire;	
	CMotionDef*				mhud_spinup;
	
	void			DrawFlame			(const Fvector& fp, const Fvector& fd);
	virtual void	UpdateFP			();
	virtual void	UpdateXForm			();
protected:
	void			FlameLOAD			();
	void			FlameUNLOAD			();
	
	virtual void	FireShotmark		(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R);
public:
					CWeaponM134			();
	virtual			~CWeaponM134		();

	// misc
	virtual void	Load				(LPCSTR section);

	virtual void	SetDefaults			();
	virtual void	FireStart			();
	virtual void	FireEnd				();

	virtual void	Hide				();
	virtual void	Show				();

	virtual	void	Update				(u32	dT		);
	virtual	void	renderable_Render	();
};

#endif //__XR_WEAPON_M134_H__
