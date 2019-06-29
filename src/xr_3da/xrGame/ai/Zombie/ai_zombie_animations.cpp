////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_animations.cpp
//	Created 	: 21.06.2002
//  Modified 	: 06.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Animations, Bone transformations and Sounds for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

// sounds
void CAI_Zombie::vfLoadSounds()
{
	// attack
	::Sound->create(m_tpaSoundAttack[0],TRUE,"monsters/zombie/attack/zm1att_1",SOUND_TYPE_MONSTER_ATTACKING);
	::Sound->create(m_tpaSoundAttack[1],TRUE,"monsters/zombie/attack/zm1att_2",SOUND_TYPE_MONSTER_ATTACKING);
	::Sound->create(m_tpaSoundAttack[2],TRUE,"monsters/zombie/attack/zm1att_3",SOUND_TYPE_MONSTER_ATTACKING);
	::Sound->create(m_tpaSoundAttack[3],TRUE,"monsters/zombie/attack/zm1att_4",SOUND_TYPE_MONSTER_ATTACKING);

	// death
	::Sound->create(m_tpaSoundDeath[0],TRUE,"monsters/zombie/death/zm1die_1",SOUND_TYPE_MONSTER_DYING);
	::Sound->create(m_tpaSoundDeath[1],TRUE,"monsters/zombie/death/zm1die_2",SOUND_TYPE_MONSTER_DYING);
	::Sound->create(m_tpaSoundDeath[2],TRUE,"monsters/zombie/death/zm1die_3",SOUND_TYPE_MONSTER_DYING);

	// hit
	::Sound->create(m_tpaSoundHit[0],TRUE,"monsters/zombie/hit/zm2a1__1",SOUND_TYPE_MONSTER_INJURING);
	::Sound->create(m_tpaSoundHit[1],TRUE,"monsters/zombie/hit/zm2a1__2",SOUND_TYPE_MONSTER_INJURING);
	::Sound->create(m_tpaSoundHit[2],TRUE,"monsters/zombie/hit/zm2a1__3",SOUND_TYPE_MONSTER_INJURING);

	// idle
	::Sound->create(m_tpaSoundIdle[0],TRUE,"monsters/zombie/idle/hh1a0mo1",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundIdle[1],TRUE,"monsters/zombie/idle/hh1a0mo2",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundIdle[2],TRUE,"monsters/zombie/idle/hh1a0mo3",SOUND_TYPE_MONSTER_TALKING);

	// notice
	::Sound->create(m_tpaSoundNotice[0],TRUE,"monsters/zombie/notice/hh1to1_1",SOUND_TYPE_MONSTER_TALKING);

	// pursuit
	::Sound->create(m_tpaSoundPursuit[0],TRUE,"monsters/zombie/pursuit/hh1a1__1",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundPursuit[1],TRUE,"monsters/zombie/pursuit/hh1a1__2",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundPursuit[2],TRUE,"monsters/zombie/pursuit/hh1a1__3",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundPursuit[3],TRUE,"monsters/zombie/pursuit/hh1a1__4",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundPursuit[4],TRUE,"monsters/zombie/pursuit/hh1a1__5",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundPursuit[5],TRUE,"monsters/zombie/pursuit/hh1a1__6",SOUND_TYPE_MONSTER_TALKING);

	// resurrect
	::Sound->create(m_tpaSoundResurrect[0],TRUE,"monsters/zombie/resurrect/hh1a3__1",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundResurrect[1],TRUE,"monsters/zombie/resurrect/hh1a3__2",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundResurrect[2],TRUE,"monsters/zombie/resurrect/hh1a3__3",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundResurrect[3],TRUE,"monsters/zombie/resurrect/hh1a3__4",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundResurrect[4],TRUE,"monsters/zombie/resurrect/hh1a3__5",SOUND_TYPE_MONSTER_TALKING);
	::Sound->create(m_tpaSoundResurrect[5],TRUE,"monsters/zombie/resurrect/hh1a3__6",SOUND_TYPE_MONSTER_TALKING);
}

