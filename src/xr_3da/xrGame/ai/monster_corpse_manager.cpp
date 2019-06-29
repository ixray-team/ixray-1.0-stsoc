#include "stdafx.h"
#include "monster_corpse_manager.h"
#include "biting/ai_biting.h"

CMonsterCorpseManager::CMonsterCorpseManager()
{
	monster		= 0;
	corpse		= 0;
	forced		= false;
}

CMonsterCorpseManager::~CMonsterCorpseManager()
{

}
void CMonsterCorpseManager::init_external(CAI_Biting *M)
{
	monster = M;
}

void CMonsterCorpseManager::update()
{
	if (forced) {
		if (corpse->m_fFood < 1) {
			corpse = 0;
			return;
		}
	} else {
		corpse = monster->CorpseMemory.get_corpse();
		
		if (corpse) {
			SMonsterCorpse corpse_info = monster->CorpseMemory.get_corpse_info();
			position		= corpse_info.position;
			vertex			= corpse_info.vertex;
			time_last_seen	= corpse_info.time;
		}
	}
}

void CMonsterCorpseManager::force_corpse(const CEntityAlive *corpse)
{
	this->corpse	= corpse;
	position		= corpse->Position();
	vertex			= corpse->level_vertex_id();
	time_last_seen	= Level().timeServer();

	forced			= true;
}

void CMonsterCorpseManager::unforce_corpse()
{
	corpse = monster->CorpseMemory.get_corpse();

	if (corpse) {
		SMonsterCorpse corpse_info = monster->CorpseMemory.get_corpse_info();
		position		= corpse_info.position;
		vertex			= corpse_info.vertex;
		time_last_seen	= corpse_info.time;
	}

	forced = false;
}

void CMonsterCorpseManager::reinit()
{
	corpse			= 0;
	forced			= false;
	time_last_seen	= 0;
}

