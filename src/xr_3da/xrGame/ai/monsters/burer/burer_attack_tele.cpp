#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer_attack_tele.h"
#include "burer.h"
#include "../../../PhysicsShell.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ai_monster_space.h"

#define GOOD_DISTANCE_FOR_TELE	15.f
#define TELE_DELAY				4000

#define MAX_TIME_CHECK_FAILURE	6000

CBurerAttackTele::CBurerAttackTele(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackTele::Init()
{
	inherited::Init();
	
	UpdateExternal();

	m_tAction		= ACTION_TELE_STARTED;
	selected_object	= 0;

	SelectObjects();

	time_started			= 0;

	// запретить взятие скриптом
	pMonster->set_script_capture(false);
}

//////////////////////////////////////////////////////////////////////////
// Главный цикл выполнения
//////////////////////////////////////////////////////////////////////////

void CBurerAttackTele::Run()
{

	switch (m_tAction) {
		/************************/
		case ACTION_TELE_STARTED:
		/************************/
			
			ExecuteTeleStart();
			m_tAction = ACTION_TELE_CONTINUE;

			break;
		/************************/
		case ACTION_TELE_CONTINUE:
		/************************/

			ExecuteTeleContinue();

			break;

		/************************/
		case ACTION_TELE_FIRE:
		/************************/

			ExecuteTeleFire();
			m_tAction = ACTION_WAIT_TRIPLE_END;

			break;
		/***************************/
		case ACTION_WAIT_TRIPLE_END:
		/***************************/
			
			if (!pMonster->MotionMan.TA_IsActive()) {
				if (IsActiveObjects())
					m_tAction = ACTION_TELE_STARTED;
				else 
					m_tAction = ACTION_COMPLETED; 
			}

		/*********************/
		case ACTION_COMPLETED:
		/*********************/

			break;
	}

	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	pMonster->DirMan.face_target(enemy, 700);

}

void CBurerAttackTele::Done()
{
	tele_objects.clear();
	pMonster->DeactivateShield();
	
	// clear particles on active objects
	if (pMonster->CTelekinesis::is_active()) {
		for (u32 i=0; i<pMonster->CTelekinesis::get_objects_count(); i++) {
			pMonster->StopTeleObjectParticle(pMonster->CTelekinesis::get_object_by_index(i).get_object());
		}
	}

	// отменить запрет на взятие скриптом
	pMonster->set_script_capture(true);
}

void CBurerAttackTele::CriticalInterrupt()
{
	pMonster->MotionMan.TA_Deactivate();
	pMonster->CTelekinesis::Deactivate();
	tele_objects.clear();
	pMonster->DeactivateShield();

	// clear particles on active objects
	if (pMonster->CTelekinesis::is_active()) {
		for (u32 i=0; i<pMonster->CTelekinesis::get_objects_count(); i++) {
			pMonster->StopTeleObjectParticle(pMonster->CTelekinesis::get_object_by_index(i).get_object());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// проверить, может ли стартовать телекинез
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::CheckTeleStart()
{
	// проверка на текущую активность 
	if (IsActiveObjects()) return false;
	
	// проверить дистанцию до врага
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < GOOD_DISTANCE_FOR_TELE) return false;

	// найти телекинетические объекты
	FindObjects();

	// если нет объектов
	if (tele_objects.empty()) return false;

	// всё ок можно начинать телекинез
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////
// выполнение состояния 'Подготовка телекинеза' (предметы уже должны быть под контролем)
////////////////////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleStart()
{
	pMonster->MotionMan.TA_Activate(&pMonster->anim_triple_tele);
	time_started = m_dwCurrentTime;
	pMonster->ActivateShield();
}

//////////////////////////////////////////////////////////////////////////
// выполнение состояния 'Держать предметы'
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleContinue()
{
	if (time_started + pMonster->m_tele_time_to_hold > m_dwCurrentTime) return;
	
	// найти объект для атаки
	bool object_found = false;
	CTelekineticObject tele_object;

	u32 i=0;
	while (i < pMonster->CTelekinesis::get_objects_count()) {
		 tele_object = pMonster->CTelekinesis::get_object_by_index(i);

		if ((tele_object.get_state() == TS_Keep) && (tele_object.time_keep_started + 1500 < m_dwCurrentTime)) {

			object_found = true;
			break;

		} else i++;

	}

	if (object_found) {
		m_tAction		= ACTION_TELE_FIRE;
		selected_object = tele_object.get_object();
	} else {
		if (!IsActiveObjects() || (time_started + MAX_TIME_CHECK_FAILURE < m_dwCurrentTime)) {
			pMonster->MotionMan.TA_Deactivate();
			m_tAction = ACTION_COMPLETED;
		} 
	}
}

//////////////////////////////////////////////////////////////////////////
// выполнение состояния 'Удар'
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::ExecuteTeleFire()
{
	pMonster->MotionMan.TA_PointBreak();

	Fvector enemy_pos = enemy->Position();
	enemy_pos.y += 5 * enemy->Radius();

	float dist = selected_object->Position().distance_to(enemy->Position());
	pMonster->CTelekinesis::fire(selected_object, enemy_pos, dist/12);

	pMonster->StopTeleObjectParticle(selected_object);
	
	pMonster->sound().play(MonsterSpace::eMonsterSoundTeleAttack);
	pMonster->DeactivateShield();
}


//////////////////////////////////////////////////////////////////////////
// Проверка, есть ли хоть один объект под контролем
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::IsActiveObjects()
{
	return (pMonster->CTelekinesis::get_objects_count() > 0);
}

//////////////////////////////////////////////////////////////////////////
// Поиск объектов для телекинеза
//////////////////////////////////////////////////////////////////////////
void CBurerAttackTele::FindObjects()
{
	tele_objects.clear();

	// получить список объектов вокруг монстра
	Level().ObjectSpace.GetNearest	(pMonster->Position(), pMonster->m_tele_find_radius);
	xr_vector<CObject*> &tpObjects	= Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[i]);
		if (!obj || !obj->PPhysicsShell() || !obj->PPhysicsShell()->bActive || (obj->m_pPhysicsShell->getMass() < pMonster->m_tele_object_min_mass) || (obj->m_pPhysicsShell->getMass() > pMonster->m_tele_object_max_mass) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		tele_objects.push_back(obj);
	}

	
	// получить список объектов между монстром и врагом
	float dist = enemy->Position().distance_to(pMonster->Position());
	Fvector dir;
	dir.sub(enemy->Position(), pMonster->Position());
	dir.normalize();

	Fvector pos;
	pos.mad(pMonster->Position(), dir, dist / 2.f);
	Level().ObjectSpace.GetNearest(pos, pMonster->m_tele_find_radius); 
	tpObjects = Level().ObjectSpace.q_nearest;

	for (u32 i=0;i<tpObjects.size();i++) {
		CPhysicsShellHolder *obj = smart_cast<CPhysicsShellHolder *>(tpObjects[i]);
		if (!obj || !obj->m_pPhysicsShell || !obj->m_pPhysicsShell->bActive || (obj->m_pPhysicsShell->getMass() < pMonster->m_tele_object_min_mass) || (obj->m_pPhysicsShell->getMass() > pMonster->m_tele_object_max_mass) || (obj == pMonster) || pMonster->CTelekinesis::is_active_object(obj)) continue;

		tele_objects.push_back(obj);
	}
}
	

//////////////////////////////////////////////////////////////////////////
// Выбор подходящих объектов для телекинеза
//////////////////////////////////////////////////////////////////////////
class best_object_predicate {
	Fvector enemy_pos;
	Fvector monster_pos;
public:
	best_object_predicate(const Fvector &m_pos, const Fvector &pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CGameObject *tpObject1, const CGameObject *tpObject2) const
	{
	
		float dist1 = monster_pos.distance_to(tpObject1->Position());
		float dist2 = enemy_pos.distance_to(tpObject2->Position());
		float dist3 = enemy_pos.distance_to(monster_pos);

		return ((dist1 < dist3) && (dist2 > dist3));
	};
};

class best_object_predicate2 {
	Fvector enemy_pos;
	Fvector monster_pos;
public:
	best_object_predicate2(const Fvector &m_pos, const Fvector &pos) {
		monster_pos = m_pos;
		enemy_pos	= pos;
	}

	bool operator()	 (const CGameObject *tpObject1, const CGameObject *tpObject2) const
	{
		float dist1 = enemy_pos.distance_to(tpObject1->Position());
		float dist2 = enemy_pos.distance_to(tpObject2->Position());

		return (dist1 < dist2);		
	};
};





void CBurerAttackTele::SelectObjects()
{
	sort(tele_objects.begin(),tele_objects.end(),best_object_predicate2(pMonster->Position(), enemy->Position()));

	// выбрать объект
	for (u32 i=0; i<tele_objects.size(); i++) {
		CPhysicsShellHolder *obj = tele_objects[i];

		// применить телекинез на объект
		pMonster->CTelekinesis::activate(obj, 3.f, 2.f, 10000);
		pMonster->StartTeleObjectParticle(obj);

		// удалить из списка
		tele_objects[i] = tele_objects[tele_objects.size()-1];
		tele_objects.pop_back();

		if (pMonster->CTelekinesis::get_objects_count() >= pMonster->m_tele_max_handled_objects) break;
	}
}

//////////////////////////////////////////////////////////////////////////
// проверка на завершения текущего состояния телекинеза
//////////////////////////////////////////////////////////////////////////
bool CBurerAttackTele::IsCompleted()
{
	return (m_tAction == ACTION_COMPLETED);
}

bool CBurerAttackTele::CheckStartCondition()
{
	return CheckTeleStart();
}

void CBurerAttackTele::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}
