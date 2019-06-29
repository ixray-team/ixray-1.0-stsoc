////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_cross_table.h
//	Created 	: 20.02.2003
//  Modified 	: 20.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#pragma once

#define CROSS_TABLE_NAME					"level.gct"

#define CROSS_TABLE_CHUNK_VERSION			0
#define CROSS_TABLE_CHUNK_DATA				1

class CSE_ALifeCrossTable {
public:
	#pragma pack(push,2)
	typedef struct tagSCrossTableHeader {
		u32									dwVersion;
		u32									dwNodeCount;
		u32									dwGraphPointCount;
	} SCrossTableHeader;
	
	typedef struct tagSCrossTableCell {
		u16									tGraphIndex;
		float								fDistance;
	} SCrossTableCell;
	#pragma pack(pop)

	SCrossTableHeader						m_tCrossTableHeader;
	IReader									*m_tpCrossTableVFS;
	SCrossTableCell							*m_tpaCrossTable;

	CSE_ALifeCrossTable						()
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
	};

	CSE_ALifeCrossTable						(LPCSTR fName)
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
		Load								(fName);
	};

	virtual ~CSE_ALifeCrossTable				()
	{
		Unload();
	};

	IC void Load							(LPCSTR fName)
	{
		m_tpCrossTableVFS					= FS.r_open(fName);
		R_ASSERT2							(m_tpCrossTableVFS,"Can't open cross table!");
		R_ASSERT2							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_VERSION),"Can't find chunk CROSS_TABLE_CHUNK_VERSION!");
		m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_VERSION);
		m_tCrossTableHeader.dwVersion		= m_tpCrossTableVFS->r_u32();
		m_tCrossTableHeader.dwNodeCount		= m_tpCrossTableVFS->r_u32();
		m_tCrossTableHeader.dwGraphPointCount	= m_tpCrossTableVFS->r_u32();
		R_ASSERT2							(m_tCrossTableHeader.dwVersion == XRAI_CURRENT_VERSION,"Cross table version mismatch!");
		R_ASSERT2							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_DATA),"Can't find chunk CROSS_TABLE_CHUNK_DATA!");
		m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_DATA);
		m_tpaCrossTable						= (SCrossTableCell*)m_tpCrossTableVFS->pointer();
	};

	IC void Unload							()
	{
		xr_delete							(m_tpCrossTableVFS);
		m_tpaCrossTable						= 0;
	};
};