////////////////////////////////////////////////////////////////////////////
//	Module 		: weapon_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Weapon manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon_manager.h"
#include "entity_alive.h"
#include "inventoryowner.h"
#include "weapon.h"
#include "enemy_manager.h"
#include "custommonster.h"
#include "weaponmagazined.h"
#include "ai/stalker/ai_stalker.h"

#define TIME_FIRE_DELTA 500

CWeaponManager::CWeaponManager		()
{
	Init						();
}

CWeaponManager::~CWeaponManager		()
{
}

void CWeaponManager::Init			()
{
}

void CWeaponManager::Load			(LPCSTR section)
{
	m_dwFireRandomMin  				= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  				= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  				= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  				= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 				= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   				= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 				= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");
	m_bCanFire						= true;
	m_bHammerIsClutched				= false;
	m_dwNoFireTime					= 0;
	m_dwStartFireTime				= 0;
}

void CWeaponManager::reinit			()
{
}

bool CWeaponManager::check_if_can_kill_target(const CEntity *tpEntity, const Fvector &target_pos, float yaw2, float pitch2, float fSafetyAngle) const
{
	Fvector fire_pos = tpEntity->Position();
	fire_pos.sub(target_pos,fire_pos);
	float yaw1,pitch1;
	fire_pos.getHP(yaw1,pitch1);
	yaw1 = angle_normalize_signed(yaw1);
	pitch1 = angle_normalize_signed(pitch1);
	yaw2 = angle_normalize_signed(yaw2);
	pitch2 = angle_normalize_signed(pitch2);
	
	const CInventoryOwner *tpInventoryOwner = dynamic_cast<const CInventoryOwner*>(tpEntity);
	if (tpInventoryOwner) {
		if (!tpInventoryOwner->m_inventory.ActiveItem() || !dynamic_cast<CWeapon*>(tpInventoryOwner->m_inventory.ActiveItem()) || !dynamic_cast<CWeapon*>(tpInventoryOwner->m_inventory.ActiveItem())->GetAmmoElapsed())
			return(false);
		if (!((angle_difference(yaw1,yaw2) <= fSafetyAngle) && (angle_difference(pitch1,pitch2) <= fSafetyAngle)))
			return(false);

#pragma todo("Dima to Dima : Recover check if can kill target function")
//		setEnabled			(false);
//		Collide::ray_query	l_tRayQuery;
//		l_tRayQuery.O		=	NULL;
//		Fvector				l_tFireDirection;
//		l_tFireDirection.setHP(yaw1,pitch1);
//		g_pGameLevel->ObjectSpace.RayPick(tpEntity->Position(), l_tFireDirection, fire_pos.distance_to(target_pos) + .5f, l_tRayQuery);
//		setEnabled			(true);
//		return				(l_tRayQuery.O && (l_tRayQuery.O->Position().distance_to(target_pos) < EPS_L));
		return				(true);
	}
	else
		return((tpEntity->Position().distance_to(target_pos) < 2.f) && (angle_difference(yaw1,yaw2) <= PI_DIV_2));
}

bool CWeaponManager::check_if_can_kill_enemy(const SRotation &orientation) const
{
	const CMemoryManager	*memory_manager = dynamic_cast<const CMemoryManager*>(this);
	const CEntity			*entity	= dynamic_cast<const CEntity *>(this);
	VERIFY					(memory_manager);
	VERIFY					(entity);
	return			(
		memory_manager->enemy() && 
		memory_manager->enemy()->g_Alive() && 
		memory_manager->visible(memory_manager->enemy()) && 
		check_if_can_kill_target(
			entity,
			memory_manager->enemy()->Position(),
			-orientation.yaw,
			-orientation.pitch
		)
	);
}

