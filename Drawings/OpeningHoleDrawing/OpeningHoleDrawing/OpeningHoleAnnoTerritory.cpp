#include "StdAfx.h"
#include "OpeningHoleAnnoTerritory.h"
#include "OpeningHoleInternalTerritory.h"

COpeningHoleAnnoTerritory::COpeningHoleAnnoTerritory(CaDraw_LogicView* pLogicView , CInternalTerritory* pIntTerritory , CExternalTerritory* pExtTerritory) : CAnnoTerritory(pLogicView , pIntTerritory , pExtTerritory)
{
}

COpeningHoleAnnoTerritory::~COpeningHoleAnnoTerritory(void)
{
	try
	{
		int d = 1;
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class      COpeningHoleAnnoTerritory
    @function   Annotate
    @return     int
    @param      list<CIsLine2d>&    oHsrLineList
    @brief
******************************************************************************/
int COpeningHoleAnnoTerritory::Annotate(list<CIsLine2d>& oHsrLineList)
{
	if(m_pLogicView)
	{
		list<CaDraw_View*> oViewList;
		m_pLogicView->GetViewList(&oViewList);
		for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
		{
			CIsPoint3d ptOrigin=(*itr)->center();
			const double  nWidth =(*itr)->GetViewWidth();
			const double  nHeight=(*itr)->GetViewHeight();
			if(itr == oViewList.begin())
			{		
				m_ExtentRect.x     = ptOrigin.x() - nWidth*0.5;
				m_ExtentRect.y     = ptOrigin.y() - nHeight*0.5;
				m_ExtentRect.width = nWidth;
				m_ExtentRect.height= nHeight;
			}
			else
			{
				if(m_ExtentRect.x > ptOrigin.x() - nWidth *0.5) m_ExtentRect.x = ptOrigin.x() - nWidth*0.5;
				if(m_ExtentRect.y > ptOrigin.y() - nHeight*0.5) m_ExtentRect.y = ptOrigin.y() - nHeight*0.5;
				if(m_ExtentRect.x + m_ExtentRect.width < ptOrigin.x() + nWidth*0.5)  m_ExtentRect.width = ptOrigin.x() + nWidth*0.5 - m_ExtentRect.x;
				if(m_ExtentRect.y + m_ExtentRect.height< ptOrigin.y() + nHeight*0.5) m_ExtentRect.height= ptOrigin.y() + nHeight*0.5- m_ExtentRect.y;
			}
		}

		m_pInternalTerritory->Annotate(oHsrLineList);
		COpeningHoleInternalTerritory* pIntTerritory = static_cast<COpeningHoleInternalTerritory*>(m_pInternalTerritory);
		for(list<COpeningHoleGroupData*>::iterator itr = pIntTerritory->m_oOpeningHoleGroupDataList.begin();itr != pIntTerritory->m_oOpeningHoleGroupDataList.end();++itr)
		{
			const int iCount = (*itr)->GetOpeningHoleCount();
			for(int i = 0;i < iCount;++i)
			{
				COpeningHoleData* pOpeningHoleData = (*itr)->GetOpeningHoleDataAt(i);

				CaDraw_DimTagItem* pDimTagItem = CaDraw_DimTagItem::CreateInstance();
				if(NULL != pDimTagItem)
				{
					pDimTagItem->enmType  = DRAFT_DIM;
					pDimTagItem->m_iTagDir= CaDraw_DimTagItem::DIM_LEFT_SIDE | CaDraw_DimTagItem::DIM_TOP_SIDE;
					pDimTagItem->m_ptOrigin.Set(pOpeningHoleData->origin().x() , pOpeningHoleData->origin().y() , 0.0);
					pDimTagItem->vecDir.Set(1.0 , 1.0 , 0.0);
					m_pExternalTerritory->AddTagItem(pDimTagItem);
				}
			}
		}

		m_pExternalTerritory->Annotate();
	}

	return ERROR_SUCCESS;
}

int COpeningHoleAnnoTerritory::Write(OFSTREAM_T& ofile)
{
	return CAnnoTerritory::Write(ofile);
}