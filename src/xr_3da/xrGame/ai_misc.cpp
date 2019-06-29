////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_misc.cpp
//	Created 	: 21.03.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : AI miscellanious functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"
#include "LevelGameDef.h"

using namespace AI;

extern void	UnpackContour		(PContour& C, u32 ID);
extern void	IntersectContours	(PSegment& Dest, PContour& C1, PContour& C2);

#define	LI_NONE				0
#define LI_COLLINEAR        0
#define	LI_INTERSECT		1
#define LI_EQUAL	        2

#pragma todo("Jim to Dima: IBaseAI_NodeEvaluator uses large amount of memory. Currently 372 bytes!!!")

IC int lines_intersect( 
			float x1, float y1,		/* First line segment */
			float x2, float y2,

			float x3, float y3,		/* Second line segment */
			float x4, float y4,

			float *x, float *y      /* Output value: point of intersection */
			)
{
	float a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
	float r1, r2, r3, r4;			/* 'Sign' values */
	float denom, num;				/* Intermediate values */
	
									/* Compute a1, b1, c1, where line joining points 1 and 2
									* is "a1 x  +  b1 y  +  c1  =  0".
	*/
	
	a1 = y2 - y1;
	b1 = x1 - x2;
	c1 = x2 * y1 - x1 * y2;
	
	/* Compute r3 and r4.
	*/
	
	r3 = a1 * x3 + b1 * y3 + c1;
	r4 = a1 * x4 + b1 * y4 + c1;
	
	/* Check signs of r3 and r4.  If both point 3 and point 4 lie on
	* same side of line 1, the line segments do not intersect.
	*/
	
	if ( r3*r4 > 0 )      return ( LI_NONE );
	
	/* Compute a2, b2, c2 */
	
	a2 = y4 - y3;
	b2 = x3 - x4;
	c2 = x4 * y3 - x3 * y4;
	
	/* Compute r1 and r2 */
	
	r1 = a2 * x1 + b2 * y1 + c2;
	r2 = a2 * x2 + b2 * y2 + c2;
	
	/* Check signs of r1 and r2.  If both point 1 and point 2 lie
	* on same side of second line segment, the line segments do
	* not intersect.
	*/
	
	if ( r1*r2 > 0 )		return ( LI_NONE );
	
	// Check for equality
	if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) return LI_EQUAL;

	/* Line segments intersect: compute intersection point. 
	*/
	
	denom = a1 * b2 - a2 * b1;
	if ( _abs(denom) < EPS ) return ( LI_COLLINEAR );
	
	num = b1 * c2 - b2 * c1;
	*x = num / denom;
	
	num = a2 * c1 - a1 * c2;
	*y = num / denom;
	
	return ( LI_INTERSECT );
} /* lines_intersect */

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
}

IC bool bfInsideContour(Fvector &tPoint, PContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC void vfIntersectContours(PSegment &tSegment, PContour &tContour0, PContour &tContour1)
{
	bool bFound = false;
	
	if (bfInsideContour(tContour0.v1,tContour1)) {
		tSegment.v1 = tContour0.v1;
		bFound = true;
	}

	if (bfInsideContour(tContour0.v2,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v2;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v2;
			return;
		}
	}
	if (bfInsideContour(tContour0.v3,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v3;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v3;
			return;
		}
	}
	if (bfInsideContour(tContour0.v4,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v4;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v4;
			return;
		}
	}
	if (bFound) {
		if (bfInsideContour(tContour1.v1,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v1)))) {
			tSegment.v2 = tContour1.v1;
			return;
		}
		if (bfInsideContour(tContour1.v2,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v2)))) {
			tSegment.v2 = tContour1.v2;
			return;
		}
		if (bfInsideContour(tContour1.v3,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v3)))) {
			tSegment.v2 = tContour1.v3;
			return;
		}
		if (bfInsideContour(tContour1.v4,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v4)))) {
			tSegment.v2 = tContour1.v4;
			return;
		}
	}
	else {
		if (bfInsideContour(tContour1.v1,tContour0)) {
			tSegment.v1 = tContour1.v1;
			bFound = true;
		}
		if (bfInsideContour(tContour1.v2,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v2;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v2;
				return;
			}
		}
		if (bfInsideContour(tContour1.v3,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v3;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v3;
				return;
			}
		}
		if (bfInsideContour(tContour1.v4,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v4;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v4;
				return;
			}
		}
	}

	if (bFound) {
		tSegment.v2 = tSegment.v1;
		Log("! AI_PathNodes: segment has null length");
	}
	else
		Log("! AI_PathNodes: Can't find intersection segment");
}

void vfComputeCircle(Fvector tPosition, Fvector tPoint0, Fvector tPoint1, float &fRadius, Fvector &tCircleCentre, Fvector &tFinalPosition, float &fBeta)
{
	Fvector tP0, tP1;
	float fAlpha, fSinus, fCosinus, fRx;

	tP0.sub(tPosition,tPoint0);
	tP1.sub(tPoint1,tPoint0);
	fRx = tP0.magnitude();
	tP0.normalize();
	tP1.normalize();

	clamp(fAlpha = tP0.dotproduct(tP1),-0.9999999f,0.9999999f);
	fAlpha = .5f*(fBeta = acosf(fAlpha));
	fBeta = PI - fBeta;

	tP0.mul(fRx);
	tP1.mul(fRx);
	
	tFinalPosition = tP1;
	tFinalPosition.add(tPoint0);
	
	tCircleCentre.add(tP0,tP1);
	fSinus			= _sin(fAlpha);
	fCosinus		= _cos(fAlpha);
	fRadius			= fRx*fSinus/fCosinus;
	fRx				= fRadius*(1.f/fSinus - 1.f);
	tCircleCentre.mul((fRadius + fRx)/tCircleCentre.magnitude());
	tCircleCentre.add(tPoint0);
}

void CAI_Space::vfChoosePoint(Fvector &tStartPoint, Fvector &tFinishPoint, PContour	&tCurContour, int iNodeIndex, Fvector &tTempPoint, int &iSavedIndex)
{
	PContour					tNextContour;
	PSegment					tNextSegment;
	Fvector						tCheckPoint1 = tStartPoint, tCheckPoint2 = tStartPoint, tIntersectPoint;
	UnpackContour				(tNextContour,iNodeIndex);
	vfIntersectContours			(tNextSegment,tNextContour,tCurContour);
	u32							dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tNextSegment.v1.x,tNextSegment.v1.z,tNextSegment.v2.x,tNextSegment.v2.z,&tIntersectPoint.x,&tIntersectPoint.z);
	if (!dwIntersect)
		return;
	for (int i=0; i<4; i++) {
		switch (i) {
			case 0 : {
				tCheckPoint1	= tNextContour.v1;
				tCheckPoint2	= tNextContour.v2;
				break;
			}
			case 1 : {
				tCheckPoint1	= tNextContour.v2;
				tCheckPoint2	= tNextContour.v3;
				break;
			}
			case 2 : {
				tCheckPoint1	= tNextContour.v3;
				tCheckPoint2	= tNextContour.v4;
				break;
			}
			case 3 : {
				tCheckPoint1	= tNextContour.v4;
				tCheckPoint2	= tNextContour.v1;
				break;
			}
			default : NODEFAULT;
		}
		dwIntersect				= lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tCheckPoint1.x,tCheckPoint1.z,tCheckPoint2.x,tCheckPoint2.z,&tIntersectPoint.x,&tIntersectPoint.z);
		if (dwIntersect == LI_INTERSECT) {
			if (tFinishPoint.distance_to_xz(tIntersectPoint) < tFinishPoint.distance_to_xz(tTempPoint) + EPS_L) {
				tTempPoint = tIntersectPoint;
				iSavedIndex = iNodeIndex;
			}
		}
		else
			if (dwIntersect == LI_EQUAL) {
				if (tStartPoint.distance_to_xz(tCheckPoint1) > tStartPoint.distance_to_xz(tTempPoint))
					if (tStartPoint.distance_to_xz(tCheckPoint1) > tStartPoint.distance_to_xz(tCheckPoint2)) {
						tTempPoint = tCheckPoint1;
						iSavedIndex = iNodeIndex;
					}
					else {
						tTempPoint = tCheckPoint2;
						iSavedIndex = iNodeIndex;
					}
				else
					if (tStartPoint.distance_to_xz(tCheckPoint2) > tStartPoint.distance_to_xz(tTempPoint)) {
						tTempPoint = tCheckPoint2;
						iSavedIndex = iNodeIndex;
					}

			}
	}
}

