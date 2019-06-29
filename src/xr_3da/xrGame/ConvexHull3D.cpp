#include "stdafx.h"
#pragma hdrstop

#include "ConvexHull3D.h"

float	CConvexHull3D::ms_fEpsilon			= 0.001f;
float	CConvexHull3D::ms_fRange			= 10.f;//10
int		CConvexHull3D::ms_iTSize			= 75;//75;
int		CConvexHull3D::ms_iQuantityFactor	= 16;//16;

IC float SymmetricRandom ()
{
    return 2.f*float(rand())/float(RAND_MAX) - 1.f;
}

//---------------------------------------------------------------------------
CConvexHull3D::CConvexHull3D (int iVertexQuantity, const Fvector* akVertex)
{
    VERIFY			(akVertex);
    m_akTriangle	= 0;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra four slots are required for temporary storage.
    Fvector			*akPoint = (Fvector*)xr_malloc((iVertexQuantity+4)*sizeof(Fvector));
    Memory.mem_copy	(akPoint,akVertex,iVertexQuantity*sizeof(Fvector));

    // compute the axis-aligned bounding box of the vertices
    m_fXMin = akPoint[0].x;
    m_fXMax = m_fXMin;
    m_fYMin = akPoint[0].y;
    m_fYMax = m_fYMin;
    m_fZMin = akPoint[0].z;
    m_fZMax = m_fZMin;

    int i;
    for (i = 1; i < iVertexQuantity; i++)
    {
        float fValue = akPoint[i].x;
        if ( m_fXMax < fValue )
            m_fXMax = fValue;
        if ( m_fXMin > fValue )
            m_fXMin = fValue;

        fValue = akPoint[i].y;
        if ( m_fYMax < fValue )
            m_fYMax = fValue;
        if ( m_fYMin > fValue )
            m_fYMin = fValue;

        fValue = akPoint[i].z;
        if ( m_fZMax < fValue )
            m_fZMax = fValue;
        if ( m_fZMin > fValue )
            m_fZMin = fValue;
    }

    m_fXRange = m_fXMax-m_fXMin;
    m_fYRange = m_fYMax-m_fYMin;
    m_fZRange = m_fZMax-m_fZMin;

    // need to scale the data later to do a correct tetrahedron count
    float fMaxRange = m_fXRange;
    if ( fMaxRange < m_fYRange )
        fMaxRange = m_fYRange;
    if ( fMaxRange < m_fZRange )
        fMaxRange = m_fZRange;

    // Tweak the points by very small random numbers to help avoid
    // cosphericities in the vertices.
    float fAmplitude = 0.5f*ms_fEpsilon*fMaxRange;
    for (i = 0; i < iVertexQuantity; i++)
    {
        akPoint[i].x += fAmplitude*SymmetricRandom();
        akPoint[i].y += fAmplitude*SymmetricRandom();
        akPoint[i].z += fAmplitude*SymmetricRandom();
    }

    float aafWork[4][3] =
    {
        { 8.0f*ms_fRange, -ms_fRange, -ms_fRange },
        { -ms_fRange, 8.0f*ms_fRange, -ms_fRange },
        { -ms_fRange, -ms_fRange, 8.0f*ms_fRange },
        { -ms_fRange, -ms_fRange, -ms_fRange }
    };

    for (i = 0; i < 4; i++)
    {
        akPoint[iVertexQuantity+i].x = m_fXMin+m_fXRange*aafWork[i][0];
        akPoint[iVertexQuantity+i].y = m_fYMin+m_fYRange*aafWork[i][1];
        akPoint[iVertexQuantity+i].z = m_fZMin+m_fZRange*aafWork[i][2];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aaII[3];
    float fTmp;

    int iSixTSize = 6*ms_iTSize;
    int** aaiTmp = (int**)xr_malloc((iSixTSize+1)*sizeof(int*));
    aaiTmp[0] = (int*)xr_malloc(3*(iSixTSize+1));
    for (i0 = 1; i0 < iSixTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 3*i0;

    // Estimate of how many tetrahedrons there can be.  Since theoretically
    // the number is O(N^2), this could be quite large.  You may need to
    // increase i1 if a call to this function fails...
    i1 = ms_iQuantityFactor*iVertexQuantity;

    int* aaID = (int*)xr_malloc(i1*sizeof(int));
    for (i0 = 0; i0 < i1; i0++)
        aaID[i0] = i0;

    int** aaiA3S = (int**)xr_malloc(i1*sizeof(int*));
    aaiA3S[0] = (int*)xr_malloc(4*i1*sizeof(int));
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 4*i0;
    aaiA3S[0][0] = iVertexQuantity;
    aaiA3S[0][1] = iVertexQuantity+1;
    aaiA3S[0][2] = iVertexQuantity+2;
    aaiA3S[0][3] = iVertexQuantity+3;

    // circumscribed centers and radii
    float** aafCCR = (float**)xr_malloc(i1*sizeof(float*));
    aafCCR[0] = (float*)xr_malloc(4*i1*sizeof(float));
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 4*i0;
    aafCCR[0][0] = 0.0;
    aafCCR[0][1] = 0.0;
    aafCCR[0][2] = 0.0;
    aafCCR[0][3] = flt_max;

    int iTetraQuantity = 1;  // number of tetrahedra
    i4 = 1;

    // compute tetrahedralization
    for (i0 = 0; i0 < iVertexQuantity; i0++)
    {
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTetraQuantity; i11++)
        {
            i1++;
            while ( aaiA3S[i1][0] < 0 )
                i1++;
            fTmp = aafCCR[i1][3];
            for (i2 = 0; i2 < 3; i2++)
            {
                float fZ = akPoint[i0][i2] - aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < 0.0 )
                    goto Corner3;
            }
            i9--;
            i4--;
            aaID[i4] = i1;
            for (i2 = 0; i2 < 4; i2++)
            {
                aaII[0] = 0;
                if ( aaII[0] == i2 )
                    aaII[0]++;
                for (i3 = 1; i3 < 3; i3++)
                {
                    aaII[i3] = aaII[i3-1] + 1;
                    if ( aaII[i3] == i2 )
                        aaII[i3]++;
                }
                if ( i7 > 2 )
                {
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {
                        for (i5 = 0; i5 < 3; i5++)
                        {
                            if ( aaiA3S[i1][aaII[i5]] != aaiTmp[i3][i5] )
                                goto Corner1;
                        }
                        for (i6 = 0; i6 < 3; i6++)
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iSixTSize )
                {
                    // Temporary storage exceeded.  Increase ms_iTSize and
                    // call the constructor again.
					goto ExitConvexHullError;
                }
                for (i3 = 0; i3 < 3; i3++)
                    aaiTmp[i7][i3] = aaiA3S[i1][aaII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {
            if ( aaiTmp[i1][0] >= iVertexQuantity )
            {
                for (i2 = 0; i2 < 3; i2++)
                {
                    for (aafWork[3][i2] = 0, i3 = 0; i3 < 3; i3++)
                    {
                        aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                            akPoint[i0][i3];
                        aafWork[3][i2] += 0.5f*aafWork[i3][i2]*
                            (akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                    }
                }

                fTmp = (aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                    aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                    aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*
                    aafWork[2][1])) + (aafWork[2][0]*(aafWork[0][1]*
                    aafWork[1][2] - aafWork[0][2]*aafWork[1][1]));
				if (fTmp == 0.0 ){
                	goto ExitConvexHullError;
            	}
//                assert( fTmp != 0.0 );
                fTmp = 1.0f/fTmp;
                aafCCR[aaID[i4]][0] = ((aafWork[3][0]*(aafWork[1][1]*
                    aafWork[2][2] - aafWork[1][2]*aafWork[2][1])) -
                    (aafWork[1][0]*(aafWork[3][1]*aafWork[2][2] -
                    aafWork[3][2]*aafWork[2][1])) + (aafWork[2][0]*(
                    aafWork[3][1]*aafWork[1][2]-aafWork[3][2]*
                    aafWork[1][1])))*fTmp;
                aafCCR[aaID[i4]][1] = ((aafWork[0][0]*(aafWork[3][1]*
                    aafWork[2][2] - aafWork[3][2]*aafWork[2][1])) -
                    (aafWork[3][0]*(aafWork[0][1]*aafWork[2][2] -
                    aafWork[0][2]*aafWork[2][1])) + (aafWork[2][0]*(
                    aafWork[0][1]*aafWork[3][2] - aafWork[0][2]*
                    aafWork[3][1])))*fTmp;
                aafCCR[aaID[i4]][2] = ((aafWork[0][0]*(aafWork[1][1]*
                    aafWork[3][2] - aafWork[1][2]*aafWork[3][1])) -
                    (aafWork[1][0]*(aafWork[0][1]*aafWork[3][2] -
                    aafWork[0][2]*aafWork[3][1])) + (aafWork[3][0]*(
                    aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                    aafWork[1][1])))*fTmp;

                for (aafCCR[aaID[i4]][3] = 0, i2 = 0; i2 < 3; i2++)
                {
                    float fZ = akPoint[i0][i2] - aafCCR[aaID[i4]][i2];
                    aafCCR[aaID[i4]][3] += fZ*fZ;
                    aaiA3S[aaID[i4]][i2] = aaiTmp[i1][i2];
                }

                aaiA3S[aaID[i4]][3] = i0;
                i4++;
                i9++;
            }
        }
        iTetraQuantity += i9;
    }

    // count the number of triangular faces
    m_iTriangleQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;

        if ( aaiA3S[i0][1] < iVertexQuantity )
            m_iTriangleQuantity++;
    }

    // create the triangles
    m_akTriangle = (Triangle*)xr_malloc(i1*m_iTriangleQuantity*sizeof(Triangle));

    m_iTriangleQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;

        if ( aaiA3S[i0][1] < iVertexQuantity )
        {
            Triangle& rkTri = m_akTriangle[m_iTriangleQuantity];
            rkTri.m_aiVertex[0] = (u16)aaiA3S[i0][1];
            rkTri.m_aiVertex[1] = (u16)aaiA3S[i0][2];
            rkTri.m_aiVertex[2] = (u16)aaiA3S[i0][3];
            m_iTriangleQuantity++;
        }
    }

//ExitConvexHull:
    xr_free	(aaiTmp[0]);
    xr_free	(aaiTmp);
    xr_free	(aaID);
    xr_free	(aaiA3S[0]);
    xr_free	(aaiA3S);
    xr_free	(aafCCR[0]);
    xr_free	(aafCCR);
    xr_free	(akPoint);
    return;
ExitConvexHullError:
    xr_free	(aaiTmp[0]);
    xr_free	(aaiTmp);
    xr_free	(aaID);
    xr_free	(aaiA3S[0]);
    xr_free	(aaiA3S);
    xr_free	(aafCCR[0]);
    xr_free	(aafCCR);
    xr_free	(akPoint);
	VERIFY(0);
}
//---------------------------------------------------------------------------
CConvexHull3D::~CConvexHull3D ()
{
    xr_free	(m_akTriangle);
}
//---------------------------------------------------------------------------
