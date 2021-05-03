// KExport.cpp: implementation of the KExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <SQLib.h>
#include <utility\iolib.h>
#include <graphics\geometry.h>

#include "Geo.h"
#include "SmartHSR.h"
#include "HSRView.h"
#include "HSRExport.h"

/// tolerance for removing too short line
const double CHSRExport::_TOLER = 0.0000001;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	
	@brief	형상의 모든 부분이 보여지는지 확인한다.
	@param	pHead	a parameter of type PVERTEX
	@return	bool	
*/
namespace
{
	bool IsShapeAllShow(PHSR_VERTEX pHead)
	{
		assert(pHead && "pHead is NULL");

		if(pHead)
		{
			PHSR_VERTEX pvt = NULL;
			for(pvt = pHead; pvt ; pvt = pvt->next)
			{
				if((INTERSECTION != pvt->render) && (SHOW != pvt->render)) break;
			}
			
			return (NULL == pvt);
		}

		return false;
	}
};

/**	
	@brief
*/
CHSRExport::CHSRExport()
{
	memset(m_pShowPrimitiveKind,0,sizeof(PSQPRIMITIVE)*24);
	memset(m_pHiddenPrimitiveKind,0,sizeof(PSQPRIMITIVE)*24);

	m_sVersion = _T("V1");
}

CHSRExport::~CHSRExport()
{
}

/**	
	@brief	The CHSRExport::IsSingleLine function

	@param	nRadius	a parameter of type const double

	@return	bool	
*/
bool CHSRExport::IsSingleLine(const double nRadius)
{
	bool bRet=false;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();
		
		if(	(SINGLE_LINE == LineMode) || 
			((BOTH == LineMode) && (nRadius <= pHSRApp->m_clsOption.GetCriticalRadius())))
			bRet = true;
	}

	return bRet;
}

