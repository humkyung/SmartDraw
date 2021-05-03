// PrePIPEFormat.cpp: implementation of the CHSRPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "HSRScanner.h"

#include "HSRPipe.h"

CHSRPipe::ENM_SINGLE_LINE_OPTION CHSRPipe::m_enmSingleLineOption = CHSRPipe::EQUAL_OD;
double CHSRPipe::m_nFixedSize = 0.f;

Begin_Enum_String(CHSRPipe::ENM_SINGLE_LINE_OPTION)
{
	Enum_String(CHSRPipe::EQUAL_OD);
	Enum_String(CHSRPipe::FIXED_SIZE);
}
End_Enum_String;

IMPLEMENTS_HSR_ELEMENT(CHSRPipe , CHSRElement , _T("__PIPE_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CHSRPipe::CHSRPipe()
{
	m_nSegments = 20;

	m_nPIPEs    = 0;
	m_nRadius[0]= 0.;
	m_pt[0].x = m_pt[0].y = m_pt[0].z = 0.;
	m_pt[1].x = m_pt[1].y = m_pt[1].z = 0.;

	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;
}

CHSRPipe::~CHSRPipe()
{

}

#ifdef VER_03
/**	\brief	The CHSRPipe::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
static double lastSize=-1.f;
bool CHSRPipe::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;
		if((0 == STRCMP_T(pHSRScanner->m_aryFactor[0],"<PIPE>"))){
			m_nPIPEs = 1;
			
			if(pVal = pHSRScanner->Val("layer")) m_strLayer = pVal;
			if(pVal = pHSRScanner->Val("radius")) m_nRadius[0] = ATOF_T(pVal);
			if(m_nRadius[0] > 1000.f)
			{ /// error size
				if(lastSize > 0)
				{
					m_nRadius[0] = lastSize;
				}
			}else	lastSize = m_nRadius[0];

			if(pVal = pHSRScanner->Val("pt1"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
			}
			if(pVal = pHSRScanner->Val("pt2"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
			}

			Volume();
			bRet = true;
		}

	}

	return bRet;
}
#else
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CHSRPipe::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		if((0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("PIPE"))) && 
		   (pHSRScanner->m_aryFactor->size() > 22))
		{
			m_nPIPEs = 3;
			
			m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			
			m_nRadius[1] = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
			m_pt[2].x = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
			m_pt[2].y = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
			m_pt[2].z = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
			m_pt[3].x = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
			m_pt[3].y = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
			m_pt[3].z = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());
			
			m_nRadius[2] = ATOF_T((*pHSRScanner->m_aryFactor)[15].c_str());
			m_pt[4].x = ATOF_T((*pHSRScanner->m_aryFactor)[16].c_str());
			m_pt[4].y = ATOF_T((*pHSRScanner->m_aryFactor)[17].c_str());
			m_pt[4].z = ATOF_T((*pHSRScanner->m_aryFactor)[18].c_str());
			m_pt[5].x = ATOF_T((*pHSRScanner->m_aryFactor)[19].c_str());
			m_pt[5].y = ATOF_T((*pHSRScanner->m_aryFactor)[20].c_str());
			m_pt[5].z = ATOF_T((*pHSRScanner->m_aryFactor)[21].c_str());

			if(pHSRScanner->m_nFactor > 22)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[22]);
			}

			Volume();
			bRet = true;
		}
		else if((0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("<MUPI>"))) && (7 == pHSRScanner->m_aryFactor->size()))
		{
			m_nPIPEs = 1;
			
			m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

			Volume();
			bRet = true;
		}
		else if((0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("PIPE"))))
		{
			m_nPIPEs = 1;
			
			m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

			if(pHSRScanner->m_nFactor > 8)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[8]);
			}

			Volume();
			bRet = true;
		}

	}

	return bRet;
}
#endif

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHSRPipe::Volume()
{
	m_vol.x[0] = m_vol.x[1] = m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_pt[0].z;
	for(int nCount=0;nCount < m_nPIPEs;nCount++)
	{
		VECTOR_T vecAxis=
		{	
			m_pt[nCount*2 + 1].x - m_pt[nCount*2].x,
			m_pt[nCount*2 + 1].y - m_pt[nCount*2].y,
			m_pt[nCount*2 + 1].z - m_pt[nCount*2].z
		};
		CMath::NormalizeVector(vecAxis);
		
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.};
		vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
		vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);
		
		POINT_T ptEDGE[8]={0.,};
		ptEDGE[0].x = m_pt[nCount*2].x - vecX.dx*m_nRadius[nCount] - vecY.dx*m_nRadius[nCount];
		ptEDGE[0].y = m_pt[nCount*2].y - vecX.dy*m_nRadius[nCount] - vecY.dy*m_nRadius[nCount];
		ptEDGE[0].z = m_pt[nCount*2].z - vecX.dz*m_nRadius[nCount] - vecY.dz*m_nRadius[nCount];
		ptEDGE[1].x = m_pt[nCount*2].x - vecX.dx*m_nRadius[nCount] + vecY.dx*m_nRadius[nCount];
		ptEDGE[1].y = m_pt[nCount*2].y - vecX.dy*m_nRadius[nCount] + vecY.dy*m_nRadius[nCount];
		ptEDGE[1].z = m_pt[nCount*2].z - vecX.dz*m_nRadius[nCount] + vecY.dz*m_nRadius[nCount];
		ptEDGE[2].x = m_pt[nCount*2].x + vecX.dx*m_nRadius[nCount] + vecY.dx*m_nRadius[nCount];
		ptEDGE[2].y = m_pt[nCount*2].y + vecX.dy*m_nRadius[nCount] + vecY.dy*m_nRadius[nCount];
		ptEDGE[2].z = m_pt[nCount*2].z + vecX.dz*m_nRadius[nCount] + vecY.dz*m_nRadius[nCount];
		ptEDGE[3].x = m_pt[nCount*2].x + vecX.dx*m_nRadius[nCount] - vecY.dx*m_nRadius[nCount];
		ptEDGE[3].y = m_pt[nCount*2].y + vecX.dy*m_nRadius[nCount] - vecY.dy*m_nRadius[nCount];
		ptEDGE[3].z = m_pt[nCount*2].z + vecX.dz*m_nRadius[nCount] - vecY.dz*m_nRadius[nCount];
		ptEDGE[4].x = m_pt[nCount*2 + 1].x - vecX.dx*m_nRadius[nCount] - vecY.dx*m_nRadius[nCount];
		ptEDGE[4].y = m_pt[nCount*2 + 1].y - vecX.dy*m_nRadius[nCount] - vecY.dy*m_nRadius[nCount];
		ptEDGE[4].z = m_pt[nCount*2 + 1].z - vecX.dz*m_nRadius[nCount] - vecY.dz*m_nRadius[nCount];
		ptEDGE[5].x = m_pt[nCount*2 + 1].x - vecX.dx*m_nRadius[nCount] + vecY.dx*m_nRadius[nCount];
		ptEDGE[5].y = m_pt[nCount*2 + 1].y - vecX.dy*m_nRadius[nCount] + vecY.dy*m_nRadius[nCount];
		ptEDGE[5].z = m_pt[nCount*2 + 1].z - vecX.dz*m_nRadius[nCount] + vecY.dz*m_nRadius[nCount];
		ptEDGE[6].x = m_pt[nCount*2 + 1].x + vecX.dx*m_nRadius[nCount] + vecY.dx*m_nRadius[nCount];
		ptEDGE[6].y = m_pt[nCount*2 + 1].y + vecX.dy*m_nRadius[nCount] + vecY.dy*m_nRadius[nCount];
		ptEDGE[6].z = m_pt[nCount*2 + 1].z + vecX.dz*m_nRadius[nCount] + vecY.dz*m_nRadius[nCount];
		ptEDGE[7].x = m_pt[nCount*2 + 1].x + vecX.dx*m_nRadius[nCount] - vecY.dx*m_nRadius[nCount];
		ptEDGE[7].y = m_pt[nCount*2 + 1].y + vecX.dy*m_nRadius[nCount] - vecY.dy*m_nRadius[nCount];
		ptEDGE[7].z = m_pt[nCount*2 + 1].z + vecX.dz*m_nRadius[nCount] - vecY.dz*m_nRadius[nCount];
		
		for(int i=0;i < 8;i++)
		{
			if(m_vol.x[0] > ptEDGE[i].x) m_vol.x[0] = ptEDGE[i].x;
			if(m_vol.x[1] < ptEDGE[i].x) m_vol.x[1] = ptEDGE[i].x;
			if(m_vol.y[0] > ptEDGE[i].y) m_vol.y[0] = ptEDGE[i].y;
			if(m_vol.y[1] < ptEDGE[i].y) m_vol.y[1] = ptEDGE[i].y;
			if(m_vol.z[0] > ptEDGE[i].z) m_vol.z[0] = ptEDGE[i].z;
			if(m_vol.z[1] < ptEDGE[i].z) m_vol.z[1] = ptEDGE[i].z;
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CHSRPipe::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

#ifdef VER_03
CHSRFace* CHSRPipe::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	static POINT_T ptEDGE[36]={0.,};
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pViewFormat)
	{
		///////////////////////////////
		//--> only top,front except iso
		VOLUME_T volModel=pViewFormat->GetModelVolume();
		POINT_T ptMin={volModel.x[0],volModel.y[0],volModel.z[0]};
		POINT_T ptMax={volModel.x[1],volModel.y[1],volModel.z[1]};
		ptMin = pViewFormat->MODEL2VIEW(ptMin);
		ptMax = pViewFormat->MODEL2VIEW(ptMax);
		double zhigh=(ptMax.z > ptMin.z) ? ptMax.z : ptMin.z;
		double zlow =(ptMin.z < ptMax.z) ? ptMin.z : ptMax.z;
		//<--
		///////////////////////////////

		POINT_T pt[2]={0.,};
		double  nRadius=0.,nShapeRadius=0.;
		bool   clipping[2] = {false , false};
		double clipping_z[2]={0,};
		
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
		for(int count=0;count < m_nPIPEs;count++)
		{
			pSide = pTop = pBottom = NULL;
			
			pt[0]  = m_pt[count*2];
			pt[1]  = m_pt[count*2 + 1];
			pt[0]  = pViewFormat->MODEL2VIEW(pt[0]);
			pt[1]  = pViewFormat->MODEL2VIEW(pt[1]);
			nRadius= pViewFormat->MODEL2VIEW(m_nRadius[count]);
			nShapeRadius = GetShapeRadius(pViewFormat,nRadius);
			clipping[0] = clipping[1] = false;
			
			VECTOR_T vecAxis={pt[1].x - pt[0].x,pt[1].y - pt[0].y,pt[1].z - pt[0].z};
			if(!CMath::NormalizeVector(vecAxis)) return NULL;
			ClipAxisByElevation(pt[0] , pt[1] , clipping[0] , clipping[1] , clipping_z[0] , clipping_z[1] , vecAxis , zhigh , zlow);
						
			VECTOR_T vec={0.},vecZ={0.,0.,1.};
			vec.dx=pt[1].x - pt[0].x;
			vec.dy=pt[1].y - pt[0].y;
			vec.dz=pt[1].z - pt[0].z;
			if(!CMath::NormalizeVector(vec)) return NULL;
			//assert((true == b) && "pipe2");
			double nDot=CMath::DotProduct(vec,vecZ);
			
			if(fabs(fabs(nDot) - 1) < 0.001)
			{ // vertical to X-Y plane
				POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
				if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nShapeRadius,vec,m_nSegments))
				{
					pTop->m_enmLayer = LAYER_PIPE;
					pTop->type    = PIPE;
					pTop->id      = nID++;
					pTop->ele     = HSR_CIRCLE_SHAPE;
					pTop->ptCenter= ptOrigin;
					pTop->radius  = nRadius;
				}
			}
			else if(fabs(nDot) < 0.001)
			{	// place on X-Y plane
				VECTOR_T vecCross={0};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				bool b = CMath::NormalizeVector(vecCross);
				assert((true == b) && "pipe3");
				
				ptEDGE[0].x = pt[0].x + vecCross.dx*nShapeRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nShapeRadius;
				ptEDGE[0].z = (pt[0].z + nRadius) + vecCross.dz*nShapeRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nShapeRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nShapeRadius;
				ptEDGE[1].z = (pt[0].z + nRadius) - vecCross.dz*nShapeRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nShapeRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nShapeRadius;
				ptEDGE[2].z = (pt[1].z + nRadius) - vecCross.dz*nShapeRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nShapeRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nShapeRadius;
				ptEDGE[3].z = (pt[1].z + nRadius) + vecCross.dz*nShapeRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					pSide->m_enmLayer = LAYER_PIPE;
					pSide->type  = PIPE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					
					pSide->pCenterline = CHSRFace::CreateLine(2,pt);
					pSide->m_enmLayer = LAYER_PIPE;
				}
			}
			else
			{	/// slope pipe
				VECTOR_T vecCross={0};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				if(!CMath::NormalizeVector(vecCross)) return pRet;

				ptEDGE[0].x = pt[0].x + vecCross.dx*nShapeRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nShapeRadius;
				ptEDGE[0].z = pt[0].z + vecCross.dz*nShapeRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nShapeRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nShapeRadius;
				ptEDGE[1].z = pt[0].z - vecCross.dz*nShapeRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nShapeRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nShapeRadius;
				ptEDGE[2].z = pt[1].z - vecCross.dz*nShapeRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nShapeRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nShapeRadius;
				ptEDGE[3].z = pt[1].z + vecCross.dz*nShapeRadius;
				ptEDGE[4] = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					pSide->m_enmLayer = LAYER_PIPE;
					pSide->type  = PIPE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					
					pSide->pCenterline = CHSRFace::CreateLine(2,pt);
					pSide->m_enmLayer = LAYER_PIPE;
				}

				if(pTop=CHSRFace::CreateCircleFace(pt[0],nShapeRadius,vec,m_nSegments))
				{
					if(true == clipping[0])
					{
						for(PVERTEX pvt = pTop->pHead; pvt ; pvt = pvt->next)
						{
							const t = (clipping_z[0] - pvt->pt->z) / vec.dz;
							pvt->pt->x += vec.dx*t;
							pvt->pt->y += vec.dy*t;
							pvt->pt->z  = clipping_z[0];
						}
					}
					pTop->m_enmLayer = LAYER_PIPE;
					pTop->type    = PIPE;
					pTop->id      = nID++;
					pTop->ele     = HSR_POLYGON_SHAPE;
					pTop->ptCenter= pt[0];
					pTop->radius  = nRadius;
					pTop->m_enmLayer = LAYER_PIPE;
				}

				if(pBottom=CHSRFace::CreateCircleFace(pt[1],nShapeRadius,vec,m_nSegments))
				{
					if(true == clipping[1])
					{
						for(PVERTEX pvt = pBottom->pHead; pvt ; pvt = pvt->next)
						{
							const t = (clipping_z[1] - pvt->pt->z) / vec.dz;
							pvt->pt->x += vec.dx*t;
							pvt->pt->y += vec.dy*t;
							pvt->pt->z  = clipping_z[1];
						}
					}
					pBottom->m_enmLayer = LAYER_PIPE;
					pBottom->type    = PIPE;
					pBottom->id      = nID++;
					pBottom->ele     = HSR_POLYGON_SHAPE;
					pBottom->ptCenter= pt[1];
					pBottom->radius  = nRadius;
					pBottom->m_enmLayer = LAYER_PIPE;
				}
			}
			pRet = pHSRApp->m_clsPreProcessor.Run(pRet,PIPE,pTop,pSide,pBottom);
		}
	}

	return pRet;
}
#else
/**	
	@brief	create pipe faces

	@param	pViewFormat	pointer of view associated with this
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CHSRPipe::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	static POINT_T ptEDGE[36];
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pViewFormat)
	{
		///////////////////////////////
		//--> only top,front except iso
		CIsVolume volModel=pViewFormat->GetModelVolume();
		POINT_T ptMin={volModel.minx() , volModel.miny() , volModel.minz()};
		POINT_T ptMax={volModel.maxx() , volModel.maxy() , volModel.maxz()};
		ptMin = pViewFormat->MODEL2VIEW(ptMin);
		ptMax = pViewFormat->MODEL2VIEW(ptMax);
		const double zhigh=(ptMax.z > ptMin.z) ? ptMax.z : ptMin.z;
		const double zlow =(ptMin.z < ptMax.z) ? ptMin.z : ptMax.z;
		//<--
		///////////////////////////////
		
		POINT_T pt[2]={0.,};
		bool   clipping[2] = {false , false};
		double clipping_z[2]={0,};
		double  nRadius=0.,nShapeRadius=0.;
		
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
		for(int count=0;count < m_nPIPEs;count++)
		{
			pSide = pTop = pBottom = NULL;
			
			pt[0]  = m_pt[count*2];
			pt[1]  = m_pt[count*2 + 1];
			pt[0]  = pViewFormat->MODEL2VIEW(pt[0]);
			pt[1]  = pViewFormat->MODEL2VIEW(pt[1]);
			nRadius= pViewFormat->MODEL2VIEW(m_nRadius[count]);
			
			nShapeRadius= GetShapeRadius(pViewFormat,nRadius);
			clipping[0] = false;
			clipping[1] = false;
			
			VECTOR_T vecAxis={pt[1].x - pt[0].x,pt[1].y - pt[0].y,pt[1].z - pt[0].z};
			if(!CMath::NormalizeVector(vecAxis)) return NULL;
			ClipAxisByElevation(pt[0] , pt[1] , clipping[0] , clipping[1] , clipping_z[0] , clipping_z[1] , vecAxis , zhigh , zlow);
			
			VECTOR_T vec={0.},vecZ={0.,0.,1.};
			vec.dx=pt[1].x - pt[0].x;
			vec.dy=pt[1].y - pt[0].y;
			vec.dz=pt[1].z - pt[0].z;
			if(!CMath::NormalizeVector(vec)) return NULL;
			double nDot=CMath::DotProduct(vec,vecZ);
			
			if(fabs(fabs(nDot) - 1) < 0.001)
			{ // vertical to X-Y plane
				POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
				if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nShapeRadius,vec,m_nSegments))
				{
					pTop->type    = PIPE;
					pTop->id      = nID++;
					pTop->ele     = HSR_CIRCLE_SHAPE;
					pTop->ptCenter= ptOrigin;
					pTop->radius  = nRadius;
					pTop->m_enmLayer = LAYER_PIPE;
					pTop->SetColor(*m_pstrColor);
				}
			}
			else if(fabs(nDot) < 0.001)
			{	/// place on X-Y plane
				VECTOR_T vecCross={0};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				CMath::NormalizeVector(vecCross);
				
				ptEDGE[0].x = pt[0].x + vecCross.dx*nShapeRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nShapeRadius;
				ptEDGE[0].z = (pt[0].z + nShapeRadius) + vecCross.dz*nShapeRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nShapeRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nShapeRadius;
				ptEDGE[1].z = (pt[0].z + nShapeRadius) - vecCross.dz*nShapeRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nShapeRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nShapeRadius;
				ptEDGE[2].z = (pt[1].z + nShapeRadius) - vecCross.dz*nShapeRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nShapeRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nShapeRadius;
				ptEDGE[3].z = (pt[1].z + nShapeRadius) + vecCross.dz*nShapeRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					pSide->type  = PIPE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					
					pSide->pCenterline = CHSRFace::CreateLine(2 , pt);
					pSide->m_enmLayer = LAYER_PIPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{	/// slope pipe
				VECTOR_T vecCross={0,};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				if(!CMath::NormalizeVector(vecCross)) return NULL;

				ptEDGE[0].x = pt[0].x + vecCross.dx*nShapeRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nShapeRadius;
				ptEDGE[0].z = pt[0].z + vecCross.dz*nShapeRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nShapeRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nShapeRadius;
				ptEDGE[1].z = pt[0].z - vecCross.dz*nShapeRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nShapeRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nShapeRadius;
				ptEDGE[2].z = pt[1].z - vecCross.dz*nShapeRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nShapeRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nShapeRadius;
				ptEDGE[3].z = pt[1].z + vecCross.dz*nShapeRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					pSide->type  = PIPE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
					
					pSide->pCenterline = CHSRFace::CreateLine(2,pt);
					pSide->m_enmLayer = LAYER_PIPE;
				}

				if(pTop = CHSRFace::CreateCircleFace(pt[0],nShapeRadius,vec,m_nSegments))
				{
					if(true == clipping[0])
					{
						for(PHSR_VERTEX pvt = pTop->pHead; pvt ; pvt = pvt->next)
						{
							const double t = (clipping_z[0] - pvt->pt->z) / vec.dz;
							pvt->pt->x += vec.dx*t;
							pvt->pt->y += vec.dy*t;
							pvt->pt->z  = clipping_z[0];
						}
					}
					pTop->type    = PIPE;
					pTop->id      = nID++;
					pTop->ele     = HSR_POLYGON_SHAPE;
					pTop->ptCenter= pt[0];
					pTop->radius  = nRadius;
					pTop->m_enmLayer = LAYER_PIPE;
					pTop->SetColor(*m_pstrColor);
				}

				if(pBottom=CHSRFace::CreateCircleFace(pt[1],nShapeRadius,vec,m_nSegments))
				{
					if(true == clipping[1])
					{
						for(PHSR_VERTEX pvt = pBottom->pHead; pvt ; pvt = pvt->next)
						{
							const double t = (clipping_z[1] - pvt->pt->z) / vec.dz;
							pvt->pt->x += vec.dx*t;
							pvt->pt->y += vec.dy*t;
							pvt->pt->z  = clipping_z[1];
						}
					}
					pBottom->type    = PIPE;
					pBottom->id      = nID++;
					pBottom->ele     = HSR_POLYGON_SHAPE;
					pBottom->ptCenter= pt[1];
					pBottom->radius  = nRadius;
					pBottom->m_enmLayer = LAYER_PIPE;
					pBottom->SetColor(*m_pstrColor);
				}
			}
			pRet = pHSRApp->m_clsPreProcessor.Run(pRet,PIPE,pTop,pSide,pBottom);
		}
	}

	return pRet;
}
#endif

/**
	@brief	PIPE와 관련된 OPTION을 읽는다.

	@author	humkyung
*/
void CHSRPipe::ReadEnv(LPCTSTR pIniFilePath)
{
	assert(pIniFilePath && "pIniFilePath is NULL");
	static TCHAR szBuf[MAX_PATH + 1]={0,};

	if(pIniFilePath)
	{
		CHSRPipe::m_enmSingleLineOption = CHSRPipe::EQUAL_OD;

		if(::GetPrivateProfileString(_T("HLR_OPTIONS") , _T("Symbolline Display") , NULL , szBuf , MAX_PATH,pIniFilePath))
		{
			/// 대문자로 변경
			const int nLen=STRLEN_T(szBuf);
			for(int i=0;i < nLen;i++) szBuf[i] = toupper(szBuf[i]);
			
			if(EnumString<CHSRPipe::ENM_SINGLE_LINE_OPTION>::From( CHSRPipe::FIXED_SIZE ) == szBuf)
			{
				CHSRPipe::m_nFixedSize = ::GetPrivateProfileInt(_T("HLR_OPTIONS") ,_T("Fixed Size"),0,pIniFilePath);
				CHSRPipe::m_enmSingleLineOption = CHSRPipe::FIXED_SIZE;
			}
		}
	}
}

/**	
	@brief	The CHSRPipe::GetShapeRadius function
	@author	humkyung
	@param	pViewFormat	a parameter of type CHSRViewFormat*
	@param	nRadius	a parameter of type const double&
	@return	const double	
*/
const double CHSRPipe::GetShapeRadius(CHSRViewFormat* pViewFormat,const double& nRadius)
{
	assert(pViewFormat && "pViewFormat is NULL");

	if(pViewFormat)
	{
		CHSRApp* pHSRApp    = CHSRApp::GetInstance();
		CHSROption* pHSREnv = pHSRApp->GetEnv();
		
		if(pHSRApp && pHSREnv)
		{
			const double nCriticalRadius=pHSREnv->GetCriticalRadius();
			const PIPELINEMODE enmPipeLineMode=pHSREnv->GetPipeLineMode();

			if((SINGLE_LINE == enmPipeLineMode) || 
				((BOTH == enmPipeLineMode) && (nRadius <= nCriticalRadius)))
				return (CHSRPipe::FIXED_SIZE == CHSRPipe::m_enmSingleLineOption) ? (CHSRPipe::m_nFixedSize)*0.5 : nRadius;
		}
	}

	return nRadius;
}

/**
 * ClipAxisByElevation:
 *
 * @param ptStart 
 * @param ptEnd 
 * @param pViewFormat 
 * @return void 
 */
void CHSRPipe::ClipAxisByElevation(POINT_T &ptStart, POINT_T& ptEnd , bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , VECTOR_T vecAxis , const double& zhigh , const double& zlow)
{
	if(0.f != vecAxis.dz)
	{
		//--> reposition the pipe point for clipping
		if(ptStart.z > zhigh)
		{
			double t=(zhigh - ptStart.z)/vecAxis.dz;
			ptStart.x = ptStart.x + vecAxis.dx*t;
			ptStart.y = ptStart.y + vecAxis.dy*t;
			ptStart.z = ptStart.z + vecAxis.dz*t;
			
			clipStart  = true;
			clipStartZ = zhigh;
		}
		else if(ptStart.z < zlow)
		{
			double t=(zlow - ptStart.z)/vecAxis.dz;
			ptStart.x = ptStart.x + vecAxis.dx*t;
			ptStart.y = ptStart.y + vecAxis.dy*t;
			ptStart.z = ptStart.z + vecAxis.dz*t;
			
			clipStart  = true;
			clipStartZ = zlow;
		}
		if(ptEnd.z > zhigh)
		{
			double t=(zhigh - ptStart.z)/vecAxis.dz;
			ptEnd.x = ptStart.x + vecAxis.dx*t;
			ptEnd.y = ptStart.y + vecAxis.dy*t;
			ptEnd.z = ptStart.z + vecAxis.dz*t;
			
			clipEnd  = true;
			clipEndZ = zhigh;
		}
		else if(ptEnd.z < zlow)
		{
			double t=(zlow - ptStart.z)/vecAxis.dz;
			ptEnd.x = ptStart.x + vecAxis.dx*t;
			ptEnd.y = ptStart.y + vecAxis.dy*t;
			ptEnd.z = ptStart.z + vecAxis.dz*t;
			
			clipEnd  = true;
			clipEndZ = zlow;
		}
	}
	//<--
}

/**
	@brief	

	@author	BHK

	@date	2009.09.25
*/
int CHSRPipe::Copy(CHSRElement* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied)
	{
		CHSRElement::Copy(pCopied);

		if(pCopied->IsKindOf(CHSRPipe::TypeString()))
		{
			CHSRPipe* pPipe = static_cast<CHSRPipe*>(pCopied);

			m_nSegments = pPipe->m_nSegments;
			m_nPIPEs = pPipe->m_nPIPEs;
			m_nRadius[0] = pPipe->m_nRadius[0];
			m_nRadius[1] = pPipe->m_nRadius[1];
			m_nRadius[2] = pPipe->m_nRadius[2];
			for(int i = 0;i < 6;++i) m_pt[i] = pPipe->m_pt[i];
			m_vol = pPipe->m_vol;
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}