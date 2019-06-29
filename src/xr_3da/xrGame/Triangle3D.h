#pragma once

class CTriangle3D {
protected:
	Fvector				m_kOrigin;
	Fvector				m_kEdge0;
	Fvector				m_kEdge1;

public:
    // Triangle points are tri(s,t) = b+s*e0+t*e1 where 0 <= s <= 1,
    // 0 <= t <= 1, and 0 <= s+t <= 1.

	IC					CTriangle3D	()
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
	IC Fvector			&Edge0		()
	{
		return m_kEdge0;
	}
	//----------------------------------------------------------------------------
	IC const Fvector	&Edge0		() const
	{
		return m_kEdge0;
	}
	//----------------------------------------------------------------------------
	IC Fvector			&Edge1		()
	{
		return m_kEdge1;
	}
	//----------------------------------------------------------------------------
	IC const Fvector	&Edge1		() const
	{
		return m_kEdge1;
	}
	//----------------------------------------------------------------------------
	float				SqrDistance (const CTriangle3D	&rkTri0, const CTriangle3D	&rkTri1,
		float	*pfTri0P0 = NULL, float	*pfTri0P1 = NULL, float	*pfTri1P0 = NULL,
		float	*pfTri1P1 = NULL);

	float				Distance	(const CTriangle3D	&rkTri0, const CTriangle3D	&rkTri1,
		float	*pfTri0P0 = NULL, float	*pfTri0P1 = NULL, float	*pfTri1P0 = NULL,
		float	*pfTri1P1 = NULL);

	float				SqrDistance (const CTriangle3D	&rkTri1);
	float				Distance	(const CTriangle3D	&rkTri1);
};

