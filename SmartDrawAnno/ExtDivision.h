#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <IsPoint3d.h>
#include <aDraw_View.h>
#include "Dimension.h"
#include "DraftLeader.h"
///#include "DraftView.h"
#include "DimLevel.h"

#include <fstream>
#include <vector>
#include <list>
using namespace std;

#define	MAXLOOP		10000

class CGroupCount
{
public:
	bool operator +=(int nGroup);
	bool operator -=(int nGroup);
	bool exist(int nGroup);
public:
	list<int> m_lstGroupCount;
};

class CExtWriter;
class CExtDivision
{
public:
	CExtDivision(const CIsLine3d& lnBaseLine,bool bOpen[],const CIsVect3d& vecTagDir,CaDraw_View* pDrawView);
	virtual ~CExtDivision();
public:
	void SetViewMatchLine(const TCHAR* pViewMatchLine)
	{
		STRNCPY_T(m_szViewMatchLine , pViewMatchLine , VIEW_MATCH_BUF_SIZ);
	}
	void SetDivisionHeight(const double& nLineNoHeight , const double& dOtherHeight);
	virtual bool IsMathLinePoint(const CIsPoint3d& pt);
	virtual bool IsInDivision(const CIsPoint3d& pt , const CIsVect3d& dir , const int& iType);
	CIsVect3d GetTagDir(){return m_vecTagDir;}
	
	virtual bool Register(CaDraw_TagItem* pTagItem);

	vector<CaDraw_TagPos*>* GetTagPosList(){return &(*m_pvtTagPos);}
	virtual void Annotate();
	double GetMaxDistToDimLine() const;
	bool HasDimLevel0()
	{
		if(m_pDimLevel0 && (0L != m_pDimLevel0->GetItemCount())) return true;
		return true;
	}
	bool HasDimLevel1()
	{
		if(m_pDimLevel1 && (0L != m_pDimLevel1->GetItemCount())) return true;
		return true;
	};
	bool HasDimLevel2()
	{
		if(m_pDimLevel2 && (0L != m_pDimLevel2->GetItemCount())) return true;
		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	/// write
	virtual void Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	//////////////////////////////////////////////////////////////////////////

	bool IsVertical();
	bool IsHorizontal();
	virtual bool IsCrossingXRay(const CIsPoint3d& pt);
	virtual bool IsCrossingYRay(const CIsPoint3d& pt);
	double GetXDistance(const CIsPoint3d& pt,const bool& bSign);
	double GetYDistance(const CIsPoint3d& pt,const bool& bSign);

	TCHAR* GetFirstTagNo(CaDraw_TagItem* pDITEM);
	TCHAR* GetSecondTagNo(CaDraw_TagItem* pDITEM);
protected:
	virtual void ComputateSpace(vector<CaDraw_TagPos*>& vtTagPos);
	void RunDimension();
	bool DoesExistEqualDim(const DIM_LEVEL_E nLevel,CaDraw_TagPos* pTagPos);
	
	void EraseSamePipeTagItem();
	void EraseSamePipeTagItem_Pt();
	void EraseSameEquiTagItem();
	void EraseSameStruTagItem();
	void RunTag();
	virtual void RunPipeAndEquiTag();
	void RunStrTag();
	void ArrangeTagPosWithAttr();

	virtual void RunLeftSideTag(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator itrFlag);
	virtual void RunRightSideTag(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator itrFlag);
	void CalculateUpperLine();
	void InsertBOP();
	
	virtual int  CountOverlap(vector<CaDraw_TagPos*>& vtTagPos);
	double OverlapCount(CaDraw_TagPos* pTagPos1,CaDraw_TagPos* pTagPos2);
	bool PickupMaxOverlapObject(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator& itr);
protected:
	int    GetPipeBOP(CaDraw_TagItem* pDITEM);
	void   GroupByBOP();
	void   GroupByTOP();
	void   GroupByCL();
public:
	CaDraw_View* view() const{ return m_pDrawView;}
	CIsLine3d GetBaseLine();
	bool IsLeftSide() const;
	bool IsTopSide() const;
	bool IsRightSide() const;
	bool IsBottomSide() const;
	CIsLine3d	 m_lnBaseLine,m_lnUpperLine;
	double	 m_nHeight[2];	/// 0 = Line No , 1 = Other
	bool     m_bOpen[2],m_bOver[2];
	CIsVect3d m_vecTagDir;
	TCHAR	 m_szViewMatchLine[VIEW_MATCH_BUF_SIZ + 1];	/// match line data
	TCHAR	 m_szDim[2][64];				/// view coordinate text
	
	vector<CaDraw_TagPos*>*  m_pvtTagPos;			/// 영역을 찾을때 사용하는 temprary 변수

	vector<CaDraw_TagPos*>* m_pvtPipeTagPos;
	vector<CaDraw_TagPos*>* m_pvtEquiTagPos;
	vector<CaDraw_TagPos*>* m_pvtStruTagPos;
	vector<CaDraw_TagPos*>* m_pvtOtherTagPos;
	vector<CaDraw_TagPos*>* m_pvtDimTagPos;
	
	vector<CaDraw_TagPos*>* m_pvtBOP;

	list<CaDraw_TagPos*>* m_plstDimension;
	list<CDraftLeader*>* m_plstLeader;
	
	int  m_nInGroup;
	list<int>* m_plstGroupCount;
protected:
	CaDraw_View* m_pDrawView;

	int  m_nGroupCount;
	TCHAR m_szBuf[512];

	CDimLevel* m_pDimLevel0;
	CDimLevel* m_pDimLevel1;
	CDimLevel* m_pDimLevel2;

	friend class CExtWriter;
};