#ifdef VER_03
/**	\brief	The CHSRExport::WriteFaceListToPrimitiveList function\n
	write pFaceList to primitive list.

	\param	pFaceList	a parameter of type PFACE

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteFaceListToPrimitiveList(CHSRFace* pFaceList)
{
	assert(pFaceList && "pFaceList is NULL");
	PSQPRIMITIVE ret = NULL;

	if(pFaceList)
	{
		for(CHSRFace* ptr = pFaceList;ptr;ptr = ptr->next)
		{
			if(LAYER_ELBOW == ptr->m_enmLayer)
			{
				if(ELBOW == ptr->type)
					ret = WriteElbowToDWG(ret,ptr);
				else if(SW_ELBOW == ptr->type)
					ret = WriteSWElbowToDWG(ret,ptr);
			}
			else if(LAYER_PIPE_OTHER == ptr->m_enmLayer)
			{
				if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_PIPE_OTHER);
			}
			else if(LAYER_EQUI == ptr->m_enmLayer)
			{
				if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_EQUI);
			}
			else if(LAYER_STRU == ptr->m_enmLayer)
			{
				if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_STRU);
			}
			else if(LAYER_CABLE == ptr->m_enmLayer)
			{
				if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_CABLE);
			}
			else if(LAYER_OTHER == ptr->m_enmLayer)
			{
				if(ptr) ret = WriteCylinderToDWG(ret,ptr,LAYER_OTHER);
			}
			else
			{
				switch(ptr->type)
				{
					case PIPE: case PIPEX: case PIPE_BEND:
						ret = WritePipeToDWG(ret,ptr);
					break;
					case ELBOW:
						ret = WriteElbowToDWG(ret,ptr);
					break;
					case TEE:
						ret = WriteTeeToDWG(ret,ptr);
					break;
					case TEEX:
						ret = WriteTeeXToDWG(ret,ptr);
					break;
					case SW_ELBOW:
						ret = WriteSWElbowToDWG(ret,ptr);
					break;
					case BOX:
						if(ptr) ret = WriteBoxToDWG(ret,ptr,ptr->m_enmLayer);
					break;
					case FLANGE:
						if(ptr) ret = WriteFaceToPrimitive(ret , ptr , LAYER_OTHER);
					break;
					case DOME:
						if(ptr)	ret = WriteDomeToDWG(ret,ptr,ptr->m_enmLayer);
					break;
					case EQP:
						if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_EQUI);
					break;
					case STRU:
						if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_STRU);
					break;
					case OTHER:
						if(ptr) ret = WriteFaceToPrimitive(ret,ptr,LAYER_OTHER);
					break;
					default:
						if((NULL != ptr->body) && ptr->body->IsKindOf(CLineElement::TypeString()))
						{
							if(ptr->pCenterline)
							{
								WriteVertexListToDWG(ptr , ptr->pCenterline , ptr->m_enmLayer);
							}
						}else if(ptr->pHead)
						{
							if(ptr) ret = WriteFaceToPrimitive(ret,ptr,ptr->m_enmLayer);
							//ptr->pHead = FindStartVertex(ptr->pHead);
							//WriteVertexListToDWG(ptr->pHead,LAYER_OTHER,SOLID_LINE,24);
						}
					break;
				}
			}
		}
	}

	return ret;
}
#else
/**	
	@brief	write pFaceList to primitive list.
	@author	humkyung
	@param	pFaceList	a parameter of type PFACE
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteFaceListToPrimitiveList(CHSRFace* pFaceList)
{
	assert(pFaceList && "pFaceList is NULL");
	PSQPRIMITIVE ret = NULL;
	
	if(pFaceList)
	{
		for(CHSRFace* ptr = pFaceList;ptr;ptr = ptr->next)
		{
			switch(ptr->type)
			{
				case PIPE:case PIPEX:case PIPE_BEND:
					ret = WritePipeToDWG(ret,ptr);
					break;
				case ELBOW:
					ret = WriteElbowToDWG(ret,ptr);
					break;
				case TEE:
					ret = WriteTeeToDWG(ret,ptr);
					break;
				case TEEX:
					ret = WriteTeeXToDWG(ret,ptr);
					break;
				case SW_ELBOW:
					ret = WriteSWElbowToDWG(ret,ptr);
					break;
				case BOX:
					if(ptr) ret = WriteBoxToDWG(ret,ptr,ptr->m_enmLayer);
					break;
				case FLANGE:
					if(ptr) ret = WriteFaceToPrimitive(ret , ptr , LAYER_OTHER);
					break;
				case DOME:
					if(ptr) ret = WriteDomeToDWG(ret,ptr,ptr->m_enmLayer);
					break;
				case MITER:
					if(ptr) ret = WriteMiterToDWG(ret,ptr);
					break;
				default:
					if(ptr->pHead) ret = WriteFaceToPrimitive(ret,ptr,ptr->m_enmLayer);
					break;
			}
		}
	}

	return ret;
}
#endif

/**	
	@brief	The CHSRExport::WritePipeToDWG function
	@author	humkyung

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	
	@remarks\n
	;2002.03.18 - if center line's segment is show,always the segment.\n
	;2002.05.20 - if line mode is SINGLE_LINE, write only pipe center-line.

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WritePipeToDWG(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	CHSRApp* pHSRApp= CHSRApp::GetInstance();
	PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();

	if(pFace)
	{
		/// single line
		if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (pFace->radius <= pHSRApp->m_clsOption.GetCriticalRadius())))
		{
			if(pFace->pCenterline)
			{
				WriteVertexListToDWG(pFace , pFace->pCenterline,LAYER_PIPE_SIN);
			}

			if(HSR_CIRCLE_SHAPE == pFace->ele)
			{
				WriteVertexListToDWG(pFace , pFace->pHead , LAYER_PIPE_SIN);
			}
		}
		else
		{	// double line mode or radius is larger than critical radius.
			long nVisibleCnt = 0L,nTotalCnt = 0L;
			double nTotalLen=0.,nVisibleLen=0.;
			if(pFace->pHead)
			{
				PSQPRIMITIVE ret = NULL;
				WriteFaceToPrimitive(ret , pFace , LAYER_PIPE_DOU);
							
				if(pHSRApp->m_clsOption.IsPipeCenterlineOn())
				{
					if(!pHSRApp->m_clsOption.IsHiddenlineOn(LAYER_PIPE_DOU))
					{
						for(PHSR_VERTEX ptr = pFace->pHead,TotalCnt = 0;ptr && ptr->next;ptr = ptr->next,nTotalCnt++)
						{
							//double dx=ptr->next->pt->x - ptr->pt->x;
							//double dy=ptr->next->pt->y - ptr->pt->y;
							//nTotalLen += sqrt(dx*dx + dy*dy);
							if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
							{
								//nVisibleLen += sqrt(dx*dx + dy*dy);
								nVisibleCnt++;
							}
						}
					}
					else
					{
						nVisibleCnt = nTotalCnt = 1;
					}
				}
			}
			
			if(pFace->pCenterline && pHSRApp->m_clsOption.IsPipeCenterlineOn())
			{
				if(0L != nVisibleCnt)
				{
					for(PHSR_VERTEX ptr = pFace->pCenterline;ptr && ptr->next;ptr = ptr->next)
					{
						if(INVALID != ptr->render) ptr->render = INTERSECTION;
					}
					WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_PIPE_CEN);
				}
			}
		}
	}

	return ret;
}

/**	
	@brief	The CHSRExport::WriteElbowToDWG function
	@author	humkyung
	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteElbowToDWG(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "face is NULL");
	size_t nCount = 0,nTotalCnt = 0;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pFace)
	{
		PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();
		if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (pFace->radius <= pHSRApp->m_clsOption.GetCriticalRadius())))
		{
			if(((HSR_POLYGON_SHAPE == HSR_SHAPE_MARK(pFace->ele)) || (HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele))) && pFace->pCenterline)
			{
				for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
				{
					if(INTERSECTION == ptr->render) nCount++;
				}
				
				m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_CIRCLE] = m_clsRule.Rule(m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_CIRCLE] , pFace);
				
				/// ARC형상이고 형상이 모드 보이면 primitive를 ARC형식으로 출력한다.
				/// 출력 파일의 크기를 줄일 수 있다.
				if((HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele)) && IsShapeAllShow(pFace->pCenterline))
				{
					VECTOR_T vec[2]={0,};
					vec[0].dx = pFace->ptEdge[0].x - pFace->ptCenter.x;
					vec[0].dy = pFace->ptEdge[0].y - pFace->ptCenter.y;
					vec[0].dz = 0;
					vec[1].dx = pFace->ptEdge[1].x - pFace->ptCenter.x;
					vec[1].dy = pFace->ptEdge[1].y - pFace->ptCenter.y;
					vec[1].dz = 0;
					
					double startDeg = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[0]));
					double endDeg   = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[1]));
					if(startDeg >= 360) startDeg -= 360;
					if(endDeg   >= 360) endDeg   -= 360;
					if((vec[0].dx*vec[1].dy - vec[0].dy*vec[1].dx) < 0) swap(endDeg , startDeg);
					
					CHSRApp* pHSRApp = CHSRApp::GetInstance();
					PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
					if(pprimitive)
					{
						pprimitive->hdr.id = ENM_ELLIPSE;
						pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
						pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy);
						pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
						pprimitive->body.ellipse.nStartAngle = startDeg;
						pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
						if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
						pprimitive->body.ellipse.vecNorm.dx = 0;
						pprimitive->body.ellipse.vecNorm.dy = 0;
						pprimitive->body.ellipse.vecNorm.dz = 1;
						pprimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
						
						m_pShowPrimitiveKind[LAYER_ELBOW_SIN] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_SIN],pprimitive);
					}
				}
				else
				{
					WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_SIN);
				}

				/// write cross line
				if(nCount > (((double)nTotalCnt)*0.25))
				{
					if(pFace->pCrossline && pHSRApp->m_clsOption.IsWeldElbowCrosslineOn())
						WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
				}
			}
			else if(SYMBOL == HSR_ELEMENT_MARK(pFace->ele))
			{
				if(pFace->pHead)
				{
					double nTotalLen = 0 , nRenderLen = 0;
					for(PHSR_VERTEX ptr = pFace->pHead;ptr && ptr->next;ptr = ptr->next,nTotalCnt++)
					{
						const double dx = ptr->next->pt->x - ptr->pt->x;
						const double dy = ptr->next->pt->y - ptr->pt->y;
						const double len= dx * dx + dy * dy;

						if(INVALID != ptr->render)
						{
							nTotalLen += len;
						}
						if((HIDE != ptr->render) && (INVALID != ptr->render))
						{
							nCount++;
							nRenderLen += len;
						}
					}
						
					/// write elbow symbol when visible length is grater than total length * 0.25
					if((double)nRenderLen > (double)(((double)nTotalLen) * 0.25))
					{
						for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
						{
							if(HIDE == ptr->render) ptr->render = INTERSECTION;
						}
														
						pFace->pHead = FindStartVertex(pFace->pHead);
						bool ret = WriteElbowSymbolToDWG(pFace,LAYER_ELBOW_CIRCLE_MARK,0,0);
						if(ret)
						{
							/// write cross line
							if(pFace->pCrossline && pHSRApp->m_clsOption.IsWeldElbowCrosslineOn())
								WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
						}
					}
					else
					{
						pFace->pHead = FindStartVertex(pFace->pHead);
						WriteVertexListToDWG(pFace , pFace->pHead , LAYER_ELBOW_CIRCLE_MARK);
					}
				}
			}
		}
		else
		{	// double line mode or radius is larger than critical radius.
			
			if(pFace->pHead)
			{
				WriteFaceToPrimitive(ret , pFace , LAYER_ELBOW_DOU);
				
				nCount = nTotalCnt = 0;
				if(!pHSRApp->m_clsOption.IsHiddenlineOn(LAYER_ELBOW_DOU))
				{
					for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
					{
						if(INTERSECTION == ptr->render) nCount++;
					}
				}
				else
				{
					nCount = nTotalCnt = 1;
				}
				/// write center line
				if(pFace->pCenterline && pHSRApp->m_clsOption.IsWeldElbowCenterlineOn())
				{
					if(nCount > (nTotalCnt*0.25))
					{
						WriteVertexListAsCenterlineToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_CEN);
					}
				}
				
				/// write cross line
				if(pFace->pCrossline && pHSRApp->m_clsOption.IsWeldElbowCrosslineOn())
				{
					if(nCount > (nTotalCnt*0.25))
					{
						WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
					}
				}
			}
		}
	}

	return ret;
}

/**	
	@brief	wirte SW-ELBOW to DWG

	@author	humkyung

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteSWElbowToDWG(PSQPRIMITIVE ret, CHSRFace* pFace)
{
	assert(pFace && "face is NULL");
	assert(pFace->radius > 0.);
	int nCount = 0,nTotalCnt  = 0;
	CHSRApp* pHSRApp= CHSRApp::GetInstance();
	PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();
	
	if(pFace && (pFace->radius > 0.))
	{
		if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (pFace->radius <= pHSRApp->m_clsOption.GetCriticalRadius()))){
			int nTotal=0,nVisible=0;
			for(PHSR_VERTEX ptr = pFace->pCenterline;ptr;ptr = ptr->next,nTotal++)
			{
				if(INTERSECTION == ptr->render) nVisible++;
			}
			
			// write cross line
			if((double)nVisible > (double)(nTotal*0.25))
			{
				m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_LINE] = m_clsRule.Rule(m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_LINE],pFace);
			}

			if(((SIDEFACE == HSR_ELEMENT_MARK(pFace->ele)) || (SIDEFACEX == HSR_ELEMENT_MARK(pFace->ele))) && pFace->pCenterline)
			{
				if((HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele)) && IsShapeAllShow(pFace->pCenterline))
				{
					VECTOR_T vec[2]={0,};
					vec[0].dx = pFace->ptEdge[0].x - pFace->ptCenter.x;
					vec[0].dy = pFace->ptEdge[0].y - pFace->ptCenter.y;
					vec[0].dz = 0;
					vec[1].dx = pFace->ptEdge[1].x - pFace->ptCenter.x;
					vec[1].dy = pFace->ptEdge[1].y - pFace->ptCenter.y;
					vec[1].dz = 0;
					
					double startDeg = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[0]));
					double endDeg   = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[1]));
					if(startDeg >= 360) startDeg -= 360;
					if(endDeg   >= 360) endDeg   -= 360;
					if((vec[0].dx*vec[1].dy - vec[0].dy*vec[1].dx) < 0) swap(endDeg , startDeg);

					CHSRApp* pHSRApp = CHSRApp::GetInstance();
					PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
					if(pprimitive)
					{
						pprimitive->hdr.id = ENM_ELLIPSE;
						pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
						pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy);
						pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
						pprimitive->body.ellipse.nStartAngle = startDeg;
						pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
						if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
						pprimitive->body.ellipse.vecNorm.dx = 0;
						pprimitive->body.ellipse.vecNorm.dy = 0;
						pprimitive->body.ellipse.vecNorm.dz = 1;
						pprimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
						
						m_pShowPrimitiveKind[LAYER_ELBOW_SIN] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_SIN],pprimitive);
					}
				}
				else
				{
					WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_SIN);
				}

				for(PHSR_VERTEX ptr = pFace->pCenterline;ptr;ptr = ptr->next,nTotalCnt++)
				{
					if(INTERSECTION == ptr->render) nCount++;
				}
				// write cross line
				if(nCount > (nTotalCnt*0.25))
				{
					if(pFace->pCrossline && pHSRApp->m_clsOption.IsSWElbowCrosslineOn())
						WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
				}
			}
			else if((SYMBOL == HSR_ELEMENT_MARK(pFace->ele)) && pFace->pHead)
			{
				int nTotalCnt = 0,nCount = 0;
				for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
				{
					if((SHOW == ptr->render) || (INTERSECTION == ptr->render)) nCount++;
				}
				if((double)nCount > (double)(nTotalCnt*0.25))
				{
					for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
					{
						if(HIDE == ptr->render) ptr->render = INTERSECTION;
					}
					
					pFace->pHead = FindStartVertex(pFace->pHead);
					bool ret = WriteElbowSymbolToDWG(pFace,LAYER_ELBOW_CIRCLE_MARK,0,0);
					if(ret)
					{
						if(pFace->pCrossline && pHSRApp->m_clsOption.IsSWElbowCrosslineOn())
						{
							WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
						}
					}
				}
			}
		}
		else
		{	// double line mode or radius is larger than critical radius.
			if(pFace->pHead)
			{
				/*
				if(IsShapeAllShow(pFace->pHead))
				{
					if(HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele))
					{
						VECTOR_T vec[2]={0,};
						vec[0].dx = pFace->ptEdge[0].x - pFace->ptCenter.x;
						vec[0].dy = pFace->ptEdge[0].y - pFace->ptCenter.y;
						vec[0].dz = 0;
						vec[1].dx = pFace->ptEdge[1].x - pFace->ptCenter.x;
						vec[1].dy = pFace->ptEdge[1].y - pFace->ptCenter.y;
						vec[1].dz = 0;
						
						double startDeg = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[0]));
						double endDeg   = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[1]));
						if(startDeg >= 360) startDeg -= 360;
						if(endDeg   >= 360) endDeg   -= 360;
						if((vec[0].dx*vec[1].dy - vec[0].dy*vec[1].dx) < 0) swap(endDeg , startDeg);
						
						CHSRApp* pHSRApp = CHSRApp::GetInstance();
						PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
						if(pprimitive)
						{
							pprimitive->hdr.id = ENM_ELLIPSE;
							pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
							pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy) + pFace->radius;
							pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
							pprimitive->body.ellipse.nStartAngle = startDeg;
							pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
							if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
							pprimitive->body.ellipse.vecNorm.dx = 0;
							pprimitive->body.ellipse.vecNorm.dy = 0;
							pprimitive->body.ellipse.vecNorm.dz = 1;
							
							m_pShowPrimitiveKind[LAYER_ELBOW_DOU] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_DOU],pprimitive);
						}
						
						pprimitive = pHSRApp->alloc_primitive();
						if(pprimitive)
						{
							pprimitive->hdr.id = ENM_ELLIPSE;
							pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
							pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy) - pFace->radius;
							pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
							pprimitive->body.ellipse.nStartAngle = startDeg;
							pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
							if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
							pprimitive->body.ellipse.vecNorm.dx = 0;
							pprimitive->body.ellipse.vecNorm.dy = 0;
							pprimitive->body.ellipse.vecNorm.dz = 1;
							
							m_pShowPrimitiveKind[LAYER_ELBOW_DOU] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_DOU],pprimitive);
						}
						
						/// write center line
						if(pFace->pCenterline && pHSRApp->m_clsOption.IsSWElbowCenterlineOn())
						{
							PSQPRIMITIVE pCenterLine = pHSRApp->alloc_primitive();
							if(pCenterLine)
							{
								pCenterLine->hdr.id = ENM_ELLIPSE;
								pCenterLine->body.ellipse.ptOrigin = pFace->ptCenter;
								pCenterLine->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy);
								pCenterLine->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
								pCenterLine->body.ellipse.nStartAngle = startDeg;
								pCenterLine->body.ellipse.nSweepAngle = endDeg - startDeg;
								if(pCenterLine->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
								pCenterLine->body.ellipse.vecNorm.dx = 0;
								pCenterLine->body.ellipse.vecNorm.dy = 0;
								pCenterLine->body.ellipse.vecNorm.dz = 1;
								
								m_pShowPrimitiveKind[LAYER_ELBOW_CEN] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_CEN],pCenterLine);
							}
						}
						
						/// write cross line
						if(pFace->pCrossline && pHSRApp->m_clsOption.IsSWElbowCrosslineOn())
						{
							WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
						}
					}
					else if(HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pFace->ele))
					{
						CHSRApp* pHSRApp = CHSRApp::GetInstance();
						PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
						if(pprimitive)
						{
							pprimitive->hdr.id = SQ_CIRCLE;
							pprimitive->body.circle.ptOrigin = pFace->ptCenter;
							pprimitive->body.circle.nRadius  = pFace->radius;
							pprimitive->body.circle.vecNorm.dx = 0;
							pprimitive->body.circle.vecNorm.dy = 0;
							pprimitive->body.circle.vecNorm.dz = 1;
							
							m_pShowPrimitiveKind[LAYER_ELBOW_DOU] = ListPushFront(m_pShowPrimitiveKind[LAYER_ELBOW_DOU],pprimitive);
						}
					}
				}
				else
				{
				*/
					//pFace->pHead = FindStartVertex(pFace->pHead);
					//WriteVertexListToDWG(pFace , pFace->pHead , LAYER_ELBOW_DOU);
					WriteFaceToPrimitive(ret , pFace , LAYER_ELBOW_DOU);
				
					if(!pHSRApp->m_clsOption.IsHiddenlineOn(LAYER_ELBOW_DOU))
					{
						for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
							if(INTERSECTION == ptr->render) nCount++;
					}
					else
					{
						nCount = nTotalCnt = 1;
					}
					
					/// write center line
					if(pFace->pCenterline && pHSRApp->m_clsOption.IsSWElbowCenterlineOn())
					{
						if(nCount > (nTotalCnt*0.25))
						{
							WriteVertexListAsCenterlineToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_CEN);
						}
					}
					
					/// write cross line
					if(pFace->pCrossline && pHSRApp->m_clsOption.IsSWElbowCrosslineOn() && pFace->pCrossline)
					{
						WriteVertexListToDWG(pFace , pFace->pCrossline , LAYER_ELBOW_CRS);
					}
				//}
			}
		}
	}

	return ret;
}

