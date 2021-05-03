//	file		: HSRImport.cpp
//	author		: baek hum kyung
//	date		: 2001.10.. ~ 
//	description	: read faces from dlist which is used in Intent!
//	update		: 2002.05.23 - add procedure extracting hold face.

// KImport.cpp: implementation of the KImport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "Geo.h"

#include "HSRImport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHSRImport::CHSRImport()
{
	 m_nID = 0;
}

CHSRImport::~CHSRImport()
{
	m_nID = 0;
}

//	parameter	:
//	description	: 1. check data type
//			: 2. get list of each primitive
//	remarks		: [APPEND]
//			: (2002-02-06) append codes extracting face.
//	returns		:
CHSRFace* CHSRImport::ExtractPrimitives(DList* pPrimitiveList,CHSRFace* pFaceList)
{
	assert(pPrimitiveList     && "pPrimitiveList is invalid");
	assert(m_queDList.empty() && "queue is not emtpy");
	DList *pDList=NULL,*pSubDList=NULL;

	if(pPrimitiveList && (df_list == pPrimitiveList->val.type))
	{
		m_queDList.push(pPrimitiveList);

		int id=0,seq=0;
		while(!m_queDList.empty())
		{
			pDList = m_queDList.front();
			m_queDList.pop();

			for(pSubDList = pDList;pSubDList;pSubDList = pSubDList->next)
			{
				if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
				{
					m_queDList.push(pSubDList->val.data.list);
				}
				else if(pSubDList && (df_integer == pSubDList->val.type))
				{
					int id = pSubDList->val.data.integer;
					if(CYLINDER == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractCylinder(pFaceList,pSubDList,CYLINDER);
					}
					else if(EQP_CYLINDER == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractCylinder(pFaceList,pSubDList,EQP_CYLINDER);
					}
					else if(BOX == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractBox(pSubDList,pFaceList);
					}
					else if(ELBOW == id)
					{
						pSubDList = pSubDList->next;
						
						assert(df_integer == pSubDList->val.type);
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractElbow(pSubDList,pFaceList);
					}
					else if(CONE == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type)){
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractCone(pFaceList,pSubDList,CONE);
						}
					}
					else if(EQP_CONE == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractCone(pFaceList,pSubDList,EQP_CONE);
						}
					}
					else if(TORUS == id)
					{
						pSubDList = pSubDList->next;
						
						assert(df_integer == pSubDList->val.type);
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractTorus(pSubDList,pFaceList);
					}
					else if(PIPE == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractPipe(pFaceList,pSubDList);
						}
					}
					else if(PIPEX == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractExtendPipe(pFaceList,pSubDList);
						}
					}
					else if(TEE == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractTee(pFaceList,pSubDList);
						}
					}
					else if(TEEX == id)
					{
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_integer == pSubDList->val.type));
						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
							
							assert(pSubDList && (df_list == pSubDList->val.type));
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
								pFaceList = ExtractExtendTee(pFaceList,pSubDList);
						}
					}
					else if(FLANGE == id)
					{
						pSubDList = pSubDList->next;
						
						assert(df_integer == pSubDList->val.type);
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractFlange(pSubDList,pFaceList);
					}
					else if(DOME == id)
					{
						pSubDList = pSubDList->next;
						
						assert(df_integer == pSubDList->val.type);
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractDome(pSubDList,pFaceList);
					}
					else if(SW_ELBOW == id)
					{
						pSubDList = pSubDList->next;
						
						assert(df_integer == pSubDList->val.type);
						seq = pSubDList->val.data.integer;
						pSubDList = pSubDList->next;
						
						assert(pSubDList && (df_list == pSubDList->val.type));
						if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							pFaceList = ExtractSWElbow(pSubDList,pFaceList);
					}
					else if(PIPE_BEND == id)
					{
						pSubDList = pSubDList->next;
						assert(pSubDList && (df_integer == pSubDList->val.type) && "pSubDList is invalid");

						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
						
							assert(pSubDList && (df_list == pSubDList->val.type)  && "pSubDList is invalid");
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							{
								pFaceList = ExtractPipeBend(pFaceList,pSubDList);
							}
						}
					}
					else if(FACE_T == id)
					{
						pSubDList = pSubDList->next;
						assert(pSubDList && (df_integer == pSubDList->val.type) && "pSubDList is invalid");

						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;

							assert(pSubDList && (df_list == pSubDList->val.type)  && "pSubDList is invalid");
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							{
								pFaceList = ExtractFace(pFaceList,pSubDList);
							}
						}
					}
					else if(HFACE_T == id)
					{
						pSubDList = pSubDList->next;
						assert(pSubDList && (df_integer == pSubDList->val.type) && "pSubDList is invalid");

						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;

							assert(pSubDList && (df_list == pSubDList->val.type)  && "pSubDList is invalid");
							if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list)
							{
								pFaceList = ExtractHFace(pFaceList,pSubDList);
							}
						}
					}
					else
					{
						pSubDList = pSubDList->next;
						assert(pSubDList && (df_integer == pSubDList->val.type) && "pSubDList is invalid");

						if(pSubDList && (df_integer == pSubDList->val.type))
						{
							seq = pSubDList->val.data.integer;
							pSubDList = pSubDList->next;
						
							assert(pSubDList && (df_list == pSubDList->val.type)  && "pSubDList is invalid");
							//if(pSubDList && (df_list == pSubDList->val.type) && pSubDList->val.data.list){
								//pFaceList = ExtractSWElbow(pSubDList,pFaceList);
							//}
							assert("Not Supported Primitive");
						}
					}
				}
			}
		}
	}
	
	assert(m_queDList.empty());
	return pFaceList;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
LONG CHSRImport::GetNumOfFace()
{
	return m_nID;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractSideFace(DList* pList,bool bCenterline)
{
	assert(pList && "pList is NULL - ExtractSideFace");
	CHSRFace* pFace = NULL;

	if(pList)
	{
		int count=0;
		POINT_T pt={0};
		DList* pSubList = NULL;

		if(pList && ((df_list == pList->val.type) && pList->val.data.list))
		{
			// extract side face.
			pSubList = pList->val.data.list;
			if(pSubList && (df_integer == pSubList->val.type))
			{
				count = pSubList->val.data.integer;
				pSubList = pSubList->next;

				assert(df_list == pSubList->val.type);
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pFace = ExtractFace(count,pSubList)))
					{
						pFace->id  = m_nID++;
						pFace->ele = SIDEFACE;
					}
				}
			}
			pList = pList->next;

			if(bCenterline)
			{
				// extract centerline.
				if(pList && (df_list == pList->val.type))
				{
					pSubList = pList->val.data.list;
					if(pSubList && (df_integer == pSubList->val.type))
					{
						count = pSubList->val.data.integer;

						pSubList = pSubList->next;
						if(pSubList && (df_list == pSubList->val.type))
						{
							pSubList = pSubList->val.data.list;
							pFace->pCenterline = ExtractVertexListFromDList(pFace,pSubList,&count,&pt,false);
						}
					}
					pList = pList->next;
				}
			}
		}
	}

	return pFace;
}

/**	\brief	The CHSRImport::ExtractCylinder function\n
	extract cylinder from dlist

	\param	pFaceList	a parameter of type PFACE
	\param	pList	a parameter of type DList*
	\param	id	a parameter of type HSR_PRIMITIVE_E

	\return	PFACE	
*/
CHSRFace* CHSRImport::ExtractCylinder(CHSRFace* pFaceList,DList* pList,HSR_PRIMITIVE_E id)
{
	assert(pList && "dlist is invalid");
	assert((df_list == pList->val.type) && "pList->val.type is invalid");
	assert((pList->val.data.list) && "pList->val.data.list is NULL");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList *pTmpList = NULL,*pSubList = NULL;	
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		
		int     count=0;
		double  radius[2]={0};
		POINT_T ptCenter[2]={0};
		pTmpList = pList->val.data.list;
		pSubList = pTmpList->val.data.list;
		if(df_integer == pSubList->val.type)
		{
			if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
			{
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pSide = ExtractFace(count,pSubList);
					if(pSide)
					{
						pSide->id  = m_nID++;
						pSide->type= id;
						pSide->ele = HSR_RECTANGLE_SHAPE;
					}
				}
			}
		}
		pTmpList = pTmpList->next;
		
		// top face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList->val.type == df_point && pSubList->next->val.type == df_number)
			{
				if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pTop = ExtractFace(count,pSubList);
						if(pTop)
						{
							pTop->ptCenter.x = ptCenter[0].x;
							pTop->ptCenter.y = ptCenter[0].y;
							pTop->ptCenter.z = ptCenter[0].z;
							pTop->radius     = radius[0];
							
							pTop->id  = m_nID++;
							pTop->type= id;
							pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
						}
					}
				}
			}
		}
		pTmpList = pTmpList->next;
		
		// bottom face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
			{ // bottom face
				if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pBottom = ExtractFace(count,pSubList);
						if(pBottom)
						{						
							pBottom->ptCenter.x = ptCenter[1].x;
							pBottom->ptCenter.y = ptCenter[1].y;
							pBottom->ptCenter.z = ptCenter[1].z;
							pBottom->radius     = radius[1];
							
							pBottom->id  = m_nID++;
							pBottom->type= id;
							pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
						}
					}
				}
			}
		}
		
		pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,CYLINDER,pTop,pSide,pBottom);
	}

	return pFaceList;
}

