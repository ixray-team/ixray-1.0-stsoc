#include "stdafx.h"
#include "LightProjector.h"
#include "..\xr_object.h"
#include "lighttrack.h"

void	CLightProjector::cache_init		()						// 
{
	receivers.resize	(P_o_count);
	index.resize		(P_o_count);
	for					(int it=0; it<P_o_count; it++)
	{
		recv&			R	= receivers[it];
		cache_index&	CI	= index[it];
		R.O					= 0;
		R.needupdate		= false;
		CI.id				= it;
		CI.priority			= flt_max;
	}
}

BOOL	CLightProjector::cache_valid	(IRenderable*	O)		// 
{
	CLightTrack*		LT		= (CLightTrack*)O->renderable.ROS;
	int					slot	= LT->Shadowed_Slot;
	if (receivers[slot].O!=O)				return false;
	Fbox				bb;
	bb.xform			(O->renderable.visual->vis.box,O->renderable.xform);
	if (receivers[slot].BB.contains(bb))	return true;
	else									return false;
}

int		CLightProjector::cache_getindex	(Fvector		C)		// <0 means no slots left
{
}

void	CLightProjector::cache_refactor	()						// build new priority table
{
	for					(int it=0; it<P_o_count; it++)
	{
		recv&			R	= receivers[it];
		cache_index&	CI	= index[it];
	}
}
