#pragma once
#include "WeaponThrust.h"

class CWeaponKnife :
	public CWeaponThrust
{
	typedef CWeaponThrust inherited;
public:
	CWeaponKnife(void);
	virtual ~CWeaponKnife(void);

	virtual void OnH_A_Chield();
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void OnH_B_Independent();
	virtual bool Activate();
	virtual void Deactivate();
	virtual bool Attach(PIItem pIItem, bool force = false);
	virtual void Throw();
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful();
	virtual void Destroy();
};
