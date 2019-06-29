#include "stdafx.h"
#include "actor_statistic_mgr.h"
#include "alife_registry_wrappers.h"

xr_token actor_stats_token[]={
	{"total",				100},
	{"stalkerkills",		1},
	{"monsterkills",		2},
	{"quests",				3},
	{"artefacts",			4},
	{"reputation",			5},
	{"foo",					0},
	{0,						0}
};
void SStatDetailData::save(IWriter &stream)
{
	save_data			(key,		stream);
	save_data			(count,		stream);
	save_data			(points,	stream);
}

void SStatDetailData::load(IReader &stream)
{
	load_data			(key,		stream);
	load_data			(count,		stream);
	load_data			(points,	stream);
}
void SStatSectionData::save(IWriter &stream)	
{
	save_data			(data,			stream);
	save_data			(key,			stream);
	save_data			(total_points,	stream);
};

void SStatSectionData::load(IReader &stream)	
{
	load_data			(data,			stream);
	load_data			(key,			stream);
	load_data			(total_points,	stream);
};

SStatDetailData&	SStatSectionData::GetData	(const shared_str& key)
{
	vStatDetailData::iterator it		= data.begin();
	vStatDetailData::iterator it_e		= data.end();

	for(;it!=it_e;++it){
		if((*it).key == key)
			return *it;
	}
	data.resize				(data.size()+1);
	data.back				().key		= key;
	data.back				().count	= 0;
	data.back				().points	= 0;
	return data.back		();
}

CActorStatisticMgr::CActorStatisticMgr		()
{
	m_actor_stats_wrapper = xr_new<CActorStatisticsWrapper>();
	m_actor_stats_wrapper->registry().init(0);
}

CActorStatisticMgr::~CActorStatisticMgr		()
{
	xr_delete(m_actor_stats_wrapper);
}

vStatSectionData& CActorStatisticMgr::GetStorage		()
{
	return m_actor_stats_wrapper->registry().objects();
}

SStatSectionData&	CActorStatisticMgr::GetSection		(int key)
{
	vStatSectionData& d					= GetStorage();
	vStatSectionData::iterator it		= d.begin();
	vStatSectionData::iterator it_e		= d.end();
	for(;it!=it_e;++it){
		if((*it).key==key)
			return *it;
	}
	d.resize					(d.size()+1);
	d.back().key				= key;
	d.back().total_points		= 0;
	return	d.back				();
}
	
void CActorStatisticMgr::AddPoints		(int key, const shared_str& detail_key, s32 cnt, s32 pts)
{
	SStatSectionData& sect		= GetSection		(key);
	SStatDetailData& d			= sect.GetData		(detail_key);
	d.count						+= cnt;
	d.points					+= cnt*pts;

	sect.total_points			+= cnt*pts;
}

s32 CActorStatisticMgr::GetSectionPoints(int key)
{
	if(key!=100)
		return GetSection(key).total_points;
	else{//total
		s32 _total = 0;
		vStatSectionData& d					= GetStorage();
		vStatSectionData::iterator it		= d.begin();
		vStatSectionData::iterator it_e		= d.end();
		for(;it!=it_e;++it)
			_total +=(*it).total_points;
		return _total;
	}
}
