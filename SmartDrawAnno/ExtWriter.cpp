// ExtWriter4ENM_PDS.cpp: implementation of the CExtWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <tchar.h>
#include <IsTools.h>
#include <IsString.h>
#include "SmartDrawAnnoModule.h"
#include "ExtWriter.h"
#include "SmartDrawAnnoEnv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtWriter::CExtWriter(CExtDivision* pDivision) : m_pDivision(pDivision)
{

}

CExtWriter::~CExtWriter()
{

}

/**	
	@brief	The CExtWriter::WriteMatchLineData function
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@return	void	
*/
void CExtWriter::WriteMatchLineData(OFSTREAM_T& file,const double& dDrawingScale)
{
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		
		TCHAR szAlign[2]={0,};
		szAlign[0] = 'L';
		
		CIsPoint3d ptMatchPos;
		/// default match pos
		ptMatchPos.x() = (m_pDivision->m_lnBaseLine[0].x() + m_pDivision->m_lnBaseLine[1].x())*0.5;
		ptMatchPos.y() = (m_pDivision->m_lnBaseLine[0].y() + m_pDivision->m_lnBaseLine[1].y())*0.5;
		double nMatchLineLen = env.m_match.m_oTextStyle.GetTextLength(m_pDivision->m_szViewMatchLine);
		if(-1 == m_pDivision->m_vecTagDir.dy())
		{	/// BOTTOM.
			if(0 == STRICMP_T(env.m_match.m_szJustify,_T("LEFT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[0].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[0].y();
			}
			else if(0 == STRICMP_T(env.m_match.m_szJustify,_T("RIGHT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[1].x() - nMatchLineLen;
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[1].y();
			}else	ptMatchPos.x() -= nMatchLineLen*0.5;
		}
		else if(1 == m_pDivision->m_vecTagDir.dx())
		{	/// RIGHT.
			if(0 == STRICMP_T(env.m_match.m_szJustify,_T("LEFT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[0].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[0].y();
			}
			else if(0 == STRICMP_T(env.m_match.m_szJustify,_T("RIGHT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[1].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[1].y() - nMatchLineLen;
			}else	ptMatchPos.y()-= nMatchLineLen*0.5;
		}
		else if(1 == m_pDivision->m_vecTagDir.dy())
		{	/// UPPER.
			if(0 == STRICMP_T(env.m_match.m_szJustify,_T("LEFT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[0].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[0].y();
			}
			else if(0 == STRICMP_T(env.m_match.m_szJustify,_T("RIGHT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[1].x() - nMatchLineLen;
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[1].y();
			}else	ptMatchPos.x()-= nMatchLineLen*0.5;
		}
		else if(-1 == m_pDivision->m_vecTagDir.dx())
		{	/// LEFT.
			if(0 == STRICMP_T(env.m_match.m_szJustify,_T("LEFT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[0].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[0].y();
			}
			else if(0 == STRICMP_T(env.m_match.m_szJustify,_T("RIGHT")))
			{
				ptMatchPos.x() = m_pDivision->m_lnBaseLine[1].x();
				ptMatchPos.y() = m_pDivision->m_lnBaseLine[1].y() - nMatchLineLen;
			}else	ptMatchPos.y()-= nMatchLineLen*0.5;
		}
		
		if(STRCMP_T(m_pDivision->m_szViewMatchLine, _T("Nil")))
		{
			if(m_pDivision->IsHorizontal())
			{
				if(-1 == m_pDivision->m_vecTagDir.dy())
				{
					file << _T("<TEXT>|");
					double d = 0;
					d = ptMatchPos.x() + m_pDivision->m_vecTagDir.dx()*(env.m_match.nMatchDistA + env.m_match.m_oTextStyle.height());
					file << d*dDrawingScale << _T(",");
					d = ptMatchPos.y() + m_pDivision->m_vecTagDir.dy()*(env.m_match.nMatchDistA + env.m_match.m_oTextStyle.height());
					file << d*dDrawingScale << _T(",0,");
					file << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",0,");
					file << m_pDivision->m_szViewMatchLine << _T(",") << STRING_T(env.m_match.m_szJustify) + STRING_T(_T(" Bottom")) << _T(",0|");
					file << env.m_match.m_oLabelProp.level << _T(",") << env.m_match.m_oLabelProp.color << _T(",") << env.m_match.m_oTextStyle << std::endl;
				}
				else if(1 == m_pDivision->m_vecTagDir.dy())
				{
					file << _T("<TEXT>|");
					double d = 0;
					d = ptMatchPos.x() + m_pDivision->m_vecTagDir.dx()*(env.m_match.nMatchDistA);
					file << d*dDrawingScale << _T(",");
					d = ptMatchPos.y() + m_pDivision->m_vecTagDir.dy()*(env.m_match.nMatchDistA);
					file << d*dDrawingScale << _T(",0,");
					file << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",0,");
					file << m_pDivision->m_szViewMatchLine << _T(",") << STRING_T(env.m_match.m_szJustify) + STRING_T(_T(" Bottom")) << _T(",0|");
					file << env.m_match.m_oLabelProp.level << _T(",") << env.m_match.m_oLabelProp.color << _T(",") << env.m_match.m_oTextStyle << std::endl;
				}
			}
			else	/// vertical division
			{
				if(-1 == m_pDivision->m_vecTagDir.dx())
				{
					file << _T("<TEXT>|");
					double d = 0;
					d = ptMatchPos.x() + m_pDivision->m_vecTagDir.dx()*(env.m_match.nMatchDistA);
					file << d*dDrawingScale << _T(",");
					d = ptMatchPos.y() + m_pDivision->m_vecTagDir.dy()*(env.m_match.nMatchDistA);
					file << d*dDrawingScale << _T(",0,");
					file << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",90,");
					file << m_pDivision->m_szViewMatchLine << _T(",") << STRING_T(env.m_match.m_szJustify) + STRING_T(_T(" Bottom")) << _T(",0|");
					file << env.m_match.m_oLabelProp.level << _T(",") << env.m_match.m_oLabelProp.color << _T(",") << env.m_match.m_oTextStyle << std::endl;
				}
				else if(1 == m_pDivision->m_vecTagDir.dx())
				{
					file << _T("<TEXT>|");
					double d = 0;
					d = ptMatchPos.x() + m_pDivision->m_vecTagDir.dx()*(env.m_match.nMatchDistA + env.m_match.m_oTextStyle.height());
					file << d*dDrawingScale << _T(",");
					d = ptMatchPos.y() + m_pDivision->m_vecTagDir.dy()*(env.m_match.nMatchDistA + env.m_match.m_oTextStyle.height());
					file << d*dDrawingScale << _T(",0,");
					file << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_match.m_oTextStyle.height()*dDrawingScale << _T(",90,");
					file << m_pDivision->m_szViewMatchLine << _T(",") << STRING_T(env.m_match.m_szJustify) + STRING_T(_T(" Bottom")) << _T(",0|");
					file << env.m_match.m_oLabelProp.level << _T(",") << env.m_match.m_oLabelProp.color << _T(",") << env.m_match.m_oTextStyle << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	The CExtWriter::Write function
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@remarks\n
	;2014.11.12 - check the env.m_generation.Match Line Info. flag to write match line data
	;2003.07.03 - check the env.m_match.bWriteMatchLine variable.
	@return	void	
*/
void CExtWriter::Write(OFSTREAM_T& file,const double& dDrawingScale)
{
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		
		if(env.m_generation.IsEnable(_T("Match Line Info."),NULL)) WriteMatchLineData(file,dDrawingScale);
		WriteDimension(file,dDrawingScale);
		m_pDivision->CalculateUpperLine();
		
		if(!m_pDivision->m_pvtTagPos->empty())
		{
			for(vector<CaDraw_TagPos*>::const_iterator itr=m_pDivision->m_pvtTagPos->begin();itr != m_pDivision->m_pvtTagPos->end();itr++)
			{
				if(DRAFT_ETIM == (*itr)->pTITEM->enmType)
				{
					WriteETIMTag(file,*itr,dDrawingScale);
				}
				else
				{
					(*itr)->Write(file , m_pDivision,dDrawingScale);
				}
			}
		}
		
		if(env.m_generation.IsEnable(_T("View Coordinate"),NULL))
		{
			WriteViewCoord(file,dDrawingScale);
		}
	}
}

/**	
	@brief	dimension 형식을 파일로 출력한다.
	@author	humkyung
	@date	????.??.??
	@param	file	출력할 파일 스트림
	@return	void	
*/
void CExtWriter::WriteDimension(OFSTREAM_T& file,const double& dDrawingScale)
{
	static bool _blank=false;
	static bool _upper=true;

	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		
		CIsLine3d lnMatchLine;
		lnMatchLine[0].x() = lnMatchLine[1].x() = 0.;
		lnMatchLine[0].y() = lnMatchLine[1].y() = 0.;
		
		const double dScale = m_pDivision->m_pDrawView->GetScale();
		TCHAR szDim[32]={0,} , szTextDir[4]={0,};
		if(m_pDivision->IsHorizontal())
		{
			int eDimLevel = DIM_LEVEL_X;
			CIsLine3d lnBase;
			for(list<CaDraw_TagPos*>::const_iterator itr=m_pDivision->m_plstDimension->begin();itr != m_pDivision->m_plstDimension->end();++itr)
			{
				SPRINTF_T(szDim, _T("%.0lf") ,SAFE_ROUND(fabs((((*itr)->m_ptStart.x() - (*itr)->m_ptEnd.x())*dScale)),1));
				CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_extDimStyle.nTextHeight ,  1.f);
				const double dDimLength = fabs((*itr)->m_ptEnd.x() - (*itr)->m_ptStart.x());
				const double dTxtLength = TextStyle.GetTextLength(szDim);
				_upper = (dDimLength > dTxtLength) ? true : !_upper;
								
				if(DIM_LEVEL_X == eDimLevel)
				{
					eDimLevel = (*itr)->pTITEM->nLevel;
					_upper = true;
				}
				else if((int)eDimLevel != (*itr)->pTITEM->nLevel)
				{
					eDimLevel = (*itr)->pTITEM->nLevel;
					_upper = true;
				}
				
				lnBase[0].y() = lnBase[1].y() = m_pDivision->m_lnBaseLine[0].y();
				CaDraw_TagPos* pTagPos=*itr;
				if((SAFE_ROUND(dScale*dDimLength,1) >= 1.))
				{
					const double dDistToDimLevel = env.m_outside.GetDistToDimLine((DIM_LEVEL_0 == eDimLevel) ? DIM_LEVEL_X : ((DIM_LEVEL_1 == eDimLevel) ? DIM_LEVEL_0 : DIM_LEVEL_1));
					double dy = (*itr)->m_ptStart.y() - lnBase.start().y();
					dy /= fabs(dy);

					file << _T("<DIMENSION>|");
					double d=((*itr)->m_ptStart.x()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = (lnBase.start().y() + dy*dDistToDimLevel); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptEnd.x()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = (lnBase.end().y() + dy*dDistToDimLevel); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptStart.x()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptStart.y()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					file << IsString::TrimedTrailingZero(dScale/dDrawingScale) << _T("|");
					file << env.m_extDimStyle.rLevel << _T(",Hor,");
					(true == _upper) ? STRCPY_T(szTextDir , _T("U")) : STRCPY_T(szTextDir , _T("D"));
					if(dDimLength < dTxtLength*0.75) STRCAT_T(szTextDir , _T("R"));
					file << szTextDir << _T(",") << env.m_extDimStyle.m_sDimStyle << std::endl;
				}
			}
		}
		else
		{
			int eDimLevel=DIM_LEVEL_X;
			CIsLine3d lnBase;
			for(list<CaDraw_TagPos*>::const_iterator itr=m_pDivision->m_plstDimension->begin();itr != m_pDivision->m_plstDimension->end();++itr)
			{
				SPRINTF_T(szDim, _T("%.0lf") ,SAFE_ROUND(fabs((((*itr)->m_ptStart.y() - (*itr)->m_ptEnd.y())*dScale)),1));
				CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_extDimStyle.nTextHeight , 1.f);
				const double dDimLength = fabs((*itr)->m_ptEnd.y() - (*itr)->m_ptStart.y());
				const double dTxtLength = TextStyle.GetTextLength(szDim);
				_upper = (dDimLength > dTxtLength) ? true : !_upper;
								
				if(DIM_LEVEL_X == eDimLevel)
				{
					eDimLevel = (*itr)->pTITEM->nLevel;
					_upper = true;
				}
				else if(eDimLevel != (*itr)->pTITEM->nLevel)
				{
					eDimLevel = (*itr)->pTITEM->nLevel;
					_upper = true;
				}
				
				lnBase[0].x() = lnBase[1].x() = m_pDivision->m_lnBaseLine[0].x();
				if((SAFE_ROUND(dScale*dDimLength,1) >= 1.))
				{
					const double dDistToDimLevel = env.m_outside.GetDistToDimLine((DIM_LEVEL_0 == eDimLevel) ? DIM_LEVEL_X : ((DIM_LEVEL_1 == eDimLevel) ? DIM_LEVEL_0 : DIM_LEVEL_1));
					double dx = (*itr)->m_ptStart.x() - lnBase.start().x();
					dx /= fabs(dx);

					file << _T("<DIMENSION>|");
					double d=(lnBase.start().x() + dx*dDistToDimLevel); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptStart.y());file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = (lnBase.end().x() + dx*dDistToDimLevel); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptEnd.y()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptStart.x()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					d = ((*itr)->m_ptStart.y()); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
					file << IsString::TrimedTrailingZero(dScale/dDrawingScale) << _T("|");
					file << env.m_extDimStyle.rLevel << _T(",Ver,");
					(true == _upper) ? STRCPY_T(szTextDir , _T("U")) : STRCPY_T(szTextDir , _T("D"));
					if(dDimLength < dTxtLength*0.75) STRCAT_T(szTextDir , _T("R"));
					file << szTextDir << _T(",") << env.m_extDimStyle.m_sDimStyle << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	The CExtWriter::WriteETIMTag function
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@param	pTagPos	a parameter of type CaDraw_TagPos*
	@return	void	
*/
void CExtWriter::WriteETIMTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale)
{
	assert(pTagPos && "pTagPos is NULL");

	if(pTagPos)
	{
		TCHAR szDir[16]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[4];
		CIsPoint3d ptName[2];
		TCHAR      szName[2][64]={0,};
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		{
			m_szBuf[0] = '\0';
			STRCPY_T(szName[0],pTagPos->pTITEM->name[0]->c_str());
			CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_anno.m_nLineTextHeight , 1.f);
			double nTxtLen1 = TextStyle.GetTextLength(szName[0]);
			STRCPY_T(szName[1],pTagPos->pTITEM->name[1]->c_str());
			double nTxtLen2 = TextStyle.GetTextLength(szName[1]);
			
			if(m_pDivision->IsHorizontal())
			{
				STRCPY_T(szDir,_T("VER"));
				
				ptStart = pTagPos->m_ptStart;
				ptEnd.x() = ptStart.x() + m_pDivision->m_vecTagDir.dx()*(1);
				ptEnd.y() = ptStart.y() + m_pDivision->m_vecTagDir.dy()*(1);
				
				ptName[0].x() = pTagPos->m_ptEnd.x() - env.m_nGap*0.5;
				ptName[0].y() = m_pDivision->m_lnUpperLine.start().y();
				ptName[1].x() = pTagPos->m_ptEnd.x() + env.m_anno.m_nLineTextHeight + env.m_nGap*0.5;
				ptName[1].y() = m_pDivision->m_lnUpperLine.start().y();
				
				ptVertex[0].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText);
				ptVertex[0].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText);
				ptVertex[1].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText + 10000);
				ptVertex[1].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText + 10000);
				ptVertex[2].x() = pTagPos->m_ptEnd.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText + 30000);
				ptVertex[2].y() = pTagPos->m_ptEnd.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText + 30000);
				ptVertex[3].x() = ptVertex[2].x() + m_pDivision->m_vecTagDir.dx()*nTxtLen1;
				ptVertex[3].y() = m_pDivision->m_lnUpperLine.start().y();
			}
			else
			{
				STRCPY_T(szDir,_T("HOR"));
				
				ptStart = pTagPos->m_ptStart;
				ptEnd.x() = ptStart.x() + m_pDivision->m_vecTagDir.dx()*(1);
				ptEnd.y() = ptStart.y() + m_pDivision->m_vecTagDir.dy()*(1);
				
				ptName[0].x() = m_pDivision->m_lnUpperLine.start().x();
				ptName[0].y() = pTagPos->m_ptEnd.y() + env.m_nGap*0.5;
				ptName[1].x() = m_pDivision->m_lnUpperLine.start().x();
				ptName[1].y() = pTagPos->m_ptEnd.y() - env.m_anno.m_nLineTextHeight - env.m_nGap*0.5;
				
				ptVertex[0].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText);
				ptVertex[0].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText);
				ptVertex[1].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText + 10000);
				ptVertex[1].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText + 10000);
				ptVertex[2].x() = pTagPos->m_ptEnd.x() + m_pDivision->m_vecTagDir.dx()*(env.m_nDistText + 30000);
				ptVertex[2].y() = pTagPos->m_ptEnd.y() + m_pDivision->m_vecTagDir.dy()*(env.m_nDistText + 30000);
				ptVertex[3].x() = m_pDivision->m_lnUpperLine.start().x();
				ptVertex[3].y() = ptVertex[2].y() + m_pDivision->m_vecTagDir.dy()*nTxtLen1;
			}
			
			file << _T("<PLINE>|");
			double d=0;
			d = ptVertex[0].x(); file << d*dDrawingScale << _T(",");
			d = ptVertex[0].y(); file << d*dDrawingScale << _T(",");
			d = ptVertex[1].x(); file << d*dDrawingScale << _T(",");
			d = ptVertex[1].y(); file << d*dDrawingScale << _T(",");
			d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
			d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
			d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
			d = ptVertex[3].y(); file << d*dDrawingScale << _T("|");
			file << _T("0,0,0,0,0") << std::endl;

			file << _T("<TEXT>|");
			d = ptName[0].x(); file << d*dDrawingScale << _T(",");
			d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
			file << _T("1.5,0,");
			file << (szName[0][0] ? szName[0] : _T("Nil")) << _T(",Right,0|");
			file << _T("0,0,3") << std::endl;
			
			file << _T("<TEXT>|");
			d = ptName[1].x(); file << d*dDrawingScale << _T(",");
			d = ptName[1].y(); file << d*dDrawingScale << _T(",0,");
			file << _T("1.5,0,");
			file << (szName[1][0] ? szName[1] : _T("Nil")) << _T(",Right,0|");
			file << _T("0,0,3") << std::endl;
		}
	}
}

/**	
	@brief	write view coordinate
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@return	void	
*/
void CExtWriter::WriteViewCoord(OFSTREAM_T& file,const double& dDrawingScale)
{
	static TCHAR szBuf[512]={0,};
	
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if((m_pDivision->m_szDim[0][0] || m_pDivision->m_szDim[1][0]))
	{
		TCHAR szDir[16]={0,},szAlign[2]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[2][5];
		CIsPoint3d ptName[2];
		TCHAR szName[2][64]={0,};
		STRING_T rAlign = _T("Left Bottom");

		CIsPoint3d ptMid(
			(m_pDivision->m_lnBaseLine[0].x() + m_pDivision->m_lnBaseLine[1].x())*0.5,
			(m_pDivision->m_lnBaseLine[0].y() + m_pDivision->m_lnBaseLine[1].y())*0.5,
			(m_pDivision->m_lnBaseLine[0].z() + m_pDivision->m_lnBaseLine[1].z())*0.5);
		CIsVect3d vec[2];
		vec[0].dx() = m_pDivision->m_lnBaseLine[0].x() - ptMid.x();
		vec[0].dy() = m_pDivision->m_lnBaseLine[0].y() - ptMid.y();
		vec[0].dz() = m_pDivision->m_lnBaseLine[0].z() - ptMid.z();
		vec[0].Normalize();
		vec[1].dx() = m_pDivision->m_lnBaseLine[1].x() - ptMid.x();
		vec[1].dy() = m_pDivision->m_lnBaseLine[1].y() - ptMid.y();
		vec[1].dz() = m_pDivision->m_lnBaseLine[1].z() - ptMid.z();
		vec[1].Normalize();

		{
			if(m_pDivision->IsHorizontal())
			{
				STRCPY_T(szDir,_T("VER"));
				rAlign = (m_pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");
				if((m_pDivision->m_vecTagDir.dx() > 0.) || (m_pDivision->m_vecTagDir.dy() > 0.))
					szAlign[0] = 'L';
				else	szAlign[0] = 'L';
				
				if(m_pDivision->m_szDim[0][0])
				{
					const double nLen = env.m_coord.m_oTextStyle.GetTextLength(m_pDivision->m_szDim[0]);
					ptName[0]     = m_pDivision->m_lnBaseLine[0];
					ptName[0].x()+= vec[0].dx()*(env.m_coord.m_dLabelOffset);
					ptName[0].y()-= m_pDivision->m_vecTagDir.dy()*env.m_coord.m_dLabelOffset;

					ptVertex[0][0] = ptName[0];
					ptVertex[0][0].x() += env.m_coord.m_dLabelOffset;
					ptVertex[0][0].y() -= (_T("Left Bottom") == rAlign) ? env.m_coord.m_dLabelOffset : -env.m_coord.m_dLabelOffset;
					ptVertex[0][1] = ptVertex[0][0];
					ptVertex[0][1].y() += (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2) : -(nLen + env.m_coord.m_dLabelOffset*2);
					ptVertex[0][2] = ptVertex[0][1];
					ptVertex[0][2].x() -= env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset*2.0;
					ptVertex[0][3] = ptVertex[0][2];
					ptVertex[0][3].y() -= (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2);
					ptVertex[0][4] = ptVertex[0][0];
				}
				
				if(m_pDivision->m_szDim[1][0])
				{
					const double nLen = env.m_coord.m_oTextStyle.GetTextLength(m_pDivision->m_szDim[1]);
					ptName[1]    = m_pDivision->m_lnBaseLine[1];
					ptName[1].x() += vec[1].dx()*(env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset);
					ptName[1].y()-= m_pDivision->m_vecTagDir.dy()*env.m_coord.m_dLabelOffset;

					ptVertex[1][0] = ptName[1];
					ptVertex[1][0].x() += env.m_coord.m_dLabelOffset;
					ptVertex[1][0].y() -= (_T("Left Bottom") == rAlign) ? env.m_coord.m_dLabelOffset : -env.m_coord.m_dLabelOffset;
					ptVertex[1][1] = ptVertex[1][0];
					ptVertex[1][1].y() += (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2) : -(nLen + env.m_coord.m_dLabelOffset*2);
					ptVertex[1][2] = ptVertex[1][1];
					ptVertex[1][2].x() -= env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset*2.0;
					ptVertex[1][3] = ptVertex[1][2];
					ptVertex[1][3].y() -= (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2.0);
					ptVertex[1][4] = ptVertex[1][0];
				}
			}
			else
			{	
				STRCPY_T(szDir,_T("HOR"));
				rAlign = (m_pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");
				if((m_pDivision->m_vecTagDir.dx() > 0.) || (m_pDivision->m_vecTagDir.dy() > 0.))
					szAlign[0] = 'L';
				else	szAlign[0] = 'L';
				
				if(m_pDivision->m_szDim[0][0])
				{
					const double nLen = env.m_coord.m_oTextStyle.GetTextLength(m_pDivision->m_szDim[0]);
					ptName[0]    = m_pDivision->m_lnBaseLine[0];
					ptName[0].x() -= m_pDivision->m_vecTagDir.dx()*env.m_coord.m_dLabelOffset;
					ptName[0].y() += vec[0].dy()*(env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset);

					ptVertex[0][0] = ptName[0];
					ptVertex[0][0].x() -= (_T("Left Bottom") == rAlign) ? env.m_coord.m_dLabelOffset : -env.m_coord.m_dLabelOffset;
					ptVertex[0][0].y() -= env.m_coord.m_dLabelOffset;
					ptVertex[0][1] = ptVertex[0][0];
					ptVertex[0][1].x() += (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2.0);
					ptVertex[0][2] = ptVertex[0][1];
					ptVertex[0][2].y() += env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset*2.0;
					ptVertex[0][3] = ptVertex[0][2];
					ptVertex[0][3].x() -= (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2.0);
					ptVertex[0][4] = ptVertex[0][0];
				}
				
				if(m_pDivision->m_szDim[1][0])
				{
					const double nLen = env.m_coord.m_oTextStyle.GetTextLength(m_pDivision->m_szDim[1]);
					ptName[1]    = m_pDivision->m_lnBaseLine[1];
					ptName[1].x() -= m_pDivision->m_vecTagDir.dx()*env.m_coord.m_dLabelOffset;
					ptName[1].y() += vec[1].dy()*(env.m_coord.m_dLabelOffset);

					ptVertex[1][0] = ptName[1];
					ptVertex[1][0].x() -= (_T("Left Bottom") == rAlign) ? env.m_coord.m_dLabelOffset : -env.m_coord.m_dLabelOffset;
					ptVertex[1][0].y() -= env.m_coord.m_dLabelOffset;
					ptVertex[1][1] = ptVertex[1][0];
					ptVertex[1][1].x() += (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2.0);
					ptVertex[1][2] = ptVertex[1][1];
					ptVertex[1][2].y() += env.m_coord.m_oTextStyle.height() + env.m_coord.m_dLabelOffset*2.0;
					ptVertex[1][3] = ptVertex[1][2];
					ptVertex[1][3].x() -= (_T("Left Bottom") == rAlign) ? (nLen + env.m_coord.m_dLabelOffset*2.0) : -(nLen + env.m_coord.m_dLabelOffset*2.0);
					ptVertex[1][4] = ptVertex[1][0];
				}
			}
			
			if(m_pDivision->m_szDim[0][0])
			{
				ptStart     = m_pDivision->m_lnUpperLine.start();
				ptEnd       = m_pDivision->m_lnUpperLine.start();

				file << _T("<TEXT>|");
				file << ptName[0].x()*dDrawingScale << _T(",");
				file << ptName[0].y()*dDrawingScale << _T(",0,");
				file << env.m_coord.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_coord.m_oTextStyle.width()*dDrawingScale << _T(",");
				file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
				file << m_pDivision->m_szDim[0] << _T(",") << rAlign << _T(",0|");
				file << env.m_coord.m_oLabelProp.level << _T(",") << env.m_coord.m_oLabelProp.color << _T(",");
				file << env.m_coord.m_oTextStyle << std::endl;
				
				if(env.m_coord.IsBoxOut())
				{
					file << _T("<PLINE>|");
					for(int i=0;i < 5;++i)
					{
						double d = 0;
						d = ptVertex[0][i].x(); file << d*dDrawingScale << _T(",");
						d = ptVertex[0][i].y(); file << d*dDrawingScale;
						((i+1) < 5) ? (file << _T(",")) : (file << _T("|"));
					}
					file << env.m_coord.m_oLabelProp << std::endl;
				}
			}
			
			if(m_pDivision->m_szDim[1][0])
			{
				ptStart     = m_pDivision->m_lnUpperLine.end();
				ptEnd       = m_pDivision->m_lnUpperLine.end();

				file << _T("<TEXT>|");
				file << ptName[1].x()*dDrawingScale << _T(",");
				file << ptName[1].y()*dDrawingScale << _T(",0,");
				file << env.m_coord.m_oTextStyle.height()*dDrawingScale << _T(",") << env.m_coord.m_oTextStyle.width()*dDrawingScale << _T(",");
				file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
				file << m_pDivision->m_szDim[1] << _T(",") << rAlign << _T(",0|");
				file << env.m_coord.m_oLabelProp.level << _T(",") << env.m_coord.m_oLabelProp.color << _T(",");
				file << env.m_coord.m_oTextStyle << std::endl;

				if(env.m_coord.IsBoxOut())
				{
					file << _T("<PLINE>|");
					for(int i=0;i < 5;i++)
					{
						double d = 0;
						d = ptVertex[1][i].x(); file << d*dDrawingScale << _T(",");
						d = ptVertex[1][i].y(); file << d*dDrawingScale;
						((i+1) < 5) ? (file << _T(",")) : (file << _T("|"));
					}
					file << env.m_coord.m_oLabelProp << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	write structure tag
	@author	백흠경
	@param	file	a parameter of type ofstream&
	@param	pTagPos	a parameter of type CaDraw_TagPos*
	@return	void	
*/
void CExtWriter::WriteStruTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale)
{
	assert(pTagPos && "pTagPos is NULL");
	static TCHAR szBuf[512]={0,};

	if(pTagPos)
	{
		TCHAR   szDir[16]={0,};
		int     nCount=0;
		CIsPoint3d ptName[2];
		TCHAR   szName[2][64]={0,};
		CIsPoint3d ptOrigin;
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if(!pTagPos->pTITEM->name[0]->empty())
		{
			STRCPY_T(szName[0],pTagPos->pTITEM->name[0]->c_str());
			nCount++;
		}
		if(!pTagPos->pTITEM->name[1]->empty())
		{
			STRCPY_T(szName[1],pTagPos->pTITEM->name[1]->c_str());
			CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_stru.nTextHeight[0] , 1.f);
			double nTxtLen2 = TextStyle.GetTextLength(szName[1]);
			nCount++;
		}

		{
			double nNeck  =	3*(env.m_anno.m_nLineTextHeight);
			double nStrPos= env.m_stru.nDist - env.m_stru.m_dRadius;
			if(m_pDivision->IsHorizontal())
			{
				STRCPY_T(szDir,_T("HOR"));
				ptOrigin.x() = pTagPos->m_ptEnd.x();
				ptOrigin.y() = pTagPos->m_ptEnd.y() + m_pDivision->m_vecTagDir.dy()*(env.m_stru.nDist);
				
				if(nCount > 1)
				{	/// two names
					CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_stru.nTextHeight[0] , env.m_nWidthFactor);
					double nTxtLen = TextStyle.GetTextLength(szName[0]);
					ptName[0].x() = ptOrigin.x();
					ptName[0].y() = ptOrigin.y() + env.m_stru.nTextHeight[0]*0.5;

					TextStyle.height() = env.m_stru.nTextHeight[1];
					nTxtLen = TextStyle.GetTextLength(szName[1]);
					ptName[1].x() = ptOrigin.x();
					ptName[1].y() = ptOrigin.y() - env.m_stru.nTextHeight[1]*0.5;
				}
				else
				{
					CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_stru.nTextHeight[0] , env.m_nWidthFactor);
					double nTxtLen = TextStyle.GetTextLength(szName[0]);
					ptName[0].x() = ptOrigin.x();
					ptName[0].y() = ptOrigin.y();
				}
			}
			else
			{
				STRCPY_T(szDir,_T("HOR"));
				ptOrigin.x() = pTagPos->m_ptEnd.x() + m_pDivision->m_vecTagDir.dx()*(env.m_stru.nDist);
				ptOrigin.y() = pTagPos->m_ptEnd.y();
				
				if(nCount > 1)
				{
					/// two names
					CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_stru.nTextHeight[0] , env.m_nWidthFactor);
					double nTxtLen = TextStyle.GetTextLength(szName[0]);
					ptName[0].x() = ptOrigin.x();
					ptName[0].y() = ptOrigin.y() + env.m_stru.nTextHeight[0]*0.5;

					TextStyle.height() = env.m_stru.nTextHeight[1];
					nTxtLen = TextStyle.GetTextLength(szName[1]);
					ptName[1].x() = ptOrigin.x();
					ptName[1].y() = ptOrigin.y() - env.m_stru.nTextHeight[1]*0.5;
				}
				else
				{
					CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_stru.nTextHeight[0] , env.m_nWidthFactor);
					double nTxtLen = TextStyle.GetTextLength(szName[0]);
					ptName[0].x() = ptOrigin.x();
					ptName[0].y() = ptOrigin.y();
				}
			}
			
			double nStartDist = (env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC + env.m_anno.m_nLineTextHeight);
			CIsPoint3d ptVertex[8];
			ptVertex[0]   = pTagPos->m_ptStart;
			ptVertex[1].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
			ptVertex[1].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
			ptVertex[2].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*nStartDist;
			ptVertex[2].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*nStartDist;
			ptVertex[3].x() = pTagPos->m_ptStart.x() + m_pDivision->m_vecTagDir.dx()*(nStartDist + env.m_extDimStyle.nTextHeight);
			ptVertex[3].y() = pTagPos->m_ptStart.y() + m_pDivision->m_vecTagDir.dy()*(nStartDist + env.m_extDimStyle.nTextHeight);
			ptVertex[4] = ptVertex[3];
			ptVertex[5].x()  = pTagPos->m_ptEnd.x() + m_pDivision->m_vecTagDir.dx()*(nStartDist + env.m_extDimStyle.nTextHeight);
			ptVertex[5].y()  = pTagPos->m_ptEnd.y() + m_pDivision->m_vecTagDir.dy()*(nStartDist + env.m_extDimStyle.nTextHeight);
			ptVertex[6] = ptVertex[5];
			ptVertex[7].x()  = pTagPos->m_ptEnd.x() + m_pDivision->m_vecTagDir.dx()*(nStrPos);
			ptVertex[7].y()  = pTagPos->m_ptEnd.y() + m_pDivision->m_vecTagDir.dy()*(nStrPos);

			if(nCount > 1)
			{
				file << _T("<PLINE>|");
				double d = 0;
				d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[5].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[5].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[7].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[7].y(); file << d*dDrawingScale << _T("|");
				file << env.m_stru.m_oLeaderProp << std::endl;

				file << _T("<CIRCLE>|");
				d = ptOrigin.x(); file << d*dDrawingScale << _T(",");
				d = ptOrigin.y(); file << d*dDrawingScale << _T(",0,");
				d = env.m_stru.m_dRadius; file << d*dDrawingScale << _T("|");
				file << env.m_stru.m_oLeaderProp << std::endl;

				file << _T("<TEXT>|");
				d = ptName[0].x(); file << d*dDrawingScale << _T(",");
				d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
				d = env.m_stru.nTextHeight[0];
				file << d*dDrawingScale << _T(",") << d*dDrawingScale << _T(",0,");
				file << (szName[0][0] ? szName[0] : _T("Nil")) << _T(",Center Center,0|");
				file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
				file << env.m_stru.m_oTextStyle << std::endl;

				file << _T("<TEXT>|");
				d = ptName[1].x(); file << d*dDrawingScale << _T(",");
				d = ptName[1].y(); file << d*dDrawingScale << _T(",0,");
				d = env.m_stru.nTextHeight[1];
				file << d*dDrawingScale << _T(",") << d*dDrawingScale << _T(",0,");
				file << (szName[1][0] ? szName[1] : _T("Nil")) << _T(",Center Center,0|");
				file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
				file << env.m_stru.m_oTextStyle << std::endl;
			}
			else
			{
				file << _T("<PLINE>|");
				double d = 0;
				d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[5].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[5].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[7].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[7].y(); file << d*dDrawingScale << _T("|");
				file << env.m_stru.m_oLeaderProp << std::endl;

				file << _T("<CIRCLE>|");
				d = ptOrigin.x(); file << d*dDrawingScale << _T(",");
				d = ptOrigin.y(); file << d*dDrawingScale << _T(",0,");
				d = env.m_stru.m_dRadius; file << d*dDrawingScale << _T("|");
				file << env.m_stru.m_oLeaderProp << std::endl;

				file << _T("<TEXT>|");
				d = ptName[0].x(); file << d*dDrawingScale << _T(",");
				d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
				d = env.m_stru.nTextHeight[0];
				file << d*dDrawingScale << _T(",") << d*dDrawingScale << _T(",0,");
				file << (szName[0][0] ? szName[0] : _T("Nil")) << _T(",Center Center,0|");
				file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
				file << env.m_stru.m_oTextStyle << std::endl;
			}
		}
	}
}
