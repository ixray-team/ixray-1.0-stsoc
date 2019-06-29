#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#include <afxmt.h>
#include <shellapi.h>

#include <cj60lib.h>
//#include "../xrGame/script_debugger_messages.h"
#include "/Projects/xray/xrGame/script_debugger_messages.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <process.h>

#include <platform.h>
#include <scintilla.h>
#include <SciLexer.h>

#include <comdef.h>


#include "ssauto.h"

extern "C" const GUID __declspec(selectany) LIBID_SourceSafeTypeLib =
    {0x783cd4e0,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSItem =
    {0x783cd4e1,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSVersions =
    {0x783cd4e7,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSVersion =
    {0x783cd4e8,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSItems =
    {0x783cd4e5,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSCheckouts =
    {0x8903a770,0xf55f,0x11cf,{0x92,0x27,0x00,0xaa,0x00,0xa1,0xeb,0x95}};
extern "C" const GUID __declspec(selectany) IID_IVSSCheckout =
    {0x783cd4e6,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSDatabase =
    {0x783cd4e2,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) CLSID_VSSItem =
    {0x783cd4e3,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) CLSID_VSSVersion =
    {0x783cd4ec,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) CLSID_VSSDatabase =
    {0x783cd4e4,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSEvents =
    {0x783cd4e9,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSS =
    {0x783cd4eb,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};
extern "C" const GUID __declspec(selectany) IID_IVSSEventHandler =
    {0x783cd4ea,0x9d54,0x11cf,{0xb8,0xee,0x00,0x60,0x8c,0xc9,0xa7,0x1f}};

_COM_SMARTPTR_TYPEDEF(IVSSItem, IID_IVSSItem);
_COM_SMARTPTR_TYPEDEF(IVSSVersions, IID_IVSSVersions);
_COM_SMARTPTR_TYPEDEF(IVSSVersion, IID_IVSSVersion);
_COM_SMARTPTR_TYPEDEF(IVSSItems, IID_IVSSItems);
_COM_SMARTPTR_TYPEDEF(IVSSCheckouts, IID_IVSSCheckouts);
_COM_SMARTPTR_TYPEDEF(IVSSCheckout, IID_IVSSCheckout);
_COM_SMARTPTR_TYPEDEF(IVSSDatabase, IID_IVSSDatabase);
_COM_SMARTPTR_TYPEDEF(IVSSEvents,IID_IVSSEvents);
_COM_SMARTPTR_TYPEDEF(IVSS, IID_IVSS);
_COM_SMARTPTR_TYPEDEF(IVSSEventHandler, IID_IVSSEventHandler);

inline void v_TestHr(HRESULT x) {if FAILED(x) _com_issue_error(x);};


inline BOOL b_DisplayAnyError()
{
	IErrorInfoPtr errorInfo;
	if (GetErrorInfo(0, &errorInfo) == S_OK)
	{
		CComBSTR bstr_errorInfo;
		errorInfo->GetDescription(&bstr_errorInfo);
		CW2A pszA( bstr_errorInfo );

		MessageBox(NULL, pszA, _T("SSBrowser"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}

	return FALSE;
}
