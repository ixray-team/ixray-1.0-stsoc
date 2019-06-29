// FastSpriteRail.h: interface for the CFastSpriteRail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTSPRITERAIL_H__0FA10331_B1EA_4147_B2C3_E3C9AF81E090__INCLUDED_)
#define AFX_FASTSPRITERAIL_H__0FA10331_B1EA_4147_B2C3_E3C9AF81E090__INCLUDED_
#pragma once

#include "FastSprite.h"

class ENGINE_API CFastSpriteRail : public CFastSprite
{
	Fmatrix				mTransformNoScale;
public:
	virtual void		Update			();

	CFastSpriteRail(Fvector &vStart, Fvector &vEnd);
	virtual ~CFastSpriteRail();
};

#endif // !defined(AFX_FASTSPRITERAIL_H__0FA10331_B1EA_4147_B2C3_E3C9AF81E090__INCLUDED_)
