////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_feel.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Visibility and look for all the biting monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../actor.h"
#include "../../ActorEffector.h"

#include "../ai_monster_effector.h"

void CAI_Biting::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive()) return;

	// ignore sounds from team
	CEntityAlive* E = smart_cast<CEntityAlive*> (who);
	if (E && (E->g_Team() == g_Team()) || (this == who)) return;
	
	// ignore unknown sounds
	if (eType == 0xffffffff) return;

	// ignore distant sounds
	if (this->Position().distance_to(Position) > get_sd()->m_max_hear_dist) return;

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;

	CScriptMonster	*script_monster = smart_cast<CScriptMonster*>(this);
	if (script_monster)
		script_monster->sound_callback(who,eType,Position,power);
	
	if (power >= get_sd()->m_fSoundThreshold) {
		if (this != who) {
			SoundMemory.HearSound(who,eType,Position,power,m_current_update);
		}
 	}
}

void CAI_Biting::HitEntity(const CEntity *pEntity, float fDamage, float impulse, Fvector &dir)
{
	if (!g_Alive()) return;
	if (!pEntity) return;

	if (!EnemyMan.get_enemy()) return;

	if ((CLSID_ENTITY == EnemyMan.get_enemy()->CLS_ID) && (EnemyMan.get_enemy() == pEntity)) {
		Fvector position_in_bone_space;
		position_in_bone_space.set(0.f,0.f,0.f);

		// перевод из локальных координат в мировые вектора направления импульса
		Fvector hit_dir;
		XFORM().transform_dir(hit_dir,dir);
		hit_dir.normalize();

		CEntity		*pEntityNC	= const_cast<CEntity*>(pEntity);
		VERIFY		(pEntityNC);
		pEntityNC->Hit(fDamage,hit_dir,this,0,position_in_bone_space,impulse);
		
		if (smart_cast<CActor *>(pEntityNC)) SetAttackEffector();
	}
}


BOOL  CAI_Biting::feel_vision_isRelevant(CObject* O)
{
	if (CLSID_ENTITY!=O->CLS_ID) return FALSE;
	
	if ((O->spatial.type & STYPE_VISIBLEFORAI) != STYPE_VISIBLEFORAI) return FALSE;
	
	CEntityAlive* E = smart_cast<CEntityAlive*> (O);
	if (!E) return FALSE;
	if (E->g_Team() == g_Team() && E->g_Alive()) return FALSE;

	return TRUE;
}

void CAI_Biting::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (!g_Alive()) return;
	
	feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);
	if (g_Alive()) CSoundPlayer::play(MonsterSpace::eMonsterSoundTakeDamage);

	if (element < 0) return;

	// Определить направление хита (перед || зад || лево || право)
	float yaw,pitch;
	vLocalDir.getHP(yaw,pitch);
	
	yaw = angle_normalize(yaw);
	
	EHitSide hit_side = eSideFront;
	if ((yaw >= PI_DIV_4) && (yaw <= 3*PI_DIV_4)) hit_side = eSideLeft;
	else if ((yaw >= 3 * PI_DIV_4) && (yaw <= 5*PI_DIV_4)) hit_side = eSideBack;
	else if ((yaw >= 5 * PI_DIV_4) && (yaw <= 7*PI_DIV_4)) hit_side = eSideRight;

	MotionMan.FX_Play(hit_side, 1.0f);

	HitMemory.add_hit(who,hit_side);

	CScriptMonster	*script_monster = smart_cast<CScriptMonster*>(this);
	if (script_monster)
		script_monster->hit_callback(amount,vLocalDir,who,element);
	
}

bool CAI_Biting::RayPickEnemy(const CObject *target_obj, const Fvector &trace_from, const Fvector &dir, float dist, float radius, u32 num_picks)
{
	bool ret_val = false;
	
	this->setEnabled(false);
	Collide::rq_result	l_rq;
	
	if (Level().ObjectSpace.RayPick(trace_from, dir, dist, Collide::rqtObject, l_rq)) {
		if ((l_rq.O == target_obj) && (l_rq.range < dist)) ret_val = true;
	} else {
		// макс. угол отклонения
		float max_alpha = atanf(radius/dist);
		
		float src_h, src_p;	
		dir.getHP(src_h,src_p);

		Fvector new_dir;
		float new_h,new_p;

		for (u32 i=0;i<num_picks;i++) {

			new_h = src_h + ::Random.randF(-max_alpha, max_alpha);
			new_p = src_p + ::Random.randF(-max_alpha, max_alpha);

			new_dir = dir;
			new_dir.setHP(new_h,new_p);
			new_dir.normalize();

			if (Level().ObjectSpace.RayPick(trace_from, new_dir, dist, Collide::rqtObject, l_rq)) {
				if ((l_rq.O == target_obj) && (l_rq.range < dist)) {
					ret_val = true;
					break;
				}
			}
		}
	}

	this->setEnabled(true);	
	return ret_val;
}

void CAI_Biting::SetAttackEffector() 
{
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());
	if (pA) {
		pA->EffectorManager().AddEffector(xr_new<CMonsterEffectorHit>(get_sd()->m_attack_effector.ce_time,get_sd()->m_attack_effector.ce_amplitude,get_sd()->m_attack_effector.ce_period_number,get_sd()->m_attack_effector.ce_power));
		Level().Cameras.AddEffector(xr_new<CMonsterEffector>(get_sd()->m_attack_effector.ppi, get_sd()->m_attack_effector.time, get_sd()->m_attack_effector.time_attack, get_sd()->m_attack_effector.time_release));
	}
}