//void CAI_Space::vfCreateFastRealisticPath(xr_vector<Fvector> &tpaPoints, u32 dwStartNode, xr_vector<Fvector> &tpaDeviations, xr_vector<Fvector> &tpaPath, xr_vector<u32> &dwaNodes, bool bLooped, bool bUseDeviations, float fRoundedDistanceMin, float fRoundedDistanceMax, float fRadiusMin, float fRadiusMax, float fSuitableAngle, float fSegmentSizeMin, float fSegmentSizeMax)
//{
//	Fvector tTravelNode;
//	Fvector tPrevPrevPoint,tTempPoint, tPrevPoint, tStartPoint, tFinishPoint, tCurrentPosition, tCircleCentre, tFinalPosition, t1, t2;
//	NodeCompressed *tpNode;
//	NodeLink *taLinks;
//	PContour tCurContour, tNextContour;
//	PSegment tSegment;
//	u32 dwCurNode, dwPrevNode, dwPrevPrevNode;
//	int i, j, iCurrentPatrolPoint, iCount, iNodeIndex, iSavedIndex = -1, iStartI;
//	float fSuitAngleCosinus = _cos(fSuitableAngle), fHalfSubNodeSize = (Header().size)*.5f, fSegmentSize, fDistance, fRadius, fAlpha0, fAlpha, fTemp, fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax), fPreviousRoundedDistance = fRoundedDistance;
//	bool bStop = false, bOk = false;
//
//	// init deviation points
//	tpaDeviations[0].set(0,0,0);
//	for ( i=1; i<(int)tpaDeviations.size(); i++) {
//		fRadius = ::Random.randF(fRadiusMin,fRadiusMax);
//		fAlpha = ::Random.randF(0.f,PI_MUL_2);
//		_sincos(fAlpha,fAlpha0,fTemp);
//		if (bUseDeviations)
//			tpaDeviations[i].set(fTemp*fRadius,0,fAlpha0*fRadius);
//		else
//			tpaDeviations[i].set(0,0,0);
//		tTempPoint.add(tpaPoints[i],tpaDeviations[i]);
//	}
//	
//	if (!bLooped)
//		tpaDeviations[tpaDeviations.size() - 1].set(0,0,0);
//		
//	// init start data
//	tpaPath.clear();
//	dwaNodes.clear();
//	iCurrentPatrolPoint = 1;
//	tStartPoint.add(tpaPoints[0],tpaDeviations[0]);
//	tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
//	
//	dwPrevNode = u32(-1);
//	dwaNodes.push_back(dwStartNode);
//	if (!bLooped)
//		tpaPath.push_back(tStartPoint);
//	dwCurNode = dwStartNode;
//	tTempPoint = tTravelNode = tPrevPoint = tStartPoint;
//	
//	fDistance = tStartPoint.distance_to_xz(tFinishPoint);
//
//	while (!bStop) {
//		
//		do {
//			// if distance to corner is small enough - round the corner
//			if (tpaPath.size() > 768) {
//				//VERIFY(false);
//				tpaPath.clear();
//				return;
//			}
//			if (((tPrevPoint.distance_to_xz(tFinishPoint) - fRoundedDistance < EPS_L))) {
//				if ((!bLooped) && (iCurrentPatrolPoint == (int)tpaPoints.size() - 1)) {
//					bStop = true;
//					break;
//				}
//				iStartI = tpaPath.size() - 1;
//				dwPrevPrevNode = dwPrevNode;
//				dwPrevNode = dwCurNode;
//				tPrevPrevPoint = tpaPath[tpaPath.size() - 1];
//				tCurrentPosition = tFinishPoint;
//				tCurrentPosition.sub(tStartPoint);
//				tCurrentPosition.normalize();
//				tCurrentPosition.mul(fDistance - fRoundedDistance);
//				tCurrentPosition.add(tStartPoint);
//				tTravelNode = tCurrentPosition;
//				tpaPath[iStartI] = tTravelNode;
//				tPrevPoint = tCurrentPosition;
//				vfComputeCircle(tCurrentPosition,tFinishPoint,tpaPoints[iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0],fRadius,tCircleCentre,tFinalPosition,fAlpha0);
//				// build circle points
//				fSegmentSize = 2*fRadius*fRadius*(1 - fSuitAngleCosinus);
//				if ((fRadius < 50.f) && (fSuitableAngle < fAlpha0) && (fSegmentSize < tPrevPoint.distance_to(tpaPoints[iCurrentPatrolPoint])) && (fSegmentSize < .5f*tpaPoints[iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0].distance_to(tpaPoints[iCurrentPatrolPoint]))) {
//					fAlpha0 = fSuitableAngle;
//					do {
//						tCurrentPosition.sub(tCircleCentre);
//						tCurrentPosition.normalize();
//						clamp(tCurrentPosition.x,-0.9999999f,0.9999999f);
//						if (tCurrentPosition.z > 0)
//							fAlpha = acosf(tCurrentPosition.x);
//						else
//							fAlpha = -acosf(tCurrentPosition.x);
//						fTemp = fAlpha - fAlpha0;
//						_sincos(fTemp,tCurrentPosition.z,tCurrentPosition.x);
//						tCurrentPosition.mul(fRadius);
//						tCurrentPosition.add(tCircleCentre);
//						if (tPrevPoint.distance_to_xz(tFinalPosition) < tCurrentPosition.distance_to_xz(tFinalPosition)) {
//							tCurrentPosition = tPrevPoint;
//							tCurrentPosition.sub(tCircleCentre);
//							tCurrentPosition.normalize();
//							fTemp = fAlpha + fAlpha0;
//							_sincos(fTemp,tCurrentPosition.z,tCurrentPosition.x);
//							tCurrentPosition.mul(fRadius);
//							tCurrentPosition.add(tCircleCentre);
//						}
//						// checking for stop to round
//						t1.sub(tCurrentPosition,tFinalPosition);
//						t2.sub(tCurrentPosition,tPrevPoint);
//						t1.normalize();
//						t2.normalize();
//						clamp(fAlpha = t1.dotproduct(t2),-0.9999999f,0.9999999f);
//						fAlpha = acosf(fAlpha);
//						if (fAlpha < PI/2)
//							break;
//						else {
//							tTravelNode = tCurrentPosition;
//							if ((!tpaPath.size()) || tCurrentPosition.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
//								tpaPath.push_back(tTravelNode);
//							tPrevPoint = tCurrentPosition;
//						}
//					}
//					while (true);
//				}
//					
//				// assign y-values to the circle points being built
//				j = iStartI;
//				dwCurNode = iNodeIndex = dwPrevNode;
//				if (bfInsideNode(tpNode = Node(iNodeIndex),tpaPath[j])) {
//					do	{
//						tpaPath[j].y = ffGetY(*(Node(iNodeIndex)),tpaPath[j].x,tpaPath[j].z);
//						j++;
//					}
//					while ((j<(int)tpaPath.size()) && (bfInsideNode(tpNode,tpaPath[j])));
//					dwCurNode = iNodeIndex;
//				}
//				while (j < (int)tpaPath.size()) {
//					tpNode = Node(dwCurNode);
//					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
//					iCount = tpNode->links;
//					for ( i=0; i < iCount; i++) {
//						iNodeIndex = UnpackLink(taLinks[i]);
//						if (bfInsideNode(tpNode = Node(iNodeIndex),tpaPath[j])) {
//							do	{
//								tpaPath[j].y = ffGetY(*(Node(iNodeIndex)),tpaPath[j].x,tpaPath[j].z);
//								tPrevPoint = tpaPath[j - 1];
//								j++;
//							}
//							while ((j<(int)tpaPath.size()) && (bfInsideNode(tpNode,tpaPath[j])));
//							dwCurNode = iNodeIndex;
//							dwaNodes.push_back(iNodeIndex);
//							break;
//						}
//					}
//					if (i >= iCount) {
//						tStartPoint = tpaPath[j-1];
//						tFinishPoint = tpaPath[j];
//						UnpackContour(tCurContour,dwCurNode);
//						tpNode = Node(dwCurNode);
//						taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
//						iCount = tpNode->links;
//						for ( i=0; i < iCount; i++) {
//							iNodeIndex = UnpackLink(taLinks[i]);
//							UnpackContour(tNextContour,iNodeIndex);
//							vfIntersectContours(tSegment,tCurContour,tNextContour);
//							u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
//							if ((dwIntersect) && (tPrevPoint.distance_to_xz(tTravelNode) >= 2*EPS_L)) {
//								dwCurNode = iNodeIndex;
//								dwaNodes.push_back(iNodeIndex);
//								tPrevPoint = tFinishPoint;
//								j++;
//								break;
//							}
//						}
//						if (i >= iCount) {
//							CAI_NodeEvaluatorTemplate<aiSearchRange | aiInsideNode> tSearch;
//							tSearch.m_fSearchRange		= 16*fHalfSubNodeSize;
//							tSearch.m_dwStartNode		= dwCurNode;
//							tSearch.m_tStartPosition	= tPrevPoint;
//							tSearch.vfShallowGraphSearch(getAI().q_mark_bit_x);
//							//q_Range_Bit_X(dwCurNode,tPrevPoint,16*fHalfSubNodeSize,&tNodePosition,dwBestNode,fBestCost);
//							if (bfInsideNode(Node(tSearch.m_dwBestNode),tpaPath[j])) {
//								dwCurNode = tSearch.m_dwBestNode;
//								tpaPath[j].y = ffGetY(*(Node(dwCurNode)),tpaPath[j].x,tpaPath[j].z);
//								dwaNodes.push_back(dwCurNode);
//								tPrevPoint = tFinishPoint;
//								j++;
//								break;
//							}
//							tpaPath.clear();
//							return;
//						}
//					}
//				}
//				
//				// finding the node
//				iCurrentPatrolPoint = iCurrentPatrolPoint > 0 ? iCurrentPatrolPoint - 1 : tpaPoints.size() - 1;
//				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
//				iCurrentPatrolPoint = iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
//				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
//				fDistance = tStartPoint.distance_to_xz(tFinishPoint);
//				dwCurNode = dwPrevNode;
//				dwPrevNode = dwPrevPrevNode;
//				tTravelNode = tPrevPoint = tPrevPrevPoint;
//
//				iSavedIndex = -1;
//				while (!bfInsideNode(Node(dwCurNode),tFinishPoint)) {
//					UnpackContour(tCurContour,dwCurNode);
//					tpNode = Node(dwCurNode);
//					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
//					iCount = tpNode->links;
//					iSavedIndex = -1;
//					tTempPoint = tStartPoint;
//					for ( i=0; i < iCount; i++) {
//						iNodeIndex = UnpackLink(taLinks[i]);
//						UnpackContour(tNextContour,iNodeIndex);
//						vfIntersectContours(tSegment,tCurContour,tNextContour);
//						u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
//						if (dwIntersect == LI_INTERSECT) {
//							if (
//								(tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) &&
//								(iNodeIndex != (int)dwPrevNode)
//								) {
//								tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//								tTempPoint = tTravelNode;
//								iSavedIndex = iNodeIndex;
//							}
//						}
//						else
//							if (dwIntersect == LI_EQUAL) {
//								if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
//									if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
//										tTempPoint = tSegment.v1;
//										iSavedIndex = iNodeIndex;
//									}
//									else {
//										tTempPoint = tSegment.v2;
//										iSavedIndex = iNodeIndex;
//									}
//								else
//									if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
//										tTempPoint = tSegment.v2;
//										iSavedIndex = iNodeIndex;
//									}
//
//							}
//					}
//
//					if (iSavedIndex > -1) {
//						tPrevPoint = tTravelNode = tTempPoint;
//						if (tFinishPoint.distance_to_xz(tTempPoint) < EPS_L)
//							tFinishPoint = tTempPoint;
//						dwPrevNode = dwCurNode;
//						dwCurNode = iSavedIndex;
//						dwaNodes.push_back(iSavedIndex);
//					}
//					else
//						if (bfInsideNode(tpNode,tFinishPoint)) {
//							tTravelNode = tFinishPoint;
//							tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//							tPrevPoint = tTravelNode;
//							dwPrevNode = dwCurNode;
//							break;
//						}
//						else {
//							//VERIFY(false);
//							tpaPath.clear();
//							return;
//						}
//				}
//				/**/
//
//				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
//				iCurrentPatrolPoint = iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
//				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
//				fDistance = tStartPoint.distance_to_xz(tFinishPoint);
//				tTempPoint = tTravelNode = tPrevPoint = tStartPoint;
//
//				bStop = iCurrentPatrolPoint == 1;
//
//				fPreviousRoundedDistance = fRoundedDistance;
//				if (bLooped)
//					fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
//				else
//					if (iCurrentPatrolPoint != (int)tpaPoints.size() - 1)
//						fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
//					else
//						fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
//
//				dwPrevNode = u32(-1);
//				
//				break;
//			}
//			else {
//				UnpackContour(tCurContour,dwCurNode);
//				tpNode = Node(dwCurNode);
//				taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
//				iCount = tpNode->links;
//				iSavedIndex = -1;
//				tTempPoint = tStartPoint;
//				for ( i=0; i < iCount; i++) {
//					iNodeIndex = UnpackLink(taLinks[i]);
//					UnpackContour(tNextContour,iNodeIndex);
//					vfIntersectContours(tSegment,tCurContour,tNextContour);
//					u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
//					if (dwIntersect == LI_INTERSECT) {
//						if (
//							(tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) &&
//							(iNodeIndex != (int)dwPrevNode)
//							) {
//							tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//							tTempPoint = tTravelNode;
//							iSavedIndex = iNodeIndex;
//						}
//					}
//					else
//						if (dwIntersect == LI_EQUAL) {
//							if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
//								if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
//									tTempPoint = tSegment.v1;
//									iSavedIndex = iNodeIndex;
//								}
//								else {
//									tTempPoint = tSegment.v2;
//									iSavedIndex = iNodeIndex;
//								}
//							else
//								if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
//									tTempPoint = tSegment.v2;
//									iSavedIndex = iNodeIndex;
//								}
//
//						}
//				}
//
//				if (iSavedIndex > -1) {
//					tTravelNode = tTempPoint;
//					
//					if ((tPrevPoint.distance_to_xz(tStartPoint) >= fPreviousRoundedDistance) || 
//						((!bLooped) && (iCurrentPatrolPoint == 1))) {
//						if ((!tpaPath.size()) || tTravelNode.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
//							/**
//							if (_abs(tTravelNode.y - tPrevPoint.y) > 1.f/256.f)
//								tpaPath.push_back(tTravelNode);
//							else
//								tpaPath[tpaPath.size() - 1] = tTravelNode;
//							/**/
//							tpaPath.push_back(tTravelNode);
//						else
//							tpaPath.push_back(tTravelNode);
//					}
//					
//					tPrevPoint = tTravelNode;
//					dwPrevNode = dwCurNode;
//					dwCurNode = iSavedIndex;
//					dwaNodes.push_back(iSavedIndex);
//				}
//				else
//					if (bfInsideNode(tpNode,tFinishPoint)) {
//						tTravelNode = tFinishPoint;
//						tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//						tPrevPoint = tTravelNode;
//						dwPrevNode = dwCurNode;
//						break;
//					}
//					else {
//						// Ooops! there is no proper neighbour node, try to search the nearest
//						// nodes for the closest to the current node intersection with the 
//						// line Start -> Finish
//						tTempPoint = tPrevPoint;
//						bOk = false;
//						for ( i=0; i < iCount; i++) {
//							iNodeIndex = UnpackLink(taLinks[i]);
//							if ((int)dwPrevNode == iNodeIndex)
//								continue;
//							UnpackContour(tNextContour,iNodeIndex);
//							for ( j=0; j<4; j++) {
//								switch(j) {
//									case 0: {
//										tSegment.v1 = tNextContour.v1;
//										tSegment.v2 = tNextContour.v2;
//										break;
//									}
//									case 1: {
//										tSegment.v1 = tNextContour.v2;
//										tSegment.v2 = tNextContour.v3;
//										break;
//									}
//									case 2: {
//										tSegment.v1 = tNextContour.v3;
//										tSegment.v2 = tNextContour.v4;
//										break;
//									}
//									case 3: {
//										tSegment.v1 = tNextContour.v4;
//										tSegment.v2 = tNextContour.v1;
//										break;
//									}
//									default: {
//										tSegment.v1 = tNextContour.v4;
//										tSegment.v2 = tNextContour.v1;
//									};
//								}
//								u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
//								if (dwIntersect == LI_INTERSECT) {
//									if (tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) {
//										if (bOk)
//											break;
//										tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//										tTempPoint = tTravelNode;
//										iSavedIndex = iNodeIndex;
//										bOk = true;
//									}
//									else 
//										if (bOk) {
//											tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
//											tTempPoint = tTravelNode;
//											iSavedIndex = iNodeIndex;
//											break;
//										}
//									
//								}
//								else
//									if (dwIntersect == LI_EQUAL) {
//										if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
//											if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
//												tTempPoint = tSegment.v1;
//												iSavedIndex = iNodeIndex;
//											}
//											else {
//												tTempPoint = tSegment.v2;
//												iSavedIndex = iNodeIndex;
//											}
//										else
//											if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
//												tTempPoint = tSegment.v2;
//												iSavedIndex = iNodeIndex;
//											}
//
//									}
//							}
//							if (bOk)
//								break;
//						}
//						if (iSavedIndex > -1) {
//							tTravelNode = tTempPoint;
//							if ((!tpaPath.size()) || tTravelNode.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
//								if (_abs(tTravelNode.y - tPrevPoint.y) > 1.f/256.f)
//									tpaPath.push_back(tTravelNode);
//								else
//									tpaPath[tpaPath.size() - 1] = tTravelNode;
//							//if ((tPrevPoint.distance_to_xz(tStartPoint) >= fPreviousRoundedDistance) || ((!bLooped) && (iCurrentPatrolPoint == 1)))
//							//	tpaPath.push_back(tTravelNode);
//							tPrevPoint = tTravelNode;
//							dwPrevNode = dwCurNode;
//							dwCurNode = iSavedIndex;
//							dwaNodes.push_back(iSavedIndex);
//						}
//						else {
//							//VERIFY(false);
//							//bool bTemp = bfCheckNodeInDirection(q_LoadSearch(tStartPoint),tStartPoint,q_LoadSearch(tFinishPoint));
//							tpaPath.clear();
//							return;
//						}
//					}
//			}
//		}
//		while (true);
//	}
//	// close the path for looped ones
//	/**/
//	if (bLooped) {
//		tpaPath.push_back(tpaPath[0]);
//		dwaNodes.push_back(dwaNodes[0]);
//	}
//	/**/
//
//	/**/
//	// check path for y-values - this is because of the bug I didn't fix
//	for ( i=0, j=0; i<(int)tpaPath.size(); i++) {
//		int k=j;
//		for ( ; (j < (int)dwaNodes.size()) && (!bfInsideNode(Node(dwaNodes[j]),tpaPath[i])); j++) ;
//		if (j >= (int)dwaNodes.size()) {
//			j=k;
//			tpaPath.erase(tpaPath.begin() + i);
//			i--;
//			continue;
//		}
//		//R_ASSERT(j < dwaNodes.size());
//		tpaPath[i].y = ffGetY(*(Node(dwaNodes[j])),tpaPath[i].x,tpaPath[i].z);
//	}
//	
//	/**
//	// check path for incorrect points - this is because of the bug I didn't fix
//	for ( i=1; i<tpaPath.size() - 1; i++) {
//		Fvector tPrevious = tpaPath[i-1];
//		Fvector tCurrent = tpaPath[i];
//		Fvector tNext = tpaPath[i+1];
//		Fvector tTemp1, tTemp2;
//		tTemp1.sub(tCurrent,tPrevious);
//		tTemp2.sub(tNext,tCurrent);
//		tTemp1.normalize_safe();
//		tTemp1.y = tTemp2.y = 0;
//		tTemp2.normalize_safe();
//		float fAlpha = tTemp1.dotproduct(tTemp2);
//		clamp(fAlpha, -.99999f, .99999f);
//		if (acosf(fAlpha) > PI_DIV_2) {
//			tpaPath.erase(tpaPath.begin() + i);
//			i--;
//		}
//	}
//	/**/
//	for ( i=2; i<(int)tpaPath.size(); i++) {
//		if (tpaPath[i].distance_to_xz(tpaPath[i-2]) <= fHalfSubNodeSize + tpaPath[i-1].distance_to_xz(tpaPath[i-2])) {
//			Fvector tPrevious = tpaPath[i-2];
//			Fvector tCurrent = tpaPath[i-1];
//			Fvector tNext = tpaPath[i];
//			Fvector tTemp1, tTemp2;
//			tTemp1.sub(tCurrent,tPrevious);
//			tTemp2.sub(tNext,tCurrent);
//			tTemp1.normalize_safe();
//			tTemp1.y = tTemp2.y = 0;
//			tTemp2.normalize_safe();
//			float fAlpha = tTemp1.dotproduct(tTemp2);
//			clamp(fAlpha, -.99999f, +.99999f);
//			if ((acosf(fAlpha) > PI_DIV_8*.375f) && (acosf(fAlpha) < 2*PI_DIV_8*.375f))
//				continue;
//			else {
//				tpaPath.erase(tpaPath.begin() + i);
//				i--;
//			}
//		}
//	}
//	/**/
//}

