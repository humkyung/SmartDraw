#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SmartHSR.h"
#include "BSPTree.h"

#include "IsPlane3d.h"

#define PREPEND_FACE(f,fl) (f->next = fl,fl = f)
#define ISVERTEX_EQ(v1,v2) \
                          (ISEQUAL((v1)->pt->x,(v2)->pt->x) && \
                           ISEQUAL((v1)->pt->y,(v2)->pt->y) && \
                           ISEQUAL((v1)->pt->z,(v2)->pt->z))

/**	\brief
*/
CBSPTree::CBSPTree(){
	m_pNode = NULL;
	m_pBSPNodePool = NULL;
}

/**	\brief
*/
CBSPTree::~CBSPTree(){
	try{
		Clear();
	}catch(...){}

#ifdef _DEBUG
	::OutputDebugString("Destructure of CBSPTree");
#endif
}

/**
	@brief	Chooses plane with which to partition. 
		: The algorithm is to examine the first MAX_CANDIDATES on face list. For
		: each candidate, count how many splits it would make against the scene.
		: Then return the one with the minimum amount of splits as the 
		: partitioning plane.

	@author	BHK

	@date	?
	
	@parameter	faceList - list of faces
			plane - plane equation returned
	
	@reutrn	bool
*/
bool CBSPTree::ChoosePartitionPlane(CHSRFace *pFaceList,PLANE_T* plane)
{
	assert(pFaceList && "pFaceList is invalid");
	assert(plane     && "plane is invalid");
	bool ret = false;

	if(pFaceList)
	{
		//! number of candidates
		const unsigned long MAX_CANDIDATES=100;

		int minCount= 0xffffffff;
		// pick first face for now 
		CHSRFace *chosenRoot= pFaceList;
		CHSRFace *rootrav=pFaceList;
		for (int ii= 0;(rootrav != NULL) && (ii < MAX_CANDIDATES);rootrav= rootrav->next, ii++)
		{
			//! except a face that it's id is -1 from tesing
			if(-1 == rootrav->id) continue;

			unsigned int count= 0;
			// for all faces in scene other than itself... 
			CHSRFace *ftrav=pFaceList;
			for (int jj = 0; ftrav != NULL; ftrav= ftrav->next,jj++)
			{
				if(-1 == ftrav->id) continue;
				if (ftrav != rootrav) if (DoesFaceStraddlePlane(ftrav,&rootrav->Plane)) count++;
			}
			
			//! remember minimum count and its corresponding face 
			if (count < (unsigned)minCount)
			{
				minCount  = count;
				chosenRoot= rootrav;
			}
			// can't do better than 0 so return this plane 
			if (0 == count) break;
		}
		//! return partitioning plane 
		*plane= chosenRoot->Plane;
		
		ret = true;
	}

	return ret;
}

/**	@brief	Returns a boolean to indicate whether the face straddles the plane

	@author	BHK

	@date	?

	@param	face	face to check
	@param	plane	a parameter of type const PLANE_T*

	@return	bool	
*/
bool CBSPTree::DoesFaceStraddlePlane(const CHSRFace *face, const PLANE_T* plane)
{
	assert((face != NULL) && "face is NULL");
	assert((face->pHead != NULL) && "face->pHead is NULL");
	assert(plane && "plane is invalid");
	HSR_VERTEX *vtrav=NULL; 

	if((face && face->pHead) && plane){
		bool anyNegative = false, anyPositive = false;

		// for all vertices... 
		for (vtrav= face->pHead; vtrav->next !=NULL; vtrav= vtrav->next) {
			double value = plane->a*vtrav->pt->x + plane->b*vtrav->pt->y + plane->c*vtrav->pt->z + plane->d;
			// check which side vertex is on relative to plane 
			SIGN_T sign= FSIGN(value);
			if (sign == NEGATIVE)		anyNegative = true; 
			else if (sign == POSITIVE)	anyPositive = true;

			// if vertices on both sides of plane then face straddles else it no 
			if (anyNegative && anyPositive) return true;
		}
	}

	return false;
}

