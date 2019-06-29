#include "stdafx.h"
#include <xmmintrin.h>

// can you say "barebone"?
struct	vec_t	{ float x,y,z,pad; };
vec_t	vec_c	( float _x, float _y, float _z)	{ vec_t v; v.x=_x;v.y=_y;v.z=_z;v.pad=0; return v; }
struct aabb_t	{ 
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

#define _MM_ALIGN16		__declspec(align(16))
#define _MM_NOINLINE	__declspec(noinline)

// turn those verbose intrinsics into something readable.
#define loadps(mem)		_mm_load_ps((const float * const)(mem))
#define storess(ss,mem)	_mm_store_ss((float * const)(mem),(ss))
#define minss			_mm_min_ss
#define maxss			_mm_max_ss
#define minps			_mm_min_ps
#define maxps			_mm_max_ps
#define mulps			_mm_mul_ps
#define subps			_mm_sub_ps
#define rotatelps(ps)	_mm_shuffle_ps((ps),(ps), 0x39)	// a,b,c,d -> b,c,d,a
#define muxhps(low,high) _mm_movehl_ps((low),(high))	// low{a,b,c,d}|high{e,f,g,h} = {c,d,g,h}


static const float flt_plus_inf = -logf(0);	// let's keep C and C++ compilers happy.
static const float _MM_ALIGN16
ps_cst_plus_inf[4]	= {  flt_plus_inf,  flt_plus_inf,  flt_plus_inf,  flt_plus_inf },
ps_cst_minus_inf[4]	= { -flt_plus_inf, -flt_plus_inf, -flt_plus_inf, -flt_plus_inf };


static BOOL _MM_NOINLINE ray_box_intersect(const aabb_t &box, const ray_t &ray, ray_segment_t &rs) {
	// you may already have those values hanging around somewhere
	const __m128
		plus_inf	= loadps(ps_cst_plus_inf),
		minus_inf	= loadps(ps_cst_minus_inf);

	// use whatever's apropriate to load.
	const __m128
		box_min	= loadps(&box.min),
		box_max	= loadps(&box.max),
		pos		= loadps(&ray.pos),
		inv_dir	= loadps(&ray.inv_dir);

	// use a div if inverted directions aren't available
	const __m128 l1 = mulps(subps(box_min, pos), inv_dir);
	const __m128 l2 = mulps(subps(box_max, pos), inv_dir);

	// the order we use for those min/max is vital to filter out
	// NaNs that happens when an inv_dir is +/- inf and
	// (box_min - pos) is 0. inf * 0 = NaN
	const __m128 filtered_l1a = minps(l1, plus_inf);
	const __m128 filtered_l2a = minps(l2, plus_inf);

	const __m128 filtered_l1b = maxps(l1, minus_inf);
	const __m128 filtered_l2b = maxps(l2, minus_inf);

	// now that we're back on our feet, test those slabs.
	__m128 lmax = maxps(filtered_l1a, filtered_l2a);
	__m128 lmin = minps(filtered_l1b, filtered_l2b);

	// unfold back. try to hide the latency of the shufps & co.
	const __m128 lmax0 = rotatelps(lmax);
	const __m128 lmin0 = rotatelps(lmin);
	lmax = minss(lmax, lmax0);
	lmin = maxss(lmin, lmin0);

	const __m128 lmax1 = muxhps(lmax,lmax);
	const __m128 lmin1 = muxhps(lmin,lmin);
	lmax = minss(lmax, lmax1);
	lmin = maxss(lmin, lmin1);

	const BOOL ret = _mm_comige_ss(lmax, _mm_setzero_ps()) & _mm_comige_ss(lmax,lmin);

	storess(lmin, &rs.t_near);
	storess(lmax, &rs.t_far);

	return  ret;
}

IC u32& uf		(float &x) { return (u32&)x; }
static BOOL _MM_NOINLINE pick_fpu	(const Fvector& min, const Fvector& max, const Fvector& origin, const Fvector& dir, Fvector& coord)
{
	Fvector				MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;
	BOOL Inside			= TRUE;

	// Find candidate planes.
	if(origin[0] < min[0]) {
		coord[0]	= min[0];
		Inside		= FALSE;
		if(uf(dir[0]))	MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
	} else if(origin[0] > max[0]) {
		coord[0]	= max[0];
		Inside		= FALSE;
		if(uf(dir[0]))	MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
	}
	if(origin[1] < min[1]) {
		coord[1]	= min[1];
		Inside		= FALSE;
		if(uf(dir[1]))	MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
	} else if(origin[1] > max[1]) {
		coord[1]	= max[1];
		Inside		= FALSE;
		if(uf(dir[1]))	MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
	}
	if(origin[2] < min[2]) {
		coord[2]	= min[2];
		Inside		= FALSE;
		if(uf(dir[2]))	MaxT[2] = (min[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
	} else if(origin[2] > max[2]) {
		coord[2]	= max[2];
		Inside		= FALSE;
		if(uf(dir[2]))	MaxT[2] = (max[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
	}

	// Ray origin inside bounding box
	if(Inside)		{
		coord		= origin;
		return		true;
	}

	// Get largest of the maxT's for final choice of intersection
	u32 WhichPlane = 0;
	if	(MaxT[1] > MaxT[0])				WhichPlane = 1;
	if	(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box (if max < 0)
	if(uf(MaxT[WhichPlane])&0x80000000) return false;

	if  (0==WhichPlane)	{	// 1 & 2
		coord[1] = origin[1] + MaxT[0] * dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		coord[2] = origin[2] + MaxT[0] * dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (1==WhichPlane)	{	// 0 & 2
		coord[0] = origin[0] + MaxT[1] * dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[2] = origin[2] + MaxT[1] * dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (2==WhichPlane)	{	// 0 & 1
		coord[0] = origin[0] + MaxT[2] * dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[1] = origin[1] + MaxT[2] * dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		return true;
	}
	return false;
}

#define	TESTS	2000000
#define TESTS_M (TESTS/1000000)
void	testbed(void)
{
	aabb_t	_MM_ALIGN16 box_sse;
	Fbox	_MM_ALIGN16 box_fpu;	
	box_sse.min.x = box_sse.min.y = box_sse.min.z = -1.f;
	box_sse.max.x = box_sse.max.y = box_sse.max.z = +1.f;
	box_fpu.min.set	(-1.f,-1.f,-1.f); 
	box_fpu.max.set	(+1.f,+1.f,+1.f);
	CTimer			t_fpu,	t_sse;

	// gen rays
	static	_MM_ALIGN16	ray_t	rays		[TESTS]	;

	FPU::m24r						();
	/*
	for (u32 i=0; i<TESTS; i++)		{
		Fvector3	r;	r.random_dir().normalize( );
		Fvector3	p;	p.random_dir().normalize( ).mul(2.f);
		Fvector3	ri;	ri.set(1.f,1.f,1.f).div	(r);
		rays_fpu[i].pos		= vec_c( p.x, p.y, p.z	);
		rays_fpu[i].inv_dir	= vec_c( r.x, r.y, r.z	);
		rays_sse[i].pos		= vec_c( p.x, p.y, p.z	);
		rays_sse[i].inv_dir	= vec_c( ri.x,ri.y,ri.z	);
	}
	*/

	// results
	static	_MM_ALIGN16	ray_segment_t	result_sse	[TESTS];
	static	_MM_ALIGN16	BOOL			result_sse_b[TESTS];
	static	_MM_ALIGN16	Fvector3		result_fpu	[TESTS];
	static	_MM_ALIGN16	BOOL			result_fpu_b[TESTS];

	float	sec_fpu	= 0; u64 cycles_fpu = 0;
	float	sec_sse	= 0; u64 cycles_sse = 0;

	for (int test=0; test<3; test++)	{
		// gen-sse
		Random.seed(0x1234567);
		for (u32 i=0; i<TESTS; i++)		{
			Fvector3	r;	r.random_dir().normalize( );
			Fvector3	p;	p.random_dir().normalize( ).mul(2.f);
			Fvector3	ri;	ri.set(1.f,1.f,1.f).div	(r);
			rays[i].pos		= vec_c( p.x, p.y, p.z	);
			rays[i].inv_dir	= vec_c( ri.x,ri.y,ri.z	);
		}

		// sse
		t_sse.Start		();
		for (u32 i=0; i<TESTS; i++)	{
			ray_t&			r		= rays		[i];
			result_sse_b	[i]		= ray_box_intersect	(box_sse,r,result_sse[i]);
		}
		cycles_sse	= t_sse.GetElapsed_clk	();
		sec_sse		= t_sse.GetElapsed_sec	();

		// gen-fpu
		Random.seed(0x1234567);
		for (u32 i=0; i<TESTS; i++)		{
			Fvector3	r;	r.random_dir().normalize( );
			Fvector3	p;	p.random_dir().normalize( ).mul(2.f);
			Fvector3	ri;	ri.set(1.f,1.f,1.f).div	(r);
			rays[i].pos		= vec_c( p.x, p.y, p.z	);
			rays[i].inv_dir	= vec_c( r.x, r.y, r.z	);
		}

		// fpu
		t_fpu.Start		();
		for (u32 i=0; i<TESTS; i++)	{
			ray_t&			r		= rays			[i];
			result_fpu_b	[i]		= pick_fpu		(box_fpu.min,box_fpu.max,*(Fvector3*)(&r.pos),*(Fvector3*)(&r.inv_dir),result_fpu[i]);
		}
		cycles_fpu	= t_fpu.GetElapsed_clk	();
		sec_fpu		= t_fpu.GetElapsed_sec	();
	}

	// compare results
	u32		_fail	= 0;
	for (u32 i=0; i<TESTS; i++)	{
		if (result_fpu_b[i] != result_sse_b[i])		{ 
			_fail++		;
			continue	;
		}
		if ((0==result_fpu_b[i]) && (0==result_sse_b[i]))	{ continue; }
		ray_t&			r		= rays			[i];
		ray_segment_t&	s		= result_sse	[i];
		Fvector3	p_dir;		p_dir.set		(r.inv_dir.x,r.inv_dir.y,r.inv_dir.z);
		Fvector3	p_fpu		= result_fpu	[i];
		Fvector3	p_sse;		p_sse.set		(r.pos.x,r.pos.y,r.pos.z).mad(p_dir,s.t_near);
		if (!p_fpu.similar(p_sse,EPS))			{
			_fail++		;
			continue	;
		}
	};

	// results
	float	p_fpu	= float(TESTS_M)/sec_fpu;	u32 c_fpu	= u32(u64(cycles_fpu/u64(TESTS)));
	float	p_sse	= float(TESTS_M)/sec_sse;	u32 c_sse	= u32(u64(cycles_sse/u64(TESTS)));
	Msg		("! sanity_fail(%d), perf: %f --- fpu(%f Mrs, %dc), sse(%f Mrs, %dc)",_fail,p_sse/p_fpu,p_fpu,c_fpu,p_sse,c_sse,result_fpu,result_fpu_b,result_sse,result_sse_b);
	FlushLog();
}
