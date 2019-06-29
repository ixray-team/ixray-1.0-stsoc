////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.h
//	Created 	: 25.12.2002
//  Modified 	: 04.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_registries.h"
#include "xrServer_Objects_ALife.h"

/// #pragma todo("Dima to Dima : Be attentive with this speed optimization - it doesn't suit to the OOP paradigm!")
#define FAST_OWNERSHIP

class CSE_ALifeCreatureAbstract;
class CSE_ALifeAnomalousZone;
class CSE_ALifeTraderAbstract;
class CSE_ALifeGroupAbstract;

class CSE_ALifeSimulator : 
	public CSE_ALifeHeader, 
	public CSE_ALifeGameTime,
	public CSE_ALifeSpawnRegistry, 
	public CSE_ALifeObjectRegistry, 
	public CSE_ALifeEventRegistry, 
	public CSE_ALifeTaskRegistry, 
	public CSE_ALifeGraphRegistry,
	public CSE_ALifeTraderRegistry,
	public CSE_ALifeScheduleRegistry,
	public CSE_ALifeAnomalyRegistry,
	public CSE_ALifeOrganizationRegistry,
	public CSE_ALifeNewsRegistry,
	public ISheduled,
	public CRandom 
{
	u64								m_max_process_time;
	u64								m_start_time;
	float							m_update_monster_factor;
	xrServer						*m_tpServer;
	bool							m_bActorEnabled;
	string256						m_caSaveName;
	u32								m_dwMaxCombatIterationCount;
	
	// temporary buffer for purchased by the particular trader artefacts
	ALife::ITEM_COUNT_MAP			m_tpTraderItems;
	ALife::ORGANIZATION_ORDER_MAP	m_tpSoldArtefacts;
	
	// temporary buffers for combats
	ALife::SCHEDULE_P_VECTOR		m_tpaCombatGroups[2];
	CSE_ALifeSchedulable			*m_tpaCombatObjects[2];
	ALife::D_OBJECT_MAP				m_tpGraphPointObjects;

	// temporary buffers for trading
	ALife::ITEM_P_VECTOR			m_tpItems1;
	ALife::ITEM_P_VECTOR			m_tpItems2;
	ALife::OBJECT_VECTOR			m_tpBlockedItems1;
	ALife::OBJECT_VECTOR			m_tpBlockedItems2;

	enum {
		MAX_STACK_DEPTH				= u32(128),
		SUM_COUNT_THRESHOLD			= u32(30),
	};

	ALife::ITEM_P_VECTOR			m_tpTrader1;
	ALife::ITEM_P_VECTOR			m_tpTrader2;
	ALife::INT_VECTOR				m_tpSums1;
	ALife::INT_VECTOR				m_tpSums2;

	ALife::SSumStackCell			m_tpStack1[MAX_STACK_DEPTH];
	ALife::SSumStackCell			m_tpStack2[MAX_STACK_DEPTH];

	DWORD_VECTOR					m_tpTempPath;

	bool							m_changing_level;

	// common
			void					vfUpdateDynamicData			(bool						bReserveID = true);
			void					vfUpdateDynamicData			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfCreateNewTask				(CSE_ALifeTrader			*tpTrader);
			void					vfCreateObjectFromSpawnPoint(CSE_ALifeDynamicObject		*&tpALifDynamicObject,		CSE_ALifeDynamicObject	*tpALifeDynamicObject,			ALife::_SPAWN_ID tSpawnID);
			void					vfPrintTime					(LPCSTR						S,							ALife::_TIME_ID			tTimeID);
			void					vfAssignDeathPosition		(CSE_ALifeCreatureAbstract	*tpALifeCreatureAbstract,	ALife::_GRAPH_ID		tGraphID,						CSE_ALifeSchedulable *tpALifeSchedulable = 0);
			void					vfAssignArtefactPosition	(CSE_ALifeAnomalousZone		*tpALifeAnomalousZone,		CSE_ALifeDynamicObject	*tpALifeDynamicObject);
	// surge
			void					vfPerformSurge				();
			void					vfGenerateAnomalousZones	();
			void					vfGenerateAnomalyMap		();
			void					vfSellArtefacts				(CSE_ALifeTrader			&tTrader);
			void					vfUpdateArtefactOrders		(CSE_ALifeTrader			&tTrader);
			void					vfBuySupplies				(CSE_ALifeTrader			&tTrader);
			void					vfGiveMilitariesBribe		(CSE_ALifeTrader			&tTrader);
			void					vfUpdateOrganizations		();
			void					vfKillCreatures				();
			void					vfBalanceCreatures			();
			void					vfUpdateTasks				();
			void					vfAssignStalkerCustomers	();
	// switch online/offline routines
			bool					bfValidatePosition			(CSE_ALifeDynamicObject		*I);
			void					vfRemoveOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject,		bool					bAddToRegistries = true);
			void					vfCreateOnlineObject		(CSE_ALifeDynamicObject		*tpALifeDynamicObject,		bool					bRemoveFromRegistries = true);
			void					vfSwitchObjectOnline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSwitchObjectOffline		(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfFurlObjectOffline			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
	// interaction routines
			void					vfInitAI_ALifeMembers		();
			void					vfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable,		ALife::_GRAPH_ID		tGraphID);
			bool					bfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2,			int				&iCombatGroupIndex,			bool					&bMutualDetection);
			bool					bfCheckObjectDetection		(CSE_ALifeSchedulable		*tpALifeSchedulable1,		CSE_ALifeSchedulable	*tpALifeSchedulable2);

			void					vfPerformAttackAction		(int						iCombatGroupIndex);
			bool					bfCheckIfRetreated			(int						iCombatGroupIndex);
			
			void					vfFillCombatGroup			(CSE_ALifeSchedulable		*tpALifeSchedulable,		int						iGroupIndex);
			void					vfFinishCombat				(ALife::ECombatResult		tCombatResult);
	// trading routines
