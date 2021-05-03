#include "PolyCurve.h"

/**	\brief
*/
CPolyCurve::CPolyCurve()
{
	m_nPoints = 0;
	m_ppt     = NULL;
}

/**	\brief
*/
CPolyCurve::CPolyCurve(long nPoints,POINT_T* ppt)
{
	assert((nPoints > 0) && "nPoints is less than 0.");
	assert(ppt && "ppt is NULL");
	bool bRet=false;

	if((nPoints > 0) && ppt)
	{
		m_nPoints = nPoints;
		if(NULL != (m_ppt = (POINT_T*)calloc(1,sizeof(POINT_T)*m_nPoints)))
		{
			memcpy(m_ppt,ppt,sizeof(POINT_T)*m_nPoints);
			bRet = true;
		}
	}
}

/**	\brief
*/
CPolyCurve::~CPolyCurve()
{
	OnDestroy();
}

/**	\brief	The CPolyCurve::GetNumOfPoints function


	\return	long	
*/
long CPolyCurve::GetNumOfPoints()
{
	return m_nPoints;
}

/**	\brief	The CPolyCurve::HasEdge function

	\param	ptStart	a parameter of type const POINT_T&
	\param	ptEnd	a parameter of type const POINT_T&

	\return	bool	
*/
bool CPolyCurve::HasEdge(const POINT_T& ptStart,const POINT_T& ptEnd)
{
	assert((m_nPoints > 0) && "m_nPoints is less than 0.");
	assert(m_ppt && "m_ppt is NULL");
	bool bRet=false;

	if((m_nPoints > 0) && m_ppt)
	{
		for(int i=0;i < m_nPoints - 1;i++)
		{
			if(0 == memcmp(&(m_ppt[i]),&ptStart,sizeof(POINT_T)))
			{
				if(0 == memcmp(&(m_ppt[i+1]),&ptEnd,sizeof(POINT_T)))
				{
					bRet = true;
					break;
				}
			}
		}
	}
	
	return bRet;
}

/**	\brief	The CPolyCurve::HasSegment function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CPolyCurve::HasSegment(const POINT_T& pt1,const POINT_T& pt2)
{
	assert((m_nPoints > 0) && "m_nPoints is less than 0.");
	assert(m_ppt && "m_ppt is NULL");
	bool bRet=false;

	if((m_nPoints > 0) && m_ppt)
	{
		for(int i=0;i < m_nPoints - 1;i++)
		{
			if(0 == memcmp(&(m_ppt[i]),&pt1,sizeof(POINT_T)))
			{
				if(0 == memcmp(&(m_ppt[i+1]),&pt2,sizeof(POINT_T)))
				{
					bRet = true;
					break;
				}
			}
			else if(0 == memcmp(&(m_ppt[i]),&pt2,sizeof(POINT_T)))
			{
				if(0 == memcmp(&(m_ppt[i+1]),&pt1,sizeof(POINT_T)))
				{
					bRet = true;
					break;
				}
			}
		}
	}
	return bRet;
}

/**	\brief	The CPolyCurve::OnDestroy function


	\return	void	
*/
void CPolyCurve::OnDestroy()
{
	if((m_nPoints > 0) && m_ppt)
	{
		::free((void*)m_ppt);
	}
}

/**	\brief	The = function

	\param	vecNorm	a parameter of type const VECTOR_T&

	\return	void CPolyCurve::operator	
*/
void CPolyCurve::operator =(const VECTOR_T& vecNorm)
{
	m_vecNorm = vecNorm;
}

/**	\brief	The () function

	\param	nPoints	a parameter of type long
	\param	ppt	a parameter of type POINT_T*

	\return	bool CPolyCurve::operator	
*/
bool CPolyCurve::operator ()(long nPoints,POINT_T* ppt)
{
	assert((nPoints > 0) && "nPoints is less than 0.");
	assert(ppt && "ppt is NULL");
	bool bRet=false;

	if((nPoints > 0) && ppt)
	{
		m_nPoints = nPoints;
		if(m_ppt = (POINT_T*)::malloc(sizeof(POINT_T)*(m_nPoints + 1)))
		{
			memcpy(m_ppt,ppt,sizeof(POINT_T)*m_nPoints);

			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The [] function

	\param	nIndex	a parameter of type long

	\return	POINT_T& CPolyCurve::operator	
*/
POINT_T& CPolyCurve::operator [](long nIndex)
{
	assert((m_nPoints > 0) && "nPoints is less than 0.");
	assert(m_ppt && "ppt is NULL");

	if((m_nPoints > 0) && m_ppt)
	{
		return m_ppt[nIndex];
	}else	return m_ppt[m_nPoints];
}