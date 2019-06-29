#pragma once

class CLine3D {
protected:
	Fvector m_kOrigin;		// P
	Fvector m_kDirection;	// D
public:
    // Line is L(t) = P+t*D for any real-valued t.  D is not necessarily
    // unit length.
	//----------------------------------------------------------------------------
	IC					CLine3D		()
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


