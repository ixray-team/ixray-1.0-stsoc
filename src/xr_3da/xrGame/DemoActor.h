#ifndef __DEMO_ACTOR_H__
#define __DEMO_ACTOR_H__

#include "entity.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CSoundStream;
class CDummyObject;
class CObjectAnimator;

class CDemoActor:		public CEntity
{
private:
	typedef CEntity		inherited;
private:
	CObjectAnimator*	animator;
	Fvector				start_position;

	CSoundStream*		music;
	char*				attached;

	CCameraBase*		camera;
	void				UpdateCamera	( );

	float				time2start,time2start_ltx;
public:
						CDemoActor		();
	virtual				~CDemoActor		();

	virtual void		Load			(LPCSTR section);

	void				PlayDemo		(const char* name);
	void				StopDemo		();

	// Update
	virtual void		shedule_Update	( u32 DT );

	// virtual
	virtual void		HitSignal		(int /**HitAmount/**/,Fvector& /**vLocalDir/**/,CEntity* /**E/**/, s16 /**element/**/) {};
	virtual void		g_fireParams	(const CHudItem* pHudItem, Fvector& P, Fvector& D);
};

#endif //__DEMO_ACTOR_H__
