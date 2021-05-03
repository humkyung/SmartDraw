// TemplateElement.cpp: implementation of the CTemplateElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "TemplateElement.h"
#include "HSRReader.h"
#include "HSRElementFactory.h"

IMPLEMENTS_HSR_ELEMENT(CTemplateElement , CHSRElement , _T("TEMPLATE"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTemplateElement::CTemplateElement() : m_pHSRReader(NULL)
{
}

/**	
	@brief
*/
CTemplateElement::CTemplateElement(CHSRReader* pHSRReader)
{
	m_pHSRReader = pHSRReader;
}

CTemplateElement::~CTemplateElement()
{

}

/**	@brief	The CTemplateElement::ParseLine function
	@author	백흠경

	@param	pScanner	a parameter of type CHSRScanner*

	@history
	;2004.12.23 - convert torus element to elbow element.
	;2008.08.04 - <PIPE_BEND>에서 BEND만 있고 PIPE가 없을 경우 처리.

	@return	true = 파싱 성공 , false = 파싱 실패
*/
bool CTemplateElement::ParseLine(CHSRScanner* pScanner)
{
	assert(pScanner && "pScanner is NULL");
	bool bRet=false;

	if(pScanner)
	{
		if(0 == STRCMP_T((*pScanner->m_aryFactor)[0].c_str(), _T("<PIPE_BEND>")))
		{
			//////////////////////////////////////////////////////////////////////////
			CreateElbow(pScanner);
			/*
			CTorsElement* pTORS=NULL;
			if(pTORS = new CTorsElement){
				pTORS->ParseLine(pScanner);
				
				m_pHSRReader->m_lstTORSFormat.push_back(pTORS);
			}
			*/
			//////////////////////////////////////////////////////////////////////////

			if(pScanner->m_nFactor > 14)
			{
				int n = 1;
				for(int i = 14;i < pScanner->m_nFactor;i++)
				{
					(*pScanner->m_aryFactor)[n][0] = '\0';
					(*pScanner->m_aryFactor)[n++] = (*pScanner->m_aryFactor)[i];
				}
				pScanner->m_nFactor -= 13;
				(*pScanner->m_aryFactor)[pScanner->m_nFactor][0] = '\0';

				CHSRPipe* pPIPE1=NULL;
				if(pPIPE1 = new CHSRPipe)
				{
					///strcpy((*pScanner->m_aryFactor)[0].c_str(),"<PIPE>");
					(*pScanner->m_aryFactor)[0] = _T("<PIPE>");
					pPIPE1->ParseLine(pScanner);
				
					m_pHSRReader->m_lstElement.push_back(pPIPE1);
				}

				n=1;
				for(int i = 8;i < pScanner->m_nFactor;i++)
				{
					(*pScanner->m_aryFactor)[n][0] = '\0';
					(*pScanner->m_aryFactor)[n++] = (*pScanner->m_aryFactor)[i];
					///strcpy((*pScanner->m_aryFactor)[n++].c_str(),(*pScanner->m_aryFactor)[i]);
				}

				CHSRPipe* pPIPE2=NULL;
				if(pPIPE2 = new CHSRPipe)
				{
					///strcpy((*pScanner->m_aryFactor)[0].c_str(),"<PIPE>");
					(*pScanner->m_aryFactor)[0] = _T("<PIPE>");
					pPIPE2->ParseLine(pScanner);
				
					m_pHSRReader->m_lstElement.push_back(pPIPE2);
				}
			}
			
			bRet = true;
		}
	}

	return bRet;
}

/**
	@brief	

	@author	HumKyung.BAEK
*/
void CTemplateElement::CreateElbow(CHSRScanner *pScanner)
{
	assert(pScanner && "pScanner is NULL");
	
	if(pScanner)
	{
		POINT_T _ptOrigin={0,};
		_ptOrigin.x= ATOF_T((*pScanner->m_aryFactor)[1].c_str());
		_ptOrigin.y= ATOF_T((*pScanner->m_aryFactor)[2].c_str());
		_ptOrigin.z= ATOF_T((*pScanner->m_aryFactor)[3].c_str());
		
		double _nTorusRadius=0.,_nTubeRadius=0.;
		_nTorusRadius = ATOF_T((*pScanner->m_aryFactor)[4].c_str());	
		_nTubeRadius  = ATOF_T((*pScanner->m_aryFactor)[5].c_str());
		
		VECTOR_T _vecNorm={0,0,0},_vecAxis={0,0,0};
		_vecNorm.dx = ATOF_T((*pScanner->m_aryFactor)[6].c_str());
		_vecNorm.dy = ATOF_T((*pScanner->m_aryFactor)[7].c_str());
		_vecNorm.dz = ATOF_T((*pScanner->m_aryFactor)[8].c_str());
		_vecAxis.dx = ATOF_T((*pScanner->m_aryFactor)[9].c_str());
		_vecAxis.dy = ATOF_T((*pScanner->m_aryFactor)[10].c_str());
		_vecAxis.dz = ATOF_T((*pScanner->m_aryFactor)[11].c_str());
		
		double _nStartAng=0.,_nSweepAng=0.;
		_nStartAng = ATOF_T((*pScanner->m_aryFactor)[12].c_str());
		_nSweepAng = ATOF_T((*pScanner->m_aryFactor)[13].c_str());
		double nEndAng = ((_nStartAng + _nSweepAng) > 2*PI) ? _nStartAng + _nSweepAng - 2*PI : _nStartAng + _nSweepAng;
		
		if(fabs(_nSweepAng - 2*PI) > 0.001)
		{
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(_vecNorm,alpha,beta);
			
			VECTOR_T vecAxis=CGeometry::RotateAboutXYAxis(_vecAxis,alpha,beta);
			double nStartAng=CGeometry::GetRotatedAngleInXYPlane(vecAxis);
			
			_nStartAng = ATOF_T((*pScanner->m_aryFactor)[12].c_str());
			_nSweepAng = ATOF_T((*pScanner->m_aryFactor)[13].c_str());
			nStartAng= ((_nStartAng + nStartAng) > 2*PI) ? _nStartAng + nStartAng - 2*PI : _nStartAng + nStartAng;
			double nEndAng  = ((nStartAng + _nSweepAng) > 2*PI) ? nStartAng + _nSweepAng - 2*PI : nStartAng + _nSweepAng;
			
			POINT_T _pt[2]={0,};
			_pt[0].x = _nTorusRadius;_pt[0].y = 0.,_pt[0].z = 0.;
			_pt[1].x = _nTorusRadius;_pt[1].y = 0.,_pt[1].z = 0.;
			_pt[0].x = cos(nStartAng)*_nTorusRadius;
			_pt[0].y = sin(nStartAng)*_nTorusRadius;
			_pt[1].x = cos(nEndAng)*_nTorusRadius;
			_pt[1].y = sin(nEndAng)*_nTorusRadius;
			
			_pt[0] = CGeometry::RotateAboutYXAxis(_pt[0],-beta,-alpha);
			_pt[0].x += _ptOrigin.x;_pt[0].y += _ptOrigin.y;_pt[0].z += _ptOrigin.z;
			_pt[1] = CGeometry::RotateAboutYXAxis(_pt[1],-beta,-alpha);
			_pt[1].x += _ptOrigin.x;_pt[1].y += _ptOrigin.y;_pt[1].z += _ptOrigin.z;

			POINT_T ptMid={0,0,0};
			ptMid.x = (_pt[0].x + _pt[1].x)*0.5;
			ptMid.y = (_pt[0].y + _pt[1].y)*0.5;
			ptMid.z = (_pt[0].z + _pt[1].z)*0.5;

			double dx=0,dy=0,dz=0;
			dx = _pt[0].x - _ptOrigin.x;dy = _pt[0].y - _ptOrigin.y;dz = _pt[0].z - _ptOrigin.z;
			const double d=sqrt(dx*dx + dy*dy + dz*dz);
			const double l=d/(cos(_nSweepAng*0.5));

			VECTOR_T vecDir={ptMid.x - _ptOrigin.x,ptMid.y - _ptOrigin.y,ptMid.z - _ptOrigin.z};
			CMath::NormalizeVector(vecDir);
			
			POINT_T _ptCross={0,0,0};
			_ptCross.x = _ptOrigin.x + vecDir.dx*l;
			_ptCross.y = _ptOrigin.y + vecDir.dy*l;
			_ptCross.z = _ptOrigin.z + vecDir.dz*l;

			CElbowElement* pEnt=new CElbowElement(CElbowElement::BW,_ptOrigin,_nTorusRadius,_nTubeRadius);
			if(NULL != pEnt)
			{
				pEnt->SetPosition(_pt[0],_pt[1]);
				pEnt->SetCrossPoint(_ptCross);
				pEnt->CalcVolume();
				m_pHSRReader->m_lstELBOWFormat.push_back(pEnt);
			}
		}	
	}
}
