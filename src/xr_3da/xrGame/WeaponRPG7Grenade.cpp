// WeaponRPG7Grenade.h: объект для эмулирования гранаты РПГ7
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponRPG7Grenade.h"
#include "WeaponRPG7.h"


CWeaponRPG7Grenade::CWeaponRPG7Grenade() 
{
}

CWeaponRPG7Grenade::~CWeaponRPG7Grenade() 
{
}




void CWeaponRPG7Grenade::Load(LPCSTR section) 
{
	inherited::Load(section);
	CExplosive::Load(section);
}

BOOL CWeaponRPG7Grenade::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	result |= CExplosive::net_Spawn(DC);

	return result;
}

void CWeaponRPG7Grenade::Contact(const Fvector &pos, const Fvector &normal)
{
	if(eCollide == m_eState) return;
	


	CExplosive::ExplodeParams(pos,normal);

	//взорвать гранату
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	}

	inherited::Contact(pos, normal);
}

void CWeaponRPG7Grenade::net_Destroy() 
{
	CExplosive::net_Destroy();
	inherited::net_Destroy();
}

void CWeaponRPG7Grenade::OnH_A_Independent() 
{
	inherited::OnH_A_Independent();
	CExplosive::OnH_A_Independent();
}

void CWeaponRPG7Grenade::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	CExplosive::OnH_B_Independent();
}

void CWeaponRPG7Grenade::UpdateCL() 
{
	if(eCollide == m_eState)
		CExplosive::UpdateCL();
	else
		inherited::UpdateCL();
}


void  CWeaponRPG7Grenade::OnEvent (NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	CExplosive::OnEvent(P, type);
}


void CWeaponRPG7Grenade::make_Interpolation ()
{
	inherited::make_Interpolation();
}

void CWeaponRPG7Grenade::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
}

void CWeaponRPG7Grenade::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
}

void CWeaponRPG7Grenade::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
}

void CWeaponRPG7Grenade::reinit				()
{
	inherited::reinit			();
	CExplosive::reinit			();
}

void CWeaponRPG7Grenade::reload					(LPCSTR section)
{
	inherited::reload			(section);
	CExplosive::reload			(section);
}

void CWeaponRPG7Grenade::activate_physic_shell	()
{
	inherited::activate_physic_shell();
}

void CWeaponRPG7Grenade::setup_physic_shell		()
{
	inherited::setup_physic_shell();
}

void CWeaponRPG7Grenade::create_physic_shell		()
{
	inherited::create_physic_shell();
}
