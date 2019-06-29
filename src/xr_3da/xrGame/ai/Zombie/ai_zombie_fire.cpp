////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"

void CAI_Zombie::Exec_Action(float /**dt/**/)
{
	switch (m_tAction) {
		case eZombieActionAttackBegin : {
			bool bOk = false;
			if (m_tpSoundBeingPlayed) {
				for (int i=0; i<SND_ATTACK_COUNT; ++i)
					if (&(m_tpaSoundAttack[i]) == m_tpSoundBeingPlayed) {
						bOk = true;
						break;
					}
			}
			if (!bOk)
				::Sound->play_at_pos(m_tpaSoundAttack[Random.randI(SND_ATTACK_COUNT)],this,eye_matrix.c);
			else
				m_tpSoundBeingPlayed->set_position(eye_matrix.c);
			u32 dwTime = Level().timeServer();
			if ((m_tSavedEnemy->g_Health() > 0) && (dwTime - m_dwStartAttackTime > m_dwHitInterval)) {
				m_bActionStarted = true;
				m_dwStartAttackTime = dwTime;
				Fvector tDirection;
				Fvector position_in_bone_space;
				position_in_bone_space.set(0.f,0.f,0.f);
				tDirection.sub(m_tSavedEnemy->Position(),this->Position());
				tDirection.normalize();
				
				if ((this->Local()) && (m_tSavedEnemy) && (m_tSavedEnemy->CLS_ID == CLSID_ENTITY))
					m_tSavedEnemy->Hit(m_fHitPower,tDirection,this,-1,position_in_bone_space,0);
			}
			else
				m_bActionStarted = false;

			break;
		}
		case eZombieActionAttackEnd : {
			m_bActionStarted = false;
			break;
		}
		default:
			break;
	}
}

void CAI_Zombie::HitSignal(float /**amount/**/, Fvector& vLocalDir, CObject* who, s16 /**element/**/)
{
	// Save event
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
	m_hit_time = Level().timeServer();
	m_hit_direction.set(D);
	m_hit_direction.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit-ref_sound
	ref_sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
	if (g_Health() > 0) {
		if (S.feedback)
			return;
		if (Random.randI(2))
			return;
		::Sound->play_at_pos		(S,this,Position());
	}
}