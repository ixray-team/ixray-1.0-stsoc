#include "stdafx.h"
#include "effectornoise.h"
#include "../PSObject.h"
#include "../PSVisual.h"
#include "../xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponProtecta.h"
#include "entity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponProtecta::CWeaponProtecta() : CWeapon("PROTECTA")
{
	pSounds->Create(sndShoot,		TRUE,	"weapons\\protecta_fire");

	iShotCount		= 0;

	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);

	fTime			= 0;
}

CWeaponProtecta::~CWeaponProtecta()
{
	// sounds
	pSounds->Delete(sndShoot);
	xr_delete			(m_pShootPS);
}

void CWeaponProtecta::Load(LPCSTR section){
	inherited::Load(ini, section);
	R_ASSERT(m_pHUD);
	
	m_pShootPS		= xr_new<CPSObject> ("protecta_smoke");

	vFirePoint		= ini->r_fvector3(section,"fire_point");
	iShotCount		= ini->r_s32(section,"shot_count");
}

void CWeaponProtecta::FireStart()
{
	if (!IsWorking() && IsValid()){ 
		CWeapon::FireStart();
		st_target	= eShoot;
	}
}

void CWeaponProtecta::FireEnd(){
	if (IsWorking()){
		CWeapon::FireEnd();
		m_pHUD->FireEnd();
		st_target = eIdle;
	}
}


void CWeaponProtecta::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (bHUDView) {
			if (m_pHUD)	{
				Fmatrix			trans;
				trans.set(
					Device.vCameraRight,
					Device.vCameraTop,
					Device.vCameraDirection,
					Device.vCameraPosition
					);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
			Fmatrix mRes;
			CKinematics* V	= PKinematics(m_pParent->Visual());
			V->Calculate	();
			Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
			Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);
			
			Fvector			R,D,N;
			D.sub			(mL.c,mR.c);	D.normalize_safe();
			R.crossproduct	(mR.j,D);	R.normalize_safe();
			N.crossproduct	(D,R);	N.normalize_safe();
			mRes.set		(R,N,D,mR.c);
			mRes.mul2		(m_pParent->XFORM());
			UpdatePosition	(mRes);
		}
	}
}

void CWeaponProtecta::UpdateFP(BOOL bHUDView)
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		CKinematics* V			= bHUDView?PKinematics(m_pHUD->Visual()):0;
		if (V) V->Calculate		();

		// fire point&direction
		UpdateXForm				(bHUDView);
		Fmatrix& fire_mat		= bHUDView?V->LL_GetTransform(m_pHUD->iFireBone):precalc_identity;
		Fmatrix& parent			= bHUDView?m_pHUD->Transform():XFORM();
		Fvector& fp				= bHUDView?m_pHUD->vFirePoint:vFirePoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponProtecta::Update(float dt, BOOL bHUDView)
{
	BOOL bShot = false;

	inherited::Update(dt,bHUDView);
	
	// on state change
	if (st_target!=st_current)
	{
		if (st_target == eShoot) UpdateFP(bHUDView);

		switch(st_target){
		case eIdle:
			break;
		case eShoot:
			pSounds->play_at_pos(sndShoot,vLastFP,true);
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (st_current == eShoot) UpdateFP(bHUDView);

	switch (st_current)
	{
	case eIdle:
		fTime	-= dt;
		if (fTime<0)	fTime = 0;
		break;
	case eShoot:
		{
			fTime-=dt;
			Fvector p1_base, d_base;
			m_pParent->g_fireParams(p1_base,d_base);
			
			while (fTime<0)
			{
				fTime			+=fTimeToFire;
				UpdateFP		(bHUDView);

				// play smoke
				m_pShootPS->Stop		();
				m_pShootPS->m_Emitter.m_ConeDirection.set	(vLastFD);
				m_pShootPS->play_at_pos	(vLastFP);

				BOOL			bHit = FALSE;
				for (int i=0; i<iShotCount; ++i)
				{
					Fvector p1=p1_base, d=d_base;
					bHit |=		FireTrace(p1,vLastFP,d);
				}
				--iAmmoElapsed;
		 		if (iAmmoElapsed==0) { m_pParent->g_fireEnd(); break; }
				m_pHUD->Shoot	();
			}

			// ref_sound fire loop
			if (sndShoot.feedback) sndShoot.set_position(vLastFP);
		}
		break;
	}

	m_pHUD->UpdateAnimation();
}

void CWeaponProtecta::Render(BOOL bHUDView)
{
	UpdateXForm	(bHUDView);
	if (bHUDView)
	{ 
		// Interpolate lighting
		float l_f	= Device.fTimeDelta*fLightSmoothFactor;
		float l_i	= 1.f-l_f;
		float&	LL	= m_pParent->AI_Lighting;
		CLevelGraph::CVertex*	NODE = m_pParent->level_vertex();
		float	CL	= NODE?float(NODE->light):255;
		LL			= l_i*LL + l_f*CL; 

		// HUD render
		if (m_pHUD){ 
			HUD().RenderModel(m_pHUD->Visual(),m_pHUD->Transform(),iFloor(LL));
			// Render PS Shoot
			HUD().RenderModel(m_pShootPS->Visual(),precalc_identity,255);
		}
	}
	else
	{
		// Actor render
		::Render.set_Transform		(&XFORM());
		::Render.set_LightLevel		(iFloor(m_pParent->AI_Lighting));
		::Render.add_leafs_Dynamic	(Visual());

		// Render PS Shoot
		::Render.add_leafs_Dynamic	(m_pShootPS->Visual());
	}
}

void CWeaponProtecta::SetDefaults()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 0;
}

void CWeaponProtecta::Hide		()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponProtecta::Show		()
{
	inherited::Show				();
}

void CWeaponProtecta::FireShotmark(const Fvector &vDir, const Fvector &vEnd, Collide::rq_result& R) 
{
	inherited::FireShotmark	(vDir, vEnd, R);

	// particles
	Fvector N,D;
	RAPID::tri* pTri	= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	IRender_Sector* S	= ::Render.getSector(pTri->sector);

	// stones
	CPSObject* PS		= xr_new<CPSObject> ("stones",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->play_at_pos		(vEnd);

	// smoke
	PS					= xr_new<CPSObject> ("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->play_at_pos		(vEnd);
}
