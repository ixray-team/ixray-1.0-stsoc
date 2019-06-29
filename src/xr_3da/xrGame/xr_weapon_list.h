#ifndef __XR_WEAPON_LIST_H__
#define __XR_WEAPON_LIST_H__

// refs
class ENGINE_API CInifile;
class CEntity;

#include "Weapon.h"
#include "WeaponHUD.h"

class				CWeaponList
{
	friend class			CWeapon;
	enum					{ SLOTs = 8 };
private:
	CEntity*				m_pParent;

	svector<CWeapon*,SLOTs>	m_Weapons;
	int						m_iActiveWeapon;		// Current active weapon
	int						m_iSelectedWeapon;		// Candidate for change to
	BOOL					m_bZoomed;
	
	BOOL 					WeaponChange			(int idx);

public:
	int						m_iHUDboneL, m_iHUDboneR;
	int						m_iACTboneL, m_iACTboneR;
public:
							CWeaponList				(CEntity* p);
#pragma todo("Dima to Yura : check if you need here a non-virtual destructor!")
							~CWeaponList			();

	void					Init					(LPCSTR act_bone_r, LPCSTR act_bone_l);
							
	BOOL					ActivateWeaponPrev		(BOOL ignore_empty_weapon=true);
	BOOL					ActivateWeaponNext		(BOOL ignore_empty_weapon=true);
	BOOL					ActivateWeaponID		(int id);
	BOOL					ActivateWeaponHistory	();

	void					weapon_add				(CWeapon* W);		// add
	void					weapon_remove			(CWeapon* W);		// remove
	void					weapon_die				();
							
	void					Zoom					(BOOL bZoom);
	void					Reload					();
	BOOL					isZoomed				()	{ return m_bZoomed; }
	float					getZoomFactor			()	{ CWeapon* W = ActiveWeapon(); if (W/*&&isZoomed()*/) return W->GetZoomFactor();  else return DEFAULT_FOV; }
	int						getAmmoElapsed			()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoElapsed(); else return 0; }
	int						getAmmoCurrent			()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoCurrent(); else return 0; }
	int						getAmmoLimit			()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoLimit();	  else return 0; }
	int						getAmmoMagazineSize		()	{ CWeapon* W = ActiveWeapon(); if (W) return W->GetAmmoMagSize(); else return 0; }
	CWeapon*				getWeaponByWeapon		(CWeapon* W);
	CWeapon*				getWeaponByIndex		(int iIndex)	{ return ((iIndex > -1) && (iIndex < (int)(m_Weapons.size()))) ? m_Weapons[iIndex] : 0; }
	u32						getWeaponCount			()				{ return (m_Weapons.size());	}
	BOOL					isSlotEmpty				(int iSlot)		{ return 0==m_Weapons[iSlot];	}
							
	void					FireStart				();
	void					FireEnd					();
	void					Fire2Start				();
	void					Fire2End					();
	void					GetFireParams			(Fvector &fire_pos, Fvector &fire_dir);
	BOOL					isWorking				();

	void					Update					(float dt, BOOL bHUDView);
							
	//
	IC	CWeapon*			ActiveWeapon			()	{ return (m_iActiveWeapon==-1)?0:m_Weapons[m_iActiveWeapon]; }
	IC	int					ActiveWeaponID			()	{ return m_iActiveWeapon;	}
	IC	int					SelectedWeaponID		()	{ return m_iSelectedWeapon; }
	IC	int					WeaponCount				()	{ return m_Weapons.size();	}
};

#endif //__XR_OBJECT_LIST_H__
