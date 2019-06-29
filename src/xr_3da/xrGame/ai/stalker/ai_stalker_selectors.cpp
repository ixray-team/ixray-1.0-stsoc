////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_selectors.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Selectors for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

CStalkerSelectorFreeHunting::CStalkerSelectorFreeHunting()
{
	Name = "selector_free_hunting"; 
}

float CStalkerSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	return(m_tEnemyPosition.distance_to(m_tCurrentPosition)*1000.f);
}

CStalkerSelectorReload::CStalkerSelectorReload()
{ 
	Name = "selector_reload"; 
}

float CStalkerSelectorReload::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	vfAddCoverFromEnemyCost();
	return(m_fResult);
}

CStalkerSelectorRetreat::CStalkerSelectorRetreat()
{ 
	Name = "selector_retreat"; 
}

float CStalkerSelectorRetreat::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
//	vfAddTravelCost();
//	CHECK_RESULT;
//	vfAddLightCost();
//	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	vfAddEnemyLookCost();
	vfAddDistanceToEnemyCost();
	return(m_fResult);
}

CStalkerSelectorAttack::CStalkerSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CStalkerSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	vfAddEnemyLookCost();
	CHECK_RESULT;
	vfAddMemberDanger();
	CHECK_RESULT;
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfComputeSurroundEnemy();
				vfAddCoverFromMemberCost();
				vfAddDeviationFromMemberViewCost();
			}
			vfAddSurroundEnemyCost();
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}