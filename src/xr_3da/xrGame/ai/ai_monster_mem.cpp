#include "stdafx.h"
#include "../CustomMonster.h"
#include "ai_monster_mem.h"
#include "../actor.h"

#include "biting/ai_biting.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSoundMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CHECK_SOUND_TYPE(a,b,c) { if ((a & b) == b) return c; }

TSoundDangerValue tagSoundElement::ConvertSoundType(ESoundTypes stype)
{ 
	
	if (((stype & SOUND_TYPE_WEAPON) != SOUND_TYPE_WEAPON) && 
		((stype & SOUND_TYPE_MONSTER) != SOUND_TYPE_MONSTER) && 
		((stype & SOUND_TYPE_WORLD) != SOUND_TYPE_WORLD)) return NONE_DANGEROUS_SOUND;

	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_RECHARGING,	WEAPON_RECHARGING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_SHOOTING,		WEAPON_SHOOTING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_ITEM_TAKING,			WEAPON_TAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_ITEM_HIDING,			WEAPON_HIDING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_EMPTY_CLICKING,	WEAPON_EMPTY_CLICKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WEAPON_BULLET_HIT,	WEAPON_BULLET_RICOCHET);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_DYING,		MONSTER_DYING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_INJURING,	MONSTER_INJURING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_STEP,		MONSTER_WALKING);
	
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_TALKING,		MONSTER_TALKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_MONSTER_ATTACKING,	MONSTER_ATTACKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_BREAKING,	OBJECT_BREAKING);
	CHECK_SOUND_TYPE(stype,	SOUND_TYPE_WORLD_OBJECT_COLLIDING,	OBJECT_FALLING);


	return NONE_DANGEROUS_SOUND;
}


void CSoundMemory::Init(TTime mem_time) 
{
	Deinit(); timeMemory = mem_time; 
	
	pMonster = dynamic_cast<CCustomMonster *>(this);
	if (!pMonster) R_ASSERT("Cannot dynamic_cast from CSoundMemory to CCustomMonster!");

	Sounds.reserve(20);
}

void CSoundMemory::Deinit()
{

}

void CSoundMemory::HearSound(const SoundElem &s)
{
	if (NONE_DANGEROUS_SOUND	== s.type) return;
	if (DOOR_OPENING			<= s.type) return;
	
	// поиск в массиве звука
	xr_vector<SoundElem>::iterator it;
	
	bool b_sound_replaced = false;
	for (it = Sounds.begin(); Sounds.end() != it; ++it) {
		if ((s.who == it->who) && (it->type == s.type)) {
			if (s.time >= it->time) {
				*it = s; 
				b_sound_replaced = true;
			}
		}
	}
	
	if (!b_sound_replaced) Sounds.push_back(s);

}

void CSoundMemory::HearSound(const CObject* who, int eType, const Fvector &Position, float power, TTime time)
{
	SoundElem s;
	s.SetConvert(who,eType,Position,power,time);
	s.CalcValue(time,pMonster->Position());

	HearSound(s);
} 

void CSoundMemory::GetSound(SoundElem &s, bool &bDangerous)
{
	VERIFY(!Sounds.empty());

	// возврат самого опасного
	s = GetSound();

	if (s.type > WEAPON_EMPTY_CLICKING) bDangerous = false;
	else bDangerous = true;
}

SoundElem &CSoundMemory::GetSound()
{
	VERIFY(!Sounds.empty());
	
	xr_vector<SoundElem>::iterator it = std::max_element(Sounds.begin(), Sounds.end());
	return (*it);
}


struct pred_remove_nonactual_sounds {
	TTime new_time;

	pred_remove_nonactual_sounds(TTime time) {new_time = time;}

	bool operator() (const SoundElem &x) {

		// удалить звуки от объектов, перешедших в оффлайн	
		if (x.who && x.who->getDestroy()) return true;

		// удалить 'старые' звуки
		if (x.time < new_time)	return true;

		// удалить звуки от неживых объектов
		if (x.who) {
			const CEntityAlive *pE = dynamic_cast<const CEntityAlive*> (x.who);
			if (pE && !pE->g_Alive()) return true;
		}

		return false;
	}
};


