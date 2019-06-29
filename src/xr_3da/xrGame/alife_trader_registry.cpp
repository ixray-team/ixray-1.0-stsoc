////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife trader registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_trader_registry.h"
#include "ai_space.h"
#include "game_graph.h"

CALifeTraderRegistry::~CALifeTraderRegistry				()
{
}

CSE_ALifeTrader *CALifeTraderRegistry::trader_nearest	(CSE_ALifeHumanAbstract *human)
{
	float						best_distance = flt_max;
	CSE_ALifeTrader				*trader = 0;
	const Fvector				&position = ai().game_graph().vertex(human->m_tGraphID)->game_point();
	
	TRADER_REGISTRY::const_iterator	I = traders().begin();
	TRADER_REGISTRY::const_iterator	E = traders().end();
	for ( ; I != E; ++I) {
		float					distance = ai().game_graph().vertex((*I)->m_tGraphID)->game_point().distance_to(position);
		if (distance < best_distance) {
			best_distance		= distance;
			trader				= *I;
		}
	}
	
	R_ASSERT2					(trader,"There is no traders in the game");
	return						(trader);
}

void CALifeTraderRegistry::add							(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeTrader				*trader = smart_cast<CSE_ALifeTrader*>(object);
	if (!trader)
		return;

	m_traders.push_back			(trader);
}

void CALifeTraderRegistry::remove						(CSE_ALifeDynamicObject *object)
{
	CSE_ALifeTrader				*trader = smart_cast<CSE_ALifeTrader*>(object);
	if (!trader)
		return;

	TRADER_REGISTRY::iterator	I = std::find(m_traders.begin(),m_traders.end(),trader);
	if (I == m_traders.end())
		return;

	m_traders.erase				(I);
}