//	parameter	:
//	description	: read elbow faces from dlist
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractElbow(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "dlist is invalid - ExtractElbow");
	assert((df_list == pList->val.type) && "pList->val.type is invalid");

	if(pList && (df_list == pList->val.type))
	{
		int count=0;
		double radius[2]={0};
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		POINT_T pt[2]={0};
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		double vtx=0,vty=0,vtz=0;

		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			vtx = 0.;
			vty = 0.;
			vtz = 1.;
		}
		else
		{
			vtx = 1.;
			vty = 0.;
			vtz = 0.;
		}

		// extract elbow side face
		if(pTmpList && (pTmpList->val.type == df_list))
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_integer == pSubList->val.type))
			{
				count = pSubList->val.data.integer;
				pSubList = pSubList->next;

				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pSide = ExtractFace(count,pSubList)))
					{
						pSide->id  = m_nID++;
						pSide->type= ELBOW;
						pSide->ele = HSR_POLYGON_SHAPE;
					}
				}
			}
			pTmpList = pTmpList->next;

			// extract center line
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				assert(pSubList && (df_integer == pSubList->val.type));
				if(pSubList && (df_integer == pSubList->val.type))
				{
					count = pSubList->val.data.integer;

					pSubList = pSubList->next;
					pSubList = pSubList->val.data.list;
					pSide->pCenterline = ExtractVertexListFromDList(pSide,pSubList,&count,&pt[0],false);
				}
				pTmpList = pTmpList->next;
			}
		}

		// extract elbow top
		assert(df_list == pTmpList->val.type);
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_point == pSubList->val.type))
			{
				pt[0].x = pSubList->val.data.point->x;
				pt[0].y = pSubList->val.data.point->y;
				pt[0].z = pSubList->val.data.point->z;
				pSubList= pSubList->next;

				assert(df_number == pSubList->val.type);
				radius[0]= pSubList->val.data.number;
				pSubList = pSubList->next;
				
				assert(df_integer == pSubList->val.type);
				count   = pSubList->val.data.integer;
				pSubList= pSubList->next; // skip number of points
				
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pTop = ExtractFace(count,pSubList);
					if(pTop)
					{
						pTop->id  = m_nID++;
						pTop->type= ELBOW;
						pTop->ele = HSR_CIRCLE_SHAPE;
						pTop->ptCenter.x = pt[0].x;
						pTop->ptCenter.y = pt[0].y;
						pTop->ptCenter.z = pt[0].z;
						pTop->radius     = radius[0];
	
						double dot = theGeo.DotProduct(pTop->Plane.a,pTop->Plane.b,pTop->Plane.c,vtx,vty,vtz);
						if((dot > (-1. + 0.01)) && (dot < (1. - 0.001)))
						{
							if(pSide) pSide->mark[0]  = true;
						}
						else
						{
							if(pSide) pSide->symbol[0]= true;
						}
					}
					else
					{
						if(pSide)
						{
							pSide->mark[0]  = true;
							pSide->symbol[0]= false; 
						}
					}
				}
			}
			pTmpList = pTmpList->next;
		}

		// extract elbow bottom face
		assert(df_list == pTmpList->val.type);
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
			{
				pt[1].x = pSubList->val.data.point->x;
				pt[1].y = pSubList->val.data.point->y;
				pt[1].z = pSubList->val.data.point->z;
				pSubList= pSubList->next;

				assert(df_number == pSubList->val.type);
				radius[1]= pSubList->val.data.number;
				pSubList = pSubList->next;
				
				assert(df_integer == pSubList->val.type);
				pSubList= pSubList->next;

				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pBottom = ExtractFace(0,pSubList);
					if(pBottom)
					{
						pBottom->id    = m_nID++;
						pBottom->type  = ELBOW;
						pBottom->ele   = HSR_CIRCLE_SHAPE;
						pBottom->ptCenter.x = pt[1].x;
						pBottom->ptCenter.y = pt[1].y;
						pBottom->ptCenter.z = pt[1].z;
						pBottom->radius     = radius[1];
					
						double dot = theGeo.DotProduct(pBottom->Plane.a,pBottom->Plane.b,pBottom->Plane.c,vtx,vty,vtz);
						if((dot > (-1. + 0.01)) && (dot < (1. - 0.001)))
						{
							if(pSide) pSide->mark[1]  = true;
						}
						else
						{
							if(pSide) pSide->symbol[1]= true;
						}
					}
					else
					{
						if(pSide)
						{
							pSide->mark[1]  = true;
							pSide->symbol[1]= false;
						}
					}
				}
			}
			pTmpList = pTmpList->next;
		}

		// get center point of elbow and set radius , edge points
		if(pTmpList && (df_point == pTmpList->val.type))
		{
			if(pSide)
			{
				pSide->radius     = (radius[0] > radius[1]) ? radius[0] : radius[1];

				pSide->ptCenter.x = pTmpList->val.data.point->x;
				pSide->ptCenter.y = pTmpList->val.data.point->y;
				pSide->ptCenter.z = pTmpList->val.data.point->z;

				pSide->ptEdge[0].x = pt[0].x;
				pSide->ptEdge[0].y = pt[0].y;
				pSide->ptEdge[0].z = pt[0].z;
				pSide->ptEdge[1].x = pt[1].x;
				pSide->ptEdge[1].y = pt[1].y;
				pSide->ptEdge[1].z = pt[1].z;
			}
		}

		if(pTop)
		{
			pTop->ptEdge[0].x = pt[1].x;
			pTop->ptEdge[0].y = pt[1].y;
			pTop->ptEdge[0].z = pt[1].z;
		}
		if(pBottom)
		{
			pBottom->ptEdge[0].x = pt[0].x;
			pBottom->ptEdge[0].y = pt[0].y;
			pBottom->ptEdge[0].z = pt[0].z;
		}
		
		if(pSide) pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,ELBOW,pTop,pSide,pBottom);
	}

	return pFaceList;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractElbowEdgeFace(DList* pList,PPOINT_T pt,double* radius)
{
	assert(pList && "pList is NULL");
	assert((df_list == pList->val.type) && "pList->val.type is invalid");
	CHSRFace* pFace=NULL;

	if(pList && (df_list == pList->val.type))
	{
		DList* pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		pSubList = pList->val.data.list;
		if(pSubList && (df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
		{
			pt->x  = pSubList->val.data.point->x;
			pt->y  = pSubList->val.data.point->y;
			pt->z  = pSubList->val.data.point->z;
			*radius= pSubList->next->val.data.number;
				
			pSubList = pSubList->next;
			pSubList = pSubList->next;
			pSubList = pSubList->next;
			if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
			{
				if(NULL != (pFace = ExtractFace(0,pSubList)))
				{
					pFace->id         = m_nID++;
					pFace->ptCenter.x = pt->x;
					pFace->ptCenter.y = pt->y;
					pFace->ptCenter.z = pt->z;
					pFace->radius     = *radius;
		
					if(pHSRApp->IsParallelPlane(pFace))
					{
						pHSRApp->FreeFace(pFace);
						pFace = NULL;
					}
				}
			}
			pList = pList->next;
		}
	}
	
	return pFace;
}

//	parameter	:
//	description	: extract SW-Elbow from pList, and push it to pFaceList.
//	remarks		: [BUG FIX]
//			: 2002.03.15 - SIDEFACEEX[0] and SIDEFACEEX[1]'s radius must be the same radius of TOPFACE.
//			: 2002.04.02 - append min size checking routine.
//	returns		:
CHSRFace* CHSRImport::ExtractSWElbow(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "pList is NULL");
	assert((df_list == pList->val.type) && "pList's type is invalid");

	if(pList && (df_list == pList->val.type))
	{
		double radius[6]={0};
		POINT_T pt[6]={0};
		PHSR_VERTEX ptCenterline[2]={0};
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		
		DList *pTmpList = NULL;//,*pSubList = NULL;
		pTmpList = pList->val.data.list;
		// extract side face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSide = ExtractSideFace(pTmpList,true)))
			{
				pSide->type = SW_ELBOW;
				pSide->ele  = SIDEFACE;
			}

			pTmpList = pTmpList->next; // skip side face list.
			pTmpList = pTmpList->next; // skip center line list.
		}
		
		// extract elbow top face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTop = ExtractElbowEdgeFace(pTmpList,&pt[0],&radius[0])))
			{
				pTop->type  = SW_ELBOW;
				pTop->ele   = TOPFACE;
				pTop->ptCenter.x = pt[0].x;
				pTop->ptCenter.y = pt[0].y;
				pTop->ptCenter.z = pt[0].z;
				pTop->radius     = radius[0];
			}

			pTmpList = pTmpList->next; // skip elbow top list
		}

		// extract elbow bottom face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottom = ExtractElbowEdgeFace(pTmpList,&pt[1],&radius[1])))
			{
				pBottom->type  = SW_ELBOW;
				pBottom->ele   = BOTTOMFACE;
				pBottom->ptCenter.x = pt[1].x;
				pBottom->ptCenter.y = pt[1].y;
				pBottom->ptCenter.z = pt[1].z;
				pBottom->radius     = radius[1];

				pBottom->ptEdge[0].x = pt[0].x;
				pBottom->ptEdge[0].y = pt[0].y;
				pBottom->ptEdge[0].z = pt[0].z;
			}

			pTmpList = pTmpList->next; // skip elbow top list
		}

		// set top edge point.
		if(pTop)
		{
			pTop->ptEdge[0].x = pt[1].x;
			pTop->ptEdge[0].y = pt[1].y;
			pTop->ptEdge[0].z = pt[1].z;
		}

		// extract elbow center point
		if(pTmpList && (df_point == pTmpList->val.type))
		{
			if(pSide)
			{
				pSide->ptCenter.x = pTmpList->val.data.point->x;
				pSide->ptCenter.y = pTmpList->val.data.point->y;
				pSide->ptCenter.z = pTmpList->val.data.point->z;
			}
			
			pTmpList = pTmpList->next;
		}
		
		// set edge point
		if(pSide)
		{
			pSide->ptEdge[0].x = pt[0].x;
			pSide->ptEdge[0].y = pt[0].y;
			pSide->ptEdge[0].z = pt[0].z;
			pSide->radius      = radius[0];

			pSide->ptEdge[1].x = pt[1].x;
			pSide->ptEdge[1].y = pt[1].y;
			pSide->ptEdge[1].z = pt[1].z;
		}

		//<--
		//--> skip min circle.
		pTmpList = pTmpList->next;
		pTmpList = pTmpList->next;
		//<--

		CHSRFace *pSideEx[2]={0},*pTopEx[2]={0},*pBottomEx[2]={0};
		// extract elbow sidex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSideEx[0] = ExtractSideFace(pTmpList)))
			{
				pSideEx[0]->type = SW_ELBOW;
				pSideEx[0]->ele  = SIDEFACEX;
				pFaceList        = pHSRApp->AppendFace(pFaceList,pSideEx[0]);
			}
			pTmpList = pTmpList->next; // skip side face
		}
		// extract elbow topex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTopEx[0] = ExtractElbowEdgeFace(pTmpList,&pt[2],&radius[2])))
			{
				pTopEx[0]->type = SW_ELBOW;
				pTopEx[0]->ele  = TOPFACEX;
				//pFaceList       = pHSRApp->AppendFace(pFaceList,pTopEx[0]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}
		// extract elbow bottomex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottomEx[0] = ExtractElbowEdgeFace(pTmpList,&pt[3],&radius[3])))
			{
				pBottomEx[0]->type = SW_ELBOW;
				pBottomEx[0]->ele  = BOTTOMFACEX;
				//pFaceList          = pHSRApp->AppendFace(pFaceList,pBottomEx[0]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}

		// append side centerline
		if(pSideEx[0] && (NULL != (ptCenterline[0] = pHSRApp->NewVertex())) && (NULL != (ptCenterline[1] = pHSRApp->NewVertex())))
		{
			pSideEx[0]->ptEdge[0].x= pt[2].x;
			pSideEx[0]->ptEdge[0].y= pt[2].y;
			pSideEx[0]->ptEdge[0].z= pt[2].z;

			pSideEx[0]->ptEdge[1].x= pt[3].x;
			pSideEx[0]->ptEdge[1].y= pt[3].y;
			pSideEx[0]->ptEdge[1].z= pt[3].z;

			//pSideEx[0]->radius     = radius[2];
			pSideEx[0]->radius     = radius[0];
			// save center line
			ptCenterline[0]->render= INTERSECTION;
			ptCenterline[0]->pt->x = pt[2].x;
			ptCenterline[0]->pt->y = pt[2].y;
			ptCenterline[0]->pt->z = pt[2].z;
			
			ptCenterline[1]->render= INTERSECTION;
			ptCenterline[1]->pt->x = pt[3].x;
			ptCenterline[1]->pt->y = pt[3].y;
			ptCenterline[1]->pt->z = pt[3].z;
			
			pSideEx[0]->pCenterline = AppendVertex(pSideEx[0]->pCenterline,ptCenterline[0]);
			pSideEx[0]->pCenterline = AppendVertex(pSideEx[0]->pCenterline,ptCenterline[1]);
		}

		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSideEx[1] = ExtractSideFace(pTmpList)))
			{
				pSideEx[1]->type = SW_ELBOW;
				pSideEx[1]->ele  = SIDEFACEX;
				pFaceList        = pHSRApp->AppendFace(pFaceList,pSideEx[1]);
			}

			pTmpList = pTmpList->next; // skip side face
		}
		// extract elbow topex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTopEx[1] = ExtractElbowEdgeFace(pTmpList,&pt[4],&radius[4])))
			{
				pTopEx[1]->type = SW_ELBOW;
				pTopEx[1]->ele  = TOPFACEX;
				//pFaceList      = pHSRApp->AppendFace(pFaceList,pTopEx[1]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}
		// extract elbow bottomex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottomEx[1] = ExtractElbowEdgeFace(pTmpList,&pt[5],&radius[5])))
			{
				pBottomEx[1]->type = SW_ELBOW;
				pBottomEx[1]->ele  = BOTTOMFACEX;
				//pFaceList          = pHSRApp->AppendFace(pFaceList,pBottomEx[1]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}

		// append side centerline
		if(pSideEx[1] && (NULL != (ptCenterline[0] = pHSRApp->NewVertex())) && (NULL != (ptCenterline[1] = pHSRApp->NewVertex())))
		{
			pSideEx[1]->ptEdge[0].x= pt[4].x;
			pSideEx[1]->ptEdge[0].y= pt[4].y;
			pSideEx[1]->ptEdge[0].z= pt[4].z;

			pSideEx[1]->ptEdge[1].x= pt[5].x;
			pSideEx[1]->ptEdge[1].y= pt[5].y;
			pSideEx[1]->ptEdge[1].z= pt[5].z;

			//pSideEx[1]->radius     = radius[4];
			pSideEx[1]->radius     = radius[0];
			// save center line
			ptCenterline[0]->render= INTERSECTION;
			ptCenterline[0]->pt->x = pt[4].x;
			ptCenterline[0]->pt->y = pt[4].y;
			ptCenterline[0]->pt->z = pt[4].z;
			
			ptCenterline[1]->render= INTERSECTION;
			ptCenterline[1]->pt->x = pt[5].x;
			ptCenterline[1]->pt->y = pt[5].y;
			ptCenterline[1]->pt->z = pt[5].z;
			
			pSideEx[1]->pCenterline = AppendVertex(pSideEx[1]->pCenterline,ptCenterline[0]);
			pSideEx[1]->pCenterline = AppendVertex(pSideEx[1]->pCenterline,ptCenterline[1]);
		}

		double vtx=0,vty=0,vtz=0;
		double dot=0;
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			vtx = 0.;
			vty = 0.;
			vtz = 1.;
		}
		else
		{
			vtx = 1.;
			vty = 0.;
			vtz = 0.;
		}
		
		if(pTop)
		{
			dot = theGeo.DotProduct(pTop->Plane.a,pTop->Plane.b,pTop->Plane.c,vtx,vty,vtz);
			if((dot > (-1. + 0.01)) && (dot < (1. - 0.01)))
			{
				if(pSide)
				{
					pSide->mark[0]     = true;
					if(pSideEx[0]) pSideEx[0]->mark[0]= true;

					pSide->ptMark[0].x = pt[3].x;
					pSide->ptMark[0].y = pt[3].y;
					pSide->ptMark[0].z = pt[3].z;
				}
			}
			else
			{
				if(pSide) pSide->symbol[0]= true;
			}
		}
		else
		{
			if(pSide)
			{
				pSide->mark[0]     = true;
				pSide->symbol[0]   = false;
				if(pSideEx[0]) pSideEx[0]->mark[0]= true;

				pSide->ptMark[0].x = pt[3].x;
				pSide->ptMark[0].y = pt[3].y;
				pSide->ptMark[0].z = pt[3].z;
			}
		}

		if(pBottom)
		{
			dot = theGeo.DotProduct(pBottom->Plane.a,pBottom->Plane.b,pBottom->Plane.c,vtx,vty,vtz);
			if((dot > (-1. + 0.01)) && (dot < (1. - 0.01)))
			{
				if(pSide)
				{
					pSide->mark[1]     = true;
					if(pSideEx[1]) pSideEx[1]->mark[1]=true;

					pSide->ptMark[1].x = pt[4].x;
					pSide->ptMark[1].y = pt[4].y;
					pSide->ptMark[1].z = pt[4].z;
				}
			}
			else
			{
				if(pSide) pSide->symbol[1]= true;
			}
		}
		else
		{
			if(pSide)
			{
				pSide->mark[1]     = true;
				pSide->symbol[1]   = false;
				if(pSideEx[1]) pSideEx[1]->mark[1]= true;

				pSide->ptMark[1].x = pt[5].x;
				pSide->ptMark[1].y = pt[5].y;
				pSide->ptMark[1].z = pt[5].z;
			}
		}
	
		if(pSide) pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,SW_ELBOW,pTop,pSide,pBottom);
	}

	return pFaceList;
}

