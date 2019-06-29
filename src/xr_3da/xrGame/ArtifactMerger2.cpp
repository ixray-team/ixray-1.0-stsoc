///////////////////////////////////////////////////////////////
// ArtefactMerger.cpp
// ArtefactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtifactMerger.h"
#include "PhysicsShell.h"
#include "net_utils.h"

template<typename A> 
int CArtefactMerger::GetArtefactNum()
{
	int artifact_count = 0;
	for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
							m_ArtefactList.end() != it;
							++it)
						if(smart_cast<A*>(*it)) ++artifact_count;
	return artifact_count;
};

int CArtefactMerger::GetMercuryBallNum()	{return GetArtefactNum<CMercuryBall>();}
int CArtefactMerger::GetGraviArtefactNum()	{return GetArtefactNum<CGraviArtefact>();}
int CArtefactMerger::GetBlackDropsNum()		{return GetArtefactNum<CBlackDrops>();}
int CArtefactMerger::GetNeedlesNum()		{return GetArtefactNum<CNeedles>();}

/**
template<typename A> void CArtefactMerger::DestroyArtefacts(int num_to_destroy)
{
	int artifact_count=0;
//		for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
//							 (m_ArtefactList.end() != it) &&
//							 (artifact_count<num_to_destroy);)

	ARTIFACT_LIST_it it = m_ArtefactList.begin();
	do
	{
		if(smart_cast<A*>(*it)) 
		{
			++artifact_count;
			//удалить объект артефакта
			PIItem pIItem = *it;
			CGameObject* pObject = static_cast<CGameObject*>(pIItem->object().H_Parent());
			R_ASSERT(pObject);
			pIItem->Drop();
			pIItem->object().DestroyObject	();
//			NET_Packet P;
//			pObject->u_EventGen(P,GE_DESTROY,pIItem->object().ID());
//			P.w_u16(u16(pIItem->object().ID()));
//			pObject->u_EventSend(P);
		
			//удалить артефакт
			m_ArtefactDeletedList.push_back(*it);
			RemoveArtefact(*it);
		
			//начать поиск сначала списка
			it = m_ArtefactList.begin();
		}
		else
		{
			++it;
		}
	} while((m_ArtefactList.end() != it) &&
		  	(artifact_count<num_to_destroy));
}


void CArtefactMerger::DestroyMercuryBall(int num_to_destroy)		{DestroyArtefacts<CMercuryBall>(num_to_destroy);}
void CArtefactMerger::DestroyGraviArtefact(int num_to_destroy)	{DestroyArtefacts<CGraviArtefact>(num_to_destroy);}
void CArtefactMerger::DestroyBlackDrops(int num_to_destroy)		{DestroyArtefacts<CBlackDrops>(num_to_destroy);}
void CArtefactMerger::DestroyNeedles(int num_to_destroy)			{DestroyArtefacts<CNeedles>(num_to_destroy);}
/**/