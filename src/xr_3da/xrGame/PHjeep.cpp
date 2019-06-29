/////////////////////////////////////////////////////////////////////////////
/////////////CPHJeep////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
///////////////////////////////////////////////////////////////
//#pragma warning(disable:4995)
//#include "..\ode\src\collision_kernel.h"
//#include <..\ode\src\joint.h>
//#include <..\ode\src\objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"
#include "PHJeep.h"

void CPHJeep::Create1(dSpaceID space, dWorldID world){
	if(bActive) return;
	static const dReal scaleParam=1.f;
	static const dVector3 scaleBox={scaleParam, scaleParam, scaleParam};
	//jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
	jeepBox[0]=REAL(4.2)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(2.08)*scaleBox[2];
	//jeepBox[0]=REAL(1.0)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(1.3)*scaleBox[2];
	cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*2.08f;
	//cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*1.3f;
	static const dReal wheelRadius = 0.79f/2.f* scaleParam;
	

	static const dVector3 startPosition={6.0f,12.f,0.f};
	static const dReal weelSepX=scaleBox[0]*2.74f/2.f,weelSepZ=scaleBox[2]*1.7f/2.f,weelSepY=scaleBox[1]*0.6f;

	dMass m;

	// car body
	//dMass m;
	dMassSetBox(&m, 1, jeepBox[0], jeepBox[1], jeepBox[2]); // density,lx,ly,lz
	dMassAdjust(&m, 50); // mass

	Bodies[0] = dBodyCreate(world);
	dBodySetMass(Bodies[0], &m);
	dBodySetPosition(Bodies[0], startPosition[0], startPosition[1], startPosition[2]); // x,y,z

	Geoms[0] = dCreateBox(0, jeepBox[0], jeepBox[1], jeepBox[2]); // lx,ly,lz
	Geoms[6] = dCreateBox(0, cabinBox[0], cabinBox[1], cabinBox[2]); // lx,ly,lz

	//dGeomSetBody(Geoms[5], Bodies[5]);
	Geoms[5]=dCreateGeomTransform(space);
	//Geoms[7]=dCreateGeomTransform(0);
	dGeomSetPosition(Geoms[6], -jeepBox[0]/2.f+cabinBox[0]/2+0.55f, cabinBox[1]/2.f+jeepBox[1]/2.f, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[0], 0,0/*-jeepBox[1]-wheelRadius*/, 0); // x,y,z
	dGeomTransformSetGeom(Geoms[5],Geoms[6]);
	//dGeomTransformSetGeom(Geoms[7],Geoms[0]);


	dGeomSetBody(Geoms[5], Bodies[0]);
	dGeomSetBody(Geoms[0], Bodies[0]);



	

	// wheel bodies
	dMassSetSphere(&m, 1, wheelRadius); // density, radius
	dMassAdjust(&m, 10); // mass
	dQuaternion q;
	dQFromAxisAndAngle(q, 1, 0, 0, M_PI * 0.5);
	u32 i;
	for(i = 1; i <= 4; ++i)
	{
		Bodies[i] = dBodyCreate(world);
		dBodySetMass(Bodies[i], &m);
		dBodySetQuaternion(Bodies[i], q);
		Geoms[i] = dCreateSphere(0, wheelRadius);
		dGeomSetBody(Geoms[i], Bodies[i]);
	}




	dBodySetPosition(Bodies[1], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
	dBodySetPosition(Bodies[2], startPosition[0]-weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
	dBodySetPosition(Bodies[3], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
	dBodySetPosition(Bodies[4], startPosition[0]+weelSepX, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z



	// wheel joints
	for(i = 0; i < 4; ++i)
	{
		Joints[i] = dJointCreateHinge2(world, 0);
		dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
		const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
		dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
		dJointSetHinge2Axis1(Joints[i], 0.f, 1.f, 0.f);
		dJointSetHinge2Axis2(Joints[i], 0.f, 0.f, ((i % 2) == 0) ? -1.f : 1.f);

		dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
		dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax, 50);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 80);

		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, 0.3f);
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 0.0003f);
	}

	GeomsGroup = dCreateGeomGroup(space);  
	for(i = 0; i < NofGeoms-1; ++i)
		dGeomGroupAdd(GeomsGroup, Geoms[i]);



	//dynamic data
	CreateDynamicData();
	bActive=true;
}