void CAI_Space::vfCreateFastRealisticPath(xr_vector<Fvector> &tpaPoints, u32 dwStartNode, xr_vector<Fvector> &tpaDeviations, xr_vector<Fvector> &tpaPath, xr_vector<u32> &dwaNodes, bool bLooped, bool bUseDeviations, float fRoundedDistanceMin, float fRoundedDistanceMax, float fRadiusMin, float fRadiusMax, float fSuitableAngle, float fSegmentSizeMin, float fSegmentSizeMax)
{
	Fvector tTravelNode;
	Fvector tPrevPrevPoint,tTempPoint, tPrevPoint, tStartPoint, tFinishPoint, tCurrentPosition, tCircleCentre, tFinalPosition, t1, t2;
	NodeCompressed *tpNode;
	NodeLink *taLinks;
	PContour tCurContour, tNextContour;
	PSegment tSegment;
	u32 dwCurNode, dwPrevNode, dwPrevPrevNode;
	int i, j, iCurrentPatrolPoint, iCount, iNodeIndex, iSavedIndex = -1, iStartI;
	float fSuitAngleCosinus = _cos(fSuitableAngle), fHalfSubNodeSize = (Header().size)*.5f, fSegmentSize, fDistance, fRadius, fAlpha0, fAlpha, fTemp, fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax), fPreviousRoundedDistance = fRoundedDistance;
	bool bStop = false, bOk = false;

	// init deviation points
	tpaDeviations[0].set(0,0,0);
	for ( i=1; i<(int)tpaDeviations.size(); i++) {
		fRadius = ::Random.randF(fRadiusMin,fRadiusMax);
		fAlpha	= ::Random.randF(0.f,PI_MUL_2);
		fAlpha0	= _sin(fAlpha);
		fTemp	= _cos(fAlpha);
		if (bUseDeviations)
			tpaDeviations[i].set(fTemp*fRadius,0,fAlpha0*fRadius);
		else
			tpaDeviations[i].set(0,0,0);
		tTempPoint.add(tpaPoints[i],tpaDeviations[i]);
	}
	
	if (!bLooped)
		tpaDeviations[tpaDeviations.size() - 1].set(0,0,0);
		
	// init start data
	tpaPath.clear();
	dwaNodes.clear();
	iCurrentPatrolPoint = 1;
	tStartPoint.add(tpaPoints[0],tpaDeviations[0]);
	tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
	
	dwPrevNode = u32(-1);
	dwaNodes.push_back(dwStartNode);
	if (!bLooped)
		tpaPath.push_back(tStartPoint);
	dwCurNode = dwStartNode;
	tTempPoint = tTravelNode = tPrevPoint = tStartPoint;
	
	fDistance = tStartPoint.distance_to_xz(tFinishPoint);

	while (!bStop) {
		
		do {
			// if distance to corner is small enough - round the corner
			if (tpaPath.size() > 768) {
				//VERIFY(false);
				tpaPath.clear();
				return;
			}
			if (((tPrevPoint.distance_to_xz(tFinishPoint) - fRoundedDistance < EPS_L))) {
				if ((!bLooped) && (iCurrentPatrolPoint == (int)tpaPoints.size() - 1)) {
					bStop = true;
					break;
				}
				iStartI = (int)tpaPath.size() - 1;
				dwPrevPrevNode = dwPrevNode;
				dwPrevNode = dwCurNode;
				tPrevPrevPoint = tpaPath[tpaPath.size() - 1];
				tCurrentPosition = tFinishPoint;
				tCurrentPosition.sub(tStartPoint);
				tCurrentPosition.normalize();
				tCurrentPosition.mul(fDistance - fRoundedDistance);
				tCurrentPosition.add(tStartPoint);
				tTravelNode = tCurrentPosition;
				tpaPath[iStartI] = tTravelNode;
				tPrevPoint = tCurrentPosition;
				vfComputeCircle(tCurrentPosition,tFinishPoint,tpaPoints[iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0],fRadius,tCircleCentre,tFinalPosition,fAlpha0);
				// build circle points
				fSegmentSize = 2*fRadius*fRadius*(1 - fSuitAngleCosinus);
				if ((fRadius < 50.f) && (fSuitableAngle < fAlpha0) && (fSegmentSize < tPrevPoint.distance_to(tpaPoints[iCurrentPatrolPoint])) && (fSegmentSize < .5f*tpaPoints[iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0].distance_to(tpaPoints[iCurrentPatrolPoint]))) {
					fAlpha0 = fSuitableAngle;
					do {
						tCurrentPosition.sub(tCircleCentre);
						tCurrentPosition.normalize();
						clamp(tCurrentPosition.x,-0.9999999f,0.9999999f);
						if (tCurrentPosition.z > 0)
							fAlpha = acosf(tCurrentPosition.x);
						else
							fAlpha = -acosf(tCurrentPosition.x);
						fTemp = fAlpha - fAlpha0;
						tCurrentPosition.z = _sin(fTemp);
						tCurrentPosition.x = _cos(fTemp);
						tCurrentPosition.mul(fRadius);
						tCurrentPosition.add(tCircleCentre);
						if (tPrevPoint.distance_to_xz(tFinalPosition) < tCurrentPosition.distance_to_xz(tFinalPosition)) {
							tCurrentPosition = tPrevPoint;
							tCurrentPosition.sub(tCircleCentre);
							tCurrentPosition.normalize();
							fTemp = fAlpha + fAlpha0;
							tCurrentPosition.z = _sin(fTemp);
							tCurrentPosition.x = _cos(fTemp);
							tCurrentPosition.mul(fRadius);
							tCurrentPosition.add(tCircleCentre);
						}
						// checking for stop to round
						t1.sub(tCurrentPosition,tFinalPosition);
						t2.sub(tCurrentPosition,tPrevPoint);
						t1.normalize();
						t2.normalize();
						clamp(fAlpha = t1.dotproduct(t2),-0.9999999f,0.9999999f);
						fAlpha = acosf(fAlpha);
						if (fAlpha < PI/2)
							break;
						else {
							tTravelNode = tCurrentPosition;
							if ((!tpaPath.size()) || tCurrentPosition.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
								tpaPath.push_back(tTravelNode);
							tPrevPoint = tCurrentPosition;
						}
					}
					while (true);
				}
					
				// assign y-values to the circle points being built
				j = iStartI;
				dwCurNode = iNodeIndex = dwPrevNode;
				if (bfInsideNode(tpNode = Node(iNodeIndex),tpaPath[j])) {
					do	{
						tpaPath[j].y = ffGetY(*(Node(iNodeIndex)),tpaPath[j].x,tpaPath[j].z);
						j++;
					}
					while ((j<(int)tpaPath.size()) && (bfInsideNode(tpNode,tpaPath[j])));
					dwCurNode = iNodeIndex;
				}
				while (j < (int)tpaPath.size()) {
					tpNode = Node(dwCurNode);
					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
					iCount = tpNode->links;
					for ( i=0; i < iCount; i++) {
						iNodeIndex = UnpackLink(taLinks[i]);
						if (bfInsideNode(tpNode = Node(iNodeIndex),tpaPath[j])) {
							do	{
								tpaPath[j].y = ffGetY(*(Node(iNodeIndex)),tpaPath[j].x,tpaPath[j].z);
								tPrevPoint = tpaPath[j - 1];
								j++;
							}
							while ((j<(int)tpaPath.size()) && (bfInsideNode(tpNode,tpaPath[j])));
							dwCurNode = iNodeIndex;
							dwaNodes.push_back(iNodeIndex);
							break;
						}
					}
					if (i >= iCount) {
						tStartPoint = tpaPath[j-1];
						tFinishPoint = tpaPath[j];
						UnpackContour(tCurContour,dwCurNode);
						tpNode = Node(dwCurNode);
						taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
						iCount = tpNode->links;
						for ( i=0; i < iCount; i++) {
							iNodeIndex = UnpackLink(taLinks[i]);
							UnpackContour(tNextContour,iNodeIndex);
							vfIntersectContours(tSegment,tCurContour,tNextContour);
							u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
							if ((dwIntersect) && (tPrevPoint.distance_to_xz(tTravelNode) >= 2*EPS_L)) {
								dwCurNode = iNodeIndex;
								dwaNodes.push_back(iNodeIndex);
								tPrevPoint = tFinishPoint;
								j++;
								break;
							}
						}
						if (i >= iCount) {
							CAI_NodeEvaluatorTemplate<aiSearchRange | aiInsideNode> tSearch;
							tSearch.m_fSearchRange		= 16*fHalfSubNodeSize;
							tSearch.m_dwStartNode		= dwCurNode;
							tSearch.m_tStartPosition	= tPrevPoint;
							tSearch.vfShallowGraphSearch(getAI().q_mark_bit_x);
							//q_Range_Bit_X(dwCurNode,tPrevPoint,16*fHalfSubNodeSize,&tNodePosition,dwBestNode,fBestCost);
							if (bfInsideNode(Node(tSearch.m_dwBestNode),tpaPath[j])) {
								dwCurNode = tSearch.m_dwBestNode;
								tpaPath[j].y = ffGetY(*(Node(dwCurNode)),tpaPath[j].x,tpaPath[j].z);
								dwaNodes.push_back(dwCurNode);
								tPrevPoint = tFinishPoint;
								j++;
								break;
							}
							tpaPath.clear();
							return;
						}
					}
				}
				
				// finding the node
				iCurrentPatrolPoint = iCurrentPatrolPoint > 0 ? iCurrentPatrolPoint - 1 : (int)tpaPoints.size() - 1;
				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				iCurrentPatrolPoint = iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				fDistance = tStartPoint.distance_to_xz(tFinishPoint);
				dwCurNode = dwPrevNode;
				dwPrevNode = dwPrevPrevNode;
				tTravelNode = tPrevPoint = tPrevPrevPoint;

				iSavedIndex = -1;
				while (!bfInsideNode(Node(dwCurNode),tFinishPoint)) {
					UnpackContour(tCurContour,dwCurNode);
					tpNode = Node(dwCurNode);
					taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
					iCount = tpNode->links;
					iSavedIndex = -1;
					tTempPoint = tStartPoint;
					for ( i=0; i < iCount; i++) {
						iNodeIndex = UnpackLink(taLinks[i]);
						UnpackContour(tNextContour,iNodeIndex);
						vfIntersectContours(tSegment,tCurContour,tNextContour);
						u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
						if (dwIntersect == LI_INTERSECT) {
							if (
								(tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) &&
								(iNodeIndex != (int)dwPrevNode)
								) {
								tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
								tTempPoint = tTravelNode;
								iSavedIndex = iNodeIndex;
							}
						}
						else
							if (dwIntersect == LI_EQUAL) {
								if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
									if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
										tTempPoint = tSegment.v1;
										iSavedIndex = iNodeIndex;
									}
									else {
										tTempPoint = tSegment.v2;
										iSavedIndex = iNodeIndex;
									}
								else
									if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
										tTempPoint = tSegment.v2;
										iSavedIndex = iNodeIndex;
									}

							}
					}

					if (iSavedIndex > -1) {
						tPrevPoint = tTravelNode = tTempPoint;
						if (tFinishPoint.distance_to_xz(tTempPoint) < EPS_L)
							tFinishPoint = tTempPoint;
						dwPrevNode = dwCurNode;
						dwCurNode = iSavedIndex;
						dwaNodes.push_back(iSavedIndex);
					}
					else
						if (bfInsideNode(tpNode,tFinishPoint)) {
							tTravelNode = tFinishPoint;
							tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
							tPrevPoint = tTravelNode;
							dwPrevNode = dwCurNode;
							break;
						}
						else {
							//VERIFY(false);
							tpaPath.clear();
							return;
						}
				}
				/**/

				tStartPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				iCurrentPatrolPoint = iCurrentPatrolPoint < (int)tpaPoints.size() - 1 ? iCurrentPatrolPoint + 1 : 0;
				tFinishPoint.add(tpaPoints[iCurrentPatrolPoint],tpaDeviations[iCurrentPatrolPoint]);
				fDistance = tStartPoint.distance_to_xz(tFinishPoint);
				tTempPoint = tTravelNode = tPrevPoint = tStartPoint;

				bStop = iCurrentPatrolPoint == 1;

				fPreviousRoundedDistance = fRoundedDistance;
				if (bLooped)
					fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
				else
					if (iCurrentPatrolPoint != (int)tpaPoints.size() - 1)
						fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);
					else
						fRoundedDistance = ::Random.randF(fRoundedDistanceMin,fRoundedDistanceMax);

				dwPrevNode = u32(-1);
				
				break;
			}
			else {
				UnpackContour(tCurContour,dwCurNode);
				tpNode = Node(dwCurNode);
				taLinks = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
				iCount = tpNode->links;
				iSavedIndex = -1;
				tTempPoint = tStartPoint;
				for ( i=0; i < iCount; i++) {
					iNodeIndex = UnpackLink(taLinks[i]);
					UnpackContour(tNextContour,iNodeIndex);
					vfIntersectContours(tSegment,tCurContour,tNextContour);
					u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
					if (dwIntersect == LI_INTERSECT) {
						if (
							(tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) &&
							(iNodeIndex != (int)dwPrevNode)
							) {
							tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
							tTempPoint = tTravelNode;
							iSavedIndex = iNodeIndex;
						}
					}
					else
						if (dwIntersect == LI_EQUAL) {
							if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
								if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
									tTempPoint = tSegment.v1;
									iSavedIndex = iNodeIndex;
								}
								else {
									tTempPoint = tSegment.v2;
									iSavedIndex = iNodeIndex;
								}
							else
								if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
									tTempPoint = tSegment.v2;
									iSavedIndex = iNodeIndex;
								}

						}
				}

				if (iSavedIndex > -1) {
					tTravelNode = tTempPoint;
					
					if ((tPrevPoint.distance_to_xz(tStartPoint) >= fPreviousRoundedDistance) || 
						((!bLooped) && (iCurrentPatrolPoint == 1))) {
						if ((!tpaPath.size()) || tTravelNode.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
							/**
							if (_abs(tTravelNode.y - tPrevPoint.y) > 1.f/256.f)
								tpaPath.push_back(tTravelNode);
							else
								tpaPath[tpaPath.size() - 1] = tTravelNode;
							/**/
							tpaPath.push_back(tTravelNode);
						else
							tpaPath.push_back(tTravelNode);
					}
					
					tPrevPoint = tTravelNode;
					dwPrevNode = dwCurNode;
					dwCurNode = iSavedIndex;
					dwaNodes.push_back(iSavedIndex);
				}
				else
					if (bfInsideNode(tpNode,tFinishPoint)) {
						tTravelNode = tFinishPoint;
						tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
						tPrevPoint = tTravelNode;
						dwPrevNode = dwCurNode;
						break;
					}
					else {
						// Ooops! there is no proper neighbour node, try to search the nearest
						// nodes for the closest to the current node intersection with the 
						// line Start -> Finish
						tTempPoint = tPrevPoint;
						bOk = false;
						for ( i=0; i < iCount; i++) {
							iNodeIndex = UnpackLink(taLinks[i]);
							if ((int)dwPrevNode == iNodeIndex)
								continue;
							UnpackContour(tNextContour,iNodeIndex);
							for ( j=0; j<4; j++) {
								switch(j) {
									case 0: {
										tSegment.v1 = tNextContour.v1;
										tSegment.v2 = tNextContour.v2;
										break;
									}
									case 1: {
										tSegment.v1 = tNextContour.v2;
										tSegment.v2 = tNextContour.v3;
										break;
									}
									case 2: {
										tSegment.v1 = tNextContour.v3;
										tSegment.v2 = tNextContour.v4;
										break;
									}
									case 3: {
										tSegment.v1 = tNextContour.v4;
										tSegment.v2 = tNextContour.v1;
										break;
									}
									default: {
										tSegment.v1 = tNextContour.v4;
										tSegment.v2 = tNextContour.v1;
									};
								}
								u32 dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tSegment.v1.x,tSegment.v1.z,tSegment.v2.x,tSegment.v2.z,&tTravelNode.x,&tTravelNode.z);
								if (dwIntersect == LI_INTERSECT) {
									if (tFinishPoint.distance_to_xz(tTravelNode) < tFinishPoint.distance_to_xz(tTempPoint) + EPS) {
										if (bOk)
											break;
										tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
										tTempPoint = tTravelNode;
										iSavedIndex = iNodeIndex;
										bOk = true;
									}
									else 
										if (bOk) {
											tTravelNode.y = ffGetY(*(tpNode),tTravelNode.x,tTravelNode.z);
											tTempPoint = tTravelNode;
											iSavedIndex = iNodeIndex;
											break;
										}
									
								}
								else
									if (dwIntersect == LI_EQUAL) {
										if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tTempPoint))
											if (tStartPoint.distance_to_xz(tSegment.v1) > tStartPoint.distance_to_xz(tSegment.v2)) {
												tTempPoint = tSegment.v1;
												iSavedIndex = iNodeIndex;
											}
											else {
												tTempPoint = tSegment.v2;
												iSavedIndex = iNodeIndex;
											}
										else
											if (tStartPoint.distance_to_xz(tSegment.v2) > tStartPoint.distance_to_xz(tTempPoint)) {
												tTempPoint = tSegment.v2;
												iSavedIndex = iNodeIndex;
											}

									}
							}
							if (bOk)
								break;
						}
						if (iSavedIndex > -1) {
							tTravelNode = tTempPoint;
							if ((!tpaPath.size()) || tTravelNode.distance_to_xz(tpaPath[tpaPath.size() - 1]) > fSegmentSizeMin)
								if (_abs(tTravelNode.y - tPrevPoint.y) > 1.f/256.f)
									tpaPath.push_back(tTravelNode);
								else
									tpaPath[tpaPath.size() - 1] = tTravelNode;
							//if ((tPrevPoint.distance_to_xz(tStartPoint) >= fPreviousRoundedDistance) || ((!bLooped) && (iCurrentPatrolPoint == 1)))
							//	tpaPath.push_back(tTravelNode);
							tPrevPoint = tTravelNode;
							dwPrevNode = dwCurNode;
							dwCurNode = iSavedIndex;
							dwaNodes.push_back(iSavedIndex);
						}
						else {
							//VERIFY(false);
							//bool bTemp = bfCheckNodeInDirection(q_LoadSearch(tStartPoint),tStartPoint,q_LoadSearch(tFinishPoint));
							tpaPath.clear();
							return;
						}
					}
			}
		}
		while (true);
	}
	
	// close the path for looped ones
	if (bLooped) {
		tpaPath.push_back(tpaPath[0]);
		dwaNodes.push_back(dwaNodes[0]);
	}

	// check path for y-values - this is because of the bug I didn't fix
	for ( i=0, j=0; i<(int)tpaPath.size(); i++) {
		int k=j;
		for ( ; (j < (int)dwaNodes.size()) && (!bfInsideNode(Node(dwaNodes[j]),tpaPath[i])); j++) ;
		if (j >= (int)dwaNodes.size()) {
			j=k;
			tpaPath.erase(tpaPath.begin() + i);
			i--;
			continue;
		}
		tpaPath[i].y = ffGetY(*(Node(dwaNodes[j])),tpaPath[i].x,tpaPath[i].z);
	}
	
	for ( i=2; i<(int)tpaPath.size(); i++) {
		if (tpaPath[i].distance_to_xz(tpaPath[i-2]) <= fHalfSubNodeSize + tpaPath[i-1].distance_to_xz(tpaPath[i-2])) {
			Fvector tPrevious = tpaPath[i-2];
			Fvector tCurrent = tpaPath[i-1];
			Fvector tNext = tpaPath[i];
			Fvector tTemp1, tTemp2;
			tTemp1.sub(tCurrent,tPrevious);
			tTemp2.sub(tNext,tCurrent);
			tTemp1.normalize_safe();
			tTemp1.y = tTemp2.y = 0;
			tTemp2.normalize_safe();
			float fAlpha = tTemp1.dotproduct(tTemp2);
			clamp(fAlpha, -.99999f, +.99999f);
			if ((acosf(fAlpha) > PI_DIV_8*.375f) && (acosf(fAlpha) < 2*PI_DIV_8*.375f))
				continue;
			else {
				tpaPath.erase(tpaPath.begin() + i);
				i--;
			}
		}
	}
}

