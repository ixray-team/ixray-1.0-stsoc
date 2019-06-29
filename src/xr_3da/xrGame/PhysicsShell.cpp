#include "stdafx.h"
#pragma hdrstop
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJoint.h"
#include "PHJointDestroyInfo.h"
#include "PHSplitedShell.h"
#include "gameobject.h"
#include "physicsshellholder.h"
#include "../skeletoncustom.h"

extern CPHWorld			*ph_world;
CPhysicsShell::~CPhysicsShell()
{
	if(ph_world)ph_world->NetRelcase(this);
}

CPhysicsElement*			P_create_Element		()
{
	CPHElement* element=xr_new<CPHElement>	();
	return element;
}

CPhysicsShell*				P_create_Shell			()
{
	CPhysicsShell* shell=xr_new<CPHShell>	();
	return shell;
}

CPhysicsShell*				P_create_splited_Shell	()
{
	CPhysicsShell* shell=xr_new<CPHSplitedShell>	();
	return shell;
}

CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{
	CPhysicsJoint* joint=xr_new<CPHJoint>	( type , first, second);
	return joint;
}


CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* bone_map)
{
	CKinematics* pKinematics=smart_cast<CKinematics*>(obj->Visual());

	CPhysicsShell* pPhysicsShell		= P_create_Shell();
#ifdef DEBUG
	pPhysicsShell->dbg_obj=smart_cast<CPhysicsShellHolder*>(obj);
#endif
	pPhysicsShell->build_FromKinematics(pKinematics,bone_map);

	pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
	pPhysicsShell->mXFORM.set(obj->XFORM());
	pPhysicsShell->Activate(not_active_state);//,
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	pPhysicsShell->SetAirResistance();//0.0014f,1.5f

	return pPhysicsShell;
}


CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,BONE_P_MAP* p_bone_map,LPCSTR	fixed_bones)
{
	CPhysicsShell* pPhysicsShell;
	CKinematics* pKinematics=smart_cast<CKinematics*>(obj->Visual());
	if(fixed_bones)
	{


		int count =					_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i) 
		{
			string64					fixed_bone							;
			_GetItem					(fixed_bones,i,fixed_bone)			;
			u16 fixed_bone_id=pKinematics->LL_BoneID(fixed_bone)			;
			R_ASSERT2(BI_NONE!=fixed_bone_id,"wrong fixed bone")			;
			p_bone_map->insert(mk_pair(fixed_bone_id,physicsBone()))			;
		}

		pPhysicsShell=P_build_Shell(obj,not_active_state,p_bone_map);

		//m_pPhysicsShell->add_Joint(P_create_Joint(CPhysicsJoint::enumType::full_control,0,fixed_element));
	}
	else
		pPhysicsShell=P_build_Shell(obj,not_active_state);


	BONE_P_PAIR_IT i=p_bone_map->begin(),e=p_bone_map->end();
	if(i!=e) pPhysicsShell->SetPrefereExactIntegration();
	for(;i!=e;i++)
	{
		CPhysicsElement* fixed_element=i->second.element;
		R_ASSERT2(fixed_element,"fixed bone has no physics");
		//if(!fixed_element) continue;
		fixed_element->Fix();
	}
	return pPhysicsShell;
}

CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,LPCSTR	fixed_bones)
{
	U16Vec f_bones;
	if(fixed_bones){
		CKinematics* K		= smart_cast<CKinematics*>(obj->Visual());
		int count =			_GetItemCount(fixed_bones);
		for (int i=0 ;i<count; ++i){
			string64		fixed_bone;
			_GetItem		(fixed_bones,i,fixed_bone);
			f_bones.push_back(K->LL_BoneID(fixed_bone));
			R_ASSERT2(BI_NONE!=f_bones.back(),"wrong fixed bone")			;
		}
	}
	return P_build_Shell	(obj,not_active_state,f_bones);
}

static BONE_P_MAP bone_map=BONE_P_MAP();
CPhysicsShell*				P_build_Shell			(CGameObject* obj,bool not_active_state,U16Vec& fixed_bones)
{
	bone_map.clear			();
	CPhysicsShell*			pPhysicsShell;
	if(!fixed_bones.empty())
		for (U16It it=fixed_bones.begin(); it!=fixed_bones.end(); it++)
			bone_map.insert(mk_pair(*it,physicsBone()));
	pPhysicsShell=P_build_Shell(obj,not_active_state,&bone_map);

	// fix bones
	BONE_P_PAIR_IT i=bone_map.begin(),e=bone_map.end();
	if(i!=e) pPhysicsShell->SetPrefereExactIntegration();
	for(;i!=e;i++){
		CPhysicsElement* fixed_element=i->second.element;
		//R_ASSERT2(fixed_element,"fixed bone has no physics");
		if(!fixed_element) continue;
		fixed_element->Fix();
	}
	return pPhysicsShell;
}

CPhysicsShell*	P_build_SimpleShell(CGameObject* obj,float mass,bool not_active_state)
{
	CPhysicsShell* pPhysicsShell		= P_create_Shell();
#ifdef DEBUG
	pPhysicsShell->dbg_obj=smart_cast<CPhysicsShellHolder*>(obj);
#endif
	Fobb obb; obj->Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
	CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
	pPhysicsShell->add_Element(E);
	pPhysicsShell->setMass(mass);
	pPhysicsShell->set_PhysicsRefObject(smart_cast<CPhysicsShellHolder*>(obj));
	if(!obj->H_Parent())
		pPhysicsShell->Activate(obj->XFORM(),0,obj->XFORM(),not_active_state);
	return pPhysicsShell;
}

void ApplySpawnIniToPhysicShell(CInifile* ini,CPhysicsShell* physics_shell,bool fixed)
{

	{

		if(ini&&ini->section_exist("collide"))
		{
			if(ini->line_exist("collide","ignore_static")&&fixed)
			{
				physics_shell->SetIgnoreStatic();
			}
			if(ini->line_exist("collide","small_object"))
			{
				physics_shell->SetSmall();
			}
			if(ini->line_exist("collide","ignore_small_objects"))
			{
				physics_shell->SetIgnoreSmall();
			}
		}
	}
}

void	get_box(CPhysicsShell*	shell,const	Fmatrix& form,	Fvector&	sz,Fvector&	c)
{
	c.set(0,0,0);
	for(int i=0;3>i;++i)
	{	
		float lo,hi;
		const	Fvector &ax=cast_fv(((const	float*)&form+i*4));
		shell->get_Extensions(ax,0,lo,hi);
		sz[i]=hi-lo;c.add(Fvector().mul(ax,(lo+hi)/2));
	}
}