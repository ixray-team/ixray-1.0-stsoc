// CustomFlyer.cpp: implementation of the CCustomFlyer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Flyer.h"
#include "../CameraLook.h"
#include "CameraFirstEye.h"
#include "../environment.h"
#include "../skeletoncustom.h"

#define	FLY_ACCEL		10.0f
#define	YAW_ACCEL		0.75f
#define RUN_COEF		4.0f
#define MAX_CAM_YAW		1.75f

#define FRICTION_AIR	1.75f
#define FRICTION_WALL	5.f
#define FRICTION_GROUND	10.f
//#define AIR_RESIST		(1.f-0.01f)

#define MIN_CRASH_SPEED	15.f
#define MAX_CRASH_SPEED	30.f

IC float sign(float t) { return (t>=0)?1.f:-1.f; }
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlyer::CFlyer() : inherited()
{

	m_PhysicMovementControl->SetCrashSpeeds	(MIN_CRASH_SPEED,MAX_CRASH_SPEED);
	m_PhysicMovementControl->SetParent		(this);
	m_PhysicMovementControl->SetMass		(2000);
	m_PhysicMovementControl->SetApplyGravity(FALSE);
	m_PhysicMovementControl->SetFriction	(FRICTION_AIR,FRICTION_WALL,FRICTION_GROUND);

	cameras[efcFrontView]	= xr_new<CCameraFirstEye>	(this, pSettings, "heli_front_cam", true);
	cameras[efcLookAt]		= xr_new<CCameraLook>		(this, pSettings, "heli_look_cam", false);
	cameras[efcFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "heli_free_cam", false);
	cam_active				= efcFrontView;

	eState		= fsWork;

	new_mstate	= 0;
	cur_mstate	= 0;

	mYaw		= 0;
	mYawVelocity= 0;

//	vOldVel.set(0,0,0);

//	Device.seqRender.Add	(this);
}

CFlyer::~CFlyer()
{
//	Device.seqRender.Remove	(this);

	for (int i=0; i<efcMaxCam; ++i) xr_delete(cameras[i]);
}

void CFlyer::Load(LPCSTR section)
{
	Msg("Loading flyer: %s",section);
	inherited::Load(ini,section);

	Fbox aabb;
	aabb.set				(CFORM()->GetBBox());
//	m_PhysicMovementControl->SetBox			(aabb);
	Fvector vfC,vfE;
	vfE.set					(1.f,.25f,1.f);
	vfC.set					(0.f,.2f,0.f);
	m_PhysicMovementControl->SetFoots		(vfC,vfE);
	m_PhysicMovementControl->CalcMaximumVelocity(mMaxAirVelocity,FLY_ACCEL*RUN_COEF,FRICTION_AIR);
	m_PhysicMovementControl->CalcMaximumVelocity(mMaxYawVelocity,YAW_ACCEL*RUN_COEF,FRICTION_AIR);
//	R_ASSERT	(Visual()->Type==MT_SKELETON);
//	smart_cast<CKinematics*>	(Visual())->PlayCycle("work");
}

void CFlyer::UpdateState(){
	// проверить живой или нет
	eState					= (1)?fsWork:fsDead;
}

void CFlyer::CheckControls(Fvector &vControlAccel, float& fYawAccel)
{
	// control accel
	vControlAccel.set(0,0,0);
	fYawAccel = 0;
	
	// update player accel
	if (cur_mstate&mcUp)		vControlAccel.y =  1;
	if (cur_mstate&mcDown)		vControlAccel.y = -1;

	if (cur_mstate&mcFwd)		vControlAccel.z =  1;
	if (cur_mstate&mcBack)		vControlAccel.z = -1;
	if (cur_mstate&mcLStrafe)	vControlAccel.x = -1;
	if (cur_mstate&mcRStrafe)	vControlAccel.x =  1;
	if (cur_mstate&mcLeft)		fYawAccel		= -1;
	if (cur_mstate&mcRight)		fYawAccel		= 1;

	if ((cam_active==efcFrontView)&&(fYawAccel==0)){
		CCameraBase* cam = cameras[efcFrontView];
		float val = cam->CheckLimYaw();
		float sgn = sign(val);
		if ((val>-.05f)&&(val<0.05f)) val = 0;
		fYawAccel = val;//sgn*val*val;
	}
//	fMouseYawMoving *= 0.8f;
	
	vControlAccel.normalize_safe();
	vControlAccel.mul			(FLY_ACCEL);
	fYawAccel					*= YAW_ACCEL;
	if (isAccelerated())	{
		vControlAccel.mul		(RUN_COEF);
		fYawAccel				*= RUN_COEF;
	}
	if (m_PhysicMovementControl->Environment()==CMovementControl::peOnGround){
		vControlAccel.x*=0.1f;
		vControlAccel.z*=0.1f;
		fYawAccel*=0.1f;
	}

	vControlAccel.y *= 0.75f;
	//if (_abs(vControlAccel.y)>EPS_L) vControlAccel.y -= psGravity*.25f;
}

