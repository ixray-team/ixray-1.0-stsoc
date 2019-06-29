#pragma once

#include "../../CustomMonster.h"
#include "../../state_internal.h"
#include "ai_chimera_movement.h"
#include "../telekinesis.h"

class CCharacterPhysicsSupport;

class CAI_Chimera : 	public CCustomMonster,
						public CChimeraMovementManager,
						public CStateInternal<CAI_Chimera> {

public:
	typedef CCustomMonster				inherited;

	CCharacterPhysicsSupport	*m_pPhysics_support;
	float						m_fGoingSpeed;

	CMotionDef					*cur_anim;
public:
							CAI_Chimera					();
	virtual					~CAI_Chimera				();	

	virtual	void			Init						();
	virtual	void			Load						(LPCSTR section);
	virtual	void			reinit						();
	virtual void			reload						(LPCSTR	section );	
	virtual BOOL			net_Spawn					(LPVOID DC);
	virtual	void			net_Destroy					();
	virtual	void			net_Import					(NET_Packet& P);
	virtual	void			net_Export					(NET_Packet& P);

	virtual void			UpdateCL					();
	virtual void			shedule_Update				(u32 dt);
	virtual void			Think						();
	virtual void			Die							();

	virtual void			SelectAnimation				(const Fvector& _view, const Fvector& _move, float speed );
	virtual BOOL			feel_vision_isRelevant		(CObject *O);
	virtual	Feel::Sound*	dcast_FeelSound				()			{ return this;	}
	
	virtual void			HitSignal					(float amount, Fvector& vLocalDir, CObject* who, s16 element) {}
	virtual void			Hit							(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound) {}

	virtual void			OnHUDDraw					(CCustomHUD* hud)			{return inherited::OnHUDDraw(hud);}
	virtual void			OnEvent						(NET_Packet& P, u16 type)	{return inherited::OnEvent(P,type);}
	virtual u16				PHGetSyncItemsNumber		()							{return inherited::PHGetSyncItemsNumber();}
	virtual CPHSynchronize*	PHGetSyncItem				(u16 item)					{return inherited::PHGetSyncItem(item);}
	virtual void			PHUnFreeze					()							{return inherited::PHUnFreeze();}
	virtual void			PHFreeze					()							{return inherited::PHFreeze();}
	virtual BOOL			UsedAI_Locations			()							{return inherited::UsedAI_Locations();}
	virtual const SRotation	Orientation					() const					{return inherited::Orientation();}
	virtual void			renderable_Render			()							{return inherited::renderable_Render();} 
	
	// CPHObject redifinition
//	virtual void 			InitContact					(dContact* c,bool& do_collide)	{}
//	virtual void 			Freeze						()								{}
//	virtual void 			UnFreeze					()								{}

#ifdef DEBUG
	virtual void			OnRender					() {}
#endif
};

