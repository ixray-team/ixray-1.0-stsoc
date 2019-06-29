#include "stdafx.h"
#include "igame_level.h"

#include "xr_object.h"
#include "xr_object_list.h"

CObject*	CObjectList::Create				( LPCSTR	name	)
{
	string128			l_name;
	POOL_IT	it			=	map_POOL.find	(strlwr(strcpy(l_name,name)));
	if (it!=map_POOL.end())
	{
		// Instance found
		CObject* O				=	it->second;
		map_POOL.erase		(it);
		objects.push_back	(O);
		return				O;
	} else {
		// Create and load new instance
		Msg					("! Uncached loading '%s'...",name);
		CLASS_ID CLS		=	pSettings->r_clsid		(name,"class");
		CObject* O			=	(CObject*) NEW_INSTANCE	(CLS);
		O->Load				(name);
		objects.push_back	(O);
		return				O;
	}
}

void		CObjectList::Destroy			( CObject*	O		)
{
	if (0==O)				return;
	net_Unregister			(O);
	OBJ_IT it				=	std::find	(objects.begin(),objects.end(),O);
	if (it!=objects.end())	objects.erase	(it);
	map_POOL.insert			(mk_pair(O->cNameSect(),O));
}
