#include "stdafx.h"
#include "effectorshot.h"
#include "../PSObject.h"
#include "../xr_trims.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponM134.h"
#include "entity.h"

void __stdcall CWeaponM134::RotateCallback_norm(CBoneInstance* B)
{
	CWeaponM134*W		= (CWeaponM134*)B->Callback_Param;
	Fmatrix&	xform	= B->mTransform;
	Fmatrix		spin;

	switch (W->bRotAxisWpn){
	case 0: spin.rotateX	(-W->fRotateAngle); break;
	case 1: spin.rotateY	(W->fRotateAngle);	break;
	case 2: spin.rotateZ	(-W->fRotateAngle); break;
	default: NODEFAULT;
	}
	xform.mulB	(spin);
}

void __stdcall CWeaponM134::RotateCallback_hud(CBoneInstance* B)
{
	CWeaponM134*W		= (CWeaponM134*)B->Callback_Param;
	Fmatrix&	xform	= B->mTransform;
	Fmatrix		spin;

	switch (W->bRotAxisHUD){
	case 0: spin.rotateX	(-W->fRotateAngle); break;
	case 1: spin.rotateY	(-W->fRotateAngle); break;
	case 2: spin.rotateZ	(-W->fRotateAngle); break;
	default: NODEFAULT;
	}

	xform.mulB	(spin);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponM134::CWeaponM134() : CWeapon("M134")
{
	pSounds->Create(sndFireStart,	TRUE,	"weapons\\m134_fire_start"  ,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create(sndFireLoop,	TRUE,	"weapons\\m134_fire_loop"	,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create(sndFireEnd,		TRUE,	"weapons\\m134_fire_end"	,FALSE, SOUND_TYPE_WEAPON_SHOOTING_MACHINEGUN);
	pSounds->Create(sndServo,		TRUE,	"weapons\\m134_servo"		,TRUE,  SOUND_TYPE_WEAPON_CHANGING_MACHINEGUN);

	fRotateSpeed	= 0;
	fRotateAngle	= 0;
	st_current=st_target=eM134Idle;
	iWpnRotBone		= -1;
	iHUDRotBone		= -1;
	iFireBone		= -1;
	
	bRotAxisHUD		= 0;
	bRotAxisWpn		= 0;
	
	dwServoMaxFreq	= 10000;
	dwServoMinFreq	= 22050;
	
	vLastFP.set		(0,0,0);
	vLastFD.set		(0,0,0);
}

CWeaponM134::~CWeaponM134()
{
	FlameUNLOAD();

	// sounds
	pSounds->Delete(sndFireStart);
	pSounds->Delete(sndFireLoop);
	pSounds->Delete(sndFireEnd);
	pSounds->Delete(sndServo);
}

void CWeaponM134::Load	(LPCSTR section)
{
	inherited::Load		(section);
	R_ASSERT(m_pHUD);
	
	LPCSTR rot_bone = pSettings->r_string	(section,"rot_bone");
	
	iWpnRotBone		= PKinematics(Visual())->LL_BoneID(rot_bone);
	iHUDRotBone		= PKinematics(m_pHUD->Visual())->LL_BoneID(rot_bone);
	
	fRotateMaxSpeed		= pSettings->r_float	(section,"rot_max_speed");
	fRotateSpinupAccel	= pSettings->r_float	(section,"rot_spinup_accel");
	fRotateBreakAccel	= pSettings->r_float	(section,"rot_break_accel");
	
	LPCSTR fire_bone= pSettings->r_string	(section,"fire_bone");
	iFireBone		= PKinematics(Visual())->LL_BoneID(fire_bone);
	
	bRotAxisHUD		= pSettings->ReadTOKEN(section,"rot_axis_hud",axis_token);
	bRotAxisWpn		= pSettings->ReadTOKEN(section,"rot_axis_wpn",axis_token);
	
	dwServoMaxFreq	= pSettings->r_s32(section,"servo_max_freq");
	dwServoMinFreq	= pSettings->r_s32(section,"servo_min_freq");
	
	// PKinematics(Visual())->PlayCycle("idle");
	PKinematics(Visual())->LL_GetBoneInstance(iWpnRotBone).set_callback			(RotateCallback_norm,this);
	PKinematics(m_pHUD->Visual())->LL_GetBoneInstance(iHUDRotBone).set_callback	(RotateCallback_hud,this);

	mhud_idle		= m_pHUD->animGet("idle");	
	mhud_fire		= m_pHUD->animGet("fire");	
	mhud_spinup		= m_pHUD->animGet("spinup");	

	FlameLOAD		();
}


void CWeaponM134::FireStart()
{
	if (!IsWorking() && IsValid())
	{
		inherited::FireStart();
		m_pHUD->animPlay		(mhud_spinup);
		st_target = eM134Spinup;
	}
}

void CWeaponM134::FireEnd(){
	if (IsWorking()){
		inherited::FireEnd	();
		m_pHUD->animPlay		(mhud_idle);
		st_target = eM134Brake;
	}
}

void CWeaponM134::UpdateXForm	()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		// 
		Fmatrix mRes;
		CKinematics* V	= PKinematics		(H_Parent()->Visual());
		V->Calculate	();
		Fmatrix& mL		= V->LL_GetTransform(m_pContainer->m_iACTboneL);
		Fmatrix& mR		= V->LL_GetTransform(m_pContainer->m_iACTboneR);

		Fvector			R,D,N;
		D.sub			(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct	(mR.j,D);		R.normalize_safe();
		N.crossproduct	(D,R);			N.normalize_safe();
		mRes.set		(R,N,D,mR.c);
		mRes.mulA		(H_Parent()->XFORM());
		UpdatePosition	(mRes);

		// 
		if (hud_mode) {
			if (m_pHUD)	{
				Fmatrix			trans;
				Level().Cameras.affected_Matrix(trans);
				m_pHUD->UpdatePosition(trans);
			}
		} else {
		}
	}
}

void CWeaponM134::UpdateFP	()
{
	if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		// update animation
		CKinematics* V	= hud_mode?PKinematics(m_pHUD->Visual()):PKinematics(Visual());
		V->Calculate	();

		// fire point&direction
		UpdateXForm				();
		Fmatrix& fire_mat		= V->LL_GetTransform(hud_mode?m_pHUD->iFireBone:iFireBone);
		Fmatrix& parent			= hud_mode?m_pHUD->Transform():XFORM();
		Fvector& fp				= hud_mode?m_pHUD->vFirePoint:vFirePoint;
		Fvector& sp				= hud_mode?m_pHUD->vShellPoint:vShellPoint;
		fire_mat.transform_tiny	(vLastFP,fp);
		parent.transform_tiny	(vLastFP);
		fire_mat.transform_tiny	(vLastSP,sp);
		parent.transform_tiny	(vLastSP);
		vLastFD.set				(0.f,0.f,1.f);
		parent.transform_dir	(vLastFD);
	}
}

void CWeaponM134::Update	(u32 T)
{
	BOOL bShot			= false;

	inherited::Update	(T);
	float dt			= float(T)/1000.f;

	// on state change
	if (st_target!=st_current)
	{
		if (eM134Idle != st_target) UpdateFP();

		switch(st_target){
		case eM134Spinup:
			m_bPending		= TRUE;
			fTime			= 0;
			// play start fire ref_sound
			pSounds->play_at_pos(sndFireStart,	this,vLastFP,false);
			pSounds->play_at_pos(sndServo,		this,vLastFP,true);
			break;
		case eM134Brake:
			// ref_sound fire loop
			sndFireLoop.stop();

			// play end fire ref_sound
			if (st_current==eM134Fire)
				pSounds->play_at_pos	(sndFireEnd,this,vLastFP,false);
			pSounds->play_at_pos	(sndServo,this,vLastFP,true);
			break;
		case eM134Idle:
			m_bPending		= FALSE;
			if (st_current==eM134Brake) {
				sndServo.stop();
			}
			break;
		case eM134Fire:
			if (st_current==eM134Spinup)
			{
				m_pHUD->animPlay		(mhud_fire);
				pSounds->play_at_pos	(sndFireLoop,this,vLastFP,true);
				sndServo.stop			();
			}
			break;
		}
		st_current=st_target;
	}

	// cycle update
	if (eM134Idle != st_current) UpdateFP();
	switch (st_current)
	{
	case eM134Idle:
		break;
	case eM134Spinup:
		fRotateSpeed += fRotateSpinupAccel*dt;
		if (sndServo.feedback){
			sndServo.set_position(vLastFP);
			float k = _sqrt(fRotateSpeed/fRotateMaxSpeed);
			u32 freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.set_frequency(freq);
		}
		if (fRotateSpeed>=fRotateMaxSpeed){
			fRotateSpeed = fRotateMaxSpeed;
			st_target = eM134Fire;
		}
		break;
	case eM134Brake:  
		fRotateSpeed -= fRotateBreakAccel*dt;
		if (sndServo.feedback){
			sndServo.set_position(vLastFP);
			float k = _sqrt(fRotateSpeed/fRotateMaxSpeed);
			u32 freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.set_frequency(freq);
		}
		if (fRotateSpeed<=0){
			st_target		= eM134Idle;
			fRotateSpeed	= 0;
		}
		break;
	case eM134Fire:
		{
			fTime-=dt;
			Fvector p1, d;
			dynamic_cast<CEntity*>(H_Parent())->g_fireParams(p1,d);

			while ((fTime<0)  && iAmmoElapsed)
			{
				fTime			+=fTimeToFire;
				
				if (hud_mode)	{
					CEffectorShot*	S		= (CEffectorShot*)Level().Cameras.GetEffector(cefShot); 
					if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot>(camMaxAngle,camRelaxSpeed));
					R_ASSERT(S); 
					S->Shot					(camDispersion); 
				}

				UpdateFP		();
				FireTrace		(p1,vLastFP,d);
			}

			// ref_sound fire loop
			if (sndFireLoop.feedback) sndFireLoop.set_position(vLastFP);
			if (0==iAmmoElapsed) st_target = eM134Brake;
		}
		break;
	}
	fRotateAngle	+= fRotateSpeed*dt;

#ifdef DEBUG
	pApp->pFont->Out(0,0,"%d",iAmmoElapsed);
	pApp->pFont->Out(0,0.05f,"%.2f",fRotateSpeed);
	pApp->pFont->Out(0,0.1f,"%.2f",fRotateAngle);
#endif
}

