#pragma once

#include "aDraw_Entity.h"
#include "aDraw_View.h"

#include <list>
using namespace std;

class ON_MODULE_EXT_DLL CaDraw_LogicView : public CaDraw_Entity
{
public:
	CaDraw_LogicView(void);
	~CaDraw_LogicView(void);

	ADRAW_DECLARE_FUNC()
private:
	void SetViewBoundaryAttr(CViewBoundaySegment aryViewBoundary[],const int nViewBoundaryEdge);
	void JoinViewLine(CViewBoundaySegment aryViewBoundary[],int& nViewBoundaryEdge , const double& nToler);
	void RemoveRedundantViewBoundary(CViewBoundaySegment aryViewBoundary[],int& nViewBoundaryEdge , const double& nToler);
	bool IsSamePoint(const CIsPoint3d& pt1,const CIsPoint3d& pt2 , const double& nToler);
	bool IsSameViewLine(const CIsLine3d& line1,const CIsLine3d& line2 , const double& nToler);
	
	CIsVolume m_volume;
	list<CaDraw_View*>* m_pViewList;
public:
	bool IsEnable(AnnoFilters* ) const;

	const CIsVolume volume() const;
	int GetViewList(list<CaDraw_View*>* pViewList) const;
	void CreateViewBoundary(const double& nToler , const int& iExtTagSide , const CaDraw_View::COORD_OPTION_E& coords);
	int GetViewIncludeEntity(list<CaDraw_View*>& ViewList , CaDraw_Entity* pEnt);
	bool IsAdjacentView(CaDraw_View* pView , const double& nTol=0.f);
	int AddView(CaDraw_View* pView);

	CViewBoundaySegment* m_pViewBoundarySegment;
};