#ifdef DEBUG
			void					vfPrintItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::ITEM_P_VECTOR			&tpItemVector);
			void					vfPrintItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract);
#endif
			u32						dwfComputeItemCost			(ALife::ITEM_P_VECTOR		&tpItemVector);
			void					vfRunFunctionByIndex		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::OBJECT_VECTOR			&tpBlockedItems,				ALife::ITEM_P_VECTOR	&tpItems,					int						i,						int						&j);
			void					vfAssignItemParents			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		int								iItemCount);
			void					vfAppendBlockedItems		(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::OBJECT_VECTOR			&tpObjectVector1,				ALife::OBJECT_VECTOR	&tpObjectVector2,			int						l_iItemCount1);
			void					vfAttachOwnerItems			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::ITEM_P_VECTOR			&tpItemVector,					ALife::ITEM_P_VECTOR	&tpOwnItems);
			void					vfRestoreItems				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::ITEM_P_VECTOR			&tpItemVector);
#ifdef FAST_OWNERSHIP
			void					vfAttachGatheredItems		(CSE_ALifeTraderAbstract	*tpALifeTraderAbstract1,	CSE_ALifeTraderAbstract			*tpALifeTraderAbstract2,		ALife::OBJECT_VECTOR	&tpObjectVector);
#else
			void					vfAttachGatheredItems		(CSE_ALifeTraderAbstract	*tpALifeTraderAbstract1,	ALife::OBJECT_VECTOR			&tpObjectVector);
#endif
			int						ifComputeBalance			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		ALife::ITEM_P_VECTOR			&tpItemVector);
			void					vfFillTraderVector			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		int								iItemCount,						ALife::ITEM_P_VECTOR	&tpItemVector);
			void					vfGenerateSums				(ALife::ITEM_P_VECTOR		&tpTrader,					ALife::INT_VECTOR				&tpSums);
			bool					bfGetItemIndexes			(ALife::ITEM_P_VECTOR		&tpTrader,					int								iSum1,							ALife::INT_VECTOR		&tpIndexes,					ALife::SSumStackCell			*tpStack,				int						iStartI,					int				iStackPointer);
			bool					bfCheckForInventoryCapacity	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ALife::ITEM_P_VECTOR			&tpTrader1,						ALife::INT_VECTOR		&tpIndexes1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,	ALife::ITEM_P_VECTOR			&tpTrader2,					ALife::INT_VECTOR		&tpIndexes2);
			bool					bfCheckForInventoryCapacity	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ALife::ITEM_P_VECTOR			&tpTrader1,						int				iSum1,						int						iMoney1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,				ALife::ITEM_P_VECTOR	&tpTrader2,		int			iSum2,		int iMoney2, int iBalance);
			bool					bfCheckForTrade				(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		ALife::ITEM_P_VECTOR			&tpTrader1,						ALife::INT_VECTOR		&tpSums1,					int						iMoney1,				CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,		ALife::ITEM_P_VECTOR	&tpTrader2,		ALife::INT_VECTOR	&tpSums2,	int iMoney2, int iBalance);
			bool					bfCheckIfCanNullTradersBalance(CSE_ALifeHumanAbstract	*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2,			int				iItemCount1,				int						iItemCount2,			int						iBalance);
			void					vfPerformTrading			(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract1,		CSE_ALifeHumanAbstract	*tpALifeHumanAbstract2);
			void					vfPerformCommunication		();
