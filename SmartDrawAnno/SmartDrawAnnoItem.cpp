#include "StdAfx.h"
#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include <IsTools.h>
#include "SmartDrawAnnoStru.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoItem.h"
#include "SmartDrawAnnoPos.h"
#include "aDraw_StruTagPos.h"
#include "ExtDivision.h"
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoEnv.h"

#include <sstream>

/**	
	@brief

	@author	humkyung
*/
CaDraw_TagItem::CaDraw_TagItem()
{
	ZeroMemory( m_szTypeString , sizeof( TCHAR ) * 64 );
	name[0] = new STRING_T;
	name[1] = new STRING_T;
	m_pEnt = NULL;
	m_nDimStyle = DIM_BOTH_SIDE;

	enmType = NONE;
	enmSubType = VERTICAL;
	nBOP = nCL = nTOP = 0.f;
	nLevel = 0;	/// dimension level

	m_pTagPosList = new list<CaDraw_TagPos*>;
}

CaDraw_TagItem::~CaDraw_TagItem()
{
	try
	{
		if( name[0] ) delete name[0];
		if( name[1] ) delete name[1];

		for(list<CaDraw_TagPos*>::iterator itr = m_pTagPosList->begin();itr != m_pTagPosList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		SAFE_DELETE(m_pTagPosList);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   CreateInstance
    @return     CaDraw_TagItem*
    @brief
******************************************************************************/
CaDraw_TagItem* CaDraw_TagItem::CreateInstance()
{
	return (new CaDraw_TagItem);
}

/**	
	@brief	The OnCreateTagPos function

	@author	BHK

	@date	?

	@param	pDivision

	@return	CaDraw_TagPos*	
*/
int CaDraw_TagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");

	TagPosList.clear();

	CaDraw_TagPos* pRet = new CaDraw_TagPos(this);
	if(NULL != pRet)
	{
		TagPosList.push_back(pRet);
		
		m_pTagPosList->push_back(pRet);

		/*
		if(DRAFT_EQP == m_pEnt->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			CDraftEqui* pEqui = static_cast<CDraftEqui*>(m_pEnt);
			if(pEqui)
			{
				string szTag[2];
				pEqui->GetTagNo(&(szTag[0]) , &(szTag[1]));
				
				if(NULL != strchr(szTag[1].c_str() , '+'))
					pRet->rect.height = CDraftEqui::ENV.nTextHeight + (CDraftEqui::ENV.nCoordHeight*2);
				else	pRet->rect.height = CDraftEqui::ENV.nTextHeight + CDraftEqui::ENV.nCoordHeight;
			}
			else
			{
				pRet->rect.height = CDraftEqui::ENV.nTextHeight + CDraftEqui::ENV.nCoordHeight;
			}
		}
		*/
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @class      CaDraw_TagItem
    @function   SetTypeString
    @return     int
    @param      const       STRING_T&
    @param      rTypeString
    @brief
******************************************************************************/
int CaDraw_TagItem::SetTypeString(const STRING_T& rTypeString)
{
	STRNCPY_T(m_szTypeString , rTypeString.c_str() , 63);
	return ERROR_SUCCESS;
}

const TCHAR* CaDraw_TagItem::GetTypeString() const
{
	return m_szTypeString;
}

void CaDraw_TagItem::OutputDebugString()
{
	OSTRINGSTREAM_T ostr;
	ostr << GetTypeString() << _T(",") << nLevel;

	::OutputDebugString( ostr.str().c_str() );	
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param	

	@return	const char*	
*/
CaDraw_PipeTagItem::CaDraw_PipeTagItem()
{
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param	

	@return	const char*	
*/
CaDraw_PipeTagItem::~CaDraw_PipeTagItem()
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
void CaDraw_PipeTagItem::OnRegisterToDivision(list<CExtDivision*>& DivisionList)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	/// return if pipe size is less than tag display size - 2014.06.14 added by humkyung
	CaDraw_TagPipe* pPipe = static_cast<CaDraw_TagPipe*>(this->m_pEnt);
	const double dNominalSizeInMetric = pPipe->pipe()->GetNominalPipeSizeInMetric();
	const double dTagDisplaySize = env.m_pipe.m_ext.GetTagDisplaySize();
	if(dNominalSizeInMetric <= dTagDisplaySize) return;
	/// up to here

	bool bAdded = false;
	for(list<CExtDivision*>::iterator itr = DivisionList.begin();itr != DivisionList.end();++itr)
	{
		if((*itr)->IsInDivision(m_ptOrigin , vecDir , 0))
		{
			(*itr)->Register(this);
			CaDraw_Pipe* pDrawPipe = (static_cast<CaDraw_TagPipe*>(m_pEnt))->pipe();
			if(pDrawPipe) pDrawPipe->m_nTagFlag = CaDraw_Pipe::ALREADY_TAGGED;
			bAdded = true;
		}
		else if((*itr)->IsMathLinePoint(m_ptOrigin))
		{
			(*itr)->Register(this);
			CaDraw_Pipe* pDrawPipe = (static_cast<CaDraw_TagPipe*>(m_pEnt))->pipe();
			if(pDrawPipe) pDrawPipe->m_nTagFlag = CaDraw_Pipe::ALREADY_TAGGED;
			bAdded = true;
		}
	}

	if(!bAdded && m_pEnt)
	{
		CaDraw_Pipe* pDrawPipe=static_cast<CaDraw_TagPipe*>(m_pEnt)->pipe();
		if(pDrawPipe && (CaDraw_Pipe::ALREADY_TAGGED != pDrawPipe->m_nTagFlag))
		{
			pDrawPipe->m_nTagFlag = CaDraw_Pipe::UNTAGGED;
		}
	}
}

/**	
	@brief	create a tag pos

	@author	humkyung

	@date	2010.05.28

	@param	

	@return	
*/
int CaDraw_PipeTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");
	TagPosList.clear();

	if(pDivision)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CaDraw_TagPos* pRet = new CaDraw_PipeTagPos(this);
		TagPosList.push_back(pRet);
		m_pTagPosList->push_back(pRet);

		if(pDivision->IsVertical())
		{
			pRet->m_ptStart.x() = (pDivision->m_lnBaseLine.start().x());
			pRet->m_ptStart.y() = (m_ptOrigin.y());
			pRet->rect.width  = 0;
			pRet->nOverlap    = 0;
			pRet->type        = DOUBLE_TAG;
			pRet->pTITEM      = this;
			pRet->nLevelOn = DIM_LEVEL_0_ON;
			if(DOUBLELINE == env.m_pipe.m_ext.eTag)
			{
				if(VERTICAL == enmSubType)
				{
					pRet->type       = SINGLE_TAG;
					pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;

					pRet->m_ptEnd.x() = pRet->m_ptStart.x();
					pRet->m_ptEnd.y() = (pRet->m_ptStart.y() + pRet->rect.height*0.5);
				}
				else
				{
					pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_oElevTextStyle.height() + 
						env.m_pipe.m_ext.m_dLabelOffset;
					pRet->m_ptEnd = pRet->m_ptStart;
				}
			}
			else
			{	/// single line
				pRet->type       = SINGLE_TAG;
				pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;

				pRet->m_ptEnd.x() = (pRet->m_ptStart.x());
				pRet->m_ptEnd.y() = (pRet->m_ptStart.y() + pRet->rect.height*0.5);
			}
		}
		else
		{ /// HORIZONTAL
			pRet->m_ptStart.x()= (m_ptOrigin.x());
			pRet->m_ptStart.y()= (pDivision->m_lnBaseLine.start().y());
			pRet->rect.width = 0;
			pRet->nOverlap   = 0;
			pRet->type       = DOUBLE_TAG;

			pRet->pTITEM     = this;
			pRet->nLevelOn = DIM_LEVEL_0_ON;

			if(DOUBLELINE == env.m_pipe.m_ext.eTag)
			{
				if(VERTICAL == enmSubType)
				{
					pRet->type       = SINGLE_TAG;
					pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;

					pRet->m_ptEnd.x() = (pRet->m_ptStart.x() - pRet->rect.height*0.5);
					pRet->m_ptEnd.y() = (pRet->m_ptStart.y());
				}
				else
				{
					pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_oElevTextStyle.height() + 
						env.m_pipe.m_ext.m_dLabelOffset;
					pRet->m_ptEnd = pRet->m_ptStart;
				}
			}
			else
			{
				pRet->type       = SINGLE_TAG;
				pRet->rect.height= env.m_pipe.m_ext.m_oLineNoTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;

				pRet->m_ptEnd.x() = (pRet->m_ptStart.x() - pRet->rect.height*0.5);
				pRet->m_ptEnd.y() = (pRet->m_ptStart.y());
			}
		}
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	const char*	
*/
CaDraw_EquiTagItem::CaDraw_EquiTagItem() : m_bWirteCenterline(false)
{
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param	

	@return	const char*	
*/
CaDraw_EquiTagItem::~CaDraw_EquiTagItem()
{
}

/**	
	@brief	add equipment tag item to division
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
void CaDraw_EquiTagItem::OnRegisterToDivision(list<CExtDivision*>& DivisionList)
{
	double nXMax=0,nYMax=0;
	CExtDivision *pXDivision=NULL,*pYDivision=NULL;

	CaDraw_TagEqui *pTagEqui = static_cast<CaDraw_TagEqui*>(m_pEnt);
	pTagEqui->equi()->origin();

	list<CExtDivision*>::iterator itr;
	for(itr = DivisionList.begin();itr != DivisionList.end();++itr)
	{
		/// x dir test
		if((*itr)->IsCrossingXRay(m_ptOrigin))
		{
			double len = (*itr)->GetXDistance(m_ptOrigin,false);
			if(!nXMax)
			{
				nXMax      =len;
				pXDivision=*itr;
			}
			else if(len < nXMax)
			{
				nXMax      =len;
				pXDivision=*itr;
			}
		}

		/// y dir test
		if((*itr)->IsCrossingYRay(m_ptOrigin))
		{
			double len = (*itr)->GetYDistance(m_ptOrigin,false);
			if(!nYMax)
			{
				nYMax      =len;
				pYDivision=*itr;
			}
			else if(len < nYMax)
			{
				nYMax      =len;
				pYDivision=*itr;
			}
		}
	}
	if(pXDivision) (pXDivision)->Register(this);
	if(pYDivision) (pYDivision)->Register(this);
}

/**	
	@brief	
	@author	humkyung
	@date	2010.05.28
	@param	
	@return	
*/
int CaDraw_EquiTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");
	TagPosList.clear();

	if(pDivision)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		if(DRAFT_DIM == enmType)
		{
			CaDraw_TagPos* pRet = new CaDraw_TagPos(this);
			TagPosList.push_back(pRet);
			m_pTagPosList->push_back(pRet);

			pRet->type     = DIM_TAG;
			pRet->nLevelOn = env.m_generation.GetDimLevelOf(CaDraw_TagEqui::TypeString());
			pRet->m_ptStart= m_ptOrigin;
		}
		else
		{
			CaDraw_TagPos* pRet = new CaDraw_EquiTagPos(this);
			TagPosList.push_back(pRet);
			m_pTagPosList->push_back(pRet);

			STRING_T szTag[2];
			CaDraw_TagEqui *pTagEqui = static_cast<CaDraw_TagEqui*>(m_pEnt);
			pTagEqui->GetTagNo(&(szTag[0]) , &(szTag[1]));
			if(NULL != STRCHR_T(szTag[1].c_str() , '+'))
				pRet->rect.height = env.m_equi.m_oNameTextStyle.height() + (env.m_equi.m_oCoordTextStyle.height()*2);
			else	pRet->rect.height = env.m_equi.m_oNameTextStyle.height() + env.m_equi.m_oCoordTextStyle.height();
			pRet->nLevelOn = env.m_generation.GetDimLevelOf(CaDraw_TagEqui::TypeString());

			if(pDivision->IsVertical())
			{
				pRet->m_ptStart.x() = (pDivision->m_lnBaseLine.start().x());
				pRet->m_ptStart.y() = (m_ptOrigin.y());
				pRet->rect.width  = 0;
				pRet->nOverlap    = 0;
				pRet->type        = DOUBLE_TAG;
				
				const double nHeight = (pRet->rect.height)*0.5;

				pRet->nLevelOn = env.m_generation.GetDimLevelOf(CaDraw_TagEqui::TypeString());
				pRet->m_ptEnd.x() = (pRet->m_ptStart.x());
				pRet->m_ptEnd.y() = (pRet->m_ptStart.y() + fabs(nHeight - env.m_equi.m_oNameTextStyle.height()));
			}
			else
			{ /// HORIZONTAL
				pRet->m_ptStart.x()= (m_ptOrigin.x());
				pRet->m_ptStart.y()= (pDivision->m_lnBaseLine.start().y());
				pRet->rect.width = 0;
				pRet->nOverlap   = 0;
				pRet->type       = DOUBLE_TAG;

				const double nHeight = (pRet->rect.height)*0.5;

				pRet->nLevelOn = env.m_generation.GetDimLevelOf(CaDraw_TagEqui::TypeString());
				pRet->m_ptEnd.x() = (pRet->m_ptStart.x() - fabs(nHeight - env.m_equi.m_oNameTextStyle.height()));
				pRet->m_ptEnd.y() = (pRet->m_ptStart.y());
			}
		}
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	const char*	
*/
CaDraw_StruTagItem::CaDraw_StruTagItem()
{
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param	

	@return	const char*	
*/
CaDraw_StruTagItem::~CaDraw_StruTagItem()
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
void CaDraw_StruTagItem::OnRegisterToDivision(list<CExtDivision*>& DivisionList)
{
	double nXLen[2]={-1,-1},nYLen[2]={-1,-1};
	CExtDivision *pXDivision[2]={NULL,},*pYDivision[2]={NULL,};
	
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	for(list<CExtDivision*>::iterator itr = DivisionList.begin();itr != DivisionList.end();++itr)
	{
		const TCHAR* pName1=name[0]->c_str();
		const TCHAR* pName2=name[1]->c_str();
		if(fabs(1 - fabs(vecDir.dx())) < 0.001)
		{
			/// x dir test
			if((*itr)->IsCrossingXRay(m_ptOrigin))
			{
				const double nLen = (*itr)->GetXDistance(m_ptOrigin,true);
				if((env.m_stru.nTagDir & STR_LEFT) && (-1.f == (*itr)->m_vecTagDir.dx()))
				{
					if((nLen <= 0.) && (nXLen[0] < 0.))
					{ // check on division
						nXLen[0]     =fabs(nLen);
						pXDivision[0]=(*itr);
					}
					else if((nLen < 0.) && (fabs(nLen) < nXLen[0]))
					{
						nXLen[0]     =fabs(nLen);
						pXDivision[0]=(*itr);
					}
				}
				if((env.m_stru.nTagDir & STR_RIGHT) && (1.f == (*itr)->m_vecTagDir.dx()))
				{
					if((nLen >= 0.) && (nXLen[1] < 0.))
					{ // check on division
						nXLen[1]     =fabs(nLen);
						pXDivision[1]=(*itr);
					}
					else if((nLen > 0.) && (nLen < nXLen[1]))
					{
						nXLen[1]     =fabs(nLen);
						pXDivision[1]=(*itr);
					}
				}
			}
		}
		else if((1 - fabs(vecDir.dy())) < 0.001)
		{
			/// y dir test
			if((*itr)->IsCrossingYRay(m_ptOrigin))
			{
				double nLen = (*itr)->GetYDistance(m_ptOrigin,true);
				if((env.m_stru.nTagDir & STR_UP) && (1.f == (*itr)->m_vecTagDir.dy()))
				{
					if((nLen >= 0.) && (nYLen[0] < 0.))
					{ // check on division
						nYLen[0]     =fabs(nLen);
						pYDivision[0]=(*itr);
					}
					else if((nLen > 0.) && (nLen < nYLen[0]))
					{
						nYLen[0]     =fabs(nLen);
						pYDivision[0]=(*itr);
					}
				}
				if((env.m_stru.nTagDir & STR_DOWN) && (-1.f == (*itr)->m_vecTagDir.dy()))
				{
					if((nLen <= 0.) && (nYLen[1] < 0.))
					{ // check on division
						nYLen[1]     =fabs(nLen);
						pYDivision[1]=(*itr);
					}
					else if((nLen < 0.) && (fabs(nLen) < nYLen[1]))
					{
						nYLen[1]     =fabs(nLen);
						pYDivision[1]=(*itr);
					}
				}
			}
		}
	}

	if(pXDivision[0]) (*(pXDivision[0])).Register(this);
	if(pXDivision[1]) (*(pXDivision[1])).Register(this);
	if(pYDivision[0]) (*(pYDivision[0])).Register(this);
	if(pYDivision[1]) (*(pYDivision[1])).Register(this);
}

/**	
	@brief	create tag pos for structure
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
int CaDraw_StruTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");
	TagPosList.clear();

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(NULL != pDivision)
	{
		CaDraw_StruTagPos *pRet = new CaDraw_StruTagPos;
		TagPosList.push_back(pRet);
		m_pTagPosList->push_back(pRet);

		if(NULL != pRet)
		{
			pRet->nLevelOn = env.m_generation.GetDimLevelOf(CaDraw_TagStru::TypeString());
			if(pDivision->IsVertical())
			{
				pRet->m_ptStart.x()	= pDivision->GetBaseLine().start().x();
				pRet->m_ptStart.y()	= m_ptOrigin.y();
				pRet->rect.width	= 0;
				pRet->nOverlap		= 0;
				pRet->type		= DOUBLE_TAG;
				pRet->pTITEM		= this;
			
				pRet->rect.height= env.m_stru.m_dRadius*2;			
				pRet->m_ptEnd.x() = pRet->m_ptStart.x();
				pRet->m_ptEnd.y() = pRet->m_ptStart.y();
			}
			else
			{
				pRet->m_ptStart.x()    = m_ptOrigin.x();
				pRet->m_ptStart.y()    = pDivision->GetBaseLine().start().y();
				pRet->rect.width = 0;
				pRet->nOverlap   = 0;
				pRet->type       = DOUBLE_TAG;
				pRet->pTITEM	 = this;

				pRet->rect.height= env.m_stru.m_dRadius*2;
				pRet->m_ptEnd.x() = pRet->m_ptStart.x();
				pRet->m_ptEnd.y() = pRet->m_ptStart.y();
			}
		}
	}

	return ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	const char*	
*/
CaDraw_TrayTagItem::CaDraw_TrayTagItem()
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	const char*	
*/
CaDraw_TrayTagItem::~CaDraw_TrayTagItem()
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
void CaDraw_TrayTagItem::OnRegisterToDivision(list<CExtDivision*>& DivisionList)
{
	
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.28
	@param	
	@return	
*/
int CaDraw_TrayTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");
	TagPosList.clear();

	return ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   CaDraw_DimTagItem
    @return
    @brief
******************************************************************************/
CaDraw_DimTagItem::CaDraw_DimTagItem()
{
	m_iTagDir = CaDraw_DimTagItem::DIM_NEAR_SIDE;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   ~CaDraw_DimTagItem
    @return
    @brief
******************************************************************************/
CaDraw_DimTagItem::~CaDraw_DimTagItem()
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   CreateInstance
    @return     CaDraw_DimTagItem*
    @brief
******************************************************************************/
CaDraw_DimTagItem* CaDraw_DimTagItem::CreateInstance()
{
	return (new CaDraw_DimTagItem);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   OnRegisterToDivision
    @return     void
    @param      list<CExtDivision*>&    oDivisionList
    @brief
******************************************************************************/
void CaDraw_DimTagItem::OnRegisterToDivision(list<CExtDivision*>& oDivisionList)
{
	double dXLen = DBL_MAX , dYLen = DBL_MAX;
	CExtDivision *pXDivision[2] = {NULL,NULL} , *pYDivision[2] = {NULL,NULL};
	
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	for(list<CExtDivision*>::iterator itr = oDivisionList.begin();itr != oDivisionList.end();++itr)
	{
		if(fabs(1 - fabs(vecDir.dx())) < 0.001)
		{
			// x dir test
			if(((*itr)->IsCrossingXRay(m_ptOrigin)) && 
				((CaDraw_DimTagItem::DIM_NEAR_SIDE == m_iTagDir) || (CaDraw_DimTagItem::DIM_LEFT_SIDE & m_iTagDir) || (CaDraw_DimTagItem::DIM_RIGHT_SIDE & m_iTagDir)))
			{
				const double dLen = (*itr)->GetXDistance(m_ptOrigin , true);
				if((m_iTagDir & CaDraw_DimTagItem::DIM_LEFT_SIDE) && (-1.f == (*itr)->m_vecTagDir.dx()))
				{
					pXDivision[0] = (*itr);
				}
				if((m_iTagDir & CaDraw_DimTagItem::DIM_RIGHT_SIDE) && (1.f == (*itr)->m_vecTagDir.dx()))
				{
					pXDivision[1] = (*itr);
				}
				if(m_iTagDir & CaDraw_DimTagItem::DIM_NEAR_SIDE)
				{
					if(fabs(dLen) < dXLen)
					{
						dXLen = fabs(dLen);
						pXDivision[0] = (*itr);
					}
				}
			}
		}
		
		if((1 - fabs(vecDir.dy())) < 0.001)
		{
			/// y dir test
			if(((*itr)->IsCrossingYRay(m_ptOrigin)) &&
				((CaDraw_DimTagItem::DIM_NEAR_SIDE == m_iTagDir) || (CaDraw_DimTagItem::DIM_TOP_SIDE & m_iTagDir) || (CaDraw_DimTagItem::DIM_BOTTOM_SIDE & m_iTagDir)))
			{
				const double dLen = (*itr)->GetYDistance(m_ptOrigin , false);
				if((m_iTagDir & CaDraw_DimTagItem::DIM_TOP_SIDE) && (1.f == (*itr)->m_vecTagDir.dy()))
				{
					pYDivision[0] = (*itr);
				}
				if((m_iTagDir & CaDraw_DimTagItem::DIM_BOTTOM_SIDE) && (-1.f == (*itr)->m_vecTagDir.dy()))
				{
					pYDivision[1] = (*itr);
				}
				if(m_iTagDir & CaDraw_DimTagItem::DIM_NEAR_SIDE)
				{
					if(fabs(dLen) < dYLen)
					{
						dYLen = fabs(dLen);
						pYDivision[0] = (*itr);
					}
				}
			}
		}
	}

	if(NULL != pXDivision[0]) pXDivision[0]->Register(this);
	if(NULL != pXDivision[1]) pXDivision[1]->Register(this);
	if(NULL != pYDivision[0]) pYDivision[0]->Register(this);
	if(NULL != pYDivision[1]) pYDivision[1]->Register(this);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   OnCreateTagPos
    @return     int
    @param      vector<CaDraw_TagPos*>& oTagPosList
    @param      CExtDivision *pDivision
    @brief
******************************************************************************/
int CaDraw_DimTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& oTagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");
	oTagPosList.clear();

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(NULL != pDivision)
	{
		CaDraw_TagPos *pRet = new CaDraw_TagPos;
		oTagPosList.push_back(pRet);
		m_pTagPosList->push_back(pRet);

		if(NULL != pRet)
		{
			pRet->nLevelOn = DIM_LEVEL_0;
			if(pDivision->IsVertical())
			{
				pRet->m_ptStart.x()	= pDivision->GetBaseLine().start().x();
				pRet->m_ptStart.y()	= m_ptOrigin.y();
				pRet->rect.width	= 0;
				pRet->nOverlap		= 0;
				pRet->type		= DIM_TAG;
				pRet->pTITEM		= this;
			
				pRet->rect.height= env.m_stru.m_dRadius*2;			
				pRet->m_ptEnd.x() = pRet->m_ptStart.x();
				pRet->m_ptEnd.y() = pRet->m_ptStart.y();
			}
			else
			{
				pRet->m_ptStart.x()    = m_ptOrigin.x();
				pRet->m_ptStart.y()    = pDivision->GetBaseLine().start().y();
				pRet->rect.width = 0;
				pRet->nOverlap   = 0;
				pRet->type       = DIM_TAG;
				pRet->pTITEM	 = this;

				pRet->rect.height= env.m_stru.m_dRadius*2;
				pRet->m_ptEnd.x() = pRet->m_ptStart.x();
				pRet->m_ptEnd.y() = pRet->m_ptStart.y();
			}
		}
	}

	return ERROR_SUCCESS;
}