/** 
	@brief
	Partitions a 3D convex polygon (face) with an arbitrary plane into its 
	negative and positive fragments, if any, w.r.t. the partitioning plane.
	Note that faceList is unusable afterwards since its vertex list has been
	parceled out to the other faces. It's set to null to avoid dangling
	pointer problem. Faces embedded in the plane are separated into two lists,
	one facing the same direction as the partitioning plane, faceSameDir, and 
	the other facing the opposite direction, faceOppDir.

	@author	BHK

	@date	?
	
	@param	plane = partion plane
	@param	faceList = 
	@param	faceNeg		= store negative planes against plane
	@param	facePos		= store positive planes against plane
	@param	faceSameDir 	= store same dir planes against plane
	@param	faceOppDir	= store oppositive planes against plane

	@remakrs
	2007.03.30 - 파이프가 잘려질 경우 파이프 centerline도 함께 잘라준다.

	@return	
*/
void CBSPTree::PartitionFaceListWithPlane(const PLANE_T *plane,CHSRFace **faceList,
					  CHSRFace**faceNeg, CHSRFace **facePos,CHSRFace **faceSameDir, CHSRFace **faceOppDir)
{
	CHSRFace *ftrav= *faceList;

	*faceSameDir= *faceOppDir = *faceNeg= *facePos= NULL;

	while (ftrav != NULL)
	{
		HSR_VERTEX *v1, *v2; CHSRFace *nextFtrav;
		double ixx1=0.,iyy1=0.,izz1=0.,ixx2=0.,iyy2=0.,izz2=0.;
		SIGN_T signV1, signV2;

		nextFtrav= ftrav->next;	//! unchain current face from list
		ftrav->next= NULL;

		// find first intersection
		if((plane->a == ftrav->Plane.a) &&  (plane->b == ftrav->Plane.b) && (plane->c == ftrav->Plane.c) && (plane->d == ftrav->Plane.d))
		{
			PREPEND_FACE(ftrav,*faceSameDir);
		}
		else
		{
			v1 = FindNextIntersection(ftrav->pHead,plane,&ixx1,&iyy1,&izz1,&signV1);
			if (v1 != NULL) 
			{
				assert(signV1 != ZERO);

				// first one found, find the 2nd one, if any 
				v2 = FindNextIntersection(v1->next,plane,&ixx2,&iyy2,&izz2,&signV2);

				// Due to numerical instabilities, both intersection points may
				// have the same sign such as in the case when splitting very close
				// to a vertex. This should not count as a split.
				if (v2 != NULL && signV1 == signV2) v2= NULL;

			}else v2 = NULL; // No first intersection found,
					 // therefore no second intersection.
                                
			// an intersection? 
			if(v1 != NULL && v2 != NULL)
			{ /// yes, intersection
				CHSRFace *newOtherFace;

				// ftrav's vertex list will be modified
				newOtherFace = CreateOtherFace(ftrav,v1,ixx1,iyy1,izz1,v2,ixx2,iyy2,izz2);
				
				if((PIPE == ftrav->type) && ftrav->pCenterline && ftrav->pCenterline->next)
				{
					double xx = 0, yy =0, zz =0;
					
					CIsPlane3d plane3d(CIsVect3d(plane->a , plane->b , plane->c) , plane->d);
					CIsPoint3d intsec;
					const bool exist = plane3d.FindIntersectionPoint(intsec , 
						CIsPoint3d(ftrav->pCenterline->pt->x , ftrav->pCenterline->pt->y , ftrav->pCenterline->pt->z) , 
						CIsPoint3d(ftrav->pCenterline->pt->x , ftrav->pCenterline->pt->y , ftrav->pCenterline->pt->z));
					
					if(exist)
					{
						POINT_T pt;
						
						pt.x = ftrav->pCenterline->next->pt->x;
						pt.y = ftrav->pCenterline->next->pt->y;
						pt.z = ftrav->pCenterline->next->pt->z;
							
						ftrav->pCenterline->next->pt->x = intsec.x();
						ftrav->pCenterline->next->pt->y = intsec.y();
						ftrav->pCenterline->next->pt->z = intsec.z();
							
						newOtherFace->pCenterline = AllocVertex(intsec.x() , intsec.y() , intsec.z());
						newOtherFace->pCenterline->next = AllocVertex(pt.x , pt.y , pt.z);
					}

					/*
					HSR_VERTEX* v3 = FindNextIntersection(ftrav->pCenterline , plane , &xx , &yy , &zz , &signV3);
										
					//SIGN_T signV3 = AnyEdgeIntersectWithPlane(
					//	ftrav->pCenterline->pt->x , ftrav->pCenterline->pt->y , ftrav->pCenterline->pt->z , 
					//	ftrav->pCenterline->next->pt->x , ftrav->pCenterline->next->pt->y , ftrav->pCenterline->next->pt->z ,
					//	plane , &xx , &yy , &zz);
					if(NULL != v3)
					{
						xx = (ixx1 + ixx2) * 0.5;
						yy = (iyy1 + iyy2) * 0.5;
						zz = (izz1 + izz2) * 0.5;
						POINT_T pt;
						if(signV1 == signV3)
						{
							pt.x = ftrav->pCenterline->next->pt->x;
							pt.y = ftrav->pCenterline->next->pt->y;
							pt.z = ftrav->pCenterline->next->pt->z;

							ftrav->pCenterline->next->pt->x = xx;
							ftrav->pCenterline->next->pt->y = yy;
							ftrav->pCenterline->next->pt->z = zz;

							newOtherFace->pCenterline = AllocVertex(xx , yy , zz);
							newOtherFace->pCenterline->next = AllocVertex(pt.x , pt.y , pt.z);
						}
						else
						{
							pt = *(ftrav->pCenterline->pt);
							
							ftrav->pCenterline->pt->x = xx;
							ftrav->pCenterline->pt->y = yy;
							ftrav->pCenterline->pt->z = zz;
							
							newOtherFace->pCenterline = AllocVertex(pt.x , pt.y , pt.z);
							newOtherFace->pCenterline->next = AllocVertex(xx , yy , zz);
						}
					}
					*/
				}

				// return split faces on appropriate lists
				if (signV1 == NEGATIVE) 
				{ 
					PREPEND_FACE(ftrav,*faceNeg);
					PREPEND_FACE(newOtherFace,*facePos);
				}
				else 
				{
					assert(signV1 == POSITIVE);
					PREPEND_FACE(newOtherFace,*faceNeg);
					PREPEND_FACE(ftrav,*facePos);
				}
			}
			else 
			{	// no intersection
				SIGN_T side;

				// Face is embedded or wholly to one side of partitioning plane.
				side= WhichSideIsFaceWRTPlane(ftrav,plane);
				if (side == NEGATIVE) PREPEND_FACE(ftrav,*faceNeg);
				else if (side == POSITIVE) PREPEND_FACE(ftrav,*facePos);
				else
				{
					assert(side == ZERO);
					if (ISEQUAL(ftrav->Plane.a,plane->a) && ISEQUAL(ftrav->Plane.b,plane->b) && ISEQUAL(ftrav->Plane.c,plane->c))
						PREPEND_FACE(ftrav,*faceSameDir);
					else	PREPEND_FACE(ftrav,*faceOppDir);
				}
			}
		}

		ftrav= nextFtrav; // get next
	}

	// faceList's vertex list has been parceled out to other lists so
	// set this to null.
	*faceList= NULL;
#if 0	// only true for BSPconstructTree() 
	// there's at least one face embedded in the partitioning plane 
	assert(*faceSameDir != NULL_FACE); 
#endif
}