//	parameter	:
//	description	: extract cone faces
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractCone(CHSRFace* pFaceList,DList* pList,HSR_PRIMITIVE_E id)
{
	assert(pList && "dlist is invalid - ExtractCone");
	assert(df_list == pList->val.type);

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList* pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		int count = 0;

		DList* pTmpList = pList->val.data.list;	
		if(pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list)
		{
			// extract side face
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_integer == pSubList->val.type))
			{
				count    = pSubList->val.data.integer;
				pSubList = pSubList->next;

				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pSide = ExtractFace(count,pSubList)))
					{
						pSide->id  = m_nID++;
						pSide->type= id;
						pSide->ele = HSR_POLYGON_SHAPE;
					}
				}
			}
		}
		pTmpList = pTmpList->next;

		double  radius[2] = {0};
		POINT_T ptCenter[2]={0};
		// extract edge face
		if(pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list)
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_point == pSubList->val.type) && pSubList->val.data.point)
			{
				ptCenter[0].x = pSubList->val.data.point->x;
				ptCenter[0].y = pSubList->val.data.point->y;
				ptCenter[0].z = pSubList->val.data.point->z;
				pSubList = pSubList->next;
				
				assert(pSubList && (df_number == pSubList->val.type));
				radius[0]= pSubList->val.data.number;
				pSubList = pSubList->next;
				
				assert(pSubList && (df_integer == pSubList->val.type));
				count    = pSubList->val.data.integer;
				pSubList = pSubList->next; // skip number of points
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pTop = ExtractFace(count,pSubList);
					if(pTop)
					{
						pTop->id  = m_nID++;
						pTop->type= id;
						pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;

						pTop->ptCenter.x = ptCenter[0].x;
						pTop->ptCenter.y = ptCenter[0].y;
						pTop->ptCenter.z = ptCenter[0].z;
						pTop->radius     = radius[0];
					}
				}
			}
		}
		pTmpList = pTmpList->next;

		// extract edge face
		if(pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list)
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
			{
				ptCenter[1].x = pSubList->val.data.point->x;
				ptCenter[1].y = pSubList->val.data.point->y;
				ptCenter[1].z = pSubList->val.data.point->z;
				pSubList = pSubList->next;
				
				assert(pSubList && (df_number == pSubList->val.type));
				radius[1]= pSubList->val.data.number;
				pSubList = pSubList->next;
				
				assert(pSubList && (df_integer == pSubList->val.type));
				count    = pSubList->val.data.integer;
				pSubList = pSubList->next;
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pBottom = ExtractFace(count,pSubList);
					if(pBottom)
					{
						pBottom->id  = m_nID++;
						pBottom->type= id;
						pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;

						pBottom->ptCenter.x = ptCenter[1].x;
						pBottom->ptCenter.y = ptCenter[1].y;
						pBottom->ptCenter.z = ptCenter[1].z;
						pBottom->radius     = radius[1];
					}
				}
			}
		}
		
		pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,CONE,pTop,pSide,pBottom);
	}

	return pFaceList;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractTorus(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "dlist is invalid");
	assert(df_list == pList->val.type);

	if(pList && (pList->val.type == df_list))
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		CHSRApp *pHSRApp = CHSRApp::GetInstance();

		if(pTmpList && (pTmpList->val.type == df_list))
		{
			CHSRFace* pSide = NULL;

			pSubList = pTmpList->val.data.list;
			if(df_integer == pSubList->val.type)
			{
				int count = pSubList->val.data.integer;
				pSubList = pSubList->next;
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pSide = ExtractFace(count,pSubList)))
					{
						pSide->id  = m_nID++;
						pSide->type= TORUS;
						pSide->ele = HSR_POLYGON_SHAPE;

						pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
					}
				}
			}
		}

	}

	return pFaceList;
}

//	parameter	:
//	description	: extract pipe from dlist.
//	remarks		: BUG FIX : 1. when pipe is vertical, face type is not setted. corrected it.-[2001-01-25]
//			: 2002.05.22 - valid region of pipe center line's point is 3 digits under dot.
//	returns		:
CHSRFace* CHSRImport::ExtractPipe(CHSRFace* pFaceList,DList* pList)
{
	assert(pList && "dlist is NULL");
	assert((df_list == pList->val.type) && "dlist's type is invaid");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;	
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		int     count = 0;
		double  radius[2]={0};
		POINT_T ptCenter[2]={0};
		pSubList = pTmpList->val.data.list;
		if(df_integer == pSubList->val.type)
		{
			if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
			{
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					pSide = ExtractFace(count,pSubList);
					if(pSide)
					{
						pSide->id  = m_nID++;
						pSide->type= PIPE;
						pSide->ele = HSR_RECTANGLE_SHAPE;
					}
				}
			}
		}
		pTmpList = pTmpList->next;

		if(pSide)
		{
			if(pTmpList && (df_list == pTmpList->val.type))
			{ // top face
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pTop = ExtractFace(count,pSubList);
							if(pTop)
							{
								pTop->ptCenter.x = ptCenter[0].x;
								pTop->ptCenter.y = ptCenter[0].y;
								pTop->ptCenter.z = ptCenter[0].z;
								pTop->radius     = radius[0];
								
								pTop->id  = m_nID++;
								pTop->type= PIPE;
								pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;
			
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{ // bottom face
					if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pBottom = ExtractFace(count,pSubList);
							if(pBottom)
							{
								pBottom->ptCenter.x = ptCenter[1].x;
								pBottom->ptCenter.y = ptCenter[1].y;
								pBottom->ptCenter.z = ptCenter[1].z;
								pBottom->radius     = radius[1];
								
								pBottom->id = m_nID++;
								pBottom->type= PIPE;
								pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}

			//--> follow up routine
			// set center line and invalidate edge line of side face
			if(pSide)
			{
				pSide->radius = (radius[0] > radius[1]) ? radius[0] : radius[1];
				
				PHSR_VERTEX  pvt[2] = {0};
				PPOINT_T ppt[2] = {0};
				pvt[0] = pHSRApp->alloc_vertex();
				pvt[1] = pHSRApp->alloc_vertex();
				ppt[0] = pHSRApp->alloc_point();
				ppt[1] = pHSRApp->alloc_point();
				if(pvt[0] && pvt[1] && ppt[0] && ppt[1])
				{
					ppt[0]->x = ptCenter[0].x;
					//ppt[0]->x = KMath::round(ppt[0]->x,3);
					ppt[0]->y = ptCenter[0].y;
					//ppt[0]->y = KMath::round(ppt[0]->y,3);
					ppt[0]->z = ptCenter[0].z;
					//ppt[0]->z = KMath::round(ppt[0]->z,3);
					
					ppt[1]->x = ptCenter[1].x;
					//ppt[1]->x = KMath::round(ppt[1]->x,3);
					ppt[1]->y = ptCenter[1].y;
					//ppt[1]->y = KMath::round(ppt[1]->y,3);
					ppt[1]->z = ptCenter[1].z;
					//ppt[1]->z = KMath::round(ppt[1]->z,3);
					
					pSide->pCenterline = ListPush(pSide->pCenterline,pvt[0]);
					pSide->pCenterline->pt = ppt[0];
					
					pSide->pCenterline = ListPush(pSide->pCenterline,pvt[1]);
					pSide->pCenterline->next->pt = ppt[1];
				}
				else
				{
					if(pvt[0]) pHSRApp->free_vertex(pvt[0]);
					if(pvt[1]) pHSRApp->free_vertex(pvt[1]);
					if(ppt[0]) pHSRApp->free_point(ppt[0]);
					if(ppt[1]) pHSRApp->free_point(ppt[1]);
				}
			}
		}
		else if(NULL == pSide)
		{ // vertical pipe.
			DList *pTopDList = NULL,*pBottomDList = NULL;

			if(pTmpList && (df_list == pTmpList->val.type))
			{ // top face list
				pSubList = pTmpList->val.data.list;
				if(pSubList && (df_point == pSubList->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
					{
						pTopDList = pSubList;
					}
				}
			}
			pTmpList = pTmpList->next;

			if(pTmpList && (df_list == pTmpList->val.type))
			{ // bottom face list
				pSubList = pTmpList->val.data.list;
				if(pSubList && (df_point == pSubList->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
					{
						pBottomDList = pSubList;
					}
				}
			}

			if(TOP == pHSRApp->m_clsView.GetViewPoint())
			{
				if(pHSRApp->IsGreatNumber(ptCenter[0].z,ptCenter[1].z))
				{
					// top face is higher than bottom face.
					if(pTopDList && (df_list == pTopDList->val.type) && pTopDList->val.data.list)
					{
						pTop = ExtractFace(count,pTopDList);
						if(pTop)
						{
							pTop->ptCenter.x = ptCenter[0].x;
							pTop->ptCenter.y = ptCenter[0].y;
							pTop->ptCenter.z = ptCenter[0].z;
							pTop->radius     = radius[0];
							
							pTop->id = m_nID++;
							pTop->type= PIPE;
							pTop->ele = SECTION;
						}
					}
				}
				else
				{
					// bottom face is higher than top face.
					if(pBottomDList && (df_list == pBottomDList->val.type) && pBottomDList->val.data.list)
					{
						pBottom = ExtractFace(count,pBottomDList);
						if(pBottom)
						{
							pBottom->ptCenter.x = ptCenter[0].x;
							pBottom->ptCenter.y = ptCenter[0].y;
							pBottom->ptCenter.z = ptCenter[0].z;
							pBottom->radius     = radius[0];
							
							pBottom->id = m_nID++;
							pBottom->type= PIPE;
							pBottom->ele = SECTION;
						}
					}
				}
			}
			else
			{
				if(pHSRApp->IsGreatNumber(ptCenter[0].x,ptCenter[1].x))
				{
					if(pTopDList && (df_list == pTopDList->val.type) && pTopDList->val.data.list)
					{
						pTop = ExtractFace(count,pTopDList);
						if(pTop)
						{
							pTop->ptCenter.x = ptCenter[0].x;
							pTop->ptCenter.y = ptCenter[0].y;
							pTop->ptCenter.x = ptCenter[0].x;
							pTop->radius     = radius[0];
							
							pTop->id = m_nID++;
							pTop->type= PIPE;
							pTop->ele = SECTION;
						}
					}
				}
				else
				{
					if(pBottomDList && (df_list == pBottomDList->val.type) && pBottomDList->val.data.list)
					{
						pBottom = ExtractFace(count,pBottomDList);
						if(pBottom)
						{
							pBottom->ptCenter.x = ptCenter[0].x;
							pBottom->ptCenter.y = ptCenter[0].y;
							pBottom->ptCenter.x = ptCenter[0].x;
							pBottom->radius     = radius[0];
							
							pBottom->id = m_nID++;
							pBottom->type= PIPE;
							pBottom->ele = SECTION;
						}
					}
				}
			}
		}
		
		pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,PIPE,pTop,pSide,pBottom);
	}

	return pFaceList;
}

//	parameter	:
//	description	: extract TEE from dlist
//	remarks		: [BUG FIX]
//			: 2002.04.26 - add extraction code of tee symbol(when tee is vertical).
//	returns		:
CHSRFace* CHSRImport::ExtractTee(CHSRFace* pFaceList,DList* pList)
{
	assert(pList && "pList is NULL");
	assert((df_list == pList->val.type) && "pList's type is invalid");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		int     count = 0;
		double  nRadius[2][2] ={0};
		POINT_T ptCenter[2][2]={0};
		for(int i = 0;(i < 2) && (pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list);i++)
		{
			CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;

			pSubList = pTmpList->val.data.list;
			// extract side face
			if(df_integer == pSubList->val.type)
			{
				if(ExtractFaceInfo(&(ptCenter[i][0]),&(nRadius[i][0]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pSide = ExtractFace(count,pSubList);
						if(pSide)
						{
							pSide->id  = m_nID++;
							pSide->type= TEE;
							pSide->ele = HSR_RECTANGLE_SHAPE;
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			 // extract top face
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[i][0]),&(nRadius[i][0]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pTop = ExtractFace(count,pSubList);
							if(pTop)
							{
								pTop->ptCenter.x = ptCenter[i][0].x;
								pTop->ptCenter.y = ptCenter[i][0].y;
								pTop->ptCenter.z = ptCenter[i][0].z;
								pTop->radius     = nRadius[i][0];

								pTop->id  = m_nID++;
								pTop->type= TEE;
								pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			 // extract bottom face
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{ // bottom face
					if(ExtractFaceInfo(&(ptCenter[i][1]),&(nRadius[i][1]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pBottom = ExtractFace(count,pSubList);
							if(pBottom)
							{
								pBottom->ptCenter.x = ptCenter[i][1].x;
								pBottom->ptCenter.y = ptCenter[i][1].y;
								pBottom->ptCenter.z = ptCenter[i][1].z;
								pBottom->radius     = nRadius[i][1];

								pBottom->id  = m_nID++;
								pBottom->type= TEE;
								pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			//--> follow up routine
			// set center line and invalidate edge line of side face
			if(pSide)
			{
				pSide->radius = (nRadius[i][0] > nRadius[i][1]) ? nRadius[i][0] : nRadius[i][1];
				
				PHSR_VERTEX  pvt[2] = {0};
				PPOINT_T ppt[2] = {0};
				pvt[0] = pHSRApp->alloc_vertex();
				pvt[1] = pHSRApp->alloc_vertex();
				ppt[0] = pHSRApp->alloc_point();
				ppt[1] = pHSRApp->alloc_point();
				if(pvt[0] && pvt[1] && ppt[0] && ppt[1])
				{
					ppt[0]->x = ptCenter[i][0].x;
					ppt[0]->y = ptCenter[i][0].y;
					ppt[0]->z = ptCenter[i][0].z;
					
					ppt[1]->x = ptCenter[i][1].x;
					ppt[1]->y = ptCenter[i][1].y;
					ppt[1]->z = ptCenter[i][1].z;
					
					pSide->pCenterline = ListPush(pSide->pCenterline,pvt[0]);
					pSide->pCenterline->pt = ppt[0];
					
					pSide->pCenterline = ListPush(pSide->pCenterline,pvt[1]);
					pSide->pCenterline->next->pt = ppt[1];
				}
				else
				{
					if(pvt[0]) pHSRApp->free_vertex(pvt[0]);
					if(pvt[1]) pHSRApp->free_vertex(pvt[1]);
					if(ppt[0]) pHSRApp->free_point(ppt[0]);
					if(ppt[1]) pHSRApp->free_point(ppt[1]);
				}
			}

			if(0 == i)
			{
				if(pSide)
				{
					PIPELINEMODE mode=pHSRApp->GetEnv()->GetPipeLineMode();
					double nCriticalRadius = pHSRApp->GetEnv()->GetCriticalRadius();
					if((SINGLE_LINE == mode) || ((BOTH == mode) && pSide->radius <= nCriticalRadius))
					{
						pSide->mark[0] = ((NULL == pTop) || (HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pTop->ele)))   ? true : false;
						// it's so dangerous condition check.
						pSide->mark[1] = ((NULL == pBottom) || (HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pBottom->ele)))? true : false;
						pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEE,pTop,pSide,pBottom);
					}else	pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEE,pTop,pSide,pBottom);
				}
				else
				{
					if(pTop)    pTop->ele    = SYMBOL;
					if(pBottom) pBottom->ele = SYMBOL;
					pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEE,pTop,pSide,pBottom);
				}
			}
			else
			{
				PIPELINEMODE mode=pHSRApp->GetEnv()->GetPipeLineMode();
				double nCriticalRadius = pHSRApp->GetEnv()->GetCriticalRadius();
				if((SINGLE_LINE == mode) || ((BOTH == mode) && nRadius[i][0] <= nCriticalRadius))
				{
					if(pSide)
					{
						pSide->mark[0] = false;
						// it's so dangerous condition check.
						pSide->mark[1] = ((NULL == pBottom) || (HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pBottom->ele)))? true : false;
					}
					
					// 2'nd pipe's top face is intersection point of tee.
					if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))))
					{
						if(pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele)) && (pTop->ptCenter.z >= pBottom->ptCenter.z))
						{
							LINE_T line={0};
							line.ptStart = ptCenter[0][0];
							line.ptEnd   = ptCenter[0][1];
							PHSR_VERTEX pSymbol=CreateTeeSymbol(line,pTop->radius);
							if(pSymbol)
							{
								for(PHSR_VERTEX ptr=pSymbol;ptr;ptr = ptr->next)
								{
									ptr->pt->x+= ptCenter[1][0].x;
									ptr->pt->y+= ptCenter[1][0].y;
									ptr->pt->z = ptCenter[1][0].z;
								}
								pHSRApp->free_vertex(pTop->pHead);
								pTop->ele   = SYMBOL;
								pTop->pHead = pSymbol;
							}
						}
						else
						{
							if(pTop)    pTop->ele    = SYMBOL;
							if(pBottom) pBottom->ele = SYMBOL;
						}
					}
					else
					{
						if(pTop)    pTop->ele    = SYMBOL;
						if(pBottom) pBottom->ele = SYMBOL;
					}
					pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEE,pTop,pSide,pBottom);
				}
				else
				{
					if(pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele)))
					{
						pHSRApp->delete_hsr_face(pTop);
						pTop = NULL;
					}
					pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEE,NULL,pSide,pBottom);
				}
			}
		}
	}

	return pFaceList;
}

