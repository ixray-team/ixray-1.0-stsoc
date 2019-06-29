////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Data storages
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename	_dist_type				= float, 
	typename	_index_type				= u32, 
	typename	_path_id_type			= u32, 
	bool		bEuclidianHeuristics	= true, 
	u8			index_bits				= 24, 
	u8			mask_bits				= 8
>	class CDataStorageBinaryHeap
{
	#pragma pack(push,4)
	struct SGraphNode {
		_index_type		open_close_mask	: mask_bits;
		_index_type		_index			: index_bits;
		_dist_type		_f;
		_dist_type		_g;
		_dist_type		_h;
		SGraphNode		*back;

		IC	_index_type	index() const
		{
			return				(_index);
		}

		IC	_dist_type	&f()
		{
			return				(_f);
		}

		IC	_dist_type	&g()
		{
			return				(_g);
		}

		IC	_dist_type	&h()
		{
			return				(_h);
		}
	};

	struct SGraphIndexNode {
		SGraphNode		*node;
		_path_id_type		path_id;
	};
#pragma pack(pop)

	class CGraphNodePredicate {
	public:
		IC			bool	operator()(SGraphNode *node1, SGraphNode *node2)
		{
			return				(node1->f() > node2->f());
		};
	};

	_path_id_type		cur_path_id;
	_index_type			node_count;
	_index_type			max_node_count;
	SGraphNode			*nodes;
	SGraphIndexNode		*indexes;
	SGraphNode			**heap;
	SGraphNode			**heap_head;
	SGraphNode			**heap_tail;

public:
	typedef SGraphNode CGraphNode;

						CDataStorageBinaryHeap(const _index_type node_count)
	{
		cur_path_id				= _path_id_type(0);
		max_node_count			= node_count;
		
		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count)*sizeof(CGraphNode);
		nodes					= (CGraphNode*)xr_malloc(byte_count);
		ZeroMemory				(nodes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(SGraphIndexNode);
		indexes					= (SGraphIndexNode*)xr_malloc(byte_count);
		ZeroMemory				(indexes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(CGraphNode*);
		heap					= (CGraphNode**)xr_malloc(byte_count);
		ZeroMemory				(heap,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageBinaryHeap()
	{
		xr_free					(nodes);
		xr_free					(indexes);
		xr_free					(heap);
	}

	IC		void		init			()
	{
		cur_path_id++;
		node_count				= _index_type(0);
		heap_head				= heap_tail = heap;
	}

	IC		bool		is_opened_empty	() const
	{
		VERIFY					(heap_head <= heap_tail);
		return					(heap_head == heap_tail);
	}

	IC		bool		is_opened		(const CGraphNode &node) const
	{
		return					(!!node.open_close_mask);
	}

	IC		bool		is_visited(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		return					(indexes[node_index].path_id == cur_path_id);
	}

	IC		bool		is_closed		(const CGraphNode &node) const
	{
		return					(is_visited(node) && !is_opened(node));
	}

	IC		CGraphNode	&create_node	(const _index_type node_index)
	{
		VERIFY					(node_index < max_node_count);
		CGraphNode				*node = indexes[node_index].node = nodes + node_count++;
		indexes[node_index].path_id = cur_path_id;
		node->_index			= node_index;
		return					(*node);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		VERIFY					(heap_head <= heap_tail);
		node.open_close_mask	= 1;
		*heap_tail				= &node;
		std::push_heap			(heap_head,++heap_tail,CGraphNodePredicate());
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());
		for (CGraphNode **i = heap_head; *i != &node; ++i);
		std::push_heap			(heap_head,i + 1,CGraphNodePredicate());
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		std::pop_heap			(heap_head,heap_tail--,CGraphNodePredicate());
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		(*heap_head)->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(**heap_head);
	}

	IC		CGraphNode	&get_node		(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}

	IC		void		assign_parent	(CGraphNode	&neighbour, CGraphNode *parent)
	{
		neighbour.back			= parent;
	}

	IC		const _index_type	get_visited_node_count() const
	{
		return					(node_count);
	}
	
	IC		void		update_successors(CGraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		CGraphNode				*best = &get_best(), *t1 = best, *t2 = t1->back;
		for (_index_type i=1; t2; t1 = t2, t2 = t2->back, i++) ;

		path.resize				(i);

		t1						= best;
		path[--i]				= best->index();
		t2						= t1->back;

		xr_vector<_index_type>::reverse_iterator	I = path.rbegin();
		xr_vector<_index_type>::reverse_iterator	E = path.rend();
		for (++I ; t2; t2 = t2->back, ++I)
			*I = t2->index();
	}
};

template <
	typename	_dist_type,
	typename	_index_type,
	typename	_path_id_type,
	bool		bEuclidianHeuristics,
	u8			index_bits,
	u8			mask_bits
>	class CDataStorageDLSL
{
	#pragma pack(push,4)
	struct SGraphNode {
		_index_type		open_close_mask	: mask_bits;
		_index_type		_index			: index_bits;
		_dist_type		_f;
		_dist_type		_g;
		_dist_type		_h;
		SGraphNode		*back;
		SGraphNode		*next;
		SGraphNode		*prev;

		IC	_index_type	index() const
		{
			return				(_index);
		}

		IC	_dist_type	&f()
		{
			return				(_f);
		}

		IC	_dist_type	&g()
		{
			return				(_g);
		}

		IC	_dist_type	&h()
		{
			return				(_h);
		}
	};

	struct SGraphIndexNode {
		SGraphNode		*node;
		_path_id_type	path_id;
	};
#pragma pack(pop)

	_path_id_type		cur_path_id;
	_index_type			node_count;
	_index_type			max_node_count;
	_dist_type			max_distance;
	SGraphNode			*nodes;
	SGraphIndexNode		*indexes;
	SGraphNode			*list_head;
	SGraphNode			*list_tail;

public:
	typedef SGraphNode CGraphNode;

						CDataStorageDLSL(const _index_type node_count, const _dist_type _max_distance = _dist_type(u32(-1)))
	{
		max_node_count			= node_count;
		max_distance			= _max_distance;
		
		cur_path_id				= _path_id_type(0);

		u32						memory_usage = 0;
		u32						byte_count;
		
		byte_count				= (node_count + 2)*sizeof(CGraphNode);
		nodes					= (CGraphNode*)xr_malloc(byte_count);
		ZeroMemory				(nodes,byte_count);
		memory_usage			+= byte_count;

		byte_count				= (node_count)*sizeof(SGraphIndexNode);
		indexes					= (SGraphIndexNode*)xr_malloc(byte_count);
		ZeroMemory				(indexes,byte_count);
		memory_usage			+= byte_count;

		Msg						("* Data storage allocated %d bytes of memory",memory_usage);
	}

	virtual				~CDataStorageDLSL()
	{
		xr_free					(nodes);
		xr_free					(indexes);
	}

	IC		void		init			()
	{
		cur_path_id++;
		node_count				= _index_type(0);
		ZeroMemory				(nodes,2*sizeof(CGraphNode));
		list_head				= nodes + node_count++;
		list_tail				= nodes + node_count++;
		list_head->next			= list_tail;
		list_tail->prev			= list_head;
		list_tail->f()			= max_distance;
	}

	IC		bool		is_opened_empty	() const
	{
		return					(list_head->next == list_tail);
	}

	IC		bool		is_opened		(const CGraphNode &node) const
	{
		return					(!!node.open_close_mask);
	}

	IC		bool		is_visited(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		return					(indexes[node_index].path_id == cur_path_id);
	}

	IC		bool		is_closed		(const CGraphNode &node) const
	{
		return					(is_visited(node) && !is_opened(node));
	}

	IC		CGraphNode	&create_node	(const _index_type node_index)
	{
		VERIFY					(node_index < max_node_count);
		CGraphNode				*node = indexes[node_index].node = nodes + node_count++;
		indexes[node_index].path_id = cur_path_id;
		node->_index			= node_index;
		return					(*node);
	}

	IC		void		add_opened		(CGraphNode &node)
	{
		node.open_close_mask	= 1;
		for (CGraphNode *i = list_head->next; ; i = i->next)
			if (i->f() >= node.f()) {
				node.next		= i;
				node.prev		= i->prev;
				i->prev->next	= &node;
				i->prev			= &node;
				break;
			}
	}

	IC		void		decrease_opened	(CGraphNode &node, const _dist_type value)
	{
		VERIFY					(!is_opened_empty());

		if (node.prev->f() <= node.f())
			return;

		node.prev->next			= node.next;
		node.next->prev			= node.prev;

		if (node.f() <= list_head->next->f()) {
			node.prev			= list_head;
			node.next			= list_head->next;
			list_head->next->prev = &node;
			list_head->next		= &node;
		}
		else {
			if (node.prev->f() - node.f() < node.f() - list_head->next->f())
				for (CGraphNode *i = node.prev->prev; ; i = i->prev) {
					if (i->f() <= node.f()) {
						node.next = i->next;
						node.prev = i;
						i->next->prev = &node;
						i->next = &node;
						break;
					}
				}
			else
				for (CGraphNode *i = list_head->next; ; i = i->next)
					if (i->f() >= node.f()) {
						node.next = i;
						node.prev = i->prev;
						i->prev->next = &node;
						i->prev = &node;
						break;
					}
		}
	}

	IC		void		remove_best_opened	()
	{
		VERIFY					(!is_opened_empty());
		list_head->next->next->prev	= list_head;
		list_head->next			= list_head->next->next;
	}

	IC		void		add_best_closed		()
	{
		VERIFY					(!is_opened_empty());
		list_head->next->open_close_mask = 0;
	}

	IC		CGraphNode	&get_best		() const
	{
		VERIFY					(!is_opened_empty());
		return					(*list_head->next);
	}

	IC		CGraphNode	&get_node		(const _index_type node_index) const
	{
		VERIFY					(node_index < max_node_count);
		VERIFY					(is_visited(node_index));
		return					(*indexes[node_index].node);
	}

	IC		void		assign_parent	(CGraphNode	&neighbour, CGraphNode *parent)
	{
		neighbour.back			= parent;
	}

	IC		const _index_type	get_visited_node_count() const
	{
		return					(node_count);
	}
	
	IC		void		update_successors(CGraphNode &tpNeighbour)
	{
		VERIFY					(false);
	}

	IC		void		get_path		(xr_vector<_index_type> &path)
	{
		VERIFY					(!is_opened_empty());
		CGraphNode				*best = &get_best(), *t1 = best, *t2 = t1->back;
		for (_index_type i=1; t2; t1 = t2, t2 = t2->back, i++) ;

		path.resize				(i);

		t1						= best;
		path[--i]				= best->index();
		t2						= t1->back;

		xr_vector<_index_type>::reverse_iterator	I = path.rbegin();
		xr_vector<_index_type>::reverse_iterator	E = path.rend();
		for (++I ; t2; t2 = t2->back, ++I)
			*I = t2->index();
	}
};