/**
	@brief	: Finds next intersection on or after vstart. 
		  If an intersection is found, 
		  a pointer to first vertex of the edge is returned, 
		  the intersection point (ixx,iyy,izz) and its sign is updated. 
		  Otherwise a null pointer is returned.

	@author	BHK

	@date	?

	@param

	@return
*/
HSR_VERTEX* CBSPTree::FindNextIntersection(HSR_VERTEX *vstart , const PLANE_T *plane,
					double *ixx , double *iyy , double *izz , SIGN_T *sign)
{
	HSR_VERTEX *vtrav = vstart;

	if(vstart)
	{
		//! for all edges starting from vstart ... 
		for (vtrav= vstart; vtrav->next != NULL; vtrav= vtrav->next) 
		{
			if ((*sign = AnyEdgeIntersectWithPlane(vtrav->pt->x,vtrav->pt->y,vtrav->pt->z,vtrav->next->pt->x,vtrav->next->pt->y,vtrav->next->pt->z,plane,ixx,iyy,izz)))
			{	
				return(vtrav);
			}
		}
	}

	return NULL;
}

//	parameter	: 
//	description	: Determines if an edge bounded by (x1,y1,z1)->(x2,y2,z2) intersects
//			  the plane.
//			  If there's an intersection, 
//			  the sign of (x1,y1,z1), NEGATIVE or POSITIVE, w.r.t. the plane is
//			  returned with the intersection (ixx,iyy,izz) updated.
//			  Otherwise ZERO is returned.    
//	remarks		: 
//	returns		: 
SIGN_T CBSPTree::AnyEdgeIntersectWithPlane(const double x1 , const double y1 , const double z1 , const double x2 , const double y2 , const double z2 ,
			       const PLANE_T *plane , double *ixx , double *iyy , double *izz)
{
	int sign1, sign2; // must be int since gonna do a bitwise ^
	const double aa = plane->a; const double bb= plane->b; const double cc= plane->c;
	double dd= plane->d;

	// get signs
	double temp1= (aa*x1) + (bb*y1) + (cc*z1) + dd;
	if (temp1 < -HSR_TOLER)     sign1= -1;
	else if (temp1 > HSR_TOLER)	sign1= 1;
	else 
	{
		// edges beginning with a 0 sign are not considered valid intersections
		// case 1 & 6 & 7, see Gems III.
		assert(ISEQUAL(temp1,0.0));
		(*ixx) = x1; (*iyy) = y1; (*izz) = z1;
		return ZERO;
	}

	double temp2 = (aa*x2) + (bb*y2) + (cc*z2) + dd;
	if (temp2 < -HSR_TOLER)     sign2= -1;
	else if (temp2 > HSR_TOLER) sign2= 1;
	else 
	{	// case 8 & 9, see Gems III
		assert(ISEQUAL(temp2,0.0));
		*ixx= x2; *iyy= y2; *izz= z2;

		return (sign1 == -1) ? NEGATIVE : POSITIVE;
	}

	// signs different? 
	// recall: -1^1 == 1^-1 ==> 1    case 4 & 5, see Gems III
	//         -1^-1 == 1^1 ==> 0    case 2 & 3, see Gems III
	if (sign1 ^ sign2) 
	{
		double dx,dy,dz;
		double denom, tt;

		// compute intersection point 
		dx= x2-x1;
		dy= y2-y1;
		dz= z2-z1;

		denom= (aa*dx) + (bb*dy) + (cc*dz);
		tt= - ((aa*x1) + (bb*y1) + (cc*z1) + dd) / denom;

		*ixx = x1 + (tt * dx);
		*iyy = y1 + (tt * dy);
		*izz = z1 + (tt * dz);

		assert(sign1 == 1 || sign1 == -1);

		return( (sign1 == -1) ? NEGATIVE : POSITIVE );
	}else return ZERO;
}

