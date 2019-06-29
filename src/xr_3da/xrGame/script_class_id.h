////////////////////////////////////////////////////////////////////////////
//	Module 		: script_class_id.h
//	Created 	: 18.04.2004
//  Modified 	: 18.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script class identifiers
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CLSID_Holder {
private:
	struct CClassName {
		LPCSTR		m_class_name;
		CLASS_ID	m_class_id;

		IC	bool	operator<		(const CClassName &class_name) const;
		IC	bool	operator<		(const CLASS_ID &class_id) const;
		IC	bool	operator==		(LPCSTR class_name) const;
	};

	struct CInternal{};

private:
	xr_vector<CClassName>	m_classes;

public:
	IC				CLSID_Holder	();
	virtual			~CLSID_Holder	();
			void	init			();
	IC		void	add				(LPCSTR class_name, const CLASS_ID &class_id);
	IC		void	script_register	(CLuaVirtualMachine *lua_virtual_machine);
	IC		int		clsid			(const CLASS_ID &class_id) const;
};

#include "script_class_id_inline.h"