#include "stdafx.h"
#include "IK/IKLimb.h"
#include "IKLimbsController.h"
#include "../SkeletonCustom.h"
CIKLimbsController::CIKLimbsController()
{
	
}

void CIKLimbsController::Create(CKinematics* K,CGameObject* O)
{
	u16 lbones[4]={K->LL_BoneID("bip01_l_thigh"),K->LL_BoneID("bip01_l_calf"),K->LL_BoneID("bip01_l_foot"),K->LL_BoneID("bip01_l_toe0")};
	m_left_leg.Create(K,lbones,Fvector().set(0.13143f,0,0.15f),CIKLimb::tpLeft);
	u16 rbones[4]={K->LL_BoneID("bip01_r_thigh"),K->LL_BoneID("bip01_r_calf"),K->LL_BoneID("bip01_r_foot"),K->LL_BoneID("bip01_r_toe0")};
	m_right_leg.Create(K,rbones,Fvector().set(0.13143f,0,0.15f),CIKLimb::tpRight);
}

void CIKLimbsController::Calculate(CKinematics *K,const Fmatrix &obj)
{
	m_left_leg.Calculate(K,obj);
	m_right_leg.Calculate(K,obj);
}

void CIKLimbsController::Destroy()
{
	m_left_leg.Destroy();
	m_right_leg.Destroy();
}