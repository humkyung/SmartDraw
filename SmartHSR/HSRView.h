#ifndef __HSR_VIEW_H__
#define __HSR_VIEW_H__

#include <Intent.h>
#include "HiddenDef.h"
#include "HSRCore.h"
#include "HSRViewFormat.h"

#define	MAX_VIEW_BOUNDARIES	100

class CHSRView
{
public:
	CHSRView();
	CHSRView(POINT_T ptLeftTop,POINT_T ptRightBottom);
	~CHSRView();
public:
	bool SetViewInfo(DList* pList);
	bool SetViewInfo(const POINT_T ptCenter,const double nWidth,const double nHeight,const double nScale , const CHSRViewFormat::ViewType& ViewType = CHSRViewFormat::RECTANGLE_VIEW);
	void Clipping(CHSRFace* pMergeFace);

	CHSRFace* GetViewFace(){return m_pViewFace;}
	int     GetNumOfViewBoundaryEdge(){return m_nViewBoundaryEdge;}
	PLINE_T GetViewBoundary(){return m_ViewBoundary;}
public:
	const double GetUOR();
	void SetUOR(const double& nUOR);
	void SetViewPoint(VIEWPOINT vpt);
	VIEWPOINT GetViewPoint();
	PSQPRIMITIVE WriteViewBorder(PSQPRIMITIVE ret);
	RECT_T GetViewArea();
	double GetViewScale();
	
	int m_nViewBoundaryEdge;
	LINE_T m_ViewBoundary[MAX_VIEW_BOUNDARIES];
private:
	void SetViewBoundray(const POINT_T ptCenter,const double nWidth,const double nHeight);
	PSQPRIMITIVE AppendPrimitive(PSQPRIMITIVE primitive,PSQPRIMITIVE append);
private:
	POINT_T m_ptLeftTop,m_ptRightBottom;
	double m_nScale;
	double m_nUOR;
	CHSRFace* m_pViewFace;

	VIEWPOINT   m_ViewPoint;
};

extern CHSRView* GetHiddenView();

#endif