void __stdcall CarHitCallback(bool& do_colide,dContact& c);
/////////////////////////////////////////////////////////////////////////////
dReal car_spring_factor=1.f;
dReal car_damping_factor=1.f;
void CPHJeep::Create(dSpaceID space, dWorldID world){
	if(bActive) return;
	static const dReal scaleParam=1.f;
	static const dVector3 scaleBox={scaleParam, scaleParam, scaleParam};
	//jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
	//jeepBox[0]=REAL(4.2)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(2.08)*scaleBox[2];
	jeepBox[0]=REAL(3.680)*scaleBox[0];jeepBox[1]=REAL(0.612)*scaleBox[1];jeepBox[2]=0.88f*2.f*scaleBox[2];
	//cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*2.08f;
	cabinBox[0]=scaleBox[0]*1.7f;cabinBox[1]=scaleBox[1]*0.66f;cabinBox[2]=scaleBox[2]*2.f*0.76f;

	static const dReal wheelRadius = 0.345f* scaleParam;

	VelocityRate=3.f;
	DriveForce=0;
	DriveVelocity=12.f * M_PI;
	DriveDirection=0;
	Breaks=false;
	startPosition[0]=10.0f;startPosition[1]=1.f;startPosition[2]=0.f;
	//static const dReal weelSepX=scaleBox[0]*2.74f/2.f,weelSepZ=scaleBox[2]*1.7f/2.f,weelSepY=scaleBox[1]*0.6f;
	static const dReal weelSepXF=scaleBox[0]*1.32f,weelSepXB=scaleBox[0]*1.155f,weelSepZ=scaleBox[2]*1.53f/2.f,weelSepY=scaleBox[1]*0.463f;
	static const dReal cabinSepX=scaleBox[0]*0.61f,cabinSepY=scaleBox[1]*0.55f;
	//MassShift=0.25f;
	dMass m;

	// car body
	//dMass m;
	dMassSetBox(&m, 1.f, jeepBox[0], jeepBox[1]/4.f, jeepBox[2]); // density,lx,ly,lz
	dMassAdjust(&m, 400.f); // mass//800
	//dMassTranslate(&m,0.f,-1.f,0.f);
	Bodies[0] = dBodyCreate(world);
	dBodySetMass(Bodies[0], &m);
	dBodySetPosition(Bodies[0], startPosition[0], startPosition[1]-MassShift, startPosition[2]); // x,y,z

	Geoms[0] = dCreateBox(0, jeepBox[0], jeepBox[1], jeepBox[2]); // lx,ly,lz
	Geoms[6] = dCreateBox(0, cabinBox[0], cabinBox[1], cabinBox[2]); // lx,ly,lz

	//dGeomSetBody(Geoms[5], Bodies[5]);
	Geoms[5]=dCreateGeomTransform(0);
	Geoms[7]=dCreateGeomTransform(0);
	//dGeomCreateUserData(Geoms[5]);
	//dGeomCreateUserData(Geoms[7]);
	dGeomCreateUserData(Geoms[0]);
	dGeomCreateUserData(Geoms[6]);
	dGeomGetUserData(Geoms[0])->material=GMLib.GetMaterialIdx("materials\\car_cabine");
	dGeomGetUserData(Geoms[6])->material=GMLib.GetMaterialIdx("materials\\car_cabine");

	dGeomUserDataSetObjectContactCallback(Geoms[0],CarHitCallback);
	dGeomUserDataSetObjectContactCallback(Geoms[6],CarHitCallback);
	dGeomUserDataSetContactCallback(Geoms[0],ContactShotMark);
	dGeomUserDataSetContactCallback(Geoms[6],ContactShotMark);
	if(m_ref_object)
	{
		dGeomUserDataSetPhysicsRefObject(Geoms[0],m_ref_object);
		dGeomUserDataSetPhysicsRefObject(Geoms[6],m_ref_object);
	}
	//dGeomGetUserData(Geoms[5])->friction=500.f;
	//dGeomGetUserData(Geoms[7])->friction=500.f;

	dGeomSetPosition(Geoms[0], 0.f, MassShift, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[6], -jeepBox[0]/2.f+cabinBox[0]/2.f+0.55f, cabinBox[1]/2.f+jeepBox[1]/2.f+MassShift, 0.f); // x,y,z
	dGeomSetPosition(Geoms[6], -cabinSepX, cabinSepY+MassShift, 0.f); // x,y,z
	//dGeomSetPosition(Geoms[0], 0,0/*-jeepBox[1]-wheelRadius*/, 0); // x,y,z

	dGeomTransformSetGeom(Geoms[5],Geoms[6]);
	dGeomTransformSetGeom(Geoms[7],Geoms[0]);

	dGeomTransformSetInfo(Geoms[5],1);
	dGeomTransformSetInfo(Geoms[7],1);

	dGeomSetBody(Geoms[5], Bodies[0]);
	dGeomSetBody(Geoms[7], Bodies[0]);

	// wheel bodies
	dMassSetSphere(&m, 1, wheelRadius); // density, radius
	dMassAdjust(&m, 100); // mass 20
	dQuaternion q;
	dQFromAxisAndAngle(q, 1, 0, 0, M_PI * 0.5);
	u32 i;
	for(i = 1; i <= 4; ++i)
	{
		Bodies[i] = dBodyCreate(world);
		dBodySetMass(Bodies[i], &m);
		dBodySetQuaternion(Bodies[i], q);
		//Geoms[i] = dCreateSphere(0, wheelRadius);
		Geoms[i] = dCreateCylinder(0, wheelRadius,0.19f);
		dGeomCreateUserData(Geoms[i]);
		dGeomGetUserData(Geoms[i])->material=GMLib.GetMaterialIdx("materials\\rubber");
		dGeomSetBody(Geoms[i], Bodies[i]);
	}




	dBodySetPosition(Bodies[1], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
	dBodySetPosition(Bodies[2], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
	dBodySetPosition(Bodies[3], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
	dBodySetPosition(Bodies[4], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z



	// wheel joints
	for(i = 0; i < 4; ++i)
	{
		Joints[i] = dJointCreateHinge2(world, 0);
		dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
		const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
		dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
		dJointSetHinge2Axis1(Joints[i], 0.f, 1.f, 0.f);
		dJointSetHinge2Axis2(Joints[i], 0.f, 0.f, ((i % 2) == 0) ? -1.f : 1.f);

		dJointSetHinge2Param(Joints[i], dParamLoStop, 0.f);
		dJointSetHinge2Param(Joints[i], dParamHiStop, 0.f);
		dJointSetHinge2Param(Joints[i], dParamFMax, 10000.f );
		dJointSetHinge2Param(Joints[i], dParamFudgeFactor, 0.001f);

		dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
		dJointSetHinge2Param(Joints[i], dParamFMax2, 500.f);
		dReal k_p=20000.f*car_spring_factor;
		dReal k_d=1000.f*car_damping_factor;
		dReal h=fixed_step;


		dJointSetHinge2Param(Joints[i], dParamSuspensionERP, h*k_p / (h*k_p + k_d));
		dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (h*k_p + k_d));

#ifndef   ODE_SLOW_SOLVER

		dJointSetHinge2Param(Joints[i], dParamStopERP, 1.f);
		dJointSetHinge2Param(Joints[i], dParamStopCFM,0.f);// world_cfm/100.f
#endif

	}


	GeomsGroup = dCreateGeomGroup(space);  
	for(i = 1; i < NofGeoms-1; ++i)
		dGeomGroupAdd(GeomsGroup, Geoms[i]);
	dGeomGroupAdd(GeomsGroup, Geoms[7]);


	//dynamic data
	CreateDynamicData();
	bActive=true;
}
////////////////////////////////////////////////////////////////
void CPHJeep::JointTune(dReal step){
	/////const	dReal k_p=15000.f;//30000.f;
//	const	dReal k_d=1000.f;//1000.f;
	//for(u32 i = 0; i < 4; ++i)
	//{


	//	dJointSetHinge2Param(Joints[i], dParamSuspensionERP, step*k_p / (step*k_p + k_d));
	//	dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (step*k_p + k_d));

	//}
	static const dReal w_limit = M_PI/16.f/0.02f;
	const dReal* rot = dBodyGetAngularVel(Bodies[0]);
	dReal mag=_sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
	if(mag>w_limit){
		dReal f=mag/w_limit;
		dBodySetAngularVel(Bodies[0],rot[0]/f,rot[1]/f,rot[2]/f);
	}
}
/////////
void CPHJeep::Destroy(){
	if(!bActive) return;
	for(u32 i=0;i<NofGeoms;i++) dGeomDestroyUserData(Geoms[i]);
	DynamicData.Destroy();

	for(u32 i=0;i<NofJoints;i++) dJointDestroy(Joints[i]);
	for(u32 i=0;i<NofBodies;i++) dBodyDestroy(Bodies[i]);

	dGeomDestroyUserData(Geoms[5]);
	dGeomDestroyUserData(Geoms[7]);
	dGeomDestroyUserData(Geoms[0]);
	dGeomDestroyUserData(Geoms[6]);

	for(u32 i=0;i<5;i++) {
		dGeomDestroy(Geoms[i]);
	}
	dGeomDestroy(Geoms[6]);
	//	dGeomDestroy(Geoms[5]);
	dGeomDestroy(Geoms[7]);
	dGeomDestroy(GeomsGroup);

	bActive=false;
}

void CPHJeep::Steer1(const char& velocity, const char& steering)
{
	if(!bActive) return;
	static const dReal steeringRate = M_PI * 4 / 3;
	static const dReal steeringLimit = M_PI / 6;
	static const dReal wheelVelocity = 1.f * M_PI;
	ULONG i;
	switch(steering)
	{
	case 1:
	case -1:
		for(i = 2; i < 4; ++i)
		{
			dJointSetHinge2Param(Joints[i], dParamHiStop, steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamLoStop, -steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamVel, ((i < 2) ? steering : -steering) * steeringRate);
		}
		break;

	default: // case 0
		for(i = 0; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);
			if(angle < 0)
			{
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, steeringRate);
			}
			else
			{
				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, -steeringRate);
			}
		}
		break;
	}

	switch(velocity)
	{
	case 1:
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -wheelVelocity : wheelVelocity);
		break;

	case -1:
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? wheelVelocity : -wheelVelocity);
		break;

	default: // case 0
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		break;
	}
}

