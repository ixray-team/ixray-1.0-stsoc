#pragma once

IC	void CMonsterMovement::set_cover_params(float min, float max, float dev, float radius)
{
	m_cover_info.min_dist		= min;
	m_cover_info.max_dist		= max;
	m_cover_info.deviation		= dev;
	m_cover_info.radius			= radius;
}

IC void CMonsterMovement::set_use_covers(bool val)
{
	m_cover_info.use_covers	= val;	
}

IC void CMonsterMovement::force_target_set() 
{	
	m_time = 0;
}

IC void CMonsterMovement::set_rebuild_time(u32 time) 
{
	m_time	= time;
}

IC bool CMonsterMovement::failed() 
{
	return m_failed;
}

IC bool CMonsterMovement::path_end()
{
	return m_path_end;
}

IC void	CMonsterMovement::set_distance_to_end(float dist)
{
	m_distance_to_path_end = dist;
}

IC bool CMonsterMovement::actual_params()
{
	return m_actual;
}
