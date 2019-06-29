////////////////////////////////////////////////////////////////////////////
//	Module 		: selector_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Selector manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CAbstractVertexEvaluator *CSelectorManager::selector(LPCSTR selector_name)
{
	SELECTORS::iterator	I = m_selectors.find(selector_name);
	VERIFY				(m_selectors.end() != I);
	return				((*I).second);
}

template <typename T>
IC	void CSelectorManager::add				(LPCSTR section, LPCSTR selector_name)
{
	SELECTORS::const_iterator	I = m_selectors.find(selector_name);
	VERIFY						(m_selectors.end() == I);
	T							*selector = xr_new<T>();
	selector->Load				(section,selector_name);
	m_selectors.insert			(std::make_pair(selector_name,selector));
}

