////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph.cpp
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_navigation_graph.h"
#include "level_navigation_graph_space.h"
#include "graph_abstract.h"

typedef LevelNavigationGraph::CCellVertex::_use_type _use_type;

const _use_type up		= 1 << 0;
const _use_type right	= 1 << 1;
const _use_type down	= 1 << 2;
const _use_type left	= 1 << 3;

#ifdef AI_COMPILER
CLevelNavigationGraph::CLevelNavigationGraph	(LPCSTR file_name) : inherited(file_name)
#else
CLevelNavigationGraph::CLevelNavigationGraph	()
#endif
{
#ifndef AI_COMPILER
	LPCSTR			file_name = "";
#endif
	m_sectors		= 0;
	generate		(file_name);
	Msg				("* Navigation info : %d nodes, %d sectors, %d edges",header().vertex_count(),sectors().vertex_count(),sectors().edge_count());
}

CLevelNavigationGraph::~CLevelNavigationGraph	()
{
	xr_delete		(m_sectors);
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const CCellVertex &vertex) const
{
	return			(u32(&vertex - &*m_temp_cross.begin()));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const CCellVertex *vertex) const
{
	return			(vertex_id(*vertex));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const _CROSS_TABLE::const_iterator &vertex) const
{
	return			(vertex_id(*vertex));
}

ICF	u32	CLevelNavigationGraph::vertex_id		(const _CROSS_PTABLE::const_iterator &vertex) const
{
	return			(vertex_id((*vertex).second));
}

ICF	bool CLevelNavigationGraph::check_left		(u32 vertex_id, u32 left_vertex_id) const
{
	u32				link = vertex(vertex_id)->link(3);
	return			((left_vertex_id == link) && valid_vertex_id(link) && (vertex(link)->link(1) == vertex_id));
}

ICF	u64	CLevelNavigationGraph::cell_id			(CCellVertex *cell) const
{
	return((u64(u32(cell->m_right)*u32(cell->m_down)) << 32) | u64((void*)cell));
}

IC	void CLevelNavigationGraph::fill_cell		(u32 start_vertex_id, u32 link)
{
	for (u32 current_vertex_id = start_vertex_id, i = 1; ;++i) {
		u32					vertex_id = vertex(current_vertex_id)->link(link);
		if (!valid_vertex_id(vertex_id))
			break;

		if (link == 1)
			m_temp_cross[current_vertex_id].m_right_next = &m_temp_cross[vertex_id];

		if (vertex(vertex_id)->link((link + 2) & 3) != current_vertex_id)
			break;

		current_vertex_id	= vertex_id;
	}

	current_vertex_id		= start_vertex_id;
	for (u32 j=0; j<i; ++j) {
		m_temp_cross[current_vertex_id].m_dirs[link-1] = u16(i-j);
		current_vertex_id	= vertex(current_vertex_id)->link(link);
	}
}

IC	void CLevelNavigationGraph::fill_cell		(u32 start_vertex_id)
{
	u32							left_current_vertex_id = vertex(start_vertex_id)->link(3);
	if (!valid_vertex_id(left_current_vertex_id) || (vertex(left_current_vertex_id)->link(1) != start_vertex_id)) {
		m_temp_cross[start_vertex_id].m_down_left = 1;
		return;
	}

	u32							current_vertex_id = start_vertex_id;
	u32							n = m_temp_cross[start_vertex_id].m_down;
	u32							i = 1;
	for ( ; i<n; ++i) {
		u32						vertex_id = vertex(current_vertex_id)->link(2);
		u32						left_vertex_id = vertex(left_current_vertex_id)->link(2);
		if (!check_left(vertex_id,left_vertex_id))
			break;
		current_vertex_id		= vertex_id;
		left_current_vertex_id	= left_vertex_id;
	}

	current_vertex_id			= start_vertex_id;
	for (u32 j=0; j<i; ++j) {
		m_temp_cross[current_vertex_id].m_down_left = u16(i-j);
		current_vertex_id		= vertex(current_vertex_id)->link(2);
	}
}

IC	void CLevelNavigationGraph::fill_cells		()
{
	m_temp.clear			();

	_CROSS_TABLE::iterator	I = m_temp_cross.begin();
	_CROSS_TABLE::iterator	E = m_temp_cross.end();
	for ( ; I != E; ++I) {
		if (!(*I).m_right)
			fill_cell		(vertex_id(I),1);

		if (!(*I).m_down)
			fill_cell		(vertex_id(I),2);

		if (!(*I).m_down_left)
			fill_cell		(vertex_id(I));

		m_temp.insert		(std::make_pair(cell_id(&*I),&*I));
	}
}

