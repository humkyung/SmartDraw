#include "wedge.h"
#include "face3d.h"
#include "surface3d.h"
#include "body.h"

CBody::CBody()
{
    	m_pSurface = NULL;
	
	m_pPrev = NULL;
	m_pNext = NULL;
}

CBody::~CBody()
{
}

/**	\brief	The CBody::operator+= function

	\param	pSurface	a parameter of type CSurface3D*

	\return	void	
*/
void CBody::operator+=(CSurface3D* pSurface)
{
	if(m_pSurface)
	{
	    	CSurface3D* ptr=NULL;
		for(ptr = m_pSurface;ptr && ptr->Next();ptr = ptr->Next());
		if(ptr)	     ptr->m_pNext = pSurface;
		if(pSurface) pSurface->m_pPrev = ptr;
	}
	else
	{
	    	m_pSurface= pSurface;
	}
}

/**	\brief	The CBody::Prev function


	\return	CBody*	
*/
CBody* CBody::Prev()
{
    	return m_pPrev;
}

/**	\brief	The CBody::Next function


	\return	CBody*	
*/
CBody* CBody::Next()
{
    	return m_pNext;
}

/**	\brief	The CBody::DumpToFile function

	\param	pFilePath	a parameter of type const char*

	\return	void	
*/
void CBody::DumpToFile(const char* pFilePath)
{
    	assert(pFilePath && "pFilePath is invalid");

	if(pFilePath)
	{
		PSQPRIMITIVE pPrimitive = (PSQPRIMITIVE)malloc(sizeof(SQPRIMITIVE));
		memset(pPrimitive,0,sizeof(SQPRIMITIVE));
		pPrimitive->hdr.id = ENM_LINE;

	    	FILE* fp=NULL;
		if(fp = fopen(pFilePath,"wb"))
		{
		    	CSurface3D* pSurface=NULL;
			for(pSurface = m_pSurface;pSurface;pSurface = pSurface->Next())
			{
			    	CFace3D *pFace=NULL;
				for(pFace = pSurface->GetFace();pFace;pFace = pFace->Next())
				{
				    	CWEdge* pWEdge=NULL;
					for(pWEdge = pFace->GetWEdge();pWEdge;pWEdge = pWEdge->Next())
					{
						pPrimitive->hdr.attr.d   = 3;
						pPrimitive->body.line.ptStart.x = pWEdge->m_pvtxStart->x;
						pPrimitive->body.line.ptStart.y = pWEdge->m_pvtxStart->y;
						pPrimitive->body.line.ptStart.z = pWEdge->m_pvtxStart->z;
						pPrimitive->body.line.ptEnd.x = pWEdge->m_pvtxEnd->x;
						pPrimitive->body.line.ptEnd.y = pWEdge->m_pvtxEnd->y;
						pPrimitive->body.line.ptEnd.z = pWEdge->m_pvtxEnd->z;
					    	fprintf(fp,"%lf,%lf,%lf %lf,%lf,%lf\r\n",pWEdge->m_pvtxStart->x,pWEdge->m_pvtxStart->y,pWEdge->m_pvtxStart->z,
							pWEdge->m_pvtxEnd->x,pWEdge->m_pvtxEnd->y,pWEdge->m_pvtxEnd->z);
					}
				}
			}
			fclose(fp);
		}
	}
}