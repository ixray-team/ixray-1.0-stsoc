#include "stdafx.h"
#include "Triangle3D.h"
#include "Segment3D.h"

//----------------------------------------------------------------------------
float CTriangle3D::SqrDistance (const CTriangle3D& rkTri0, const CTriangle3D& rkTri1,
    float* pfTri0P0, float* pfTri0P1, float* pfTri1P0, float* pfTri1P1)
{
    float fS, fT, fU, fV, fS0, fT0, fU0, fV0, fSqrDist, fSqrDist0;
    CSegment3D kSeg;

    // compare edges of tri0 against all of tri1
    kSeg.Origin() = rkTri0.Origin();
    kSeg.Direction() = rkTri0.Edge0();
    fSqrDist = SqrDistance(kSeg,rkTri1,&fS,&fU,&fV);
    fT = 0.0f;

    kSeg.Direction() = rkTri0.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkTri1,&fT0,&fU0,&fV0);
    fS0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = kSeg.Origin() + rkTri0.Edge0();
    kSeg.Direction() = kSeg.Direction() - rkTri0.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkTri1,&fT0,&fU0,&fV0);
    fS0 = 1.0f-fT0;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    // compare edges of tri1 against all of tri0
    kSeg.Origin() = rkTri1.Origin();
    kSeg.Direction() = rkTri1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkTri0,&fU0,&fS0,&fT0);
    fV0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Direction() = rkTri1.Edge1();
    fSqrDist0 = SqrDistance(kSeg,rkTri0,&fV0,&fS0,&fT0);
    fU0 = 0.0f;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    kSeg.Origin() = kSeg.Origin() + rkTri1.Edge0();
    kSeg.Direction() = kSeg.Direction() - rkTri1.Edge0();
    fSqrDist0 = SqrDistance(kSeg,rkTri0,&fV0,&fS0,&fT0);
    fU0 = 1.0f-fV0;
    if ( fSqrDist0 < fSqrDist )
    {
        fSqrDist = fSqrDist0;
        fS = fS0;
        fT = fT0;
        fU = fU0;
        fV = fV0;
    }

    if ( pfTri0P0 )
        *pfTri0P0 = fS;

    if ( pfTri0P1 )
        *pfTri0P1 = fT;

    if ( pfTri1P0 )
        *pfTri1P0 = fU;

    if ( pfTri1P1 )
        *pfTri1P1 = fV;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
float CTriangle3D::Distance (const CTriangle3D& rkTri0, const CTriangle3D& rkTri1,
    float* pfTri0P0, float* pfTri0P1, float* pfTri1P0, float* pfTri1P1)
{
    return Math::Sqrt(SqrDistance(rkTri0,rkTri1,pfTri0P0,pfTri0P1,pfTri1P0,
        pfTri1P1));
}
//----------------------------------------------------------------------------

float CTriangle3D::SqrDistance (const CTriangle3D	&rkTri1)
{
	return		(SqrDistance(*this,rkTri1,0,0,0,0));
}

float CTriangle3D::Distance	(const CTriangle3D	&rkTri1)
{
	return		(Distance(*this,rkTri1,0,0,0,0));
}

//----------------------------------------------------------------------------