/*	
	@brief		: Memory allocated for split face's vertices and pointers tediously updated.

	@author		: humkyung

	@date		: ????.??.??

	@param		: face - face to be split
	@param		: v1   - 1st vertex of edge of where 1st intersection was found 
	@param		: (ixx1,iyy1,izz1) - 1st intersection
	@param		: v2   - 1st vertex of edge of where 2nd intersection was found 
	@param		: (ixx2,iyy2,izz2) - 2nd intersection

	@return		: CHSRFace*
*/
CHSRFace* CBSPTree::CreateOtherFace(CHSRFace *pFace, HSR_VERTEX *v1, double ixx1, double iyy1, double izz1,
				HSR_VERTEX *v2, double ixx2, double iyy2, double izz2){
	assert((v1 != v2) && "v1 and v2 is equal");

	PHSR_VERTEX i1p1=NULL,i2p1=NULL;	// new vertices for original pFace  
	PHSR_VERTEX i1p2=NULL,i2p2=NULL;	// new vertices for new pFace 
	PHSR_VERTEX p2end=NULL;		// new vertex for end of new pFace 
	PHSR_VERTEX vtemp=NULL;		// place holders 
	CHSRFace* newFace = NULL;	// newly allocated pFace 
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	// new intersection vertices 
	i1p1 = AllocVertex(ixx1,iyy1,izz1);
	i2p1 = AllocVertex(ixx2,iyy2,izz2);
	i1p2 = AllocVertex(ixx1,iyy1,izz1);
	i2p2 = AllocVertex(ixx2,iyy2,izz2); 
	if((NULL == i1p1) || (NULL == i2p1) || (NULL == i1p2) || (NULL == i2p2)) return newFace;

	// duplicate 1st vertex of 2nd list to close it up 
	if(NULL != (p2end = pHSRApp->alloc_vertex()))
	{
		p2end->pt = i1p2->pt;
		if(NULL != (p2end->pt = pHSRApp->alloc_point()))
		{
			p2end->pt->x = ixx1;
			p2end->pt->y = iyy1;
			p2end->pt->z = izz1;
		}else	return NULL;
	}else	return NULL;
	/*if(p2end = (PHSR_VERTEX)malloc(sizeof(HSR_VERTEX))){
		memset(p2end,0,sizeof(HSR_VERTEX));
		p2end->pt = i1p2->pt;
	}
	
	//p2end= AllocVertex(ixx1,iyy1,izz1);
	*/
	vtemp= v1->next;

	// merge both intersection vertices i1p1 & i2p1 into 1st list 
	if(ISVERTEX_EQ(i1p1,v1)){
		assert(i1p1->next == NULL);
		FreeVertexList(&i1p1);
		i1p1 = v1;
		i1p2->next = v1->next;
	}else if(ISVERTEX_EQ(i1p1,v1->next)){
		assert(i1p1->next == NULL);
		FreeVertexList(&i1p1);
		i1p1 = v1->next;
		i1p2->next = v1->next->next;
	}else{
		i1p2->next = v1->next;
		v1->next = i1p1;
	}

	if(ISVERTEX_EQ(i2p1,v2)){
		assert(i2p1->next == NULL);
		FreeVertexList(&i2p1);
		i2p1       = v2;
		i1p1->next = i2p1;
		
		PHSR_VERTEX beforeV2 = NULL;
		for (beforeV2 = v1; beforeV2->next != v2; beforeV2= beforeV2->next);
		beforeV2->next = i2p2;
	}else if(ISVERTEX_EQ(i2p1,v2->next)){
		assert(i2p1->next == NULL);
		FreeVertexList(&i2p1);
		i2p1       = v2->next;
		i1p1->next = v2->next;
		v2->next   = i2p2;
	}else{
		i1p1->next = i2p1;
		i2p1->next = v2->next;
		v2->next   = i2p2;
	}
	i2p2->next = p2end;

//	follow up routine...
	i1p2->render = v1->render;
	i1p1->render = CUT;
	i2p2->render = CUT;
	i2p1->render = v2->render;
//
	if(ISVERTEX_EQ(i1p1,i2p1)){
		i1p1->next = i2p1->next;
		i2p1->next = NULL;
		FreeVertexList(&i2p1);
	}
	
	if(ISVERTEX_EQ(i1p2,i2p2)){
		assert(p2end->next == NULL);
		FreeVertexList(&p2end);
		i2p2->next = NULL;
	}

	//! copy original pFace info but with new vertex list 
	if(NULL != (newFace = AllocFace(i1p2,pFace)))
	{
		newFace->pSibling = pFace->pSibling;
		pFace->pSibling    = newFace;
		newFace->body = pFace->body;	/// copy body - 2012.01.12 added by humkyung
	}
	else
	{ // fail to creat other pFace.
	}

	return newFace;
}

