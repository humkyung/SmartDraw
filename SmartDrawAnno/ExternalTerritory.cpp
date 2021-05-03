// ExternalDim.cpp: implementation of the CExternalTerritory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <IsTools.h>
#include "ExternalTerritory.h"
#include "SmartDrawAnnoView.h"
#include "AnnoTerritory.h"
#include "SmartDrawAnnoEnv.h"

#include <sstream>
#include <list>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CExternalTerritory::CExternalTerritory(/*CAnnoTerritory* pTerritory*/)/// : m_pTerritory(pTerritory)
{
	m_plstEqpCenterline = new list<CIsLine3d>;
	m_plstDivision = new list<CExtDivision*>;
	m_plstTagItem = new list<CaDraw_TagItem*>;
}

/**	
	@brief	

	@author	BHK

	@date	?

	@param	

	@return	
*/
CExternalTerritory::~CExternalTerritory()
{
	try
	{
		for(list<CaDraw_TagItem*>::iterator itr = m_plstTagItem->begin();itr != m_plstTagItem->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_plstTagItem->clear();
		SAFE_DELETE(m_plstTagItem);

		for(list<CExtDivision*>::iterator itr=m_plstDivision->begin();itr != m_plstDivision->end();)
		{
			SAFE_DELETE(*itr);
			itr = m_plstDivision->erase(itr);
		}
		SAFE_DELETE(m_plstDivision);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @class      CExternalTerritory
    @function   SetAnnoTerritory
    @return     int
    @param      CAnnoTerritory* pTerritory
    @brief
******************************************************************************/
int CExternalTerritory::SetAnnoTerritory(CAnnoTerritory* pTerritory)
{
	m_pTerritory = pTerritory;
	return ERROR_SUCCESS;
}

/**	
	@brief	store tag items to division and then run.
	@author	humkyung
	@date	????.??.??
	@param	pLogicView	a parameter of type CLogicView*
	@return	void	
*/
int CExternalTerritory::Annotate()
{
	CreateDivisions();

	for(list<CSmartDrawAnnoView*>::iterator itr = m_pTerritory->m_pAnnoViewList->begin();itr != m_pTerritory->m_pAnnoViewList->end();++itr)
	{
		(*itr)->Annotate(m_pTerritory->m_ExtentRect , this);
	}

	///ofstream ofile("d:\\debug.txt");
	for(list<CaDraw_TagItem*>::iterator itr = m_plstTagItem->begin();itr != m_plstTagItem->end();++itr)
	{
		(*itr)->OnRegisterToDivision( *m_plstDivision );

		///if((*itr)->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString()))
		///{
		///	CaDraw_TagItem *pTagItem = *itr;

			///ofile << pTagItem->ptOrigin.x() << "," << pTagItem->ptOrigin.y() << std::endl;

		///	(*itr)->OnRegisterToDivision(m_lstDivision);
		///}
		///else if((*itr)->m_pEnt->IsKindOf(CaDraw_TagEquipment::TypeString()))
		//{
		///	(*itr)->OnRegisterToDivision(m_lstDivision);
			/*
			double nXMax=0,nYMax=0;
			CExtDivision *pXDivision=NULL,*pYDivision=NULL;

			for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
			{
				// x dir test
				if((*itrDivision)->IsCrossingXRay((*itrDITEM)->ptOrigin))
				{
					double len = (*itrDivision)->GetXDistance((*itrDITEM)->ptOrigin,false);
					if(!nXMax)
					{
						nXMax      =len;
						pXDivision=*itrDivision;
					}
					else if(len < nXMax)
					{
						nXMax      =len;
						pXDivision=*itrDivision;
					}
				}

				// y dir test
				if((*itrDivision)->IsCrossingYRay((*itrDITEM)->ptOrigin))
				{
					double len = (*itrDivision)->GetYDistance((*itrDITEM)->ptOrigin,false);
					if(!nYMax)
					{
						nYMax      =len;
						pYDivision=*itrDivision;
					}
					else if(len < nYMax)
					{
						nYMax      =len;
						pYDivision=*itrDivision;
					}
				}
			}
			if(pXDivision) (*pXDivision)+=*itrDITEM;
			if(pYDivision) (*pYDivision)+=*itrDITEM;
			*/
		//}
		/*
		else if(DRAFT_STR == (*itrDITEM)->enmType)
		{
			StoreStrToDivision(*itrDITEM);
		}
		else if(DRAFT_ETIM == (*itrDITEM)->enmType)
		{
			for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
			{
				POINT_T ptOrigin=(*itrDITEM)->ptOrigin;
				if((*itrDivision)->IsInDivision(ptOrigin , (*itrDITEM)->vecDir))
				{
					(*(*itrDivision)) += *itrDITEM;
				}
			}
		}
		else if(DRAFT_DIM == (*itrDITEM)->enmType)
		{
			double nXMax=0,nYMax=0;
			CExtDivision *pXDivision=NULL,*pYDivision=NULL;

			for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
			{
				// x dir test
				if((*itrDivision)->IsCrossingXRay((*itrDITEM)->ptOrigin))
				{
					const double len = (*itrDivision)->GetXDistance((*itrDITEM)->ptOrigin,false);
					if(!nXMax)
					{
						nXMax      =len;
						pXDivision=*itrDivision;
					}
					else if(len < nXMax)
					{
						nXMax      =len;
						pXDivision=*itrDivision;
					}
				}

				// y dir test
				if((*itrDivision)->IsCrossingYRay((*itrDITEM)->ptOrigin))
				{
					const double len = (*itrDivision)->GetYDistance((*itrDITEM)->ptOrigin,false);
					if(!nYMax)
					{
						nYMax      =len;
						pYDivision=*itrDivision;
					}
					else if(len < nYMax)
					{
						nYMax      =len;
						pYDivision=*itrDivision;
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			/// 2005.11.21
			if(DIM_BOTH_SIDE == (*itrDITEM)->m_nDimStyle)
			{
				if(pXDivision) (*pXDivision)+=*itrDITEM;
				if(pYDivision) (*pYDivision)+=*itrDITEM;
			}
			else if(DIM_ONE_SIDE == (*itrDITEM)->m_nDimStyle)
			{
				if((nXMax < nYMax) && pXDivision) (*pXDivision)+=*itrDITEM;
				if((nYMax < nXMax) && pYDivision) (*pYDivision)+=*itrDITEM;
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else if(DRAFT_CABLE == (*itrDITEM)->enmType)
		{
			bool bStore=false;
			for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
			{
				double cross=(*itrDITEM)->vecDir.dx*(*itrDivision)->m_vecTagDir.dy - (*itrDITEM)->vecDir.dy*(*itrDivision)->m_vecTagDir.dx;
				CaDraw_TagItem* pDITEM   =*itrDITEM;
				CExtDivision* pDivision=*itrDivision;
				if(fabs(cross) < 0.001)
				{ // test is same dir?
					POINT_T ptOrigin=(*itrDITEM)->ptOrigin;
					if((*itrDivision)->IsInDivision(ptOrigin , (*itrDITEM)->vecDir))
					{
						(*(*itrDivision)) += *itrDITEM;
						bStore = true;
					}
				}
			}
		}
		else
		{
			CaDraw_TagItem* pItem=(*itrDITEM);
			pItem->OnStartTestItem();
			for(list<CExtDivision*>::iterator itr=m_plstDivision->begin();itr != m_plstDivision->end();++itr)
			{
				pItem->OnTestItem(*itr);
			}
			pItem->OnEndTestItem();
		}
		*/
	}
	//ofile.close();

	for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
	{
		(*itrDivision)->Annotate();
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	store structure tag item to division

	\param	pDITEM	a parameter of type CaDraw_TagItem*

	\return	void	
*/
void CExternalTerritory::StoreStrToDivision(CaDraw_TagItem* pDITEM)
{
	assert(pDITEM && "pDITEM is NULL");

	if(pDITEM)
	{
		/*
		pDITEM->OnStartTestItem();
		for(list<CExtDivision*>::iterator itr=m_plstDivision->begin();itr != m_plstDivision->end();++itr)
		{
			pDITEM->OnTestDivisionItem(*itr);
		}
		pDITEM->OnEndTestItem();
		*/
	}
}

/**	
	@brief	write
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
void CExternalTerritory::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	
	/// draw equipment center line
	TCHAR szBuf[256]={0,};
	for(list<CIsLine3d>::iterator itr=m_plstEqpCenterline->begin();itr != m_plstEqpCenterline->end();++itr)
	{
		double d = 0;

		ofile << _T("<LINE>|");
		d = itr->start().x(); ofile << d*dDrawingScale << _T(",");
		d = itr->start().y(); ofile << d*dDrawingScale << _T(",0,");
		d = itr->end().x(); ofile << d*dDrawingScale << _T(",");	
		d = itr->end().y(); ofile << d*dDrawingScale << _T(",0,|");
		ofile << _T("0,0,0,0,0") << std::endl;
	}

	if(env.m_generation.IsEnable(_T("View Information"),NULL)) this->WriteViewInfo(ofile,dDrawingScale);
	for(list<CExtDivision*>::iterator itrDivision=m_plstDivision->begin();itrDivision != m_plstDivision->end();++itrDivision)
	{
		CExtDivision* pExtDivision=static_cast<CExtDivision*>(*itrDivision);
		pExtDivision->Write(ofile,dDrawingScale);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   WriteViewInfo
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CExternalTerritory::WriteViewInfo(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	{
		STRING_T sViewInfo;
		TCHAR chAlign='L';

		list<CaDraw_View*> oViewList;
		m_pTerritory->m_pLogicView->GetViewList(&oViewList);
		for(list<CaDraw_View*>::iterator itr=oViewList.begin();itr != oViewList.end();++itr)
		{
			sViewInfo = (*itr)->GetViewInfo();
			if(!sViewInfo.empty()) break;
		}
		if(sViewInfo.empty())
		{
			/// default value of view information
			CIsVolume volume = m_pTerritory->m_pLogicView->volume();
			OSTRINGSTREAM_T oss;
			oss << _T("EL. ") << volume.minz() << _T(" ~ EL. ") << volume.maxz();
			sViewInfo = oss.str();
		}

		if(0 == STRICMP_T(env.m_elev.m_szJustify, _T("CENTER")))
		{
			ofile << _T("<TEXT>|");
			double d = 0;
			d = m_pTerritory->m_ExtentRect.x + (m_pTerritory->m_ExtentRect.width*0.5); 
			ofile << d*dDrawingScale << _T(",");
			d = m_pTerritory->m_ExtentRect.y - env.m_elev.m_dDistA;
			ofile << d*dDrawingScale << _T(",0,");
			ofile << env.m_elev.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_elev.m_oTextStyle.width()*dDrawingScale << _T(",0,");
			ofile << sViewInfo << _T(",Center Center,0|");
			ofile << env.m_elev.m_oLabelProp.level << _T(",") << env.m_elev.m_oLabelProp.color << _T(",");
			ofile << env.m_elev.m_oTextStyle << std::endl;
		}
		else if(0 == STRICMP_T(env.m_elev.m_szJustify, _T("LEFT")))
		{	
			ofile << _T("<TEXT>|");
			double d = 0;
			d = m_pTerritory->m_ExtentRect.x;
			ofile << d*dDrawingScale << _T(",");
			d = m_pTerritory->m_ExtentRect.y - env.m_elev.m_dDistA;
			ofile << d*dDrawingScale << _T(",0,");
			ofile << env.m_elev.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_elev.m_oTextStyle.width()*dDrawingScale << _T(",0,");
			ofile << sViewInfo << _T(",Left Center,0|");
			ofile << env.m_elev.m_oLabelProp.level << _T(",") << env.m_elev.m_oLabelProp.color << _T(",");
			ofile << env.m_elev.m_oTextStyle << std::endl;
		}
		else if(0 == STRICMP_T(env.m_elev.m_szJustify, _T("RIGHT")))
		{
			ofile << _T("<TEXT>|");
			double d = 0;
			d = m_pTerritory->m_ExtentRect.x + (m_pTerritory->m_ExtentRect.width);
			ofile << d*dDrawingScale << _T(",");
			d = m_pTerritory->m_ExtentRect.y - env.m_elev.m_dDistA;
			ofile << d*dDrawingScale << _T(",0,");
			ofile << env.m_elev.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_elev.m_oTextStyle.width()*dDrawingScale << _T(",0,");
			ofile << sViewInfo << _T(",Right Center,0|");
			ofile << env.m_elev.m_oLabelProp.level << _T(",") << env.m_elev.m_oLabelProp.color << _T(",");
			ofile << env.m_elev.m_oTextStyle << std::endl;
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	The CExternalTerritory::operator+= function
	@param	pDivision	a parameter of type CExtDivision*
	@return	bool	
*/
bool CExternalTerritory::operator+=(CExtDivision* pDivision)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision) m_plstDivision->push_back(pDivision);

	return true;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return
*/
int CExternalTerritory::CreateDivisions(void)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	for(CViewBoundaySegment* ptr = m_pTerritory->m_pLogicView->m_pViewBoundarySegment;ptr;ptr = ptr->next)
	{
		if(-1.f == ptr->vecTagDir.dy())
		{
			ptr->nHeight[0] = ptr->pView->GetViewHeight() * env.m_area.LineNoRange[BOTTOM_SIDE];
			ptr->nHeight[1] = ptr->pView->GetViewHeight() * env.m_area.Range[BOTTOM_SIDE];
		}
		else if(1.f == ptr->vecTagDir.dx())
		{
			ptr->nHeight[0] = ptr->pView->GetViewWidth() * env.m_area.LineNoRange[RIGHT_SIDE];
			ptr->nHeight[1] = ptr->pView->GetViewWidth() * env.m_area.Range[RIGHT_SIDE];
		}
		else if(1.f == ptr->vecTagDir.dy())
		{
			ptr->nHeight[0] = ptr->pView->GetViewHeight() * env.m_area.LineNoRange[TOP_SIDE];
			ptr->nHeight[1] = ptr->pView->GetViewHeight() * env.m_area.Range[TOP_SIDE];
		}
		else if(-1.f == ptr->vecTagDir.dx())
		{
			ptr->nHeight[0] = ptr->pView->GetViewWidth() * env.m_area.LineNoRange[LEFT_SIDE];
			ptr->nHeight[1] = ptr->pView->GetViewWidth() * env.m_area.Range[LEFT_SIDE];
		}

		bool bOpen[2]={true,false};
		if(	(CaDraw_View::LEFT_VIEW_T == ptr->pView->GetViewDir())|| 
			(CaDraw_View::FRONT_VIEW_T== ptr->pView->GetViewDir())||
			(CaDraw_View::BACK_VIEW_T == ptr->pView->GetViewDir())||
			(CaDraw_View::RIGHT_VIEW_T== ptr->pView->GetViewDir()))
		{
			/// SECTION VIEW일 때....
			if(IsSecTagBoundary(ptr))
			{
				CExtDivision* pDivision=new CExtDivision(ptr->edge,ptr->bOpen,ptr->vecTagDir,ptr->pView);
				if(pDivision)
				{
					pDivision->SetViewMatchLine(ptr->szViewMatchLine);
					pDivision->SetDivisionHeight(ptr->nHeight[0] , ptr->nHeight[1]);
					STRNCPY_T(pDivision->m_szDim[0],ptr->szDim[0] , 64);
					STRNCPY_T(pDivision->m_szDim[1],ptr->szDim[1] , 64);
					m_plstDivision->push_back(pDivision);
				}
			}
		}
		else	/// plan view
		{
			CExtDivision* pDivision = NULL;
			
			/// if divsion flag is DIVISION_OFF, skip below routine that create CDivision instance.
			if(0 == ptr->nDivisionFlag) continue;

			/*
			if(ARC_VBS == ptr->nType)
			{
				pDivision = new CQuadExtDivision(ptr->edge,ptr->bOpen,ptr->vecTagDir,ptr->pView);
				CQuadExtDivision* pQuadExtDiv = (CQuadExtDivision*)pDivision;
				if(pQuadExtDiv) pQuadExtDiv->SetCenterOfQuad(ptr->ptCenterOfQuad);
			}
			else
			*/
			{
				pDivision = new CExtDivision(ptr->edge,ptr->bOpen,ptr->vecTagDir,ptr->pView);
			}

			if(pDivision)
			{
				pDivision->SetViewMatchLine(ptr->szViewMatchLine);
				pDivision->SetDivisionHeight(ptr->nHeight[0] , ptr->nHeight[1]);
				STRNCPY_T(pDivision->m_szDim[0],ptr->szDim[0] , 64);
				STRNCPY_T(pDivision->m_szDim[1],ptr->szDim[1] , 64);
				m_plstDivision->push_back(pDivision);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	Section뷰에서 Tag가 가능한 View Boundary인지를 리턴한다.

	@author	HumKyung.BAEK

	@param	pVBS	a parameter of type PVBS

	@return	bool	
*/
bool CExternalTerritory::IsSecTagBoundary(CViewBoundaySegment* pVBS)
{
	assert(pVBS && "pVBS is NULL");
	const static double nTol=0.001;
	if(pVBS)
	{
		const CaDraw_View::ROTATE_T rotate = pVBS->pView->GetViewAngle();
		switch(rotate)
		{
			case CaDraw_View::R0_T:
				return (( 1. == pVBS->vecTagDir.dy()) || 
					( 1. == pVBS->vecTagDir.dx()) ||
					(-1. == pVBS->vecTagDir.dx()));
			case CaDraw_View::R90_T:
				return ((-1. == pVBS->vecTagDir.dx()) ||
					( 1. == pVBS->vecTagDir.dy()) ||
					(-1. == pVBS->vecTagDir.dy()));
			case CaDraw_View::R180_T:
				return ((-1. == pVBS->vecTagDir.dy()) ||
					(-1. == pVBS->vecTagDir.dx()) ||
					( 1. == pVBS->vecTagDir.dx()));
			case CaDraw_View::R270_T:
				return (( 1. == pVBS->vecTagDir.dx()) ||
					( 1. == pVBS->vecTagDir.dy()) ||
					(-1. == pVBS->vecTagDir.dy()));
		}
	}

	return false;
}