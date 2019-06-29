// CkSettings.h: interface for the CkSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_)
#define AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CkSettings  
{
public:
	CkSettings();
	virtual ~CkSettings();

	static void __cdecl disableStringCaching(void);

	static void __cdecl cleanupMemory(void);
};

#endif // !defined(AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_)
