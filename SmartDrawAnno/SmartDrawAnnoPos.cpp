#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsString.h>
#include <Tokenizer.h>
#include "ExtDivision.h"
#include "SmartDrawAnnoPos.h"
#include "DimLevel.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"

using namespace tag_pos;

CaDraw_TagPos::CaDraw_TagPos(CaDraw_TagItem* pTagItem) : nLevelOn(DIM_LEVEL_X) , pTITEM(pTagItem)
{
	bFixed = false;
	ZeroMemory(&rect,sizeof(RECT_T));
	nOverlap = 0;
	nPrevSpace = nNextSpace = 0;
	next = NULL;
}

CaDraw_TagPos::~CaDraw_TagPos()
{
}

/**	
	@brief	register to dimension
	@author	humkyung
	@date	2010.05.28
	@param	
	@return	bool	
*/
bool CaDraw_TagPos::OnRegisterToDimension(CDimLevel* pDimLevel)
{
	if(nLevelOn & pDimLevel->GetLevel())
	{
		(*pDimLevel) += this;
		
		return true;
	}

	return false;
}

/**	
	@brief	register pipe to dimension
	@author	humkyung
	@date	2013.06.16
	@param	
	@return	bool	
*/
bool CaDraw_PipeTagPos::OnRegisterToDimension(CDimLevel* pDimLevel)
{
	if(nLevelOn & pDimLevel->GetLevel())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		CaDraw_TagPipe* pPipe = static_cast<CaDraw_TagPipe*>(this->pTITEM->m_pEnt);
		const double dNominalSizeInMetric = pPipe->pipe()->GetNominalPipeSizeInMetric();
		
		const double dTagDisplaySize = env.m_pipe.m_ext.GetTagDisplaySize();
		const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
		if((dNominalSizeInMetric > dTagDisplaySize) && (dNominalSizeInMetric > dDimDisplaySize))
		{
			(*pDimLevel) += this;
		}
		
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	@brief	write pipe tag to given file
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@param	pTagPos	a parameter of type CaDraw_TagPos*
	@return	void
*/
void CaDraw_PipeTagPos::Write(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		TCHAR szBuf[1024];	/// buffer to store text string

		TCHAR    szDir[16]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[4];
		CIsPoint3d ptName[2];
		TCHAR    szName[2][64]={0,};
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		STRING_T rAlign = _T("Left Bottom");

		szBuf[0] = '\0';
		if(DOUBLE_TAG == this->type)
		{
			if(this->next)
			{	/// have elevation
				for(list<int>::iterator itr=pDivision->m_plstGroupCount->begin();itr != pDivision->m_plstGroupCount->end();itr++)
				{
					if(*itr == this->nGroup)
					{
						pDivision->m_plstGroupCount->erase(itr);
						break;
					}
				}
				(pDivision->m_nInGroup)--;
				WriteElevTag(file,pDivision,dDrawingScale);
			}
			else
			{
				TCHAR* pName1=pDivision->GetFirstTagNo(this->pTITEM);
				STRCPY_T(szName[0],pName1);
				CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_anno.m_nLineTextHeight , 1.f);
				const double nTxtLen1 = TextStyle.GetTextLength(szName[0]);
				TCHAR* pName2=pDivision->GetSecondTagNo(this->pTITEM);
				STRCPY_T(szName[1],pName2);
				const double nTxtLen2 = TextStyle.GetTextLength(szName[1]);

				const double nStartDist = pDivision->GetMaxDistToDimLine() + env.m_anno.m_nLineTextHeight;
				if(pDivision->IsHorizontal())
				{
					STRCPY_T(szDir , _T("VER"));
					
					ptStart = this->m_ptStart;
					ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
					ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);

					if((pDivision->IsTopSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetTopSideAlignment())) || (pDivision->IsBottomSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetBottomSideAlignment())))
					{
						rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");

						ptName[0].x() = this->m_ptEnd.x() - env.m_pipe.m_ext.m_dLabelOffset;
						ptName[0].y() = pDivision->m_lnUpperLine.start().y();
						ptName[1].x() = this->m_ptEnd.x() + env.m_pipe.m_ext.m_oElevTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;
						ptName[1].y() = pDivision->m_lnUpperLine.start().y();
					}
					else
					{
						rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Left Bottom") : _T("Right Bottom");

						const double dDist = env.m_outside.InsidePipeEqpDist();
						ptName[0].x() = this->m_ptEnd.x() - env.m_pipe.m_ext.m_dLabelOffset;
						ptName[0].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*dDist;
						ptName[1].x() = this->m_ptEnd.x() + env.m_pipe.m_ext.m_oElevTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;
						ptName[1].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*dDist;
					}

					ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
					ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
					ptVertex[2] = this->m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
					ptVertex[3].x() = ptVertex[2].x() + pDivision->m_vecTagDir.dx()*nTxtLen1;
					ptVertex[3].y() = pDivision->m_lnUpperLine.start().y();
				}
				else
				{
					STRCPY_T(szDir , _T("HOR"));
					
					ptStart = this->m_ptStart;
					ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
					ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);

					if((pDivision->IsLeftSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetLeftSideAlignment())) || (pDivision->IsRightSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetRightSideAlignment())))
					{
						rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");

						ptName[0].x() = pDivision->m_lnUpperLine.start().x();
						ptName[0].y() = this->m_ptEnd.y() + env.m_pipe.m_ext.m_dLabelOffset;
						ptName[1].x() = pDivision->m_lnUpperLine.start().x();
						ptName[1].y() = this->m_ptEnd.y() - env.m_pipe.m_ext.m_oElevTextStyle.height() - env.m_pipe.m_ext.m_dLabelOffset;
					}
					else
					{
						rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Left Bottom") : _T("Right Bottom");

						const double dDist = env.m_outside.InsidePipeEqpDist();
						ptName[0].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*dDist;
						ptName[0].y() = this->m_ptEnd.y() + env.m_pipe.m_ext.m_dLabelOffset;
						ptName[1].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*dDist;
						ptName[1].y() = this->m_ptEnd.y() - env.m_pipe.m_ext.m_oElevTextStyle.height() - env.m_pipe.m_ext.m_dLabelOffset;
					}

					ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
					ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
					ptVertex[2] = this->m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
					ptVertex[3].x() = pDivision->m_lnUpperLine.start().x();
					ptVertex[3].y() = ptVertex[2].y() + pDivision->m_vecTagDir.dy()*nTxtLen1;
				}

				double d = 0.0;
				//////////////////////////////////////////////////////////////////////////
				file << _T("<GROUP>|0,PIPE,Pat 01") << std::endl;

				file << _T("<PLINE>|");
				d = ptVertex[0].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[0].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[1].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[1].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
				d = ptVertex[3].y(); file << d*dDrawingScale << _T("|");
				file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;

				file << _T("<TEXT>|");
				d = ptName[0].x(); file << d*dDrawingScale << _T(",");
				d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
				file << env.m_pipe.m_ext.m_oLineNoTextStyle.height()*dDrawingScale << _T(",");
				file << env.m_pipe.m_ext.m_oLineNoTextStyle.height()*env.m_pipe.m_ext.m_oLineNoTextStyle.GetWidthFactor()*dDrawingScale << _T(",");
				file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
				file << (szName[0][0] ? szName[0] : _T("")) << "," << rAlign << _T(",0|");
				file << env.m_pipe.m_ext.m_oLineNoProp.level << _T(",") << env.m_pipe.m_ext.m_oLineNoProp.color << _T(",");
				file << env.m_pipe.m_ext.m_oLineNoTextStyle << std::endl;

				file << _T("<TEXT>|");
				d = ptName[1].x(); file << d*dDrawingScale << _T(",");
				d = ptName[1].y(); file << d*dDrawingScale << _T(",0,");
				file << env.m_pipe.m_ext.m_oElevTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_ext.m_oElevTextStyle.height()*env.m_pipe.m_ext.m_oElevTextStyle.GetWidthFactor()*dDrawingScale << _T(",");
				file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
				file << (szName[1][0] ? szName[1] : _T("")) << _T(",") << rAlign << _T(",0|");
				file << env.m_pipe.m_ext.m_oElevProp.level << _T(",") << env.m_pipe.m_ext.m_oElevProp.color << _T(",");
				file << env.m_pipe.m_ext.m_oElevTextStyle << std::endl;

				file << _T("</GROUP>") << std::endl;
				//////////////////////////////////////////////////////////////////////////

				/// write leader line
				if(env.m_pipe.m_ext.m_bLeaderLine)
				{
					//////////////////////////////////////////////////////////////////////////
					file << _T("<LINE>|");
					d = (this->pTITEM->m_ptOrigin.x()); file << d*dDrawingScale << _T(",");
					d = (this->pTITEM->m_ptOrigin.y()); file << d*dDrawingScale << _T(",0,");
					d = ptStart.x(); file << d*dDrawingScale << _T(",");
					d = ptStart.y(); file << d*dDrawingScale << _T(",0|");
					file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
					//////////////////////////////////////////////////////////////////////////
				}
			}
		}
		else if(SINGLE_TAG == this->type)
		{
			WriteSinglePipeTag(file,pDivision,dDrawingScale);
		}
	}
}