bool CAI_Space::bfCheckNodeInDirection(u32 dwStartNode, Fvector tStartPosition, u32 dwFinishNode)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tStartPoint = tStartPosition, tTempPoint = tStartPosition, tFinishPoint = tfGetNodeCenter(dwFinishNode);;
	float					fCurDistance = 0.f, fDistance = tStartPosition.distance_to_xz(tfGetNodeCenter(dwFinishNode));
	u32						dwCurNode = dwStartNode;

	while ((dwCurNode != dwFinishNode) && (fCurDistance < (fDistance + EPS_L))) {
		if (bfInsideNode(Node(dwCurNode),tFinishPoint))
			break;
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
			iPrevIndex		= dwCurNode;
			dwCurNode		= iSavedIndex;
		}
		else
			return(false);
	}
	return(dwCurNode == dwFinishNode);
}

u32 CAI_Space::dwfCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tStartPoint = tStartPosition, tTempPoint = tStartPosition, tFinishPoint = tFinishPosition;
	float					fCurDistance = 0.f, fDistance = tStartPosition.distance_to_xz(tFinishPosition);
	u32						dwCurNode = dwStartNode;

	while (!bfInsideNode(Node(dwCurNode),tFinishPosition) && (fCurDistance < (fDistance + EPS_L))) {
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
			iPrevIndex		= dwCurNode;
			dwCurNode		= iSavedIndex;
		}
		else
			return((u32)(-1));
	}
	
	if (bfInsideNode(Node(dwCurNode),tFinishPosition) && (_abs(ffGetY(*Node(dwCurNode),tFinishPosition.x,tFinishPosition.z) - tFinishPosition.y) < .5f))
		return(dwCurNode);
	else
		return((u32)(-1));
}