void CFlyer::SetAnimation( )
{
}

#define BANK_FACTOR 1.25f
void CFlyer::Update(u32 DT)
{
	if (!bEnabled) return;

	float dt		= float(DT)/1000.f;

	// enabled & disabled motion state on current frame
	cur_mstate		= new_mstate;

	// update controls
	Fvector vControlAccel;
	float	fYawAccel;
	CheckControls	(vControlAccel,fYawAccel);

	// prepare safe translate object
	Fvector			accel;
	/*
	switch(m_PhysicMovementControl->Environment())
	{
		case CMovementControl::peInAir:		pApp->pFont->Out(-1.f,0.5f,"Env: air");		break;
		case CMovementControl::peAtWall:	pApp->pFont->Out(-1.f,0.5f,"Env: wall");	break;
		case CMovementControl::peOnGround:	pApp->pFont->Out(-1.f,0.5f,"Env: ground");	break;
		default: NODEFAULT;
	}
	*/

	// rotation
	mYaw			= mYaw + mYawVelocity*dt + fYawAccel*dt*dt*0.5f;
	mYawVelocity	= mYawVelocity + fYawAccel*dt; 
	mYawVelocity	-= dt*mYawVelocity*m_PhysicMovementControl->GetCurrentFriction();

	Fvector D,N;
	D.set(_sin(mYaw),0.f,_cos(mYaw));
	N.set(0,1,0);
	mRotate.rotation(D,N);

	// movement
	mRotate.transform_dir(accel, vControlAccel);

	m_PhysicMovementControl->SetPosition(Position());
	m_PhysicMovementControl->Calculate	(accel,mYawVelocity,0,dt,false);
	m_PhysicMovementControl->GetPosition(Position());

//---------------------------------------------------------
// рассчитаем крен
//---------------------------------------------------------
	const Fvector& VM = m_PhysicMovementControl->GetVelocity();
	Fvector		V;
//	V.set		(vOldVel);
//	V.inertion	(VM,0.999f);
//	vOldVel.set	(VM);
	Fmatrix		M;
	M.transpose	(mRotate);
	M.transform_dir(V,VM);

//	pApp->pFont->Out(-1,0.8f,"Vel: %.2f, %.2f, %.2f",V.x,V.y,V.z);

	// рассчитаем возможный крен по скорости
	V.div		(mMaxAirVelocity*2.f*BANK_FACTOR);
	Fmatrix		Rbank;
	Fquaternion Q;

	// рассчитаем крен по повороту
	float bank = mYawVelocity/(mMaxYawVelocity*BANK_FACTOR);
	bank*=_abs		(V.z*4.f);
	bank+=V.x;

	// повернем модель
	Q.rotationYawPitchRoll(bank,0,-(V.z));
	Rbank.rotation	(Q);
	mRotate.mulB_43	(Rbank);

	// crop yaw angle
	if (mYaw>PI_MUL_2){ 
		mYaw-=PI_MUL_2;
		cameras[efcLookAt]->yaw-=PI_MUL_2;
	}
	if (mYaw<-PI_MUL_2){
		mYaw+=PI_MUL_2;
		cameras[efcLookAt]->yaw+=PI_MUL_2;
	}

	// look camera delay computing
	float vm = V.magnitude()*BANK_FACTOR*2; clamp(vm,0.f,1.f);
	{
		float Yo, k, Ay, z, f;
		Yo	= cameras[efcLookAt]->yaw;
		z	= 1.6f;
		f	= 1.2f+vm;
		k	= PI_MUL_2*f;
		Ay	= (mYaw-Yo)*k*k - Vy*k*z;
		cameras[efcLookAt]->yaw = Yo + Vy*dt + Ay*dt*dt*0.5f;
		Vy	= 0.99f*Vy + Ay*dt;
	}

//---------------------------------------------------------
	// test nearest object
//	Fvector C; float R;
//	m_PhysicMovementControl->GetBoundingSphere(C,R);
//	g_pGameLevel->ObjectSpace.TestNearestObject(CFORM(), C, R);

	// check state
	UpdateState			();

	// Check ground-contact
	if (m_PhysicMovementControl->gcontact_Was) {
	}

	// update motions
	SetAnimation		();


	UpdateTransform		();
//	smart_cast<CKinematics*>(Visual())->Update();

	inherited::Update	(DT);

	if (IsMyCamera()) UpdateCamera();

	// HUD update

//	pApp->pFont->Out(-1,0.88f,"Fly P: %.3f, %.3f, %.3f",Position().x,Position().y,Position().z);
//	pApp->pFont->Out(-1,0.93f,"Fly D: %.3f, %.3f, %.3f",mRotate.k.x,mRotate.k.y,mRotate.k.z);
}

#ifdef DEBUG
void CFlyer::OnRender	()
{
	if (!bDebug)		return;
	m_PhysicMovementControl->dbg_Draw	();
}
#endif
