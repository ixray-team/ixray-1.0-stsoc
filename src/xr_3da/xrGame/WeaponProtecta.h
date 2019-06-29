#ifndef __XR_WEAPON_PROTECTA_H__
#define __XR_WEAPON_PROTECTA_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;
class ENGINE_API CPSObject;

#define SND_RIC_COUNT 5

class CWeaponProtecta: public CWeapon
{
	enum EGrozaState
	{
		eIdle,
		eReload,
		eShoot
	};
	typedef CWeapon inherited;

private:
	// General
	float			fTime;

	ref_sound			sndShoot;

	Fvector			vFirePoint;

	Fvector			vLastFP, vLastFD;
	u32			dwFP_Frame;
	u32			dwXF_Frame;

	int				iShotCount;
	
	EGrozaState		st_current, st_target;
	
	CPSObject*		m_pShootPS;

	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			UpdateFP		(BOOL bHUD);
	void			UpdateXForm		(BOOL bHUD);

protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::rq_result& R);
public:
					CWeaponProtecta	();
	virtual			~CWeaponProtecta();

	// misc
	virtual void	Load			(LPCSTR section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	
	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);
};

#endif //__XR_WEAPON_PROTECTA_H__
