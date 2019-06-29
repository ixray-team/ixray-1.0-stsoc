//////////////////////////////////////////////////////////////////////////
// character_community.cpp:		структура представления группировки
//							
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "character_community.h"

#define GAME_RELATIONS_SECT "game_relations"
#define GAME_COMMUNITIES	"communities"
#define COMMUNITIES_TABLE	"communities_relations"
#define SYMPATHY_TABLE_SECT "communities_sympathy"

//////////////////////////////////////////////////////////////////////////
COMMUNITY_DATA::COMMUNITY_DATA (CHARACTER_COMMUNITY_INDEX idx, CHARACTER_COMMUNITY_ID idn, LPCSTR team_str)
{
	index = idx;
	id = idn;
	team = (u8)atoi(team_str);
}

//////////////////////////////////////////////////////////////////////////
CHARACTER_COMMUNITY::GOODWILL_TABLE CHARACTER_COMMUNITY::m_relation_table;
CHARACTER_COMMUNITY::SYMPATHY_TABLE CHARACTER_COMMUNITY::m_sympathy_table;

//////////////////////////////////////////////////////////////////////////
CHARACTER_COMMUNITY::CHARACTER_COMMUNITY	()
{
	m_current_index = NO_COMMUNITY_INDEX;
}
CHARACTER_COMMUNITY::~CHARACTER_COMMUNITY	()
{
}


void  CHARACTER_COMMUNITY::set	(CHARACTER_COMMUNITY_ID id)
{
	m_current_index	 = IdToIndex(id);

}

CHARACTER_COMMUNITY_ID		 CHARACTER_COMMUNITY::id			() const
{
	return IndexToId(m_current_index);
}

u8							 CHARACTER_COMMUNITY::team			() const
{
	return (*m_pItemDataVector)[m_current_index].team;
}


void CHARACTER_COMMUNITY::InitIdToIndex	()
{
	section_name = GAME_RELATIONS_SECT;
	line_name = GAME_COMMUNITIES;

	m_relation_table.set_table_params(COMMUNITIES_TABLE);
	m_sympathy_table.set_table_params(SYMPATHY_TABLE_SECT, 1);
}


CHARACTER_GOODWILL CHARACTER_COMMUNITY::relation		(CHARACTER_COMMUNITY_INDEX to)
{
	return relation(m_current_index, to);
}

CHARACTER_GOODWILL  CHARACTER_COMMUNITY::relation		(CHARACTER_COMMUNITY_INDEX from, CHARACTER_COMMUNITY_INDEX to)
{
	VERIFY(from >= 0 && from <(int)m_relation_table.table().size());
	VERIFY(to >= 0 && to <(int)m_relation_table.table().size());
	
	return m_relation_table.table()[from][to];
}

float  CHARACTER_COMMUNITY::sympathy			(CHARACTER_COMMUNITY_INDEX comm)
{
	VERIFY(comm >= 0 && comm <(int)m_sympathy_table.table().size());
	return m_sympathy_table.table()[comm][0];
}

void CHARACTER_COMMUNITY::DeleteIdToIndexData	()
{
	m_relation_table.clear();
	m_sympathy_table.clear();
	inherited::DeleteIdToIndexData();
}