bool CWeaponManager::check_if_can_kill_member() const
{
	bool bCanKillMember = false;
	const CAI_Stalker	*stalker = dynamic_cast<const CAI_Stalker*>(this);
	VERIFY				(stalker);
	
	for (int i=0, iTeam = (int)stalker->g_Team(); i<(int)stalker->visible_objects().size(); ++i) {
		const CEntityAlive	*entity_alive = dynamic_cast<const CEntityAlive*>(stalker->visible_objects()[i]);
		if (entity_alive && (entity_alive->g_Team() == iTeam))
			if (entity_alive->g_Alive() && check_if_can_kill_target(stalker,entity_alive->Position(),-stalker->head_orientation().current.yaw,-stalker->head_orientation().current.pitch)) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CWeaponManager::update(const MonsterSpace::EObjectAction &weapon_state)
{
	CAI_Stalker				*stalker = dynamic_cast<CAI_Stalker*>(this);
	VERIFY					(stalker);

	CWeapon					*tpWeapon = dynamic_cast<CWeapon*>(stalker->m_inventory.ActiveItem());

	if (!stalker->m_inventory.ActiveItem() || !tpWeapon)
		return;

	if (stalker->mental_state() == MonsterSpace::eMentalStatePanic) {
		stalker->m_inventory.Activate(u32(-1));
		return;
	}

	bool bSafeFire = m_bFiring;

	u32						dwStartFireAmmo, dwFireDelayMin = 0, dwFireDelayMax = 0;
	CWeaponMagazined		*tpWeaponMagazined = dynamic_cast<CWeaponMagazined*>(tpWeapon);
	if (tpWeaponMagazined && stalker->enemy()) {
		float fDistance = stalker->Position().distance_to(stalker->enemy()->Position());
		float fDistance1 = tpWeaponMagazined->m_fMaxRadius, fDistance2 = (tpWeaponMagazined->m_fMaxRadius + 0*tpWeaponMagazined->m_fMinRadius)/2,fDistance3 = 0*tpWeaponMagazined->m_fMinRadius;
		u32 dwFuzzyDistance;
		if (_abs(fDistance - fDistance1) <	_abs(fDistance - fDistance2))
			dwFuzzyDistance = 0;
		else
			if (_abs(fDistance - fDistance2) <	_abs(fDistance - fDistance3))
				dwFuzzyDistance = 1;
			else
				dwFuzzyDistance = 2;
		
		u32	dwMinQueueSize = 0, dwMaxQueueSize = 0;
		switch (stalker->movement_type()) {
			case eMovementTypeRun : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 2000;
						dwFireDelayMax	= 4000;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 2;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 2000;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 3;
						dwMaxQueueSize	= 7;
						dwFireDelayMin	= 700;
						dwFireDelayMax	= 1500;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			case eMovementTypeWalk : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 3000;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 2;
						dwMaxQueueSize	= 4;
						dwFireDelayMin	= 900;
						dwFireDelayMax	= 2000;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 5;
						dwMaxQueueSize	= 10;
						dwFireDelayMin	= 700;
						dwFireDelayMax	= 1200;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			case eMovementTypeStand : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1500;
						dwFireDelayMax	= 2500;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 3;
						dwMaxQueueSize	= 5;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 1800;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 10;
						dwMaxQueueSize	= 15;
						dwFireDelayMin	= 500;
						dwFireDelayMax	= 1000;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			default : {
				stalker->m_inventory.Action(kWPN_FIRE, CMD_STOP);
				stalker->m_inventory.ActiveItem()->Deactivate();
				dwMaxQueueSize			= 0;
				break;
			}
		}
		if (dwMaxQueueSize)
			if (tpWeaponMagazined->GetAmmoMagSize() > 1)
				tpWeaponMagazined->SetQueueSize(::Random.randI(iFloor(float(dwMinQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize())),iFloor(float(dwMaxQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize()))));
			else
				tpWeaponMagazined->SetQueueSize(1);
	}

//	Msg	("Time : %d\nDESIRED : %s\nREAL : %s\nFIRING : %d",Level().timeServer(),(weapon_state == eObjectActionIdle) ? "IDLE" : "FIRE",(tpWeapon->STATE == CWeapon::eFire) ? "FIRING" : ((tpWeapon->STATE == CWeapon::eIdle) ? "IDLE" : ((tpWeapon->STATE == CWeapon::eReload) ? "RELOAD" : "UNKNOWN")),int(m_bFiring));
	if (weapon_state == eObjectActionIdle) {
		m_bFiring = false;
		if (tpWeapon->STATE == CWeapon::eFire)
			stalker->m_inventory.Action(kWPN_FIRE, CMD_STOP);
		xr_vector<CInventorySlot>::iterator I = stalker->m_inventory.m_slots.begin(), B = I;
		xr_vector<CInventorySlot>::iterator E = stalker->m_inventory.m_slots.end() - 2;
		s32 best_slot = -1;
		for ( ; I != E; ++I)
			if ((*I).m_pIItem && ((I - B) != (int)stalker->m_inventory.GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
				best_slot = u32(I - B);
		if (best_slot > (int)stalker->m_inventory.GetActiveSlot())
			stalker->m_inventory.Activate(best_slot);
	}
	else
		if (weapon_state == eObjectActionPrimaryFire)
			if (tpWeapon->STATE == CWeapon::eFire)
				if (check_if_can_kill_enemy(stalker->head_orientation().current) && !check_if_can_kill_member()) {
					u32 dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
					if (dwStartFireAmmo) {
						m_bFiring = true;
						m_dwNoFireTime = Level().timeServer() + ::Random.randI(dwFireDelayMin,dwFireDelayMax);
					}
					else {
						stalker->m_inventory.Action(kWPN_FIRE, CMD_STOP);
						m_bFiring = false;
					}
				}
				else {
					stalker->m_inventory.Action(kWPN_FIRE, CMD_STOP);
					m_bFiring = false;
					m_dwNoFireTime = Level().timeServer();
				}
			else
				if (tpWeapon->STATE == CWeapon::eIdle)
					if (check_if_can_kill_enemy(stalker->head_orientation().current) && !check_if_can_kill_member()) {
						dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
						if (dwStartFireAmmo && (Level().timeServer() > m_dwNoFireTime)) {
							if (m_bFiring && (Level().timeServer() - m_dwStartFireTime > TIME_FIRE_DELTA)) {
								stalker->m_inventory.Action(kWPN_FIRE, CMD_START);
//								Msg				("Weapon could fire (%d)",Level().timeServer());
							}
							else {
								m_dwNoFireTime = Level().timeServer();
								stalker->m_inventory.Action(kWPN_FIRE, CMD_STOP);
							}
							if (!m_bFiring) {
								m_dwStartFireTime	= Level().timeServer();
//								Msg				("Weapon shouldn't fire (%d)",Level().timeServer());
							}
							m_bFiring = true;
						}
					}
					else
						m_bFiring = false;
				else
					m_bFiring = false;
		else {
			if (tpWeapon->STATE == CWeapon::eFire)
				stalker->m_inventory.Action(kWPN_FIRE,	CMD_STOP);
			m_bFiring = false;
		}
			
	if (tpWeaponMagazined) {
		dwStartFireAmmo = tpWeaponMagazined->GetAmmoElapsed();
		if ((!dwStartFireAmmo) && (CWeapon::eReload != tpWeapon->STATE))
			if (tpWeaponMagazined->IsAmmoAvailable())
				stalker->m_inventory.Action(kWPN_RELOAD,	CMD_START);
			else
				if ((CWeapon::eHidden != tpWeapon->STATE) && (CWeapon::eHiding != tpWeapon->STATE)) {
					if (tpWeaponMagazined->STATE == CWeapon::eFire)
						stalker->m_inventory.Action(kWPN_FIRE,	CMD_STOP);
					xr_vector<CInventorySlot>::iterator I = stalker->m_inventory.m_slots.begin(), B = I;
					xr_vector<CInventorySlot>::iterator E = stalker->m_inventory.m_slots.end() - 2;
					u32 best_slot = u32(-1);
					for ( ; I != E; ++I)
						if ((*I).m_pIItem && ((I - B) != (int)stalker->m_inventory.GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
							best_slot = u32(I - B);
					if (-1 != best_slot)														   
						stalker->m_inventory.Activate(best_slot);
//					else {
//						stalker->m_inventory.Ruck(tpWeaponMagazined);
//						tpWeaponMagazined->Drop();
//					}
				}
	}

	bool bFiring = (tpWeapon->STATE == CWeapon::eFire) || (tpWeapon->STATE == CWeapon::eFire2);

	if (tpWeapon && (tpWeapon->STATE != CWeapon::eIdle)) {
		CStalkerMovementManager	*stalker_movement_manager = dynamic_cast<CStalkerMovementManager*>(this);
		if (stalker_movement_manager && (stalker_movement_manager->mental_state() == eMentalStateFree))
			stalker_movement_manager->set_mental_state(eMentalStateDanger);
	}


	CGroup &Group = *stalker->getGroup();
	if ((bSafeFire) && (!bFiring))
		--(Group.m_dwFiring);
	else
		if ((!bSafeFire) && (bFiring))
			++Group.m_dwFiring;
}