/**	
	@brief	write TEE primitive.

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE

	@remarks
	;2002.04.26 - append a code treat tee's symbol.\n
	;2003.06.12 - write tee symbol to 'LAYER_TEE_SIN' layer.

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteTeeToDWG(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");

	if(pFace)
	{
		CHSRApp* pHSRApp= CHSRApp::GetInstance();

		if(IsSingleLine(pFace->radius))
		{
			if((HSR_RECTANGLE_SHAPE == pFace->ele) && pFace->pCenterline)
			{
				int nTotal=0,nVisible=0;
				for(PHSR_VERTEX ptr = pFace->pCenterline;ptr;ptr = ptr->next,nTotal++)
				{
					if(ptr->next && (INTERSECTION == ptr->render)) nVisible++;
				}
			
				/// write weld mark circle
				if(pHSRApp->GetEnv()->m_oCompProp[LAYER_ELBOW_WELD_MARK_CIRCLE].IsOn() && ((double)nVisible > (double)(nTotal*0.25)))
				{
					m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_CIRCLE] = m_clsRule.Rule(m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_CIRCLE],pFace);
				}
				WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_TEE_SIN);
			}
			else if((SYMBOL == HSR_ELEMENT_MARK(pFace->ele)) && pFace->pHead)
			{
				pFace->pHead = FindStartVertex(pFace->pHead);
				WriteVertexListToDWG(pFace , pFace->pHead , LAYER_TEE_SIN);
			}
		}
		else
		{
			int nCount = 0,nTotalCnt = 0;

			if(pFace->pHead)
			{
				pFace->pHead = FindStartVertex(pFace->pHead);
				WriteVertexListToDWG(pFace , pFace->pHead , LAYER_TEE_DOU);

				if(pHSRApp->m_clsOption.IsTeeCenterlineOn())
				{
					if(!pHSRApp->m_clsOption.IsHiddenlineOn(LAYER_TEE_DOU))
					{
						for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,nTotalCnt++)
						{
							if(INTERSECTION == ptr->render) nCount++;
						}
					}
					else
					{
						nCount = nTotalCnt = 1;
					}
				}
			}

			if(nCount > (nTotalCnt*0.25))
			{
				if(pFace->pCenterline)	WriteVertexListAsCenterlineToDWG(pFace , pFace->pCenterline , LAYER_TEE_CEN);
			}
		}
	}

	return ret;
}

/**	
	@brief	The CHSRExport::WriteTeeXToDWG function

	@author	humkyung

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE

	@remarks
	;2003.06.12 - write tee symbol to 'LAYER_TEE_SIN' layer.

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteTeeXToDWG(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");

	if(pFace)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();

		if(IsSingleLine(pFace->radius))
		{
			if((HSR_RECTANGLE_SHAPE == pFace->ele) && pFace->pCenterline)
			{
				int nTotal=0,nVisible=0;
				for(PHSR_VERTEX ptr = pFace->pCenterline;ptr;ptr = ptr->next,nTotal++)
				{
					if(ptr->next && (INTERSECTION == ptr->render)) nVisible++;
				}
			
				// write cross line
				if((double)nVisible > (double)(nTotal*0.25))
				{
					m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_LINE] = m_clsRule.Rule(m_pShowPrimitiveKind[LAYER_ELBOW_WELD_MARK_LINE],pFace);
				}
				WriteVertexListToDWG(pFace , pFace->pCenterline,LAYER_TEE_SIN);
			}
			else if((SYMBOL == HSR_ELEMENT_MARK(pFace->ele)) && pFace->pHead)
			{
				pFace->pHead = FindStartVertex(pFace->pHead);
				WriteVertexListToDWG(pFace , pFace->pHead,LAYER_TEE_SIN);
			}
		}
		else
		{
			int Count = 0,TotalCnt = 0;

			if(pFace->pHead)
			{
				pFace->pHead = FindStartVertex(pFace->pHead);
				WriteVertexListToDWG(pFace , pFace->pHead,LAYER_TEE_DOU);

				for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next,TotalCnt++)
				{
					if(INTERSECTION == ptr->render) Count++;
				}
			}

			if(Count > (TotalCnt/4))
			{
				if(pFace->pCenterline)	WriteVertexListAsCenterlineToDWG(pFace , pFace->pCenterline , LAYER_TEE_CEN);
			}
		}
	}

	return ret;
}

/**	
	@brief	write face to primitive

	@author	humkyung

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	@param	kind	a parameter of type HSR_LAYER_E

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteFaceToPrimitive(PSQPRIMITIVE ret, CHSRFace* pFace , HSR_LAYER_E kind)
{
	assert(pFace && "face is NULL");

	/// check this face created from LINE element - 2012.01.19 added by humkyung
	if(pFace && (NULL != pFace->body) && pFace->body->IsKindOf(CLineElement::TypeString()))
	{
		if(pFace->pCenterline)
		{
			WriteVertexListToDWG(pFace , pFace->pCenterline , kind);
		}
	}
	/// up to here
	else if(pFace && pFace->pHead)
	{
		if(IsShapeAllShow(pFace->pHead))
		{
			if(HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pFace->ele))
			{
				CHSRApp* pHSRApp = CHSRApp::GetInstance();
				PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
				if(pprimitive)
				{
					pprimitive->hdr.id = SQ_CIRCLE;
					pprimitive->body.circle.ptOrigin = pFace->ptCenter;
					pprimitive->body.circle.nRadius  = pFace->radius;
					pprimitive->body.circle.vecNorm.dx = 0;
					pprimitive->body.circle.vecNorm.dy = 0;
					pprimitive->body.circle.vecNorm.dz = 1;
					pprimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
					
					m_pShowPrimitiveKind[kind] = ListPushFront(m_pShowPrimitiveKind[kind],pprimitive);
				}
			}
			else if(HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele))
			{
				VECTOR_T vec[2]={0,};
				vec[0].dx = pFace->ptEdge[0].x - pFace->ptCenter.x;
				vec[0].dy = pFace->ptEdge[0].y - pFace->ptCenter.y;
				vec[0].dz = 0;
				vec[1].dx = pFace->ptEdge[1].x - pFace->ptCenter.x;
				vec[1].dy = pFace->ptEdge[1].y - pFace->ptCenter.y;
				vec[1].dz = 0;
				
				double startDeg = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[0]));
				double endDeg   = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[1]));
				if(startDeg >= 360) startDeg -= 360;
				if(endDeg   >= 360) endDeg   -= 360;
				if((vec[0].dx*vec[1].dy - vec[0].dy*vec[1].dx) < 0) swap(endDeg , startDeg);
				
				CHSRApp* pHSRApp = CHSRApp::GetInstance();
				PSQPRIMITIVE pprimitive = pHSRApp->alloc_primitive();
				if(pprimitive)
				{
					pprimitive->hdr.id = ENM_ELLIPSE;
					pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
					pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy) + pFace->radius;
					pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
					pprimitive->body.ellipse.nStartAngle = startDeg;
					pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
					if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
					pprimitive->body.ellipse.vecNorm.dx = 0;
					pprimitive->body.ellipse.vecNorm.dy = 0;
					pprimitive->body.ellipse.vecNorm.dz = 1;
					pprimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
					
					m_pShowPrimitiveKind[kind] = ListPushFront(m_pShowPrimitiveKind[kind],pprimitive);
				}
				
				pprimitive = pHSRApp->alloc_primitive();
				if(pprimitive)
				{
					pprimitive->hdr.id = ENM_ELLIPSE;
					pprimitive->body.ellipse.ptOrigin = pFace->ptCenter;
					pprimitive->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy) - pFace->radius;
					pprimitive->body.ellipse.nAxis[1] = pprimitive->body.ellipse.nAxis[0];
					pprimitive->body.ellipse.nStartAngle = startDeg;
					pprimitive->body.ellipse.nSweepAngle = endDeg - startDeg;
					if(pprimitive->body.ellipse.nSweepAngle < 0) pprimitive->body.ellipse.nSweepAngle += 360;
					pprimitive->body.ellipse.vecNorm.dx = 0;
					pprimitive->body.ellipse.vecNorm.dy = 0;
					pprimitive->body.ellipse.vecNorm.dz = 1;
					pprimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
					
					m_pShowPrimitiveKind[kind] = ListPushFront(m_pShowPrimitiveKind[kind],pprimitive);
				}
			}
			else
			{
				pFace->pHead = FindStartVertex(pFace->pHead);
				if(pFace->pHead) WriteVertexListToDWG(pFace , pFace->pHead , kind);
			}
		}
		else
		{
			pFace->pHead = FindStartVertex(pFace->pHead);
			if(pFace->pHead) WriteVertexListToDWG(pFace , pFace->pHead , kind);
		}
	}

	return ret;
}

/**	
	@brief	write box face to primitive
	@author	humkyung
	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	@param	kind	a parameter of type HSR_LAYER_E
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteBoxToDWG(PSQPRIMITIVE ret, CHSRFace* pFace,HSR_LAYER_E kind)
{
	assert(pFace && "face is invalid");

	/// check this face created from LINE element - 2012.01.19 added by humkyung
	if(pFace && (NULL != pFace->body) && pFace->body->IsKindOf(CLineElement::TypeString()))
	{
		if(pFace->pCenterline)
		{
			WriteVertexListToDWG(pFace , pFace->pCenterline , kind);
		}
	}
	/// up to here
	else if(pFace && pFace->pHead)
	{
		pFace->pHead = FindStartVertex(pFace->pHead);
		WriteVertexListToDWG(pFace , pFace->pHead , kind);
	}

	return ret;
}

/**	\brief	The CHSRExport::WriteDomeToDWG function

	\param	pPrimitiveList	a parameter of type PSQPRIMITIVE
	\param	pFace	a parameter of type PFACE
	\param	kind	a parameter of type HSR_LAYER_E

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WriteDomeToDWG(PSQPRIMITIVE pPrimitiveList, CHSRFace* pFace,HSR_LAYER_E kind)
{
	assert(pFace && "pFace is NULL");

	if(pFace && pFace->pHead)
	{
		pFace->pHead = FindStartVertex(pFace->pHead);
		WriteVertexListToDWG(pFace , pFace->pHead , kind);
	}

	return pPrimitiveList;
}

/**
 * WriteVertexListToDWG:
 
	@author	humkyung

 * @param pFace 
 * @param pVertex 
 * @param kind 
 * @return void 
 */