/**	
	@brief	write single pipe to given file
	@author	humkyung
	@date	????.??.??
	@param	file	a parameter of type ofstream&
	@return	void	
*/
void CaDraw_PipeTagPos::WriteSinglePipeTag(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale)
{
	assert(pDivision && "pDivision is NULL");
	
	if(pDivision)
	{
		TCHAR szDir[16] ={0,};
		TCHAR szAlign[2]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[4];
		CIsPoint3d ptName[2];
		TCHAR szName[2][64]={0,};
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		STRING_T rAlign = _T("Left Bottom");

		bool _bFind=false;
		for(list<int>::iterator itrGroupCount=pDivision->m_plstGroupCount->begin();itrGroupCount != pDivision->m_plstGroupCount->end();++itrGroupCount)
		{
			if(*itrGroupCount == this->nGroup)
			{
				_bFind = true;
				break;
			}
		}
		if(!_bFind)
		{
			(pDivision->m_nInGroup)++; /// increase (m_pDivision->m_nInGroup)
			pDivision->m_plstGroupCount->push_back(this->nGroup);
		}
		
		TCHAR* pName=pDivision->GetFirstTagNo(this->pTITEM);
		STRCPY_T(szName[0],pName);
		CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_anno.m_nLineTextHeight , 1.f);
		double nTxtLen = TextStyle.GetTextLength(szName[0]);
		
		const double nStartDist = pDivision->GetMaxDistToDimLine() + env.m_anno.m_nLineTextHeight;
		if(pDivision->IsHorizontal())
		{
			STRCPY_T(szDir, _T("VER"));
			
			ptStart = this->m_ptStart;
			ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
			ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
			
			if((pDivision->IsTopSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetTopSideAlignment())) || (pDivision->IsBottomSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetBottomSideAlignment())))
			{
				rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");

				ptName[0].x() = this->m_ptEnd.x() + (this->rect.height*0.5) - env.m_pipe.m_ext.m_dLabelOffset;
				ptName[0].y() = pDivision->m_lnUpperLine.start().y();
				ptName[1].x() = this->m_ptEnd.x() + env.m_pipe.m_ext.m_oLineNoTextStyle.height();
				ptName[1].y() = pDivision->m_lnUpperLine.start().y();
			}
			else
			{
				rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Left Bottom") : _T("Right Bottom");

				const double dDist = env.m_outside.InsidePipeEqpDist();///(float)(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC + env.m_outside.m_nDistE + env.m_anno.m_nLineTextHeight);
				ptName[0].x() = this->m_ptEnd.x() + (this->rect.height*0.5) - env.m_pipe.m_ext.m_dLabelOffset;
				ptName[0].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*dDist;
				ptName[1].x() = this->m_ptEnd.x() + env.m_pipe.m_ext.m_oLineNoTextStyle.height();
				ptName[1].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*dDist;
			}
			
			ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
			ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
			ptVertex[2].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE) + this->rect.height*0.5;
			ptVertex[2].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
			ptVertex[3].x() = ptVertex[2].x() + pDivision->m_vecTagDir.dx()*nTxtLen;
			ptVertex[3].y() = pDivision->m_lnUpperLine.start().y();
		}
		else
		{
			STRCPY_T(szDir, _T("HOR"));
						
			ptStart = this->m_ptStart;
			ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
			ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
			
			if((pDivision->IsLeftSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetLeftSideAlignment())) || (pDivision->IsRightSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetRightSideAlignment())))
			{
				rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");

				ptName[0].x() = pDivision->m_lnUpperLine.start().x();
				ptName[0].y() = this->m_ptEnd.y() - (this->rect.height*0.5) + env.m_pipe.m_ext.m_dLabelOffset;
				ptName[1].x() = pDivision->m_lnUpperLine.start().x();
				ptName[1].y() = this->m_ptEnd.y() - env.m_pipe.m_ext.m_oElevTextStyle.height();
			}
			else
			{
				rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Left Bottom") : _T("Right Bottom");

				const double dDist = env.m_outside.InsidePipeEqpDist();
				ptName[0].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*dDist;
				ptName[0].y() = this->m_ptEnd.y() - (this->rect.height*0.5) + env.m_pipe.m_ext.m_dLabelOffset;
				ptName[1].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*dDist;
				ptName[1].y() = this->m_ptEnd.y() - env.m_pipe.m_ext.m_oElevTextStyle.height();
			}
			
			ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
			ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
			ptVertex[2].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
			ptVertex[2].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE) - this->rect.height*0.5;
			ptVertex[3].x() = pDivision->m_lnUpperLine.start().x();
			ptVertex[3].y() = ptVertex[2].y() + pDivision->m_vecTagDir.dy()*nTxtLen;
		}

		double d = 0;
