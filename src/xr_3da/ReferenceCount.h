// ReferenceCount.h: interface for the CReferenceCount class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFERENCECOUNT_H__166A16CD_E846_4257_8567_89D91AFA66F0__INCLUDED_)
#define AFX_REFERENCECOUNT_H__166A16CD_E846_4257_8567_89D91AFA66F0__INCLUDED_
#pragma once

class ENGINE_API CReferenceCount  
{
	u32		dwReference;
public:
	IC void		ref_inc		()	{ dwReference++; }
	IC void		ref_release	()	{ dwReference--; if (0==dwReference) delete this; }

	CReferenceCount() : dwReference(1) {};
	virtual ~CReferenceCount() {
		R_ASSERT(0==dwReference);
	}
};

#endif // !defined(AFX_REFERENCECOUNT_H__166A16CD_E846_4257_8567_89D91AFA66F0__INCLUDED_)