void CHSRExport::WriteVertexListToDWG(CHSRFace* pFace , PHSR_VERTEX pVertex,HSR_LAYER_E kind)
{
	assert(pFace && "pFace is NULL");
	assert(pVertex && "pVertex is NULL");

	if(pFace && pVertex)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PHSR_VERTEX pEndVt = NULL;

		for(PHSR_VERTEX ptr = pVertex;ptr && ptr->next;)
		{
			if(INTERSECTION == ptr->render)
			{
				for(pEndVt = ptr;pEndVt && pEndVt->next && (pEndVt->render == ptr->render);pEndVt = pEndVt->next);
				if(HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pFace->ele))
				{
					m_pShowPrimitiveKind[kind] = WriteArcToDWG(m_pShowPrimitiveKind[kind] , pFace , ptr , pEndVt);
				}
				else
				{
					m_pShowPrimitiveKind[kind] = WritePolylineToDWG(m_pShowPrimitiveKind[kind],ptr,pEndVt , pFace->body , 0 , ATOI_T(pFace->color().c_str()));
				}
				ptr = pEndVt;
			}
			else if(pHSRApp->m_clsOption.IsHiddenlineOn(kind) && (HIDE == ptr->render))
			{
				for(pEndVt = ptr;pEndVt && pEndVt->next && (pEndVt->render == ptr->render);pEndVt = pEndVt->next);
				m_pHiddenPrimitiveKind[kind] = WritePolylineToDWG(m_pHiddenPrimitiveKind[kind],ptr,pEndVt , pFace->body , 0 , ATOI_T(pFace->color().c_str()));
				ptr = pEndVt;
			}else	ptr = ptr->next;
		}
	}
}

#define area(ax,ay,bx,by) fabs((ax)*(by) - (ay)*(bx))

