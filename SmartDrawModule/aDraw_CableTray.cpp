#include "StdAfx.h"
#include <tchar.h>
#include <IsString.h>

#include <fstream>
#include "aDraw_View.h"
#include "aDraw_CableTray.h"

using namespace std;

ADRAW_IMPLEMENT_FUNC(CaDraw_CableTray , CaDraw_Entity , _T("CaDraw_CableTray"))

CaDraw_CableTray::CaDraw_CableTray(void)
{
	m_eType = CaDraw_CableTray::NONE_T;
	m_eSubType = CaDraw_CableTray::NONE_ST;
	m_eCategory= CaDraw_CableTray::ELECTRIC;

	m_width[0] = m_width[1] = 0.0;
	m_dAngle = 0.0;
	
	m_pEdgeList = new vector<CIsPoint3d>;
}

CaDraw_CableTray::~CaDraw_CableTray(void)
{
	try
	{
		SAFE_DELETE( m_pEdgeList );
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
CIsPoint3d CaDraw_CableTray::endPoint() const
{
	return m_pt[1];
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
CIsPoint3d CaDraw_CableTray::startPoint() const
{
	return m_pt[0];
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   Get3rdPoint
    @return     CIsPoint3d
    @brief
******************************************************************************/
CIsPoint3d CaDraw_CableTray::Get3rdPoint() const
{
	return m_pt[2];
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
double CaDraw_CableTray::height() const
{
	return m_height;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
double CaDraw_CableTray::width1() const
{
	return m_width[0];
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   width2
    @return     double
    @brief
******************************************************************************/
double CaDraw_CableTray::width2() const
{
	return m_width[1];
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_CableTray::SetEndPoint(const CIsPoint3d& pt)
{
	m_pt[1] = pt;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_CableTray::SetStartPoint(const CIsPoint3d& pt)
{
	m_pt[0] = pt;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   Set3rdPoint
    @return     int
    @param      const   CIsPoint3d&
    @param      pt
    @brief
******************************************************************************/
int CaDraw_CableTray::Set3rdPoint(const CIsPoint3d& pt)
{
	m_pt[2] = pt;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_CableTray::SetHeight(const double& height)
{
	m_height = height;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_CableTray::SetWidth1(const double& width)
{
	m_width[0] = width;

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   SetWidth2
    @return     int
    @param      const   double&
    @param      width
    @brief
******************************************************************************/
int CaDraw_CableTray::SetWidth2(const double& width)
{
	m_width[1] = width;

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   GetType
    @return     ELEMENT_E
    @brief
******************************************************************************/
CaDraw_CableTray::ELEMENT_E CaDraw_CableTray::GetType() const
{
	return m_eType;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetTypeString
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_CableTray::GetTypeString() const
{
	STRING_T sType;

	if(CaDraw_CableTray::STRAIGHT_T == m_eType)
	{
		sType = _T("STRAIGHT");
	}
	else if(CaDraw_CableTray::ELBOW_T == m_eType)
	{
		sType = _T("ELBOW");
	}
	else if(CaDraw_CableTray::TEE_T == m_eType)
	{
		sType = _T("TEE");
	}
	else if(CaDraw_CableTray::REDUCE_T == m_eType)
	{
		sType = _T("REDUCE");
	}

	return sType;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   GetSubType
    @return     SUBTYPE_E
    @brief
******************************************************************************/
CaDraw_CableTray::SUBTYPE_E CaDraw_CableTray::GetSubType() const
{
	return m_eSubType;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   SetType
    @return     int
    @param      const   CaDraw_CableTray::ELEMENT_E&
    @param      type
    @brief
******************************************************************************/
int CaDraw_CableTray::SetType(const CaDraw_CableTray::ELEMENT_E& eType)
{
	m_eType = eType;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   SetSubType
    @return     int
    @param      const   CaDraw_CableTray::SUBTYPE_E&
    @param      SubType
    @brief
******************************************************************************/
int CaDraw_CableTray::SetSubType(const CaDraw_CableTray::SUBTYPE_E& eSubType)
{
	m_eSubType = eSubType;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   GetSubTypeString
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_CableTray::GetSubTypeString() const
{
	if(CaDraw_CableTray::NONE_ST == m_eSubType)
		return _T("NONE");
	else if(CaDraw_CableTray::STRAIGHT_ST == m_eSubType)
		return _T("STRAIGHT");
	else if(CaDraw_CableTray::LEFT_ST == m_eSubType)
		return _T("LEFT");
	else if(CaDraw_CableTray::RIGHT_ST == m_eSubType)
		return _T("RIGHT");
	else if(CaDraw_CableTray::HORIZONTAL_ST == m_eSubType)
		return _T("HORIZONTAL");
	else if(CaDraw_CableTray::VERTICAL_ST == m_eSubType)
		return _T("VERTICAL");

	return STRING_T();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetCategory
    @return     CATEGORY_E
    @brief
******************************************************************************/
CaDraw_CableTray::CATEGORY_E CaDraw_CableTray::GetCategory() const
{
	return m_eCategory;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetCategory
    @return     int
    @param      const       CATEGORY_E&
    @param      category
    @brief
******************************************************************************/
int CaDraw_CableTray::SetCategory(const CaDraw_CableTray::CATEGORY_E& category)
{
	m_eCategory = category;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetCategoryString
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_CableTray::GetCategoryString() const
{
	STRING_T res;

	switch(m_eCategory)
	{
		case CaDraw_CableTray::ELECTRIC:
			res = _T("ELEC");
			break;
		case CaDraw_CableTray::INSTRUMENT:
			res = _T("INST");
			break;
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetAngle
    @return     double
    @brief
******************************************************************************/
double CaDraw_CableTray::GetAngle() const
{
	return m_dAngle;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetAngle
    @return     int
    @param      const   double&
    @param      angle
    @brief
******************************************************************************/
int CaDraw_CableTray::SetAngle(const double& angle)
{
	m_dAngle = angle;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetLengthDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_CableTray::GetLengthDir() const
{
	return m_lengthDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetWidthDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_CableTray::GetWidthDir() const
{
	return m_widthDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetHeightDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_CableTray::GetHeightDir() const
{
	return m_heightDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetLengthDir
    @return     int
    @param      const   CIsVect3d&
    @param      dir
    @brief
******************************************************************************/
int CaDraw_CableTray::SetLengthDir(const CIsVect3d& dir)
{
	m_lengthDir = dir;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetWidthDir
    @return     int
    @param      const   CIsVect3d&
    @param      dir
    @brief
******************************************************************************/
int CaDraw_CableTray::SetWidthDir(const CIsVect3d& dir)
{
	m_widthDir = dir;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetHeightDir
    @return     int
    @param      const   CIsVect3d&
    @param      dir
    @brief
******************************************************************************/
int CaDraw_CableTray::SetHeightDir(const CIsVect3d& dir)
{
	m_heightDir = dir;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   GetEdgeList
    @return     int
    @param      vector<CIsPoint3d>& oEdgeList
    @brief
******************************************************************************/
int CaDraw_CableTray::GetEdgeList(vector<CIsPoint3d>& oEdgeList) const
{
	oEdgeList.clear();
	oEdgeList.insert(oEdgeList.end() , m_pEdgeList->begin() , m_pEdgeList->end());
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_CableTray
    @function   SetEdgeList
    @return     int
    @param      const       vector<CIsPoint3d>&
    @param      oEdgeList
    @brief
******************************************************************************/
int CaDraw_CableTray::SetEdgeList(const vector<CIsPoint3d>& oEdgeList)
{
	m_pEdgeList->clear();
	m_pEdgeList->insert(m_pEdgeList->end() , oEdgeList.begin() , oEdgeList.end());
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTray
    @function   Dump
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CaDraw_CableTray::Dump(OFSTREAM_T& ofile)
{
	static long index = 1L;

	ofile << (index++) << _T(",CABL,");
	switch(m_eType)
	{
		case CaDraw_CableTray::STRAIGHT_T:
			ofile << _T("STRAIGHT,");
			break;
		case CaDraw_CableTray::ELBOW_T:
			ofile << _T("ELBOW,");
			ofile << this->GetSubTypeString() << _T(",");
			ofile << IsString::TrimedTrailingZero(m_dAngle) << _T(",");
			break;
		case CaDraw_CableTray::TEE_T:
			ofile << _T("TEE,");
			ofile << this->GetSubTypeString() << _T(",");
			break;
		case CaDraw_CableTray::REDUCE_T:
			ofile << _T("REDUCE,");
			ofile << this->GetSubTypeString() << _T(",");
			break;
	}

	ofile << this->GetCategoryString() << _T(",");
	ofile << IsString::TrimedTrailingZero(m_pt[0].z()) << _T(",");
	ofile << m_pView->name() << _T(",");
	ofile << this->name() << _T(",");
	if((CaDraw_CableTray::STRAIGHT_T == m_eType) || (CaDraw_CableTray::REDUCE_T == m_eType))
	{
		CIsPoint3d pt = m_pView->ModelCoord2ViewCoord(m_pt[0]);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(pt.z()) << _T(",");
		pt = m_pView->ModelCoord2ViewCoord(m_pt[1]);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(pt.z());

		if(CaDraw_CableTray::STRAIGHT_T == m_eType)
		{
			ofile << _T(",") << IsString::TrimedTrailingZero(m_width[0]) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_height) << _T(",");

			ofile << IsString::TrimedTrailingZero(m_lengthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dz()) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_widthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dz()) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_heightDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dz());
		}
		else if(CaDraw_CableTray::REDUCE_T == m_eType)
		{
			ofile << _T(",") << IsString::TrimedTrailingZero(m_width[0]) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_width[1]) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_height) << _T(",");

			ofile << IsString::TrimedTrailingZero(m_lengthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dz()) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_widthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dz()) << _T(",");
			ofile << IsString::TrimedTrailingZero(m_heightDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dz());

			for(vector<CIsPoint3d>::iterator itr = m_pEdgeList->begin();itr != m_pEdgeList->end();++itr)
			{
				CIsPoint3d pt = m_pView->ModelCoord2ViewCoord(*itr);
				ofile << _T(",") << IsString::TrimedTrailingZero(pt.x());
				ofile << _T(",") << IsString::TrimedTrailingZero(pt.y());
				ofile << _T(",") << IsString::TrimedTrailingZero(pt.z());
			}
		}
	}
	else if((CaDraw_CableTray::ELBOW_T == m_eType) || (CaDraw_CableTray::TEE_T == m_eType))
	{
		CIsPoint3d pt = m_pView->ModelCoord2ViewCoord(m_pt[0]);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(pt.z()) << _T(",");
		pt = m_pView->ModelCoord2ViewCoord(m_pt[1]);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(pt.z()) << _T(",");
		pt = m_pView->ModelCoord2ViewCoord(m_pt[2]);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(pt.z()) << _T(",");

		ofile << IsString::TrimedTrailingZero(m_width[0]) << _T(",");
		if(CaDraw_CableTray::TEE_T == m_eType)
		{
			ofile << IsString::TrimedTrailingZero(m_width[1]) << _T(",");
		}
		ofile << IsString::TrimedTrailingZero(m_height) << _T(",");
		
		ofile << IsString::TrimedTrailingZero(m_lengthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_lengthDir.dz()) << _T(",");
		ofile << IsString::TrimedTrailingZero(m_widthDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_widthDir.dz()) << _T(",");
		ofile << IsString::TrimedTrailingZero(m_heightDir.dx()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dy()) << _T(",") << IsString::TrimedTrailingZero(m_heightDir.dz());
	}

	ofile << std::endl;

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_CableTray::IsEnable(AnnoFilters* pAnnoFilters) const
{
	return true;
}
