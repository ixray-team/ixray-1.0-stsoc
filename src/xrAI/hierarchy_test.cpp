#include "stdafx.h"
#include "level_graph.h"
#include "level_navigation_graph_space.h"
#include "profile.h"
#include "graph_abstract.h"

typedef LevelNavigationGraph::CSector				CSector;
typedef LevelNavigationGraph::CCellVertexEx			CCellVertex;
typedef CCellVertex::_use_type						_use_type;

const _use_type left	= 1 << 0;
const _use_type up		= 1 << 3;
const _use_type right	= 1 << 2;
const _use_type down	= 1 << 1;

typedef CGraphAbstractSerialize<CSector,u32,u32>	CSectorGraph;
typedef xr_vector<CCellVertex>						VERTEX_VECTOR;
typedef xr_vector<VERTEX_VECTOR>					VERTEX_VECTOR1;
typedef xr_vector<VERTEX_VECTOR1>					VERTEX_VECTOR2;
typedef xr_vector<CCellVertex*>						CROSS_VECTOR;

IC	CCellVertex &get_vertex_by_group_id(VERTEX_VECTOR &vertices, u32 group_id)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id)
			return			(*I);

	NODEFAULT;
#ifdef DEBUG
	static CCellVertex		last_result(u32(-1),0,0);
	return					(last_result);
#endif
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex, VERTEX_VECTOR &vertices, u32 group_id, u32 link, CROSS_VECTOR &cross, u32 use)
{
	u32						_link = level_graph.vertex(vertex.m_vertex_id)->link(link);
//	if (!level_graph.valid_vertex_id(_link))
//		return				(false);

	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (_link == (*I).m_vertex_id) {
			if ((*I).m_mark)
				return		(false);

			(*I).m_mark		= group_id;
			(*I).m_use		|= use;
			cross[_link]	= &*I;
			vertex			= *I;
			return			(true);
		}
	return					(false);
}

IC	bool connect(const CLevelGraph &level_graph, CCellVertex &vertex1, CCellVertex &vertex2, VERTEX_VECTOR &vertices, u32 group_id, CROSS_VECTOR &cross, u32 use)
{
	u32						link1 = level_graph.vertex(vertex1.m_vertex_id)->link(2);
//	if (!level_graph.valid_vertex_id(link1))
//		return				(false);

	u32						link2 = level_graph.vertex(vertex2.m_vertex_id)->link(1);
	if (link1 != link2)
		return				(false);

	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if (link1 == (*I).m_vertex_id) {
			if ((*I).m_mark)
				return		(false);

			(*I).m_mark		= group_id;
			(*I).m_use		|= use;
			cross[link2]	= &*I;
			return			(true);
		}
	return					(false);
}

IC	void remove_mark(VERTEX_VECTOR &vertices, u32 group_id, CROSS_VECTOR &cross)
{
	VERTEX_VECTOR::iterator	I = vertices.begin();
	VERTEX_VECTOR::iterator	E = vertices.end();
	for ( ; I != E; ++I)
		if ((*I).m_mark == group_id) {
			(*I).m_data				= 0;
			cross[(*I).m_vertex_id]	= 0;
			return;
		}
	NODEFAULT;
}

u32 global_count = 0;

