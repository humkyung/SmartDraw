#include "StdAfx.h"
#include <IsTools.h>
#include "AnnoTerritory.h"
#include "SmartDrawAnnoView.h"
#include "SmartDrawAnnoEnv.h"

#include <sstream>

CAnnoTerritory::CAnnoTerritory(CaDraw_LogicView* pLogicView , CInternalTerritory* pIntTerritory , CExternalTerritory* pExtTerritory) : m_pLogicView(pLogicView)
{
	m_ExtentRect.x = m_ExtentRect.y = 0;
	m_ExtentRect.width = m_ExtentRect.height = 0;

	m_pExternalTerritory = (NULL == pExtTerritory) ? new CExternalTerritory() : pExtTerritory;
	m_pInternalTerritory = (NULL == pIntTerritory) ? new CInternalTerritory() : pIntTerritory;
	m_pExternalTerritory->SetAnnoTerritory(this);
	m_pInternalTerritory->SetAnnoTerritory(this);

	m_pAnnoViewList = new list<CSmartDrawAnnoView*>;
}

CAnnoTerritory::~CAnnoTerritory(void)
{
	try
	{
		if(m_pExternalTerritory) SAFE_DELETE(m_pExternalTerritory);
		if(m_pInternalTerritory) SAFE_DELETE(m_pInternalTerritory);

		for(list<CSmartDrawAnnoView*>::iterator itr = m_pAnnoViewList->begin();itr != m_pAnnoViewList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pAnnoViewList->clear();
		SAFE_DELETE(m_pAnnoViewList);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetExternalTerritory
    @return     CExternalTerritory*
    @brief
******************************************************************************/
CExternalTerritory* CAnnoTerritory::GetExternalTerritory()
{
	return m_pExternalTerritory;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetInternalTerritory
    @return     CInternalTerritory*
    @brief
******************************************************************************/
CInternalTerritory* CAnnoTerritory::GetInternalTerritory()
{
	return m_pInternalTerritory;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   Prepare
    @return     int
    @brief
******************************************************************************/
int CAnnoTerritory::Prepare()
{
	list<CaDraw_View*> oViewList;
	m_pLogicView->GetViewList(&oViewList);
	for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
	{
		CSmartDrawAnnoView *pAnnoView = new CSmartDrawAnnoView(*itr);
		if(pAnnoView)
		{
			pAnnoView->CreateTagEntities();
			m_pAnnoViewList->push_back(pAnnoView);
		}
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @function   GetAnnoViewList
    @return     list<CSmartDrawAnnoView*>*
    @brief
******************************************************************************/
list<CSmartDrawAnnoView*>* CAnnoTerritory::GetAnnoViewList()
{
	return m_pAnnoViewList;
}

/**	
	@brief	annotate for a drawing

	@author	humkyung

	@date	2010.05.27

	@param	oHiddenLineList - backdrawing data for internal

	@return	int
*/
int CAnnoTerritory::Annotate(list<CIsLine2d>& oHiddenLineList)
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

		m_pExternalTerritory->Annotate();
		m_pInternalTerritory->Annotate(oHiddenLineList);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write annotation data for territory
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int CAnnoTerritory::Write(OFSTREAM_T& ofile , const double& dDrawingScale)
{
	m_pExternalTerritory->Write(ofile,dDrawingScale);
	m_pInternalTerritory->Write(ofile,dDrawingScale);
	
	return ERROR_SUCCESS;
}
