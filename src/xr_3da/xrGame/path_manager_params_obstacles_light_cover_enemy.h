////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_params_obstacles_light_cover_enemy.h
//	Created 	: 21.03.2002
//  Modified 	: 04.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Obstacles light cover enemy path manager parameters
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SObstaclesLightCoverEnemy : public SObstaclesLightCover<
	_dist_type,
	_index_type,
	_iteration_type
> {
	const Fvector			&enemy_position;
	float					enemy_distance;
	float					enemy_view_weight;

	IC	SObstaclesLightCoverEnemy (
			_dist_type		max_range = _dist_type(6000),
			_iteration_type	max_iteration_count = _iteration_type(-1),
			u32				max_visited_node_count = u32(-1),
			bool			avoid_dynamic_obstacles = true,
			float			light_weight = 10.f,
			float			cover_weight = 20.f,
			float			distance_weight = 40.f,
			const Fvector	&enemy_position = Fvector().set(0,0,0),
			float			enemy_distance = 30.f,
			float			enemy_view_weight = 100.f
		) : 
		SObstaclesLightCover<
			_dist_type,
			_index_type,
			_iteration_type
		>(
			max_range,
			max_iteration_count,
			max_visited_node_count,
			avoid_dynamic_obstacles,
			light_weight,
			cover_weight,
			distance_weight
		),
		enemy_position(enemy_position), 
		enemy_distance(enemy_distance), 
		enemy_view_weight(enemy_view_weight)
	{
	}
};
