// aDraw_StruTagPos.cpp: implementation of the CaDraw_StruTagPos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <Tokenizer.h>
#include <IsString.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoPos.h"
#include "aDraw_StruTagPos.h"

using namespace tag_pos;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CaDraw_StruTagPos::CaDraw_StruTagPos()
{
	bFixed = false;
	ZeroMemory(&rect,sizeof(RECT_T));
	nOverlap = 0;
	nPrevSpace = nNextSpace = 0;
	pTITEM = NULL;
	next = NULL;
}

CaDraw_StruTagPos::~CaDraw_StruTagPos()
{

}

/**	
	@brief	write structure tag information to given file.
	@author	HumKyung.BAEK
	@param	file	a parameter of type tstream&
	@param	pDivision	a parameter of type CExtDivision*
	@param	pEnv	a parameter of type CDraftEnv*
	@return	void	
*/
void CaDraw_StruTagPos::Write(OFSTREAM_T& file,CExtDivision* pDivision,const double& dDrawingScale)
{
	assert(pDivision && "pDivision is NULL");
	static TCHAR szBuf[512]={0,};
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	
	if(pDivision)
	{
		TCHAR szDir[16]={0,};
		int  nCount=0;
		CIsPoint3d ptName[2];
		TCHAR szName[2][64]={0,};
		CIsPoint3d ptOrigin;
		
		if(!pTITEM->name[0]->empty())
		{
			STRCPY_T(szName[0],pTITEM->name[0]->c_str());
			nCount++;
		}
		if(!pTITEM->name[1]->empty())
		{
			STRCPY_T(szName[1],pTITEM->name[1]->c_str());
			nCount++;
		}
		
		//////////////////////////////////////////////////////////////////////////
		/// 2006.9.29
		const double nStartDist = pDivision->GetMaxDistToDimLine() + env.m_anno.m_nLineTextHeight;
		double nNeck  =	3*(env.m_anno.m_nLineTextHeight);
		double nStrDist = env.m_stru.nDist;
		if(nStrDist - env.m_stru.m_dRadius < nStartDist + env.m_extDimStyle.nTextHeight)
		{
			nStrDist = nStartDist + env.m_extDimStyle.nTextHeight + env.m_stru.m_dRadius;
		}
		//////////////////////////////////////////////////////////////////////////
		double nStrPos= nStrDist - env.m_stru.m_dRadius;

		string rCoordValue;
		if(pDivision->IsHorizontal())
		{
			STRCPY_T(szDir, _T("HOR"));
			ptOrigin.x() = m_ptEnd.x();
			ptOrigin.y() = m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(nStrDist);
			
			if(nCount > 1)
			{	//! two names
				ptName[0].x() = ptOrigin.x();
				ptName[0].y() = ptOrigin.y() + (env.m_stru.nTextHeight[0])*1.f;
				
				ptName[1].x() = ptOrigin.x();
				ptName[1].y() = ptOrigin.y() - (env.m_stru.nTextHeight[1])*1.f;
			}
			else
			{				
				ptName[0].x() = ptOrigin.x();
				ptName[0].y() = ptOrigin.y();
			}

			///CaDraw_TagStru* pTagStru = static_cast<CaDraw_TagStru*>(pTITEM->m_pEnt);
			//if(/pTagStru) pTagStru->GetCoordValue(rCoordValue , pDivision->view() , BOTTOM_SIDE);
		}
		else
		{
			STRCPY_T(szDir, _T("HOR"));
			ptOrigin.x() = m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(nStrDist);
			ptOrigin.y() = m_ptEnd.y();
			
			if(nCount > 1)
			{
				/// two names
				ptName[0].x() = ptOrigin.x();
				ptName[0].y() = ptOrigin.y() + (env.m_stru.nTextHeight[0])*1.f;
				
				ptName[1].x() = ptOrigin.x();
				ptName[1].y() = ptOrigin.y() - (env.m_stru.nTextHeight[1])*1.f;
			}
			else
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult,szName[0],CIsFromString(_T("-")));
				
				ptName[0].x() = ptOrigin.x();
				ptName[0].y() = ptOrigin.y();
			}
		}
		
		CIsPoint3d ptVertex[8];
		ptVertex[0] = m_ptStart;
		ptVertex[1] = m_ptStart + pDivision->m_vecTagDir*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
		ptVertex[2] = m_ptStart + pDivision->m_vecTagDir*nStartDist;
		ptVertex[3] = m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
		ptVertex[4] = ptVertex[3];
		ptVertex[5] = m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
		ptVertex[6] = ptVertex[5];
		ptVertex[7] = m_ptEnd + pDivision->m_vecTagDir*(nStrPos);
		
		if(nCount > 1)
		{
			file << _T("<GROUP>|0,STRU,Pat 01") << std::endl;
//////////////////////////////////////////////////////////////////////////
			file << _T("<PLINE>|");
			double d = 0;
			d = ptVertex[2].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[2].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[5].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[5].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[7].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[7].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T("|");
			file << env.m_stru.m_oLeaderProp << std::endl;

			file << _T("<CIRCLE>|");
			d = ptOrigin.x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptOrigin.y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			d = env.m_stru.m_dRadius; file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T("|");
			file << env.m_stru.m_oLabelProp << std::endl;

			file << _T("<TEXT>|");
			d = ptName[0].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptName[0].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			d = env.m_stru.m_oTextStyle.height()*0.5;
			file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			file << IsString::TrimedTrailingZero(d*env.m_stru.m_oTextStyle.GetWidthFactor()*dDrawingScale) << _T(",0,");
			file << (szName[0][0] ? szName[0] : _T("Nil")) << _T(",Center Center,0|");
			file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
			file << env.m_stru.m_oTextStyle << std::endl;

			file << _T("<TEXT>|");
			d = ptName[1].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptName[1].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			d = env.m_stru.m_oTextStyle.height()*0.5;
			file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			file << IsString::TrimedTrailingZero(d*env.m_stru.m_oTextStyle.GetWidthFactor()*dDrawingScale) << _T(",") << _T(",0,");
			file << (szName[1][0] ? szName[1] : _T("Nil")) << _T(",Center Center,0|");
			file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
			file << env.m_stru.m_oTextStyle << std::endl;
//////////////////////////////////////////////////////////////////////////
			file << _T("</GROUP>") << std::endl;
		}
		else
		{
			file << _T("<GROUP>|0,STRU,Pat 01") << std::endl;
//////////////////////////////////////////////////////////////////////////
			file << _T("<PLINE>|");
			double d = 0;
			d = ptVertex[2].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[2].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[5].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[5].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[7].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[7].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T("|");
			file << env.m_stru.m_oLeaderProp << std::endl;

			file << _T("<CIRCLE>|");
			d = ptOrigin.x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptOrigin.y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			d = env.m_stru.m_dRadius; file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T("|");
			file << env.m_stru.m_oLeaderProp << std::endl;

			file << _T("<TEXT>|");
			d = ptName[0].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptName[0].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			d = env.m_stru.m_oTextStyle.height();
			file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			file << IsString::TrimedTrailingZero(d*env.m_stru.m_oTextStyle.GetWidthFactor()*dDrawingScale) << _T(",0,");
			file << (szName[0][0] ? szName[0] : _T("Nil")) << _T(",Center Center,0|");
			file << env.m_stru.m_oLabelProp.level << _T(",") << env.m_stru.m_oLabelProp.color << _T(",");
			file << env.m_stru.m_oTextStyle << std::endl;
//////////////////////////////////////////////////////////////////////////
			file << _T("</GROUP>") << std::endl;
		}
	}
}