float CAI_Space::ffMarkNodesInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tDirection, float fDistance, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks)
{
	Fvector					tFinishPoint;
	tDirection.normalize	();
	tFinishPoint.mul		(tDirection,fDistance);
	tFinishPoint.add		(tStartPosition);
	return					(ffMarkNodesInDirection(dwStartNode,tStartPosition,tFinishPoint,tpaStack,tpaMarks));
}

float CAI_Space::ffMarkNodesInDirection(u32 dwStartNode, Fvector tStartPosition, u32 dwFinishNode, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks)
{
	return					(ffMarkNodesInDirection(dwStartNode,tStartPosition,tfGetNodeCenter(dwFinishNode),tpaStack,tpaMarks));
}

float CAI_Space::ffMarkNodesInDirection(u32 dwStartNode, Fvector tStartPoint, Fvector tFinishPoint, xr_vector<u32> &tpaStack, xr_vector<bool> *tpaMarks)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tTempPoint = tStartPoint;
	float					fDistance = tStartPoint.distance_to(tFinishPoint), fCurDistance = 0.f;
	u32						dwCurNode = dwStartNode;

	while (!bfInsideNode(Node(dwCurNode),tFinishPoint) && (fCurDistance < (fDistance + EPS_L))) {
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
			iPrevIndex		= dwCurNode;
			dwCurNode		= iSavedIndex;
		}
		else
			return(fCurDistance);
		
		if (tpaMarks)
			(*tpaMarks)[dwCurNode]	= true;
		tpaStack.push_back	(dwCurNode);
	}
	
	return(fCurDistance);
}