void CSoundMemory::UpdateHearing(TTime dt)
{
	timeCurrent = dt;

	// удаление устаревших звуков
	xr_vector<SoundElem>::iterator I = remove_if(Sounds.begin(), Sounds.end(), pred_remove_nonactual_sounds(timeCurrent - timeMemory));
	Sounds.erase(I,Sounds.end());

	// пересчитать value
	for (I=Sounds.begin(); I!=Sounds.end(); I++) I->CalcValue(timeCurrent, pMonster->Position());


	

//#ifdef DEBUG
//	CAI_Biting *pB = dynamic_cast<CAI_Biting*>(this);
//
//	pB->HDebug->HT_Clear();
//	pB->HDebug->L_Clear();
//
//	for (u32 i=0; i<Sounds.size();i++) {
//		string128 s_type;
//		switch(Sounds[i].type){
//			case WEAPON_SHOOTING:			strcpy(s_type,"WEAPON_SHOOTING"); break;
//			case MONSTER_ATTACKING:			strcpy(s_type,"MONSTER_ATTACKING"); break;
//			case WEAPON_BULLET_RICOCHET:	strcpy(s_type,"WEAPON_BULLET_RICOCHET"); break;
//			case WEAPON_RECHARGING:			strcpy(s_type,"WEAPON_RECHARGING"); break;
//
//			case WEAPON_TAKING:				strcpy(s_type,"WEAPON_TAKING"); break;
//			case WEAPON_HIDING:				strcpy(s_type,"WEAPON_HIDING"); break;
//			case WEAPON_CHANGING:			strcpy(s_type,"WEAPON_CHANGING"); break;
//			case WEAPON_EMPTY_CLICKING:		strcpy(s_type,"WEAPON_EMPTY_CLICKING"); break;
//
//			case MONSTER_DYING:				strcpy(s_type,"MONSTER_DYING"); break;
//			case MONSTER_INJURING:			strcpy(s_type,"MONSTER_INJURING"); break;
//			case MONSTER_WALKING:			strcpy(s_type,"MONSTER_WALKING"); break;
//			case MONSTER_JUMPING:			strcpy(s_type,"MONSTER_JUMPING"); break;
//			case MONSTER_FALLING:			strcpy(s_type,"MONSTER_FALLING"); break;
//			case MONSTER_TALKING:			strcpy(s_type,"MONSTER_TALKING"); break;
//
//			case DOOR_OPENING:				strcpy(s_type,"DOOR_OPENING"); break;
//			case DOOR_CLOSING:				strcpy(s_type,"DOOR_CLOSING"); break;
//			case OBJECT_BREAKING:			strcpy(s_type,"OBJECT_BREAKING"); break;
//			case OBJECT_FALLING:			strcpy(s_type,"OBJECT_FALLING"); break;
//			case NONE_DANGEROUS_SOUND:		strcpy(s_type,"NONE_DANGEROUS_SOUND"); break;
//		}
//
//		string128 s;
//		sprintf(s,"S[%i]:: type=%s : time=%u : power=%.3f !!! val=%i", i, s_type, Sounds[i].time, Sounds[i].power, Sounds[i].value);
//		pB->HDebug->HT_Add(50, 50 + i*20, s);
//		pB->HDebug->L_AddPoint(Sounds[i].position,0.15f,D3DCOLOR_XRGB(0,255,255));
//		Fvector new_v;
//		new_v = Sounds[i].position;
//		new_v.y += 5.0f;
//		pB->HDebug->L_AddLine(Sounds[i].position, new_v, D3DCOLOR_XRGB(0,255,255));
//	}
//
//#endif

}



//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------
CVisionMemory::CVisionMemory()
{

}

