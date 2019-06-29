////////////////////////////////////////////////////////////////////////////
// script_string_table.cpp :	экспорт таблицы строк в скрипты
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
/*
#include "script_string_table.h"
#include "string_table.h"


CScriptStringTable::CScriptStringTable()
{
	m_pStringTable = xr_new<CStringTable>();
}

CScriptStringTable::~CScriptStringTable	()
{
	xr_delete(m_pStringTable);
}


int CScriptStringTable::index_by_id (LPCSTR str_id) const
{
	return (int)m_pStringTable->IndexById(STRING_ID(str_id));
}
LPCSTR CScriptStringTable::get_by_id (LPCSTR str_id) const
{
	return *shared_str((*m_pStringTable)(STRING_ID(str_id)));
}
LPCSTR CScriptStringTable::get_by_index	(int str_index) const
{
	return *shared_str((*m_pStringTable)(STRING_INDEX(str_index)));
}
*/