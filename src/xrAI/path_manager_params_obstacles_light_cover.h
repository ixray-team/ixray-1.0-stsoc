////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_params_obstacles_light_cover.h
//	Created 	: 21.03.2002
//  Modified 	: 04.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Obstacles light cover path manager parameters
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SObstaclesLightCover : public SObstacleParams<
	_dist_type,
	_index_type,
	_iteration_type
> {
	float	light_weight;
	float	cover_weight;
	float	distance_weight;

	IC SObstaclesLightCover(
			_dist_type		max_range = _dist_type(6000),
			_iteration_type	max_iteration_count = _iteration_type(-1),
			u32				max_visited_node_count = u32(-1),
			bool			avoid_dynamic_obstacles = true,
			float			light_weight = 10.f,
			float			cover_weight = 20.f,
			float			distance_weight = 40.f
		) :
		SObstacleParams <
			_dist_type,
			_index_type,
			_iteration_type
		>(
			max_range,
			max_iteration_count,
			max_visited_node_count,
			avoid_dynamic_obstacles
		),
		light_weight(light_weight),
		cover_weight(cover_weight),
		distance_weight(distance_weight)
	{
	}
};

