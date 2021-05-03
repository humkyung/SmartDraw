// aDraw_AnnotationView.h: interface for the CSmartDrawAnnoView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADRAW_ANNOTATIONVIEW_H__3532E573_6537_4843_BDFD_87DE82905D3E__INCLUDED_)
#define AFX_ADRAW_ANNOTATIONVIEW_H__3532E573_6537_4843_BDFD_87DE82905D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Enum.h"
#include <aDraw_View.h>
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoStru.h"
#include "SmartDrawAnnoItem.h"

class CExternalTerritory;
class CSmartDrawAnnoView
{
public:
	CaDraw_View* drawView() const
	{
		return m_pDrawView;
	}

	CSmartDrawAnnoView(CaDraw_View* pView);
	virtual ~CSmartDrawAnnoView();
public:
	virtual void Annotate(const RECT_T& ExtentRect , CExternalTerritory* pTerritory);

	int CreateTagEntities();	/// 2012.02.21 added by humkyung

	int GetTagEntList(list<CaDraw_TagEntity*>* pTagEntList) const;
	CIsPoint3d GetTagPoint(const CIsPoint3d& ptTag,const CIsPoint3d& ptStart,const int& dir);
	/*
	void SetParent(CLogicView* pParent){m_pParent = pParent;}
	void Write(FILE* fp);

	const char*    GetViewName(){return m_szViewName;}
	const VIEW_DIR_T   GetViewDir() const {return m_enmView;}
	const SQVOLUME GetVolume(){return m_volModel;}
	const POINT_T  GetViewOrigin(){return m_ptOrigin;}
	const double   GetViewWidth(){return m_nWidth;}
	const double   GetViewHeight(){return m_nHeight;}
	const double   GetViewScale(){return m_nScale;}
	const ROTATE_T GetViewAngle(){return m_nAngle;} const
	const SQVOLUME GetModelVolume(){return m_volModel;}

	list<CDraftEqui*>* GetDraftEquipmentList(){return m_plstDraftEqui;}
	list<CaDraw_TagItem*>*  GetDITEMList(){return m_plstDITEM;}

	bool operator +=(CDraftPipe* pDrawingPipe);
	bool operator +=(CDraftEqui* pDraftEqp);
	bool operator +=(CDraftStr* pDrawingStr);
	bool operator +=(CDraftETIM* pDrawingETIM);
	bool operator +=(CDraftEntity* pDraftEntity);
protected:
	void Clear();
	*/

	void Run4Pipe(const RECT_T& ExtentRect);
	void Run4VerticalPipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pPipe);
	void RunYDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pPipe);
	void RunXDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pPipe);
	virtual void RunXYDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pPipe);
	void Run4SlopePipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pPipe);
	void Run4Equi(const RECT_T& ExtentRect);
	void Run4Stru(const RECT_T& ExtentRect);
	void CreateTagItemForCableTray(const RECT_T& ExtentRect);
/*
public:
	AREA_T GetTagRange();
	void GetViewInfo(char* pViewInfo);
	void SetViewInfo(const char* pViewInfo);
	void RegisterTagObject(CaDraw_TagItem* pTagObject);
	void GetCoordText(char* pCoordText,int nDir,double nVal);
	const double GetCoordValue(int where);
	bool IsEqual(CSmartDrawAnnoView* pDraftView);
	POINT_T m_ptEdge[4];
*/
private:
	CExternalTerritory* m_pExternalTerritory;

	CaDraw_View* m_pDrawView;
	list<CaDraw_TagEntity*> m_oTagEntList;
};

#endif // !defined(AFX_ADRAW_ANNOTATIONVIEW_H__3532E573_6537_4843_BDFD_87DE82905D3E__INCLUDED_)
