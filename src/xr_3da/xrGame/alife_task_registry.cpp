////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_task_registry.h"
#include "object_broker.h"

struct CTaskLoader : public object_loader::detail::CEmptyPredicate {
	using object_loader::detail::CEmptyPredicate::operator();
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value, bool first) const {return(!first);}
	template <typename T1, typename T2>
	IC	void after_load	(T1 &data, T2 &stream) const
	{
		const_cast<object_type_traits::remove_const<typename T1::first_type>::type&>(data.first) = data.second->m_tTaskID;
	}
};

CALifeTaskRegistry::~CALifeTaskRegistry	()
{
	clear						();
}

void CALifeTaskRegistry::save			(IWriter &memory_stream)
{
	Msg							("* Saving tasks...");
	memory_stream.open_chunk	(TASK_CHUNK_DATA);
	memory_stream.w				(&m_id,sizeof(m_id));
	save_data					(m_tasks,memory_stream,CTaskLoader());
	memory_stream.close_chunk	();
}

void CALifeTaskRegistry::load			(IReader &file_stream)
{
	Msg							("* Loading tasks...");
	R_ASSERT2					(file_stream.find_chunk(TASK_CHUNK_DATA),"Can't find chunk TASK_CHUNK_DATA");
	file_stream.r				(&m_id,sizeof(m_id));
	load_data					(m_tasks,file_stream,CTaskLoader());
	ALife::TASK_PAIR_IT			I = m_tasks.begin();
	ALife::TASK_PAIR_IT			E = m_tasks.end();
	for ( ; I != E; ++I)
		update					((*I).second);
}
