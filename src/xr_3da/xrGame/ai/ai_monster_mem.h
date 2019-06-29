#pragma once

#include "ai_monster_defs.h"
#include "../entity.h"
#include "../level_graph.h"

class CCustomMonster;

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMORY MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

#define TIME_TO_RESELECT_ENEMY			3000
#define TIME_TO_UPDATE_IGNORE_OBJECTS	3000

typedef enum {
	WEAPON_SHOOTING = 0,
	MONSTER_ATTACKING,
	WEAPON_BULLET_RICOCHET,
	WEAPON_RECHARGING,
	
	WEAPON_TAKING,
	WEAPON_HIDING,			
	WEAPON_CHANGING,
	WEAPON_EMPTY_CLICKING,

	MONSTER_DYING,	
	MONSTER_INJURING,
	MONSTER_WALKING,			
	MONSTER_JUMPING,			
	MONSTER_FALLING,			
	MONSTER_TALKING,			

	DOOR_OPENING,		
	DOOR_CLOSING,		
	OBJECT_BREAKING,	
	OBJECT_FALLING,
	NONE_DANGEROUS_SOUND
} TSoundDangerValue;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSoundMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FACTOR_SOUND_TYPE	int(8)
#define FACTOR_DISTANCE		int(1)
#define FACTOR_DELTA_TIME	int(2)
#define FACTOR_SOUND_POWER	int(50)


typedef struct tagSoundElement
{
	const CObject		*who;
	TSoundDangerValue	type;
	Fvector				position;		// позиция звука, не объекта, издавшего звук
	float				power;
	TTime				time;			// время обнаружения звука

	int					value;			// оценочное значение данного звука		

	tagSoundElement() {  
		who = 0; type = NONE_DANGEROUS_SOUND; position.set(0,0,0); power = 0.f; time = 0; value = 0;
	}
	bool operator < (const tagSoundElement &s) const  { 
		return (value < s.value);
	}
	IC void SetConvert(const CObject* who, int eType, const Fvector &position, float power, TTime time) {
		this->who = who; type = ConvertSoundType((ESoundTypes)eType); this->position = position; this->power = power; this->time = time;
	}
	TSoundDangerValue ConvertSoundType(ESoundTypes stype);
	
	void CalcValue(TTime cur_time, Fvector cur_pos) {
		value = FACTOR_SOUND_TYPE * u32(NONE_DANGEROUS_SOUND - WEAPON_SHOOTING) - iFloor(FACTOR_DISTANCE * cur_pos.distance_to(position)) - FACTOR_DELTA_TIME * iFloor(float((cur_time - time) / 1000)) + FACTOR_SOUND_POWER * iFloor(power);
	}

} SoundElem;


class CSoundMemory
{
	TTime					timeMemory;				// время хранения звуков
	xr_vector<SoundElem>	Sounds;

	TTime					timeCurrent;			// текущее время

	CCustomMonster			*pMonster;

	friend class CMonsterMemory;

public:
				CSoundMemory			() {}
	virtual		~CSoundMemory			() {}

		void	HearSound				(const SoundElem &s);
		void	HearSound				(const CObject* who, int eType, const Fvector &Position, float power, TTime time);
	IC	bool	IsRememberSound			() {return (!Sounds.empty());}		
		void	GetSound				(SoundElem &s, bool &bDangerous);	// возвращает самый опасный звук
	SoundElem	&GetSound				();

protected:
		void	Init					(TTime mem_time);
		void	Deinit					();

		void	UpdateHearing			(TTime dt);
};


//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

typedef struct tagVisionElem
{
	const CEntity				*obj;
	Fvector						position;
	const CLevelGraph::CVertex	*vertex;
	u32							node_id;
	TTime						time;

	tagVisionElem() {obj = 0;}

	IC void Set(const CEntity *pE, TTime t) {
		obj = pE; 	position = pE->Position(); 	vertex = pE->level_vertex();  node_id = pE->level_vertex_id(); time = t; 
	}
	void operator = (const tagVisionElem &ve) {
		obj = ve.obj; 	position = ve.position; vertex = ve.vertex;  node_id = ve.node_id; time = ve.time; 
	}

	bool operator == (const tagVisionElem &ve) {
		return (ve.obj == obj);
	}

} VisionElem;