///////////////////////////////////////////////////////////////
void CPHJeep::CreateDynamicData(){
	DynamicData.Create(4,Bodies[0]);
	DynamicData.SetChild(0,0,Bodies[1]);
	DynamicData.SetChild(1,0,Bodies[2]);
	DynamicData.SetChild(2,0,Bodies[3]);
	DynamicData.SetChild(3,0,Bodies[4]);
	DynamicData.SetTransform(Geoms[5]);
	DynamicData.SetGeom(Geoms[0]);
	Fmatrix Translate;

	DynamicData.CalculateData();
	DynamicData.SetAsZeroRecursive();
	Translate.identity();
	Translate.translate(0,-MassShift+0.810f,0);
	DynamicData.SetZeroTransform(Translate);

}

dReal steeringRate = M_PI * 4 / 5;
dReal steeringLimit = M_PI / 4;
void CPHJeep::Steer(const char& steering)
{
	if(!bActive) return;


	ULONG i;
	switch(steering)
	{
	case 1:
	case -1:
		weels_limited=true;
		for(i = 2; i < 4; ++i)
		{
			dJointSetHinge2Param(Joints[i], dParamHiStop, steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamLoStop, -steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamVel, ((i < 2) ? steering : -steering) * steeringRate);
		}
		break;

	default: // case 0
		weels_limited=false;
		for(i = 2; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);


			if(angle < 0)
			{
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);

				dJointSetHinge2Param(Joints[i], dParamVel, steeringRate);
			}
			else
			{	

				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, -steeringRate);
			}

		}
		break;
	}

}

