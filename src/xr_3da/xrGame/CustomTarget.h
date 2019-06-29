#pragma once
#include "gameobject.h"

class CCustomTarget : virtual public CGameObject//need m_pPhysicShell?
{
	typedef CGameObject inherited;
public:
	// Generic
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual void			shedule_Update		(u32 dt);
	virtual void			UpdateCL			();
	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	();
	//virtual	void			Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse){};
	CCustomTarget();
	virtual ~CCustomTarget();
};
