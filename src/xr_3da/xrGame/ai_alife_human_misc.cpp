////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_human_.cpp
//	Created 	: 24.07.2003
//  Modified 	: 24.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life humans simulation : miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "ai_alife_predicates.h"

bool CSE_ALifeHumanAbstract::bfHealthIsGood			()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfItemCanTreat			(CSE_ALifeInventoryItem *tpALifeInventoryItem)
{
	return(false);
}

void CSE_ALifeHumanAbstract::vfUseItem				(CSE_ALifeInventoryItem *tpALifeInventoryItem)
{
}

bool CSE_ALifeHumanAbstract::bfCanTreat				()
{
	xr_vector<u16>::iterator	I = children.begin();
	xr_vector<u16>::iterator	E = children.end();
	for ( ; I != E; ++I) {
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(m_tpALife->object(*I));
		R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item ihas a parent?!");
		if (bfItemCanTreat(l_tpALifeInventoryItem))
			vfUseItem			(l_tpALifeInventoryItem);
	}
	return						(bfHealthIsGood());
}

bool CSE_ALifeHumanAbstract::bfEnoughMoneyToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfEnoughTimeToTreat		()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfEnoughEquipmentToGo	()
{
	return(true);
}

bool CSE_ALifeHumanAbstract::bfDistanceToTraderIsDanger()
{
	return(false);
}

bool CSE_ALifeHumanAbstract::bfEnoughMoneyToEquip		()
{
	return(true);
}