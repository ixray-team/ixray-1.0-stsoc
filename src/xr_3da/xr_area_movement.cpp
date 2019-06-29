#include "stdafx.h"
#include "igame_level.h"

#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "cl_intersect.h"

using namespace	collide;

// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify 
//         pO - Origin of plane
//         pN - Normal to plane 
// Notes : 
// Return: One of 3 classification codes
// -----------------------------------------------------------------------  
IC float classifyPoint(const Fvector& point, const Fvector& planeO, const Fvector& planeN)
{
	Fvector dir;
	dir.sub	(point,planeO);
	return dir.dotproduct(planeN);
}
// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - _vector_ describing direction of ray in world space
//         pOrigin - Origin of plane 
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------  
IC float intersectRayPlane(const Fvector& rayOrigin,	const Fvector& rayDirection, 
						const Fvector& planeOrigin,	const Fvector& planeNormal)
{
	float numer = classifyPoint(rayOrigin,planeOrigin,planeNormal);
	float denom = planeNormal.dotproduct(rayDirection);
	
	if (denom == 0)  // normal is orthogonal to _vector_, cant intersect
		return (-1.0f);
	
	return -(numer / denom);	
}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from 
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------  

IC void closestPointOnLine(Fvector& res, const Fvector& a, const Fvector& b, const Fvector& p) 
{
	
	// Determine t (the length of the _vector_ from ‘a’ to ‘p’)
	Fvector c; c.sub(p,a);
	Fvector V; V.sub(b,a); 
	
	float d = V.magnitude();
	
	V.div(d);  
	float t = V.dotproduct(c);
	
	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t <= 0.0f)	{ res.set(a); return; }
	if (t >= d)		{ res.set(b); return; }
	
	// Return the point between ‘a’ and ‘b’
	// set length of V to t. V is normalized so this is easy
	res.mad(a,V,t);
}
IC void closestPointOnEdge(Fvector& res,						// result
						   const Fvector& a, const Fvector& b,	// points
						   const Fvector& ED, float elen,		// edge direction (b-a) and length
						   const Fvector& P)					// query point
{
	// Determine t (the length of the _vector_ from ‘a’ to ‘p’)
	Fvector c; c.sub(P,a);
	float t = ED.dotproduct(c);
	
	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t <= 0.0f)	{ res.set(a); return; }
	if (t >= elen)	{ res.set(b); return; }
	
	// Return the point between ‘a’ and ‘b’
	res.mad(a,ED,t);
}

// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
//         p - point we wish to find closest point on triangle from 
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------  

IC void closestPointOnTriangle(Fvector& result, const Fvector* V, const Fvector& p) 
{
	Fvector Rab; closestPointOnLine(Rab, V[0], V[1], p); float dAB = p.distance_to_sqr(Rab);
	Fvector Rbc; closestPointOnLine(Rbc, V[1], V[2], p); float dBC = p.distance_to_sqr(Rbc);
	Fvector Rca; closestPointOnLine(Rca, V[2], V[0], p); float dCA = p.distance_to_sqr(Rca);
	
	float min = dAB;
	result.set(Rab);
	if (dBC < min) {
		min = dBC;
		result.set(Rbc);
    }
	if (dCA < min)
		result.set(Rca);
}
IC void closestPointOnTriangle(Fvector& result, const tri& T, const Fvector& P) 
{
	Fvector Rab; closestPointOnEdge(Rab, T.p[0], T.p[1], T.e10, T.e10s, P); float dAB = P.distance_to_sqr(Rab);
	Fvector Rbc; closestPointOnEdge(Rbc, T.p[1], T.p[2], T.e21, T.e21s, P); float dBC = P.distance_to_sqr(Rbc);
	Fvector Rca; closestPointOnEdge(Rca, T.p[2], T.p[0], T.e02, T.e02s, P); float dCA = P.distance_to_sqr(Rca);
	
	float min;
	if (dBC < dAB)	{ min = dBC; result.set(Rbc); } 
	else			{ min = dAB; result.set(Rab); }
	if (dCA < min)	result.set(Rca);
}

// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - _vector_ describing direction of ray in world space
//         sO - Origin of sphere 
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------  

IC float intersectRaySphere(const Fvector& rO, const Fvector& rV, const Fvector& sO, float sR) 
{
	Fvector Q;
	Q.sub(sO,rO);
	
	float c = Q.magnitude();
	float v = Q.dotproduct(rV);
	float d = sR*sR - (c*c - v*v);
	
	// If there was no intersection, return -1
	if (d < 0.0) return (-1.0f);
	
	// Return the distance to the [first] intersecting point
	return (v - _sqrt(d));
}
IC float intersectRayIdentitySphere(const Fvector& rO, const Fvector& rV) 
{
	Fvector Q;
	Q.invert(rO);
	
	float c = Q.magnitude();
	float v = Q.dotproduct(rV);
	float d = 1 - (c*c - v*v);
	
	// If there was no intersection, return -1
	if (d < 0.0) return (-1.0f);
	
	// Return the distance to the [first] intersecting point
	return (v - _sqrt(d));
}
// ----------------------------------------------------------------------
// Name  : CheckPointInSphere()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere 
// Notes : 
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------  
IC BOOL CheckPointInSphere(const Fvector& point, const Fvector& sO, float sR) 
{
	return (sO.distance_to_sqr(point)< sR*sR);
}
IC BOOL CheckPointInIdentitySphere(const Fvector& point) 
{ 
	return (point.square_magnitude() <= 1);
}

BOOL CObjectSpace::EllipsoidCollide( ICollisionForm *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bounding_box ){
	Fvector vRadius;
	bounding_box.getradius(vRadius);

	object->owner->setEnabled(false);
	BoxQuery				(bounding_box,T,
		clGET_TRIS|clGET_BOXES|clQUERY_TOPLEVEL|clQUERY_STATIC|clQUERY_DYNAMIC);
	object->owner->setEnabled(true);

	Fvector scaledPosition;	 
	Fvector inv_radius;
	inv_radius.set(1,1,1);
	inv_radius.div(vRadius);

	// the first thing we do is scale the player and his velocity to
	// ellipsoid space
	scaledPosition.mul(center_pos,inv_radius);
	
	Fvector p[3], e0,e1;

	// Get the data for the triangles in question and scale to ellipsoid space
	int tri_count = q_result.tris.size();
	for (int i_t=0; i_t<tri_count; i_t++){
		clQueryTri&	O = q_result.tris[i_t];
		
		p[0].mul(O.p[0],inv_radius);
		p[1].mul(O.p[1],inv_radius);
		p[2].mul(O.p[2],inv_radius);
		e0.sub(p[1],p[0]);
		e1.sub(p[2],p[0]);
		if (CDB::TestSphereTri(scaledPosition, 1.f-EPS_L, p[0], e0, e1)) return TRUE;
	}
	
	return FALSE;
}