/**	
	@brief	Allocates a vertex with position (xx,yy,zz) 

	@author	humkyung

	\param	xx	a parameter of type double
	\param	yy	a parameter of type double
	\param	zz	a parameter of type double

	\return	PHSR_VERTEX	
*/
PHSR_VERTEX CBSPTree::AllocVertex(double xx,double yy,double zz)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	assert(pHSRApp && "HiddenApp is not created");
	HSR_VERTEX* pNewVertex = NULL;

	if(pHSRApp){
		if(NULL != (pNewVertex = pHSRApp->alloc_vertex()))
		{
			if(NULL != (pNewVertex->pt = pHSRApp->alloc_point()))
			{
				pNewVertex->id    = 0;
				pNewVertex->render= INTERSECTION;

				pNewVertex->pt->x = xx;
				pNewVertex->pt->y = yy;
				pNewVertex->pt->z = zz;
			}
			else
			{
				pHSRApp->free_vertex(pNewVertex);
				pNewVertex = NULL;
			}
		}
	}

	return pNewVertex;
}

/**	
	@brief	: Allocates a face with list of vertices,plane equation.

	@author	: BHK

	@date	: ?

	@param	: vlist	list of vertices
	@param	: face	a parameter of type const FACE*

	@return	: FACE*	
*/
CHSRFace* CBSPTree::AllocFace(HSR_VERTEX *vlist,const CHSRFace* face)
{
	assert(vlist && "vertex list is invalid");
	assert(face  && "face is invalid");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	assert(pHSRApp && "pHSRApp is not created");
	CHSRFace* pNewFace = NULL;

	if(pHSRApp && vlist && face)
	{
		if(NULL != (pNewFace = pHSRApp->new_hsr_face()))
		{
			pNewFace->m_enmLayer = face->m_enmLayer;
			pNewFace->type     = face->type;
			pNewFace->ele      = face->ele;
			pNewFace->id       = face->id;
			pNewFace->radius   = face->radius;
			pNewFace->ptCenter = face->ptCenter;
			pNewFace->ptCross  = face->ptCross;
			pNewFace->ptEdge[0]= face->ptEdge[0];
			pNewFace->ptEdge[1]= face->ptEdge[1];
			pNewFace->mark[0] = face->mark[0];
			pNewFace->mark[1] = face->mark[1];
			pNewFace->ptMark[0]= face->ptMark[0];
			pNewFace->ptMark[1]= face->ptMark[1];
			pNewFace->SetColor(face->color());
			
			memcpy(&(pNewFace->Plane),&(face->Plane),sizeof(PLANE_T));
			memcpy(&(pNewFace->volume),&(face->volume),sizeof(VOLUME_T));
			pNewFace->pHead = vlist;
		}
	}
	
	return pNewFace;
}