void CPHJeep::LimitWeels()
{
	if(!bActive) return;
	if(weels_limited) return;

	for(int i = 2; i < 4; ++i)
	{
		dReal angle = dJointGetHinge2Angle1(Joints[i]);
		if(dFabs(angle)<M_PI/180.f)
		{

			dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
			dJointSetHinge2Param(Joints[i], dParamLoStop, 0);///
			dJointSetHinge2Param(Joints[i], dParamVel, 0);
			weels_limited=true;
		}
	}

}
////////////////////////////////////////////////////////////////
void CPHJeep::Drive(const char& velocity,dReal force)
{
	if(!bActive) return;

	static const dReal wheelVelocity = 12.f * M_PI;//3*18.f * M_PI;
	ULONG i;


	switch(velocity)
	{
	case 1:
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -wheelVelocity*VelocityRate : wheelVelocity*VelocityRate);
		break;

	case -1:
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? wheelVelocity*VelocityRate : -wheelVelocity*VelocityRate);
		break;

	default: // case 0
		for(i = 0; i < 4; ++i)
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		break;
	}
	for(i=0;i<4;i++)
		dJointSetHinge2Param(Joints[i], dParamFMax2, force);
}
//////////////////////////////////////////////////////////

dReal car_neutral_drive_resistance=100.f;
dReal car_breaks_resistance		  =5000.f;