//	parameter	:
//	description	: extract flange from dlist.
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractFlange(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "dlist is NULL");
	assert((df_list == pList->val.type) && "pList->val.type is NULL");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;	
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		int     count = 0;
		double  radius[2]={0};
		POINT_T ptCenter[2]={0};
		for(int i = 0;i < 2;i++,pTmpList = pTmpList->next)
		{
			pSide  = NULL;
			pTop   = NULL;
			pBottom= NULL;

			pSubList = pTmpList->val.data.list;
			if(df_integer == pSubList->val.type)
			{
				if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pSide = ExtractFace(count,pSubList);
						if(pSide)
						{
							pSide->id  = m_nID++;
							pSide->type= FLANGE;
							pSide->ele = HSR_RECTANGLE_SHAPE;
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			if(pTmpList && (df_list == pTmpList->val.type))
			{ // top face
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pTop = ExtractFace(count,pSubList);
							if(pTop)
							{
								pTop->ptCenter.x = ptCenter[0].x;
								pTop->ptCenter.y = ptCenter[0].y;
								pTop->ptCenter.z = ptCenter[0].z;
								pTop->radius     = radius[0];

								pTop->id  = m_nID++;
								pTop->type= FLANGE;
								pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{ // bottom face
					if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pBottom = ExtractFace(count,pSubList);
							if(pBottom)
							{
								pBottom->ptCenter.x = ptCenter[1].x;
								pBottom->ptCenter.y = ptCenter[1].y;
								pBottom->ptCenter.z = ptCenter[1].z;
								pBottom->radius     = radius[1];

								pBottom->id = m_nID++;
								pBottom->type= FLANGE;
								pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,FLANGE,pTop,pSide,pBottom);
		}
	}

	return pFaceList;
}

//	parameter	:
//	description	: read box faces from dlist.
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractBox(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "pList is invalid");
	assert((df_list == pList->val.type) && "pList->val.type is invalid");

	if(pList && (df_list == pList->val.type))
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
				
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			CHSRFace* pSide = NULL;

			for(int i = 0;pTmpList && (i < 6);i++,pTmpList = pTmpList->next)
			{
				pSubList = pTmpList->val.data.list;
				if(pSubList && (df_point == pSubList->val.type))
				{
					pSide = NULL;
					pSide = ExtractFace(4,pTmpList);
					if(pSide)
					{
						pSide->id  = m_nID++;
						pSide->type= BOX;
						pSide->ele = HSR_RECTANGLE_SHAPE;
						
						pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
					}
				}
			}
		}
	}

	return pFaceList;
}

//	parameter	:
//	description	: read dome faces from dlist
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractDome(DList* pList,CHSRFace* pFaceList)
{
	assert(pList && "dlist is invalid");
	assert(df_list == pList->val.type);

	if(pList && (df_list == pList->val.type))
	{
		CHSRFace *pSide = NULL,*pTop = NULL;
		POINT_T pt={0};
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		
		int count=0;
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_integer == pSubList->val.type))
			{
				count = pSubList->val.data.integer;
				pSubList = pSubList->next;

				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pSide = ExtractFace(count,pSubList)))
					{
						pSide->id  = m_nID++;
						pSide->type= DOME;
						pSide->ele = HSR_RECTANGLE_SHAPE;
					}
				}
			}
		}
		pTmpList = pTmpList->next;

		if(pTmpList && (df_list == pTmpList->val.type))
		{
			pSubList = pTmpList->val.data.list;
			if(pSubList && (df_point == pSubList->val.type))
			{
				pt.x  = pSubList->val.data.point->x;
				pt.y  = pSubList->val.data.point->y;
				pt.z  = pSubList->val.data.point->z;
				pSubList = pSubList->next;

				assert(pSubList && (df_number == pSubList->val.type));
				double radius   = pSubList->val.data.number;
				pSubList = pSubList->next;

				assert(pSubList && (df_integer == pSubList->val.type));
				count = pSubList->val.data.integer;
				pSubList = pSubList->next;
				if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
				{
					if(NULL != (pTop = ExtractFace(count,pSubList)))
					{
						pTop->id  = m_nID++;
						pTop->type= DOME;
						pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;

						pTop->ptCenter.x = pt.x;
						pTop->ptCenter.y = pt.y;
						pTop->ptCenter.z = pt.z;
						pTop->radius     = radius;
					}
				}
			}
		}
		
		pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,DOME,pTop,pSide,NULL);
	}

	return pFaceList;
}

