// PreVIEWFormat.cpp: implementation of the CHSRViewFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <Tokenizer.h>
#include "HSRVIEWFormat.h"

#include "SmartHSR.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CHSRViewFormat::CHSRViewFormat(const double& nUOR)
{
	m_volume.Set
		(
		0.0 , 0.0 , 0.0,
		0.0 , 0.0 , 0.0
		);

	m_rotate = R0_T;
	m_nScale = 1.;
	m_nUOR   = nUOR;
	m_direction = TOP_VIEW_T;

	m_oRMatrix[0][0] = 1.;m_oRMatrix[0][1] = 0.;m_oRMatrix[0][2] = 0.;
	m_oRMatrix[1][0] = 0.;m_oRMatrix[1][1] = 1.;m_oRMatrix[1][2] = 0.;
	m_oRMatrix[2][0] = 0.;m_oRMatrix[2][1] = 0.;m_oRMatrix[2][2] = 1.;

	m_pViewFace = NULL;
}

CHSRViewFormat::~CHSRViewFormat()
{

}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   CreateInstance
    @return     CHSRViewFormat*
    @brief
******************************************************************************/
CHSRViewFormat* CHSRViewFormat::CreateInstance()
{
	return (new CHSRViewFormat);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   Parse
    @return     int
    @param      CMarkup&    xml
    @brief
******************************************************************************/
int CHSRViewFormat::Parse(CMarkup& xml)
{
	vector<STRING_T> oResult;
	
	int iCount = 0;
	while ( 1 )
	{
		STRING_T sName = xml.GetAttribName(iCount++);
		if ( sName.empty() ) break;

		STRING_T sValue = xml.GetAttrib( sName );
		if(0 == STRICMP_T(sName.c_str() , _T("Name")))
		{
			m_sName = sValue;
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
			m_direction = this->GetViewDirFrom(sValue);
			switch(m_direction)
			{
				case TOP_VIEW_T:
					m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=1.;m_oRMatrix[1][2]=0.;
					m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=1.;
					break;
				case BOTTOM_VIEW_T:
					m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=-1.;m_oRMatrix[1][2]=0.;
					m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=1.;
					break;
				case LEFT_VIEW_T:
					m_oRMatrix[0][0]=0.;m_oRMatrix[0][1]=-1.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
					m_oRMatrix[2][0]=-1.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=0.;
					break;
				case RIGHT_VIEW_T:
					m_oRMatrix[0][0]=0.;m_oRMatrix[0][1]=1.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
					m_oRMatrix[2][0]=1.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=0.;
					break;
				case FRONT_VIEW_T:
					m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
					m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=-1.;m_oRMatrix[2][2]=0.;
					break;
				case BACK_VIEW_T:
					m_oRMatrix[0][0]=-1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
					m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
					m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=1.;m_oRMatrix[2][2]=0.;
					break;
			};
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Scale")))
		{
			m_nScale = 1.0 / ATOF_T(sValue.c_str());
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("Rotate")))
		{
			m_rotate = this->GetViewAngleFrom(sValue);
		}
		else if(0 == STRICMP_T(sName.c_str() , _T("ViewOrigin")))	/// it's lower left value
		{
			CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
			if(2 == oResult.size())
			{
				m_ptViewLowLeft.x = ATOF_T(oResult[0].c_str());
				m_ptViewLowLeft.y = ATOF_T(oResult[1].c_str());
				m_ptViewLowLeft.z = 0.0;
			}
		}
	}

	this->CalculateViewInfo();
	m_ViewType = CHSRViewFormat::RECTANGLE_VIEW;
	
	m_oModelList.clear();
	xml.IntoElem();
	while(true == xml.FindElem(MCD_T("REF")))
	{
		CHSRElement::MODEL_T modelT;
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
					modelT.modelType  = this->GetModelType(sValue);
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

		m_oModelList.push_back(modelT);
	}
	
	xml.OutOfElem();

	return ERROR_SUCCESS;

}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   DoesInclude
    @return     bool
    @param      const   CHSRElement::MODEL_T&
    @param      modelT
    @brief
******************************************************************************/
bool CHSRViewFormat::DoesInclude(const CHSRElement::MODEL_T& modelT)
{
	for(list<CHSRElement::MODEL_T>::iterator itr = m_oModelList.begin();itr != m_oModelList.end();++itr)
	{
		if((modelT.modelType == itr->modelType) && (modelT.rModelName == itr->rModelName)) return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   GetViewDirFrom
    @return     CHSRViewFormat::DIRECTIONT_T
    @param      const       STRING_T&
    @param      sViewDir
    @brief
******************************************************************************/
CHSRViewFormat::DIRECTIONT_T CHSRViewFormat::GetViewDirFrom(const STRING_T& sViewDir)
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
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   GetViewAngleFrom
    @return     CHSRViewFormat::ROTATE_T
    @param      const       STRING_T&
    @param      sViewAngle
    @brief
******************************************************************************/
CHSRViewFormat::ROTATE_T CHSRViewFormat::GetViewAngleFrom(const STRING_T& sViewAngle)
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

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRViewFormat
    @function   GetModelType
    @return     CHSRViewFormat::MODEL_TYPE_T
    @param      const       STRING_T&
    @param      rModelType
    @brief
******************************************************************************/
CHSRElement::MODEL_TYPE_T CHSRViewFormat::GetModelType(const STRING_T& rModelType)
{
	if(0 == STRICMP_T(_T("Pipe") , rModelType.c_str()))
	{
		return CHSRElement::PIPE_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Equipment") , rModelType.c_str()))
	{
		return CHSRElement::EQUIPMENT_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Structure") , rModelType.c_str()))
	{
		return CHSRElement::STRUCTURE_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("CableT") , rModelType.c_str()))
	{
		return CHSRElement::CABLE_T_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Misc") , rModelType.c_str()))
	{
		return CHSRElement::MISC_MODEL_T;
	}

	throw exception("unexpected model type");
}

/**	
	@brief	The CHSRViewFormat::ParseLine function

	@author	humkyung

	@date	????.??.??

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	void	
*/
void CHSRViewFormat::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");

	if(pHSRScanner)
	{
		pHSRScanner->m_aryFactor[1];
		
		m_ptViewLowLeft.x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_ptViewLowLeft.y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_ptViewLowLeft.z = 0.;
		
		m_rotate = this->GetViewAngleFrom((*pHSRScanner->m_aryFactor)[4].c_str());

		m_volume.Add( CIsPoint3d( 
			ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str()) , 
			ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str()) , 
			ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str())));

		m_volume.Add( CIsPoint3d(
			ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str()) ,
			ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str()) ,
			ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str())));
		
		int nLen=STRLEN_T((*pHSRScanner->m_aryFactor)[11].c_str());
		for(int i=0;i < nLen;i++) (*pHSRScanner->m_aryFactor)[11][i] = toupper((*pHSRScanner->m_aryFactor)[11][i]);
		if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("TOP")))
		{
			m_direction = TOP_VIEW_T;
			m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=1.;m_oRMatrix[1][2]=0.;
			m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=1.;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("FRONT")))
		{
			m_direction = FRONT_VIEW_T;
			m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
			m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=-1.;m_oRMatrix[2][2]=0.;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("BACK")))
		{
			m_direction = BACK_VIEW_T;
			m_oRMatrix[0][0]=-1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
			m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=1.;m_oRMatrix[2][2]=0.;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("LEFT")))
		{
			m_direction = LEFT_VIEW_T;
			m_oRMatrix[0][0]=0.;m_oRMatrix[0][1]=-1.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
			m_oRMatrix[2][0]=-1.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=0.;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("RIGHT")))
		{
			m_direction = RIGHT_VIEW_T;
			m_oRMatrix[0][0]=0.;m_oRMatrix[0][1]=1.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=0.;m_oRMatrix[1][2]=1.;
			m_oRMatrix[2][0]=1.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=0.;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[11].c_str(),_T("BOTTOM")))
		{
			m_direction = BOTTOM_VIEW_T;
			m_oRMatrix[0][0]=1.;m_oRMatrix[0][1]=0.;m_oRMatrix[0][2]=0.;
			m_oRMatrix[1][0]=0.;m_oRMatrix[1][1]=-1.;m_oRMatrix[1][2]=0.;
			m_oRMatrix[2][0]=0.;m_oRMatrix[2][1]=0.;m_oRMatrix[2][2]=1.;
		}else	OutputDebugString(_T("Not support this view type!!!"));

		const TCHAR* pDivide = STRCHR_T((*pHSRScanner->m_aryFactor)[12].c_str(),'/');
		if(pDivide)
		{
			TCHAR txt[16]={0};

			int i = 0;
			for(i = 0;(*pHSRScanner->m_aryFactor)[12][i] && ('/' != (*pHSRScanner->m_aryFactor)[12][i]);i++)
			{
				txt[i] = (*pHSRScanner->m_aryFactor)[12][i];
			}
			txt[i] = '\0';

			m_nScale = ATOF_T(txt)/ATOF_T(pDivide + 1);
		}else	m_nScale = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());

		CalculateViewInfo();

		m_ViewType = CHSRViewFormat::RECTANGLE_VIEW;
		if(pHSRScanner->m_nFactor > 20)
		{
			if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[20].c_str() , _T("Q1") , STRLEN_T(_T("Q1"))))
			{
				m_ViewType = CHSRViewFormat::QUADRANT1_VIEW;
			}
			else if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[20].c_str() , _T("Q2") , STRLEN_T(_T("Q2"))))
			{
				m_ViewType = CHSRViewFormat::QUADRANT2_VIEW;
			}
			else if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[20].c_str() , _T("Q3") , STRLEN_T(_T("Q3"))))
			{
				m_ViewType = CHSRViewFormat::QUADRANT3_VIEW;
			}
			else if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[20].c_str() , _T("Q4") , STRLEN_T(_T("Q4"))))
			{
				m_ViewType = CHSRViewFormat::QUADRANT4_VIEW;
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CHSRViewFormat::IsIn(CHSRViewFormat* pView)
{
	assert(pView && "pView is NULL");

	if(pView)
	{
		double x[2]={0.,},y[2]={0.,};
		double X[2]={0.,},Y[2]={0.,};

		x[LOW] = m_ptCenter.x - m_nWidth*0.5;
		x[HIGH]= m_ptCenter.x + m_nWidth*0.5;
		y[LOW] = m_ptCenter.y - m_nHeight*0.5;
		y[HIGH]= m_ptCenter.y + m_nHeight*0.5;
		
		X[LOW] = pView->m_ptCenter.x - pView->m_nWidth*0.5;
		X[HIGH]= pView->m_ptCenter.x + pView->m_nWidth*0.5;
		Y[LOW] = pView->m_ptCenter.y - pView->m_nHeight*0.5;
		Y[HIGH]= pView->m_ptCenter.y + pView->m_nHeight*0.5;

		if(	(x[LOW] >= X[LOW]) && (x[HIGH] <= X[HIGH]) && 
			(y[LOW] >= Y[LOW]) && (y[HIGH] <= Y[HIGH])) return true;
	}

	return false;
}

/**	
	@brief	The CHSRViewFormat::MODEL2VIEW function

	@param	pt	a parameter of type const POINT_T

	@return	POINT_T	
*/
POINT_T CHSRViewFormat::MODEL2VIEW(const POINT_T pt)
{
	POINT_T _pt = {0,};
	_pt.x = pt.x - m_ptModelCenter.x;
	_pt.y = pt.y - m_ptModelCenter.y;
	_pt.z = pt.z - m_ptModelCenter.z;
	_pt.reserved = pt.reserved;	/// set reserved value - 2012.01.11 added by humkyung
	
	const double cs=cos(DEG2RAD(m_rotate));
	const double sn=sin(DEG2RAD(m_rotate));
	double tmp[2][3]={0,};
	tmp[0][0] = m_oRMatrix[0][0]*_pt.x + m_oRMatrix[0][1]*_pt.y + m_oRMatrix[0][2]*_pt.z;
	tmp[0][1] = m_oRMatrix[1][0]*_pt.x + m_oRMatrix[1][1]*_pt.y + m_oRMatrix[1][2]*_pt.z;
	tmp[0][2] = m_oRMatrix[2][0]*_pt.x + m_oRMatrix[2][1]*_pt.y + m_oRMatrix[2][2]*_pt.z;
	tmp[1][0] = tmp[0][0]*cs - tmp[0][1]*sn;
	tmp[1][1] = tmp[0][0]*sn + tmp[0][1]*cs;
	tmp[1][2] = tmp[0][2];

	_pt.x = m_ptCenter.x + tmp[1][0]*m_nScale;
	_pt.y = m_ptCenter.y + tmp[1][1]*m_nScale;
	_pt.z = m_ptCenter.z + tmp[1][2]*m_nScale;

	return _pt;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
VECTOR_T CHSRViewFormat::MODEL2VIEW(const VECTOR_T vec)
{
	VECTOR_T _vec={0.};
	_vec = vec;
	
	const double cs=cos(DEG2RAD(m_rotate));
	const double sn=sin(DEG2RAD(m_rotate));
	double tmp[2][3]={0,};
	tmp[0][0] = m_oRMatrix[0][0]*_vec.dx + m_oRMatrix[0][1]*_vec.dy + m_oRMatrix[0][2]*_vec.dz;
	tmp[0][1] = m_oRMatrix[1][0]*_vec.dx + m_oRMatrix[1][1]*_vec.dy + m_oRMatrix[1][2]*_vec.dz;
	tmp[0][2] = m_oRMatrix[2][0]*_vec.dx + m_oRMatrix[2][1]*_vec.dy + m_oRMatrix[2][2]*_vec.dz;
	tmp[1][0] = tmp[0][0]*cs - tmp[0][1]*sn;
	tmp[1][1] = tmp[0][0]*sn + tmp[0][1]*cs;
	tmp[1][2] = tmp[0][2];
	_vec.dx = tmp[1][0]*m_nScale;
	_vec.dy = tmp[1][1]*m_nScale;
	_vec.dz = tmp[1][2]*m_nScale;

	return _vec;
}

/**	\brief	The CHSRViewFormat::MODEL2VIEW function

	\param	nValue	a parameter of type const double

	\return	double	
*/
double CHSRViewFormat::MODEL2VIEW(const double nValue)
{
	return nValue*m_nScale;
}

/**	
	@brief	create element's outline faces.
	@author	BHK
	@date	?
	@param	nID	a parameter of type long&
	@return	PFACE	
*/
CHSRFace* CHSRViewFormat::CreateFace(long& nID)
{
	CHSRFace* pRet=NULL;
	CHSRFace* pFace=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pHSRApp)
	{
		pHSRApp->m_clsView.SetViewInfo(m_ptCenter , m_nWidth , m_nHeight , m_nScale , m_ViewType);
		pHSRApp->m_clsView.SetUOR(m_nUOR);
		m_pViewFace = pHSRApp->m_clsView.GetViewFace();
		
		/*
		try
		{
			for(list<CHSRPipe*>::iterator itrPIPE=m_lstPIPEFormat.begin();itrPIPE != m_lstPIPEFormat.end();++itrPIPE)
			{
				if(pFace=(*itrPIPE)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString("Error in PIPE");
		}
		*/

		long l=0L;
		/*
		try
		{
			for(list<CCylnElement*>::iterator itrCYLN=m_lstCYLNFormat.begin();itrCYLN != m_lstCYLNFormat.end();++itrCYLN)
			{
				if(pFace=(*itrCYLN)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
				l++;
			}
		}
		catch(...)
		{
			OutputDebugString("Error in CYLN");
		}
		try
		{
			for(list<CConeElement*>::iterator itrCONE=m_lstConeElement.begin();itrCONE != m_lstConeElement.end();++itrCONE)
			{
				if(pFace=(*itrCONE)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString("Error in CONE");
		}
		*/

		for(list<CElbowElement*>::iterator itrELBOW=m_lstELBOWFormat.begin();itrELBOW != m_lstELBOWFormat.end();++itrELBOW)
		{
			try
			{
				if(pFace=(*itrELBOW)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in ELBOW"));
			}
		}
		
		try
		{
			for(list<CTeeElement*>::iterator itrTEE=m_lstTEEFormat.begin();itrTEE != m_lstTEEFormat.end();++itrTEE)
			{
				if(pFace=(*itrTEE)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in TEE"));
		}
		
		try
		{
			for(list<CDomeElement*>::iterator itrDOME=m_lstDomeElement.begin();itrDOME != m_lstDomeElement.end();++itrDOME)
			{
				if(pFace=(*itrDOME)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in DOME"));
		}

		try
		{
			for(list<CTorsElement*>::iterator itrTORS=m_lstTORSFormat.begin();itrTORS != m_lstTORSFormat.end();++itrTORS)
			{
				if(pFace=(*itrTORS)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in TORS"));
		}

		try
		{
			for(list<CCTorElement*>::iterator itrCTOR=m_lstCTORFormat.begin();itrCTOR != m_lstCTORFormat.end();++itrCTOR)
			{
				if(pFace=(*itrCTOR)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in CTOR"));
		}

		try
		{
			for(list<CRTorElement*>::iterator itrRTOR=m_lstRTorElement.begin();itrRTOR != m_lstRTorElement.end();++itrRTOR)
			{
				if(pFace=(*itrRTOR)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in RTOR"));
		}

		/*
		try
		{
			for(list<CSphereElement*>::iterator itrSPHERE=m_lstSPHEREFormat.begin();itrSPHERE != m_lstSPHEREFormat.end();++itrSPHERE)
			{
				if(pFace=(*itrSPHERE)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString("Error in SPHERE");
		}
		*/

		l = 0;
		for(list<CHSRElement*>::iterator itrElement=m_lstElement.begin();itrElement != m_lstElement.end();++itrElement,++l)
		{
			CHSRElement* pElm = (*itrElement);
			try
			{
				if(pFace = pElm->CreateFace(this,nID))
				{
					
					CHSRFace* ptr_next=NULL;
					for(CHSRFace* ptr=pFace;ptr;ptr = ptr_next)
					{
						ptr_next = ptr->next;ptr->next = NULL;
						ptr->id = nID++;
						pHSRApp->AppendFace(NULL,ptr);
					}
					//pRet = ListPush(pRet,pFace);
				}
			}
			catch(...)
			{
				OutputDebugString(STRING_T(_T("Error in Element : ") + pElm->GetTypeString()).c_str());
			}
		}

		try
		{
			for(list<CHSRNormalFace*>::iterator itrFace=m_lstNormalFace.begin();itrFace != m_lstNormalFace.end();++itrFace)
			{
				if(pFace=(*itrFace)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in Face"));
		}

		try
		{
			for(list<CPreHNormalFace*>::iterator itrHFace=m_lstHNormalFace.begin();itrHFace != m_lstHNormalFace.end();++itrHFace)
			{
				if(pFace=(*itrHFace)->CreateFace(this,nID))
				{
					pRet = ListPush(pRet,pFace);
				}
			}
		}
		catch(...)
		{
			OutputDebugString(_T("Error in HFace"));
		}
	}

	return pFace;
}

/**	
	@brief	The CHSRViewFormat::CalculateViewInfo function

	@author	humkyung

	@date	????.??.??

	@remarks\n
	;[2003-03-28] - append FRONT and BACK VIEW code.

	@return	void	
*/
void CHSRViewFormat::CalculateViewInfo()
{
	double nModelXLen[2]={0,},nModelYLen[2]={0,};
	double nModelZLen[2]={0,};
	
	nModelXLen[0]=fabs(m_volume.maxx() - m_volume.minx());
	nModelYLen[0]=fabs(m_volume.maxy() - m_volume.miny());
	nModelZLen[0]=fabs(m_volume.maxz() - m_volume.minz());
	
	if((TOP_VIEW_T == m_direction) || (BOTTOM_VIEW_T == m_direction))
	{
		nModelXLen[1] = nModelXLen[0];
		nModelYLen[1] = nModelYLen[0];
	}
	else if((FRONT_VIEW_T == m_direction) || (BACK_VIEW_T == m_direction))
	{
		nModelXLen[1] = nModelXLen[0];
		nModelYLen[1] = nModelZLen[0];
	}
	else if((LEFT_VIEW_T == m_direction) || (RIGHT_VIEW_T == m_direction))
	{
		nModelXLen[1] = nModelYLen[0];
		nModelYLen[1] = nModelZLen[0];
	}

	if((0. == m_rotate) || (180. == m_rotate))
		m_nWidth = nModelXLen[1]*m_nScale;
	else	m_nWidth = nModelYLen[1]*m_nScale;
	if((0. == m_rotate) || (180. == m_rotate))
		m_nHeight = nModelYLen[1]*m_nScale;
	else	m_nHeight = nModelXLen[1]*m_nScale;
	
	CIsPoint3d center = m_volume.center();
	m_ptModelCenter.x = center.x();
	m_ptModelCenter.y = center.y();
	m_ptModelCenter.z = center.z();

	m_ptCenter.x = m_ptViewLowLeft.x + m_nWidth*0.5;
	m_ptCenter.y = m_ptViewLowLeft.y + m_nHeight*0.5;
	m_ptCenter.z = 0.;
}

/**	\brief	The += function

	\param	pPipeElm	a parameter of type CHSRPipe*

	\return	bool CHSRViewFormat::operator	
*/
/*
bool CHSRViewFormat::operator +=(CHSRPipe* pPipeElm)
{
	assert(pPipeElm && "pPipeElm is NULL");

	if(pPipeElm)
	{
		m_lstPIPEFormat.push_back(pPipeElm);
		return true;
	}

	return false;
}
*/

/**	\brief	The += function

	\param	pPreTORSFormat	a parameter of type CTorsElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CTorsElement* pPreTORSFormat){

	assert(pPreTORSFormat && "pPreTORSFormat is NULL");

	if(pPreTORSFormat)
	{
		m_lstTORSFormat.push_back(pPreTORSFormat);
		return true;
	}

	return false;
}

/**	\brief	The += function

	\param	pCTorElm	a parameter of type CCTorElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CCTorElement* pCTorElm)
{
	assert(pCTorElm && "pCTorElm is NULL");

	if(pCTorElm)
	{
		m_lstCTORFormat.push_back(pCTorElm);
		return true;
	}

	return false;
}

/**	\brief	The += function

	\param	pRTorElm	a parameter of type CRTorElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CRTorElement* pRTorElm)
{
	assert(pRTorElm && "pRTorElm is NULL");

	if(pRTorElm)
	{
		m_lstRTorElement.push_back(pRTorElm);
		return true;
	}

	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
/*
bool CHSRViewFormat::operator +=(CCylnElement* pCylnElm)
{
	assert(pCylnElm && "pCylnElm is NULL");

	if(pCylnElm)
	{
		m_lstCYLNFormat.push_back(pCylnElm);
		return true;
	}

	return false;
}
*/

/**	\brief	The += function

	\param	pConeElm	a parameter of type CConeElement*

	\return	bool CHSRViewFormat::operator	
*/
/*
bool CHSRViewFormat::operator +=(CConeElement* pConeElm)
{
	assert(pConeElm && "pConeElm is NULL");

	if(pConeElm)
	{
		m_lstConeElement.push_back(pConeElm);
		return true;
	}

	return false;
}
*/

/**	\brief	The += function

	\param	pElbowElm	a parameter of type CElbowElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CElbowElement* pElbowElm)
{
	assert(pElbowElm && "pElbowElm is NULL");

	if(pElbowElm)
	{
		m_lstELBOWFormat.push_back(pElbowElm);
		return true;
	}

	return false;
}

/**	\brief	The += function

	\param	pTeeElm	a parameter of type CTeeElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CTeeElement* pTeeElm)
{
	assert(pTeeElm && "pTeeElm is NULL");

	if(pTeeElm)
	{
		m_lstTEEFormat.push_back(pTeeElm);
		return true;
	}

	return false;
}

/**	\brief	The += function

	\param	pDomeElm	a parameter of type CDomeElement*

	\return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CDomeElement* pDomeElm)
{
	assert(pDomeElm && "pDomeElm is NULL");

	if(pDomeElm)
	{
		m_lstDomeElement.push_back(pDomeElm);
		return true;
	}

	return false;
}

/**	
	@brief	add hsr element to list.

	@author	BHK

	@date	?

	@param	pElement	a parameter of type CHSRElement*

	@return	bool CHSRViewFormat::operator	
*/
bool CHSRViewFormat::operator +=(CHSRElement* pElement)
{
	assert(pElement && "pElement is NULL");

	if(pElement)
	{
		m_lstElement.push_back(pElement);
		return true;
	}

	return false;
}

/**	\brief	The += function

	\param	pPreSPHEREFormat	a parameter of type CSphereElement*

	\return	bool CHSRViewFormat::operator	
*/
/*
bool CHSRViewFormat::operator +=(CSphereElement* pPreSPHEREFormat)
{
	assert(pPreSPHEREFormat && "pPreSPHEREFormat is NULL");

	if(pPreSPHEREFormat)
	{
		m_lstSPHEREFormat.push_back(pPreSPHEREFormat);
		return true;
	}

	return false;
}
*/

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CHSRViewFormat::operator +=(CHSRNormalFace* pPreNormalFace)
{
	assert(pPreNormalFace && "pPreNormalFace is NULL");

	if(pPreNormalFace)
	{
		m_lstNormalFace.push_back(pPreNormalFace);
		return true;
	}

	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CHSRViewFormat::operator +=(CPreHNormalFace* pPreHNormalFace)
{
	assert(pPreHNormalFace && "pPreHNormalFace is NULL");

	if(pPreHNormalFace)
	{
		m_lstHNormalFace.push_back(pPreHNormalFace);
		return true;
	}

	return false;
}

const CHSRViewFormat::DIRECTIONT_T CHSRViewFormat::GetViewDir() const
{
	return m_direction;
}

VOLUME_T CHSRViewFormat::GetViewVolume()
{
	POINT_T ptModelEdge[8]={0.,};
	const double width =(m_volume.maxx() - m_volume.minx());
	const double height=(m_volume.maxy() - m_volume.miny());
	const double depth =(m_volume.maxz() - m_volume.minz());

	ptModelEdge[0].x = -width*0.5;ptModelEdge[0].y = -height*0.5;ptModelEdge[0].z = -depth*0.5;
	ptModelEdge[1].x = width*0.5;ptModelEdge[1].y = -height*0.5;ptModelEdge[1].z = -depth*0.5;
	ptModelEdge[2].x = width*0.5;ptModelEdge[2].y = height*0.5;ptModelEdge[2].z = -depth*0.5;
	ptModelEdge[3].x = -width*0.5;ptModelEdge[3].y = height*0.5;ptModelEdge[3].z = -depth*0.5;
	ptModelEdge[4].x = -width*0.5;ptModelEdge[4].y = -height*0.5;ptModelEdge[4].z = depth*0.5;
	ptModelEdge[5].x = width*0.5;ptModelEdge[5].y = -height*0.5;ptModelEdge[5].z = depth*0.5;
	ptModelEdge[6].x = width*0.5;ptModelEdge[6].y = height*0.5;ptModelEdge[6].z = depth*0.5;
	ptModelEdge[7].x = -width*0.5;ptModelEdge[7].y = height*0.5;ptModelEdge[7].z = depth*0.5;
	
	const double cs=cos(DEG2RAD(m_rotate));
	const double sn=sin(DEG2RAD(m_rotate)); 
	double tmp[2][3]={0,};
	for(int i=0;i < 8;i++)
	{	
		tmp[0][0] =	m_oRMatrix[0][0]*ptModelEdge[i].x +
				m_oRMatrix[0][1]*ptModelEdge[i].y +
				m_oRMatrix[0][2]*ptModelEdge[i].z;
		tmp[0][1] =	m_oRMatrix[1][0]*ptModelEdge[i].x +
				m_oRMatrix[1][1]*ptModelEdge[i].y +
				m_oRMatrix[1][2]*ptModelEdge[i].z;
		tmp[0][2] =	m_oRMatrix[2][0]*ptModelEdge[i].x +
				m_oRMatrix[2][1]*ptModelEdge[i].y +
				m_oRMatrix[2][2]*ptModelEdge[i].z;
		tmp[1][0] = tmp[0][0]*cs - tmp[0][1]*sn;
		tmp[1][1] = tmp[0][0]*sn + tmp[0][1]*cs;
		tmp[1][2] = tmp[0][2];

		ptModelEdge[i].x = tmp[1][0]*m_nScale;
		ptModelEdge[i].y = tmp[1][1]*m_nScale;
		ptModelEdge[i].z = tmp[1][2]*m_nScale;
	}

	VOLUME_T vol;

	vol.x[0] = vol.x[1] = ptModelEdge[0].x;
	vol.y[0] = vol.y[1] = ptModelEdge[0].y;
	vol.z[0] = vol.z[1] = ptModelEdge[0].z;
	for(int i = 1;i < 8;i++)
	{
		if(ptModelEdge[i].x < vol.x[0]) vol.x[0] = ptModelEdge[i].x;
		if(ptModelEdge[i].y < vol.y[0]) vol.y[0] = ptModelEdge[i].y;
		if(ptModelEdge[i].z < vol.z[0]) vol.z[0] = ptModelEdge[i].z;
		if(ptModelEdge[i].x > vol.x[1]) vol.x[1] = ptModelEdge[i].x;
		if(ptModelEdge[i].y > vol.y[1]) vol.y[1] = ptModelEdge[i].y;
		if(ptModelEdge[i].z > vol.z[1]) vol.z[1] = ptModelEdge[i].z;
	}

	return vol;
}
