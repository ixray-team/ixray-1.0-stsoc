////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.h
//	Created 	: 05.01.2002
//  Modified 	: 05.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_objects.h"
#include "game_graph.h"

class CSE_ALifeCreatureActor;
class CSE_ALifeHumanAbstract;

class CSE_ALifeObjectRegistry : public IPureALifeLSObject {
public:
	ALife::D_OBJECT_MAP				m_tObjectRegistry;			// список событий игры

									CSE_ALifeObjectRegistry		();
	virtual							~CSE_ALifeObjectRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual void					Load						(IReader &tFileStream);
			void					Add							(CSE_ALifeDynamicObject *tpALifeDynamicObject);
	IC		CSE_ALifeDynamicObject	*object						(ALife::_OBJECT_ID tObjectID, bool bNoAssert = false);
};

class CSE_ALifeEventRegistry : public IPureALifeLSObject {
public:
	ALife::_EVENT_ID				m_tEventID;					// идентификатор карты событий
	ALife::EVENT_MAP				m_tEventRegistry;			// список событий игры

									CSE_ALifeEventRegistry		();
	virtual							~CSE_ALifeEventRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeEvent	*tpEvent);
	IC		CSE_ALifeEvent			*event						(ALife::_EVENT_ID tEventID, bool bNoAssert = false);
};

class CSE_ALifeTaskRegistry : public IPureALifeLSObject {
public:
	ALife::_TASK_ID					m_tTaskID;					// идентификатор карты событий
	ALife::TASK_MAP					m_tTaskRegistry;			// список событий игры
	ALife::OBJECT_TASK_MAP			m_tTaskCrossMap;

									CSE_ALifeTaskRegistry		();
	virtual							~CSE_ALifeTaskRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
	virtual	void					Add							(CSE_ALifeTask *tpTask);
	virtual	void					Update						(CSE_ALifeTask *tpTask);
	IC		CSE_ALifeTask			*task						(ALife::_TASK_ID tTaskID, bool bNoAssert = false);
};

