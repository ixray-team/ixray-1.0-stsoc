////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph.h
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"

namespace LevelNavigationGraph {
#ifdef AI_COMPILER
	struct CCellVertexEx;
#endif
	struct CCellVertex;
	class  CSector;
};

template <typename, typename, typename> class CGraphAbstractSerialize;

class CLevelNavigationGraph : public CLevelGraph {
private:
	typedef CLevelGraph									inherited;

public:
	typedef LevelNavigationGraph::CSector				CSector;
	typedef CGraphAbstractSerialize<CSector,u32,u32>	CSectorGraph;

public:
	typedef LevelNavigationGraph::CCellVertex			CCellVertex;
	typedef xr_vector<u16>								CROSS_TABLE;

// temporary structures for building sector hierarchy
private:
	CSectorGraph				*m_sectors;
	CROSS_TABLE					m_cross;
	xrGUID						m_level_guid;

// temporary structures for building sector hierarchy
public:
	typedef xr_vector<CCellVertex>						_CROSS_TABLE;
	typedef xr_map<u64,CCellVertex*,std::greater<u64> >	_CROSS_PTABLE;

private:
	_CROSS_TABLE				m_temp_cross;
	_CROSS_PTABLE				m_temp;
	
#ifdef DEBUG
private:
	u32							m_global_count;

protected:
	IC		void				check_vertices			();
	IC		void				check_edges				();

public:
			void				render					();
#endif

public:
	using CLevelGraph::vertex_id;

#ifndef AI_COMPILER
								CLevelNavigationGraph	();
#else
								CLevelNavigationGraph	(LPCSTR file_name);
#endif
	virtual						~CLevelNavigationGraph	();

protected:
	IC		u32					vertex_id				(const CCellVertex &vertex) const;
	IC		u32					vertex_id				(const CCellVertex *vertex) const;
	IC		u32					vertex_id				(const _CROSS_TABLE::const_iterator &vertex) const;
	IC		u32					vertex_id				(const _CROSS_PTABLE::const_iterator &vertex) const;
	ICF		u64					cell_id					(CCellVertex *cell) const;
	ICF		bool				check_left				(u32 vertex_id, u32 left_vertex_id) const;
	IC		void				fill_cell				(u32 start_vertex_id, u32 link);
	IC		void				fill_cell				(u32 start_vertex_id);
	IC		void				fill_cells				();
	IC		void				fast_sort				();
	IC		void				update_cell				(u32 start_vertex_id, u32 link);
	IC		void				update_cells			(u32 vertex_id, u32 right, u32 down);
	IC		void				select_sector			(CCellVertex *v, u32 &right, u32 &down, u32 max_square);
	IC		void				select_sector			(u32 &vertex_id, u32 &right, u32 &down);
	IC		void				build_sector			(u32 vertex_id, u32 right, u32 down, u32 group_id);
	IC		void				generate_sectors		();
	IC		void				generate_edges			();
			void				generate_cross			();
			bool				valid					(LPCSTR file_name);
			void				generate				(LPCSTR file_name);

public:
	IC		const CSectorGraph	&sectors				() const;

protected:
	IC		CSectorGraph		&sectors				();
};

#include "level_navigation_graph_inline.h"