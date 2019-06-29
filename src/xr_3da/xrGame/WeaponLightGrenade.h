// WeaponLightGrenade.h:	объект для эмулирования подствольной гранаты
//							которая свется (типа ракетницы) 	
//////////////////////////////////////////////////////////////////////

#pragma once

#include "weaponfakegrenade.h"


class CWeaponLightGrenade :	public CWeaponFakeGrenade
{
private:
	typedef CWeaponFakeGrenade inherited;
	friend CWeaponMagazinedWGrenade;
public:
	

	CWeaponLightGrenade(void);
	virtual ~CWeaponLightGrenade(void);


	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	//virtual void OnH_B_Independent();
	//virtual void UpdateCL();

	//virtual void Explode(const Fvector &pos, const Fvector &normal);
	//virtual void Destroy();

	
protected:
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;
};
