////////////////////////////////////////////////////////////////////////////
// script_task.h :		структура для передачи информации о несюжетных 
//						задания в скрипты
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "script_export_space.h"

class CALifeTask;

class CScriptTask;
DEFINE_VECTOR(CScriptTask, ALIFE_TASK_VECTOR, ALIFE_TASK_VECTOR_IT);

class CScriptTask
{
public:
	LPCSTR				m_sName;
	LPCSTR				m_sOrganization;
	int 				m_iQuantity;
	int 				m_iPrice;

public:
				CScriptTask		();
	virtual		~CScriptTask	();


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptTask)
#undef script_type_list
#define script_type_list save_type_list(CScriptTask)


//////////////////////////////////////////////////////////////////////////
// список заданий на атефакты
//////////////////////////////////////////////////////////////////////////

class CScriptTaskList
{
private:
	ALIFE_TASK_VECTOR*					alife_tasks;
public:
				CScriptTaskList			(u16 trader_id);
	virtual		~CScriptTaskList		();

	//количество заданий в списке
	int				size				()		const;
	CScriptTask		get					(int i) const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptTaskList)
#undef script_type_list
#define script_type_list save_type_list(CScriptTaskList)