void CAI_Zombie::vfLoadAnimations()
{
	CSkeletonAnimated* tpVisualObject = PSkeletonAnimated(Visual());
	
	// loading normal animations
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[0]  = tpVisualObject->ID_Cycle("norm_death_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[1]  = tpVisualObject->ID_Cycle("norm_death_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeath[2]  = tpVisualObject->ID_Cycle("norm_death_2");
	
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[0] = tpVisualObject->ID_Cycle("norm_death_idle_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[1] = tpVisualObject->ID_Cycle("norm_death_idle_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[2] = tpVisualObject->ID_Cycle("norm_death_idle_2");

	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[0] = tpVisualObject->ID_Cycle("norm_stand_up_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[1] = tpVisualObject->ID_Cycle("norm_stand_up_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[2] = tpVisualObject->ID_Cycle("norm_stand_up_2");

	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[0] = tpVisualObject->ID_Cycle("norm_attack_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[1] = tpVisualObject->ID_Cycle("norm_attack_1");
	m_tZombieAnimations.tNormal.tGlobal.tpaAttack[2] = tpVisualObject->ID_Cycle("norm_attack_2");
	
	m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0]	 = tpVisualObject->ID_Cycle("norm_idle_0");
	m_tZombieAnimations.tNormal.tGlobal.tpaIdle[1]   = tpVisualObject->ID_Cycle("norm_idle_1");
	
	m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd	 = tpVisualObject->ID_Cycle("norm_walk_fwd_0");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.back	 = tpVisualObject->ID_Cycle("norm_walk_back");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.ls	 = tpVisualObject->ID_Cycle("norm_walk_ls");
	m_tZombieAnimations.tNormal.tGlobal.tWalk.rs	 = tpVisualObject->ID_Cycle("norm_walk_rs");
	
	m_tZombieAnimations.tNormal.tGlobal.tpWalkForwardCSIP = tpVisualObject->ID_Cycle("norm_walk_fwd_1");

	m_tZombieAnimations.tNormal.tGlobal.tpRunForward	 = tpVisualObject->ID_Cycle("norm_run_fwd");
	
	m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft	 = tpVisualObject->ID_Cycle("norm_turn_ls");
	m_tZombieAnimations.tNormal.tGlobal.tpTurnRight  = tpVisualObject->ID_Cycle("norm_turn_rs");
	
	m_tZombieAnimations.tNormal.tTorso.tpDamageLeft  = tpVisualObject->ID_FX("norm_damage_ls");
	m_tZombieAnimations.tNormal.tTorso.tpDamageRight = tpVisualObject->ID_FX("norm_damage_rs");

//	m_tZombieAnimations.tNormal.tTorso.tpBlaBlaBla0  = tpVisualObject->ID_FX("norm_fx_0");

	tpVisualObject->PlayCycle(m_tZombieAnimations.tNormal.tGlobal.tpaIdle[0]);
}

void CAI_Zombie::SelectAnimation(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	CSkeletonAnimated* tpVisualObject = PSkeletonAnimated(Visual());
	CMotionDef*	tpGlobalAnimation = 0;

	if (!g_Alive()) {
		for (int i=0 ;i<3; ++i)
			if (m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i] == m_tpCurrentGlobalAnimation) {
				tpGlobalAnimation = m_tpCurrentGlobalAnimation;
				break;
			}
		
		if (!tpGlobalAnimation) {
			for (int i=0 ;i<3; ++i)
				if (m_tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					if (!m_tpCurrentGlobalBlend->playing)
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i];
					break;
				}
			if (!tpGlobalAnimation)
				tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,3)];
		}
	}
	else
		if (m_eCurrentState == aiZombieResurrect) {
			for (int i=0; i<3; ++i)
				if (m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i] == m_tpCurrentGlobalAnimation) {
					tpGlobalAnimation = m_tpCurrentGlobalAnimation;
					break;
				}
			if (!tpGlobalAnimation) {
				for ( i=0; i<3; ++i)
					if (m_tZombieAnimations.tNormal.tGlobal.tpaDeathIdle[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i];
						break;
					}
				
				if (!tpGlobalAnimation)
					for ( i=0; i<3; ++i)
						if (m_tZombieAnimations.tNormal.tGlobal.tpaDeath[i] == m_tpCurrentGlobalAnimation) {
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaStandUp[i];
							break;
						}
			}
		}
		else
			if (m_bFiring) {
				for (int i=0 ;i<3; ++i)
					if (m_tZombieAnimations.tNormal.tGlobal.tpaAttack[i] == m_tpCurrentGlobalAnimation) {
						tpGlobalAnimation = m_tpCurrentGlobalAnimation;
						break;
					}
				
				if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
					tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaAttack[::Random.randI(0,3)];
			}
			else
				if (m_eCurrentState == aiZombieTurn)
					if (_abs(m_body.target.yaw - m_body.current.yaw) <= PI)
						if (m_body.target.yaw - m_body.current.yaw >= 0)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnRight;
						else
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
					else
						if (m_body.target.yaw > m_body.current.yaw)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnLeft;
						else
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpTurnRight;
				else
					if (m_fSpeed < 0.2f) {
						for (int i=0 ;i<2; ++i)
							if (m_tZombieAnimations.tNormal.tGlobal.tpaIdle[i] == m_tpCurrentGlobalAnimation) {
								tpGlobalAnimation = m_tpCurrentGlobalAnimation;
								break;
							}
						
						if (!tpGlobalAnimation || !m_tpCurrentGlobalBlend || !m_tpCurrentGlobalBlend->playing)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpaIdle[::Random.randI(0,2)];
					}
					else
						if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L)
							tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpRunForward;
						else
							if (_abs(m_fSpeed - m_fMaxSpeed) < EPS_L)
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tpWalkForwardCSIP;
							else
								tpGlobalAnimation = m_tZombieAnimations.tNormal.tGlobal.tWalk.fwd;
	
	if (tpGlobalAnimation != m_tpCurrentGlobalAnimation) { 
		m_tpCurrentGlobalAnimation = tpGlobalAnimation;
		if (tpGlobalAnimation) {
			if (tpGlobalAnimation == m_tZombieAnimations.tNormal.tGlobal.tpRunForward) {
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
				m_tpCurrentGlobalBlend->timeCurrent = ::Random.randF(m_tpCurrentGlobalBlend->timeTotal);
			}
			else
				m_tpCurrentGlobalBlend = tpVisualObject->PlayCycle(tpGlobalAnimation);
		}
	}
}