void CVisionMemory::Init(TTime mem_time) 
{
	Deinit();

	timeMemoryDefault	= timeMemory = mem_time;
	timeLastUpdateIgnoreObjects = 0;

	timeCurrent			= 0;

	Selected.obj		= 0;

	Enemies.reserve(20);
	Objects.reserve(20);

	pMonster = dynamic_cast<CCustomMonster *>(this);
	if (!pMonster) R_ASSERT("Cannot dynamic_cast from CVisionMemory to CCustomMonster!");
}
void CVisionMemory::Deinit() 
{
	Objects.clear	(); 
	Enemies.clear	();
	Selected.obj	= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка видимой информации, обновление и классификация видимых объектов
// Заполняет массивы Objects и Enemies
void CVisionMemory::UpdateVision(TTime dt) 
{
	timeCurrent	= dt;

	// Обновить врагов
	VisionElem ve;
	xr_set<const CEntityAlive *>::const_iterator	I1 = pMonster->enemies().begin();
	xr_set<const CEntityAlive *>::const_iterator	E1 = pMonster->enemies().end();
	for ( ; I1 != E1; ++I1) {
		if (!pMonster->CMemoryManager::visible(*I1)) continue;
		ve.Set	(*I1,timeCurrent);
		AddEnemy(ve);
	}

	// Обновить объекты
	xr_set<const CGameObject *>::const_iterator	I2 = pMonster->CItemManager::items().begin();
	xr_set<const CGameObject *>::const_iterator	E2 = pMonster->CItemManager::items().end();
	for ( ; I2 != E2; ++I2) {
		ve.Set	(dynamic_cast<const CEntity*>(*I2),timeCurrent);
		AddObject(ve);
	}

	RemoveNonactualElems();
	
	// обновить Selected
	if (IsEnemy())			SelectEnemy();
	else if (IsObject())	SelectCorpse();
	else Selected.obj = 0;
	
}
 
void CVisionMemory::AddObject(VisionElem &ve)
{
	ITERATOR_VE res;

	res = std::find(Objects.begin(), Objects.end(), ve);
	if (Objects.end() == res) Objects.push_back(ve);
	else *res = ve;

}

void CVisionMemory::AddEnemy(VisionElem &ve)
{
	ITERATOR_VE res;

	res = std::find(Enemies.begin(), Enemies.end(), ve);
	if (Enemies.end() == res) Enemies.push_back(ve);
	else *res = ve;

	if (ve.obj == Selected.obj) Selected = ve;
}


void CVisionMemory::SelectEnemy()
{
	// пришло время перевыбора врага
	if (Selected.obj && (Selected.time + TIME_TO_RESELECT_ENEMY < timeCurrent)) 
		Selected.obj = 0;

	// Необходимо выбрать другого врага?
	//if (!Selected.obj) Selected = GetNearestObject(ENEMY);

	Selected = GetNearestObject(ENEMY);
}

void CVisionMemory::SelectCorpse()
{
	Selected = GetNearestObject(OBJECT);
}

bool CVisionMemory::Get(VisionElem &ve)
{
	if (!Selected.obj) return false;
	ve = Selected;
	return true;
}


// Выбрать ближайший объект к позиции pos
// массив Enemies/Objects (в зависимости от obj_type) не должен быть пустым!
VisionElem &CVisionMemory::GetNearestObject(EObjectType obj_type)
{
	float		optimal_val;
	float		cur_val;
	int			index = 0;		// Индекс ближайшего объекта в массиве объектов 

	VECTOR_VE *ObjectsVector = 0;
	if (ENEMY == obj_type) ObjectsVector = &Enemies;
	else ObjectsVector = &Objects;
	
	optimal_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[index].position) * (timeCurrent - (*ObjectsVector)[index].time + 1);

	for (u32 i=1; i < ObjectsVector->size(); ++i) {
		cur_val = 1000 * pMonster->Position().distance_to((*ObjectsVector)[i].position) * (timeCurrent - (*ObjectsVector)[i].time + 1);

		if ( cur_val < optimal_val){
			optimal_val = cur_val;
			index = i;
		}
	}
	return (*ObjectsVector)[index];
}


// предикат удаления 'неактуальных' врагов
struct predicate_remove_nonactual_enemies {

	TTime	cur_time;
	TTime	mem_time;
	Fvector monster_pos;
	float	eye_range;

	predicate_remove_nonactual_enemies (TTime c_time, TTime m_time, Fvector m_pos, float range) { 
		cur_time = c_time;	mem_time = m_time;	monster_pos = m_pos; eye_range = range;
	}
	bool operator() (const VisionElem &x) { 
		return (
			!x.obj || 
			!x.obj->g_Alive() || 
			x.obj->getDestroy() ||
			(x.time + mem_time < cur_time)
//			(x.obj->Position().distance_to(monster_pos) > eye_range) ||
//			((x.obj->Position().distance_to(monster_pos) > 30) && (x.time != cur_time))
		);
	}
};


