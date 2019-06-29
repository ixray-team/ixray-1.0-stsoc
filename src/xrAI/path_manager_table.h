////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_table.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Table path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "test_table.h"

template <
	typename _cell_type,
	u32		 rows,
	u32		 columns,
	typename _DataStorage,
	typename _Parameters,
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>	class CPathManager <
		CTestTable<_cell_type,rows,columns>,
		_DataStorage,
		_Parameters,
		_dist_type,
		_index_type,
		_iteration_type
	> : public CPathManagerGeneric <
			CTestTable<_cell_type,rows,columns>,
			_DataStorage,
			_Parameters,
			_dist_type,
			_index_type,
			_iteration_type
		>
{
protected:
	typedef CTestTable<
		_cell_type,
		rows,
		columns
	>					_Graph;
	typedef typename CPathManagerGeneric <
		_Graph,
		_DataStorage,
		_Parameters,
		_dist_type,
		_index_type,
		_iteration_type
	>					inherited;

protected:
	int					_i,_j;
	_index_type			m_best_node_index;

public:
	virtual				~CPathManager		();
	IC		void		begin				(const _index_type &vertex_id, const_iterator &begin, const_iterator &end);
	IC		void		setup				(const _Graph *graph, _DataStorage *_data_storage, xr_vector<_index_type> *_path, const _index_type	&_start_node_index, const _index_type &_goal_node_index, const _Parameters &params);
	IC		_dist_type	evaluate			(const _index_type &node_index1, const _index_type &node_index2, const typename _Graph::const_iterator &i) const;
	IC		_dist_type	estimate			(const _index_type &node_index) const;
	IC		bool		is_limit_reached	(const _iteration_type iteration_count) const;
};

#include "path_manager_table_inline.h"