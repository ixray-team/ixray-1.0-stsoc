////////////////////////////////////////////////////////////////////////////
// script_string_table.h :	экспорт таблицы строк в скрипты
//////////////////////////////////////////////////////////////////////////
/*
#pragma once

#include "alife_space.h"
#include "script_export_space.h"

class CStringTable;

class CScriptStringTable
{
private:
	CStringTable* m_pStringTable;
public:
	CScriptStringTable				();
	virtual		~CScriptStringTable	();

	int			index_by_id					(LPCSTR str_id) const;
	LPCSTR		get_by_id					(LPCSTR str_id) const;
	LPCSTR		get_by_index				(int str_index) const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptStringTable)
#undef script_type_list
#define script_type_list save_type_list(CScriptStringTable)
*/