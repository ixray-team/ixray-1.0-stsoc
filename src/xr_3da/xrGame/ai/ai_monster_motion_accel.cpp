#include "stdafx.h"
#include "ai_monster_motion.h"
#include "biting\ai_biting.h"

void CMotionManager::accel_init()
{
	m_accel.active = false;
}
void CMotionManager::accel_load(LPCSTR section)
{
	m_accel.calm			= pSettings->r_float(section, "Accel_Calm");
	m_accel.aggressive		= pSettings->r_float(section, "Accel_Aggressive");
}

void CMotionManager::accel_activate(EAccelType type)
{
	m_accel.active			= true;
	m_accel.type			= type;
	
	m_accel.enable_braking	= true;
}

float CMotionManager::accel_get(EAccelValue val)
{
	if (!accel_active(val)) return flt_max;

	switch(m_accel.type) {
	case eAT_Calm:			return m_accel.calm;
	case eAT_Aggressive:	return m_accel.aggressive;
	default:				return m_accel.calm;
	}
}

// -----------------------------------------------------------------------------------------

void CMotionManager::accel_chain_add(EMotionAnim anim1, EMotionAnim anim2)
{
	SEQ_VECTOR v_temp;
	v_temp.push_back(anim1);
	v_temp.push_back(anim2);

	m_accel.chain.push_back(v_temp);
}

bool CMotionManager::accel_chain_get(float cur_speed, EMotionAnim target_anim, EMotionAnim &new_anim, float &a_speed)
{
	VELOCITY_CHAIN_VEC_IT B = m_accel.chain.begin(), I;
	VELOCITY_CHAIN_VEC_IT E = m_accel.chain.end();

	// пройти по всем Chain-векторам
	for (I = B; I != E; I++) {
		SEQ_VECTOR_IT	IT_B		= I->begin(), IT;
		SEQ_VECTOR_IT	IT_E		= I->end();
		SEQ_VECTOR_IT	best_anim	= IT_E;
		SVelocityParam	*best_param	= 0;

		bool		  found		= false;

		// Пройти по текущему вектору
		for (IT = IT_B; IT != IT_E; IT++) {


			ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(*IT);
			SVelocityParam		*param	= item_it->second.velocity;
			float				from	= param->velocity.linear * param->min_factor;
			float				to		= param->velocity.linear * param->max_factor;

			if ( ((from <= cur_speed+EPS_L) && (cur_speed <= to + EPS_L))	|| 
				((cur_speed < from) && (IT == I->begin()))					|| 
				((cur_speed + EPS_L >= to) &&	(IT+1 == I->end())) ) {
					best_anim	= IT;
					best_param	= item_it->second.velocity;
				}

				if ((*IT) == target_anim) found = true;
				if (found && best_param) break;
		}

		if (!found) continue;

		R_ASSERT2(best_param,"probably incompatible speed ranges");
		// calc anim_speed
		new_anim	= *best_anim;
		a_speed		= GetAnimSpeed(new_anim) * cur_speed / best_param->velocity.linear;
		return true;
	}
	return false;
}

bool CMotionManager::accel_chain_test()
{
	string256 error_msg;

	// пройти по всем Chain-векторам
	for (VELOCITY_CHAIN_VEC_IT I = m_accel.chain.begin(); I != m_accel.chain.end(); I++) {

		VERIFY2(I->size() >= 2, error_msg);

		ANIM_ITEM_MAP_IT	anim_from	= get_sd()->m_tAnims.find(*(I->begin()));
		ANIM_ITEM_MAP_IT	anim_to;

		// Пройти по текущему вектору
		for (SEQ_VECTOR_IT IT = I->begin() + 1; IT != I->end(); IT++) {
			anim_to = get_sd()->m_tAnims.find(*IT);
			
			float from	=	anim_from->second.velocity->velocity.linear * anim_from->second.velocity->max_factor;
			float to	=	anim_to->second.velocity->velocity.linear * anim_to->second.velocity->min_factor;
			
			sprintf(error_msg,"Incompatible speed ranges. Monster[%s] From animation  [%s] To animation [%s]",*pMonster->cName(),*anim_from->second.target_name, *anim_to->second.target_name);
			VERIFY2(to < from, error_msg);
			
			anim_from = anim_to;
		}
	}

	return true;
}

bool CMotionManager::accel_check_braking(float before_interval)
{
	if (!pMonster->IsMovingOnPath())	return false;
	if (!accel_active(eAV_Braking))		return false;

	float acceleration = accel_get(eAV_Braking);
	float braking_dist	= (pMonster->m_velocity_linear.current * pMonster->m_velocity_linear.current) / acceleration;
	braking_dist += before_interval;

	// проверить точки пути, где необходимо остановиться
	float dist = 0.f;	// дистанция до найденной точки	
	for (u32 i=pMonster->curr_travel_point_index()+1; i < pMonster->CDetailPathManager::path().size(); i++) {
		dist += pMonster->CDetailPathManager::path()[i].position.distance_to(pMonster->CDetailPathManager::path()[i-1].position);

		if ((pMonster->CDetailPathManager::path()[i].velocity == pMonster->eVelocityParameterStand) && (dist < braking_dist)) {
			return true;
		}
	}

	if (pMonster->CMonsterMovement::IsPathEnd(braking_dist)) return true;

	return false;
}


