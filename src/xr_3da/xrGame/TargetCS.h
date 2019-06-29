#pragma once
#include "customtarget.h"
#include "physic_item.h"

class CTargetCS : 
	public CCustomTarget,
	public CPhysicItem
{
	typedef	CPhysicItem	inherited;
public:
							CTargetCS			();
	virtual					~CTargetCS			();

	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	();

	virtual void			Load				(LPCSTR section);
	
	virtual void			reload				(LPCSTR section);
	virtual void			reinit				();
	virtual void			OnH_B_Chield		();
	
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual void			shedule_Update		(u32 dt);
	
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, 
		s16 element,Fvector p_in_object_space, 
		float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			UpdateCL			();					// Called each frame, so no need for dt
	virtual BOOL			renderable_ShadowGenerate		()		{ return FALSE;	}
	virtual BOOL			renderable_ShadowReceive		()		{ return TRUE;	}
	//virtual	void			Hit				(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse){
	//											m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,P/5.f);
	//											}
	void SetPos									(const Fmatrix& pos);

	//CPhysicsShell*			m_pPhysicsShell;
	Fmatrix					m_pos;

	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();
	virtual CPhysicsShellHolder*		cast_physics_shell_holder	()	{return this;}
	virtual CParticlesPlayer*			cast_particles_player		()	{return this;}
};