// предикат удаления 'неактуальных' объектов
struct predicate_remove_nonactual_objects {
	TTime			new_time;
	predicate_remove_nonactual_objects(TTime time) { new_time = time; }

	bool operator() (const VisionElem &x) { 
		return ((x.time < new_time) ||
				(x.obj && x.obj->getDestroy())
		); 
	}
};

// предикат удаления 'старых' врагов
struct predicate_remove_ignore_objects {
	const CEntity *pObj;

	predicate_remove_ignore_objects(const CEntity *p) { pObj = p; }

	bool operator() (const VisionElem &x) { 
		return (pObj == x.obj); 
	}
};



void CVisionMemory::RemoveNonactualElems()
{
	// удалить 'старых' врагов и тех, расстояние до которых > 30м и др.
	ITERATOR_VE I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_nonactual_enemies(timeCurrent,timeMemory,pMonster->Position(), pMonster->eye_range));
	Enemies.erase(I,Enemies.end());

	// удалить 'старые' объекты
	I = remove_if(Objects.begin(), Objects.end(), predicate_remove_nonactual_objects(timeCurrent - timeMemory));
	Objects.erase(I,Objects.end());

	// удалить в соответствии с ignore_objects
	for (ITERATOR_VE IT = IgnoreObjects.begin(); IgnoreObjects.end() != IT; ++IT)	{
		I = remove_if(Objects.begin(), Objects.end(), predicate_remove_ignore_objects(IT->obj));
		Objects.erase(I,Objects.end());

		I = remove_if(Enemies.begin(), Enemies.end(), predicate_remove_ignore_objects(IT->obj));
		Enemies.erase(I,Enemies.end());
	}

	// удалить 'старые' ignore_objects
	I = remove_if(IgnoreObjects.begin(), IgnoreObjects.end(), predicate_remove_nonactual_objects(timeCurrent - timeMemory));
	IgnoreObjects.erase(I,IgnoreObjects.end());

}

void CVisionMemory::AddIgnoreObject(CEntity *pObj)
{
	VisionElem ve;
	ve.Set(pObj, timeCurrent);
	
	ITERATOR_VE res;
	res = std::find(IgnoreObjects.begin(), IgnoreObjects.end(), ve);
	if (IgnoreObjects.end() == res) IgnoreObjects.push_back(ve);
	else *res = ve;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMonsterMemory::UpdateMemory()
{
	TTime curtime = Level().timeServer();

	UpdateVision(curtime);
	UpdateHearing(curtime);

	RemoveOldDangerousEnemies();

	// AddVisualObjects if there's sounds from them
	for (u32 i=0;i<Sounds.size();i++) {
		if (Sounds[i].who && (Sounds[i].time == curtime)) {
			for (u32 k = 0; k<Enemies.size(); k++) {
				if ((Sounds[i].who->ID() == Enemies[k].obj->ID()) && (Enemies[k].time != curtime)) {
					Enemies[k].time = curtime; 
					LOG_EX("Vision memory updated with sound memory!");
				}
			}
		}
	}

}

void CMonsterMemory::AddDangerousEnemy(const CObject *pO, TTime ttr)
{
	DANGER_ENEMIES_VEC_IT res;
	SDangerousEnemies new_elem;

	res = std::find(danger_enemies.begin(), danger_enemies.end(), pO);
	if (res == danger_enemies.end()) {
		new_elem.who = pO;
		new_elem.time_till_remember = Level().timeServer() + ttr;	
		danger_enemies.push_back(new_elem);
	} else {
		if (res->time_till_remember < Level().timeServer() + ttr) 
			res->time_till_remember = Level().timeServer() + ttr;
	}
}

bool CMonsterMemory::IsDangerousEnemy(const CObject *pO)
{
	DANGER_ENEMIES_VEC_IT res;

	res = std::find(danger_enemies.begin(), danger_enemies.end(), pO);
	if (res == danger_enemies.end()) return false;
	else return true;
}

void CMonsterMemory::RemoveOldDangerousEnemies()
{
	// удалить 'старых' врагов 
	DANGER_ENEMIES_VEC_IT I = remove_if(danger_enemies.begin(), danger_enemies.end(), predicate_remove_old_dangerous_enemies(Level().timeServer()));
	danger_enemies.erase(I,danger_enemies.end());
}