public:
	// members
	bool							m_bLoaded;
	LPSTR							*m_cppServerOptions;
	float							m_fSwitchDistance;
	float							m_fSwitchFactor;
	float							m_fOnlineDistance;
	float							m_fOfflineDistance;
	u32								m_dwInventorySlotCount;
	ALife::WEAPON_P_VECTOR			m_tpWeaponVector;	
	BOOL_VECTOR						m_baMarks;
	ALife::ECombatType				m_tCombatType;
	
	// temporary buffers for combats
	ALife::ITEM_P_VECTOR			m_tpItemVector;
	ALife::ITEM_P_VECTOR			m_tpTempItemBuffer;
	// constructors/destructors
									CSE_ALifeSimulator			(xrServer					*tpServer);
	virtual							~CSE_ALifeSimulator			();
	// scheduler
	virtual float					shedule_Scale				();
	virtual void					shedule_Update				(u32 dt);	
	virtual BOOL					shedule_Ready				();
	virtual ref_str					cName						();
	// game interface
	virtual void					Load						(LPCSTR						caSaveName);
			void					Save						(LPCSTR						caSaveName);
			void					Save						();
			bool					change_level				(NET_Packet &net_packet);
			void					vfNewGame					(LPCSTR						caSaveName);
			void					vfReleaseObject				(CSE_Abstract				*tpSE_Abstract,				bool					bALifeRequest = true);
			void					vfCreateItem				(CSE_ALifeObject			*object);
	// miscellanious
			void					vfCheckForInteraction		(CSE_ALifeSchedulable		*tpALifeSchedulable);
			void					vfAppendItemVector			(ALife::OBJECT_VECTOR		&tObjectVector,				ALife::ITEM_P_VECTOR	&tItemList);
			ALife::ECombatAction	tfChooseCombatAction		(int						iCombatGroupIndex);
			ALife::ERelationType	tfGetRelationType			(CSE_ALifeMonsterAbstract	*tpALifeMonsterAbstract1,	CSE_ALifeMonsterAbstract*tpALifeMonsterAbstract2);
			CSE_Abstract			*tpfCreateGroupMember		(CSE_ALifeGroupAbstract		*tpALifeGroupAbstract,		CSE_ALifeDynamicObject	*j);
	// trading routines
			void					vfCommunicateWithCustomer	(CSE_ALifeHumanAbstract		*tpALifeHumanAbstract,		CSE_ALifeTrader			*tpALifeTrader);
			float					distance					(const DWORD_VECTOR &path) const;
			CSE_Abstract			*spawn_item					(LPCSTR section,			const Fvector &position,	u32 level_vertex_id,	ALife::_GRAPH_ID game_vertex_id,	u16 parent_id);
	virtual void					check_for_switch			(CSE_ALifeDynamicObject		*tpALifeDynamicObject);
			void					vfSetProcessTime			(int						iMicroSeconds);
	// console commands support
#ifdef ALIFE_SUPPORT_CONSOLE_COMMANDS
			void					vfListObjects				();
			void					vfListEvents				();
			void					vfListTasks					();
			void					vfListTerrain				();
			void					vfListSpawnPoints			();
			void					vfObjectInfo				(ALife::_OBJECT_ID			tObjectID);
			void					vfEventInfo					(ALife::_EVENT_ID			tEventID);
			void					vfTaskInfo					(ALife::_TASK_ID			tTaskID);
			void					vfSpawnPointInfo			(ALife::_SPAWN_ID			tSpawnID);
			void					vfGraphVertexInfo			(ALife::_GRAPH_ID			tGraphID);
			void					vfSetSwitchDistance			(float						fNewDistance);
			float					ffGetSwitchDistance			();
			void					vfSetSwitchFactor			(float						fSwitchFactor);
			void					vfSetSwitchDelay			(int						iMilliSeconds);
			void					vfSetScheduleMin			(int						iMilliSeconds);
			void					vfSetScheduleMax			(int						iMilliSeconds);
#endif
};