IC	void CLevelNavigationGraph::update_cell		(u32 start_vertex_id, u32 link)
{
	for (u32 current_vertex_id = start_vertex_id, i = 1, index = (link + 1) & 3; ;++i) {
		u32					vertex_id = vertex(current_vertex_id)->link(link);
		if (!valid_vertex_id(vertex_id))
			break;

		if (m_temp_cross[vertex_id].m_mark)
			break;

		if (vertex(vertex_id)->link((link + 2) & 3) != current_vertex_id)
			break;

		CCellVertex			*cell = &m_temp_cross[vertex_id];

		m_temp.erase		(cell_id(cell));

		cell->m_dirs[index]	= (u16)i;

		m_temp.insert		(std::make_pair(cell_id(cell),cell));

		if (!link)
			cell->m_down_left = _min(cell->m_down,cell->m_down_left);
		current_vertex_id	= vertex_id;
	}
}

IC	void CLevelNavigationGraph::update_cells	(u32 vertex_id, u32 right, u32 down)
{
	for (u32 right_id = vertex_id, i=0; i<right; right_id = vertex(right_id)->link(1), ++i)
		update_cell			(right_id,0);

	for (u32 down_id = vertex_id, j=0; j<down; down_id = vertex(down_id)->link(2), ++j)
		update_cell			(down_id,3);
}

IC	void CLevelNavigationGraph::select_sector	(CCellVertex *v, u32 &right, u32 &down, u32 max_square)
{
	right					= 1;
	down					= v->m_down;
	u32						current_down = v->m_down;
	CCellVertex				*right_id = v->m_right_next;
	u32						i = 2;
	for	( ; i<=v->m_right; right_id = right_id->m_right_next, ++i) {
		VERIFY				(right_id);
		VERIFY				(!right_id->m_mark);

		current_down		= (u32)_min((s32)current_down,(s32)right_id->m_down_left);
		
		if (current_down*v->m_right <= max_square)
			return;

		if (i*current_down > max_square) {
			max_square		= i*current_down;
			right			= i;
			down			= current_down;
			continue;
		}
	}
}

IC	void CLevelNavigationGraph::select_sector	(u32 &vertex_id, u32 &right, u32 &down)
{
	u32						max_square = 0, current_right, current_down;
	_CROSS_PTABLE::iterator	I = m_temp.begin();
	_CROSS_PTABLE::iterator	E = m_temp.end();

	select_sector			((*I).second,current_right,current_down,max_square);
	right					= current_right;
	down					= current_down;
	max_square				= right*down;
	vertex_id				= this->vertex_id(I);

	for (++I; I != E; ++I) {
		if (u32((*I).first >> 32) <= max_square)
			return;

		select_sector		((*I).second,current_right,current_down,max_square);

		if (current_right*current_down <= max_square)
			continue;

		right				= current_right;
		down				= current_down;
		max_square			= right*down;
		vertex_id			= this->vertex_id(I);
	}
}

IC	void CLevelNavigationGraph::build_sector	(u32 vertex_id, u32 _right, u32 _down, u32 group_id)
{
	CCellVertex					*cell = 0, *right_id = &m_temp_cross[vertex_id];
	u32							j, down_id, mask = left;
	for (u32 i=0; i<_right; right_id = right_id->m_right_next, ++i) {
		right_id->m_use			= mask | up;

		if (i == (_right-1))
			mask				|= right;
		
		for (down_id = this->vertex_id(right_id), j=0; j<_down; down_id = vertex(down_id)->link(2), ++j) {
			cell				= &m_temp_cross[down_id];

			m_temp.erase		(cell_id(cell));
			VERIFY				(!cell->m_mark);
			cell->m_mark		= group_id;
			cell->m_use			|= mask;
			if (j == (_down-1)) {
				cell->m_use		|= down;
				break;
			}
		}

		mask					= 0;
	}

	sectors().add_vertex		(CSector(vertex_id,down_id),group_id - 1);

#ifdef DEBUG
	m_global_count				+= _right*_down;
#endif
}

IC	void CLevelNavigationGraph::generate_sectors()
{
	xr_delete				(m_sectors);
	m_sectors				= xr_new<CSectorGraph>();
#ifdef DEBUG
	m_global_count			= 0;
#endif

	u32						group_id = 0;
	u32						id,right,down;
	
	m_temp_cross.resize		(header().vertex_count());

	fill_cells				();
	
#ifdef AI_COMPILER
#	ifdef DEBUG
		Phase				("generating sectors");
		Progress			(0.f);
#	endif
#endif
	while (!m_temp.empty()) {
		select_sector		(id,right,down);
		build_sector		(id,right,down,++group_id);
#ifdef AI_COMPILER
#	ifdef DEBUG
		Progress			((float(m_global_count)/float(header().vertex_count())));
#	endif
#endif
		update_cells		(id,right,down);
	}

#ifdef DEBUG
	check_vertices			();
#endif
}

