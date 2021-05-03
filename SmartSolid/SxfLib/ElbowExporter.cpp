// ElbowExporter.cpp: implementation of the CElbowExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <math.h>
#include "ElbowExporter.h"

#include <fstream>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElbowExporter::CElbowExporter()
{

}

CElbowExporter::~CElbowExporter()
{

}

/*
	@brief	The CNSQExporter::WriteElbow function

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&
  
	@remarks\n
	- 2003.07.31 - support extended elbow.
	- 2007.03.20 - bug fix to write pipe element
	- 2007.04.11 - nSkipLevel에 위치한 엔터티는 파일로 저장하지 않는다.
	    
	@return	void	
*/
void CElbowExporter::WriteElbow(ofstream& ofile , PSQPRIMITIVE pPrimitive, long &nSeq, long &nGroup , const int& nSkipLevel)
{
	assert(pPrimitive && "pPrimitive is NULL");
	
	if(pPrimitive)
	{
		if(IsSweepSurf(pPrimitive->body.pPrimitive))
		{
			WriteSweepElbow(ofile , pPrimitive->body.pPrimitive , nSeq , nGroup , nSkipLevel);
			return;
		}
		
		POINT_T pt[2]={0},ptCross={0},ptCenter={0};
		double  nTubeRadius=0.,nElbowRadius=0.;
		
		PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;
		if(pElement && (SQ_POINTS == pElement->hdr.id))
		{
			vector<POINT_T>::iterator itr=pElement->body.pvtPoints->begin();
			ptCross = *itr;
		}
		pElement = pElement->next;
		
		while(pElement)
		{
			if(pElement->hdr.pLayer && (nSkipLevel == pElement->hdr.pLayer[0]))
			{
				pElement = pElement->next;
				continue;
			}

			if(pElement && (SQ_CONE == pElement->hdr.id))
			{
				POINT_T ptCenter[2]={0};
				double  nRadius[2]={0.};
				
				nRadius[0] = pElement->body.cone.nRadius[0];
				nRadius[1] = pElement->body.cone.nRadius[1];
				ptCenter[0]= pElement->body.cone.ptOrigin[0];
				ptCenter[1]= pElement->body.cone.ptOrigin[1];
				
				if(fabs(nRadius[0] - nRadius[1]) < 0.00001)
				{
					ofile << _T("PIPE,") << nRadius[0] << _T(",");
					ofile << ptCenter[0].x << _T(",") << ptCenter[0].y << _T(",") << ptCenter[0].z << _T(",");
					ofile << ptCenter[1].x << _T(",") << ptCenter[1].y << _T(",") << ptCenter[1].z << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
				else
				{
					ofile << _T("CONE,");
					ofile << nRadius[0] << _T(",") << nRadius[1] << _T(",");
					ofile << ptCenter[0].x << _T(",") << ptCenter[0].y << _T(",") << ptCenter[0].z << _T(",");
					ofile << ptCenter[1].x << _T(",") << ptCenter[1].y << _T(",") << ptCenter[1].z << _T(",");
					ofile << _T("0,0,0,") << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
				pElement = pElement->next;
			}
			//else if(IsInsulationElbow(pElement)) pElement = pElement->next;
			else if(pElement && (SQ_SURFACE == pElement->hdr.id) && (SURF_OF_REVOLUTION == pElement->body.pSurface->nType))
			{
				SQSURFACE::PSURFELM_T PSURFELM_T=pElement->body.pSurface->pElement;
				
				if(PSURFELM_T->pBoundaryCurve && (ENM_ELLIPSE == PSURFELM_T->pBoundaryCurve->hdr.id))
				{
					nTubeRadius = PSURFELM_T->pBoundaryCurve->body.ellipse.nAxis[0];
					pt[0] = PSURFELM_T->pBoundaryCurve->body.ellipse.ptOrigin;
					pt[1] = PSURFELM_T->pBoundaryCurve->next->body.ellipse.ptOrigin;
				}
				if(PSURFELM_T->pRuleCurve && (ENM_ELLIPSE == PSURFELM_T->pRuleCurve->hdr.id))
				{
					ptCenter    = PSURFELM_T->pRuleCurve->body.ellipse.ptOrigin;
					//nElbowRadius= PSURFELM_T->pRuleCurve->body.ellipse.nAxis[0];
					
					double dx=pt[0].x - ptCenter.x;
					double dy=pt[0].y - ptCenter.y;
					double dz=pt[0].z - ptCenter.z;
					nElbowRadius  = sqrt(dx*dx + dy*dy + dz*dz);
				}
				
				if(nTubeRadius > 0.)
				{
					ofile << _T("ELBOW,");
					ofile << pt[0].x << _T(",") << pt[0].y << _T(",") << pt[0].z << _T(",");
					ofile << pt[1].x << _T(",") << pt[1].y << _T(",") << pt[1].z << _T(",");
					ofile << ptCross.x << _T(",") << ptCross.y << _T(",") << ptCross.z << _T(",");
					ofile << ptCenter.x << _T(",") << ptCenter.y << _T(",") << ptCenter.z << _T(",");
					ofile << nElbowRadius << _T(",") << nTubeRadius << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
				
				if(IsInsulationElbow(pElement)) pElement = pElement->next;
				pElement = pElement->next;
			}
			else	pElement = pElement->next;
		}
	}
}

/**
 * IsInsulationElbow:
 *
 * @param pElement 
 * @return bool 
 */
bool CElbowExporter::IsInsulationElbow(PSQPRIMITIVE pElement)
{
	assert(pElement && "pElement is NULL");

	if(pElement)
	{
		if((SQ_SURFACE == pElement->hdr.id) && (SURF_OF_REVOLUTION == pElement->body.pSurface->nType))
		{
			if(pElement->next && (SQ_SURFACE == pElement->next->hdr.id) && (SURF_OF_REVOLUTION == pElement->next->body.pSurface->nType))
			{
				return true;
			}
		}
	}

	return false;
}

bool CElbowExporter::IsSweepSurf(PSQPRIMITIVE pPrimitive) const
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive && (NULL != pPrimitive->next) && (NULL != pPrimitive->next->next))
	{
		PSQPRIMITIVE pElem = pPrimitive->next->next;
		return ((SQ_SURFACE == pElem->hdr.id) && (SWEEP_SURF == pElem->body.pSurface->nType));
	}

	return false;
}

/**	\brief	The CreateEllipseSegments function
	\date	2006.8.21
	\param	pts	a parameter of type POINT_T*
	\param	ellipse	a parameter of type const ELLIPSE_T&

	\return	void

	
*/
void CreateEllipseSegments(POINT_T* pts , const ELLIPSE_T& ellipse)
{
	assert(pts && "pts is NULL");
	static POINT_T __pt[64] = {0 ,};

	if(pts)
	{
		memset(__pt , 0 , sizeof(POINT_T) * 64);

		double nStartAng = DEG2RAD(ellipse.nStartAngle);
		double nSweepAng = DEG2RAD(ellipse.nSweepAngle);
		double nStep=nSweepAng / 16;
		double c=cos(nStep);
		double s=sin(nStep);
		double ab=1.,ba=1.;
		if((0. != ellipse.nAxis[0]) && (0. != ellipse.nAxis[1]))
		{
			ab = ellipse.nAxis[0] / ellipse.nAxis[1];
			ba = ellipse.nAxis[1] / ellipse.nAxis[0];
		}
		__pt[0].x = ellipse.nAxis[0]*cos(nStartAng);
		__pt[0].y = ellipse.nAxis[0]*sin(nStartAng);
		__pt[0].z = 0.;
		int i = 0;
		for(i = 1;i < 16;i++)
		{
			__pt[i].x = __pt[i-1].x*c - ab*__pt[i-1].y*s;
			__pt[i].y = ba * __pt[i-1].x*s + __pt[i-1].y*c;
			__pt[i].z = 0.;
		}
		__pt[i].x = ellipse.nAxis[0] * cos(nStartAng + nSweepAng);
		__pt[i].y = ellipse.nAxis[1] * sin(nStartAng + nSweepAng);
		__pt[i].z = 0.;
		
		double nRotateAng=DEG2RAD(ellipse.nRotate);
		c=cos(nRotateAng);
		s=sin(nRotateAng);
		for(i = 0;i < 16 + 1;i++)
		{
			const double x = __pt[i].x*c - __pt[i].y*s;
			const double y = __pt[i].x*s + __pt[i].y*c;
			__pt[i].x = x;
			__pt[i].y = y;
		}
		
		double alpha=0.,beta=0;
		CGeometry::GetRotatedAngleOfAxis(ellipse.vecNorm,alpha,beta);
		for(i = 0;i < 16 + 1;i++)
		{
			__pt[i] = CGeometry::RotateAboutYXAxis(__pt[i],-beta,-alpha);
			__pt[i].x += ellipse.ptOrigin.x;
			__pt[i].y += ellipse.ptOrigin.y;
			__pt[i].z += ellipse.ptOrigin.z;
		}

		memcpy(pts , __pt , sizeof(POINT_T) * 17);
	}
}

/**	
	@brief	write sweep eblow
	
	@author	humkyung

	@date	2003.08.21
	@date	2011.08.24 - skip to write a element on skip level
	
	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long &
	@param	nGroup	a parameter of type long &

	@return	void
*/
void CElbowExporter::WriteSweepElbow(ofstream& ofile , PSQPRIMITIVE pPrimitive , long &nSeq, long &nGroup , const int& iSkipLevel)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		PSQPRIMITIVE pElement = pPrimitive->next; /// skip pointstring

		double nRadius = 0;
		POINT_T origin[2] = {0, };
		double  radius[2] = {0 ,};
		for(PSQPRIMITIVE ptr = pElement;ptr;ptr = ptr->next)
		{
			/// skip to write a element on skip level - 2011.08.24 added by humkyung
			if(ptr->hdr.pLayer && (iSkipLevel == ptr->hdr.pLayer[0])) continue;
			/// up to here

			SQSURFACE::PSURFELM_T PSURFELM_T = ptr->body.pSurface->pElement;
			
			POINT_T pts[2][32] = {0 ,};
			if(PSURFELM_T->pBoundaryCurve && (ENM_ELLIPSE == PSURFELM_T->pBoundaryCurve->hdr.id))
			{
				nRadius = PSURFELM_T->pBoundaryCurve->body.ellipse.nAxis[0];
				origin[0] = PSURFELM_T->pBoundaryCurve->body.ellipse.ptOrigin;
				CreateEllipseSegments(pts[0] , PSURFELM_T->pBoundaryCurve->body.ellipse);
			}
			if(PSURFELM_T->pBoundaryCurve->next && (ENM_ELLIPSE == PSURFELM_T->pBoundaryCurve->next->hdr.id))
			{
				origin[1] = PSURFELM_T->pBoundaryCurve->next->body.ellipse.ptOrigin;
				CreateEllipseSegments(pts[1] , PSURFELM_T->pBoundaryCurve->next->body.ellipse);
			}

			ofile << "SWEEP_ELBOW=" << std::endl;
			ofile << "{" << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			ofile << "LHS_ORIGIN=" << origin[0].x << "," << origin[0].y << "," << origin[0].z << "," << nRadius << std::endl;
			for(int i = 0;i < 16;i++)
			{
				ofile << "LHS=" << pts[0][i].x << "," << pts[0][i].y << "," << pts[0][i].z << std::endl;
			}
			ofile << "RHS_ORIGIN=" << origin[1].x << "," << origin[1].y << "," << origin[1].z << "," << nRadius << std::endl;
			for(int i = 0;i < 16;i++)
			{
				ofile << "RHS=" << pts[1][i].x << "," << pts[1][i].y << "," << pts[1][i].z << std::endl;
			}
			ofile << "}" << std::endl;
		}
	}
}