//////////////////////////////////////////////////////////////////////////
		file << _T("<GROUP>|0,PIPE,Pat 01") << std::endl;

		file << _T("<PLINE>|");
		d = ptVertex[0].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[0].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[1].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[1].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[3].y(); file << d*dDrawingScale << _T("|");
		file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
		
		file << _T("<TEXT>|");
		d = ptName[0].x(); file << d*dDrawingScale << _T(",");
		d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
		file << env.m_pipe.m_ext.m_oLineNoTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_ext.m_oLineNoTextStyle.width()*dDrawingScale << _T(",");
		file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
		file << (szName[0][0] ? szName[0] : _T("")) << _T(",") << rAlign << _T(",0|");
		file << env.m_pipe.m_ext.m_oLineNoProp.level << _T(",") << env.m_pipe.m_ext.m_oLineNoProp.color << _T(",");
		file << env.m_pipe.m_ext.m_oLineNoTextStyle << std::endl;

		file << _T("</GROUP>") << std::endl;
//////////////////////////////////////////////////////////////////////////
		/// write leader line
		if(env.m_pipe.m_ext.m_bLeaderLine)
		{	
			file << _T("<LINE>|");
			d = (this->pTITEM->m_ptOrigin.x()); file << d*dDrawingScale << _T(",");
			d = (this->pTITEM->m_ptOrigin.y()); file << d*dDrawingScale << _T(",0,");
			d = ptStart.x(); file << d*dDrawingScale << _T(",");
			d = ptStart.y(); file << d*dDrawingScale << _T(",0|");
			file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
		}
	}
}

