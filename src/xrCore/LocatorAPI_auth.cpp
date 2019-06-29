#include "stdafx.h"
#pragma hdrstop

struct	auth_options	{
	xr_vector<xr_string>				ignore;
	xr_vector<xr_string>				important;
};


void	auth_entry		(void* p)
{
	FS.auth_runtime		(p);
}

void	CLocatorAPI::auth_generate		(xr_vector<xr_string>&	ignore, xr_vector<xr_string>&	important)
{
	auth_options*	_o	= xr_new<auth_options>	();
	_o->ignore			= ignore	;
	_o->important		= important	;
	if( strstr(Core.CompName,"RIP") ){//stupid Prescott :(
		FS.auth_runtime		(_o);
	}else
		thread_spawn		(auth_entry,"checksum",0,_o);
}

u64		CLocatorAPI::auth_get			()
{
	m_auth_lock.Enter	()	;
	m_auth_lock.Leave	()	;
	return	m_auth_code		;
}

void	CLocatorAPI::auth_runtime		(void*	params)
{
	m_auth_lock.Enter	()	;
	auth_options*		_o	= (auth_options*)	params	;
	m_auth_code			= 0;
	bool				do_break = false;
	for (files_it it = files.begin(); it!=files.end(); ++it)
	{
		const file&	f	=	*it;

		// test for skip
		BOOL	bSkip	=	FALSE;
		for (u32 s=0; s<_o->ignore.size(); s++)		{
			if (strstr(f.name,_o->ignore[s].c_str()))	
				bSkip	=	TRUE;
		}
		if (bSkip)		continue	;

		// test for important
		for (s=0; s<_o->important.size(); s++)	{
			if (strstr(f.name,_o->important[s].c_str()))	{
				// crc for file
				IReader*	r	= FS.r_open	(f.name);
				if (!r) {
					do_break	= true;
					break;
				}
				u32 crc			= crc32		(r->pointer(),r->length());
				r->close		();
				m_auth_code	^=	u64(crc);
			}
		}

		if (do_break)
			break;
	}
	xr_delete			(_o);
	m_auth_lock.Leave	()	;
}