/////////////////////////////////////////////////////////////////////////////////////////////
// CVisionMemory class
class CVisionMemory
{
	TTime					timeMemoryDefault;
	TTime					timeMemory;				// время хранения визуальных объектов
	TTime					timeCurrent;			// текущее время
	
	typedef xr_vector			<VisionElem> VECTOR_VE;
	typedef VECTOR_VE::iterator	ITERATOR_VE;	

	VECTOR_VE				Objects;
	VECTOR_VE				Enemies;

	VisionElem				Selected;
	enum EObjectType {ENEMY, OBJECT};

	CCustomMonster			*pMonster;
	
	VECTOR_VE				IgnoreObjects;
	TTime					timeLastUpdateIgnoreObjects;

	VisionElem				ForcedEnemy;
	VisionElem				ForcedCorpse;
	
	friend class CMonsterMemory;
		
public:
					CVisionMemory	();
		virtual		~CVisionMemory	(){}


	IC	bool		GetEnemy		(VisionElem &ve) {
		if (ForcedEnemy.obj) { ve = ForcedEnemy; return true; }
		if (IsEnemy())  return Get(ve); else return false;
	} 	
	
	IC	bool		GetCorpse		(VisionElem &ve) {
		if (ForcedCorpse.obj) { ve = ForcedCorpse; return true;}
		if (IsObject()) return Get(ve); else return false;
	}
	
	IC	void		AddCorpse		(VisionElem &ve) {AddObject(ve);}

	IC	void		SetMemoryTime	(TTime t) {timeMemory = t;}
	IC	void		SetMemoryTimeDef() {timeMemory = timeMemoryDefault;}

		void		AddIgnoreObject	(CEntity *pObj);
		
	IC	u8			GetEnemyNumber	() {return u8(Enemies.size());}

	IC	void		SetForcedEnemy	(CEntity *pObj) {ForcedEnemy.Set	(pObj, timeCurrent);}
	IC	void		SetForcedCorpse	(CEntity *pObj) {ForcedCorpse.Set	(pObj, timeCurrent);}

	IC	void		ClearForcedEntities() {ForcedEnemy.obj = ForcedCorpse.obj = 0;}

protected:
		void		Init			(TTime mem_time);
		void		Deinit			();

		// Заполняет массивы Objects и Enemies и Selected
		void		UpdateVision	(TTime dt);
	
private:
	IC	bool		IsEnemy			() {return (!Enemies.empty());}	 
	IC	bool		IsObject		() {return (!Objects.empty());}	 

		void		AddObject		(VisionElem &ve);
		void		AddEnemy		(VisionElem &ve);
	
		bool		Get				(VisionElem &ve);

		void		SelectEnemy		();
		void		SelectCorpse	();

		VisionElem	&GetNearestObject(EObjectType obj_type = ENEMY);
		
		
		void		RemoveNonactualElems();
};

//---------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMonsterMemory class
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------

typedef struct tagDangerousEnemies {
	const CObject	*who;
	TTime			time_till_remember;	

	bool operator == (const tagDangerousEnemies &obj) {return (who == obj.who);}
	bool operator == (const CObject *obj) {return (who == obj);}
} SDangerousEnemies;

// предикат удаления 'старых' объектов
struct predicate_remove_old_dangerous_enemies {
	TTime new_time;
	predicate_remove_old_dangerous_enemies(TTime time) { new_time = time; }
	bool operator() (const SDangerousEnemies &x) { return (x.time_till_remember < new_time); }
};


class CMonsterMemory : public CSoundMemory, public CVisionMemory {
	
	DEFINE_VECTOR(SDangerousEnemies, DANGER_ENEMIES_VEC, DANGER_ENEMIES_VEC_IT);

	DANGER_ENEMIES_VEC	danger_enemies;
public:
	void	InitMemory					(TTime sound_mem, TTime vision_mem) {
		CSoundMemory::Init(sound_mem);
		CVisionMemory::Init(vision_mem);
	}
	void	DeinitMemory				() {
		CSoundMemory::Deinit();
		CVisionMemory::Deinit();
	}

	void	UpdateMemory				();

	//-------------------------------------------------------

	void	AddDangerousEnemy			(const CObject *pO, TTime ttr);
	bool	IsDangerousEnemy			(const CObject *pO);

private:

	void	RemoveOldDangerousEnemies	();
};

