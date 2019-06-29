#pragma once
#include "alife_abstract_registry.h"

extern xr_token actor_stats_token[];


struct SStatDetailData: public IPureSerializeObject<IReader,IWriter>
{
	shared_str	key;
	s32			count;
	s32			points;

	virtual void save								(IWriter &stream);
	virtual void load								(IReader &stream);
};

typedef xr_vector<SStatDetailData>	vStatDetailData;

struct SStatSectionData: public IPureSerializeObject<IReader,IWriter>{
	int					key;
	s32					total_points;
	vStatDetailData		data;

	SStatDetailData&	GetData			(const shared_str&);
	virtual void		save			(IWriter &stream);
	virtual void		load			(IReader &stream);
};

typedef xr_vector<SStatSectionData> vStatSectionData;
typedef CALifeAbstractRegistry<u16, vStatSectionData> CActorStatisticRegistry;