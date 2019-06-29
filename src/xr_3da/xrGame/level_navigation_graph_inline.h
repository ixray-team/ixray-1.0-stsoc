////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CLevelNavigationGraph::CSectorGraph	&CLevelNavigationGraph::sectors	() const
{
	VERIFY		(m_sectors);
	return		(*m_sectors);
}

IC	CLevelNavigationGraph::CSectorGraph	&CLevelNavigationGraph::sectors			()
{
	VERIFY		(m_sectors);
	return		(*m_sectors);
}
