#include "stdafx.h"
#include "xr_weapon_list.h"
#include "entity.h"

class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC BOOL operator() (CWeapon* W) { return cls==W->SUB_CLS_ID; }
};

//--------------------------------------------------------------------------
CWeaponList::CWeaponList	(CEntity* p)
{
	m_pParent			= p;
	m_iActiveWeapon		= -1;
	m_iSelectedWeapon	= -1;
	m_iHUDboneL			= -1;
	m_iHUDboneR			= -1;
	m_iACTboneL			= -1;
	m_iACTboneR			= -1;
	m_bZoomed			= FALSE;
	for (int i=0; i<8; i++)	m_Weapons.push_back	(0);
}

CWeaponList::~CWeaponList	( )
{
}

#define HUD_BONE_NAME_R "bip01_r_hand"
#define HUD_BONE_NAME_L "bip01_l_finger1"
#define ACT_BONE_NAME_R "bip01_r_hand"
#define ACT_BONE_NAME_L "bip01_l_finger1"

void CWeaponList::Init	(LPCSTR act_bone_r, LPCSTR act_bone_l)
{
	m_iHUDboneL		= -1;
	m_iHUDboneR		= -1;
	CKinematics* V	= PKinematics(m_pParent->Visual());
	m_iACTboneL		= V->LL_BoneID(act_bone_l);
	m_iACTboneR		= V->LL_BoneID(act_bone_r);
}

void CWeaponList::FireStart()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->FireStart();
}

void CWeaponList::FireEnd()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->FireEnd();
}

void CWeaponList::Fire2Start()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->Fire2Start();
}

void CWeaponList::Fire2End()
{ 
	if (m_iActiveWeapon==-1) return;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	W->Fire2End();
}

BOOL CWeaponList::isWorking()
{
	if (m_iActiveWeapon==-1) return false;
	CWeapon* W = m_Weapons[m_iActiveWeapon];
	return W->IsWorking();
}

BOOL CWeaponList::WeaponChange		(int idx)
{
	// Analyze desired ID
	if (idx==m_iActiveWeapon)												return false;
	if ((idx<0) || (idx>=(int)(m_Weapons.size())) || (0==m_Weapons[idx]))	return false;

	// Zoom - off
	Zoom				(FALSE);

	// Stop current weapon
	if (m_iActiveWeapon>=0)			m_Weapons[m_iActiveWeapon]->Hide	();

	// Select _new
	m_iSelectedWeapon				= idx;
	return true;
}

void	CWeaponList::weapon_add			(CWeapon* W)		// add, return index
{
	int slot						= W->GetSlot		();
	R_ASSERT						(0==m_Weapons[slot]);
	m_Weapons[slot]					= W;
}
void	CWeaponList::weapon_remove		(CWeapon* W)		// remove, return last
{
	int slot						= W->GetSlot		();
	R_ASSERT						(W==m_Weapons[slot]);
	m_Weapons[slot]					= 0;
	m_iActiveWeapon					= -1;
	m_iSelectedWeapon				= -1;
}

void CWeaponList::weapon_die		()
{
	NET_Packet		P;

	for (int it=0; it<(int)(m_Weapons.size()); it++)
	{
		CWeapon* W	= m_Weapons[it];
		if (0==W)	continue;
		if (it==m_iActiveWeapon)
		{
			m_pParent->u_EventGen		(P,GE_OWNERSHIP_REJECT,m_pParent->ID());
			P.w_u16						(u16(W->ID()));
			m_pParent->u_EventSend		(P);
		} else {
			m_pParent->u_EventGen		(P,GE_DESTROY,W->ID());
			m_pParent->u_EventSend		(P);
		}
	}

	m_iSelectedWeapon	= -1;
	m_iActiveWeapon		= -1;
}

BOOL CWeaponList::ActivateWeaponNext(BOOL ignore)
{
	if (m_Weapons.size()>1){
		int wpn_count=m_Weapons.size();
		for (int Q=0, cur_wpn = m_iActiveWeapon+1;cur_wpn<=wpn_count; cur_wpn++, Q++){
			if (cur_wpn>=wpn_count)cur_wpn=0;
			if (ignore||m_Weapons[cur_wpn]->IsValid()) break;
			if (Q>=wpn_count) return false;
		}
		return WeaponChange(cur_wpn);	
	}
	return false;
}

BOOL CWeaponList::ActivateWeaponID(int id)
{
	return WeaponChange(id);
}

BOOL CWeaponList::ActivateWeaponHistory()
{
	return ActivateWeaponNext(TRUE);
}

void CWeaponList::Zoom(BOOL bZoom)
{
	CWeapon*	W = ActiveWeapon();
	if (W && W->HasOpticalAim()){
		if (bZoom)	{
			m_bZoomed	= TRUE;
			W->OnZoomIn	();
		} else {
			m_bZoomed	= FALSE;
			W->OnZoomOut();
		}
	}
}
void CWeaponList::Reload()
{
	CWeapon*	W = ActiveWeapon();
	if (W)		W->Reload		();
}


CWeapon*	CWeaponList::getWeaponByWeapon(CWeapon* W)
{
	VERIFY			(W);
	int slot		= W->GetSlot();
	return m_Weapons[slot];

	/*
	if (0==m_Weapons[slot])					return 0;
	else if 

	for (u32 it=0; it<m_Weapons.size(); it++)
	{
		CWeapon* T											= m_Weapons[it];
		if (typeid(*W).raw_name() == typeid(*T).raw_name())	return	T;
	}
	return 0;
	*/
}

void CWeaponList::Update	(float dt, BOOL bHUDView)
{
	// Change weapon if needed and can be done
	if (m_iSelectedWeapon>=0)	
	{
		if ((m_iActiveWeapon<0) || (!m_Weapons[m_iActiveWeapon]->IsVisible()))
		{
			m_iActiveWeapon		= m_iSelectedWeapon;
			m_Weapons[m_iActiveWeapon]->Show	();
		}
	}

	// Setup HUD mode
	if (m_iActiveWeapon>=0)		m_Weapons[m_iActiveWeapon]->SetHUDmode(bHUDView);
}

void CWeaponList::GetFireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	if (m_iActiveWeapon==-1) return;
	CWeapon* W		= m_Weapons[m_iActiveWeapon];
	fire_pos.set	(W->vLastFP);
	fire_dir.set	(W->vLastFD);
}
