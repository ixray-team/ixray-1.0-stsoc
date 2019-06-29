////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory.h
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_vision.h"

class CActor;
class CVisualMemoryManager;

class CActorMemory : 
	public ISheduled,
	public Feel::Vision
{
private:
	typedef ISheduled				inherited;

private:
	CActor							*m_actor;
	CVisualMemoryManager			*m_visual;
	u32								m_state;
	u32								m_time_stamp;
	Fvector							m_position;

private:
			void					eye_pp_s01				();
			void					eye_pp_s2				();

public:
									CActorMemory			(CActor *actor);
	virtual							~CActorMemory			();

public:
	virtual float					shedule_Scale			();
	virtual void					shedule_Update			(u32 dt);
	virtual	shared_str				shedule_Name			() const;
	virtual bool					shedule_Needed			()				{return true;};

public:
	virtual	BOOL					feel_vision_isRelevant	(CObject* O);
	virtual	float					feel_vision_mtl_transp	(CObject* O, u32 element);

public:
	virtual	void					Load					(LPCSTR section);
	virtual	void					reinit					();
	virtual	void					reload					(LPCSTR section);
			void					remove_links			(CObject *object);

public:
	IC		CVisualMemoryManager	&visual					() const;
	IC		CActor					&object					() const;
};

#include "actor_memory_inline.h"