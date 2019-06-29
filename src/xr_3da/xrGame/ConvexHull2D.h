#ifndef MGCCONVEXHULL2D_H
#define MGCCONVEXHULL2D_H

// The construction of a convex hull in 2D is a modification of code written
// by Dave Watson.  It uses an algorithm described in
//
//     Watson, D.F., 1981, Computing the n-dimensional Delaunay
//     tessellation with application to Voronoi polytopes:
//     The Computer J., 24(2), p. 167-172.

class CConvexHull2D
{
public:
    // Construction and destruction.  MgcConvexHull2D does not take ownership
    // of the input array.  The application is responsible for deleting it.
    CConvexHull2D (int iVertexQuantity, const Fvector2* akVertex);
    ~CConvexHull2D ();

    // edges
    class Edge
    {
    public:
        // vertices forming edge
        int m_aiVertex[2];
    };

    int GetEdgeQuantity () const;
    const Edge& GetEdge (int i) const;
    const Edge* GetEdges () const;

    // tweaking parameters
    static float& Epsilon ();  // default = 0.00001
    static float& Range ();    // default = 10.0
    static int& TSize ();        // default = 75

public:
    // smallest axis-aligned rectangle containing the vertices
    float m_fXMin, m_fXMax, m_fXRange;
    float m_fYMin, m_fYMax, m_fYRange;

    // edges
    int m_iEdgeQuantity;
    Edge* m_akEdge;

    // tweaking parameters
    static float ms_fEpsilon;
    static float ms_fRange;
    static int ms_iTSize;
};

#endif
