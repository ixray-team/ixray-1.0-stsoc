//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ConvexHull2D.h"

float CConvexHull2D::ms_fEpsilon = 0.00001;
float CConvexHull2D::ms_fRange = 10.0;
int CConvexHull2D::ms_iTSize = 75;

IC float SymmetricRandom (){
    return 2.f*float(rand())/float(RAND_MAX) - 1.f;
}
//---------------------------------------------------------------------------
CConvexHull2D::CConvexHull2D (int iVertexQuantity, const Fvector2* akVertex)
{
    VERIFY( akVertex );
    m_akEdge = 0;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra three slots are required for temporary storage.
    Fvector2* akPoint = new Fvector2[iVertexQuantity+3];
    memcpy(akPoint,akVertex,iVertexQuantity*sizeof(Fvector2));

    // compute the axis-aligned bounding rectangle of the vertices
    m_fXMin = akPoint[0].x;
    m_fXMax = m_fXMin;
    m_fYMin = akPoint[0].y;
    m_fYMax = m_fYMin;

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
    }

    m_fXRange = m_fXMax-m_fXMin;
    m_fYRange = m_fYMax-m_fYMin;

    // need to scale the data later to do a correct triangle count
    float fMaxRange = ( m_fXRange > m_fYRange ? m_fXRange : m_fYRange );
//    float fMaxRangeSqr = fMaxRange*fMaxRange;

    // Tweak the points by very small random numbers to help avoid
    // cocircularities in the vertices.
    float fAmplitude = 0.5*ms_fEpsilon*fMaxRange;
    for (i = 0; i < iVertexQuantity; i++)
    {
        akPoint[i].x += fAmplitude*SymmetricRandom();
        akPoint[i].y += fAmplitude*SymmetricRandom();
    }

    float aafWork[3][2] =
    {
        { 5.0*ms_fRange, -ms_fRange },
        { -ms_fRange, 5.0*ms_fRange },
        { -ms_fRange, -ms_fRange }
    };

    for (i = 0; i < 3; i++)
    {
        akPoint[iVertexQuantity+i].x = m_fXMin+m_fXRange*aafWork[i][0];
        akPoint[iVertexQuantity+i].y = m_fYMin+m_fYRange*aafWork[i][1];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aiII[3];
    float fTmp;

    int iTwoTSize = 2*ms_iTSize;
    int** aaiTmp = new int*[iTwoTSize+1];
    aaiTmp[0] = new int[2*(iTwoTSize+1)];
    for (i0 = 1; i0 < iTwoTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 2*i0;
    i1 = 2*(iVertexQuantity + 2);

    int* aiID = new int[i1];
    for (i0 = 0; i0 < i1; i0++)
        aiID[i0] = i0;

    int** aaiA3S = new int*[i1];
    aaiA3S[0] = new int[3*i1];
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 3*i0;
    aaiA3S[0][0] = iVertexQuantity;
    aaiA3S[0][1] = iVertexQuantity+1;
    aaiA3S[0][2] = iVertexQuantity+2;

    // circumscribed centers and radii
    float** aafCCR = new float*[i1];
    aafCCR[0] = new float[3*i1];
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 3*i0;
    aafCCR[0][0] = 0.0;
    aafCCR[0][1] = 0.0;
    aafCCR[0][2] = flt_max;

    int iTriangleQuantity = 1;  // number of triangles
    i4 = 1;

    // compute triangulation
    for (i0 = 0; i0 < iVertexQuantity; i0++)
    {
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTriangleQuantity; i11++)
        {
            i1++;
            while ( aaiA3S[i1][0] < 0 )
                i1++;
            fTmp = aafCCR[i1][2];
            for (i2 = 0; i2 < 2; i2++)
            {
                float fZ = akPoint[i0][i2] - aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < 0.0 )
                    goto Corner3;
            }
            i9--;
            i4--;
            aiID[i4] = i1;
            for (i2 = 0; i2 < 3; i2++)
            {
                aiII[0] = 0;
                if (aiII[0] == i2)
                    aiII[0]++;
                for (i3 = 1; i3 < 2; i3++)
                {
                    aiII[i3] = aiII[i3-1] + 1;
                    if (aiII[i3] == i2)
                        aiII[i3]++;
                }
                if ( i7 > 1 )
                {
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {
                        for (i5 = 0; i5 < 2; i5++)
                            if ( aaiA3S[i1][aiII[i5]] != aaiTmp[i3][i5] )
                                goto Corner1;
                        for (i6 = 0; i6 < 2; i6++)
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iTwoTSize )
                {
                    // Temporary storage exceeded.  Increase ms_iTSize and
                    // call the constructor again.
                    VERIFY( false );
                    goto ExitConvexHull;
                }
                for (i3 = 0; i3 < 2; i3++)
                    aaiTmp[i7][i3] = aaiA3S[i1][aiII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {
            if ( aaiTmp[i1][0] >= iVertexQuantity )
            {
                for (i2 = 0; i2 < 2; i2++)
                {
                    for (aafWork[2][i2] = 0, i3 = 0; i3 < 2; i3++)
                    {
                        aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                            akPoint[i0][i3];
                        aafWork[2][i2] += 0.5*aafWork[i3][i2]*(
                            akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                    }
                }

                fTmp = aafWork[0][0]*aafWork[1][1] -
                    aafWork[1][0]*aafWork[0][1];
                VERIFY( fabsf(fTmp) > 0.0 );
                fTmp = 1.0/fTmp;
                aafCCR[aiID[i4]][0] = (aafWork[2][0]*aafWork[1][1] -
                    aafWork[2][1]*aafWork[1][0])*fTmp;
                aafCCR[aiID[i4]][1] = (aafWork[0][0]*aafWork[2][1] -
                    aafWork[0][1]*aafWork[2][0])*fTmp;

                for (aafCCR[aiID[i4]][2] = 0, i2 = 0; i2 < 2; i2++)
                {
                    float fZ = akPoint[i0][i2] - aafCCR[aiID[i4]][i2];
                    aafCCR[aiID[i4]][2] += fZ*fZ;
                    aaiA3S[aiID[i4]][i2] = aaiTmp[i1][i2];
                }

                aaiA3S[aiID[i4]][2] = i0;
                i4++;
                i9++;
            }
        }
        iTriangleQuantity += i9;
    }

    // count the number of hull edges
    m_iEdgeQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTriangleQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;

        if ( aaiA3S[i0][1] < iVertexQuantity )
            m_iEdgeQuantity++;
    }

    // create the hull m_akEdge list
    m_akEdge = new Edge[m_iEdgeQuantity];
    m_iEdgeQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTriangleQuantity; i11++)
    {
        i0++;
         while ( aaiA3S[i0][0] < 0 )
             i0++;

        if ( aaiA3S[i0][1] < iVertexQuantity )
        {
            Edge& rkEdge = m_akEdge[m_iEdgeQuantity];
            rkEdge.m_aiVertex[0] = aaiA3S[i0][1];
            rkEdge.m_aiVertex[1] = aaiA3S[i0][2];
            m_iEdgeQuantity++;
        }
    }

ExitConvexHull:;
    delete[] aaiTmp[0];
    delete[] aaiTmp;
    delete[] aiID;
    delete[] aaiA3S[0];
    delete[] aaiA3S;
    delete[] aafCCR[0];
    delete[] aafCCR;
    delete[] akPoint;
}
//---------------------------------------------------------------------------
CConvexHull2D::~CConvexHull2D ()
{
    delete[] m_akEdge;
}
//---------------------------------------------------------------------------