float CAI_Space::ffFindFarthestNodeInDirection(u32 dwStartNode, Fvector tStartPoint, Fvector tFinishPoint, u32 &dwFinishNode, xr_vector<bool> *tpaMarks)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tTempPoint = tStartPoint;
	float					fDistance = tStartPoint.distance_to(tFinishPoint), fCurDistance = 0.f;
	u32						dwCurNode = dwStartNode;

	while (!bfInsideNode(Node(dwCurNode),tFinishPoint) && (fCurDistance < (fDistance + EPS_L))) {
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
			iPrevIndex		= dwCurNode;
			dwCurNode		= iSavedIndex;
		}
		else
			return(fCurDistance);
		
		if (tpaMarks)
			(*tpaMarks)[dwCurNode]	= true;
		dwFinishNode		= dwCurNode;
	}
	return(fCurDistance);
}

bool CAI_Space::bfCreateStraightPTN_Path(u32 dwStartNode, Fvector tStartPoint, Fvector tFinishPoint, xr_vector<Fvector> &tpaOutputPoints, xr_vector<u32> &tpaOutputNodes, bool bAddFirstPoint)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tTempPoint = tStartPoint;
	float					fDistance = tStartPoint.distance_to(tFinishPoint), fCurDistance = 0.f;
	u32						dwCurNode = dwStartNode;

	tpaOutputPoints.clear	();
	tpaOutputNodes.clear	();
	if (bAddFirstPoint) {
		tpaOutputPoints.push_back(tStartPoint);
		tpaOutputNodes.push_back(dwStartNode);
	}

	while (!bfInsideNode(Node(dwCurNode),tFinishPoint) && (fCurDistance < (fDistance + EPS_L))) {
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance		= tStartPoint.distance_to_xz(tTempPoint);
			PContour			tNextContour;
			PSegment			tNextSegment;
			Fvector				tIntersectPoint;
			UnpackContour		(tNextContour,iSavedIndex);
			vfIntersectContours	(tNextSegment,tNextContour,tCurContour);
			u32					dwIntersect = lines_intersect(tStartPoint.x,tStartPoint.z,tFinishPoint.x,tFinishPoint.z,tNextSegment.v1.x,tNextSegment.v1.z,tNextSegment.v2.x,tNextSegment.v2.z,&tIntersectPoint.x,&tIntersectPoint.z);
			VERIFY				(dwIntersect);
			tIntersectPoint.y	= ffGetY(*tpNode,tIntersectPoint.x,tIntersectPoint.z);
			
			tpaOutputPoints.push_back(tIntersectPoint);
			tpaOutputNodes.push_back(iSavedIndex);
			
			iPrevIndex			= dwCurNode;
			dwCurNode			= iSavedIndex;
		}
		else {
			int					iNodeIndex;
			taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
			bool				bOk = false;
			for ( i=0; i < iCount; i++) {
				NodeCompressed *tpLastNode = Node(iNodeIndex = UnpackLink(taLinks[i]));
				if (bfInsideNode(tpLastNode,tFinishPoint)) {
					PContour			tNextContour;
					PSegment			tNextSegment;
					UnpackContour		(tNextContour,iNodeIndex);
					vfIntersectContours	(tNextSegment,tNextContour,tCurContour);
					Fvector				tAdditionalPoint = tFinishPoint.distance_to_xz(tNextSegment.v1) < tFinishPoint.distance_to_xz(tNextSegment.v2) ? tNextSegment.v1 : tNextSegment.v2;
					tAdditionalPoint.y	= ffGetY(*tpNode,tAdditionalPoint.x,tAdditionalPoint.z);
					
					tpaOutputPoints.push_back(tAdditionalPoint);
					tpaOutputNodes.push_back(iNodeIndex);
					
					fCurDistance		= fDistance;
					dwCurNode			= iNodeIndex;
					bOk					= true;
					break;
				}
			}
			if (!bOk)
				return(false);
		}
	}

	if (bfInsideNode(Node(dwCurNode),tFinishPoint)) {
		tpaOutputPoints.push_back(tFinishPoint);
		tpaOutputNodes.push_back(dwCurNode);
		return(true);
	}
	else
		return(false);
}

