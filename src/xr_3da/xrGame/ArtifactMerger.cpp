///////////////////////////////////////////////////////////////
// ArtefactMerger.cpp
// ArtefactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtifactMerger.h"
#include "PhysicsShell.h"
//#include "script_process.h"
#include "xrserver_objects_alife.h"
#include "level.h"
#include "script_space.h"
#include "ai_object_location.h"

//глобальный указатель на функтор, который
//запускает функцию сочетания артефактов из 
//скиптов
luabind::functor<void> *g_ArtefactMergeFunctor = NULL;

CArtefactMerger::CArtefactMerger(void) 
{
	m_ArtefactList.clear();
}

CArtefactMerger::~CArtefactMerger(void) 
{
	m_ArtefactList.clear();
	xr_delete			(g_ArtefactMergeFunctor);
}

void SetArtefactMergeFunctor(const luabind::functor<void>& artifactMergeFunctor)
{
	g_ArtefactMergeFunctor = xr_new<luabind::functor<void> >(artifactMergeFunctor);
}

BOOL CArtefactMerger::net_Spawn(CSE_Abstract* DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CArtefactMerger::Load(LPCSTR section) 
{
	//загрузить все возможные комбинации артефактов для получения
	//новых

	inherited::Load(section);
}

void CArtefactMerger::net_Destroy() 
{
	inherited::net_Destroy();
}

void CArtefactMerger::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtefactMerger::UpdateCL() 
{
	inherited::UpdateCL();
}


void CArtefactMerger::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CArtefactMerger::OnH_B_Independent(bool just_before_destroy) 
{
	inherited::OnH_B_Independent(just_before_destroy);
}

void CArtefactMerger::renderable_Render() 
{
	inherited::renderable_Render();
}
////////////////////////////////////////////
//функции для работы со списком артефактов
////////////////////////////////////////////

void CArtefactMerger::AddArtefact(CArtefact* pArtefact)
{
	m_ArtefactList.push_back(pArtefact);
}
void CArtefactMerger::RemoveArtefact(CArtefact* pArtefact)
{
	m_ArtefactList.remove(pArtefact);
}
void CArtefactMerger::RemoveAllArtefacts()
{
	m_ArtefactList.clear();
}

//выполняет операцию слияния над теми
//артефктами, что находятся в списке
//(вызов скриптованной процедуры)
bool CArtefactMerger::PerformMerge()
{
	R_ASSERT2(g_ArtefactMergeFunctor->is_valid(), "The function that perform artifact merge doesn't set yet");
	
	m_ArtefactDeletedList.clear();
	m_ArtefactNewList.clear();
	
	(*g_ArtefactMergeFunctor)(this);
	return false;
}

void CArtefactMerger::SpawnArtefact(const char* af_section)
{
	CSE_Abstract*		D	= F_entity_Create(af_section);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
								 smart_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	l_tpALifeDynamicObject->m_tNodeID = ai_location().level_vertex_id();
		
	// Fill
	D->s_name			= af_section;
	D->set_name_replace	("");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
}