/**	
	@brief	construct BSP node with faces partitioned by a partition face.

	@author	BHK

	@date	?

	@param	pFaceList	a parameter of type PFACE *

	@return	CBSPNode*	
*/
CBSPNode* CBSPTree::Construct(CHSRFace* *pFaceList)
{
	assert(pFaceList && "pFaceList is invalid");
	CBSPNode *newBSPNode = NULL;
	CHSRFace *sameDirList=NULL,*oppDirList=NULL,*faceNegList=NULL,*facePosList=NULL;

	PLANE_T plane={0};
	if(ChoosePartitionPlane(*pFaceList,&plane))
	{
		PartitionFaceListWithPlane(&plane,pFaceList,&faceNegList,&facePosList,&sameDirList,&oppDirList);
		assert(*pFaceList == NULL);
		assert(sameDirList != NULL);

		if(NULL == m_pBSPNodePool) m_pBSPNodePool = new boost::object_pool<CBSPNode>;
		if(NULL != (newBSPNode = m_pBSPNodePool->construct())){
			newBSPNode->SetPartitionFaceList(sameDirList,oppDirList);

			if(faceNegList != NULL) newBSPNode->SetNegNode(Construct(&faceNegList));
			if(facePosList != NULL) newBSPNode->SetPosNode(Construct(&facePosList));
		}
	}

	return newBSPNode;
}

