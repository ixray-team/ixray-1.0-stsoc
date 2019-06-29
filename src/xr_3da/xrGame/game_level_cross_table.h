////////////////////////////////////////////////////////////////////////////
//	Module 		: game_level_cross_table.h
//	Created 	: 20.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Cross table between game and level graphs
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "../xrLevel.h"
#endif

#include "alife_space.h"
#include "game_graph_space.h"

#define CROSS_TABLE_NAME					"level.gct"

#define CROSS_TABLE_CHUNK_VERSION			0
#define CROSS_TABLE_CHUNK_DATA				1

class CGameLevelCrossTable {
public:
	#pragma pack(push,2)
	class CHeader {
		u32					dwVersion;
		u32					dwNodeCount;
		u32					dwGraphPointCount;
		xrGUID				m_level_guid;
		xrGUID				m_game_guid;

	public:
		IC	u32				version					() const;
		IC	u32				level_vertex_count		() const;
		IC	u32				game_vertex_count		() const;
		IC	const xrGUID	&level_guid				() const;
		IC	const xrGUID	&game_guid				() const;

#ifdef AI_COMPILER		
		friend class CLevelGameGraph;
		friend class CCrossTableBuilder;
		friend class CRenumbererConverter;
		friend class CGameGraphBuilder;
#endif
	};
	
	class  CCell {
		GameGraph::_GRAPH_ID	tGraphIndex;
		float				fDistance;
	public:
		IC	GameGraph::_GRAPH_ID game_vertex_id			() const;
		IC	float			distance				() const;
#ifdef AI_COMPILER		
		friend class CLevelGameGraph;
		friend class CCrossTableBuilder;
		friend class CRenumbererConverter;
		friend class CGameGraphBuilder;
#endif
	};
	#pragma pack(pop)

protected:
	CHeader					m_tCrossTableHeader;
	IReader					*m_tpCrossTableVFS;
	IReader					*m_chunk;
	CCell					*m_tpaCrossTable;

public:
#ifdef AI_COMPILER
	IC						CGameLevelCrossTable	(LPCSTR fName);
#else
	IC						CGameLevelCrossTable	();
#endif
	IC virtual				~CGameLevelCrossTable	();
	IC		const CCell		&vertex					(u32 level_vertex_id) const;
	IC		const CHeader	&header					() const;
#ifdef AI_COMPILER		
	friend class CLevelGameGraph;
	friend class CCrossTableBuilder;
	friend class CRenumbererConverter;
	friend class CGameGraphBuilder;
#endif
};

#include "game_level_cross_table_inline.h"