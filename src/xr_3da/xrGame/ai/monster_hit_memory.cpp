#include "stdafx.h"
#include "monster_hit_memory.h"
#include "biting/ai_biting.h"

CMonsterHitMemory::CMonsterHitMemory()
{
	monster			= 0;
	time_memory		= 10000; 
}

CMonsterHitMemory::~CMonsterHitMemory()
{
}

void CMonsterHitMemory::init_external(CAI_Biting *M, TTime mem_time) 
{
	monster = M; 
	time_memory = mem_time;
}


void CMonsterHitMemory::update()
{
	// удалить устаревшие hits
	remove_non_actual();
}

bool CMonsterHitMemory::is_hit(CObject *pO)
{
	MONSTER_HIT_VECTOR_IT it = find(m_hits.begin(), m_hits.end(), pO);
	
	return (it != m_hits.end());
}

void CMonsterHitMemory::add_hit(CObject *who, EHitSide side)
{
	SMonsterHit			new_hit_info;
	new_hit_info.object		= who;
	new_hit_info.time		= Level().timeServer();
	new_hit_info.side		= side;
	new_hit_info.position	= monster->Position();

	MONSTER_HIT_VECTOR_IT it = find(m_hits.begin(), m_hits.end(), who);

	if (it == m_hits.end()) m_hits.push_back(new_hit_info);
	else *it = new_hit_info;
}

struct predicate_old_hit {
	TTime cur_time;
	TTime mem_time;

	predicate_old_hit(TTime mem_time, TTime cur_time){
		this->cur_time = cur_time;
		this->mem_time = mem_time;
	}

	IC bool	operator() (const SMonsterHit &hit_info) {
		return ((mem_time + hit_info.time) < cur_time);
	}

};

void CMonsterHitMemory::remove_non_actual() 
{
	MONSTER_HIT_VECTOR_IT it = remove_if(m_hits.begin(), m_hits.end(), predicate_old_hit(time_memory, Level().timeServer()));
	m_hits.erase(it, m_hits.end());
}

Fvector CMonsterHitMemory::get_last_hit_dir()
{
	Fvector			dir = monster->Direction();

	// найти последний по времени хит
	SMonsterHit		last_hit;
	last_hit.time	= 0;
	last_hit.side	= eSideFront;

	for (u32 i = 0; i < m_hits.size(); i++) {
		if (m_hits[i].time > last_hit.time)	last_hit = m_hits[i];
	}

	// если есть хит, вычислить направление
	if (last_hit.time != 0) {

		float h,p;
		dir.getHP(h,p);
		
		switch (last_hit.side) {
			case eSideBack:
				h += PI;
				break;
			case eSideLeft:
				h += PI_DIV_2;
				break;
			case eSideRight:
				h -= PI_DIV_2;
				break;
		}

		h = angle_normalize(h);
		dir.setHP(h,p);
		dir.normalize();
	}

	return dir;
}

TTime CMonsterHitMemory::get_last_hit_time()
{
	SMonsterHit		last_hit;
	last_hit.time	= 0;

	for (u32 i = 0; i < m_hits.size(); i++) {
		if (m_hits[i].time > last_hit.time)	last_hit = m_hits[i];
	}

	return last_hit.time;
}

CObject	*CMonsterHitMemory::get_last_hit_object()
{
	SMonsterHit		last_hit;
	last_hit.object	= 0;
	last_hit.time	= 0;

	for (u32 i = 0; i < m_hits.size(); i++) {
		if (m_hits[i].time > last_hit.time)	last_hit = m_hits[i];
	}

	return last_hit.object;
}

Fvector CMonsterHitMemory::get_last_hit_position()
{
	SMonsterHit		last_hit;
	last_hit.time	= 0;
	last_hit.position.set(0.f,0.f,0.f);

	for (u32 i = 0; i < m_hits.size(); i++) {
		if (m_hits[i].time > last_hit.time)	last_hit = m_hits[i];
	}

	return last_hit.position;
}

