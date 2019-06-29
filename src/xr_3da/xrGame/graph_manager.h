////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_manager.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"
#include "graph_manager_interface.h"

template <
	typename _Data,
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _vertex_index_type
>
class CGraphManagerAbstract : public IGraphManager {
public:
	typedef CGraphAbstract<
		_Data,
		_edge_weight_type,
		_vertex_id_type,
		_vertex_index_type
	>											CSGraphAbstract;
private:
	CSGraphAbstract								*m_graph;
	_vertex_id_type								m_current_vertex_id;
	_vertex_id_type								m_dest_vertex_id;
	xr_vector<_vertex_id_type>					m_path;
	bool										m_actuality;

protected:
	IC		void								set_current_vertex_id	(const _vertex_id_type dest_vertex_id);
	IC		CSGraphAbstract						&graph					();

public:
	IC											CGraphManagerAbstract	();
	virtual										~CGraphManagerAbstract	();
			void								init					();
	virtual	void								reinit					(bool clear_all = false);
	IC		void								set_dest_vertex_id		(const _vertex_id_type dest_vertex_id);
	IC		const _vertex_id_type				&dest_vertex_id			() const;
	IC		const _vertex_id_type				&current_vertex_id		() const;
	IC		bool								actual					() const;
	IC		const xr_vector<_vertex_id_type>	&path					() const;
	IC		xr_vector<_vertex_id_type>			&path					();
	virtual	void								update					(u32 time_delta);
	IC		const CSGraphAbstract				&graph					() const;
	IC		void								go_path					();
};

#include "graph_manager_inline.h"