void CPHJeep::Drive()
{

	//	static const dReal wheelVelocity = 12.f * M_PI;//3*18.f * M_PI;
	ULONG i;

	if(!Breaks)
		switch(DriveDirection)
	{
		case 1:
			for(i = 0; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
			break;

		case -1:
			for(i = 0; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
			break;
		case 0:
			for(i = 0; i < 4; ++i){
				dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
				dJointSetHinge2Param(Joints[i], dParamFMax2, car_neutral_drive_resistance);
			}
			return;
	}
	else {
		for(i = 0; i < 2; ++i){

			dJointSetHinge2Param(Joints[i], dParamFMax2,car_breaks_resistance);
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		}
		/////////////
		switch(DriveDirection)
		{
		case 1:
			for(i = 2; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
			break;

		case -1:
			for(i = 2; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
			break;
		case 0:
			for(i = 2; i < 4; ++i){
				dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
				dJointSetHinge2Param(Joints[i], dParamFMax2, 100);
			}
			return;
		}
		/////////////
		for(i=2;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
		return;
	}

	for(i=0;i<4;i++)
		dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
}
/////////////////////////////////////////

void CPHJeep::NeutralDrive(){
	if(!bActive) return;
	//////////////////
	for(u32 i = 0; i < 4; ++i){
		dJointSetHinge2Param(Joints[i], dParamFMax2, 10.f);
		dJointSetHinge2Param(Joints[i], dParamVel2, 0);
	}
}
//////////////////////////////////////////////////////////
void CPHJeep::Revert(){
	if(!bActive) return;
	dBodyAddForce(Bodies[0], 0, 2*9000, 0);
	dBodyAddRelTorque(Bodies[0], 300, 0, 0);
}


void CPHJeep::SetPhRefObject(CPhysicsRefObject * ref_object)
{
	m_ref_object=ref_object;
	if(bActive)
	{
		dGeomUserDataSetPhysicsRefObject(Geoms[0],m_ref_object);
		dGeomUserDataSetPhysicsRefObject(Geoms[6],m_ref_object);
	}
}

float CPHJeep::GetSteerAngle()
{
	if(!bActive) return 0;
	return dJointGetHinge2Angle1 (Joints[2]);
}
