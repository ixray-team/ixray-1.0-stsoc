////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_news_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife news registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_news_registry.h"
#include "object_broker.h"

struct CNewsLoader : public object_loader::detail::CEmptyPredicate {
	using object_loader::detail::CEmptyPredicate::operator();
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value, bool first) const {return(!first);}
	template <typename T1, typename T2>
	IC	void after_load	(T1 &data, T2 &stream) const
	{
		const_cast<object_type_traits::remove_const<typename T1::first_type>::type&>(data.first) = data.second->m_news_id;
	}
};

CALifeNewsRegistry::CALifeNewsRegistry	(LPCSTR section)
{
	clear						();
}

CALifeNewsRegistry::~CALifeNewsRegistry	()
{
	clear						();
}

void CALifeNewsRegistry::clear			()
{
	m_last_id					= 0;
	delete_data					(m_news);
}

void CALifeNewsRegistry::save			(IWriter &memory_stream)
{
	Msg							("* Saving news...");
	memory_stream.open_chunk	(NEWS_CHUNK_DATA);
	memory_stream.w				(&m_last_id,sizeof(m_last_id));
	save_data					(m_news,memory_stream,CNewsLoader());
	memory_stream.close_chunk	();
}

void CALifeNewsRegistry::load			(IReader &file_stream)
{
	Msg							("* Loading news...");
	R_ASSERT2					(file_stream.find_chunk(NEWS_CHUNK_DATA),"Can't find chunk NEWS_CHUNK_DATA!");
	file_stream.r				(&m_last_id,sizeof(m_last_id));
	load_data					(m_news,file_stream,CNewsLoader());
}
