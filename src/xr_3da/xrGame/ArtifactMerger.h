///////////////////////////////////////////////////////////////
// ArtefactMerger.h
// ArtefactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item_object.h"
#include "Artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "xrmessages.h"
#include "script_export_space.h"

DEF_LIST (ARTIFACT_LIST, CArtefact*);

// void SetArtefactMergeFunctor(const luabind::functor<void> &artifactMergeFunctor);

class CArtefactMerger : public CInventoryItemObject {
	typedef	CInventoryItemObject	inherited;
public:
	CArtefactMerger(void);
	virtual ~CArtefactMerger(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();


	//функции для работы со списком артефактов
	void AddArtefact(CArtefact* pArtefact);
	void RemoveArtefact(CArtefact* pArtefact);
	void RemoveAllArtefacts();

	//выполняет операцию слияния над теми
	//артефктами, что находятся в списке
	//(вызов скриптованной процедуры)
	bool PerformMerge();

	//количество различных артефактов в списке
	template<typename A> int GetArtefactNum();
	int GetMercuryBallNum()		;
	int GetGraviArtefactNum()	;
	int GetBlackDropsNum()		;
	int GetNeedlesNum()			;

	//удаление определенного числа артефактов некоторого типа из списка
//	template<typename A> void DestroyArtefacts(int num_to_destroy);
//	void DestroyMercuryBall(int num_to_destroy)		;
//	void DestroyGraviArtefact(int num_to_destroy)	;
//	void DestroyBlackDrops(int num_to_destroy)		;
//	void DestroyNeedles(int num_to_destroy)			;


	//добавление определенного числа новых артефактов 
	void SpawnArtefact(const char* af_section);
	
	void SpawnMercuryBall()		{SpawnArtefact("af_mercury_ball");}
	void SpawnGraviArtefact()	{SpawnArtefact("af_magnet");}
	void SpawnBlackDrops()		{SpawnArtefact("af_black_drops");}
	void SpawnNeedles()			{SpawnArtefact("af_needles");}


	//список артефактов только что удаленных
	ARTIFACT_LIST m_ArtefactDeletedList;
	//только что появившихся
	ARTIFACT_LIST m_ArtefactNewList;


protected:
	//список артефактов помещенных в устройство
	ARTIFACT_LIST m_ArtefactList;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CArtefactMerger)
#undef script_type_list
#define script_type_list save_type_list(CArtefactMerger)
