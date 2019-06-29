// Script.h: interface for the CScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPT_H__401F20AC_037F_4AC3_A1C8_C0F801BC86EA__INCLUDED_)
#define AFX_SCRIPT_H__401F20AC_037F_4AC3_A1C8_C0F801BC86EA__INCLUDED_
#pragma once

#define SC_STRING_UNUSED
#include "seer.h"

class ENGINE_API	CScript
{
	scInstance*		instance;

	int				iStartupPoint;
	int				iStartupPointFastCall;
	char			cStartupName[64];

	int				VCall(scInstance* inst,int address,int paramc,int *params);
public:
	static void		RegisterSymbol(char *name, void *addr);

	int	__cdecl		Call(char* name, ...);
	int __cdecl		FastCall(int fake=0, ...);
	void			SetupFastCall(char *name);

	CScript			(scScript script);
	~CScript		();
};

#endif // !defined(AFX_SCRIPT_H__401F20AC_037F_4AC3_A1C8_C0F801BC86EA__INCLUDED_)
