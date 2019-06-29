#include "stdafx.h"
#include "ISpatial.h"

extern Fvector	c_spatial_offset[8];

template <bool b_first>
class	walker
{
public:
	u32			mask;
	Fvector		center;
	float		radius;
	Fbox		bb;
public:
	walker					(u32 _mask, const Fvector& _center, const float	_radius)
	{
		mask	= _mask;
		center	= _center;
		size	= _size;
		box.setb(center,size);
	}
	void		walk		(ISpatial_NODE* N, Fvector& n_center, float n_radius)
	{
		// box
		Fbox	BB;			BB.set	(n_center.x-n_radius, n_center.y-n_radius, n_center.z-n_radius, n_center.x+n_radius, n_center.y+n_radius, n_center.z+n_radius);
		if		(!BB.intersect(box))			return;

		// test items
		xr_vector<ISpatial*>::iterator _it	=	N->items.begin	();
		xr_vector<ISpatial*>::iterator _end	=	N->items.end	();
		for (; _it!=_end; _it++)
		{
			ISpatial*		S	= *_it;
			if (0==(S->spatial_type&mask))	continue;

			Fvector&		sC		= S->spatial_center;
			float			sR		= S->spatial_radius;
			Fbox			sB;		sB.set	(sC.x-sR, sC.y-sR, sC.z-sR, sC.x+sR, sC.y+sR, sC.z+sR);
			if (!sB.intersect(box))	continue;

			SpatialSpace->q_result.push_back(S);
			if (b_first)			return;
		}

		// recurse
		float	c_radius		= n_radius/2;
		for (u32 octant=0; octant<8; octant++)
		{
			if (0==N->children[octant])		continue;
			Fvector		c_center;			c_center.mad	(n_center,c_spatial_offset[octant],n_radius/4.f);
			walk							(N->children[octant],c_center,c_radius);
			if (b_first && !SpatialSpace->q_result.empty())	return;
		}
	}
};

void	ISpatial_DB::q_sphere(u32 _o, u32 _mask, const Fvector& _center, const float _radius)
{
	q_result.clear	();
	if (_o & O_ONLYFIRST)			{ walker<true>	W(_mask,_center,_size);	W.walk(m_root,m_center,m_bounds); } 
	else							{ walker<false>	W(_mask,_center,_size);	W.walk(m_root,m_center,m_bounds); } 
}
