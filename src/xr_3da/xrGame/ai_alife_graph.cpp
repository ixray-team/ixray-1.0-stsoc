////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_graph.cpp
//	Created 	: 18.02.2003
//  Modified 	: 23.06.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_graph.h"

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "../xrLevel.h"
#endif

void CSE_ALifeGraph::Load				(LPCSTR fName)
{ 
	R_ASSERT3							(FS.exist(fName),"There is no graph for the level ",fName);
	m_tpGraphVFS						= FS.r_open(fName);
	m_tGraphHeader.dwVersion			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwLevelCount			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwVertexCount		= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwEdgeCount			= m_tpGraphVFS->r_u32();
	m_tGraphHeader.dwDeathPointCount	= m_tpGraphVFS->r_u32();
	m_tGraphHeader.tpLevels.clear		();
	{
		for (u32 i=0; i<m_tGraphHeader.dwLevelCount; i++) {
			ALife::SLevel				l_tLevel;
			m_tpGraphVFS->r_stringZ		(l_tLevel.caLevelName);
			m_tpGraphVFS->r_fvector3	(l_tLevel.tOffset);
			m_tpGraphVFS->r				(&l_tLevel.tLevelID,sizeof(l_tLevel.tLevelID));
			m_tGraphHeader.tpLevels.insert(mk_pair(l_tLevel.tLevelID,l_tLevel));
		}
	}
	R_ASSERT2							(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION,"Graph version mismatch!");
	m_tpaGraph							= (SGraphVertex*)m_tpGraphVFS->pointer();
};