void CWeaponM134::renderable_Render	()
{
	inherited::renderable_Render	();
	UpdateXForm				();
	if (hud_mode && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
	{
		// Actor render
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
	if (st_current==eM134Fire) 
	{
		UpdateFP	();
	}
}

void CWeaponM134::SetDefaults	()
{
	CWeapon::SetDefaults();
	iAmmoElapsed = 0;
}

void CWeaponM134::Hide			()
{
	inherited::Hide				();
	signal_HideComplete			();
}
void CWeaponM134::Show			()
{
	inherited::Show				();
}

void CWeaponM134::FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R) 
{
	inherited::FireShotmark		(vDir, vEnd, R);

	// particles
	Fvector N,D;
	CDB::TRI* pTri		= g_pGameLevel->ObjectSpace.GetStaticTris()+R.element;
	N.mknormal			(pTri->V(0),pTri->V(1),pTri->V(2));
	D.reflect			(vDir,N);

	IRender_Sector* S	= ::Render->getSector(pTri->sector);

	// stones or sparks
	LPCSTR ps_gibs		= (Random.randI(5)==0)?"sparks_1":"stones";
	CPSObject* PS		= xr_new<CPSObject> (ps_gibs,S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->play_at_pos		(vEnd);

	// smoke
	PS					= xr_new<CPSObject> ("smokepuffs_1",S,true);
	PS->m_Emitter.m_ConeDirection.set(D);
	PS->play_at_pos		(vEnd);
}