IC	void CLevelNavigationGraph::generate_edges	()
{
	_CROSS_TABLE::const_iterator	_i = m_temp_cross.begin();
	const_vertex_iterator		i = begin();
	const_vertex_iterator		e = end();
	for ( ; i != e; ++i, ++_i) {
		const CCellVertex		*current_cell = &*_i;
		VERIFY					(current_cell);

		if (!current_cell->m_use)
			continue;

		u32						current_mark = current_cell->m_mark - 1;
		CSectorGraph::CVertex	*sector_vertex = sectors().vertex(current_mark);
		u32						usage = current_cell->m_use;
		u32						I;
		do {
			I					= usage;
			usage				&= usage - 1;
			I					^= usage;
			I					= (I >> 1) + 1;
			I					= (I ^ (I >> 2)) - 1;

			u32					vertex_id = (*i).link(I);
			if (!valid_vertex_id(vertex_id))
				continue;

			VERIFY2				(vertex_id != this->vertex_id(i),"AI map is CORRUPTED! REGENERATE IT");

			CCellVertex			*cell = &m_temp_cross[vertex_id];
			VERIFY				(cell);

			u32					mark = cell->m_mark - 1;
			VERIFY				(mark != current_mark);

			if (sector_vertex->edge(mark))
				continue;

			CSectorGraph::CVertex	*sector_vertex1 = sectors().vertex(mark);

			const CPosition		&min0 = vertex(sector_vertex->data().min_vertex_id())->position();
			const CPosition		&max0 = vertex(sector_vertex->data().max_vertex_id())->position();
			const CPosition		&min1 = vertex(sector_vertex1->data().min_vertex_id())->position();
			const CPosition		&max1 = vertex(sector_vertex1->data().max_vertex_id())->position();

			int					x0	 = (min0.x(row_length()) + max0.x(row_length())) >> 1;
			int					z0	 = (min0.z(row_length()) + max0.z(row_length())) >> 1;
			int					x1	 = (min1.x(row_length()) + max1.x(row_length())) >> 1;
			int					z1	 = (min1.z(row_length()) + max1.z(row_length())) >> 1;

			u32					edge_weight = _abs(x1 - x0) + _abs(z1 - z0);

			sectors().add_edge	(current_mark,mark,edge_weight);
		}
		while (usage);
	}

#ifdef DEBUG
	check_edges					();
#endif

	m_temp_cross.clear			();
}

#ifdef DEBUG
IC	void CLevelNavigationGraph::check_vertices	()
{
	VERIFY							(m_global_count == header().vertex_count());
	_CROSS_TABLE::const_iterator	I = m_temp_cross.begin();
	_CROSS_TABLE::const_iterator	E = m_temp_cross.end();
	for ( ; I != E; ++I)
		VERIFY						((*I).m_mark);
}

IC	void CLevelNavigationGraph::check_edges		()
{
	u32									count = 0;
	CSectorGraph::const_vertex_iterator	I = sectors().vertices().begin();
	CSectorGraph::const_vertex_iterator	E = sectors().vertices().end();
	for ( ; I != E; ++I) {
		if (!(*I).second->edges().empty())
			continue;

		++count;
		if ((*I).second->data().min_vertex_id() == (*I).second->data().max_vertex_id())
			Msg							("! Node %d is not connected to the graph!",(*I).second->data().min_vertex_id());
		else
			Msg							("! Sector [%d][%d] is not connected to the graph!",(*I).second->data().min_vertex_id(),(*I).second->data().max_vertex_id());
	}
	if (count) {
		if (count == 1)
			Msg							("! There is single not connected island");
		else
			Msg							("! There are %d not connected islands",count);
	}
}
#endif

void CLevelNavigationGraph::generate_cross	()
{
}

bool CLevelNavigationGraph::valid			(LPCSTR file_name)
{
	if (!FS.exist(file_name)) {
		Msg			("! Sector graph doesn't exist : building...");
		return		(false);
	}

	IReader			*reader = FS.r_open(file_name);
	R_ASSERT		(reader);
	IReader			*chunk = reader->open_chunk(0);
	if (!chunk) {
		Msg			("! Sector graph is invalid : rebuilding...");
		return		(false);
	}

	chunk->r		(&m_level_guid,sizeof(m_level_guid));
	chunk->close	();
	if (m_level_guid != header().guid()) {
		FS.r_close	(reader);
		Msg			("! Sector graph doesn't correspond to the level graph : rebuilding...");
		return		(false);
	}

	chunk			= reader->open_chunk(1);
	if (!chunk) {
		Msg			("! Sector graph is invalid : rebuilding...");
		return		(false);
	}

	load_data		(m_sectors,*chunk);
	chunk->close	();
	
	FS.r_close		(reader);
	return			(true);
}

void CLevelNavigationGraph::generate	(LPCSTR filename)
{
#ifndef AI_COMPILER
	string256			file_name;
	FS.update_path		(file_name,"$level$","level_sectors.ai");
#	ifndef DEBUG
		R_ASSERT		(valid(file_name));
#	endif
#else
	string256			file_name;
	strconcat			(file_name,filename,"level_sectors.ai");
#endif
	
	if (valid(file_name))
		return;

	generate_sectors	();
	generate_edges		();
	generate_cross		();
	
	CMemoryWriter		m;
	
	m.open_chunk		(0);
	save_data			(m_level_guid = header().guid(),m);
	m.close_chunk		();

	m.open_chunk		(1);
	save_data			(sectors(),m);
	m.close_chunk		();

	m.save_to			(file_name);
}