void CAI_Space::vfFindGraphPointNodeInDirection(u32 dwStartNode, Fvector tStartPoint, Fvector tDirection, u32 &dwFinishNode, _GRAPH_ID tGraphID)
{
	PContour				tCurContour;
	NodeCompressed			*tpNode;
	NodeLink				*taLinks;
	int						i, iCount, iSavedIndex, iPrevIndex = -1, iNextNode;
	Fvector					tFinishPoint = tStartPoint, tTempPoint = tStartPoint;
	u32						dwCurNode = dwStartNode;
	tDirection.mul			(2000.f);
	tFinishPoint.add		(tDirection);
	float					fCurDistance = 0.f;

	for (;;) {
		tpNode				= Node(dwCurNode);
		taLinks				= (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
		iCount				= tpNode->links;
		iSavedIndex			= -1;
		UnpackContour		(tCurContour,dwCurNode);
		for ( i=0; i < iCount; i++)
			if ((iNextNode = UnpackLink(taLinks[i])) != iPrevIndex)
				if (m_tpaCrossTable[iNextNode].tGraphIndex == tGraphID)
					vfChoosePoint	(tStartPoint,tFinishPoint,tCurContour, iNextNode,tTempPoint,iSavedIndex);

		if (iSavedIndex > -1) {
			fCurDistance	= tStartPoint.distance_to_xz(tTempPoint);
			iPrevIndex		= dwCurNode;
			dwCurNode		= iSavedIndex;
		}
		else
			return;
		
		dwFinishNode		= dwCurNode;
	}
}