IC	void fill_mark(
	const CLevelGraph &level_graph, 
	CSectorGraph &sector_graph,
	VERTEX_VECTOR2 &table, 
	u32 i, 
	u32 j, 
	CCellVertex &cell_vertex, 
	u32 &group_id, 
	u32 min_z, 
	u32 max_z, 
	u32 min_x, 
	u32 max_x,
	CROSS_VECTOR &cross
)
{
	++group_id;
	cell_vertex.m_mark			= group_id;
	cell_vertex.m_use			= left | up;
	cross[cell_vertex.m_vertex_id]	= &cell_vertex;
	CCellVertex					v = cell_vertex, v1;

	VERTEX_VECTOR1				&vi = table[i];
	for (u32 j2 = j + 1; j2<=max_x; ++j2)
		if (vi[j2].empty() || !connect(level_graph,v,vi[j2],group_id,2,cross,up))
			break;

	CCellVertex					&_v = get_vertex_by_group_id(vi[j2-1],group_id);
	_v.m_use					|= right;
	v							= _v;
	bool						ok = true;

	VERTEX_VECTOR1::iterator	_j2, j1, j1_1, i1_1_j1;
	VERTEX_VECTOR2::iterator	i1 = table.begin() + i + 1, i1_1 = i1 - 1;
	VERTEX_VECTOR2::iterator	e = table.end();
	for ( ; i1 != e; ++i1, ++i1_1) {
		VERTEX_VECTOR			&table_i1_j = (*i1)[j];
		if (table_i1_j.empty())
			goto enough;

		v1						= get_vertex_by_group_id((*i1_1)[j],group_id);
		if (!connect(level_graph,v1,table_i1_j,group_id,1,cross,left))
			goto enough;
		
		
		j1						= (*i1).begin() + j + 1;
		j1_1					= j1 - 1;
		i1_1_j1					= (*i1_1).begin() + j + 1;
		_j2						= (*i1).begin() + j2;
		for ( ; j1 != _j2; ++j1, ++j1_1, ++i1_1_j1) {
			v					= get_vertex_by_group_id(*j1_1,group_id);
			v1					= get_vertex_by_group_id(*i1_1_j1,group_id);
			if ((*j1).empty() || !connect(level_graph,v,v1,*j1,group_id,cross,0)) {
				ok				= false;

				VERTEX_VECTOR1::iterator	J = (*i1).begin() + j;
				for ( ; J!=j1; ++J)
					remove_mark	(*J,group_id,cross);
				break;
			}
		}

		if (ok) {
			CCellVertex					&_v = get_vertex_by_group_id(*j1_1,group_id);
			_v.m_use					|= right;
			v							= _v;
			continue;
		}

enough:
		{
			VERTEX_VECTOR1::iterator		j1 = (*i1_1).begin() + j;
			VERTEX_VECTOR1::iterator		_j2 = (*i1_1).begin() + j2;
			for ( ; j1 != _j2; ++j1)
				get_vertex_by_group_id(*j1,group_id).m_use	|= down;
			break;
		}
	}
	
	sector_graph.add_vertex		(CSector(cell_vertex.m_vertex_id,get_vertex_by_group_id((*i1_1)[j2 - 1],group_id).m_vertex_id),group_id - 1);

	global_count				+= (u32(i1 - table.begin()) - i)*(j2 - j);
}

CTimer							timer;

