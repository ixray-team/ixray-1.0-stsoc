////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_predicates.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life predicates
////////////////////////////////////////////////////////////////////////////

#ifndef AI_ALIFE_PREDICATESH
#define AI_ALIFE_PREDICATESH
#pragma once
#ifndef _EDITOR
	#ifndef AI_COMPILER
		#include "ai_alife_registries.h"
	#endif
#endif

#include "xrserver_objects_alife_monsters.h"

class CArtefactOrderPredicate {
public:
	bool							operator()							(const ALife::SArtefactOrder &tArtefactOrder1, const ALife::SArtefactOrder &tArtefactOrder2) const
	{
		return						(tArtefactOrder1.m_dwPrice > tArtefactOrder2.m_dwPrice);
	};
};

class CArtefactPredicate {
public:
	bool							operator()							(const ALife::SArtefactTraderOrder tArtefactOrder1, const ALife::SArtefactTraderOrder tArtefactOrder2) const
	{
		return						(tArtefactOrder1.m_tpOrders[0].m_dwPrice > tArtefactOrder2.m_tpOrders[0].m_dwPrice);
	};
};

IC const ALife::_EVENT_ID			tfChooseEventKeyPredicate			(const CSE_ALifeEvent *T)
{
	return							(T->m_tEventID);
};

IC const ALife::_TASK_ID			tfChooseTaskKeyPredicate			(const CSE_ALifeTask *T)
{
	return							(T->m_tTaskID);
};

class CCompareTraderRanksPredicate {
public:
	bool							operator()							(const CSE_ALifeTrader *tpALifeTrader1, const CSE_ALifeTrader *tpALifeTrader2) const
	{
		return						(tpALifeTrader1->m_tRank < tpALifeTrader2->m_tRank);
	};
};

class CRemovePersonalEventPredicate {
private:
	ALife::EVENT_MAP				*m_tpMap;
public:
									CRemovePersonalEventPredicate		(ALife::EVENT_MAP &tpMap)
	{
		m_tpMap						= &tpMap;
	};

	IC bool							operator()							(const CSE_ALifePersonalEvent *tPersonalEvent)  const
	{
		return						(m_tpMap->find(tPersonalEvent->m_tEventID) == m_tpMap->end());
	};
};

class CSortItemPredicate {
public:
	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem1, const CSE_ALifeInventoryItem *tpALifeInventoryItem2)  const
	{
		return						(float(tpALifeInventoryItem1->m_dwCost)/1/**tpALifeInventoryItem1->m_fMass/**/ > float(tpALifeInventoryItem2->m_dwCost)/1/**tpALifeInventoryItem2->m_fMass/**/);
	};
};

class CSortItemByValuePredicate {
public:
	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem1, const CSE_ALifeInventoryItem *tpALifeInventoryItem2)  const
	{
		return						(tpALifeInventoryItem1->m_dwCost < tpALifeInventoryItem2->m_dwCost);
	};
};

class CRemoveAttachedItemsPredicate {
public:
	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem)
	{
		return						(tpALifeInventoryItem->bfAttached());
	};
};

class CSortItemVolumePredicate {
public:
	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem1, const CSE_ALifeInventoryItem *tpALifeInventoryItem2)  const
	{
		return						(float(tpALifeInventoryItem1->m_iGridWidth*tpALifeInventoryItem1->m_iGridHeight) > float(tpALifeInventoryItem2->m_iGridWidth*tpALifeInventoryItem2->m_iGridHeight));
	};
};

class CRemoveSlotAndCellItemsPredicate {
public:
	u32								m_dwMaxCount;
	u32								m_dwCurCount;
	ALife::WEAPON_P_VECTOR			*m_tpWeaponVector;

	CRemoveSlotAndCellItemsPredicate	(ALife::WEAPON_P_VECTOR *tpWeaponVector, u32 dwMaxCount) : m_dwMaxCount(dwMaxCount), m_dwCurCount(0)
	{
		m_tpWeaponVector			= tpWeaponVector;
	}

	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem)
	{
		const CSE_ALifeItemWeapon	*l_tpALifeItemWeapon = dynamic_cast<const CSE_ALifeItemWeapon*>(tpALifeInventoryItem);
		if (l_tpALifeItemWeapon && ((*m_tpWeaponVector)[l_tpALifeItemWeapon->m_dwSlot] == l_tpALifeItemWeapon))
			return					(true);
		else
			if ((m_dwCurCount < m_dwMaxCount) && (1 == tpALifeInventoryItem->m_iVolume)) {
				++m_dwCurCount;
				return				(true);
			}
		return						(false);
	}
};

#ifndef _EDITOR
#ifndef AI_COMPILER
class CRemoveNonAttachedItemsPredicate {
	CSE_ALifeObjectRegistry			*m_tpALifeObjectRegistry;
public:

	CRemoveNonAttachedItemsPredicate	(CSE_ALifeObjectRegistry *tpALifeObjectRegistry)
	{
		m_tpALifeObjectRegistry		= tpALifeObjectRegistry;
	}

	IC bool							operator()							(const ALife::_OBJECT_ID tObjectID)
	{
		return						(0xffff == m_tpALifeObjectRegistry->object(tObjectID)->ID_Parent);
	};
};

class CSortByOwnerPredicate {
	CSE_ALifeObjectRegistry			*m_tpALifeObjectRegistry;
public:
	ALife::_OBJECT_ID				m_tParentID;
	
									CSortByOwnerPredicate	(CSE_ALifeObjectRegistry *tpALifeObjectRegistry, ALife::_OBJECT_ID tParentID)
	{
		m_tpALifeObjectRegistry		= tpALifeObjectRegistry;
		m_tParentID					= tParentID;
	}

	IC bool							operator()							(const CSE_ALifeInventoryItem *tpALifeInventoryItem1, const CSE_ALifeInventoryItem *tpALifeInventoryItem2) const
	{
		if (tpALifeInventoryItem1->m_dwCost == tpALifeInventoryItem2->m_dwCost)
			if (tpALifeInventoryItem1->m_tPreviousParentID == m_tParentID)
				if (tpALifeInventoryItem2->m_tPreviousParentID == m_tParentID)
					return				(tpALifeInventoryItem1->ID < tpALifeInventoryItem2->ID);
				else
					return				(true);
			else
				if (tpALifeInventoryItem2->m_tPreviousParentID == m_tParentID)
					return				(false);
				else
					return				(tpALifeInventoryItem1->ID < tpALifeInventoryItem2->ID);
		else
			return						(tpALifeInventoryItem1->m_dwCost > tpALifeInventoryItem2->m_dwCost);
	}
};
#endif
#endif

#endif
