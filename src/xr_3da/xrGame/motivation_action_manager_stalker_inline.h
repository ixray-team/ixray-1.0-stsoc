////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_action_manager_stalker_inline.h
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivation action manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void CMotivationActionManagerStalker::affect_cover	(bool value)
{
	m_affect_cover	= value;
}

IC	bool CMotivationActionManagerStalker::affect_cover	() const
{
	return			(m_affect_cover);
}
