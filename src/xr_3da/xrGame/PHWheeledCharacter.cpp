#include "stdafx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "../cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "phwheeledcharacter.h"


void CPHWheeledCharacter::Create(dVector3 /**sizes/**/){

	//chRCylinder=dCreateBox(0,0.8f,2.f,0.8f);
	//m_radius=_min(sizes[0],sizes[2])/2.f;
	//m_cap=dCreateSphere(0,0.5f);
	//m_wheel=dCreateSphere(0,0.5f);
	//m_geom_shell=dCreateCylinder(0,0.4f,2.f);


	//dGeomTransformSetInfo(chTransform1,1);
	//dGeomTransformSetInfo(chTransform2,1);
	/*
	dMatrix3 RZ;
	dRFromAxisAndAngle (RZ,0.f, 0.f, 1.f, M_PI/1800.f);
	//dGeomSetRotation(m_cap,RZ);
	dGeomSetRotation(m_geom_shell,RZ);
	dGeomSetPosition(m_cap,0.f,1.f,0.f);
	m_cap_transform=dCreateGeomTransform(0);
	m_shell_transform=dCreateGeomTransform(0);
	dGeomTransformSetInfo(m_cap_transform,1);
	dGeomTransformSetInfo(m_shell_transform,1);

	//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);
	//dGeomSetPosition(chRCylinder,0.f,0.f,0.f);
	dGeomTransformSetGeom(m_cap_transform,m_cap);
	dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
	m_body=dBodyCreate(phWorld);
	m_wheel_body=dBodyCreate(phWorld);




	dGeomSetBody(m_cap_transform,m_body);
	dGeomSetBody(m_shell_transform,m_body);
	//dGeomSetBody(chRCylinder,chRBody);
	dGeomSetBody(m_wheel,m_wheel_body);
	dBodySetPosition(m_body,-9,4,0);
	dBodySetPosition(m_wheel_body,-9,2.5,0);
	dMass m;

	dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
	dMassAdjust(&m,70.f);
	dBodySetMass(m_body,&m);

	dMassSetSphere(&m,1,0.5);
	dMassAdjust(&m,1.f);
	dBodySetMass(m_wheel_body,&m);

	m_wheel_joint=dJointCreateHinge2(phWorld,0);
	dJointAttach(m_wheel_joint,m_body,m_wheel_body);

	dJointSetHinge2Anchor (m_wheel_joint,-9 , 2.5, 0);
	dJointSetHinge2Axis1 (m_wheel_joint, 0, 1, 0);
	dJointSetHinge2Axis2 (m_wheel_joint, 0, 0, 1);


	//dJointSetHinge2Param(chRJoint, dParamLoStop, 0);
	//dJointSetHinge2Param(chRJoint, dParamHiStop, 0);
	dJointSetHinge2Param(m_wheel_joint, dParamFMax, 5000);

	dJointSetHinge2Param(m_wheel_joint, dParamVel2, 0);
	dJointSetHinge2Param(m_wheel_joint, dParamFMax2, 1000);

	dReal k_p=4000.f;
	dReal k_d=500.f;
	dReal h=0.025f;



	//	dWorldSetCFM(world,  1.f / (h*k_p + k_d));
	//	dWorldSetERP(world,  h*k_p / (h*k_p + k_d));

	dJointSetHinge2Param(m_wheel_joint, dParamSuspensionERP, h*k_p / (h*k_p + k_d));
	dJointSetHinge2Param(m_wheel_joint, dParamSuspensionCFM, 1.f / (h*k_p + k_d));


	m_geom_group=dCreateGeomGroup(ph_world->GetSpace());


	dGeomGroupAdd(m_geom_group,m_wheel);
	dGeomGroupAdd(m_geom_group,m_cap_transform);
	dGeomGroupAdd(m_geom_group,m_shell_transform);
	//dGeomGroupAdd(chRGeomGroup,chRCylinder);


	*/
}

void CPHWheeledCharacter::Destroy(){
	/*
	if(m_body) {
	dBodyDestroy(m_body);
	m_body=NULL;
	}
	*/

	if(m_wheel_joint){
		dJointDestroy(m_wheel_joint);
		m_wheel_joint=NULL;
	}

	if(m_wheel_body){
		dBodyDestroy(m_wheel_body);
		m_wheel_body=NULL;
	}

	/*
	if(m_geom_shell){
	dGeomDestroyUserData(m_geom_shell);
	dGeomDestroy(m_geom_shell);
	m_geom_shell=NULL;
	}

	if(m_wheel) {
	dGeomDestroyUserData(m_wheel);
	dGeomDestroy(m_wheel);
	m_wheel=NULL;
	}


	if(m_cap) {
	dGeomDestroyUserData(m_cap);
	dGeomDestroy(m_cap);
	m_cap=NULL;
	}

	if(m_wheel_transform){
	dGeomDestroyUserData(m_wheel_transform);
	dGeomDestroy(m_wheel_transform);
	m_wheel_transform=NULL;
	}
	if(m_shell_transform){
	dGeomDestroyUserData(m_shell_transform);
	dGeomDestroy(m_shell_transform);
	m_shell_transform=NULL;
	}
	if(m_cap_transform){
	dGeomDestroyUserData(m_cap_transform);
	dGeomDestroy(m_cap_transform);
	m_cap_transform=NULL;
	}
	if(m_geom_group){
	dGeomDestroy(m_geom_group);
	m_geom_group=NULL;
	}

	*/
}

void CPHWheeledCharacter::SetMas(dReal /**mass/**/){
	/*
	dMass m;
	dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
	dMassAdjust(&m,mass);
	dBodySetMass(m_body,&m);

	}
	EEnvironment	 CPHWheeledCharacter::CheckInvironment(){

	return peOnGround;
	*/
}
CPHWheeledCharacter::CPHWheeledCharacter()
{
	m_wheel_body=NULL;
	m_wheel_joint=NULL;
}


