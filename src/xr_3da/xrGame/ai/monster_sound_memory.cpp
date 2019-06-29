#include "stdafx.h"
#include "monster_sound_memory.h"
#include "biting/ai_biting.h"

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


CMonsterSoundMemory::CMonsterSoundMemory()
{
	Sounds.reserve(20);
}
CMonsterSoundMemory::~CMonsterSoundMemory()
{

}

void CMonsterSoundMemory::init_external(CAI_Biting *M, TTime mem_time)
{
	monster		= M;
	time_memory = mem_time;
}

void CMonsterSoundMemory::HearSound(const SoundElem &s)
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

void CMonsterSoundMemory::HearSound(const CObject* who, int eType, const Fvector &Position, float power, TTime time)
{
	SoundElem s;
	s.SetConvert(who,eType,Position,power,time);
	s.CalcValue(time,monster->Position());

	HearSound(s);
} 

void CMonsterSoundMemory::GetSound(SoundElem &s, bool &bDangerous)
{
	VERIFY(!Sounds.empty());

	// возврат самого опасного
	s = GetSound();

	if (s.type > WEAPON_EMPTY_CLICKING) bDangerous = false;
	else bDangerous = true;
}

SoundElem &CMonsterSoundMemory::GetSound()
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
			const CEntityAlive *pE = smart_cast<const CEntityAlive*> (x.who);
			if (pE && !pE->g_Alive()) return true;
		}

		return false;
	}
};


void CMonsterSoundMemory::UpdateHearing()
{

	// удаление устаревших звуков
	xr_vector<SoundElem>::iterator I = remove_if(Sounds.begin(), Sounds.end(), pred_remove_nonactual_sounds(Level().timeServer() - time_memory));
	Sounds.erase(I,Sounds.end());

	// пересчитать value
	for (I=Sounds.begin(); I!=Sounds.end(); I++) I->CalcValue(Level().timeServer(), monster->Position());




	//#ifdef DEBUG
	//	CAI_Biting *pB = smart_cast<CAI_Biting*>(this);
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

bool CMonsterSoundMemory::is_loud_sound(float val)
{
	for (u32 i=0; i<Sounds.size(); i++) 
		if (Sounds[i].power > val) return true;

	return false;
}
