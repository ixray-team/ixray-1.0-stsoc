////////////////////////////////////////////////////////////////////////////
//	Module 		: script_class_id_inline.h
//	Created 	: 18.04.2004
//  Modified 	: 18.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script class identifiers inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once


IC	bool CLSID_Holder::CClassName::operator<(const CLSID_Holder::CClassName &class_name) const
{
	return				(m_class_id < class_name.m_class_id);
}

IC	bool CLSID_Holder::CClassName::operator<(const CLASS_ID &class_id) const
{
	return				(m_class_id < class_id);
}

IC	bool CLSID_Holder::CClassName::operator==(LPCSTR class_name) const
{
	return				(!xr_strcmp(m_class_name,class_name));
}

IC	CLSID_Holder::CLSID_Holder	()
{
	init				();
}

IC	void CLSID_Holder::add		(LPCSTR class_name, const CLASS_ID &class_id)
{
	CClassName			temp;
	temp.m_class_name	= class_name;
	temp.m_class_id		= class_id;
	VERIFY				(std::find(m_classes.begin(),m_classes.end(),class_name) == m_classes.end());
	m_classes.push_back	(temp);
}

IC	void CLSID_Holder::script_register	(CLuaVirtualMachine *lua_virtual_machine)
{
	xr_vector<CClassName>::const_iterator	I = m_classes.begin(), B = I;
	xr_vector<CClassName>::const_iterator	E = m_classes.end();
	
	luabind::class_<CInternal>		instance("clsid");
	
	for ( ; I != E; ++I) {
		instance.enum_("_clsid")
		[
			luabind::value((*I).m_class_name,int(I - B))
		];
	}
	
	luabind::module(lua_virtual_machine)
	[
		instance
	];
}

IC	int CLSID_Holder::clsid		(const CLASS_ID &class_id) const
{
	xr_vector<CClassName>::const_iterator	I = std::lower_bound(m_classes.begin(),m_classes.end(),class_id);
	VERIFY2				((m_classes.end() != I) && (*I).m_class_id == class_id,"There is no corresponding class id!");
	return				(int(I - m_classes.begin()));
}
