#pragma once

class CConvexHull3D
{
    // tweaking parameters
    static float ms_fEpsilon;
    static float ms_fRange;
    static int ms_iTSize;
    static int ms_iQuantityFactor;
public:
    // triangles
    class Triangle
    {
    public:
        // vertices forming triangle
        WORD m_aiVertex[3];
    };

    // smallest axis-aligned box containing the vertices
    float  m_fXMin, m_fXMax, m_fXRange;
    float  m_fYMin, m_fYMax, m_fYRange;
    float  m_fZMin, m_fZMax, m_fZRange;
protected:
    // triangles
    int m_iTriangleQuantity;
    Triangle* m_akTriangle;
public:
    // Construction and destruction.  MgcConvexHull3D does not take ownership
    // of the input array.  The application is responsible for deleting it.
    CConvexHull3D (int iVertexQuantity, const Fvector* akVertex);
    virtual ~CConvexHull3D ();

    IC const Triangle& GetTriangle (int i) const{
	    VERIFY( 0 <= i && i < m_iTriangleQuantity );
    	return m_akTriangle[i];
    }
    IC const Triangle* GetTriangles () const{
	    return m_akTriangle;
    }
    IC const int GetTriangleQuantity() const{ return m_iTriangleQuantity;}
};

