////////////////////////////////////////////////////////////////////////////
//	Module 		: level_navigation_graph_space.h
//	Created 	: 02.10.2001
//  Modified 	: 27.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Level navigation graph space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"

namespace LevelNavigationGraph {
#ifdef AI_COMPILER
	struct CCellVertexEx {
		u32				m_vertex_id;
		union {
			typedef u8	_use_type;

			struct {
				u32		m_mark : 24;
				u32		m_use  : 8;
			};
			u32			m_data;
		};

		CCellVertexEx	()
		{
		}

		CCellVertexEx	(u32 vertex_id, u32 mark, u32 use) :
			m_vertex_id	(vertex_id),
			m_mark		(mark),
			m_use		(use)
		{
		}
	};
#endif

	struct CCellVertex {
		union {
			struct {
				u16		m_right;
				u16		m_down;
			};
			u16			m_dirs[2];
			u32			m_all_dirs;
		};
		
		union {
			typedef u8	_use_type;

			struct {
				u32		m_mark : 24;
				u32		m_use  : 8;
			};
			u32			m_data;
		};
		
		CCellVertex		*m_right_next;
		u32				m_down_left;

		ICF CCellVertex	() :
			m_data		(0),
			m_all_dirs	(0),
			m_down_left	(0),
			m_right_next(0)
		{
		}
	};
	
	class CSector : public IPureSerializeObject<IReader,IWriter> {
	private:
		typedef u32		vertex_id_type;
	private:
		vertex_id_type	m_min_vertex_id;
		vertex_id_type	m_max_vertex_id;

	public:
		IC				CSector		() {}
		
		IC				CSector		(const vertex_id_type &min_vertex_id, const vertex_id_type &max_vertex_id)
		{
			m_min_vertex_id	= min_vertex_id;
			m_max_vertex_id	= max_vertex_id;
		}

		virtual void	save		(IWriter &stream)
		{
			stream.w	(&m_min_vertex_id,sizeof(m_min_vertex_id));
			stream.w	(&m_max_vertex_id,sizeof(m_max_vertex_id));
		}
		
		virtual void	load		(IReader &stream)
		{
			stream.r	(&m_min_vertex_id,sizeof(m_min_vertex_id));
			stream.r	(&m_max_vertex_id,sizeof(m_max_vertex_id));
		}

		IC		bool	operator==	(const CSector &obj) const
		{
			return		(
				(min_vertex_id() == obj.min_vertex_id()) &&
				(max_vertex_id() == obj.max_vertex_id())
			);
		}

		IC		const vertex_id_type &min_vertex_id	() const
		{
			return		(m_min_vertex_id);
		}

		IC		const vertex_id_type &max_vertex_id	() const
		{
			return		(m_max_vertex_id);
		}
	};
};