/**	
	@brief	write a polyline which located between start vertex and end vertex.
	이웃한 4개의 정점들로 이루어진 2개의 라인이 일직선 상에 존재하면 하나의 라인으로 대체한다.

	@author	humkyung

	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pStartVt	start vertex
	@param	pEndVt	end vertex
	@param	ltype	a parameter of type int
	@param	color	a parameter of type int

	@remarks
	;2004.03.12 - change tolerance value
	
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRExport::WritePolylineToDWG(PSQPRIMITIVE ret,PHSR_VERTEX pStartVt,PHSR_VERTEX pEndVt,CHSRElement* body,const int& ltype , const int& lcolor)
{
	assert(pStartVt && "pStartVt is NULL");
	assert(pEndVt   && "pEndVt is NULL");

	if(pStartVt && pEndVt && (pStartVt != pEndVt))
	{
		double px = 1.,py = -1.,x,y,tol,val;
		double nLength = 0.;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		POINT_T point={0,};
		PSQPRIMITIVE primitive = NULL;
		PPOINT_TNODE polyline = NULL,pTail=NULL;
		CHSRElement* hider = pStartVt->hider;	/// 2012.01.12 added by humkyung

		PHSR_VERTEX pVt = NULL;
		for(pVt = pStartVt;pVt && (pVt != pEndVt) && pVt->next;pVt = pVt->next)
		{
			if(polyline)
			{
				x = pVt->next->pt->x - pVt->pt->x;
				y = pVt->next->pt->y - pVt->pt->y;
				double dz= pVt->next->pt->z - pVt->pt->z;

				nLength+=x*x + y*y;

				tol = 0.01*x*px;
				val = x*py - y*px;

				/// 두개의 라인의 변화를 검사.
				if((dz > HSR_TOLER) || (val < -tol || val > tol))
				{
					point.x = pVt->pt->x;
					point.y = pVt->pt->y;
					point.z = pVt->pt->z;
					if(NULL == polyline)
						polyline = pTail = PolylinePush(polyline,&point);
					else	pTail = PolylinePush(pTail,&point);
				
			 		px = x;
					py = y;
				}
			}
			else
			{ // first
				px = pVt->next->pt->x - pVt->pt->x;
				py = pVt->next->pt->y - pVt->pt->y;
				nLength+= px*px + py*py;

				point.x = pVt->pt->x;
				point.y = pVt->pt->y;
				point.z = pVt->pt->z;
				if(NULL == polyline)
					polyline = pTail = PolylinePush(polyline,&point);
				else	pTail = PolylinePush(pTail,&point);
				//polyline = PolylinePush(polyline,&point);
			}
		}
		
		/// check length in order to remove too short line and self hidden
		if(polyline && ((NULL == body) || (NULL == hider) || (body != hider)) && (nLength > CHSRExport::_TOLER))
		{
			if(NULL != (primitive = pHSRApp->alloc_primitive()))
			{
				point = *(pVt->pt);
				if(NULL == polyline)
					polyline = pTail = PolylinePush(polyline,&point);
				else	pTail = PolylinePush(pTail,&point);

				primitive->hdr.id    = ENM_POLYLINE;
				primitive->hdr.attr.d= 3;
				primitive->hdr.display.ltype = (short)ltype;
				primitive->hdr.display.lscale= 0.;
				primitive->body.polyline     = polyline;
				primitive->hdr.display.lcolor = (BYTE)(lcolor);
			
				ret = ListPushFront(ret,primitive);
			}
		}
		else if(polyline)
		{
			/// free allocated memory - 2012.01.13 added by humkyung
			PPOINT_TNODE pNext = NULL;
			for(PPOINT_TNODE p = polyline;p;p = pNext)
			{
				pNext = p->next;
				::free((void*)p);
			}
			/// up to here
		}
	}

	return ret;
}

/**
	@author		humkyung
	
	@parameter	 pVertex - Vertex list.
	
	@description 1. get number of vertexs in vertex list.
				 2. if vertex's count is larger than 6, consider line segments.
	
	@remarks	 if shape is double-line,center line must be drawed.

	@returns	 
*/
void CHSRExport::WriteVertexListAsCenterlineToDWG(CHSRFace* pFace , PHSR_VERTEX pVertex , HSR_LAYER_E kind)
{
	assert(pFace && "pFace is NULL");

	if(pFace && pVertex)
	{
		int i,count;
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PSQPRIMITIVE primitive = NULL;
		PHSR_VERTEX pVt = NULL;

		for(count = 0,pVt = pVertex;pVt;pVt = pVt->next) count++;
		if(count > 6)
		{
			int i,nSeg[5];

			nSeg[0] = nSeg[4] = (count - 6)/2;
			nSeg[1] = nSeg[2] = nSeg[3] = 2;
			for(i = 0,pVt = pVertex;pVt && i < nSeg[0];i++,pVt = pVt->next)
				pVt->render = INTERSECTION;
			for(i = 0;pVt && i < nSeg[1];i++,pVt = pVt->next)
				pVt->render = HIDE;
			for(i = 0;pVt && i < nSeg[2];i++,pVt = pVt->next)
				pVt->render = INTERSECTION;
			for(i = 0;pVt && i < nSeg[3];i++,pVt = pVt->next)
				pVt->render = HIDE;
			for(i = 0;pVt && i < nSeg[4];i++,pVt = pVt->next)
				pVt->render = INTERSECTION;

			WriteVertexListToDWG(pFace , pVertex , kind);
		}
		else if(count >= 2)
		{
			for(i = 0,pVt = pVertex;(i < count - 1) && pVt && pVt->next;i++,pVt = pVt->next)
			{
				if((INVALID != pVt->render) && (NULL != (primitive = pHSRApp->alloc_primitive())))
				{
					primitive->hdr.id       = ENM_LINE;
					primitive->hdr.attr.d   = 3;
					primitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
					primitive->hdr.display.ltype    = CENTER_LINE;
					primitive->hdr.display.lscale   = 0.;//GetHiddenView()->GetViewScale();
					
					primitive->body.line.ptStart.x = pVt->pt->x;
					primitive->body.line.ptStart.y = pVt->pt->y;
					primitive->body.line.ptStart.z = pVt->pt->z;
					primitive->body.line.ptEnd.x   = pVt->next->pt->x;
					primitive->body.line.ptEnd.y   = pVt->next->pt->y;
					primitive->body.line.ptEnd.z   = pVt->next->pt->z;
			
					m_pShowPrimitiveKind[kind] = ListPushFront(m_pShowPrimitiveKind[kind],primitive);
				}
			}
		}
	}
}

/**	
	@brief	find new start vertex

	@author	humkyung

	@param	pStart	a parameter of type PVERTEX

	@return	new start vertex
*/
PHSR_VERTEX CHSRExport::FindStartVertex(PHSR_VERTEX pStart)
{
	assert(pStart && "pStart is NULL");

	if(pStart)
	{
		PHSR_VERTEX pEnd=NULL,pBeforeEnd=NULL;
		PHSR_VERTEX pNewStart=NULL,pBeforeNewStart=NULL;

		for(pNewStart = pStart;pNewStart && ((INTERSECTION == pNewStart->render) || (SHOW == pNewStart->render));pNewStart = pNewStart->next);
		for(pBeforeNewStart = pNewStart;pNewStart && !((INTERSECTION == pNewStart->render) || (SHOW == pNewStart->render));pNewStart = pNewStart->next)
			pBeforeNewStart = pNewStart;
		if(pNewStart && pNewStart->next)
		{ // not new end vertex
			for(pBeforeEnd = pNewStart,pEnd = pNewStart;pEnd && pEnd->next;pEnd = pEnd->next)
				pBeforeEnd = pEnd;
			
			assert(pBeforeEnd && "pBeforeEnd is NULL");
			assert(pEnd && "pEnd is NULL");
			if(pBeforeEnd && pEnd)
			{
				pBeforeEnd->next     = pStart;
				pBeforeNewStart->next= pEnd;
				pEnd->pt             = pNewStart->pt;
				pEnd->next           = NULL;
				
				return pNewStart;
			}
		}
	}

	return pStart;
}

