////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_detail_path_manager_inline.h
//	Created 	: 01.11.2005
//  Modified 	: 01.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human detail path manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeHumanDetailPathManager::object_type &CALifeHumanDetailPathManager::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

IC	void CALifeHumanDetailPathManager::speed										(const float &speed)
{
	VERIFY		(_valid(speed));
	m_speed		= speed;
}

IC	const float &CALifeHumanDetailPathManager::speed								() const
{
	VERIFY		(_valid(m_speed));
	return		(m_speed);
}

IC	const CALifeHumanDetailPathManager::PATH &CALifeHumanDetailPathManager::path	() const
{
	return		(m_path);
}
