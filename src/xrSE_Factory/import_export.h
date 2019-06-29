////////////////////////////////////////////////////////////////////////////
//	Module 		: import_export.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_EXPORT_H
#	define IMPORT_EXPORT_H

#include "fastdelegate.h"

using namespace fastdelegate;

class DLL_API CTestInterface {
public:
	FastDelegate1<LPCSTR>	test0;
	FastDelegate1<int>		test1;

public:
//	virtual	void xr_stdcall	test_0			(LPCSTR) = 0;
//	virtual void xr_stdcall	test_1			(int) = 0;

public:
	virtual void __stdcall	test_test0		()=0;
	virtual void __stdcall	test_test1		()=0;
};

#endif // IMPORT_EXPORT_H