//	parameter	:
//	description	: read extended pipe faces from dlist
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractExtendPipe(CHSRFace* pFaceList,DList* pList)
{
	assert(pList && "dlist is invalid");
	assert((df_list == pList->val.type) && "pList->val.type is invalid");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		CHSRFace *pSide = NULL,*pTop= NULL,*pBottom = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		int     count = 0;
		double  radius[2]={0};
		POINT_T ptCenter[2]={0};
		DList *pTmpList = pList->val.data.list;
		DList* pSubList = NULL;
		for(int i = 0; (i < 3) && (pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list);i++)
		{
			memset(radius,0,sizeof(double)*2);
			memset(ptCenter,0,sizeof(POINT_T)*2);
			pSide   = NULL;
			pTop    = NULL;
			pBottom = NULL;

			pSubList = pTmpList->val.data.list;
			if(df_integer == pSubList->val.type)
			{
				if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pSide = ExtractFace(count,pSubList);
						if(pSide)
						{
							pSide->id  = m_nID++;
							pSide->type= PIPE;
							pSide->ele = HSR_RECTANGLE_SHAPE;
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			if(pSide)
			{
				if(pTmpList && (df_list == pTmpList->val.type))
				{ // top face
					pSubList = pTmpList->val.data.list;
					if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
					{
						if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
						{
							if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
							{
								pTop = ExtractFace(count,pSubList);
								if(pTop)
								{
									pTop->ptCenter.x = ptCenter[0].x;
									pTop->ptCenter.y = ptCenter[0].y;
									pTop->ptCenter.z = ptCenter[0].z;
									pTop->radius     = radius[0];
									
									pTop->id  = m_nID++;
									pTop->type= PIPEX;
									pTop->ele = (pTop->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
								}
							}
						}
					}
				}
				pTmpList = pTmpList->next;
				
				if(pTmpList && (df_list == pTmpList->val.type))
				{
					pSubList = pTmpList->val.data.list;
					if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
					{ // bottom face
						if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
						{
							if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
							{
								pBottom = ExtractFace(count,pSubList);
								if(pBottom)
								{
									pBottom->ptCenter.x = ptCenter[1].x;
									pBottom->ptCenter.y = ptCenter[1].y;
									pBottom->ptCenter.z = ptCenter[1].z;
									pBottom->radius     = radius[1];
									
									pBottom->id  = m_nID++;
									pBottom->type= PIPEX;
									pBottom->ele = (pBottom->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
								}
							}
						}
					}
				}
				pTmpList = pTmpList->next;

				//--> follow up routine
				// set center line and invalidate edge line of side face
				if(pSide)
				{
					pSide->radius = (radius[0] > radius[1]) ? radius[0] : radius[1];
					
					PHSR_VERTEX  pvt[2] = {0};
					PPOINT_T ppt[2] = {0};
					pvt[0] = pHSRApp->alloc_vertex();
					pvt[1] = pHSRApp->alloc_vertex();
					ppt[0] = pHSRApp->alloc_point();
					ppt[1] = pHSRApp->alloc_point();
					if(pvt[0] && pvt[1] && ppt[0] && ppt[1])
					{
						ppt[0]->x = ptCenter[0].x;
						ppt[0]->y = ptCenter[0].y;
						ppt[0]->z = ptCenter[0].z;
						
						ppt[1]->x = ptCenter[1].x;
						ppt[1]->y = ptCenter[1].y;
						ppt[1]->z = ptCenter[1].z;
						
						pSide->pCenterline = ListPush(pSide->pCenterline,pvt[0]);
						pSide->pCenterline->pt = ppt[0];
						
						pSide->pCenterline = ListPush(pSide->pCenterline,pvt[1]);
						pSide->pCenterline->next->pt = ppt[1];
					}
					else
					{
						if(pvt[0]) pHSRApp->free_vertex(pvt[0]);
						if(pvt[1]) pHSRApp->free_vertex(pvt[1]);
						if(ppt[0]) pHSRApp->free_point(ppt[0]);
						if(ppt[1]) pHSRApp->free_point(ppt[1]);
					}
				}
			}
			else if(NULL == pSide)
			{
				DList *pTopDList = NULL,*pBottomDList = NULL;

				if(pTmpList && (df_list == pTmpList->val.type))
				{
					pSubList = pTmpList->val.data.list;
					if(pSubList && (df_point == pSubList->val.type))
					{
						if(ExtractFaceInfo(&(ptCenter[0]),&(radius[0]),&count,&pSubList))
						{
							pTopDList = pSubList;
						}
					}
				}
				pTmpList = pTmpList->next;

				if(pTmpList && (df_list == pTmpList->val.type))
				{ // top face
					pSubList = pTmpList->val.data.list;
					if(pSubList && (df_point == pSubList->val.type))
					{
						if(ExtractFaceInfo(&(ptCenter[1]),&(radius[1]),&count,&pSubList))
						{
							pBottomDList = pSubList;
						}
					}
				}
				pTmpList = pTmpList->next;

				if(TOP == pHSRApp->m_clsView.GetViewPoint())
				{
					if(pHSRApp->IsGreatNumber(ptCenter[0].z,ptCenter[1].z))
					{
						if(pTopDList && (df_list == pTopDList->val.type) && pTopDList->val.data.list)
						{
							pTop = ExtractFace(count,pTopDList);
							if(pTop)
							{
								pTop->ptCenter.x = ptCenter[0].x;
								pTop->ptCenter.y = ptCenter[0].y;
								pTop->ptCenter.z = ptCenter[0].z;
								pTop->radius     = radius[0];
								
								pTop->id = m_nID++;
								pTop->type= PIPEX;
								pTop->ele = SECTION;
							}
						}
					}
					else
					{
						if(pBottomDList && (df_list == pBottomDList->val.type) && pBottomDList->val.data.list)
						{
							pBottom = ExtractFace(count,pBottomDList);
							if(pBottom)
							{
								pBottom->ptCenter.x = ptCenter[0].x;
								pBottom->ptCenter.y = ptCenter[0].y;
								pBottom->ptCenter.z = ptCenter[0].z;
								pBottom->radius     = radius[0];
								
								pBottom->id = m_nID++;
								pBottom->type= PIPEX;
								pBottom->ele = SECTION;
							}
						}
					}
				}
				else
				{
					if(pHSRApp->IsGreatNumber(ptCenter[0].x,ptCenter[1].x))
					{
						if(pTopDList && (df_list == pTopDList->val.type) && pTopDList->val.data.list)
						{
							pTop = ExtractFace(count,pTopDList);
							if(pTop)
							{
								pTop->ptCenter.x = ptCenter[0].x;
								pTop->ptCenter.y = ptCenter[0].y;
								pTop->ptCenter.x = ptCenter[0].x;
								pTop->radius     = radius[0];
								
								pTop->id = m_nID++;
								pTop->type= PIPEX;
								pTop->ele = SECTION;
							}
						}
					}
					else
					{
						if(pBottomDList && (df_list == pBottomDList->val.type) && pBottomDList->val.data.list)
						{
							pBottom = ExtractFace(count,pBottomDList);
							if(pBottom)
							{
								pBottom->ptCenter.x = ptCenter[0].x;
								pBottom->ptCenter.y = ptCenter[0].y;
								pBottom->ptCenter.x = ptCenter[0].x;
								pBottom->radius     = radius[0];
								
								pBottom->id = m_nID++;
								pBottom->type= PIPEX;
								pBottom->ele = SECTION;
							}
						}
					}
				}
			}
			
			pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,PIPE,pTop,pSide,pBottom);
		}
	}

	return pFaceList;
}

//	parameter	:
//	description	: read pipe bend faces from dlist
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractPipeBend(CHSRFace* pFaceList,DList* pList)
{
	assert(pList && "pList is invalid");
	assert((df_list == pList->val.type) && "pList's type is invalid");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		double radius[6]={0};
		POINT_T pt[6]={0};
		PHSR_VERTEX ptCenterline[2]={0};
		CHSRFace *pSide = NULL,*pTop = NULL,*pBottom = NULL;
		CHSRFace *pSideEx[2]={0},*pTopEx[2]={0},*pBottomEx[2]={0};
		CHSRApp* pHSRApp = (CHSRApp*)CHSRApp::GetInstance();
		
		DList *pTmpList = NULL;
		pTmpList = pList->val.data.list;
		// extract side face
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSide = ExtractSideFace(pTmpList,true)))
			{
				pSide->type = PIPE_BEND;
				pSide->ele  = SIDEFACE;
			}

			pTmpList = pTmpList->next; // skip side face list.
			pTmpList = pTmpList->next; // skip center line list.
		}
		
		// extract elbow top
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTop = ExtractElbowEdgeFace(pTmpList,&pt[0],&radius[0])))
			{
				pTop->type  = PIPE_BEND;
				pTop->ele   = TOPFACE;
				pTop->ptCenter.x = pt[0].x;
				pTop->ptCenter.y = pt[0].y;
				pTop->ptCenter.z = pt[0].z;
				pTop->radius= radius[0];
			}

			pTmpList = pTmpList->next; // skip elbow top list
		}

		// extract elbow bottom
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottom = ExtractElbowEdgeFace(pTmpList,&pt[1],&radius[1])))
			{
				pBottom->type  = PIPE_BEND;
				pBottom->ele   = BOTTOMFACE;
				pBottom->ptCenter.x = pt[1].x;
				pBottom->ptCenter.y = pt[1].y;
				pBottom->ptCenter.z = pt[1].z;
				pBottom->radius= radius[1];

				pBottom->ptEdge[0].x = pt[0].x;
				pBottom->ptEdge[0].y = pt[0].y;
				pBottom->ptEdge[0].z = pt[0].z;
			}

			pTmpList = pTmpList->next; // skip elbow top list
		}

		// set top edge point.
		if(pTop)
		{
			pTop->ptEdge[0].x = pt[1].x;
			pTop->ptEdge[0].y = pt[1].y;
			pTop->ptEdge[0].z = pt[1].z;
		}
		
		// set edge point
		if(pSide)
		{
			pSide->ptEdge[0].x = pt[0].x;
			pSide->ptEdge[0].y = pt[0].y;
			pSide->ptEdge[0].z = pt[0].z;
			pSide->radius      = radius[0];

			pSide->ptEdge[1].x = pt[1].x;
			pSide->ptEdge[1].y = pt[1].y;
			pSide->ptEdge[1].z = pt[1].z;
		}

		// extract elbow sidex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSideEx[0] = ExtractSideFace(pTmpList)))
			{
				pSideEx[0]->type = PIPE_BEND;
				pSideEx[0]->ele  = SIDEFACEX;
				pFaceList        = pHSRApp->AppendFace(pFaceList,pSideEx[0]);

				if(pSide)
				{
					PHSR_VERTEX ptr=NULL;
					for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
					{
						if((INTERSECTION == ptr->render) && pSideEx[0]->IsHiddenLine(ptr,ptr->next))
						{
							ptr->render = INVALID;
						}
					}

					for(ptr = pSideEx[0]->pHead;ptr && ptr->next;ptr = ptr->next)
					{
						if((INTERSECTION == ptr->render) && pSide->IsHiddenLine(ptr,ptr->next))
						{
							ptr->render = INVALID;
						}
					}
				}
			}
			pTmpList = pTmpList->next; // skip side face
		}
		// extract elbow topex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTopEx[0] = ExtractElbowEdgeFace(pTmpList,&pt[2],&radius[2])))
			{
				pTopEx[0]->type = PIPE_BEND;
				pTopEx[0]->ele  = TOPFACEX;
				//pFaceList       = pHSRApp->AppendFace(pFaceList,pTopEx[0]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}
		// extract elbow bottomex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottomEx[0] = ExtractElbowEdgeFace(pTmpList,&pt[3],&radius[3])))
			{
				pBottomEx[0]->type = PIPE_BEND;
				pBottomEx[0]->ele  = BOTTOMFACEX;
				//pFaceList        = pHSRApp->AppendFace(pFaceList,pBottomEx[0]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}

		// append side centerline
		if(pSideEx[0] && (NULL != (ptCenterline[0] = pHSRApp->NewVertex())) && (NULL != (ptCenterline[1] = pHSRApp->NewVertex())))
		{
			pSideEx[0]->ptEdge[0].x= pt[2].x;
			pSideEx[0]->ptEdge[0].y= pt[2].y;
			pSideEx[0]->ptEdge[0].z= pt[2].z;

			pSideEx[0]->ptEdge[1].x= pt[3].x;
			pSideEx[0]->ptEdge[1].y= pt[3].y;
			pSideEx[0]->ptEdge[1].z= pt[3].z;

			pSideEx[0]->radius     = radius[2];
			// save center line
			ptCenterline[0]->render= INTERSECTION;
			ptCenterline[0]->pt->x = pt[2].x;
			ptCenterline[0]->pt->y = pt[2].y;
			ptCenterline[0]->pt->z = pt[2].z;
			
			ptCenterline[1]->render= INTERSECTION;
			ptCenterline[1]->pt->x = pt[3].x;
			ptCenterline[1]->pt->y = pt[3].y;
			ptCenterline[1]->pt->z = pt[3].z;
			
			pSideEx[0]->pCenterline = AppendVertex(pSideEx[0]->pCenterline,ptCenterline[0]);
			pSideEx[0]->pCenterline = AppendVertex(pSideEx[0]->pCenterline,ptCenterline[1]);
		}

		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pSideEx[1] = ExtractSideFace(pTmpList)))
			{
				pSideEx[1]->type = PIPE_BEND;
				pSideEx[1]->ele  = SIDEFACEX;
				pFaceList        = pHSRApp->AppendFace(pFaceList,pSideEx[1]);

				if(pSide)
				{
					PHSR_VERTEX ptr=NULL;
					for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
					{
						if((INTERSECTION == ptr->render) && pSideEx[1]->IsHiddenLine(ptr,ptr->next))
						{
							ptr->render = INVALID;
						}
					}

					for(ptr = pSideEx[1]->pHead;ptr && ptr->next;ptr = ptr->next)
					{
						if((INTERSECTION == ptr->render) && pSide->IsHiddenLine(ptr,ptr->next))
						{
							ptr->render = INVALID;
						}
					}
				}
			}

			pTmpList = pTmpList->next; // skip side face
		}
		// extract elbow topex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pTopEx[1] = ExtractElbowEdgeFace(pTmpList,&pt[4],&radius[4])))
			{
				pTopEx[1]->type = PIPE_BEND;
				pTopEx[1]->ele  = TOPFACEX;
				//pFaceList     = pHSRApp->AppendFace(pFaceList,pTopEx[1]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}
		// extract elbow bottomex
		if(pTmpList && (df_list == pTmpList->val.type))
		{
			if(NULL != (pBottomEx[1] = ExtractElbowEdgeFace(pTmpList,&pt[5],&radius[5])))
			{
				pBottomEx[1]->type = PIPE_BEND;
				pBottomEx[1]->ele  = BOTTOMFACEX;
				//pFaceList        = pHSRApp->AppendFace(pFaceList,pBottomEx[1]);
			}

			pTmpList = pTmpList->next; // skip elbow topex list
		}

		// append side centerline
		if(pSideEx[1] && (NULL != (ptCenterline[0] = pHSRApp->NewVertex())) && (NULL != (ptCenterline[1] = pHSRApp->NewVertex())))
		{
			pSideEx[1]->ptEdge[0].x= pt[4].x;
			pSideEx[1]->ptEdge[0].y= pt[4].y;
			pSideEx[1]->ptEdge[0].z= pt[4].z;

			pSideEx[1]->ptEdge[1].x= pt[5].x;
			pSideEx[1]->ptEdge[1].y= pt[5].y;
			pSideEx[1]->ptEdge[1].z= pt[5].z;

			pSideEx[1]->radius     = radius[4];
			// save center line
			ptCenterline[0]->render= INTERSECTION;
			ptCenterline[0]->pt->x = pt[4].x;
			ptCenterline[0]->pt->y = pt[4].y;
			ptCenterline[0]->pt->z = pt[4].z;
			
			ptCenterline[1]->render= INTERSECTION;
			ptCenterline[1]->pt->x = pt[5].x;
			ptCenterline[1]->pt->y = pt[5].y;
			ptCenterline[1]->pt->z = pt[5].z;
			
			pSideEx[1]->pCenterline = AppendVertex(pSideEx[1]->pCenterline,ptCenterline[0]);
			pSideEx[1]->pCenterline = AppendVertex(pSideEx[1]->pCenterline,ptCenterline[1]);
		}

		double vtx=0,vty=0,vtz=0;
		double dot=0;
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			vtx = 0.;
			vty = 0.;
			vtz = 1.;
		}
		else
		{
			vtx = 1.;
			vty = 0.;
			vtz = 0.;
		}
		
		if(pTop)
		{
			dot = theGeo.DotProduct(pTop->Plane.a,pTop->Plane.b,pTop->Plane.c,vtx,vty,vtz);
			if((dot > (-1. + 0.01)) && (dot < (1. - 0.01)))
			{
				if(pSide)
				{
					pSide->mark[0]     = true;
					if(pSideEx[0]) pSideEx[0]->mark[0]= true;

					pSide->ptMark[0].x = pt[3].x;
					pSide->ptMark[0].y = pt[3].y;
					pSide->ptMark[0].z = pt[3].z;
				}
			}
			else
			{
				if(pSide) pSide->symbol[0]= true;
			}
		}
		else
		{
			if(pSide)
			{
				pSide->mark[0]     = true;
				pSide->symbol[0]   = false;
				if(pSideEx[0]) pSideEx[0]->mark[0]= true;

				pSide->ptMark[0].x = pt[3].x;
				pSide->ptMark[0].y = pt[3].y;
				pSide->ptMark[0].z = pt[3].z;
			}
		}

		if(pBottom)
		{
			dot = theGeo.DotProduct(pBottom->Plane.a,pBottom->Plane.b,pBottom->Plane.c,vtx,vty,vtz);
			if((dot > (-1. + 0.01)) && (dot < (1. - 0.01)))
			{
				if(pSide)
				{
					pSide->mark[1]     = true;
					if(pSideEx[1]) pSideEx[1]->mark[1]=true;

					pSide->ptMark[1].x = pt[4].x;
					pSide->ptMark[1].y = pt[4].y;
					pSide->ptMark[1].z = pt[4].z;
				}
			}
			else
			{
				if(pSide) pSide->symbol[1]= true;
			}
		}
		else
		{
			if(pSide)
			{
				pSide->mark[1]     = true;
				pSide->symbol[1]   = false;
				if(pSideEx[1]) pSideEx[1]->mark[1]= true;

				pSide->ptMark[1].x = pt[5].x;
				pSide->ptMark[1].y = pt[5].y;
				pSide->ptMark[1].z = pt[5].z;
			}
		}

		if(pSide) pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,PIPE_BEND,pTop,pSide,pBottom);
	}

	return pFaceList;
}


