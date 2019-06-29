// WeaponRail.cpp: implementation of the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../cameramanager.h"
#include "../effectorrecoil.h"
#include "../environment.h"
#include "../xr_area.h"
#include "../collide\cl_defs.h"
#include "../fastspriterail.h"

#include "entity.h"
#include "WeaponRail.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponRail::CWeaponRail() : CWeapon("rail")
{
	pTrail			= NULL;
	pSounds->Create	(sndFire,TRUE,"weapons\\rail_fire.ogg");
	bCanFire		= true;
	iAmmoElapsed	= 10;
}

CWeaponRail::~CWeaponRail()
{
	pSounds->Delete	(sndFire);
}

void CWeaponRail::FireStart()
{
	if (!IsWorking() && IsValid() && bCanFire) {
		bCanFire = false;
		CWeapon::FireStart();
		fTime = 0;
	}
}

void CWeaponRail::FireEnd()
{
	if (IsWorking()) {
		CWeapon::FireEnd();
		/*
		VERIFY(pParent);
		if (pParent->bLocal)
			g_pGameLevel->Cameras.AddEffector(NULL,0);
		*/
	}
}

void CWeaponRail::OnMove()
{
//	CWeapon::OnMove();
	if (IsWorking()) {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
//			VERIFY(pParent);
			fTime+=iHitPower;
			bCanFire = false;

			// real fire
			--iAmmoElapsed;
			if (iAmmoElapsed==0) m_pParent->g_fireEnd();

//			BOOL bLocal = pParent->SUB_CLS_ID==CLSID_OBJECT_ACTOR;

//			if (bLocal)
//				g_pGameLevel->Cameras.AddEffector(
//					xr_new<CEffectorRecoil> (.20f),Recoil_TIME);

			Fvector p1,d;
			m_pParent->g_fireParams(p1,d);
			float	dist=g_pGameLevel->Environment.CurrentEnv.Far;

		//	sounds
			Fvector r_pos; r_pos.mad(p1,d,2);
			pSounds->play_at_pos(sndFire,r_pos);

			m_pParent->bEnabled = false;
			Collide::rq_result RQ;
			if (g_pGameLevel->ObjectSpace.RayPick( p1, d, dist, Collide::rqtBoth, RQ )) {
				if ( RQ.range>0.3f) 
				{
					SetRail		(p1,d,RQ,TRUE);
					if (RQ.O)	{
						if (RQ.O->CLS_ID == CLSID_ENTITY)
							dynamic_cast<CEntity*>(RQ.O)->Hit(iHitPower,d,m_pParent);
					}
				}
			} else {
				RQ.range	= g_pGameLevel->Environment.CurrentEnv.Far;
				SetRail		(p1,d,RQ,FALSE);
			}
			m_pParent->bEnabled = true;
		}
	} else {
		fTime-=Device.fTimeDelta;
		if (fTime<0) {
			bCanFire = true;
		}
	}
	if (pTrail)
	{
		if (pTrail->isReadyForDestroy())	{xr_delete(pTrail);}
		else								pTrail->Update();
	}
}

void CWeaponRail::SetDefaults()
{
	CWeapon::SetDefaults();
	bCanFire		= true;
	iAmmoElapsed	= 1000;
}

void CWeaponRail::SetRail(Fvector &p1, Fvector &d, Collide::rq_result& R, BOOL bWallmark)
{
	float dist = R.range;
	if (dist>50.f) dist=50.f;
	Fvector vStart,vEnd;
	vStart.mad(p1,d,.2f);
	vEnd.mad(p1,d,dist);
	xr_delete(pTrail);

	if (bWallmark) AddShotmark(d,vEnd,R);
}

