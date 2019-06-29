#include "stdafx.h"
#pragma hdrstop

#include "actor.h"
#include "../skeletoncustom.h"
#include "Car.h"
void CActor::create_Skeleton(){
	//if(m_pPhysicsShell) return;
	//
	//if(smart_cast<CCar*>(m_holder)) return;
	//m_PhysicMovementControl->GetDeathPosition	(Position());
	//m_PhysicMovementControl->DestroyCharacter();
	////Position().y+=.1f;
	////#else
	////Position().y+=0.1f;

	//if (!Visual())
	//	return;
	//m_pPhysicsShell		= P_create_Shell();
	//m_pPhysicsShell->build_FromKinematics(smart_cast<CKinematics*>(Visual()));
	//m_pPhysicsShell->mXFORM.set(XFORM());
	//m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
	//	0.3f*skel_airr_ang_factor);
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	//m_pPhysicsShell->set_JointResistance(0.f);
	//m_pPhysicsShell->set_PhysicsRefObject(this);
	////CInifile* ini = smart_cast<CKinematics*>(Visual())->LL_UserData();
	////R_ASSERT2(ini,"NO INI FILE IN MODEL");

	//
	////SAllDDOParams	disable_pars	= worldDisablingParams.objects_params;
	////float translational_factor=ini->r_float("disable","linear_factor");
	////float rotational_factor	  =ini->r_float("disable","angular_factor");
	////disable_pars.rotational.acceleration*=translational_factor;
	////disable_pars.translational.velocity*=translational_factor;
	////disable_pars.rotational.acceleration *=rotational_factor;
	////disable_pars.rotational.velocity *=rotational_factor;
	////m_pPhysicsShell->set_DisableParams(disable_pars);
	//m_pPhysicsShell->Activate();
	//smart_cast<CKinematics*>(Visual())->CalculateBones();
}
