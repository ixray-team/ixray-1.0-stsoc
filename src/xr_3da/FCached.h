// FCached.h: interface for the FCached class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_)
#define AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_
#pragma once

#include "fbasicvisual.h"

class ENGINE_API FCached  : public IRender_Visual
{
public:
	LPVOID			pVertices;
	u32				vCount;
	LPWORD			pIndices;
	u32				iCount;

	virtual void	Load	(const char* N, IReader *data, u32 dwFlags);
	virtual void	Release	();						// Shared memory release
	virtual void	Copy	(IRender_Visual* from);

	FCached();
	virtual ~FCached();
};

void TransferGeometry	(LPVOID vDest, LPVOID vSrc, u32 vCount, u32 vStride,
						 LPWORD iDest, LPWORD iSrc, u32 iCount, u32 iOffset,
						 Fmatrix* xform = NULL);

#endif // !defined(AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_)
