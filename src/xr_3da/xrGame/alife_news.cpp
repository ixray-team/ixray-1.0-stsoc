////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_news.cpp
//	Created 	: 05.01.2003
//  Modified 	: 17.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife news class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_news.h"

CALifeNews::~CALifeNews	()
{
}

void CALifeNews::load	(IReader &file_stream)
{
	file_stream.r		(&m_news_id			,sizeof(m_news_id		));
	file_stream.r		(&m_game_time		,sizeof(m_game_time		));
	file_stream.r		(&m_game_vertex_id	,sizeof(m_game_vertex_id));
	file_stream.r		(&m_news_type		,sizeof(m_news_type		));
	file_stream.r		(&m_object_id[0]	,sizeof(m_object_id[0]	));
	file_stream.r		(&m_object_id[1]	,sizeof(m_object_id[1]	));
	file_stream.r		(&m_class_id		,sizeof(m_class_id		));
}

void CALifeNews::save	(IWriter &memory_stream)
{
	memory_stream.w		(&m_news_id			,sizeof(m_news_id		));
	memory_stream.w		(&m_game_time		,sizeof(m_game_time		));
	memory_stream.w		(&m_game_vertex_id	,sizeof(m_game_vertex_id));
	memory_stream.w		(&m_news_type		,sizeof(m_news_type		));
	memory_stream.w		(&m_object_id[0]	,sizeof(m_object_id[0]	));
	memory_stream.w		(&m_object_id[1]	,sizeof(m_object_id[1]	));
	memory_stream.w		(&m_class_id		,sizeof(m_class_id		));
}
