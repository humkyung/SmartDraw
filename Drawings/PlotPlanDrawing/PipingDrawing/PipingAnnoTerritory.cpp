#include "StdAfx.h"
#include "CableTrayAnnoTerritory.h"
#include "CableTrayInternalTerritory.h"

CCableTrayAnnoTerritory::CCableTrayAnnoTerritory(CaDraw_LogicView* pLogicView) : CAnnoTerritory(pLogicView)
{
	SAFE_DELETE(m_pInternalTerritory);
	m_pInternalTerritory = new CCableTrayInternalTerritory(this);
}

CCableTrayAnnoTerritory::~CCableTrayAnnoTerritory(void)
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
    @class      CCableTrayAnnoTerritory
    @function   Annotate
    @return     int
    @param      list<CIsLine2d>&    oHsrLineList
    @brief
******************************************************************************/
int CCableTrayAnnoTerritory::Annotate(list<CIsLine2d>& oHsrLineList)
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
		m_pInternalTerritory->Annotate(oHsrLineList);		
	}

	return ERROR_SUCCESS;
}

int CCableTrayAnnoTerritory::Write(OFSTREAM_T& ofile)
{
	return CAnnoTerritory::Write(ofile);
}