IC	void build_convex_hierarchy(const CLevelGraph &level_graph, CSectorGraph &sector_graph)
{
	sector_graph.clear			();
	
	u32							max_z = 0;
	u32							max_x = 0;
	u32							min_z = u32(-1);
	u32							min_x = u32(-1);
	u32							n = level_graph.header().vertex_count();
	u32							r = level_graph.row_length();

	{
		u32									cur_x, cur_z;
		CLevelGraph::const_vertex_iterator	I = level_graph.begin();
		CLevelGraph::const_vertex_iterator	E = level_graph.end();
		for ( ; I != E; ++I) {
			const CLevelGraph::CPosition &position = (*I).position();
			cur_x					= position.x(r);
			cur_z					= position.z(r);
			if (cur_z > max_z)
				max_z				= cur_z;
			if (cur_x > max_x)
				max_x				= cur_x;
			if (cur_z < min_z)
				min_z				= cur_z;
			if (cur_x < min_x)
				min_x				= cur_x;
		}
	}
	
	Msg							("MinMax time %f",timer.GetElapsed_sec());

	// allocating memory
	VERTEX_VECTOR2				table;
	CROSS_VECTOR				cross;
	cross.assign				(n,0);
	{
		table.resize				(max_z - min_z + 1);
		u32							size_x = max_x - min_x + 1;
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z;
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I)
			(*I).resize				(size_x);
	}
	
	Msg							("Allocate time %f",timer.GetElapsed_sec());

	{
		u32									cur_x, cur_z;
		CCellVertex							v(0,0,0);
		CLevelGraph::const_vertex_iterator	I = level_graph.begin();
		CLevelGraph::const_vertex_iterator	E = level_graph.end();
		for ( ; I != E; ++I, ++v.m_vertex_id) {
			const CLevelGraph::CPosition	&position = (*I).position();
			cur_z							= position.z(r);
			cur_x							= position.x(r);
			table[cur_z][cur_x].push_back	(v);
		}
	}
	Msg							("Fill time %f",timer.GetElapsed_sec());

	u32							group_id = 0;
	{
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z, B = table.begin();
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I) {
			VERTEX_VECTOR1::iterator	i = (*I).begin() + min_x, b = (*I).begin();
			VERTEX_VECTOR1::iterator	e = (*I).end();
			for ( ; i != e; ++i) {
				VERTEX_VECTOR::iterator	II = (*i).begin();
				VERTEX_VECTOR::iterator	EE = (*i).end();
				for ( ; II != EE; ++II) {
					if ((*II).m_mark)
						continue;
					fill_mark		(level_graph,sector_graph,table,u32(I - B),u32(i - b),*II,group_id,min_z,max_z,min_x,max_x,cross);
				}
			}
		}
	}
	Msg							("Recursive fill time %f",timer.GetElapsed_sec());

	{
		VERTEX_VECTOR2::iterator	I = table.begin() + min_z, B = table.begin();
		VERTEX_VECTOR2::iterator	E = table.end();
		for ( ; I != E; ++I) {
			VERTEX_VECTOR1::iterator	i = (*I).begin() + min_x, b = (*I).begin();
			VERTEX_VECTOR1::iterator	e = (*I).end();
			for ( ; i != e; ++i) {
				VERTEX_VECTOR::iterator	II = (*i).begin();
				VERTEX_VECTOR::iterator	EE = (*i).end();
				for ( ; II != EE; ++II) {
					VERIFY			((*II).m_mark);
				}
			}
		}
	}
	Msg								("Check marks time %f",timer.GetElapsed_sec());

	Msg								("Group ID : %d (%d vertices)",group_id,sector_graph.vertex_count());

	{
		CLevelGraph::const_vertex_iterator	i = level_graph.begin(), b = i;
		CLevelGraph::const_vertex_iterator	e = level_graph.end();
		for ( ; i != e; ++i) {
			u32							current_vertex_id = u32(i - b);
			CCellVertex					*current_cell = cross[current_vertex_id];
			VERIFY						(current_cell);
			if (!current_cell->m_use)
				continue;
			u32							current_mark = current_cell->m_mark - 1;
			CSectorGraph::CVertex		*sector_vertex = sector_graph.vertex(current_mark);
			u32 usage					= current_cell->m_use, I;
			do {
				I						= usage;
				usage					&= usage - 1;
				I						^= usage;
				I						= (I >> 1) + 1;
				I						= (I ^ (I >> 2)) - 1;

				u32						vertex_id = (*i).link(I);
				if (!level_graph.valid_vertex_id(vertex_id))
					continue;

				CCellVertex				*cell = cross[vertex_id]; VERIFY(cell);
				u32						mark = cell->m_mark - 1;
				VERIFY					(mark != current_mark);

				if (sector_vertex->edge(mark))
					continue;

				sector_graph.add_edge	(current_mark,mark,1);
			}
			while (usage);
		}
	}
	Msg								("Fill edges time %f",timer.GetElapsed_sec());

	Msg								("Sector Graph : %d vertices, %d edges",sector_graph.vertex_count(),sector_graph.edge_count());

	{
		u32									count = 0;
		CSectorGraph::const_vertex_iterator	I = sector_graph.vertices().begin();
		CSectorGraph::const_vertex_iterator	E = sector_graph.vertices().end();
		for ( ; I != E; ++I) {
//			VERIFY							(!(*I).second->edges().empty());
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

	Msg								("Check edges time %f",timer.GetElapsed_sec());
}

#define TEST_COUNT 1

struct STravelPathPoint {
	Fvector				position;
	u32					vertex_id;
	u32					velocity;

	IC	void set_position	(const Fvector &pos)
	{
		position		= pos;
	}

	IC	void set_vertex_id	(const u32 _vertex_id)
	{
		vertex_id		= _vertex_id;
	}

	IC	Fvector &get_position	()
	{
		return			(position);
	}

	IC	u32		get_vertex_id	()
	{
		return			(vertex_id);
	}
};

#include "game_graph.h"

void test_game_graph	()
{
	string256			file_name;
	FS.update_path		(file_name,"$game_data$",GRAPH_NAME);
	CGameGraph			*graph = xr_new<CGameGraph>(file_name);

	const CGameGraph::CVertex	*I = graph->vertex(0), *B = I;
	const CGameGraph::CVertex	*E = graph->vertex(0) + graph->header().vertex_count();
	for ( ; I != E; ++I) {
		CGameGraph::const_iterator	i, e;
		u32							vertex_id = u32(I - B);
		graph->begin				(vertex_id,i,e);
		for ( ; i != e; ++i) {
			const CGameGraph::CVertex	*J = graph->vertex(graph->value(vertex_id,i));
			if ((*I).level_id() != (*J).level_id()) {
				Msg					(
					"Connection point %s[%f][%f][%f] -> %s[%f][%f][%f]",
					*graph->header().level((*I).level_id()).name(),
					VPUSH((*I).level_point()),
					*graph->header().level((*J).level_id()).name(),
					VPUSH((*J).level_point())
				);
			}
		}
	}

	xr_delete			(graph);
}

void test_hierarchy		(LPCSTR name)
{
	test_game_graph				();

	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);

	///
	///
	///
#if 0
	{
		STravelPathPoint			t;
		u32							vertex_id = 97666;
		Fvector2					start = Fvector2().set(-235.900452f,60.900448f);
		Fvector2					dest  = Fvector2().set(-236.249985f,61.250000f);;
		xr_vector<STravelPathPoint>	path;
		bool						value = level_graph->create_straight_path<false>(
			vertex_id,
			start,
			dest,
			path,
			t,
			false,
			false
		);
	}
#endif
	///
	///
	///

	CSectorGraph				*sector_graph = xr_new<CSectorGraph>();

	Msg							("ai map : %d nodes",level_graph->header().vertex_count());

#if 0
	SetPriorityClass			(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep						(1);
#endif

	timer.Start					();
	for (u32 i=0; i<TEST_COUNT; ++i) {
		build_convex_hierarchy	(*level_graph,*sector_graph);
		Msg						("Destroy time %f",timer.GetElapsed_sec());
	}
	Msg							("Total time %f (%d test(s) : %f)",timer.GetElapsed_sec(),TEST_COUNT,1000.f*timer.GetElapsed_ms()/float(TEST_COUNT));

	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);

	Msg							("Graphs are %s",equal(*sector_graph,((const CLevelGraph*)level_graph)->sectors()) ? "EQUAL" : "NOT EQUAL");

#if 1
	CMemoryWriter				stream;
	save_data					(sector_graph,stream);
	stream.save_to				("x:\\sector_graph.dat");
	Msg							("Save time %f",timer.GetElapsed_sec());

	CSectorGraph				*test0;
#if 0
	CSectorGraph				*test1;
#endif
	
	{
		IReader					*reader = FS.r_open("x:\\sector_graph.dat");
		load_data				(test0,*reader);
		FS.r_close				(reader);
	}
	Msg							("Load1 time %f",timer.GetElapsed_sec());

	{
		IReader					*reader = FS.r_open("x:\\sector_graph.dat");
		load_data				(test0,*reader);
		FS.r_close				(reader);
	}
	Msg							("Load1 cached time %f",timer.GetElapsed_sec());

#if 0
	{
		IReader					*reader = FS.r_open("x:\\sector_graph.dat.save");
		load_data				(test1,*reader);
		FS.r_close				(reader);
	}
	Msg							("Load2 time %f",timer.GetElapsed_sec());

	{
		IReader					*reader = FS.r_open("x:\\sector_graph.dat.save");
		load_data				(test1,*reader);
		FS.r_close				(reader);
	}
	Msg							("Load2 cached time %f",timer.GetElapsed_sec());
#endif

	Msg							("sector_graph and loaded graph are %s",equal(sector_graph,test0) ? "EQUAL" : "NOT EQUAL");
	Msg							("Compare1 time %f",timer.GetElapsed_sec());

#if 0
	Msg							("sector_graph and old loaded graph are %s",equal(sector_graph,test1) ? "EQUAL" : "NOT EQUAL");
	Msg							("Compare2 time %f",timer.GetElapsed_sec());
	
	Msg							("new loaded graph and old loaded graph are %s",equal(test0,test1) ? "EQUAL" : "NOT EQUAL");
	Msg							("Compare3 time %f",timer.GetElapsed_sec());
#endif
	
	xr_delete					(test0);
	Msg							("Destroy1 time %f",timer.GetElapsed_sec());
	
#if 0
	xr_delete					(test1);
	Msg							("Destroy2 time %f",timer.GetElapsed_sec());
#endif
#endif
	
	xr_delete					(level_graph);
	Msg							("Destroy level graph time %f",timer.GetElapsed_sec());
	
	xr_delete					(sector_graph);
	Msg							("Destroy sector graph time %f",timer.GetElapsed_sec());

	xr_delete					(level_graph);

	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass			(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
}