class CSE_ALifeGraphRegistry {
protected:
	typedef ALife::D_OBJECT_MAP::iterator _iterator;

public:
	CSE_ALifeCreatureActor			*m_tpActor;
	ALife::_LEVEL_ID				m_tCurrentLevelID;
	ALife::D_OBJECT_MAP				*m_tpCurrentLevel;
	ALife::GRAPH_POINT_VECTOR		m_tpGraphObjects;	// по точке графа получить все 
	ALife::GRAPH_VECTOR				m_tpTerrain[LOCATION_TYPE_COUNT][ALife::LOCATION_COUNT];	
	// массив списков: по идетнификатору 
	//	местности получить список точек 
	//  графа
	u64								m_cycle_count;
	_iterator						m_next_iterator;
	bool							m_first_update;
	u64								m_start_time;
	u64								m_max_process_time;

public:
									CSE_ALifeGraphRegistry		();
	virtual							~CSE_ALifeGraphRegistry		();
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAssignGraphPosition		(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract);
			void					vfAddObjectToCurrentLevel	(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfRemoveObjectFromCurrentLevel(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					vfAddObjectToGraphPoint		(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tNextGraphPointID,		bool bUpdateSwitchObjects = true);
			void					vfRemoveObjectFromGraphPoint(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tGraphID,				bool bUpdateSwitchObjects = true);
			void					vfChangeObjectGraphPoint	(CSE_ALifeDynamicObject *tpALifeDynamicObject,	ALife::_GRAPH_ID		tGraphPointID,			ALife::_GRAPH_ID tNextGraphPointID);
			// events
			void					vfRemoveEventFromGraphPoint	(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tGraphID);
			void					vfAddEventToGraphPoint		(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tNextGraphPointID);
			void					vfChangeEventGraphPoint		(CSE_ALifeEvent			*tpEvent,				ALife::_GRAPH_ID		tGraphPointID,			ALife::_GRAPH_ID tNextGraphPointID);
			void					vfAttachItem				(CSE_Abstract			&tAbstract,				CSE_ALifeInventoryItem	*tpALifeInventoryItem,	ALife::_GRAPH_ID tGraphID,			bool bALifeRequest = true);
			void					vfDetachItem				(CSE_Abstract			&tAbstract,				CSE_ALifeInventoryItem	*tpALifeInventoryItem,	ALife::_GRAPH_ID tGraphID,			bool bALifeRequest = true);
	virtual void					check_for_switch			(CSE_ALifeDynamicObject *tpALifeDynamicObject) = 0;
	IC		void					update_next					();
	IC		_iterator				&next						();
			void					update						();
	IC		bool					time_over					();
	IC		void					set_process_time			(u64 process_time);
};

class CSE_ALifeTraderRegistry {
public:
	ALife::TRADER_P_VECTOR			m_tpTraders;			// массив торговцев
	ALife::TRADER_SET_MAP			m_tpCrossTraders;

	virtual							~CSE_ALifeTraderRegistry	(){}
			void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			CSE_ALifeTrader			*trader_nearest(CSE_ALifeHumanAbstract *tpALifeHumanAbstract);
};

class CSE_ALifeScheduleRegistry {
protected:
	typedef ALife::SCHEDULE_P_PAIR_IT _iterator;

public:
	ALife::SCHEDULE_P_MAP			m_tpScheduledObjects;	// массив обновляемых объектов
	u64								m_cycle_count;
	_iterator						m_next_iterator;
	u64								m_start_time;
	u64								m_max_process_time;

public:
	IC								CSE_ALifeScheduleRegistry	();
	virtual							~CSE_ALifeScheduleRegistry	();
	IC		void					Init						();
			void					Update						(CSE_ALifeDynamicObject *tpALifeDynamicObject);
			void					add							(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects = true);
			void					remove						(CSE_ALifeDynamicObject *tpALifeDynamicObject, bool bUpdateSchedulableObjects = true);
	IC		void					update_next					();
	IC		_iterator				&next						();
			void					update						();
	IC		bool					time_over					();
	IC		void					set_process_time			(u64 process_time);
};

class CSE_ALifeSpawnRegistry : public CSE_ALifeSpawnHeader {
public:
	typedef CSE_ALifeSpawnHeader inherited;
	
	ALife::D_OBJECT_P_VECTOR		m_tpSpawnPoints;
	CGameGraph::LEVEL_POINT_VECTOR	m_tpArtefactSpawnPositions;
	xr_vector<bool>					m_baAliveSpawnObjects;
	ALife::ITEM_SET_MAP				m_tArtefactAnomalyMap;


									CSE_ALifeSpawnRegistry		();
	virtual							~CSE_ALifeSpawnRegistry		();
			void					Init						();
	virtual void					Load						(IReader &tFileStream);
};

class CSE_ALifeAnomalyRegistry : public IPureALifeLSObject {
public:
	ALife::ANOMALY_P_VECTOR_VECTOR	m_tpAnomalies;
	ALife::ANOMALY_P_VECTOR_SVECTOR	m_tpCrossAnomalies;

									CSE_ALifeAnomalyRegistry	();
	virtual							~CSE_ALifeAnomalyRegistry	();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};

class CSE_ALifeOrganizationRegistry : public IPureALifeLSObject {
public:
	ALife::ORGANIZATION_P_MAP		m_tOrganizationRegistry;
	ALife::DISCOVERY_P_MAP			m_tDiscoveryRegistry;
	ALife::LPSTR_BOOL_MAP			m_tArtefactRegistry;

									CSE_ALifeOrganizationRegistry();
	virtual							~CSE_ALifeOrganizationRegistry();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
};

class CSE_ALifeNewsRegistry : public IPureALifeLSObject {
public:
	typedef xr_map<ALife::_NEWS_ID,ALife::SGameNews*> NEWS_REGISTRY;

	struct CNewsPredicate {
		ALife::_NEWS_ID operator				()(const ALife::SGameNews *news) const
		{
			return					(news->m_news_id);
		}
	};

protected:
	NEWS_REGISTRY					m_news;
	ALife::_NEWS_ID					m_last_id;

protected:
	IC		void					add							(const ALife::SGameNews &news);
	IC		void					remove						(const ALife::_NEWS_ID &news_id);

public:
									CSE_ALifeNewsRegistry		();
	virtual							~CSE_ALifeNewsRegistry		();
	virtual	void					Save						(IWriter &tMemoryStream);
	virtual	void					Load						(IReader &tFileStream);
			void					clear						();
	IC		const NEWS_REGISTRY		&news						() const;
	IC		const ALife::SGameNews	*news						(const ALife::_NEWS_ID &news_id) const;
};

#include "ai_alife_registries_inline.h"