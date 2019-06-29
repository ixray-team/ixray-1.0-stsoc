////////////////////////////////////////////////////////////////////////////
//	Module 		: test_table.h
//	Created 	: 21.03.2002
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Test table
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename	_cell_type,
	u32			rows,
	u32			columns
>
class CTestTable {
public:
	struct CHeader {
		IC u32 vertex_count() const
		{
			return		((rows + 2)*(columns + 2));
		}
	};
protected:
	_cell_type			table[(rows+2)*(columns + 2)];
	_cell_type			invalid_value;
	_cell_type			valid_value;
	CHeader				m_header;
	u32					m_current_vertex_id;

public:
	typedef	_cell_type const_iterator;

						CTestTable		(LPCSTR, const _cell_type _invalid_value = _cell_type(-1), const _cell_type _valid_value = _cell_type(0))
	{
		invalid_value	= _invalid_value;
		valid_value		= _valid_value;
		for (u32 i=0; i < rows + 2; i++)
			for (u32 j=0; j < columns + 2; j++) {
				if (!i || !j || (i == rows + 1) || (j == columns + 1))
					table[i*(rows + 2) + j] = invalid_value;
				else
					table[i*(rows + 2) + j] = valid_value;
			}
	}

	virtual				~CTestTable		()
	{
	}

	IC		void		begin			(const u32 node_index, const_iterator &begin, const_iterator &end) const
	{
		begin					= 0;
		end						= 4;
	}

	IC		u32			get_edge_weight	(const u32 node_index1, const u32 node_index2) const
	{
		return					(1);
	}

	IC		u32			value		(const u32 node_index, const_iterator &i) const
	{
		switch (i) {
			case 0 : return		(node_index - rows - 2);
			case 1 : return		(node_index - 1);
			case 2 : return		(node_index + rows + 2);
			case 3 : return		(node_index + 1);
			default: NODEFAULT;
		}
		return					(u32(-1));
	}

	IC		bool		is_accessible	(const u32 node_index) const
	{
		VERIFY					(node_index < m_header.vertex_count());
		return					(table[node_index] != invalid_value);
	}

	IC const CHeader	&header() const
	{
		return					(m_header);
	}

	IC	void			enable			(const u32 &node_index, bool value)
	{
		VERIFY					(node_index < m_header.vertex_count());
		table[node_index]		= value ? valid_value : invalid_value;
	}
};