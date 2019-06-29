////////////////////////////////////////////////////////////////////////////
//	Module 		: vehicle_helicopter.h
//	Created 	: 13.02.2004
//  Modified 	: 13.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Vehicle helicopter class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_touch.h"
#include "holder_custom.h"
#include "entity.h"

class CObjectAnimator;
class CMotionDef;
class CCameraBase;

class CVehicleHelicopter : 
	public CEntity,
	public Feel::Touch,
	public CHolderCustom
{
	typedef CEntity inherited;
protected:
	ref_sound			m_engine_sound;
	CObjectAnimator		*m_animator;
	shared_str			m_animation_sting;
	CMotionDef			*m_animation;
public:
						CVehicleHelicopter	();
	virtual				~CVehicleHelicopter	();
	virtual CHolderCustom	*cast_holder_custom	()				{return this;}
			void		init				();
	virtual	void		Load				(LPCSTR		section);
	virtual	void		reinit				();
	virtual	void		reload				(LPCSTR		section);
	virtual BOOL		net_Spawn			(CSE_Abstract*		DC);
	virtual void		net_Destroy			();
	virtual void		net_Export			(NET_Packet &P);
	virtual void		net_Import			(NET_Packet &P);
	virtual void		renderable_Render	();
	virtual void		UpdateCL			();
	virtual void		shedule_Update		(u32		time_delta);
	virtual void		feel_touch_new		(CObject	*O);
	virtual void		feel_touch_delete	(CObject	*O);
	virtual BOOL		feel_touch_contact	(CObject	*O);

// control functions
	virtual void		OnMouseMove			(int x, int y);
	virtual void		OnKeyboardPress		(int dik);
	virtual void		OnKeyboardRelease	(int dik);
	virtual void		OnKeyboardHold		(int dik);

	virtual CInventory*	GetInventory		(){return 0;}

	virtual bool		Use					(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos){return false;}
	virtual bool		attach_Actor		(CActor* actor)	{return false;}
	virtual void		detach_Actor		()				{}

	virtual Fvector		ExitPosition		()				{return XFORM().c;}

	virtual CCameraBase*Camera				()				{return 0;}
	virtual void		cam_Update			(float dt);

	virtual void		HitSignal			(float P, Fvector &local_dir,	CObject* who, s16 element){;}
	virtual void		HitImpulse			(float P, Fvector &vWorldDir, 	Fvector& vLocalDir){;}
protected:
	IC		const CObjectAnimator	*animator	() const;
	IC		const shared_str			&animation	() const;
};

#include "vehicle_helicopter_inline.h"