//	parameter	:
//	description	: extract tee from dlist
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractExtendTee(CHSRFace* pFaceList,DList* pList)
{
	assert(pList && "pList is NULL");
	assert((df_list == pList->val.type) && "pList's type is invalid");

	if(pList && (df_list == pList->val.type) && pList->val.data.list)
	{
		DList *pTmpList = pList->val.data.list,*pSubList = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		int     count = 0;
		double  radius[5]={0};
		POINT_T ptCenter[5][2]  ={0};
		static CHSRFace* pSide[5]   = {NULL};
		static CHSRFace* pTop[5]    = {NULL};
		static CHSRFace* pBottom[5] = {NULL};
		for(int i = 0;(i < 5) && (pTmpList && (df_list == pTmpList->val.type) && pTmpList->val.data.list);i++)
		{
			pSide[i]  = NULL;
			pTop[i]   = NULL;
			pBottom[i]= NULL;

			pSubList = pTmpList->val.data.list;
			if(df_integer == pSubList->val.type)
			{
				if(ExtractFaceInfo(&(ptCenter[i][0]),&(radius[i]),&count,&pSubList))
				{
					if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
					{
						pSide[i] = ExtractFace(count,pSubList);
						if(pSide[i])
						{
							pSide[i]->id  = m_nID++;
							pSide[i]->type= TEEX;
							pSide[i]->ele = HSR_RECTANGLE_SHAPE;
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			 // top face
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{
					if(ExtractFaceInfo(&(ptCenter[i][0]),&(radius[i]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pTop[i] = ExtractFace(count,pSubList);
							if(pTop[i])
							{
								pTop[i]->ptCenter.x = ptCenter[i][0].x;
								pTop[i]->ptCenter.y = ptCenter[i][0].y;
								pTop[i]->ptCenter.z = ptCenter[i][0].z;
								pTop[i]->radius     = radius[i];

								pTop[i]->id  = m_nID++;
								pTop[i]->type= TEEX;
								pTop[i]->ele = (pTop[i]->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			 // bottom face
			if(pTmpList && (df_list == pTmpList->val.type))
			{
				pSubList = pTmpList->val.data.list;
				if((df_point == pSubList->val.type) && (df_number == pSubList->next->val.type))
				{ // bottom face
					if(ExtractFaceInfo(&(ptCenter[i][1]),&(radius[i]),&count,&pSubList))
					{
						if(pSubList && (df_list == pSubList->val.type) && pSubList->val.data.list)
						{
							pBottom[i] = ExtractFace(count,pSubList);
							if(pBottom[i])
							{
								pBottom[i]->ptCenter.x = ptCenter[i][1].x;
								pBottom[i]->ptCenter.y = ptCenter[i][1].y;
								pBottom[i]->ptCenter.z = ptCenter[i][1].z;
								pBottom[i]->radius     = radius[i];

								pBottom[i]->id  = m_nID++;
								pBottom[i]->type= TEEX;
								pBottom[i]->ele = (pBottom[i]->IsPerpendicularToViewPoint()) ? SECTION : HSR_CIRCLE_SHAPE;
							}
						}
					}
				}
			}
			pTmpList = pTmpList->next;

			//--> follow up routine
			// set center line and invalidate edge line of side face
			if(pSide[i])
			{
				pSide[i]->radius = radius[i];//(radius[0] > radius[1]) ? radius[0] : radius[1];
				
				PHSR_VERTEX  pvt[2] = {0};
				PPOINT_T ppt[2] = {0};
				pvt[0] = pHSRApp->alloc_vertex();
				pvt[1] = pHSRApp->alloc_vertex();
				ppt[0] = pHSRApp->alloc_point();
				ppt[1] = pHSRApp->alloc_point();
				if(pvt[0] && pvt[1] && ppt[0] && ppt[1])
				{
					ppt[0]->x = ptCenter[i][0].x;
					ppt[0]->y = ptCenter[i][0].y;
					ppt[0]->z = ptCenter[i][0].z;
					
					ppt[1]->x = ptCenter[i][1].x;
					ppt[1]->y = ptCenter[i][1].y;
					ppt[1]->z = ptCenter[i][1].z;
					
					pSide[i]->pCenterline = ListPush(pSide[i]->pCenterline,pvt[0]);
					pSide[i]->pCenterline->pt = ppt[0];
					
					pSide[i]->pCenterline = ListPush(pSide[i]->pCenterline,pvt[1]);
					pSide[i]->pCenterline->next->pt = ppt[1];
				}
				else
				{
					if(pvt[0]) pHSRApp->free_vertex(pvt[0]);
					if(pvt[1]) pHSRApp->free_vertex(pvt[1]);
					if(ppt[0]) pHSRApp->free_point(ppt[0]);
					if(ppt[1]) pHSRApp->free_point(ppt[1]);
				}
			}
		}

		for(int i = 0;i < 5;i++)
		{
			if(pSide[i] && pSide[i]->pCenterline && pSide[i]->pCenterline->next)
			{
				pSide[i]->mark[0] = true;
				pSide[i]->mark[1] = true;
				for(int j=0;j < 5;j++)
				{
					if((j != i) && pSide[j] && pSide[j]->pCenterline && pSide[j]->pCenterline->next)
					{
						double dx=pSide[j]->pCenterline->pt->x - pSide[i]->pCenterline->pt->x;
						double dy=pSide[j]->pCenterline->pt->y - pSide[i]->pCenterline->pt->y;
						double dz=pSide[j]->pCenterline->pt->z - pSide[i]->pCenterline->pt->z;
						if((fabs(dx) < 0.00001) && (fabs(dy) < 0.00001) && (fabs(dz) < 0.00001)) pSide[i]->mark[0] = false;
						
						dx=pSide[j]->pCenterline->next->pt->x - pSide[i]->pCenterline->pt->x;
						dy=pSide[j]->pCenterline->next->pt->y - pSide[i]->pCenterline->pt->y;
						dz=pSide[j]->pCenterline->next->pt->z - pSide[i]->pCenterline->pt->z;
						if((fabs(dx) < 0.00001) && (fabs(dy) < 0.00001) && (fabs(dz) < 0.00001)) pSide[i]->mark[0] = false;

						dx=pSide[j]->pCenterline->pt->x - pSide[i]->pCenterline->next->pt->x;
						dy=pSide[j]->pCenterline->pt->y - pSide[i]->pCenterline->next->pt->y;
						dz=pSide[j]->pCenterline->pt->z - pSide[i]->pCenterline->next->pt->z;
						if((fabs(dx) < 0.00001) && (fabs(dy) < 0.00001) && (fabs(dz) < 0.00001)) pSide[i]->mark[1] = false;

						dx=pSide[j]->pCenterline->next->pt->x - pSide[i]->pCenterline->next->pt->x;
						dy=pSide[j]->pCenterline->next->pt->y - pSide[i]->pCenterline->next->pt->y;
						dz=pSide[j]->pCenterline->next->pt->z - pSide[i]->pCenterline->next->pt->z;
						if((fabs(dx) < 0.00001) && (fabs(dy) < 0.00001) && (fabs(dz) < 0.00001)) pSide[i]->mark[1] = false;
					}
				}
			}
		}

		for(int i = 0;i < 5;i++)
		{
			if(1 == i)
			{
				PIPELINEMODE mode=pHSRApp->GetEnv()->GetPipeLineMode();
				double nCriticalRadius = pHSRApp->GetEnv()->GetCriticalRadius();
				if((SINGLE_LINE == mode) || ((BOTH == mode) && radius[i] <= nCriticalRadius))
				{
					// 2'nd pipe's top face is intersection point of tee.
					if((pTop[i] && SECTION == HSR_ELEMENT_MARK(pTop[i]->ele)))
					{
						if(pBottom[i] && (SECTION == HSR_ELEMENT_MARK(pBottom[i]->ele)) && (pTop[i]->ptCenter.z >= pBottom[i]->ptCenter.z))
						{
							LINE_T line={0};
							line.ptStart = ptCenter[0][0];
							line.ptEnd   = ptCenter[0][1];
							PHSR_VERTEX pSymbol=CreateTeeSymbol(line,pTop[i]->radius);
							if(pSymbol)
							{
								for(PHSR_VERTEX ptr=pSymbol;ptr;ptr = ptr->next)
								{
									ptr->pt->x+= ptCenter[1][0].x;
									ptr->pt->y+= ptCenter[1][0].y;
									ptr->pt->z = ptCenter[1][0].z;
								}
								pHSRApp->free_vertex(pTop[i]->pHead);
								pTop[i]->ele   = SYMBOL;
								pTop[i]->pHead = pSymbol;
							}
						}
						else
						{
							if(pTop[i])    pTop[i]->ele    = SYMBOL;
							if(pBottom[i]) pBottom[i]->ele = SYMBOL;
						}
					}
					else
					{
						if(pTop[i])    pTop[i]->ele    = SYMBOL;
						if(pBottom[i]) pBottom[i]->ele = SYMBOL;
					}
				}
			}
			else if(i > 1)
			{
				PIPELINEMODE mode=pHSRApp->GetEnv()->GetPipeLineMode();
				double nCriticalRadius = pHSRApp->GetEnv()->GetCriticalRadius();
				if((SINGLE_LINE == mode) || ((BOTH == mode) && radius[0] <= nCriticalRadius))
				{
					if(pSide[i])
					{
						pSide[i]->radius  = radius[0];
					}

					if(pTop[i] && (SECTION == HSR_ELEMENT_MARK(pTop[i]->ele)))
					{
						pTop[i]->radius = radius[0];
						pTop[i]->ele    = SYMBOL;
					}
					if(pBottom[i] && (SECTION == HSR_ELEMENT_MARK(pBottom[i]->ele)))
					{
						pBottom[i]->radius = radius[0];
						pBottom[i]->ele    = SYMBOL;
					}
				}
			}
			pFaceList = pHSRApp->m_clsPreProcessor.Run(pFaceList,TEEX,pTop[i],pSide[i],pBottom[i]);
		}
	}

	return pFaceList;
}

//	parameter	: nCount - storage would filled with number of verex,
//			: box - whether calculate plane's volume or not.
//	description	: extract vertex from dlist and update model volume.
//	remarks		:
//	returns		: return vertex list.
PHSR_VERTEX CHSRImport::ExtractVertexListFromDList(CHSRFace* pFace,DList* pVertexList,int *nCount,PPOINT_T pt,bool box)
{
	assert(pFace       && "face is invalid");
	assert(pVertexList && "dlist is invaid");
	assert(pt          && "point is invalid");
	PHSR_VERTEX ret = NULL;
	static PPOINT_T pPoint[512]={0};
	static PHSR_VERTEX  pVertex[512]={0};

	if((pFace && pt) && (pVertexList && pVertexList->next))
	{
		DList *pSubList = pVertexList;
		PHSR_VERTEX pVt = NULL;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		memset(pPoint,0,sizeof(PPOINT_T)*512);
		memset(pVertex,0,sizeof(PHSR_VERTEX)*512);
		(*nCount) = 0;
		
		assert((df_point == pVertexList->val.type) && (df_point == pVertexList->next->val.type));
		double dx[2]={0.},dy[2]={0.},dz[2]={0.},l[2]={0.};
		if((df_point == pVertexList->val.type) && (df_point == pVertexList->next->val.type))
		{
			if(pVertexList->val.data.point->x < 100)
			{
				int i=0;
			}
			dx[0] = pVertexList->next->val.data.point->x - pVertexList->val.data.point->x;
			dy[0] = pVertexList->next->val.data.point->y - pVertexList->val.data.point->y;
			dz[0] = pVertexList->next->val.data.point->z - pVertexList->val.data.point->z;
			l[0]  = sqrt(dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]);
			
			pPoint[(*nCount)++] = pVertexList->val.data.point;
			//--> update model volume
			if(	(pHSRApp->m_volModel.x[LOW] == pHSRApp->m_volModel.x[HIGH]) && 
				(pHSRApp->m_volModel.y[LOW] == pHSRApp->m_volModel.y[HIGH]) && 
				(pHSRApp->m_volModel.z[LOW] == pHSRApp->m_volModel.z[HIGH]))
			{
				pHSRApp->m_volModel.x[LOW] = pHSRApp->m_volModel.x[HIGH] = pVertexList->val.data.point->x;
				pHSRApp->m_volModel.y[LOW] = pHSRApp->m_volModel.y[HIGH] = pVertexList->val.data.point->y;
				pHSRApp->m_volModel.z[LOW] = pHSRApp->m_volModel.z[HIGH] = pVertexList->val.data.point->z;
			}
			else
			{
				if(pHSRApp->m_volModel.x[LOW] > pVertexList->val.data.point->x) pHSRApp->m_volModel.x[LOW] = pVertexList->val.data.point->x;
				if(pHSRApp->m_volModel.x[HIGH]< pVertexList->val.data.point->x) pHSRApp->m_volModel.x[HIGH]= pVertexList->val.data.point->x;
				if(pHSRApp->m_volModel.y[LOW] > pVertexList->val.data.point->y) pHSRApp->m_volModel.y[LOW] = pVertexList->val.data.point->y;
				if(pHSRApp->m_volModel.y[HIGH]< pVertexList->val.data.point->y) pHSRApp->m_volModel.y[HIGH]= pVertexList->val.data.point->y;
				if(pHSRApp->m_volModel.z[LOW] > pVertexList->val.data.point->z) pHSRApp->m_volModel.z[LOW] = pVertexList->val.data.point->z;
				if(pHSRApp->m_volModel.z[HIGH]< pVertexList->val.data.point->z) pHSRApp->m_volModel.z[HIGH]= pVertexList->val.data.point->z;
			}
			//<--

			if(box)
			{	// calculate face's volume
				pFace->volume.x[LOW] = pVertexList->val.data.point->x;
				pFace->volume.y[LOW] = pVertexList->val.data.point->y;
				pFace->volume.z[LOW] = pVertexList->val.data.point->z;
				pFace->volume.x[HIGH]= pVertexList->val.data.point->x;
				pFace->volume.y[HIGH]= pVertexList->val.data.point->y;
				pFace->volume.z[HIGH]= pVertexList->val.data.point->z;
			}
		}else	return NULL;
		
		for(pSubList = pSubList->next;pSubList && pSubList->next;pSubList = pSubList->next)
		{
			if((df_point == pSubList->val.type) && (df_point == pSubList->next->val.type))
			{
				dx[1] = pSubList->next->val.data.point->x - pSubList->val.data.point->x;
				dy[1] = pSubList->next->val.data.point->y - pSubList->val.data.point->y;
				dz[1] = pSubList->next->val.data.point->z - pSubList->val.data.point->z;
				l[1]  = sqrt(dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]);
				
				//--> update model volulme
				if(pHSRApp->m_volModel.x[LOW] > pSubList->val.data.point->x) pHSRApp->m_volModel.x[LOW] = pSubList->val.data.point->x;
				if(pHSRApp->m_volModel.x[HIGH]< pSubList->val.data.point->x) pHSRApp->m_volModel.x[HIGH]= pSubList->val.data.point->x;
				if(pHSRApp->m_volModel.y[LOW] > pSubList->val.data.point->y) pHSRApp->m_volModel.y[LOW] = pSubList->val.data.point->y;
				if(pHSRApp->m_volModel.y[HIGH]< pSubList->val.data.point->y) pHSRApp->m_volModel.y[HIGH]= pSubList->val.data.point->y;
				if(pHSRApp->m_volModel.z[LOW] > pSubList->val.data.point->z) pHSRApp->m_volModel.z[LOW] = pSubList->val.data.point->z;
				if(pHSRApp->m_volModel.z[HIGH]< pSubList->val.data.point->z) pHSRApp->m_volModel.z[HIGH]= pSubList->val.data.point->z;
				//<--
				// check whether two points are on a line
				if(l[0] && l[1])
				{
					dx[0] /= l[0];
					dy[0] /= l[0];
					dz[0] /= l[0];
					dx[1] /= l[1];
					dy[1] /= l[1];
					dz[1] /= l[1];
					l[0]   = 1.;

					if(!(fabs(dx[1] - dx[0]) <= 0.00001) || !(fabs(dy[1] - dy[0]) < 0.00001) || !(fabs(dz[1] - dz[0]) < 0.00001))
					{
						pPoint[(*nCount)++] = pSubList->val.data.point;
						dx[0] = dx[1];
						dy[0] = dy[1];
						dz[0] = dz[1];
					}
				}
				else
				{
					dx[0] = dx[1];
					dy[0] = dy[1];
					dz[0] = dz[1];
					l[0]  = l[1];
				}
				/*if(dx[0]*dy[1] != dx[1]*dy[0]){
					pPoint[(*nCount)++] = pSubList->val.data.point;
					dx[0] = dx[1];
					dy[0] = dy[1];
					dz[0] = dz[1];
				}
				*/
			}
		}
		//--> update model volume
		if(pHSRApp->m_volModel.x[LOW] > pSubList->val.data.point->x) pHSRApp->m_volModel.x[LOW] = pSubList->val.data.point->x;
		if(pHSRApp->m_volModel.x[HIGH]< pSubList->val.data.point->x) pHSRApp->m_volModel.x[HIGH]= pSubList->val.data.point->x;
		if(pHSRApp->m_volModel.y[LOW] > pSubList->val.data.point->y) pHSRApp->m_volModel.y[LOW] = pSubList->val.data.point->y;
		if(pHSRApp->m_volModel.y[HIGH]< pSubList->val.data.point->y) pHSRApp->m_volModel.y[HIGH]= pSubList->val.data.point->y;
		if(pHSRApp->m_volModel.z[LOW] > pSubList->val.data.point->z) pHSRApp->m_volModel.z[LOW] = pSubList->val.data.point->z;
		if(pHSRApp->m_volModel.z[HIGH]< pSubList->val.data.point->z) pHSRApp->m_volModel.z[HIGH]= pSubList->val.data.point->z;
		//<--
		
		pPoint[(*nCount)++] = pSubList->val.data.point;
		pt->x       = pSubList->val.data.point->x;
		pt->y       = pSubList->val.data.point->y;
		pt->z       = pSubList->val.data.point->z;
		
		PHSR_VERTEX pvtPrev = NULL;
		for(int i = 0;i < (*nCount);i++)
		{
			if(NULL != (pVt = pHSRApp->alloc_vertex()))
			{
				pVertex[i]  = pVt;
				pVt->render = INTERSECTION;
				pVt->pt     = pPoint[i];
				if(pvtPrev) pvtPrev->next = pVt;
				pvtPrev = pVt;
			}
			if(box)
			{
				if(pFace->volume.x[LOW] > pVertex[i]->pt->x) pFace->volume.x[LOW] = pVertex[i]->pt->x;
				if(pFace->volume.y[LOW] > pVertex[i]->pt->y) pFace->volume.y[LOW] = pVertex[i]->pt->y;
				if(pFace->volume.z[LOW] > pVertex[i]->pt->z) pFace->volume.z[LOW] = pVertex[i]->pt->z;
				if(pFace->volume.x[HIGH]< pVertex[i]->pt->x) pFace->volume.x[HIGH]= pVertex[i]->pt->x;
				if(pFace->volume.y[HIGH]< pVertex[i]->pt->y) pFace->volume.y[HIGH]= pVertex[i]->pt->y;
				if(pFace->volume.z[HIGH]< pVertex[i]->pt->z) pFace->volume.z[HIGH]= pVertex[i]->pt->z;
			}
		}
		ret = pVertex[0];
	}

	return ret;
}

//	parameter	: nCount - number of points in face list.
//	description	: 1. check data type and contents.(content exits or not)
//			: 2. extract vertexs from DList
//			: 3. calculate plane equation.
//	remarks		: 4. if plane is parallel to view point free the plane.
//			: [BUG FIX] 
//			: 2002.07.12 - if vertical plane is whole contained in model volume, does'nt add face list
//	returns		: return a face extracted from dlist.
CHSRFace* CHSRImport::ExtractFace(const int nCount,DList* pFaceList)
{
	assert(pFaceList && "pFaceList is invalid");
	assert(df_list == pFaceList->val.type && "pFaceList->val.type is invalid");
//	assert(pFaceList->val.data.list && "dlist value is invalid");
	CHSRFace* pFace = NULL;

	DList* pTmpList = pFaceList;
	if(pTmpList && (df_list == pTmpList->val.type) && (pTmpList->val.data.list))
	{
		int count=0;
		POINT_T pt={0};
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		DList *pSubList = pTmpList->val.data.list;;
		if(pSubList && (df_point == pSubList->val.type) && (NULL != (pFace = pHSRApp->new_hsr_face())))
		{
			pFace->pHead = ExtractVertexListFromDList(pFace,pSubList,&count,&pt);
			pFace->nCount= count;
			if(pFace->pHead && (count >= 3))
			{	// plane has more than 3 points.
				if(     (pFace->pHead->pt->x != pt.x) || 
					(pFace->pHead->pt->y != pt.y) ||
					(pFace->pHead->pt->z != pt.z))
				{

					PHSR_VERTEX pEnd = NULL;
					if((pEnd = pHSRApp->alloc_vertex()) != NULL)
					{
						pEnd->render= INTERSECTION;
						pEnd->pt    = pFace->pHead->pt;			
						pFace->pHead = ListPush(pFace->pHead,pEnd);
						(pFace->nCount)++;
					}
					else
					{
						pHSRApp->FreeFace(pFace);
						
						return NULL;
					}
				}
				if(pFace->CalcPlaneEquation())
				{
					if(pHSRApp->IsParallelPlane(pFace) && 
						(pFace->volume.x[LOW] > pHSRApp->m_volModel.x[LOW]) && (pFace->volume.x[HIGH] < pHSRApp->m_volModel.x[HIGH]) && 
						(pFace->volume.y[LOW] > pHSRApp->m_volModel.y[LOW]) && (pFace->volume.y[HIGH] < pHSRApp->m_volModel.y[HIGH]) &&
						(pFace->volume.z[LOW] > pHSRApp->m_volModel.z[LOW]) && (pFace->volume.z[HIGH] < pHSRApp->m_volModel.z[HIGH]))
					{
						pHSRApp->FreeFace(pFace);
						pFace = NULL;
						//
					}
					//if(pHSRApp->IsParallelPlane(pFace)){
						/* //for test 2002.07.04
						pHSRApp->FreeFace(pFace);
						pFace = NULL;
						*/
					//}
				}
				else
				{
					pHSRApp->FreeFace(pFace);
					pFace = NULL;
				}
			}
			else
			{
				pHSRApp->FreeFace(pFace);
				pFace = NULL;
			}
		}
	}

	return pFace;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractHFace(const int nCount,DList* pFaceList)
{
	assert(pFaceList && "pFaceList is invalid");
	assert(df_list == pFaceList->val.type && "pFaceList->val.type is invalid");
	CHSRFace* pFace = NULL;

	if(pFaceList && (df_list == pFaceList->val.type) && (pFaceList->val.data.list))
	{
		int count=0;
		POINT_T pt={0};
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		DList *pSubList = pFaceList->val.data.list;;
		if(pSubList && (df_point == pSubList->val.type) && (NULL != (pFace = pHSRApp->new_hsr_face())))
		{
			static PPOINT_T pPoint[512] ={0};
			static PHSR_VERTEX  pVertex[512]={0};
			static bool     visible[512]={0};
			
			int index[2]={0};
			for(DList* pSubList=pFaceList->val.data.list;pSubList;pSubList = pSubList->next)
			{
				if(df_point == pSubList->val.type)
				{
					pPoint[index[0]] = pSubList->val.data.point;
					index[0]++;
				}
				else if(df_boolean == pSubList->val.type)
				{
					visible[index[1]] = (pSubList->val.data.boolean) ? true : false;;
					index[1]++;
				}
			}

			for(int i = 0;i < index[0];i++)
			{
				PHSR_VERTEX pvt=NULL;
				if(NULL != (pvt = pHSRApp->alloc_vertex()))
				{
					if(visible[i])  pvt->render = INTERSECTION;
					else		pvt->render = INVALID;
					pvt->pt = pPoint[i];
					pFace->pHead = ListPush(pFace->pHead,pvt);

					if(pFace->volume.x[LOW] > pvt->pt->x) pFace->volume.x[LOW] = pvt->pt->x;
					if(pFace->volume.y[LOW] > pvt->pt->y) pFace->volume.y[LOW] = pvt->pt->y;
					if(pFace->volume.z[LOW] > pvt->pt->z) pFace->volume.z[LOW] = pvt->pt->z;
					if(pFace->volume.x[HIGH]< pvt->pt->x) pFace->volume.x[HIGH]= pvt->pt->x;
					if(pFace->volume.y[HIGH]< pvt->pt->y) pFace->volume.y[HIGH]= pvt->pt->y;
					if(pFace->volume.z[HIGH]< pvt->pt->z) pFace->volume.z[HIGH]= pvt->pt->z;
				}
			}

			if(pFace->pHead && (index[0] >= 3))
			{	// plane has more than 3 points.
				if(     (pFace->pHead->pt->x != pPoint[index[0]-1]->x) || 
					(pFace->pHead->pt->y != pPoint[index[0]-1]->y) ||
					(pFace->pHead->pt->z != pPoint[index[0]-1]->z))
				{
					
					PHSR_VERTEX pEnd = NULL;
					if((pEnd = pHSRApp->alloc_vertex()) != NULL)
					{
						pEnd->render= INTERSECTION;
						pEnd->pt    = pFace->pHead->pt;			
						pFace->pHead = ListPush(pFace->pHead,pEnd);
						index[0]++;
					}
					else
					{
						pHSRApp->FreeFace(pFace);
						
						return NULL;
					}
				}
				pFace->nCount = index[0];
				if(pFace->CalcPlaneEquation())
				{
					if(pHSRApp->IsParallelPlane(pFace))
					{
						pHSRApp->FreeFace(pFace);
						pFace = NULL;
					}
				}
				else
				{
					pHSRApp->FreeFace(pFace);
					pFace = NULL;
				}
			}
			else
			{
				pHSRApp->FreeFace(pFace);
				pFace = NULL;
			}
		}
	}

	return pFace;
}

//	parameter	:
//	description	: extract face from dlist, and append to pFaceList.
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractFace(CHSRFace* pFaceList, DList *pDList)
{
	assert(pDList && "pDList is NULL");
	assert((df_list == pDList->val.type) && "pList->val.type is invalid");
	
	if(pDList && (df_list == pDList->val.type))
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		DList* pSubDList = pDList->val.data.list;
		
		if(pSubDList && (df_integer == pSubDList->val.type))
		{
			int nCount = pSubDList->val.data.integer;
			pSubDList  = pSubDList->next;
			
			if(pSubDList)
			{
				CHSRFace* pFace=ExtractFace(0,pSubDList);
				if(pFace)
				{
					pFace->id  = m_nID++;
					pFace->ele = NORMAL;
					pFaceList  = pHSRApp->AppendFace(pFaceList,pFace);
				}
			}
		}
	}
	
	return pFaceList;
}

//	parameter	:
//	description	: extract hole face from dlist, and append to pFaceList.
//	remarks		:
//	returns		:
CHSRFace* CHSRImport::ExtractHFace(CHSRFace* pFaceList, DList *pDList)
{
	assert(pDList && "pDList is NULL");
	assert((df_list == pDList->val.type) && "pList->val.type is invalid");
	
	if(pDList && (df_list == pDList->val.type))
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		DList* pSubDList = pDList->val.data.list;
		
		if(pSubDList && (df_integer == pSubDList->val.type))
		{
			int nCount = pSubDList->val.data.integer;
			pSubDList  = pSubDList->next;
			
			if(pSubDList)
			{
				CHSRFace* pFace=ExtractHFace(0,pSubDList);
				if(pFace)
				{
					pFace->id  = m_nID++;
					pFace->ele = NORMAL;
					pFaceList  = pHSRApp->AppendFace(pFaceList,pFace);
				}
			}
		}
	}
	
	return pFaceList;
}

//	parameter	:
//	description	: append pValue to end of pVertex by lined list.
//	remarks		: pValue must be memory allocated.
//	returns		:
PHSR_VERTEX CHSRImport::AppendVertex(PHSR_VERTEX pVertex,PHSR_VERTEX pValue)
{
	if(pVertex && pValue){
		PHSR_VERTEX pVertexNode = pVertex;
		for(pVertexNode = pVertex;pVertexNode && pVertexNode->next;pVertexNode = pVertexNode->next);
		if(pVertexNode)
		{
			pVertexNode->next= pValue;
			pValue->prev     = pVertexNode;
		}

		return pVertex;
	}
	else if((pVertex == NULL) && pValue)
	{
		return pValue;
	}

	return NULL;
}

//	parameter	:
//	description	: extract face info(center point,radius,number of vertics)
//	remarks		:
//	returns		:
bool CHSRImport::ExtractFaceInfo(PPOINT_T pptCenter, double *pRadius, int *pCount, DList **pList)
{
	assert(pptCenter && "pptCenter is NULL");
	assert(pRadius   && "pRadius is NULL");
	assert(pCount    && "pCount is NULL");
	assert((*pList)  && "(*pList) is NULL");
	bool ret = false;

	if(pptCenter && pRadius && pCount && (*pList))
	{
		if(df_integer == (*pList)->val.type)
		{
			(*pCount)= (*pList)->val.data.integer; // number of vertics
 			(*pList) = (*pList)->next;
			
			ret      = true;
		}
		else if(df_point == (*pList)->val.type)
		{
			pptCenter->x = (*pList)->val.data.point->x; // center point
			pptCenter->y = (*pList)->val.data.point->y;
			pptCenter->z = (*pList)->val.data.point->z;
			(*pList)     = (*pList)->next;

			assert(df_number == (*pList)->val.type);
			(*pRadius)= (*pList)->val.data.number; // radius
			(*pList)  = (*pList)->next;

			assert(df_integer == (*pList)->val.type);
			(*pCount)= (*pList)->val.data.integer; // number of vertics
			(*pList) = (*pList)->next;

			ret      = true;
		}
	}

	return ret;
}

//	parameter	:
//	description	: create symbol of tee which is vertical.
//	remarks		: (|)	 [BUG FIX]
//			: 2002.05.23 - check routine which tee input distance is larger than radius.
//			:	     - if tee input distance is larger than radius, no create tee symbol
//			:            - change 'nDist*0.5/(2*radius)' -> 'nDist*0.5/radius'
//	returns		:
PHSR_VERTEX CHSRImport::CreateTeeSymbol(const LINE_T &line, const double nRadius)
{
	assert((nRadius > 0) && "nRadius is less than 0");
	static POINT_T ptCurve[24] ={0};
	static bool    bVisible[24]={0};
	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	double nDist =pHSRApp->GetEnv()->GetTeeDistA();

	assert((nDist > 0) && "nDist is less than 0");
	PHSR_VERTEX pVertices = NULL;
	if((nRadius > 0) && (nDist > 0))
	{
		POINT_T pt={0};

		pt.x = line.ptEnd.x - line.ptStart.x;
		pt.y = line.ptEnd.y - line.ptStart.y;
		double alpha=CGeometry::GetRotatedAngleInXYPlane(pt);
		double c=cos(alpha),s=sin(alpha);

		double sval=(nDist*0.5)/nRadius;
		if(sval <= 1.)
		{
			int nIndex=0;
			double nTheta=asin(sval);
			double nStep=PI/8.;
			for(double nRad=0.;nRad < nTheta;nRad += nStep,nIndex++)
			{
				ptCurve[nIndex].x = nRadius*cos(nRad);
				ptCurve[nIndex].y = nRadius*sin(nRad);

				bVisible[nIndex] = false;

				double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
				double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
				ptCurve[nIndex].x = x;
				ptCurve[nIndex].y = y;
			}
			ptCurve[nIndex].x = nRadius*cos(nTheta);
			ptCurve[nIndex].y = nRadius*sin(nTheta);
			bVisible[nIndex] = false;
			double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
			double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
			ptCurve[nIndex].x = x;
			ptCurve[nIndex].y = y;
			nIndex++;
			for(double nRad = nTheta;nRad < PI-nTheta;nRad+=nStep,nIndex++)
			{
				ptCurve[nIndex].x = nRadius*cos(nRad);
				ptCurve[nIndex].y = nRadius*sin(nRad);

				bVisible[nIndex] = true;

				double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
				double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
				ptCurve[nIndex].x = x;
				ptCurve[nIndex].y = y;
			}
			for(double nRad = PI-nTheta;nRad < PI+nTheta;nRad+=nStep,nIndex++)
			{
				ptCurve[nIndex].x = nRadius*cos(nRad);
				ptCurve[nIndex].y = nRadius*sin(nRad);

				bVisible[nIndex] = false;

				double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
				double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
				ptCurve[nIndex].x = x;
				ptCurve[nIndex].y = y;
			}
			ptCurve[nIndex].x = nRadius*cos(PI+nTheta);
			ptCurve[nIndex].y = nRadius*sin(PI+nTheta);
			bVisible[nIndex] = false;
			x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
			y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
			ptCurve[nIndex].x = x;
			ptCurve[nIndex].y = y;
			nIndex++;
			for(double nRad = PI+nTheta;nRad < 2*PI-nTheta;nRad+=nStep,nIndex++)
			{
				ptCurve[nIndex].x = nRadius*cos(nRad);
				ptCurve[nIndex].y = nRadius*sin(nRad);

				bVisible[nIndex] = true;

				double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
				double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
				ptCurve[nIndex].x = x;
				ptCurve[nIndex].y = y;
			}
			ptCurve[nIndex].x = nRadius*cos(2*PI-nTheta);
			ptCurve[nIndex].y = nRadius*sin(2*PI-nTheta);
			bVisible[nIndex]  = true;
			x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
			y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
			ptCurve[nIndex].x = x;
			ptCurve[nIndex].y = y;
			nIndex++;
			for(double nRad = 2*PI-nTheta;nRad < 2*PI;nRad+=nStep,nIndex++)
			{
				ptCurve[nIndex].x = nRadius*cos(nRad);
				ptCurve[nIndex].y = nRadius*sin(nRad);

				bVisible[nIndex] = false;

				double x=ptCurve[nIndex].x*c - ptCurve[nIndex].y*s;
				double y=ptCurve[nIndex].x*s + ptCurve[nIndex].y*c;
				ptCurve[nIndex].x = x;
				ptCurve[nIndex].y = y;
			}
			bVisible[nIndex]  = false;
			ptCurve[nIndex].x = ptCurve[0].x;
			ptCurve[nIndex].y = ptCurve[0].y;
			nIndex++;

			CHSRApp* pHSRApp = CHSRApp::GetInstance();
			for(int i = 0;i < nIndex;i++)
			{
				PHSR_VERTEX  pvt=pHSRApp->alloc_vertex();
				if(pvt)
				{
					pvt->render = bVisible[i] ? SHOW : HIDE;
					PPOINT_T ppt=pHSRApp->alloc_point();
					if(ppt)
					{
						ppt->x = ptCurve[i].x;
						ppt->y = ptCurve[i].y;
						ppt->z = 0.;
						pvt->pt= ppt;

						pVertices = ListPush(pVertices,pvt);
					}else	pHSRApp->free_vertex(pvt);
				}
			}
		}
	}
	
	return pVertices;
}
