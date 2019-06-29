// MovementControl.cpp: implementation of the CMovementControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\cl_intersect.h"

#include "MovementControl.h"
#include "entity.h"

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

void CMovementControl::dbg_Draw()
{
	if (!bDebug)		return;
	/*
	Fvector P2; P2.add(vPosition,vVelocity);
	RCache.dbg_DrawLINE(Fidentity,vPosition,P2,D3DCOLOR_RGBA(255,255,255,255));
	P2.add(vPosition,vLastMotion);
	RCache.dbg_DrawLINE(Fidentity,vPosition,P2,D3DCOLOR_RGBA(0,255,0,255));
	*/

	Fvector sz,C; aabb.getsize(sz); sz.div(2); aabb.getcenter(C);
	Fmatrix	M = pObject->svXFORM();
	M.c.add (C);
	RCache.dbg_DrawOBB (M,sz,D3DCOLOR_RGBA(0,0,255,255));

	Fvector fct;
	pObject->svXFORM().transform_tiny(fct,vFootCenter);
	Fmatrix fm;
	fm.translate(fct);
	RCache.dbg_DrawOBB (fm,vFootExt,D3DCOLOR_RGBA(255,0,255,255));

	Fmatrix	A;	A.set(pObject->Rotation());
	Fmatrix		xform	=	pObject->svXFORM();
	Fvector		foot_ext, foot_center;
	aabb.getsize(foot_ext);
	foot_ext.set(foot_ext.x*.5f,vFootExt.y,foot_ext.z*.5f);
	xform.transform_tiny(foot_center,vFootCenter);
	A.c.set		(foot_center);
	RCache.dbg_DrawOBB (A,foot_ext,D3DCOLOR_RGBA(255,255,255,255));
/*	
	Fvector		foot_ext;
	foot_ext.set	(2.f*vFootExt.x,vFootExt.y,2.f*vFootExt.z);
	RCache.dbg_DrawAABB (fct,foot_ext.x,foot_ext.y,foot_ext.z,D3DCOLOR_RGBA(255,255,255,255));
*/
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovementControl::CMovementControl()
{
	pObject	=			NULL;
	eOldEnvironment =	peInAir;
	eEnvironment =		peInAir;
	aabb.set			(-def_X_SIZE_2,0,-def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2*2, def_Z_SIZE_2);
	vFootCenter.set		(0,0,0);
	vFootExt.set		(0,0,0);
	fMass				= 100;
	fMinCrashSpeed		= 12.0f;
	fMaxCrashSpeed		= 25.0f;
	vVelocity.set		(0,0,0);
	vPosition.set		(0,0,0);
	vExternalImpulse.set(0,0,0);
	fLastMotionMag		= 1.f;
	fAirFriction		= AIR_FRICTION;
	fWallFriction		= WALL_FRICTION;
	fGroundFriction		= GROUND_FRICTION;
	fFriction			= fAirFriction;
	bIsAffectedByGravity= TRUE;
	fActualVelocity		= 0;
	m_fGroundDelayFactor= 1.f;
	gcontact_HealthLost = 0;
	fContactSpeed		= 0.f;
}

CMovementControl::~CMovementControl()
{

}

float	psSqueezeVelocity;
int		psPhysicsFPS=100;

// Velocity+Friction(dumping), Movement
void Integrate(float& v, float &s, float& a, float dt, float f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	float	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0 = v;
		v += a*Q - (v0 + a*Q)*QmulF;
		// motion
		s += (v0 + v)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0 = v;
	v += a*Q - (v0 + a*Q)*QmulF;

	// motion
	s += (v0 + v)*Qdiv2;
}

float Integrate1D_to	(float v, float &s, float& a,  float f, float s_desired)
{
	float	Q		= 1.f/float(psPhysicsFPS*10.f);
	float	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	while (_abs(s)<_abs(s_desired)) 
	{
		// velocity
		v0 = v;
		v += a*Q - (v0 + a*Q)*QmulF;

		// motion
		s += (v0 + v)*Qdiv2;
	}
	return v0;
}

void Integrate(Fvector& v, Fvector &s, Fvector& a, float dt, float f, float yf=1.f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	Fvector	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0.set(v);
		v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
//		v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
		v.y += a.y*Q - (v0.y + a.y*Q)*(QmulF*yf);
		v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

		// motion
		s.x += (v0.x + v.x)*Qdiv2;
		s.y += (v0.y + v.y)*Qdiv2;
		s.z += (v0.z + v.z)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0.set	(v);
	v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
//	v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
	v.y += a.y*Q - (v0.y + a.y*Q)*(QmulF*yf);
	v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

	// motion
	s.x += (v0.x + v.x)*Qdiv2;
	s.y += (v0.y + v.y)*Qdiv2;
	s.z += (v0.z + v.z)*Qdiv2;
}

void CMovementControl::CalcMaximumVelocity	(float& v, float a, float friction){
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v = a*Q - a*Q*QmulF;

	// sample iterations
	float		S; S=0;
	Integrate	(v,S,a,100,friction);
}

void CMovementControl::CalcMaximumVelocity(Fvector& v, Fvector& a, float friction)
{
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v.x = a.x*Q - a.x*Q*QmulF;
	v.y = a.y*Q - a.y*Q*QmulF;
	v.z = a.z*Q - a.z*Q*QmulF;

	// sample iterations
	Fvector		S; S.set(0,0,0);
	Integrate	(v,S,a,100,friction,1.f);
}

static const float s_fGroundDelaySpeed		= 10.f;
static const float s_fMaxGroundDelayFactor	= 30.f;

/*
void CMovementControl::Calculate(Fvector &_Accel, float ang_speed, float jump, float dt, bool bLight)
{
	Fvector motion,vAccel;
	float	fOldFriction	= fFriction;
	float	fVelocityBefore	= vVelocity.magnitude	();
	float	fVelocityY		= vVelocity.y;

	vExternalImpulse.div(dt);
	vAccel.add			(vExternalImpulse,_Accel);

	// Analyze jump
	bool bJump = (_abs(jump)>EPS) && (eEnvironment==peOnGround);

	// Check for Sleep-State
	bSleep = FALSE;
	float a_speed = _abs(ang_speed);
	float accel_mag = vAccel.magnitude();
	switch (eEnvironment)
	{
	case peOnGround:
		bSleep = (accel_mag<EPS) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	default:
		bSleep = (accel_mag<EPS) && (!bIsAffectedByGravity) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	}
	if (bSleep)	{
		fLastMotionMag		= 0;
		fActualVelocity		= 0;
		vVelocity.set		(0,0,0);
		return;
	}

	Device.Statistic.Physics.Begin();

	// Calculate gravity
	float gravity;
	gravity = psGravity;
	if (eEnvironment==peOnGround && !bJump) gravity *= 0.75f;
	vAccel.y -= bIsAffectedByGravity?gravity:0;

	// Physics integration
	vExternalImpulse.set(0,0,0);
	motion.set			(0,0,0);
	if (bJump){
//		vPosition.y	+= vFootExt.y;
		vVelocity.x	*= 0.9f;
		vVelocity.z	*= 0.9f;
		vVelocity.y = 0; 
		vAccel.y	+= psGravity+jump/dt;
		fFriction	= fAirFriction;
	}

	// stay ground friction
	if ((eEnvironment==peOnGround)&&(_Accel.magnitude()<EPS_L)){
		m_fGroundDelayFactor += s_fGroundDelaySpeed*dt;
		clamp(m_fGroundDelayFactor,1.f,s_fMaxGroundDelayFactor);
	}else m_fGroundDelayFactor = 1.f;
 
	Integrate	(vVelocity,motion,vAccel,dt,fFriction*m_fGroundDelayFactor);

	// Calculate collisions
	Fvector	final_pos,final_vel;
	
	Fbox bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svXFORM(), 
		motion, aabb, bbf, final_pos, final_vel, EPS);
	
	// Velocity stuff
	float s_calc	= motion.magnitude();	// length of motion - dS - requested
	if (bJump)		final_pos.y	+= vFootExt.y;
	motion.sub		(final_pos,vPosition);	// motion - resulting

	if (peAtWall==eEnvironment)
	{
		float s_dummy	= 0;
		float s_desired	= motion.y;
		vVelocity.y		= Integrate1D_to	(fVelocityY,s_dummy,vAccel.y,fOldFriction,s_desired);
	}

	//	Don't allow _new velocity to go against original velocity unless told otherwise
	Fvector vel_dir;
	final_vel.normalize_safe	();
	vel_dir.normalize_safe		(vVelocity);
	float s_res					; // length of motion - dS - resulting
	if (s_calc>EPS_S) 
	{
		s_res					= motion.magnitude();	
		// не ставить final_vel.dotproduct(vel_dir)< (=) 0.f равным нулю!!!
		if ((final_vel.dotproduct(vel_dir)<0.f) || (s_res/s_calc)<0.001f){ 
			vVelocity.set	(0,0,0);
			final_pos.set	(vPosition);
			s_res			= 0;
		} else {
			vPosition.set	(final_pos);
		}
	} else {
		vPosition.set	(final_pos);
		s_res			= 0;
	}
	float src				= 20*dt;	clamp(src,0.f,1.f);
	float dst				= 1-src;
	fLastMotionMag			= dst*fLastMotionMag + src*s_res;
	float	fAvgVelocity	= fLastMotionMag/dt;
	fActualVelocity			= dst*fActualVelocity+ src*fAvgVelocity;
	
	// Environment
	if (fLastMotionMag>EPS_S)
		CheckEnvironment	(vPosition);
 
	// Set movement friction
	switch(eEnvironment)
	{
		case peInAir:		fFriction = fAirFriction;		break;
		case peAtWall:		fFriction = fWallFriction;		break;
		case peOnGround:	fFriction = fGroundFriction;	break;
		default: NODEFAULT;
	}

	// если определили приземление или резко уменьшилась скорость (порядок fMinCrashSpeed)
	// определим возможное повреждение
	gcontact_Was		= FALSE;
	if (
		((eOldEnvironment==peInAir || eOldEnvironment==peAtWall) && (eEnvironment==peOnGround)) // ||
//		((_abs(jump)>EPS) && (eOldEnvironment==peOnGround) && (eEnvironment==peOnGround))
		)
	{
		gcontact_Was		= TRUE;
		gcontact_Power		= 0;
		gcontact_HealthLost = 0;
		if (s_res<=s_calc)	
		{
			float		dummy_s	= 0;
			float		a		= vAccel.magnitude();
			fContactSpeed		= Integrate1D_to	(fVelocityBefore,dummy_s,a,fOldFriction,s_res);
			// s_res, dummy_s ???
			// contact with ground
			gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

			gcontact_HealthLost			= 0;
			if (fContactSpeed>fMinCrashSpeed) 
			{
				//float k=10000.f/(B-A);
				//float dh=_sqrt((dv-A)*k);
				gcontact_HealthLost = 
					(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
			}
		}
	}

	Device.Statistic.Physics.End();

	//
	Engine.Sheduler.Slice();
}

void CMovementControl::Move(Fvector& Dest, Fvector& Motion, BOOL bDynamic)
{
	Device.Statistic.Physics.Begin();
	Fbox	bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	Fvector final_vel;
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svXFORM(), 
		Motion, aabb, bbf, Dest, final_vel, EPS, bDynamic);
	Device.Statistic.Physics.End();

	//
	Engine.Sheduler.Slice();
}
*/

void CMovementControl::CheckEnvironment(const Fvector& newpos)
{
	// verify surface and update Player State
	size_t cp_cnt=pCreator->ObjectSpace.q_result.tris.size();
	eOldEnvironment	= eEnvironment;
	eEnvironment	= peInAir;

	if (cp_cnt)
	{
		Fmatrix33	A; A.set(pObject->Rotation());
		Fmatrix		xform	=	pObject->svXFORM();
		xform.c				=	newpos;
		Fvector		foot_ext, foot_center;
		aabb.getsize(foot_ext);
		foot_ext.set(foot_ext.x*.5f,vFootExt.y,foot_ext.z*.5f);
		xform.transform_tiny(foot_center,vFootCenter);
		for(int i=0; i<(int)cp_cnt; i++)
		{
			clQueryTri& T=pCreator->ObjectSpace.q_result.tris[i];
			// тестируем положение ног
			if (CDB::TestBBoxTri(A,foot_center,foot_ext,&T.p[0],false))
			{
				Fvector N; 
				N.mknormal(T.p[0],T.p[1],T.p[2]);
				if ((N.y>.7f))
				{
					eEnvironment=peOnGround;
					return;
				} else  {
					if((N.y>=0)&&(N.y<=0.7f))
					{
//						eEnvironment=peOnGround;
						eEnvironment=peInAir;//AtWall;
					}
				}
			}
		}
	}
}

void	CMovementControl::SetEnvironment( int enviroment,int old_enviroment){
	switch(enviroment){
	case 0: eEnvironment=peOnGround;
	break;
	case 1: eEnvironment=peAtWall;
	break;
	case 2: eEnvironment=peInAir;
	}
	switch(old_enviroment){
	case 0: eOldEnvironment=peOnGround;
	break;
	case 1: eOldEnvironment=peAtWall;
	break;
	case 2: eOldEnvironment=peInAir;
	}
}