///////////////////////////////////////////////////////////////
// Scope.h
// Scope - апгрейд оружия снайперский прицел
///////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"
#include "script_export_space.h"

class CScope : public CInventoryItemObject {
private:
	typedef CInventoryItemObject inherited;
public:
			CScope ();
	virtual ~CScope();

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	(bool just_before_destroy);

	virtual void UpdateCL			();
	virtual void renderable_Render	();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScope)
#undef script_type_list
#define script_type_list save_type_list(CScope)
