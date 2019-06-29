#pragma once

class CSegment3D
{
protected:
	Fvector m_kOrigin;			// P
	Fvector m_kDirection;		// D
public:
    // Segment is S(t) = P+t*D for 0 <= t <= 1.  D is not necessarily unit
    // length.  The end points are P and P+D.
	//----------------------------------------------------------------------------
	IC					CSegment3D	()
	{
		// no initialization for efficiency
	}
	//----------------------------------------------------------------------------
	IC Fvector			&Origin		()
	{
		return m_kOrigin;
	}
	//----------------------------------------------------------------------------
	IC const Fvector	&Origin		() const
	{
		return m_kOrigin;
	}
	//----------------------------------------------------------------------------
	IC Fvector			&Direction	()
	{
		return m_kDirection;
	}
	//----------------------------------------------------------------------------
	IC const Fvector	&Direction	() const
	{
		return m_kDirection;
	}
	//----------------------------------------------------------------------------
};