// layer name
const static TCHAR* szLayer0_1[50]=
{
	_T("[PIPE_SINGLELINE]"),_T("[PIPE_SINGLELINE_HIDDEN]"),
	_T("[PIPE_DOUBLELINE]"),_T("[PIPE_DOUBLELINE_HIDDEN]"),
	_T("[PIPE_CENTERLINE]"),_T("[]"),
	_T("[PIPE_OTHER]"),_T("[H_OTHER]"),
	_T("[PIPE_MARK]"),_T("[]"),
	_T("[ELBOW_SINGLELINE]"),_T("[ELBOW_SINGLELINE_HIDDEN]"),
	_T("[ELBOW_DOUBLELINE]"),_T("[ELBOW_DOUBLELINE_HIDDEN]"),
	_T("[ELBOW_CENTERLINE]"),_T("[]"),
	_T("[CROSS_LINE]"),_T("[]"),
	_T("[WELD_MARK]"),_T("[]"),
	_T("[WELD_MARK]"),_T("[]"),
	_T("[CIRCLE_MARK]"),_T("[]"),
	_T("[TEE_SINGLELINE]"),_T("[TEE_SINGLELINE_HIDDEN]"),
	_T("[TEE_DOUBLELINE]"),_T("[TEE_DOUBLELINE_HIDDEN]"),
	_T("[TEE_CENTERLINE]"),_T("[]"),
	_T("[OTHER_COMPONENT]"),_T("[OTHER_COMPONENT_HIDDEN]"),
	_T("[EQUIPMENT]"),_T("[EQUIPMENT_HIDDEN]"),
	_T("[EQUIPMENT_CENTER]"),_T("[]"),
	_T("[STRUCTURE]"),_T("[STRUCTURE_HIDDEN]"),
	_T("[STRUCTURE_CENTERLINE]"),_T("[]"),
	_T("[CABLE]"),_T("[H_CABLE]"),
	_T("[VIEW]"),_T("[]"),
	_T("[DIMENSION]"),_T("[]"),
	_T("[ANNOTATION]"),_T("[]"),
	_T("[BORDER]"),_T("[]")
};

const static TCHAR* szLayer0_2[50]=
{
	_T("[S_PIPE]"),_T("[H_S_PIPE]"),
	_T("[D_PIPE]"),_T("[H_D_PIPE]"),
	_T("[C_PIPE]"),_T("[0]"),
	_T("[PIPE_OTHER]"),_T("[H_OTHER]"),
	_T("[PIPE_MARK]"),_T("[0]"),
	_T("[S_ELBOW]"),_T("[H_S_ELBOW]"),
	_T("[D_ELBOW]"),_T("[H_D_ELBOW]"),
	_T("[C_ELBOW]"),_T("[0]"),
	_T("[CROSS]"),_T("[0]"),
	_T("[WELD_MARK]"),_T("[0]"),
	_T("[WELD_MARK]"),_T("[0]"),
	_T("[CIRCLE_MARK]"),_T("[0]"),
	_T("[S_TEE]"),_T("[H_S_TEE]"),
	_T("[D_TEE]"),_T("[H_D_TEE]"),
	_T("[C_TEE]"),_T("[0]"),
	_T("[OTHER]"),_T("[H_OTHER]"),
	_T("[EQP]"),_T("[H_EQP]"),
	_T("[C_EQP]"),_T("[0]"),
	_T("[STR]"),_T("[H_STR]"),
	_T("[C_STR]"),_T("[0]"),
	_T("[CABLE]"),_T("[H_CABLE]"),
	_T("[VIEW_LINE]"),_T("[0]"),
	_T("[DIMENSION]"),_T("[0]"),
	_T("[ANNOTATION]"),_T("[0]"),
	_T("[BORDER]"),_T("[0]")
};

const static TCHAR* szLayer0_3[50]=
{
	_T("[S_PIPE]"),_T("[H_S_PIPE]"),
	_T("[D_PIPE]"),_T("[H_D_PIPE]"),
	_T("[C_PIPE]"),_T("[]"),
	_T("[PIPE_OTHER]"),_T("[H_OTHER]"),
	_T("[PIPE_MARK]"),_T("[]"),
	_T("[S_ELBOW]"),_T("[H_S_ELBOW]"),
	_T("[D_ELBOW]"),_T("[H_D_ELBOW]"),
	_T("[C_ELBOW]"),_T("[]"),
	_T("[CROSS]"),_T("[]"),
	_T("[WELD_MARK]"),_T("[]"),
	_T("[WELD_MARK]"),_T("[]"),
	_T("[CIRCLE_MARK]"),_T("[]"),
	_T("[S_TEE]"),_T("[H_S_TEE]"),
	_T("[D_TEE]"),_T("[H_D_TEE]"),
	_T("[C_TEE]"),_T("[]"),
	_T("[OTHER]"),_T("[H_OTHER]"),
	_T("[EQP]"),_T("[H_EQP]"),
	_T("[C_EQP]"),_T("[]"),
	_T("[STR]"),_T("[H_STR]"),
	_T("[C_STR]"),_T("[]"),
	_T("[CABLE]"),_T("[H_CABLE]"),
	_T("[VIEW_LINE]"),_T("[]"),
	_T("[DIMENSION]"),_T("[]"),
	_T("[ANNOTATION]"),_T("[]"),
	_T("[BORDER]"),_T("[]")
};