/* Determines which side a face is with respect to a plane.
 *
 * However, due to numerical problems, when a face is very close to the plane,
 * some vertices may be misclassified. 
 * There are several solutions, two of which are mentioned here:
 *    1- classify the one vertex furthest away from the plane, (note that
 *       one need not compute the actual distance) and use that side.
 *    2- count how many vertices lie on either side and pick the side
 *       with the maximum. (this is the one implemented).
 */
SIGN_T CBSPTree::WhichSideIsFaceWRTPlane(CHSRFace *face, const PLANE_T *plane){
	assert(face && "face is NULL");

	register HSR_VERTEX *vtrav=NULL;
	double value=0.;
	bool isNeg=false, isPos=false;

	isNeg= isPos= false;

	if(face){
		for (vtrav= face->pHead;vtrav &&  vtrav->next != NULL; vtrav= vtrav->next){
			value= (plane->a*vtrav->pt->x) + (plane->b*vtrav->pt->y) + (plane->c*vtrav->pt->z) + plane->d;
			if (value < -HSR_TOLER) 
				isNeg= true;
			else if (value > HSR_TOLER)
				isPos= true;
			else assert(ISEQUAL(value,0.0));
		} /* for vtrav */ 
		
		/* in the very rare case that some vertices slipped thru to other side of
		 * plane due to round-off errors, execute the above again but count the 
		 * vertices on each side instead and pick the maximum.
		*/
		if (isNeg && isPos) {	/* yes so handle this numerical problem */
			int countNeg, countPos;
			
			/* count how many vertices are on either side */
			countNeg= countPos= 0;
			for (vtrav= face->pHead; vtrav->next != NULL; vtrav= vtrav->next) {
				value= (plane->a*vtrav->pt->x) + (plane->b*vtrav->pt->y) + (plane->c*vtrav->pt->z) + plane->d;
				if (value < -HSR_TOLER)
					countNeg++;
				else if (value > HSR_TOLER)
					countPos++;
				else assert(ISEQUAL(value,0.0));
			} /* for */
			
			/* return the side corresponding to the maximum */
			if (countNeg > countPos) return(NEGATIVE);
			else if (countPos > countNeg) return(POSITIVE);
			else return(ZERO);
		}else {			/* this is the usual case */
			if (isNeg) return(NEGATIVE);
			else if (isPos) return(POSITIVE);
			else return(ZERO);
		}
	}

	return ZERO;
}

/**	\brief	The CBSPTree::FreeVertexList function\n
	Frees list of vertices. 

	\param	*vlist	a parameter of type HSR_VERTEX *

	\return	void	
*/
void CBSPTree::FreeVertexList(HSR_VERTEX **vlist){
	assert(*vlist && "*vlist is invalid");
	
	if(*vlist){
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		assert(pHSRApp);

		HSR_VERTEX *vtrav= *vlist, *vdel;
		while (vtrav != NULL) {
			vdel= vtrav;
			vtrav= vtrav->next;

			pHSRApp->free_vertex(vdel);
		}
		*vlist= NULL;
	}
}

/**	\brief	The CBSPTree::IsViewerInPositiveSideOfPlane function

	\param	plane	a parameter of type const PLANE_T *
	\param	position	a parameter of type const PPOINT_T

	\return	bool	
*/
bool CBSPTree::IsViewerInPositiveSideOfPlane(const PLANE_T *plane,const PPOINT_T position){
	double dp= plane->a*position->x + plane->b*position->y + plane->c*position->z + plane->d;
   
	return( (dp > 0.0) ? 1 : 0 );
}

CBSPNode* CBSPTree::NewBSPNode()
{
	CBSPNode* pBSPNode=new CBSPNode;
	if(NULL != pBSPNode){
		m_lstBSPNode.push_back(pBSPNode);
		return pBSPNode;
	}

	return NULL;
}

void CBSPTree::Clear()
{
	if(NULL != m_pBSPNodePool){
		//delete m_pBSPNodePool;
	}
}
