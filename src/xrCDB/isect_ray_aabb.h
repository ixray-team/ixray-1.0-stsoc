#pragma once
// fastest single(ray) vs multiple(aabb) intersection
// SSE version is ~2 times faster on K8 & ~3 times faster on P4


// sse  types
struct	vec_t	{ float x,y,z,pad; };
vec_t	vec_c	( float _x, float _y, float _z)	{ vec_t v; v.x=_x;v.y=_y;v.z=_z;v.pad=0; return v; }
struct	aabb_t	{ 
	vec_t		min;
	vec_t		max;
};
struct ray_t	{
	vec_t		pos;
	vec_t		inv_dir;
};
struct ray_segment_t {
	float		t_near,t_far;
};
