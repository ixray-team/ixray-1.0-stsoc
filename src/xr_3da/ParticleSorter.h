// ParticleSorter.h: interface for the CParticleSorter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLESORTER_H__FFF58046_855A_4BFF_A38B_D02470C6414B__INCLUDED_)
#define AFX_PARTICLESORTER_H__FFF58046_855A_4BFF_A38B_D02470C6414B__INCLUDED_
#pragma once

const u32	PS_SORT_HASH =	200;

struct ENGINE_API PS_SORT_RECORD 
{
	u32	record	:	16;
	u32 id		:	16;
};

class ENGINE_API CParticleSorter  
{
public:
	CParticleSorter				();
	virtual ~CParticleSorter	();
};

#endif // !defined(AFX_PARTICLESORTER_H__FFF58046_855A_4BFF_A38B_D02470C6414B__INCLUDED_)