/**	
	@brief	write primitive to immediate file
	@param	pFileName	immediate file name
	@author	humkyung
	@remarks	[2006.02.13] - add type to output format.
	@return	bool	
*/
bool CHSRExport::WritePrimitiveAsAsciiFile(LPCTSTR pFileName)
{
	assert(pFileName && "pFileName is NULL-[CHSRExport::WritePrimitiveAsIntermediateFile]");
	
	static const TCHAR* format[5]=
	{
		_T("LINE %lf %lf %lf %lf\r\n") ,
		_T("CIRCLE %lf %lf %lf\r\n") ,
		_T("ARC %lf %lf %lf %lf %lf\r\n") , 
		_T("DONUT %lf %lf %lf\r\n") , 
		_T("POLYLINE %d\r\n")
	};
	PPOINT_TNODE  pPolyline=NULL;
	bool ret = false;
		
	OFSTREAM_T ofile(pFileName);
	if(ofile.is_open())
	{
#ifdef VER_03
		const char** szLayer = szLayer0_3;
#else
		const TCHAR** szLayer=(0 == STRCMP_T(VER,_T("HSR Ver 0.2"))) ? szLayer0_2 : szLayer0_1;
#endif
		{
			int nCount=0;
			double nStartAngle=0.,nEndAngle=0.;
			PSQPRIMITIVE ptr = NULL;
			CHSRApp*  pHSRApp = CHSRApp::GetInstance();
			CHSROption* pEnv = pHSRApp->GetEnv();
			const bool bViewLineOn = pEnv->IsViewLineDisplayOn();
			
			if(TOP == pHSRApp->m_clsView.GetViewPoint())
			{
				/// write hidden primitive
				for(int i = 0;i < 23;++i)
				{
					if(m_pHiddenPrimitiveKind[i])
					{
						for(ptr = m_pHiddenPrimitiveKind[i];ptr;ptr = ptr->next)
						{
							if(ENM_LINE == ptr->hdr.id)
							{
								if(LAYER_VIEW == i)
								{
									if(true == bViewLineOn)
									{
										ofile << _T("<LINE>|");
										ofile << ptr->body.line.ptStart.x << _T(",") << ptr->body.line.ptStart.y << _T(",0,"); 
										ofile << ptr->body.line.ptEnd.x << _T(",") << ptr->body.line.ptEnd.y << _T(",0|");
										ofile << pEnv->m_oCompProp[i] << std::endl;
									}
								}
								else if(pEnv->m_oHiddenCompProp[i].IsOn())
								{
									ofile << _T("<LINE>|");
									ofile << ptr->body.line.ptStart.x << _T(",") << ptr->body.line.ptStart.y << _T(",0,");
									ofile << ptr->body.line.ptEnd.x << _T(",") << ptr->body.line.ptEnd.y << _T(",0|");
									if(pEnv->m_oHiddenCompProp[i].IsAutomaticColor())
										ofile << pEnv->m_oHiddenCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oHiddenCompProp[i].m_sStyle << _T(",") << pEnv->m_oHiddenCompProp[i].m_sScale << _T(",") << pEnv->m_oHiddenCompProp[i].m_sWeight << std::endl;
									else
										ofile << pEnv->m_oHiddenCompProp[i] << std::endl;
								}
							}
							else if(ENM_POLYLINE == ptr->hdr.id)
							{
								if(pEnv->m_oHiddenCompProp[i].IsOn())
								{
									ofile << _T("<PLINE>|");
									for(pPolyline = ptr->body.polyline;pPolyline;pPolyline = pPolyline->next)
									{
										ofile << pPolyline->pt.x << _T(",") << pPolyline->pt.y;
										(NULL != pPolyline->next) ? (ofile << _T(",")) : (ofile << _T("|"));
									}
									if(pEnv->m_oHiddenCompProp[i].IsAutomaticColor())
										ofile << pEnv->m_oHiddenCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oHiddenCompProp[i].m_sStyle << _T(",") << pEnv->m_oHiddenCompProp[i].m_sScale << _T(",") << pEnv->m_oHiddenCompProp[i].m_sWeight << std::endl;
									else
										ofile << pEnv->m_oHiddenCompProp[i] << std::endl;
								}
							}
						}
					}
				}

				/// write show primitive
				for(int i = 0;i < 23;i++)
				{
					if(pEnv->m_oCompProp[i].IsOn() && m_pShowPrimitiveKind[i])
					{
						for(ptr = m_pShowPrimitiveKind[i];ptr;ptr = ptr->next)
						{
							if(ENM_LINE == ptr->hdr.id)
							{
								if(LAYER_VIEW == i)
								{
									ofile << _T("<LINE>|");
									ofile << ptr->body.line.ptStart.x << _T(",") << ptr->body.line.ptStart.y << _T(",0,");
									ofile << ptr->body.line.ptEnd.x << _T(",") << ptr->body.line.ptEnd.y << _T(",0|");
									ofile << pEnv->m_oCompProp[i] << std::endl;
								}
								else
								{
									ofile << _T("<LINE>|");
									ofile << ptr->body.line.ptStart.x << _T(",") << ptr->body.line.ptStart.y << _T(",0,");
									ofile << ptr->body.line.ptEnd.x << _T(",") << ptr->body.line.ptEnd.y << _T(",0|");
									if(pEnv->m_oCompProp[i].IsAutomaticColor())
										ofile << pEnv->m_oCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oCompProp[i].m_sStyle << _T(",") << pEnv->m_oCompProp[i].m_sScale << _T(",") << pEnv->m_oCompProp[i].m_sWeight << std::endl;
									else
										ofile << pEnv->m_oCompProp[i] << std::endl;
								}
							}
							else if(ENM_POLYLINE == ptr->hdr.id)
							{
								ofile << _T("<PLINE>|");
								for(pPolyline = ptr->body.polyline;pPolyline;pPolyline = pPolyline->next)
								{
									ofile << pPolyline->pt.x << _T(",") << pPolyline->pt.y;
									(NULL != pPolyline->next) ? (ofile << _T(",")) : (ofile << _T("|"));
								}
								if(pEnv->m_oCompProp[i].IsAutomaticColor())
									ofile << pEnv->m_oCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oCompProp[i].m_sStyle << _T(",") << pEnv->m_oCompProp[i].m_sScale << _T(",") << pEnv->m_oCompProp[i].m_sWeight << std::endl;
								else
									ofile << pEnv->m_oCompProp[i] << std::endl;
							}
							else if(SQ_CIRCLE == ptr->hdr.id)
							{
								if(LAYER_ELBOW_WELD_MARK_CIRCLE == i)
								{
									ofile << _T("<DONUT>|");
									ofile << _T("0,") << ptr->body.circle.nRadius << _T(",");
									ofile << ptr->body.circle.ptOrigin.x << _T(",") << ptr->body.circle.ptOrigin.y << _T("|");
									ofile << pEnv->m_oCompProp[i].m_sLevel << _T(",") << pEnv->m_oCompProp[i].color() << std::endl;
								}
								else
								{
									ofile << _T("<CIRCLE>|");
									ofile << ptr->body.circle.ptOrigin.x << _T(",") << ptr->body.circle.ptOrigin.y << _T(",0,");
									ofile << ptr->body.circle.nRadius << _T("|");
									if(pEnv->m_oCompProp[i].IsAutomaticColor())
										ofile << pEnv->m_oCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oCompProp[i].m_sStyle << _T(",") << pEnv->m_oCompProp[i].m_sScale << _T(",") << pEnv->m_oCompProp[i].m_sWeight << std::endl;
									else
										ofile << pEnv->m_oCompProp[i] << std::endl;
								}
							}
							else if(ENM_ELLIPSE == ptr->hdr.id)
							{
								nStartAngle = ptr->body.ellipse.nStartAngle;
								nEndAngle   = ptr->body.ellipse.nStartAngle + ptr->body.ellipse.nSweepAngle;
								nEndAngle   = (nEndAngle > 360.) ? nEndAngle - 360. : nEndAngle;
								
								ofile << _T("<ARC>|");
								ofile << ptr->body.ellipse.ptOrigin.x << _T(",") << ptr->body.ellipse.ptOrigin.y << _T(",0,");
								ofile << ptr->body.ellipse.nAxis[0] << _T(",");
								ofile << nStartAngle << _T(",") << nEndAngle << _T("|");
								if(pEnv->m_oCompProp[i].IsAutomaticColor())
									ofile << pEnv->m_oCompProp[i].m_sLevel << _T(",") << int(ptr->hdr.display.lcolor) << _T(",") << pEnv->m_oCompProp[i].m_sStyle << _T(",") << pEnv->m_oCompProp[i].m_sScale << _T(",") << pEnv->m_oCompProp[i].m_sWeight << std::endl;
								else
									ofile << pEnv->m_oCompProp[i] << std::endl;
							}
						}
					}
				}
			}
			ofile.flush();
			ofile.close();

			ret = true;
		}
	}

	return ret;
}

//	parameter	:
//	description	: calculate number of edges in pPrimitive
//	remarks		:
//	returns		:
int CHSRExport::GetEdgeCount(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	int count = 0;

	if(pPrimitive)
	{
		PSQPRIMITIVE ptr;
		for(ptr = pPrimitive;ptr;ptr = ptr->next)
		{
			if(ENM_LINE == ptr->hdr.id)
			{
				count++;
			}
			else if(ENM_POLYLINE == ptr->hdr.id)
			{
				count++;
				//PPOINT_TNODE  pPolyline;
				//for(pPolyline = ptr->body.polyline;pPolyline && pPolyline->next;pPolyline = pPolyline->next) count++;
			}
			else if(SQ_CIRCLE == ptr->hdr.id)
			{
				count++;
			}
			else if(ENM_ELLIPSE == ptr->hdr.id)
			{
				count++;
			}
			else if(ENM_ARC == ptr->hdr.id)
			{
				count++;
			}
		}
	}

	return count;
}

//	parameter	:
//	description	: pure member data
//	remarks		: desitination area is deleted, but it's contents exists
//	returns		:
void CHSRExport::Clear()
{
	for(int i = 0;i < 24;i++)
	{
		m_pShowPrimitiveKind[i] = NULL;
		m_pHiddenPrimitiveKind[i] = NULL;
	}
}

/**	\brief	The CHSRExport::Add function

	\param	enmLineKind	a parameter of type HSR_LAYER_E
	\param	render	a parameter of type RENDERTYPE
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool	
*/
bool CHSRExport::Add(HSR_LAYER_E enmLineKind,RENDERTYPE render,PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive)
	{
		if(SHOW == render)
			m_pShowPrimitiveKind[enmLineKind]  = ListPushFront(m_pShowPrimitiveKind[enmLineKind],pPrimitive);
		else if(HIDE == render)
			m_pHiddenPrimitiveKind[enmLineKind]= ListPushFront(m_pShowPrimitiveKind[enmLineKind],pPrimitive);
		
		bRet = true;
	}

	return bRet;
}

//	parameter	:
//	description	: write elbow symbol
//	remarks		: [2002.05.25] - change original routine to new routine writing elbow symbol using edge point.
//			:	       - when elbow type is SW_ELBOW add routine checking min size.
//	returns		:
bool CHSRExport::WriteElbowSymbolToDWG(CHSRFace* pFace, HSR_LAYER_E kind, int ltype, int lcolor)
{
	assert(pFace        && "pFace is invalid");
	assert(pFace->pHead && "pFace->pHead is invalid");
	ltype  = ltype;
	lcolor = lcolor;

	if(pFace && pFace->pHead)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		double nAngle = (ELBOW == pFace->type) ? pHSRApp->m_clsOption.GetWeldElbowAngle() : pHSRApp->m_clsOption.GetSWElbowAngle();
		
		POINT_T pt={0};
		pt.x = pFace->ptEdge[0].x - pFace->ptCenter.x;
		pt.y = pFace->ptEdge[0].y - pFace->ptCenter.y;
		pt.z = pFace->ptEdge[0].z - pFace->ptCenter.z;
		double len= sqrt(pt.x*pt.x + pt.y*pt.y + pt.z*pt.z);
		assert((len > 0.) && "len is less equal than 0");
		if(len <= 0.)
		{
			return false;
		}

		PSQPRIMITIVE pPrimitive= NULL;
		if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
		{
			pPrimitive->hdr.id    = ENM_ELLIPSE;
			pPrimitive->hdr.attr.f= 1;
			pPrimitive->hdr.attr.d= 2;
			pPrimitive->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());
			
			pPrimitive->body.ellipse.ptOrigin.x = pFace->ptCenter.x;
			pPrimitive->body.ellipse.ptOrigin.y = pFace->ptCenter.y;
			pPrimitive->body.ellipse.ptOrigin.z = pFace->ptCenter.z;
			if(SW_ELBOW == pFace->type)
			{
				const double nMinSize=pHSRApp->m_clsOption.GetSWElbowMinSize();
				if((pFace->radius*2) >= nMinSize)
				{
					pPrimitive->body.ellipse.nAxis[0]   = pFace->radius;
					pPrimitive->body.ellipse.nAxis[1]   = pFace->radius;
				}
				else
				{
					pPrimitive->body.ellipse.nAxis[0]   = nMinSize*0.5;
					pPrimitive->body.ellipse.nAxis[1]   = nMinSize*0.5;
				}
			}
			else if(ELBOW == pFace->type)
			{
				const double nMinSize=pHSRApp->m_clsOption.GetWeldElbowMinSize();
				if((pFace->radius*2) >= nMinSize)
				{
					pPrimitive->body.ellipse.nAxis[0]   = pFace->radius;
					pPrimitive->body.ellipse.nAxis[1]   = pFace->radius;
				}
				else
				{
					pPrimitive->body.ellipse.nAxis[0]   = nMinSize*0.5;
					pPrimitive->body.ellipse.nAxis[1]   = nMinSize*0.5;
				}
			}
			else
			{
				pPrimitive->body.ellipse.nAxis[0]   = pFace->radius;
				pPrimitive->body.ellipse.nAxis[1]   = pFace->radius;
			}
			
			double nStartAngle=RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(pt)) + nAngle*0.5;
			if(nStartAngle > 360.) nStartAngle -= 360.;
			double nEndAngle  =nStartAngle - nAngle;
			if(nEndAngle < 0) nEndAngle += 360.;

			double delta[2]={0};
			if(nStartAngle > nEndAngle)
			{
				delta[0]   = nStartAngle;
				nStartAngle= nEndAngle;
				nEndAngle  = delta[0];
			}
			delta[0] = nEndAngle - nStartAngle;
			delta[1] = 360. - delta[0];
			if(delta[0] > delta[1])
			{
				pPrimitive->body.ellipse.nStartAngle = nStartAngle;
				pPrimitive->body.ellipse.nSweepAngle = 360. - nAngle;
			}
			else
			{
				pPrimitive->body.ellipse.nStartAngle = nEndAngle;
				pPrimitive->body.ellipse.nSweepAngle = 360. - nAngle;
			}
		
			m_pShowPrimitiveKind[kind] = ListPushFront(m_pShowPrimitiveKind[kind],pPrimitive);
		}else	return false;
	}else	return false;
	
	return true;
}