/**	
	@brief	write elevation pipe to given file
	@author	humkyung
	@date	????.??.??
	@param	file		a parameter of type ofstream&
	@param	pDivision	a parameter of type CExtDivision*
	@return	void	
*/
void CaDraw_PipeTagPos::WriteElevTag(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		TCHAR   szDir[8]={0,};
		TCHAR   szAlign[2]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[4];
		CIsPoint3d ptName[2];
		TCHAR   szName[2][64]={0,};
		STRING_T rAlign = _T("Left Bottom");
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		TCHAR* pName=pDivision->GetFirstTagNo(this->pTITEM);
		STRCPY_T(szName[0],pName);
		double nTxtLen = env.m_pipe.m_ext.m_oLineNoTextStyle.GetTextLength(szName[0]);
		
		const double nStartDist = pDivision->GetMaxDistToDimLine() + env.m_anno.m_nLineTextHeight;
		if(pDivision->IsHorizontal())
		{
			STRCPY_T(szDir,_T("VER"));
			ptStart = this->m_ptStart;
			ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
			ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
			
			if((pDivision->IsTopSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetTopSideAlignment())) || (pDivision->IsBottomSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetBottomSideAlignment())))
			{
				rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");

				ptName[0].x() = this->m_ptEnd.x() - env.m_nGap;
				ptName[0].y() = pDivision->m_lnUpperLine.start().y();
			}
			else
			{
				rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Left Bottom") : _T("Right Bottom");

				const float nDist=(float)(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC + env.m_outside.m_nDistE + env.m_anno.m_nLineTextHeight);
				ptName[0].x() = this->m_ptEnd.x() - env.m_pipe.m_ext.m_dLabelOffset;
				ptName[0].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*nDist;
			}
			
			ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
			ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
			ptVertex[2] = this->m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
			ptVertex[3].x() = ptVertex[2].x() + pDivision->m_vecTagDir.dx()*nTxtLen;
			ptVertex[3].y() = pDivision->m_lnUpperLine.start().y();
		}
		else
		{
			STRCPY_T(szDir, _T("HOR"));
			ptStart = this->m_ptStart;
			ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
			ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
			
			if((pDivision->IsLeftSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetLeftSideAlignment())) || (pDivision->IsRightSide() && (OUTSIDE_ALIGN == env.m_pipe.m_ext.GetRightSideAlignment())))
			{
				rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");

				ptName[0].x() = pDivision->m_lnUpperLine.start().x();
				ptName[0].y() = this->m_ptEnd.y() + env.m_pipe.m_ext.m_dLabelOffset;
			}
			else
			{
				rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Left Bottom") : _T("Right Bottom");

				const float nDist=(float)(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC + env.m_outside.m_nDistE + env.m_anno.m_nLineTextHeight);
				ptName[0].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*nDist;
				ptName[0].y() = this->m_ptEnd.y() + env.m_pipe.m_ext.m_dLabelOffset;
			}
			
			ptVertex[0] = this->m_ptStart + pDivision->m_vecTagDir*nStartDist;
			ptVertex[1] = this->m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
			ptVertex[2] = this->m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
			ptVertex[3].x() = pDivision->m_lnUpperLine.start().x();
			ptVertex[3].y() = ptVertex[2].y() + pDivision->m_vecTagDir.dy()*nTxtLen;
		}
		
		double d = 0;
		//////////////////////////////////////////////////////////////////////////
		file << _T("<GROUP>|0,ELEV,Pat 01") << std::endl;

		file << _T("<PLINE>|");
		d = ptVertex[0].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[0].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[1].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[1].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[2].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[2].y(); file << d*dDrawingScale << _T(",");
		d = ptVertex[3].x(); file << d*dDrawingScale << _T(",");
		d = ptVertex[3].y(); file << d*dDrawingScale << _T("|");
		file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
		
		file << _T("<TEXT>|");
		d = ptName[0].x(); file << d*dDrawingScale << _T(",");
		d = ptName[0].y(); file << d*dDrawingScale << _T(",0,");
		file << env.m_pipe.m_ext.m_oLineNoTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_ext.m_oLineNoTextStyle.width()*dDrawingScale << _T(",");
		file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
		file << (szName[0][0] ? szName[0] : _T("")) << _T(",") << rAlign << _T(",0|");
		file << env.m_pipe.m_ext.m_oLineNoProp.level << _T(",") << env.m_pipe.m_ext.m_oLineNoProp.color << _T(",");
		file << env.m_pipe.m_ext.m_oLineNoTextStyle << std::endl;

		file << _T("</GROUP>") << std::endl;
		
		/// write leader line
		if(env.m_pipe.m_ext.m_bLeaderLine)
		{
			file << _T("<LINE>|");
			d = (this->pTITEM->m_ptOrigin.x()); file << d*dDrawingScale << _T(",");
			d = (this->pTITEM->m_ptOrigin.y()); file << d*dDrawingScale << _T(",0,");
			d = ptStart.x(); file << d*dDrawingScale << _T(",");
			d = ptStart.y(); file << d*dDrawingScale << _T(",0|");
			file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
		}
		
		TCHAR* pElev=pDivision->GetSecondTagNo(this->pTITEM);
		if(pElev)
		{
			TCHAR* pBOP=STRSTR_T(pElev, _T("BOP"));
			if(pBOP)
				STRCPY_T(szName[0],pBOP);
			else	STRCPY_T(szName[0],pElev);
		}else	STRCPY_T(szName[0], _T("BOP EL."));
		nTxtLen = env.m_pipe.m_ext.m_oElevTextStyle.GetTextLength(szName[0]);
		if(pDivision->IsHorizontal())
		{
			rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");

			ptStart   = this->m_ptEnd;
			ptStart.x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist) + (env.m_pipe.m_ext.m_oElevTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset*2);
			ptStart.y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist + (pDivision->m_nInGroup)*(env.m_outside.m_nElevOffset));
			ptEnd.x() = ptStart.x();
			ptEnd.y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.ElevDist());

			ptName[1].x() = this->m_ptEnd.x() + env.m_pipe.m_ext.m_oElevTextStyle.height() + env.m_pipe.m_ext.m_dLabelOffset;
			ptName[1].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.ElevDist());
		}
		else
		{
			rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");

			ptStart   = this->m_ptEnd;
			ptStart.x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist + (pDivision->m_nInGroup)*env.m_outside.m_nElevOffset);
			ptStart.y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist) - env.m_pipe.m_ext.m_oElevTextStyle.height() - (env.m_pipe.m_ext.m_dLabelOffset*2);
			ptEnd.x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.ElevDist());
			ptEnd.y() = ptStart.y();
			
			ptName[1].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.ElevDist());
			ptName[1].y() = m_ptEnd.y() - env.m_pipe.m_ext.m_oElevTextStyle.height() - env.m_pipe.m_ext.m_dLabelOffset;
		}
		
		int nCount=1;
		static CIsLine3d ln[8];
		ln[0][0] = ptStart;
		if(pDivision->IsHorizontal())
		{
			ln[0][1].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist);
			ln[0][1].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist + (pDivision->m_nInGroup)*env.m_outside.m_nElevOffset);
		}
		else
		{
			ln[0][1].x() = this->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist + (pDivision->m_nInGroup)*env.m_outside.m_nElevOffset);
			ln[0][1].y() = this->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist);
		}
		
		for(CaDraw_TagPos* ptr=this->next;ptr && (nCount < 8);ptr=ptr->next,nCount++)
		{
			ln[nCount][0] = ptStart;
			if(pDivision->IsHorizontal())
			{
				ln[nCount][1].x() = ptr->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist) + ptr->rect.height*0.5 + env.m_nGap*0.5;
				ln[nCount][1].y() = ptr->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist + pDivision->m_vecTagDir.dy()*(pDivision->m_nInGroup)*env.m_outside.m_nElevOffset);
			}
			else
			{
				ln[nCount][1].x() = ptr->m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nElevStartDist + (pDivision->m_nInGroup)*env.m_outside.m_nElevOffset);
				ln[nCount][1].y() = ptr->m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nElevStartDist) - ptr->rect.height*0.5;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		file << _T("<LINE>|");
		d = ptStart.x(); file << d*dDrawingScale << _T(",");
		d = ptStart.y(); file << d*dDrawingScale << _T(",0,");
		d = ptEnd.x(); file << d*dDrawingScale << _T(",");
		d = ptEnd.y(); file << d*dDrawingScale << _T(",0|");
		file << env.m_pipe.m_ext.m_oLeaderProp << std::endl;
		
		file << _T("<TEXT>|");
		d = ptName[1].x(); file << d*dDrawingScale << _T(",");
		d = ptName[1].y(); file << d*dDrawingScale << _T(",0,");
		file << env.m_pipe.m_ext.m_oElevTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_ext.m_oElevTextStyle.width()*dDrawingScale << _T(",");
		file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
		file << (szName[0][0] ? szName[0] : _T("")) << _T(",");
		file << rAlign << _T(",0|");	
		file << env.m_pipe.m_ext.m_oElevProp.level << _T(",") << env.m_pipe.m_ext.m_oElevProp.color << _T(",");
		file << env.m_pipe.m_ext.m_oElevTextStyle << std::endl;

		//////////////////////////////////////////////////////////////////////////		
		for(int i=0;i < nCount;++i)
		{	
			file << _T("<LEADER>|");
			file << ln[i].end().x()*dDrawingScale   << _T(",") << ln[i].end().y()*dDrawingScale   << _T(",");
			file << ln[i].start().x()*dDrawingScale << _T(",") << ln[i].start().y()*dDrawingScale << _T("|");
			file << env.m_pipe.m_ext.m_oLeaderProp.level << _T(",") << env.m_pipe.m_ext.m_oLeaderProp.color << _T(",");
			file << env.m_pipe.m_ext.m_sArrowType << _T(",") << env.m_pipe.m_ext.m_dArrowSize*dDrawingScale << _T(",") << env.m_pipe.m_ext.m_dArrowSize/3.0*dDrawingScale << std::endl;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
	@brief	write equipment tag to given file
	@author	humkyung
	@param	file	a parameter of type ofstream&
	@param	pTagPos	a parameter of type CaDraw_TagPos*
	@remarks\n
	;2003.03.17 - if bWriteCoordinate is true, write eqp coordinate else write 'Nil'
	
	@return	void
*/
void CaDraw_EquiTagPos::Write(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		TCHAR    szDir[16]={0,};
		CIsPoint3d ptStart,ptEnd;
		CIsPoint3d ptVertex[4];
		CIsPoint3d ptName[2];
		TCHAR    szName[2][64]={0,};
		STRING_T rAlign = _T("Left Bottom");
		CIsPoint3d ptBox[4];
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		STRCPY_T(szName[0],pTITEM->name[0]->c_str());
		double nTxtLen1 = env.m_equi.m_oNameTextStyle.GetTextLength(szName[0]);
		STRCPY_T(szName[1],pTITEM->name[1]->c_str());
		double nTxtLen2 = env.m_equi.m_oCoordTextStyle.GetTextLength(szName[1]);

		{
			const double nStartDist = pDivision->GetMaxDistToDimLine() + env.m_anno.m_nLineTextHeight;
			if(pDivision->IsHorizontal())
			{
				STRCPY_T(szDir , _T("VER"));
				if((pDivision->IsTopSide() && (OUTSIDE_ALIGN == env.m_equi.GetTopSideAlignment())) || (pDivision->IsBottomSide() && (OUTSIDE_ALIGN == env.m_equi.GetBottomSideAlignment())))
				{
					rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Right Bottom") : _T("Left Bottom");
				}
				else
				{
					rAlign = (pDivision->m_vecTagDir.dy() > 0) ? _T("Left Bottom") : _T("Right Bottom");
				}
				ptStart = m_ptStart;
				ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
				ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);

				const double nHeight=fabs((env.m_equi.m_oNameTextStyle.height() + env.m_equi.m_oCoordTextStyle.height())*0.5 - env.m_equi.m_oNameTextStyle.height());
				m_ptEnd.x()   += nHeight;

				if((pDivision->IsTopSide() && (OUTSIDE_ALIGN == env.m_equi.GetTopSideAlignment())) || (pDivision->IsBottomSide() && (OUTSIDE_ALIGN == env.m_equi.GetBottomSideAlignment())))
				{
					ptName[0].x() = m_ptEnd.x() - env.m_nGap;
					ptName[0].y() = pDivision->m_lnUpperLine.start().y();
					ptName[1].x() = m_ptEnd.x() + env.m_equi.m_oCoordTextStyle.height() + env.m_nGap;
					ptName[1].y() = pDivision->m_lnUpperLine.start().y();
				}
				else
				{
					const double dDist = env.m_outside.InsidePipeEqpDist();

					ptName[0].x() = m_ptEnd.x() - env.m_nGap;
					ptName[0].y() = m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(dDist);
					ptName[1].x() = m_ptEnd.x() + env.m_equi.m_oCoordTextStyle.height() + env.m_nGap;
					ptName[1].y() = m_ptEnd.y() + pDivision->m_vecTagDir.dy()*(dDist);
				}

				ptVertex[0] = m_ptStart + pDivision->m_vecTagDir*nStartDist;
				ptVertex[1] = m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
				ptVertex[2] = m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
				ptVertex[3].x() = ptVertex[2].x() + pDivision->m_vecTagDir.dx()*nTxtLen1;
				ptVertex[3].y() = pDivision->m_lnUpperLine.start().y();
			}
			else
			{
				STRCPY_T(szDir , _T("HOR"));
				if((pDivision->IsLeftSide() && (OUTSIDE_ALIGN == env.m_equi.GetLeftSideAlignment())) || (pDivision->IsRightSide() && (OUTSIDE_ALIGN == env.m_equi.GetRightSideAlignment())))
				{
					rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Right Bottom") : _T("Left Bottom");
				}
				else
				{
					rAlign = (pDivision->m_vecTagDir.dx() > 0) ? _T("Left Bottom") : _T("Right Bottom");
				}
				ptStart = m_ptStart;
				ptEnd.x() = ptStart.x() + pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
				ptEnd.y() = ptStart.y() + pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);

				const double nHeight=fabs((env.m_equi.m_oNameTextStyle.height() + env.m_equi.m_oCoordTextStyle.height())*0.5 - env.m_equi.m_oNameTextStyle.height());
				m_ptEnd.y() -= nHeight;

				if((pDivision->IsLeftSide() && (OUTSIDE_ALIGN == env.m_equi.GetLeftSideAlignment())) || (pDivision->IsRightSide() && (OUTSIDE_ALIGN == env.m_equi.GetRightSideAlignment())))
				{
					ptName[0].x() = pDivision->m_lnUpperLine.start().x();
					ptName[0].y() = m_ptEnd.y() + env.m_nGap;
					ptName[1].x() = pDivision->m_lnUpperLine.start().x();
					ptName[1].y() = m_ptEnd.y() - env.m_equi.m_oCoordTextStyle.height() - env.m_nGap;
				}
				else
				{
					const double dDist = env.m_outside.InsidePipeEqpDist();

					ptName[0].x() = m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(dDist);
					ptName[0].y() = m_ptEnd.y() + env.m_nGap;
					ptName[1].x() = m_ptEnd.x() + pDivision->m_vecTagDir.dx()*(dDist);
					ptName[1].y() = m_ptEnd.y() - env.m_equi.m_oCoordTextStyle.height() - env.m_nGap;
				}

				ptVertex[0] = m_ptStart + pDivision->m_vecTagDir*nStartDist;
				ptVertex[1] = m_ptStart + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD);
				ptVertex[2] = m_ptEnd + pDivision->m_vecTagDir*(nStartDist + env.m_outside.m_nDistD + env.m_outside.m_nDistE);
				ptVertex[3].x() = pDivision->m_lnUpperLine.start().x();
				ptVertex[3].y() = ptVertex[2].y() + pDivision->m_vecTagDir.dy()*nTxtLen1;
			}

			file << _T("<GROUP>|0,EQUI,Pat 01") << std::endl;

			double d = 0;
			file << _T("<PLINE>|");
			d = ptVertex[0].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[0].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[1].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[1].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[2].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[2].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptVertex[3].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T("|");
			/// color,layer,ltype,lscale,lweight
			file << env.m_equi.m_oLeaderProp << std::endl;

			file << _T("<TEXT>|");
			d = ptName[0].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptName[0].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			file << IsString::TrimedTrailingZero(env.m_equi.m_oNameTextStyle.height()*dDrawingScale) << _T(",");
			file << IsString::TrimedTrailingZero(env.m_equi.m_oNameTextStyle.height()*env.m_equi.m_oNameTextStyle.GetWidthFactor()*dDrawingScale) << _T(",");
			file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
			file << (szName[0][0] ? szName[0] : _T("Nil")) << "," << rAlign << _T(",0|");
			/// layer,color
			file << env.m_equi.m_oNameProp.level << _T(",") << env.m_equi.m_oNameProp.color << _T(",");
			/// text style,lweight
			file << env.m_equi.m_oNameTextStyle << std::endl;

			file << _T("<TEXT>|");
			d = ptName[1].x(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
			d = ptName[1].y(); file << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
			file << IsString::TrimedTrailingZero(env.m_equi.m_oCoordTextStyle.height()*dDrawingScale) << _T(",");
			file << IsString::TrimedTrailingZero(env.m_equi.m_oCoordTextStyle.height()*env.m_equi.m_oCoordTextStyle.GetWidthFactor()*dDrawingScale) << _T(",");
			file << ((STRING_T(szDir) == _T("VER")) ? _T("90,") : _T("0,"));
			file << (szName[1][0] ? szName[1] : _T("")) << (",") << rAlign << _T(",0|");
			file << env.m_equi.m_oCoordProp.level << _T(",") << env.m_equi.m_oCoordProp.color << _T(",");
			file << env.m_equi.m_oCoordTextStyle << std::endl;

			file << _T("</GROUP>") << std::endl;

			if(env.m_equi.m_bCenterLine)
			{ /// center line on
				CaDraw_EquiTagItem *pEquiTagItem = static_cast<CaDraw_EquiTagItem*>(pTITEM);
				if(false == pEquiTagItem->m_bWirteCenterline)
				{
					ptBox[0].x() = pTITEM->ptMin.x();
					ptBox[0].y() = pTITEM->m_ptOrigin.y();
					ptBox[1].x() = pTITEM->ptMax.x();
					ptBox[1].y() = pTITEM->m_ptOrigin.y();
					ptBox[2].x() = pTITEM->m_ptOrigin.x();
					ptBox[2].y() = pTITEM->ptMin.y();
					ptBox[3].x() = pTITEM->m_ptOrigin.x();
					ptBox[3].y() = pTITEM->ptMax.y();

					file << _T("<LINE>|");
					file << ptBox[0]*dDrawingScale << _T(",");
					file << ptBox[1]*dDrawingScale << _T("|");
					/// color,layer,ltype,lscale,lweight
					file << env.m_equi.m_oCenterlineProp << std::endl;

					file << _T("<LINE>|");
					file << ptBox[2]*dDrawingScale << _T(",");
					file << ptBox[3]*dDrawingScale << _T("|");
					file << env.m_equi.m_oCenterlineProp << std::endl;

					pEquiTagItem->m_bWirteCenterline = true;
				}
			}
		}
	}
}