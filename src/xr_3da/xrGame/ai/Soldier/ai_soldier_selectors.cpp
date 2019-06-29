////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_selectors.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI selectors for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\entity.h"
#include "..\\..\\CustomMonster.h"
#include "ai_soldier_selectors.h"

CSoldierSelectorAttack::CSoldierSelectorAttack()
{ 
	Name = "selector_attack"; 
}

float CSoldierSelectorAttack::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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

CSoldierSelectorDefend::CSoldierSelectorDefend()
{ 
	Name = "selector_defend"; 
}

float CSoldierSelectorDefend::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	CHECK_RESULT;
	//vfAddLightCost();
	//CHECK_RESULT;
	//vfAddTotalCoverCost();
	//CHECK_RESULT;
	//vfAddDistanceToEnemyCost();
	//CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
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
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorFindEnemy::CSoldierSelectorFindEnemy()
{ 
	Name = "selector_find_enemy"; 
}

float CSoldierSelectorFindEnemy::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	vfAddDistanceToLastPositionCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorFollowLeader::CSoldierSelectorFollowLeader()
{ 
	Name = "selector_follow_leader"; 
}

float CSoldierSelectorFollowLeader::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfAddCoverFromMemberCost();
			}
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorFreeHunting::CSoldierSelectorFreeHunting()
{ 
	Name = "selector_free_hunting"; 
}

float CSoldierSelectorFreeHunting::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfAddCoverFromMemberCost();
			}
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorMoreDeadThanAlive::CSoldierSelectorMoreDeadThanAlive()
{ 
	Name = "selector_mdta"; 
}

float CSoldierSelectorMoreDeadThanAlive::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorNoWeapon::CSoldierSelectorNoWeapon()
{ 
	Name = "selector_no_weapon"; 
}

float CSoldierSelectorNoWeapon::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	/**
	vfAddTravelCost();
	CHECK_RESULT;
	vfAddLightCost();
	CHECK_RESULT;
	vfAddTotalCoverCost();
	CHECK_RESULT;
	/**/
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	/**
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	/**/
	// returning a value
	return(m_fResult);
}

CSoldierSelectorPatrol::CSoldierSelectorPatrol()
{ 
	Name = "selector_patrol"; 
}

float CSoldierSelectorPatrol::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
//	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
//	// computations
//	vfAddDistanceToEnemyCost();
//	return(m_fResult);
	return(m_tEnemyPosition.distance_to(m_tCurrentPosition)*1000.f);
}

CSoldierSelectorPursuit::CSoldierSelectorPursuit()
{ 
	Name = "selector_pursuit"; 
}

float CSoldierSelectorPursuit::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	if (taMemberPositions.size()) {
		if (m_iAliveMemberCount) {
			for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)taMemberPositions.size(); m_iCurrentMember++) {
				vfAssignMemberPositionAndNode();
				vfComputeMemberDirection();
				vfAddDistanceToMemberCost();
				vfComputeSurroundEnemy();
				vfAddCoverFromMemberCost();
			}
			vfAddSurroundEnemyCost();
		}
	}
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorReload::CSoldierSelectorReload()
{ 
	Name = "selector_reload"; 
}

float CSoldierSelectorReload::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	//vfAddTravelCost();
	//CHECK_RESULT;
	//vfAddLightCost();
	//CHECK_RESULT;
	//vfAddTotalCoverCost();
	//CHECK_RESULT;
	//vfAddDistanceToEnemyCost();
	//CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	CHECK_RESULT;
	// checking for epsilon
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorRetreat::CSoldierSelectorRetreat()
{ 
	Name = "selector_retreat"; 
}

float CSoldierSelectorRetreat::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	//vfAddTotalCoverCost();
	//CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	CHECK_RESULT;
	//vfAddCoverFromEnemyCost();
	//CHECK_RESULT;
	// checking for epsilon
	//vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorSenseSomething::CSoldierSelectorSenseSomething()
{ 
	Name = "selector_sense_something"; 
}

float CSoldierSelectorSenseSomething::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)
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
	// checking for epsilon
	vfCheckForEpsilon(bStop);
	// returning a value
	return(m_fResult);
}

CSoldierSelectorUnderFireCover::CSoldierSelectorUnderFireCover()
{ 
	Name = "selector_under_fire_cover"; 
}

float CSoldierSelectorUnderFireCover::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	//CHECK_RESULT;
	vfAddLightCost();
	//CHECK_RESULT;
	vfAddTotalCoverCost();
	//CHECK_RESULT;
	vfAddEnemyLookCost(PI_DIV_4);
	//CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	//CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	
	//CHECK_RESULT;
	return(m_fResult);
}

CSoldierSelectorUnderFireLine::CSoldierSelectorUnderFireLine()
{ 
	Name = "selector_under_fire_line"; 
}

float CSoldierSelectorUnderFireLine::Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop)	// min - best, max - worse
{
	// initialization
	m_tpCurrentNode = tNode;
	m_fDistance = fDistance;
	vfInit();
	// computations
	vfAddTravelCost();
	//CHECK_RESULT;
	vfAddLightCost();
	//CHECK_RESULT;
	vfAddTotalCoverCost();
	//CHECK_RESULT;
	vfAddEnemyLookCost(PI_DIV_4);
	//CHECK_RESULT;
	vfAddDistanceToEnemyCost();
	//CHECK_RESULT;
	vfAddCoverFromEnemyCost();
	
	//CHECK_RESULT;
	return(m_fResult);
}