//	parameter	:
//	description	: write view boundary.
//	remarks		:
//	returns		:
void CHSRExport::WriteViewBoundary()
{
	CHSRApp* pHSRApp  = CHSRApp::GetInstance();
	PLINE_T pptViewBoundary = pHSRApp->m_clsView.GetViewBoundary();
	int nViewEdge = pHSRApp->m_clsView.GetNumOfViewBoundaryEdge();
	PSQPRIMITIVE pPrimitive = NULL;

	for(int i = 0;i < nViewEdge;i++)
	{
		if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
		{
			pPrimitive->hdr.id     = ENM_LINE;
			pPrimitive->hdr.attr.d = 2;

			pPrimitive->body.line.ptStart.x = pptViewBoundary[i].ptStart.x;
			pPrimitive->body.line.ptStart.y = pptViewBoundary[i].ptStart.y;
			pPrimitive->body.line.ptStart.z = pptViewBoundary[i].ptStart.z;
			pPrimitive->body.line.ptEnd.x = pptViewBoundary[i].ptEnd.x;
			pPrimitive->body.line.ptEnd.y = pptViewBoundary[i].ptEnd.y;
			pPrimitive->body.line.ptEnd.z = pptViewBoundary[i].ptEnd.z;

			m_pShowPrimitiveKind[LAYER_VIEW] = ListPushFront(m_pShowPrimitiveKind[LAYER_VIEW],pPrimitive);
		}
	}
}

//	parameter	:
//	description	: write view boundary.
//	remarks		:
//	returns		:
void CHSRExport::WriteViewBoundary(CViewBoundary* pViewBoundray)
{
	assert(pViewBoundray && "pViewBoundary is NULL");
	CHSRApp* pHSRApp  = CHSRApp::GetInstance();

	if(pViewBoundray)
	{
		PLINE_T pptViewBoundary = pViewBoundray->GetViewBoundary();
		int nViewEdge = pViewBoundray->GetNumOfViewBoundaryEdge();

		PSQPRIMITIVE pPrimitive = NULL;
		for(int i = 0;i < nViewEdge;i++)
		{
			if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
			{
				pPrimitive->hdr.id     = ENM_LINE;
				pPrimitive->hdr.attr.d = 2;

				pPrimitive->body.line.ptStart.x = pptViewBoundary[i].ptStart.x;
				pPrimitive->body.line.ptStart.y = pptViewBoundary[i].ptStart.y;
				pPrimitive->body.line.ptStart.z = pptViewBoundary[i].ptStart.z;
				pPrimitive->body.line.ptEnd.x = pptViewBoundary[i].ptEnd.x;
				pPrimitive->body.line.ptEnd.y = pptViewBoundary[i].ptEnd.y;
				pPrimitive->body.line.ptEnd.z = pptViewBoundary[i].ptEnd.z;

				m_pShowPrimitiveKind[LAYER_VIEW] = ListPushFront(m_pShowPrimitiveKind[LAYER_VIEW],pPrimitive);
			}
		}
	}
}

/**
 * WriteArcToDWG:
 *
 * @param ret 
 * @param pStartVt 
 * @param pEndVt 
 * @return PSQPRIMITIVE 
 */
PSQPRIMITIVE CHSRExport::WriteArcToDWG(PSQPRIMITIVE ret , CHSRFace* pFace , PHSR_VERTEX pStartVt,PHSR_VERTEX pEndVt)
{
	assert(pFace && "pFace is NULL");
	assert(pStartVt && "pStartVt is NULL");
	assert(pEndVt   && "pEndVt is NULL");

	if(pFace && pStartVt && pEndVt && (pStartVt != pEndVt))
	{
		VECTOR_T vec[3]={0,};
		vec[0].dx = pStartVt->pt->x - pFace->ptCenter.x;
		vec[0].dy = pStartVt->pt->y - pFace->ptCenter.y;
		vec[0].dz = 0;
		vec[1].dx = pEndVt->pt->x - pFace->ptCenter.x;
		vec[1].dy = pEndVt->pt->y - pFace->ptCenter.y;
		vec[1].dz = 0;
		vec[2].dx = pStartVt->next->pt->x - pFace->ptCenter.x;
		vec[2].dy = pStartVt->next->pt->y - pFace->ptCenter.y;
		vec[2].dz = 0;
		
		double startDeg = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[0]));
		double endDeg   = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vec[1]));
		if(startDeg >= 360) startDeg -= 360;
		if(endDeg   >= 360) endDeg   -= 360;
		/// remove condition is 180 degree
		if((vec[0].dx*vec[2].dy - vec[0].dy*vec[2].dx) < 0) swap(endDeg , startDeg);
		
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PSQPRIMITIVE pPrim = NULL;
		if(pPrim = pHSRApp->alloc_primitive())
		{
			pPrim->hdr.id = ENM_ELLIPSE;
			pPrim->body.ellipse.ptOrigin = pFace->ptCenter;
			pPrim->body.ellipse.nAxis[0] = sqrt(vec[0].dx*vec[0].dx + vec[0].dy*vec[0].dy);
			pPrim->body.ellipse.nAxis[1] = pPrim->body.ellipse.nAxis[0];
			pPrim->body.ellipse.nStartAngle = startDeg;
			pPrim->body.ellipse.nSweepAngle = endDeg - startDeg;
			if(pPrim->body.ellipse.nSweepAngle < 0) pPrim->body.ellipse.nSweepAngle += 360;
			pPrim->body.ellipse.vecNorm.dx = 0;
			pPrim->body.ellipse.vecNorm.dy = 0;
			pPrim->body.ellipse.vecNorm.dz = 1;
			pPrim->hdr.display.lcolor = (BYTE)ATOI_T(pFace->color().c_str());

			ret = ListPushFront(ret , pPrim);
		}
	}
	
	return ret;
}

/**
	@brief	Miter 형상을 Write한다. ELBOW LAYER로 저장한다.
	@author	백흠경
*/
PSQPRIMITIVE CHSRExport::WriteMiterToDWG(PSQPRIMITIVE ret, CHSRFace *pFace)
{
	assert(pFace && "face is NULL");
	size_t nCount = 0,nTotalCnt = 0;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pFace)
	{
		PIPELINEMODE LineMode = pHSRApp->m_clsOption.GetPipeLineMode();
		if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (pFace->radius <= pHSRApp->m_clsOption.GetCriticalRadius())))
		{
			/// CENTER LINE만 WRITE한다.
			if(pFace->pCenterline)
			{
				if(pFace) WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_SIN);
			}
		}
		else
		{
			/// OUTLINE과 CENTER LINE 모두 WRITE한다.
			if(pFace->pHead)
			{
				if(pFace) ret = WriteFaceToPrimitive(ret,pFace,pFace->m_enmLayer);
			}
			
			if(pFace->pCenterline)
			{
				if(pFace) WriteVertexListToDWG(pFace , pFace->pCenterline , LAYER_ELBOW_CEN);
			}

			if(pFace->pSymboline)
			{
				if(pFace) WriteVertexListToDWG(pFace , pFace->pSymboline , pFace->m_enmLayer);
			}
		}

		return NULL;
	}

	return NULL;
}
