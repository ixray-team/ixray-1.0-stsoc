#include "stdafx.h"
#include "stats_timer.h"

CStatTimer::CStatTimer()
{
	accum	= 0;
	result	= 0;
	count	= 0;
}

void	CStatTimer::FrameStart	()
{
	accum	= 0;
	count	= 0;
}
void	CStatTimer::FrameEnd	()
{
	float	_time		= float(accum)*CPU::cycles2milisec;
	if (_time > result)	result	=	_time;
	else				result	=	0.99f*result + 0.01f*_time;
}
