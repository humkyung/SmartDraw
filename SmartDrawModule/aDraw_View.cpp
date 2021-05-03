#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <IsString.h>
#include <Tokenizer.h>
#include "aDraw_Structure.h"
#include "aDraw_Equipment.h"
#include "aDraw_View.h"
#include "aDraw_DraFile.h"

#include <sstream>
#include <fstream>
using namespace std;

ADRAW_IMPLEMENT_FUNC(CaDraw_View , CaDraw_Entity , _T("CaDraw_View"))

CaDraw_View::CaDraw_View() : m_nIndex(-1) , m_scale(1.f)
{
	m_direction = CaDraw_View::TOP_VIEW_T;
	m_rotate = CaDraw_View::R0_T;
	m_scale = 10;

	m_nViewWidth = m_nViewHeight = 0.f;

	m_pModelList = new list<MODEL_T>;
	m_pEntMap    = new map<STRING_T , list<CaDraw_Entity*>*>;
	m_pViewInfo  = new STRING_T;
	
	memset(m_szViewMatch[UP_SIDE_T]   , '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
	memset(m_szViewMatch[DOWN_SIDE_T] , '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
	memset(m_szViewMatch[EAST_SIDE_T] , '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
	memset(m_szViewMatch[WEST_SIDE_T] , '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
	memset(m_szViewMatch[NORTH_SIDE_T], '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
	memset(m_szViewMatch[SOUTH_SIDE_T], '\0' , sizeof(TCHAR)*(VIEW_MATCH_BUF_SIZ + 1));
}

CaDraw_View::~CaDraw_View(void)
{
	try
	{
		if(m_pModelList) SAFE_DELETE(m_pModelList);
		if(m_pEntMap)  
		{
			for(map<STRING_T , list<CaDraw_Entity*>*>::iterator itr = m_pEntMap->begin();itr != m_pEntMap->end();++itr)
			{
				for(list<CaDraw_Entity*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
				{
					SAFE_DELETE(*jtr);
				}
				SAFE_DELETE(itr->second);
			}
			SAFE_DELETE(m_pEntMap);
		}

		if(m_pViewInfo) SAFE_DELETE(m_pViewInfo);
	}
	catch(...)
	{
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_View::Parse(CMarkup& xml)
{
	vector<STRING_T> oResult;
	double nViewWidth = 0.f , nViewHeight = 0.f;
	
	int iCount = 0;
	while ( 1 )
	{
		STRING_T sName = xml.GetAttribName(iCount++);
		if ( sName.empty() ) break;
		STRING_T sValue = xml.GetAttrib( sName );

		if(0 == STRICMP_T(sName.c_str() , _T("Name")))
		{
			(*m_pName) = sValue;
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Volume")))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
			if(6 == oResult.size())
			{
				m_volume.Set(
					ATOF_T(oResult[0].c_str()) , ATOF_T(oResult[1].c_str()) , ATOF_T(oResult[2].c_str()) ,
					ATOF_T(oResult[3].c_str()) , ATOF_T(oResult[4].c_str()) , ATOF_T(oResult[5].c_str()));
			}
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Direction")))
		{
			m_direction = CaDraw_View::GetViewDirFrom(sValue);
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Scale")))
		{
			m_scale = ATOF_T(sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Rotate")))
		{
			m_rotate = CaDraw_View::GetViewAngleFrom(sValue);
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("ViewOrigin")))	/// it's lower left value
		{
			CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
			if(2 == oResult.size())
			{
				m_ptOrigin.Set(ATOF_T(oResult[0].c_str()) , ATOF_T(oResult[1].c_str()) , 0.f);
			}
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineUpData")))
		{
			STRCPY_T(m_szViewMatch[UP_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineDownData")))
		{
			STRCPY_T(m_szViewMatch[DOWN_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineEastData")))
		{
			STRCPY_T(m_szViewMatch[EAST_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineWestData")))
		{
			STRCPY_T(m_szViewMatch[WEST_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineNorthData")))
		{
			STRCPY_T(m_szViewMatch[NORTH_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("MatchLineSouthData")))
		{
			STRCPY_T(m_szViewMatch[SOUTH_SIDE_T] , sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("ViewInfo")))
		{
			(*m_pViewInfo) = sValue;
		}
	}

	switch(m_direction)
	{
		case TOP_VIEW_T:
			nViewWidth = m_volume.maxx() - m_volume.minx();
			nViewHeight= m_volume.maxy() - m_volume.miny();
			break;
		case BOTTOM_VIEW_T:
			nViewWidth = m_volume.maxx() - m_volume.minx();
			nViewHeight= m_volume.maxy() - m_volume.miny();
			break;
		case LEFT_VIEW_T:
			nViewWidth = m_volume.maxy() - m_volume.miny();
			nViewHeight= m_volume.maxz() - m_volume.minz();
			break;
		case RIGHT_VIEW_T:
			nViewWidth = m_volume.maxy() - m_volume.miny();
			nViewHeight= m_volume.maxz() - m_volume.minz();
			break;
		case FRONT_VIEW_T:
			nViewWidth = m_volume.maxx() - m_volume.minx();
			nViewHeight= m_volume.maxz() - m_volume.minz();
			break;
		case BACK_VIEW_T:
			nViewWidth = m_volume.maxx() - m_volume.minx();
			nViewHeight= m_volume.maxz() - m_volume.minz();
			break;
	};
	m_nViewWidth = nViewWidth / m_scale;
	m_nViewHeight= nViewHeight/ m_scale;

	TCHAR tmp[VIEW_MATCH_BUF_SIZ + 1]={0,};
	switch(m_rotate)
	{
		case R90_T:
		{
			swap(m_nViewWidth , m_nViewHeight);
		}
		break;
		case R180_T:
		{
		}
		break;
		case R270_T:
		{
			swap(m_nViewWidth , m_nViewHeight);
		}
		break;
	};

	///m_ptOrigin.Set(m_ptOrigin.x() + m_nViewWidth*0.5 , m_ptOrigin.y() + m_nViewHeight*0.5 , 0.0);

	xml.IntoElem();
	while(true == xml.FindElem(MCD_T("REF")))
	{
		MODEL_T modelT;
		modelT.modelType = CaDraw_Entity::NONE_MODEL_T;	/// 2015.08.21 added by humkyung
		iCount = 0;
		while(1)
		{
			STRING_T sName = xml.GetAttribName(iCount++);
			if ( sName.empty() ) break;
			STRING_T sValue = xml.GetAttrib( sName );

			if(0 == STRICMP_T(sName.c_str() , _T("Type")))
			{
				try
				{
					modelT.modelType  = CaDraw_Entity::GetModelType(sValue);
				}
				catch(const exception& ex)
				{
					ex;
				}
			}
			else if(0 == STRICMP_T(sName.c_str() , _T("Name")))
			{
				modelT.rModelName = sValue;
			}
		}
		m_pModelList->push_back(modelT);
	}
	xml.OutOfElem();

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   Save
    @return     int
    @param      CMarkup&    xml
    @brief
******************************************************************************/
int CaDraw_View::Save(CMarkup& xml)
{
	OSTRINGSTREAM_T oss;

	xml.AddAttrib( _T("Name") , (*m_pName));
	
	oss.setf(ios_base::fixed, ios_base::floatfield);
	oss << IsString::TrimedTrailingZero(m_volume.minx()) << _T(",") << IsString::TrimedTrailingZero(m_volume.miny()) << _T(",") << IsString::TrimedTrailingZero(m_volume.minz()) << _T(",");
	oss << IsString::TrimedTrailingZero(m_volume.maxx()) << _T(",") << IsString::TrimedTrailingZero(m_volume.maxy()) << _T(",") << IsString::TrimedTrailingZero(m_volume.maxz());
	xml.AddAttrib( _T("Volume") , oss.str());
	
	xml.AddAttrib( _T("Direction") , this->GetViewDirString());

	oss.str(_T(""));
	oss << IsString::TrimedTrailingZero(m_scale);
	xml.AddAttrib( _T("Scale") , oss.str());

	xml.AddAttrib( _T("Rotate") , this->GetViewAngleString());

	oss.str(_T(""));
	oss << IsString::TrimedTrailingZero(m_ptOrigin.x()) << _T(",") << IsString::TrimedTrailingZero(m_ptOrigin.y());
	xml.AddAttrib( _T("ViewOrigin") , oss.str());

	xml.AddAttrib( _T("MatchLineUpData")    , m_szViewMatch[UP_SIDE_T]);
	xml.AddAttrib( _T("MatchLineDownData")  , m_szViewMatch[DOWN_SIDE_T]);
	xml.AddAttrib( _T("MatchLineEastData")  , m_szViewMatch[EAST_SIDE_T]);
	xml.AddAttrib( _T("MatchLineWestData")  , m_szViewMatch[WEST_SIDE_T]);
	xml.AddAttrib( _T("MatchLineNorthData") , m_szViewMatch[NORTH_SIDE_T]);
	xml.AddAttrib( _T("MatchLineSouthData") , m_szViewMatch[SOUTH_SIDE_T]);

	xml.AddAttrib( _T("ViewInfo") , (*m_pViewInfo));

	for(list<MODEL_T>::iterator itr = m_pModelList->begin();itr != m_pModelList->end();++itr)
	{
		xml.AddChildElem( _T("REF") );
		xml.IntoElem();
		xml.AddAttrib( _T("Type") , CaDraw_Entity::GetModelTypeString(itr->modelType));
		xml.AddAttrib( _T("Name") , itr->rModelName);
		xml.OutOfElem();
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_View::Copy(CaDraw_View* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_nIndex = unnamed->m_nIndex;
		(*m_pName)  = *(unnamed->m_pName);

		m_direction   = unnamed->m_direction;
		m_nViewWidth  = unnamed->m_nViewWidth;
		m_nViewHeight = unnamed->m_nViewHeight;
		m_rotate = unnamed->m_rotate;
		m_scale  = unnamed->m_scale;
		
		memcpy(m_szViewMatch[0] , unnamed->m_szViewMatch[0] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		memcpy(m_szViewMatch[1] , unnamed->m_szViewMatch[1] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		memcpy(m_szViewMatch[2] , unnamed->m_szViewMatch[2] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		memcpy(m_szViewMatch[3] , unnamed->m_szViewMatch[3] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		memcpy(m_szViewMatch[4] , unnamed->m_szViewMatch[4] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		memcpy(m_szViewMatch[5] , unnamed->m_szViewMatch[5] , sizeof(char)*(VIEW_MATCH_BUF_SIZ + 1));
		
		(*m_pViewInfo) = *(unnamed->m_pViewInfo);

		m_pModelList->clear();
		m_pModelList->insert(m_pModelList->begin() , unnamed->m_pModelList->begin() , unnamed->m_pModelList->end());
		
		
		for(map<STRING_T , list<CaDraw_Entity*>*>::iterator itr = unnamed->m_pEntMap->begin();itr != unnamed->m_pEntMap->end();++itr)
		{
			map<STRING_T , list<CaDraw_Entity*>*>::iterator where = m_pEntMap->find(itr->first);
			if(where == m_pEntMap->end())
			{
				m_pEntMap->insert(make_pair(itr->first , new list<CaDraw_Entity*>));
			}
			else
			{
				for(list<CaDraw_Entity*>::iterator jtr = where->second->begin();jtr != where->second->end();++jtr)
				{
					SAFE_DELETE(*jtr);
				}
				where->second->clear();
			}

			for(list<CaDraw_Entity*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
			{
				(*m_pEntMap)[itr->first]->push_back((*jtr)->Clone());
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_View
    @function   name
    @return     LPCTSTR
    @brief
******************************************************************************/
LPCTSTR CaDraw_View::name() const
{
	return m_pName->c_str();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_View
    @function   SetName
    @return     void
    @param      const   STRING_T&
    @param      rName
    @brief
******************************************************************************/
void CaDraw_View::SetName(const STRING_T& rName)
{
	(*m_pName) = rName;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
bool CaDraw_View::IsEntityIn(CaDraw_Entity* pEnt)
{
	if(m_volume.CollideWith(pEnt->GetVolume()))
	{
		for(list<MODEL_T>::iterator itr = m_pModelList->begin();itr != m_pModelList->end();++itr)
		{
			STRING_T::size_type at = pEnt->GetModelName().find_last_of(_T("\\"));
			STRING_T rModelName = pEnt->GetModelName().substr(at + 1);
			if((itr->modelType == pEnt->GetModelType()) && (0 == STRICMP_T(itr->rModelName.c_str() , rModelName.c_str())))
			{
				return true;
			}
		}
	}

	return false;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
int CaDraw_View::Add(CaDraw_Entity* pEnt)
{
	assert(pEnt && "pEnt is NULL");

	if(pEnt)
	{
		pEnt->SetView(this);
		
		map<STRING_T , list<CaDraw_Entity*>*>::iterator where = m_pEntMap->find(pEnt->typeString());
		if(where == m_pEntMap->end())
		{
			m_pEntMap->insert(make_pair(pEnt->typeString() , new list<CaDraw_Entity*>));
		}

		if(pEnt->IsKindOf(CaDraw_Structure::TypeString()))
		{
			CaDraw_Structure* pStru = static_cast<CaDraw_Structure*>(pEnt);

			list<CaDraw_Entity*>* pEntList = (*m_pEntMap)[pEnt->typeString()];
			list<CaDraw_Entity*>::iterator where = find_if(pEntList->begin() , pEntList->end() , CaDraw_Entity::DataFinder(pEnt));
			if(where == pEntList->end())
			{
				pEntList->push_back(pEnt);
			}
			else
			{
				return ERROR_BAD_ENVIRONMENT;
			}
		}
		else
		{
			(*m_pEntMap)[pEnt->typeString()]->push_back(pEnt);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
void CaDraw_View::SetScale(const double& nScale)
{
	m_scale = nScale;
}

/**	
	@brief	return view scale
	@author	HumKyung.BAEK
	@date	2010.05.25
	@param	
	@return
*/
double CaDraw_View::GetScale() const
{
	return m_scale;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
CIsPoint3d CaDraw_View::origin() const
{
	return m_ptOrigin;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
double CaDraw_View::GetViewWidth() const
{
	return m_nViewWidth;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
double CaDraw_View::GetViewHeight() const
{
	return m_nViewHeight;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
bool CaDraw_View::IsSameShapeView(CaDraw_View *pView)
{
	assert(pView && "pView is NULL");
	const double TOLER = 0.001;

	if(pView)
	{
		double x[2]={0.,},y[2]={0.,};
		double X[2]={0.,},Y[2]={0.,};
		
		x[0] = m_ptOrigin.x() - m_nViewWidth*0.5;
		x[1] = m_ptOrigin.x() + m_nViewWidth*0.5;
		y[0] = m_ptOrigin.y() - m_nViewHeight*0.5;
		y[1] = m_ptOrigin.y() + m_nViewHeight*0.5;
		
		X[0] = pView->m_ptOrigin.x() - pView->m_nViewWidth*0.5;
		X[1] = pView->m_ptOrigin.x() + pView->m_nViewWidth*0.5;
		Y[0] = pView->m_ptOrigin.y() - pView->m_nViewHeight*0.5;
		Y[1] = pView->m_ptOrigin.y() + pView->m_nViewHeight*0.5;
	
		double dx[2]={x[0] - X[0],x[1] - X[1]};
		double dy[2]={y[0] - Y[0],y[1] - Y[1]};
		if((fabs(dx[0]) < TOLER) && (fabs(dx[1]) < TOLER) &&
			(fabs(dy[0]) < TOLER) && (fabs(dy[1]) < TOLER)) return true;
	}
	
	return false;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
bool CaDraw_View::IsInsideViewOf(CaDraw_View* pView)
{
	assert(pView && "pView is NULL");

	if(pView)
	{
		double x[2]={0.,},y[2]={0.,};
		double X[2]={0.,},Y[2]={0.,};

		x[0] = m_ptOrigin.x() - m_nViewWidth*0.5;
		x[1] = m_ptOrigin.x() + m_nViewWidth*0.5;
		y[0] = m_ptOrigin.y() - m_nViewHeight*0.5;
		y[1] = m_ptOrigin.y() + m_nViewHeight*0.5;
		
		X[0] = pView->m_ptOrigin.x() - pView->m_nViewWidth*0.5;
		X[1] = pView->m_ptOrigin.x() + pView->m_nViewWidth*0.5;
		Y[0] = pView->m_ptOrigin.y() - pView->m_nViewHeight*0.5;
		Y[1] = pView->m_ptOrigin.y() + pView->m_nViewHeight*0.5;

		if(	(x[0] >= X[0]) && (x[1] <= X[1]) && 
			(y[0] >= Y[0]) && (y[1] <= Y[1])) return true;
	}

	return false;
}

/**
	@brief	create view boundary for draft view
	@author	humkyung
	@date	2010.05.25
	@param
	@return
*/
CViewBoundary* CaDraw_View::CreateViewBoundary(const int& iExtTagSide , const CaDraw_View::COORD_OPTION_E& coords)
{
	CViewBoundary* pRet=NULL;
	
	const double nTol = 0.f;
	double nWidth = GetViewWidth();
	double nHeight= GetViewHeight();
	CaDraw_View::ROTATE_T nRotate = GetViewAngle();
	const bool bReverse = ((CaDraw_View::R90_T == nRotate) || (CaDraw_View::R270_T == nRotate)) ? true : false;
	
	if(pRet = (CViewBoundary*)CViewBoundary::CreateInstance())
	{
		CViewBoundaySegment* node = NULL;
		
		node = new CViewBoundaySegment();
		/// BOTTOM SIDE
		if(node)
		{
			node->pView = this;
			node->pt = this->center();
			node->pt.Set(node->pt.x()- nWidth*0.5 + nTol , node->pt.y() - nHeight*0.5+ nTol , 0.f);
			
			node->render       = SHOW;
			node->nHeight[0] = node->nHeight[1] = 0.f;
			node->vecTagDir.Set(0.f , -1.f , 0.f);
			STRCPY_T(node->szViewMatchLine,this->GetViewMatchLine(BOTTOM_SIDE));
			
			node->nDivisionFlag = ((iExtTagSide) & CaDraw_DraFile::BOTTOM_EXT_SIDE) ? 1 : 0;
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_BOTTOMLEFT_TOPRIGHT == coords))
			{
				this->GetCoordText(node->szDim[0] , bReverse ? 2 : 1 , this->GetCoordValue(BOTTOM_LEFT));
			}
			else
			{
				node->szDim[0][0] = '\0';
			}

			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT == coords))
			{
				this->GetCoordText(node->szDim[1] , bReverse ? 2 : 1 , this->GetCoordValue(BOTTOM_RIGHT));
			}
			else
			{
				node->szDim[1][0] = '\0';
			}

			(*pRet) += node;
		}
		
		/// RIGHT SIDE
		if(node = new CViewBoundaySegment)
		{
			node->pView = this;
			node->pt   = this->center();
			node->pt.Set(node->pt.x() + nWidth*0.5  + nTol , node->pt.y() - nHeight*0.5 + nTol , 0.f);
			
			node->render       = SHOW;
			node->nHeight[0] = node->nHeight[1] = 0.f;
			node->vecTagDir.Set(1.f , 0.f , 0.f);
			STRCPY_T(node->szViewMatchLine,this->GetViewMatchLine(RIGHT_SIDE));

			node->nDivisionFlag = ((iExtTagSide) & CaDraw_DraFile::RIGHT_EXT_SIDE) ? 1 : 0;
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT == coords))
			{
				this->GetCoordText(node->szDim[0] , bReverse ? 1 : 2 , this->GetCoordValue(RIGHT_DOWN));
			}
			else
			{
				node->szDim[0][0] = '\0';
			}
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_BOTTOMLEFT_TOPRIGHT == coords))
			{
				this->GetCoordText(node->szDim[1] , bReverse ? 1 : 2 , this->GetCoordValue(RIGHT_UP));
			}
			else
			{
				node->szDim[1][0] = '\0';
			}

			(*pRet) += node;
		}
		
		/// TOP SIDE
		if(node = new CViewBoundaySegment)
		{
			node->pView = this;
			node->pt   = this->center();
			node->pt.Set(node->pt.x() + nWidth*0.5  + nTol , node->pt.y() + nHeight*0.5 + nTol , 0.f);
			
			node->render       = SHOW;
			node->nHeight[0] = node->nHeight[1] = 0.f;
			node->vecTagDir.Set(0.f , 1.f , 0.f);
			STRCPY_T(node->szViewMatchLine,this->GetViewMatchLine(TOP_SIDE));
						
			node->nDivisionFlag = ((iExtTagSide) & CaDraw_DraFile::TOP_EXT_SIDE) ? 1 : 0;
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT == coords))
			{
				this->GetCoordText(node->szDim[1] , bReverse ? 2 : 1 , this->GetCoordValue(TOP_LEFT));
			}
			else
			{
				node->szDim[0][0] = '\0';
			}
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_BOTTOMLEFT_TOPRIGHT == coords))
			{
				this->GetCoordText(node->szDim[0] , bReverse ? 2 : 1 , this->GetCoordValue(TOP_RIGHT));
			}
			else
			{
				node->szDim[0][0] = '\0';
			}
			
			(*pRet) += node;
		}
		
		/// LEFT SIDE
		if(node = new CViewBoundaySegment)
		{
			node->pView = this;
			node->pt   = this->center();
			node->pt.Set(node->pt.x() - nWidth*0.5  + nTol , node->pt.y() + nHeight*0.5 + nTol , 0.f);
			
			node->render       = SHOW;
			node->nHeight[0] = node->nHeight[1] = 0.f;
			node->vecTagDir.Set(-1.f , 0.f , 0.f);
			STRCPY_T(node->szViewMatchLine,this->GetViewMatchLine(LEFT_SIDE));
			
			node->nDivisionFlag = ((iExtTagSide) & CaDraw_DraFile::LEFT_EXT_SIDE) ? 1 : 0;
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_BOTTOMLEFT_TOPRIGHT == coords))
			{
				this->GetCoordText(node->szDim[1] , bReverse ? 1 : 2 , this->GetCoordValue(LEFT_DOWN));
			}
			else
			{
				node->szDim[1][0] = '\0';
			}
			if((CaDraw_View::COORD_ALL == coords) || (CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT == coords))
			{
				this->GetCoordText(node->szDim[0] , bReverse ? 1 : 2 , this->GetCoordValue(LEFT_UP));
			}
			else
			{
				node->szDim[0][0] = '\0';
			}

			(*pRet) += node;
		}
			
		{
			if(node = new CViewBoundaySegment)
			{	// end!!
				node->pView = this;
				node->pt = this->center();
				node->pt.Set(node->pt.x() - nWidth*0.5  + nTol , node->pt.y() - nHeight*0.5 + nTol , 0.f);
				
				node->render       = SHOW;
				node->nHeight[0] = node->nHeight[1] = 0.f;
				node->vecTagDir.Set(0.f , -1.f , 0.f);
				
				(*pRet) += node;
			}
		}
	}
	
	return pRet;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetViewDirFrom
    @return     DIRECTIONT_T
    @param      const       STRING_T&
    @param      sViewDir
    @brief
******************************************************************************/
CaDraw_View::DIRECTIONT_T CaDraw_View::GetViewDirFrom(const STRING_T& sViewDir)
{
	if(_T("Top") == sViewDir)
	{
		return TOP_VIEW_T;
	}
	else if(_T("Bottom") == sViewDir)
	{
		return BOTTOM_VIEW_T;
	}
	else if(_T("Left") == sViewDir)
	{
		return LEFT_VIEW_T;
	}
	else if(_T("Right") == sViewDir)
	{
		return RIGHT_VIEW_T;
	}
	else if(_T("Front") == sViewDir)
	{
		return FRONT_VIEW_T;
	}
	else if(_T("Back") == sViewDir)
	{
		return BACK_VIEW_T;
	}
	else if(_T("Iso1") == sViewDir)
	{
		return ISO1_VIEW_T;
	}
	else if(_T("Iso2") == sViewDir)
	{
		return ISO2_VIEW_T;
	}
	else if(_T("Iso3") == sViewDir)
	{
		return ISO3_VIEW_T;
	}

	throw exception("unexpected view dir");
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetViewDirString
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_View::GetViewDirString() const
{
	STRING_T sViewDirString;

	switch(m_direction)
	{
		case TOP_VIEW_T:
			sViewDirString = _T("Top");
			break;
		case BOTTOM_VIEW_T:
			sViewDirString = _T("Bottom");
			break;
		case LEFT_VIEW_T:
			sViewDirString = _T("Left");
			break;
		case RIGHT_VIEW_T:
			sViewDirString = _T("Right");
			break;
		case FRONT_VIEW_T:
			sViewDirString = _T("Front");
			break;
		case BACK_VIEW_T:
			sViewDirString = _T("Back");
			break;
		case ISO1_VIEW_T:
			sViewDirString = _T("Iso1");
			break;
		case ISO2_VIEW_T:
			sViewDirString = _T("Iso2");
			break;
		case ISO3_VIEW_T:
			sViewDirString = _T("Iso3");
			break;
	}

	return sViewDirString;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param

	@return
*/
CaDraw_View::DIRECTIONT_T CaDraw_View::GetViewDir() const
{
	return m_direction;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   SetViewDir
    @return     int
    @param      const   CaDraw_View::DIRECTIONT_T&
    @param      dir
    @brief
******************************************************************************/
int CaDraw_View::SetViewDir(const CaDraw_View::DIRECTIONT_T& dir)
{
	m_direction = dir;

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetViewAngleFrom
    @return     CaDraw_View::ROTATE_T
    @param      const       STRING_T&
    @param      sViewAngle
    @brief
******************************************************************************/
CaDraw_View::ROTATE_T CaDraw_View::GetViewAngleFrom(const STRING_T& sViewAngle)
{
	if(_T("0") == sViewAngle)
	{
		return R0_T;
	}
	else if(_T("90") == sViewAngle)
	{
		return R90_T;
	}
	else if(_T("180") == sViewAngle)
	{
		return R180_T;
	}
	else if(_T("270") == sViewAngle)
	{
		return R270_T;
	}

	throw exception("unexpected rotate value.");
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.25
	@param
	@return
*/
CaDraw_View::ROTATE_T CaDraw_View::GetViewAngle() const
{
	return m_rotate;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   SetViewAngle
    @return     int
    @param      const   CaDraw_View::ROTATE_T&
    @param      rot
    @brief
******************************************************************************/
int CaDraw_View::SetViewAngle(const CaDraw_View::ROTATE_T& rot)
{
	m_rotate = rot;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   GetViewAngleString
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_View::GetViewAngleString() const
{
	STRING_T sAngle = _T("0");

	switch(m_rotate)
	{
		case R0_T:
			sAngle = _T("0");
			break;
		case R90_T:
			sAngle = _T("90");
			break;
		case R180_T:
			sAngle = _T("180");
			break;
		case R270_T:
			sAngle = _T("270");
			break;
	}

	return sAngle;
}

/**
	@brief	return match line data corresponding to given view side
	@author	HumKyung.BAEK
	@date	2010.05.25
	@param
	@return	const TCHAR*
*/
const TCHAR* CaDraw_View::GetViewMatchLine(const int& nSide) const
{
	assert((nSide >= 0) && (nSide <= 3));

	if((nSide >= 0) && (nSide <= 3))
	{
		static TCHAR szViewMatch[4][VIEW_MATCH_BUF_SIZ + 1];

		switch(m_direction)
		{
			case TOP_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[SOUTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[EAST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[NORTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[WEST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
			case BOTTOM_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[NORTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[EAST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[SOUTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[WEST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
			case LEFT_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[DOWN_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[SOUTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[UP_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[NORTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
			case RIGHT_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[DOWN_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[NORTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[UP_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[SOUTH_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
			case FRONT_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[DOWN_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[EAST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[UP_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[WEST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
			case BACK_VIEW_T:
				STRNCPY_T(szViewMatch[BOTTOM_SIDE] , m_szViewMatch[DOWN_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[BOTTOM_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[RIGHT_SIDE] , m_szViewMatch[WEST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[RIGHT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[TOP_SIDE] , m_szViewMatch[UP_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[TOP_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				STRNCPY_T(szViewMatch[LEFT_SIDE] , m_szViewMatch[EAST_SIDE_T] , VIEW_MATCH_BUF_SIZ);
				szViewMatch[LEFT_SIDE][VIEW_MATCH_BUF_SIZ] = '\0';
				break;
		};

		TCHAR tmp[VIEW_MATCH_BUF_SIZ + 1]={0,};
		switch(m_rotate)
		{
			case R90_T:
				{
					STRCPY_T(tmp , szViewMatch[0]);
					int i = 0;
					for(i = 0;i < 3;++i)
					{
						const int n = (i+1)%4;
						STRCPY_T(szViewMatch[i],szViewMatch[n]); 
					}
					STRCPY_T(szViewMatch[i] , tmp); 
				}
				break;
			case R180_T:
				{
					STRCPY_T(tmp , szViewMatch[0]);
					STRCPY_T(szViewMatch[0] , szViewMatch[2]);
					STRCPY_T(szViewMatch[2] , tmp);
					STRCPY_T(tmp , szViewMatch[1]);
					STRCPY_T(szViewMatch[1] , szViewMatch[3]);
					STRCPY_T(szViewMatch[3] , tmp);
				}
				break;
			case R270_T:
				{
					STRCPY_T(tmp,szViewMatch[3]);
					int i = 0;
					for(i = 3;i > 0;--i)
					{
						const int n = (i-1)%4;
						STRCPY_T(szViewMatch[i],szViewMatch[n]);
					}
					STRCPY_T(szViewMatch[i],tmp); 
				}
				break;
		};

		return szViewMatch[nSide];
	}

	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetMatchLine
    @return     TCHAR*
    @param      const   int&
    @param      nSide
    @brief
******************************************************************************/
const TCHAR* CaDraw_View::GetMatchLineData(const int& nSide) const
{
	assert((nSide >= 0) && (nSide <= 5));

	if((nSide >= 0) && (nSide <= 5))
	{
		return (m_szViewMatch[nSide]);
	}

	return _T("");
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   SetMatchLineData
    @return     int
    @param      const   int&
    @param      nSide   const
    @param      TCHAR*  pData
    @brief
******************************************************************************/
int CaDraw_View::SetMatchLineData(const int& nSide , const TCHAR* pData)
{
	assert((nSide >= 0) && (nSide <= 5));

	if((nSide >= 0) && (nSide <= 5))
	{
		STRCPY_T(m_szViewMatch[nSide] , pData);
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param

	@return
*/
int CaDraw_View::GetEntList(list<CaDraw_Entity*>* pEntList) const
{
	assert(pEntList && "pEntList is NULL");

	if(pEntList)
	{
		pEntList->clear();

		for(map<STRING_T , list<CaDraw_Entity*>*>::iterator itr = m_pEntMap->begin();itr != m_pEntMap->end();++itr)
		{
			pEntList->insert(pEntList->begin() , itr->second->begin() , itr->second->end());
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.26

	@param

	@return
*/
CIsPoint3d CaDraw_View::ModelCoord2ViewCoord(const CIsPoint3d& pt)
{
	CIsPoint3d center(m_volume.center());
	CIsPoint3d _pt(pt);
	_pt -= center;

	double angle = 0.f;
	switch(m_rotate)
	{
		case R0_T:
			angle = 0.f;
			break;
		case R90_T:
			angle = DEG2RAD(90.f);
			break;
		case R180_T:
			angle = DEG2RAD(180.f);
			break;
		case R270_T:
			angle = DEG2RAD(270.f);
			break;
		default:
			throw exception("unexpected rotate value");
			break;
	}
	///const double cs=cos(angle);
	///const double sn=sin(angle);
	CIsMatrix3d mat3d = GetRMatrix();
	_pt = mat3d*_pt;
	_pt = _pt.RotateAboutZAxis(angle);
	_pt = this->center() + _pt / m_scale;

	return _pt;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.11

	@param

	@return
*/
double CaDraw_View::ModelCoord2ViewCoord(const double& value)
{
	return (value / m_scale);
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.12

	@param

	@return
*/
CIsVolume CaDraw_View::ModelCoord2ViewCoord(const CIsVolume& volume)
{
	CIsVolume ret;

	CIsPoint3d pt[8];
	pt[0].Set(volume.minx() , volume.miny() , volume.minz());
	pt[1].Set(volume.maxx() , volume.miny() , volume.minz());
	pt[2].Set(volume.maxx() , volume.maxy() , volume.minz());
	pt[3].Set(volume.minx() , volume.maxy() , volume.minz());
	pt[4].Set(volume.minx() , volume.miny() , volume.maxz());
	pt[5].Set(volume.maxx() , volume.miny() , volume.maxz());
	pt[6].Set(volume.maxx() , volume.maxy() , volume.maxz());
	pt[7].Set(volume.minx() , volume.maxy() , volume.maxz());
	for(int i = 0;i < 8;++i)
	{
		ret.Add(ModelCoord2ViewCoord(pt[i]));
	}

	return ret;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-19
    @class      CaDraw_View
    @function   ViewCoord2ModelCoord
    @return     CIsPoint3d
    @param      const CIsPoint3d& pt
    @brief		pt should have z value in order to conrrectly conversion
******************************************************************************/
CIsPoint3d CaDraw_View::ViewCoord2ModelCoord(const CIsPoint3d& pt)
{
	CIsPoint3d _pt(pt);

	_pt -= this->center();
	_pt = _pt*m_scale;

	double angle = 0.f;
	switch(m_rotate)
	{
		case R0_T:
			angle = 0.f;
			break;
		case R90_T:
			angle = DEG2RAD(90.f);
			break;
		case R180_T:
			angle = DEG2RAD(180.f);
			break;
		case R270_T:
			angle = DEG2RAD(270.f);
			break;
		default:
			throw exception("unexpected rotate value");
			break;
	}
	_pt = _pt.RotateAboutZAxis(-angle);

	CIsMatrix3d mat3d = this->GetRMatrix();
	mat3d = mat3d.Inverse();
	_pt = mat3d*_pt;

	CIsPoint3d center(m_volume.center());
	_pt += center;

	return _pt;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.26

	@param

	@return
*/
CIsMatrix3d CaDraw_View::GetRMatrix(void)
{
	CIsMatrix3d rmat;

	switch(m_direction)
	{
		case TOP_VIEW_T:
			rmat.Set(
				1.f , 0.f , 0.f ,
				0.f , 1.f , 0.f ,
				0.f , 0.f , 1.f );
			break;
		case BOTTOM_VIEW_T:
			rmat.Set(
				1.f , 0.f  , 0.f ,
				0.f , -1.f , 1.f ,
				0.f , 0.f  , 1.f );
			break;
		case LEFT_VIEW_T:
			rmat.Set(
				0.f , -1.f, 0.f ,
				0.f , 0.f , 1.f ,
				-1.f, 0.f , 0.f );
			break;
		case RIGHT_VIEW_T:
			rmat.Set(
				0.f , 1.f , 0.f ,
				0.f , 0.f , 1.f ,
				1.f , 0.f , 0.f );
			break;
		case FRONT_VIEW_T:
			rmat.Set(
				1.f , 0.f , 0.f ,
				0.f , 0.f , 1.f ,
				0.f , -1.f, 0.f );
			break;
		case BACK_VIEW_T:
			rmat.Set(
				-1.f, 0.f, 0.f ,
				0.f , 0.f, 1.f ,
				0.f , 1.f, 0.f );
			break;
		case ISO1_VIEW_T: case ISO2_VIEW_T: case ISO3_VIEW_T:
			break;
		default:
			throw exception("unexpected view direction");
			break;
	}

	return rmat;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.26

	@param

	@return
*/
double CaDraw_View::GetViewTopElev(void)
{
	CIsPoint3d pts[6];
	pts[0] = m_volume.center();
	pts[0]+= CIsPoint3d(0.f , 0.f , (m_volume.maxz() - m_volume.minz())*0.5);
	pts[1] = m_volume.center();
	pts[1]+= CIsPoint3d(0.f , 0.f , -(m_volume.maxz() - m_volume.minz())*0.5);
	pts[2] = m_volume.center();
	pts[2]+= CIsPoint3d(-(m_volume.maxx() - m_volume.minx())*0.5 , 0.f , 0.f);
	pts[3] = m_volume.center();
	pts[3]+= CIsPoint3d((m_volume.maxx() - m_volume.minx())*0.5 , 0.f , 0.f);
	pts[4] = m_volume.center();
	pts[4]+= CIsPoint3d(0.f , -(m_volume.maxy() - m_volume.miny())*0.5 , 0.f);
	pts[5] = m_volume.center();
	pts[5]+= CIsPoint3d(0.f , (m_volume.maxy() - m_volume.miny())*0.5 , 0.f);

	for(int i = 0;i < 6;++i)
	{
		pts[i] = ModelCoord2ViewCoord(pts[i]);
	}

	double nViewTopElev = 0.f;
	for(int i = 0;i < 6;++i)
	{
		if(0 == i)
			nViewTopElev = pts[i].z();
		else if(nViewTopElev < pts[i].z())
			nViewTopElev = pts[i].z();
	}

	return nViewTopElev;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.26

	@param

	@return
*/
const TCHAR* CaDraw_View::GetViewInfo(void) const
{
	return m_pViewInfo->c_str();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   SetViewInfo
    @return     int
    @param      const   TCHAR*
    @param      p
    @brief
******************************************************************************/
int CaDraw_View::SetViewInfo(const TCHAR* p)
{
	assert(p && "p is NULL");

	if(p)
	{
		(*m_pViewInfo) = p;
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetModelList
    @return     list<MODEL_T>*
    @brief
******************************************************************************/
list<CaDraw_View::MODEL_T>* CaDraw_View::GetModelList() const
{
	return m_pModelList;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   SetModelList
    @return     int
    @param      list<MODEL_T>*  pModelList
    @brief
******************************************************************************/
int CaDraw_View::SetModelList(list<MODEL_T>* pModelList)
{
	assert(pModelList && "pModelList is NULL");

	if(pModelList)
	{
		m_pModelList->clear();
		m_pModelList->insert(m_pModelList->begin() , pModelList->begin() , pModelList->end());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CaDraw_View
    @function   DoesIncludeVolume
    @return     bool
    @param      const   CIsVolume&
    @param      volume
    @brief
******************************************************************************/
bool CaDraw_View::DoesIncludeVolume(const CIsVolume& volume)
{
	if(m_volume.CollideWith(volume)) return true;
	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @function   center
    @return     CIsPoint3d
    @brief
******************************************************************************/
CIsPoint3d CaDraw_View::center() const
{
	const double x = m_ptOrigin.x() + m_nViewWidth*0.5;
	const double y = m_ptOrigin.y() + m_nViewHeight*0.5;
	const double z = 0.0;

	return CIsPoint3d(x,y,z);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_View
    @function   IsPlanView
    @return     bool
    @param      void
    @brief
******************************************************************************/
bool CaDraw_View::IsPlanView(void)
{
	return ((CaDraw_View::TOP_VIEW_T == m_direction) || (CaDraw_View::BOTTOM_VIEW_T == m_direction));
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_View
    @function   IsSectionView
    @return     bool
    @param      void
    @brief
******************************************************************************/
bool CaDraw_View::IsSectionView(void)
{
	return ((CaDraw_View::FRONT_VIEW_T == m_direction) || (CaDraw_View::BACK_VIEW_T == m_direction) ||
		(CaDraw_View::LEFT_VIEW_T == m_direction) || (CaDraw_View::RIGHT_VIEW_T == m_direction));
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   GetCoordText
    @return     void
    @param      TCHAR    *pCoordText
    @param      int     nDir
    @param      double  nVal
    @brief
******************************************************************************/
void CaDraw_View::GetCoordText(TCHAR *pCoordText, int nDir, double nVal)
{
	assert(pCoordText && "pCoordText is NULL");
	
	if(pCoordText)
	{
		CaDraw_View::DIRECTIONT_T dirT = this->GetViewDir();
		const double uor = 1.f;
		if(CaDraw_View::TOP_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// E or W
				if(nVal > 0.)
					SPRINTF_T(pCoordText , _T("E. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("W. %d") , abs(iVal));
			}
			else if(2 == nDir)
			{	// N or S
				if(nVal > 0.)
					SPRINTF_T(pCoordText , _T("N. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("S. %d") , abs(iVal));
			}
		}
		else if(CaDraw_View::BOTTOM_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// E or W
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("E. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("W. %d") , abs(iVal));
			}else if(2 == nDir){	// S or N
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("N. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("S. %d") , abs(iVal));
			}
		}
		else if(CaDraw_View::FRONT_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// East - West
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("E. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("W. %d") , abs(iVal));
			}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
		}
		else if(CaDraw_View::BACK_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// West - East
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("E. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("W. %d") , abs(iVal));
			}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
		}
		else if(CaDraw_View::LEFT_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// South - North
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("N. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("S. %d") , abs(iVal));
			}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
		}
		else if(CaDraw_View::RIGHT_VIEW_T == dirT)
		{
			const int iVal = int(SAFE_ROUND(nVal / uor , 1));
			if(1 == nDir)
			{		// North - South
				if(nVal >= 0)
					SPRINTF_T(pCoordText , _T("N. %d") , iVal);
				else	SPRINTF_T(pCoordText , _T("S. %d") , abs(iVal));
			}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
		}
	}
}

/**
	@brief	GetCoordValue:

	@author	humkyung

	@date	????.??.??

	@param	where 

	@return const double 
 */
const double CaDraw_View::GetCoordValue(const int& where)
{
	double nCoord=0.;
	
	CIsVolume volume = this->GetVolume();
	CaDraw_View::DIRECTIONT_T nViewType = this->GetViewDir();
	
	const double nAllCoords[8][6]=
	{
	///       TOP_VIEW  ,BOTTOM_VIEW,FRONT_VIEW , BACK_VIEW , LEFT_VIEW ,RIGHT_VIEW
		{volume.minx(),volume.minx(),volume.minx(),volume.maxx(),volume.maxy(),volume.miny()}, /// BOTTOM_LEFT
		{volume.maxx(),volume.maxx(),volume.maxx(),volume.minx(),volume.miny(),volume.maxy()}, /// BOTTOM_RIGHT
		{volume.miny(),volume.maxy(),volume.minz(),volume.minz(),volume.minz(),volume.minz()}, /// RIGHT_DOWN
		{volume.maxy(),volume.miny(),volume.maxz(),volume.maxz(),volume.maxz(),volume.maxz()}, /// RIGHT_UP
		{volume.maxx(),volume.maxx(),volume.maxx(),volume.minx(),volume.miny(),volume.maxy()}, /// TOP_RIGHT
		{volume.minx(),volume.minx(),volume.minx(),volume.maxx(),volume.maxy(),volume.miny()}, /// TOP_LEFT
		{volume.maxy(),volume.miny(),volume.maxz(),volume.maxz(),volume.maxz(),volume.maxz()}, /// LEFT_UP
		{volume.miny(),volume.maxy(),volume.minz(),volume.minz(),volume.minz(),volume.minz()}  /// LEFT_DOWN
	};

	double nCoords[8]={0.,};
	for(int i=0;i < 8;i++) nCoords[i] = nAllCoords[i][nViewType];
	
	double nRotate = this->GetViewAngle();
	double nTmp[2]={0,};
	if(fabs(nRotate - 90.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	else if(fabs(nRotate - 180.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	else if(fabs(nRotate - 270.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	
	nCoord = nCoords[where];
	
	return nCoord;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   FindEquipmentByName
    @return     CaDraw_Equipment*
    @param      const STRING_T& name
    @brief
******************************************************************************/
CaDraw_Equipment* CaDraw_View::FindEquipmentByName( const STRING_T& name )
{
	CaDraw_Equipment* res = NULL;

	map<STRING_T , list<CaDraw_Entity*>*>::iterator where = m_pEntMap->find(CaDraw_Equipment::TypeString());
	if(where != m_pEntMap->end())
	{
		for(list<CaDraw_Entity*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
		{
			CaDraw_Equipment* pEqui = static_cast<CaDraw_Equipment*>(*itr);
			if(name == STRING_T(pEqui->name()))
			{
				res = pEqui;
				break;
			}
		}
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CaDraw_View
    @function   Dump
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CaDraw_View::Report(soci::session& oSession,const STRING_T& sDocGuid) const
{
	CIsPoint3d center = m_volume.center();

	OSTRINGSTREAM_T oss;
	const STRING_T sViewGuid(NewGuid());
	oss << _T("insert into View values('") << sViewGuid << _T("','") << sDocGuid << _T("','");
	oss << (*m_pName) << _T("',");
	oss << IsString::TrimedTrailingZero(center.x()) << _T(",") << IsString::TrimedTrailingZero(center.y()) << _T(",") << IsString::TrimedTrailingZero(center.z()) << _T(",");
	oss << IsString::TrimedTrailingZero((m_volume.maxx() - m_volume.minx())) << _T(",");
	oss << IsString::TrimedTrailingZero((m_volume.maxy() - m_volume.miny())) << _T(",");
	oss << IsString::TrimedTrailingZero((m_volume.maxz() - m_volume.minz())) << _T(",");
	oss << IsString::TrimedTrailingZero(this->center().x()) << _T(",") << IsString::TrimedTrailingZero(this->center().y()) << _T(",0,");
	oss  << IsString::TrimedTrailingZero(m_nViewWidth) << _T(",") << IsString::TrimedTrailingZero(m_nViewHeight) << _T(",");
	oss << IsString::TrimedTrailingZero(m_scale) << _T(",");
	oss << IsString::TrimedTrailingZero(m_rotate) << _T(",'");
	oss << this->GetViewDirString() << _T("','");
	oss << m_szViewMatch[0] << _T("','") << m_szViewMatch[1] << _T("','") << m_szViewMatch[2] << _T("','");
	oss << m_szViewMatch[3] << _T("','") << m_szViewMatch[4] << _T("','") << m_szViewMatch[5] << _T("','");
	oss << (*m_pViewInfo) << _T("')");
	oSession << oss.str();

	for(map<STRING_T , list<CaDraw_Entity*>*>::const_iterator itr = m_pEntMap->begin();itr != m_pEntMap->end();++itr)
	{
		for(list<CaDraw_Entity*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			(*jtr)->Report(oSession,sViewGuid);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
@brief		return status of view
@author     humkyung
@date       2014-06-18
@class      CaDraw_View
@function   status
@return     int
******************************************************************************/
int CaDraw_View::status() const
{
	int res = 0;

	for(list<MODEL_T>::const_iterator itr = m_pModelList->begin();itr != m_pModelList->end();++itr)
	{
		/*if(GetFileAttributes(itr->rModelName.c_str()) == INVALID_FILE_ATTRIBUTES)
		{
			res |= CaDraw_View::NO_MODEL_FILE;
		}*/
		for(list<MODEL_T>::const_iterator jtr = m_pModelList->begin();jtr != m_pModelList->end();++jtr)
		{
			if(itr == jtr) continue;
			if((itr->modelType == jtr->modelType) && (itr->rModelName == jtr->rModelName))
			{
				res |= CaDraw_View::DUPLICATED_MODEL_FILE;
			}
		}
	}

	return res;
}

/******************************************************************************
@brief		return which side is adjacent to given view, return NONE if don't adjacent.
@author     humkyung
@date       2016.03.07
@return     int
******************************************************************************/
int CaDraw_View::IsAdjacent(const CaDraw_View* rhs,const double dToler) const
{
	if(m_volume.maxx() + dToler < rhs->GetVolume().minx()) return NONE_SIDE_T;
	if(m_volume.maxy() + dToler < rhs->GetVolume().miny()) return NONE_SIDE_T;
	if(m_volume.maxz() + dToler < rhs->GetVolume().minz()) return NONE_SIDE_T;
	if(m_volume.minx() - dToler > rhs->GetVolume().maxx()) return NONE_SIDE_T;
	if(m_volume.miny() - dToler > rhs->GetVolume().maxy()) return NONE_SIDE_T;
	if(m_volume.minz() - dToler > rhs->GetVolume().maxz()) return NONE_SIDE_T;

	if(std::fabs(m_volume.minx() - rhs->GetVolume().maxx()) < dToler) return WEST_SIDE_T;
	if(std::fabs(m_volume.maxx() - rhs->GetVolume().minx()) < dToler) return EAST_SIDE_T;
	if(std::fabs(m_volume.miny() - rhs->GetVolume().maxy()) < dToler) return SOUTH_SIDE_T;
	if(std::fabs(m_volume.maxy() - rhs->GetVolume().miny()) < dToler) return NORTH_SIDE_T;
	if(std::fabs(m_volume.minz() - rhs->GetVolume().maxz()) < dToler) return DOWN_SIDE_T;
	if(std::fabs(m_volume.maxz() - rhs->GetVolume().minz()) < dToler) return UP_SIDE_T;
		
	return NONE_SIDE_T;
}

bool CaDraw_View::IsEnable(AnnoFilters*) const
{
	return true;
}