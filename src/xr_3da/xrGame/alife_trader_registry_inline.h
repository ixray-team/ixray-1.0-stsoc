////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife trader registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CALifeTraderRegistry::TRADER_REGISTRY &CALifeTraderRegistry::traders	() const
{
	return					(m_traders);
}
