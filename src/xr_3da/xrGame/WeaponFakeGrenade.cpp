// WeaponFakeGrenade.cpp: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WeaponFakeGrenade.h"

#include "WeaponMagazinedWGrenade.h"

#include "Entity.h"
#include "PSObject.h"
#include "ParticlesObject.h"

#include "PhysicsShell.h"
#include "extendedgeom.h"



CWeaponFakeGrenade::CWeaponFakeGrenade() 
{
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;

	m_explodeTime = m_dwEngineTime = m_flashTime = 0;
	m_fJumpHeight = 0;
}

CWeaponFakeGrenade::~CWeaponFakeGrenade	()
{
}

void __stdcall CWeaponFakeGrenade::ObjectContactCallback(bool& do_colide,dContact& c) 
{
	do_colide = false;


	dxGeomUserData *l_pUD1 = NULL;
	dxGeomUserData *l_pUD2 = NULL;
	l_pUD1 = retrieveGeomUserData(c.geom.g1);
	l_pUD2 = retrieveGeomUserData(c.geom.g2);

	CWeaponFakeGrenade *l_this = l_pUD1 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD1->ph_ref_object) : NULL;
	Fvector vUp;
	if(!l_this){
		l_this = l_pUD2 ? dynamic_cast<CWeaponFakeGrenade*>(l_pUD2->ph_ref_object) : NULL;
		vUp.invert(*(Fvector*)&c.geom.normal);
	}else{
		vUp.set(*(Fvector*)&c.geom.normal);	
	}
	if(!l_this) return;

	CGameObject *l_pOwner = l_pUD1 ? dynamic_cast<CGameObject*>(l_pUD1->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner == (CGameObject*)l_this) l_pOwner = l_pUD2 ? dynamic_cast<CGameObject*>(l_pUD2->ph_ref_object) : NULL;
	if(!l_pOwner || l_pOwner != l_this->m_pOwner) 
	{
		if(l_this->m_pOwner) 
		{
			Fvector l_pos; l_pos.set(l_this->Position());
			if(!l_pUD1||!l_pUD2) 
			{
				dxGeomUserData *&l_pUD = l_pUD1?l_pUD1:l_pUD2;
				if(l_pUD->pushing_neg) 
				{
					Fvector velocity;
					l_this->PHGetLinearVell(velocity);
					velocity.normalize();
					float cosinus=velocity.dotproduct(*((Fvector*)l_pUD->neg_tri.norm));
					float dist=l_pUD->neg_tri.dist/cosinus;
					velocity.mul(dist);
					l_pos.sub(velocity);

				}
			}
			l_pos.mad(vUp, l_this->m_fJumpHeight);
			l_this->Explode(l_pos, vUp);
		}
	} else {}
}

void CWeaponFakeGrenade::Load(LPCSTR section) 
{
	CExplosive::Load	(section);

	m_mass = pSettings->r_float(section,"ph_mass");
	m_fJumpHeight = pSettings->r_float(section, "jump_height");

	m_state = stInactive;

	m_slot = NO_ACTIVE_SLOT;
}

static const u32 EXPLODE_TIME	= 5000;
static const u32 FLASH_TIME		= 300;
static const u32 ENGINE_TIME	= 3000;

void CWeaponFakeGrenade::Explode(const Fvector &pos, const Fvector &/**normal/**/) 
{
	if(m_state != stEngine && m_state != stFlying) return;

	m_state			= stExplode;
	m_explodeTime	= EXPLODE_TIME;
	m_flashTime		= FLASH_TIME;

	Position().set(pos);
	CExplosive::ExplodeParams(Position(), XFORM().k);	


	//Generate Expode event
	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};
	
	m_pOwner = NULL;
}

BOOL CWeaponFakeGrenade::net_Spawn(LPVOID DC) 
{
	BOOL result = CExplosive::net_Spawn(DC);
	
	m_state = stInactive;
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
	m_pOwner = NULL;
	m_explodeTime = m_dwEngineTime = m_flashTime = 0;
	return result;
}


void CWeaponFakeGrenade::net_Destroy() 
{
	CExplosive::net_Destroy();
}

void CWeaponFakeGrenade::Destroy() 
{
	//Generate Expode event
	CExplosive::ExplodeParams(Position(), XFORM().k);

	if (Local()) 
	{
		NET_Packet		P;
		u_EventGen		(P,GE_GRENADE_EXPLODE,ID());	
		u_EventSend		(P);
	};
}

void CWeaponFakeGrenade::OnH_B_Independent() 
{
	CExplosive::OnH_B_Independent();
}

void CWeaponFakeGrenade::UpdateCL()
{
	
	switch (m_state)
	{
	case stInactive:
		CInventoryItem::UpdateCL();
		break;
	case stExplode: 
		CExplosive::UpdateCL();
		break;
	case stEngine:
		CInventoryItem::UpdateCL();
		if(getVisible() && m_pPhysicsShell) 
		{
			if (m_dwEngineTime <= 0) {
				m_state		= stFlying;
			}
			else
			{
				// двигатель все еще работает
				m_dwEngineTime -= Device.dwTimeDelta;
				Fvector l_pos, l_dir;
				l_pos.set(0, 0, 3.f); 
				l_dir.set(XFORM().k); 
				l_dir.normalize();

				float l_force = m_engine_f * Device.dwTimeDelta / 1000.f;

				l_dir.set(0, 1.f, 0);
				l_force = m_engine_u * Device.dwTimeDelta / 1000.f;
			}
		}
		break;
	case stFlying:
		CInventoryItem::UpdateCL();
		break;
	}
}

void CWeaponFakeGrenade::net_Import			(NET_Packet& P)
{
}

void CWeaponFakeGrenade::net_Export			(NET_Packet& P)
{
}
void CWeaponFakeGrenade::make_Interpolation ()
{
}

void CWeaponFakeGrenade::PH_B_CrPr			()
{
}

void CWeaponFakeGrenade::PH_I_CrPr			()
{
}

void CWeaponFakeGrenade::PH_A_CrPr			()
{
}

void CWeaponFakeGrenade::activate_physic_shell	()
{
	CObject		*E = dynamic_cast<CObject*>(H_Parent());
	R_ASSERT	(E);
	
	Fmatrix trans;
	//!!!~! ну и какого такую фигню было писать????
	Level().Cameras.unaffected_Matrix(trans);

	CWeaponMagazinedWGrenade *l_pW = dynamic_cast<CWeaponMagazinedWGrenade*>(E);
	Fmatrix l_p1, l_r; 
	l_r.rotateY(M_PI*2.f); 
	l_p1.mul(l_pW->GetHUDmode()?trans:l_pW->XFORM(), l_r); 
	l_p1.c.set(*l_pW->m_pGrenadePoint);

	Fvector a_vel; 
	a_vel.set(0, 0, 0);
	m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
	m_pPhysicsShell->Update	();
	XFORM().set(m_pPhysicsShell->mXFORM);
	Position().set(m_pPhysicsShell->mXFORM.c);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->set_ObjectContactCallback(ObjectContactCallback);
	m_pPhysicsShell->set_ContactCallback(NULL);

	m_state			= stEngine;
	m_dwEngineTime	= ENGINE_TIME;
}

void CWeaponFakeGrenade::create_physic_shell	()
{
	create_box2sphere_physic_shell();
}