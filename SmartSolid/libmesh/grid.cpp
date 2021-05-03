// CGrid.cpp: implementation of the CGrid class.
//
//////////////////////////////////////////////////////////////////////
#include "..\StdAfx.h"
#include <math.h>
#include <graphics\geometry.h>
//#include "d:\\project\\spoon_0_0_1\\utility\\profile.h"
#include "libmesh2d.h"
#include "grid.h"

using namespace mesh;
unsigned long CGrid::ID=0L;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CGrid::CGrid()
{
	m_nId = CGrid::ID++;
	m_pNode = NULL;
	m_bDeleted = false;

	CalcRect();
#ifdef _DEBUG
	//PROFILE("CGrid()");
	//OutputDebugString("CGrid()");
#endif
}

/**	\brief
*/
CGrid::CGrid(const POINT_T pt[])
{
	m_nId = CGrid::ID++;
	m_pNode  = NULL;

	_node* p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(pt[0].x,LIBMESH2D_PRECISION);
	p->element.y = _round(pt[0].y,LIBMESH2D_PRECISION);
	p->element.z = _round(pt[0].z,LIBMESH2D_PRECISION);
	m_pNode = ListPush(m_pNode,p);

	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(pt[1].x,LIBMESH2D_PRECISION);
	p->element.y = _round(pt[1].y,LIBMESH2D_PRECISION);
	p->element.z = _round(pt[1].z,LIBMESH2D_PRECISION);
	m_pNode = ListPush(m_pNode,p);

	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(pt[2].x,LIBMESH2D_PRECISION);
	p->element.y = _round(pt[2].y,LIBMESH2D_PRECISION);
	p->element.z = _round(pt[2].z,LIBMESH2D_PRECISION);
	m_pNode = ListPush(m_pNode,p);

	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(pt[3].x,LIBMESH2D_PRECISION);
	p->element.y = _round(pt[3].y,LIBMESH2D_PRECISION);
	p->element.z = _round(pt[3].z,LIBMESH2D_PRECISION);
	m_pNode = ListPush(m_pNode,p);

	m_bDeleted = false;

	CalcRect();

#ifdef _DEBUG
	//PROFILE("CGrid()");
	//OutputDebugString("CGrid()");
#endif
}

/**	\brief
*/
CGrid::CGrid(const RECT_T& rect)
{
	m_nId = CGrid::ID++;
	_node* p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(rect.x,LIBMESH2D_PRECISION);
	p->element.y = _round(rect.y,LIBMESH2D_PRECISION);
	p->element.z = 0;
	m_pNode = ListPush(m_pNode,p);
	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(rect.x + rect.width,LIBMESH2D_PRECISION);
	p->element.y = _round(rect.y,LIBMESH2D_PRECISION);
	p->element.z = 0;
	m_pNode = ListPush(m_pNode,p);
	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(rect.x + rect.width,LIBMESH2D_PRECISION);
	p->element.y = _round(rect.y + rect.height,LIBMESH2D_PRECISION);
	p->element.z = 0;
	m_pNode = ListPush(m_pNode,p);
	p = (_node*)calloc(1,sizeof(_node));
	p->element.x = _round(rect.x,LIBMESH2D_PRECISION);
	p->element.y = _round(rect.y + rect.height,LIBMESH2D_PRECISION);
	p->element.z = 0;
	m_pNode = ListPush(m_pNode,p);

	m_bDeleted = false;

	CalcRect();
#ifdef _DEBUG
	//PROFILE("CGrid()");
	//OutputDebugString("CGrid()");
#endif
}

/**	\brief
*/
CGrid::~CGrid()
{
	if(NULL != m_pNode)
	{
		for(_node* ptr=m_pNode;ptr;)
		{
			_node* ptr_next=ptr->next;
			::free((void*)ptr);
			ptr = ptr_next;
		}
	}
#ifdef _DEBUG
	//PROFILE("~CGrid()");
	//OutputDebugString("~CGrid()");
#endif
}

/**	
	@brief	The CGrid::IsInnerPoint function

	@param	pt	a parameter of type const POINT_T &

	@return	bool	
*/
bool CGrid::IsInnerPoint(const POINT_T &pt)
{
	assert(m_pNode && "m_pNode is NULL");

	if(m_pNode)
	{
		double xMin=0,xMax=0;
		double yMin=0,yMax=0;

		_node* ptr=m_pNode;
		xMin = xMax = ptr->element.x;
		yMin = yMax = ptr->element.y;
		for(ptr=ptr->next;ptr;ptr = ptr->next)
		{
			if(ptr->element.x < xMin) xMin = ptr->element.x;
			if(ptr->element.x > xMax) xMax = ptr->element.x;
			if(ptr->element.y < yMin) yMin = ptr->element.y;
			if(ptr->element.y > yMax) yMax = ptr->element.y;
		}

		if((pt.x < xMin) || (pt.x > xMax)) return false;
		if((pt.y < yMin) || (pt.y > yMax)) return false;
	}

	return true;
}

/**	
	@brief	The CGrid::InsertPoint function

	@author	BHK

	@date	?

	@param	lstGrid	a parameter of type list<CGrid*>&
	@param	pt	a parameter of type const POINT_T&

	@return	if grid is segmented return true else return false.
*/
bool CGrid::InsertPoint(list<CGrid*>& newGridList , const POINT_T& pt)
{
	bool bSegmented=false;

	if(IsOnApex(pt))
	{
	}
	else if(IsOnBoundary(pt))
	{
		double xMin=0,xMax=0;
		double yMin=0,yMax=0;

		_node* ptr=m_pNode;
		xMin = xMax = ptr->element.x;
		yMin = yMax = ptr->element.y;
		for(ptr=ptr->next;ptr;ptr = ptr->next)
		{
			if(ptr->element.x < xMin) xMin = ptr->element.x;
			if(ptr->element.x > xMax) xMax = ptr->element.x;
			if(ptr->element.y < yMin) yMin = ptr->element.y;
			if(ptr->element.y > yMax) yMax = ptr->element.y;
		}

		if((pt.x > xMin) && (pt.x < xMax))
		{
			POINT_T _pt[4]={0};

			_pt[0].x = xMin;
			_pt[0].y = yMin;
			_pt[1].x = pt.x;
			_pt[1].y = yMin;
			_pt[2].x = pt.x;
			_pt[2].y = yMax;
			_pt[3].x = xMin;
			_pt[3].y = yMax;
			CGrid* pGrid = new CGrid(_pt);
			if(pGrid->IsValid())
			{
				newGridList.push_back(pGrid);
			}
			else
			{
				delete pGrid;
				int i = 1;
			}

			_pt[0].x = pt.x;
			_pt[0].y = yMin;
			_pt[1].x = xMax;
			_pt[1].y = yMin;
			_pt[2].x = xMax;
			_pt[2].y = yMax;
			_pt[3].x = pt.x;
			_pt[3].y = yMax;
			pGrid = new CGrid(_pt);
			if(pGrid->IsValid())
			{
				newGridList.push_back(pGrid);
			}
			else
			{
				delete pGrid;
				int i = 1;
			}

			bSegmented = true;
		}
		else
		{
			POINT_T _pt[4]={0};

			_pt[0].x = xMin;
			_pt[0].y = yMin;
			_pt[1].x = xMax;
			_pt[1].y = yMin;
			_pt[2].x = xMax;
			_pt[2].y = pt.y;
			_pt[3].x = xMin;
			_pt[3].y = pt.y;
			CGrid* pGrid = new CGrid(_pt);
			if(pGrid->IsValid())
			{
				newGridList.push_back(pGrid);
			}
			else
			{
				delete pGrid;
				int i = 1;
			}

			_pt[0].x = xMin;
			_pt[0].y = pt.y;
			_pt[1].x = xMax;
			_pt[1].y = pt.y;
			_pt[2].x = xMax;
			_pt[2].y = yMax;
			_pt[3].x = xMin;
			_pt[3].y = yMax;
			pGrid = new CGrid(_pt);
			if(pGrid->IsValid())
			{
				newGridList.push_back(pGrid);
			}
			else
			{
				delete pGrid;
				int i = 1;
			}

			bSegmented = true;
		}
	}
	else
	{
		double xMin=0,xMax=0;
		double yMin=0,yMax=0;

		_node* ptr=m_pNode;
		xMin = xMax = ptr->element.x;
		yMin = yMax = ptr->element.y;
		for(ptr=ptr->next;ptr;ptr = ptr->next)
		{
			if(ptr->element.x < xMin) xMin = ptr->element.x;
			if(ptr->element.x > xMax) xMax = ptr->element.x;
			if(ptr->element.y < yMin) yMin = ptr->element.y;
			if(ptr->element.y > yMax) yMax = ptr->element.y;
		}

		POINT_T _pt[4]={0};
		
		_pt[0].x = xMin;
		_pt[0].y = yMin;
		_pt[1].x = pt.x;
		_pt[1].y = yMin;
		_pt[2].x = pt.x;
		_pt[2].y = pt.y;
		_pt[3].x = xMin;
		_pt[3].y = pt.y;
		CGrid* pGrid = new CGrid(_pt);
		if(pGrid->IsValid())
		{
			newGridList.push_back(pGrid);
		}
		else
		{
			delete pGrid;
			int i = 1;
		}
		
		_pt[0].x = pt.x;
		_pt[0].y = yMin;
		_pt[1].x = xMax;
		_pt[1].y = yMin;
		_pt[2].x = xMax;
		_pt[2].y = pt.y;
		_pt[3].x = pt.x;
		_pt[3].y = pt.y;
		pGrid = new CGrid(_pt);
		if(pGrid->IsValid())
		{
			newGridList.push_back(pGrid);
		}
		else
		{
			delete pGrid;
			int i = 1;
		}

		_pt[0].x = pt.x;
		_pt[0].y = pt.y;
		_pt[1].x = xMax;
		_pt[1].y = pt.y;
		_pt[2].x = xMax;
		_pt[2].y = yMax;
		_pt[3].x = pt.x;
		_pt[3].y = yMax;
		pGrid = new CGrid(_pt);
		if(pGrid->IsValid())
		{
			newGridList.push_back(pGrid);
		}
		else
		{
			delete pGrid;
			int i = 1;
		}

		_pt[0].x = xMin;
		_pt[0].y = pt.y;
		_pt[1].x = pt.x;
		_pt[1].y = pt.y;
		_pt[2].x = pt.x;
		_pt[2].y = yMax;
		_pt[3].x = xMin;
		_pt[3].y = yMax;
		pGrid = new CGrid(_pt);
		if(pGrid->IsValid())
		{
			newGridList.push_back(pGrid);
		}
		else
		{
			delete pGrid;
			int i = 1;
		}

		bSegmented = true;
	}

	return bSegmented;
}

/**	\brief	The CGrid::IsOnBoundary function

	\param	pt	a parameter of type const POINT_T &

	\return	int	
*/
int CGrid::IsOnBoundary(const POINT_T &pt)
{
	int nIndex=1;
	snode<POINT_T>* ptr=NULL;
	for(ptr=m_pNode;ptr && ptr->next;ptr=ptr->next,nIndex++)
	{
		if(IsSameValue(ptr->element.x,ptr->next->element.x))
		{
			if(IsSameValue(pt.x,ptr->element.x)) return nIndex;
		}
		else
		{
			if(IsSameValue(pt.y,ptr->element.y)) return nIndex;
		}
	}

	if(IsSameValue(ptr->element.x,m_pNode->element.x))
	{
		if(IsSameValue(pt.x,ptr->element.x)) return 4;
	}
	else
	{
		if(IsSameValue(pt.y,ptr->element.y)) return 4;
	}

	return 0;
}

/**	\brief	The CGrid::IsOnApex function

	\param	pt	a parameter of type const POINT_T &

	\return	bool	
*/
bool CGrid::IsOnApex(const POINT_T &pt)
{
	for(_node* ptr=m_pNode;ptr;ptr=ptr->next)
	{
		if(IsSamePoint(ptr->element,pt)) return true;
	}

	return false;
}

/**	
	@brief	The CGrid::IsValid function


	@return	bool	
*/
bool CGrid::IsValid()
{
	double val[2]={0.f , 0.f};

	val[0] = m_pNode->element.x;
	val[1] = m_pNode->next->next->element.x;
	if(IsSameValue(val[0],val[1])) return false;

	val[0] = m_pNode->next->element.y;
	val[1] = m_pNode->next->next->next->element.y;
	if(IsSameValue(val[0],val[1])) return false;

	return true;
}

/**	
	@brief	check has an edge created with pt1 and pt2 or not.

	@author	BHK

	@date	?

	@param	line	a parameter of type const LINE_T&

	@return	return index of edge in grid if has an edge, otherwise return 0;
*/
int CGrid::HasEdge(const LINE_T& line)
{
	bool bFirst=false,bSecond=false;

	if(IsSamePoint(line.ptStart,line.ptEnd)) return 0;

	int nIndex=1;
	snode<POINT_T>* ptr=NULL;
	for(snode<POINT_T>* ptr=m_pNode;ptr && ptr->next;ptr = ptr->next,nIndex++)
	{
		bFirst = false;
		if(IsSamePoint(ptr->element,line.ptStart) || IsSamePoint(ptr->next->element,line.ptStart))
			bFirst = true;
		
		bSecond = false;
		if(IsSamePoint(ptr->element,line.ptEnd) || IsSamePoint(ptr->next->element,line.ptEnd))
			bSecond = true;

		if(bFirst && bSecond) return nIndex;
	}

	bFirst = false;
	if(IsSamePoint(ptr->element,line.ptStart) || IsSamePoint(m_pNode->element,line.ptStart))
		bFirst = true;
	bSecond = false;
	if(IsSamePoint(ptr->element,line.ptEnd) || IsSamePoint(m_pNode->element,line.ptEnd))
		bSecond = true;
	
	if(bFirst && bSecond) return nIndex;

	return 0;
}

/**	
	@brief	The CGrid::IsLeftSide function

	@author	BHK

	@date	?

	@param	nIndex	a parameter of type const int&
	@param	line	a parameter of type const LINE_T&

	@return	bool	
*/
bool CGrid::IsLeftSide(const int& nIndex,const LINE_T& line)
{
	POINT_T ptTest={0};
	for(snode<POINT_T>* ptr=m_pNode;ptr;ptr = ptr->next)
	{
		if(!IsSamePoint(ptr->element,line.ptStart) && !IsSamePoint(ptr->element,line.ptEnd))
		{
			ptTest = ptr->element;

			break;
		}
	}

	return CGeometry::IsLeftSidePoint(ptTest,line);
}

/**	\brief	The CGrid::IsDiagonalLine function

	\param	line	a parameter of type const LINE_T&

	\return	bool	
*/
bool CGrid::IsDiagonalLine(const LINE_T& line)
{
	static POINT_T pt[4]={0,};
	if(IsSameValue(line.ptStart.x,line.ptEnd.x) || IsSameValue(line.ptStart.y,line.ptEnd.y)) return false;

	double xMin=0,xMax=0;
	double yMin=0,yMax=0;
	double xMid=0,yMid=0;
	
	int i = 0;
	_node* ptr=m_pNode;
	xMin = xMax = pt[i].x = ptr->element.x;
	yMin = yMax = pt[i++].y = ptr->element.y;
	for(ptr=ptr->next;ptr;ptr = ptr->next,i++)
	{
		pt[i].x = ptr->element.x;
		pt[i].y = ptr->element.y;
		if(ptr->element.x < xMin) xMin = ptr->element.x;
		if(ptr->element.x > xMax) xMax = ptr->element.x;
		if(ptr->element.y < yMin) yMin = ptr->element.y;
		if(ptr->element.y > yMax) yMax = ptr->element.y;
	}
	xMid = (xMin + xMax)*0.5;
	yMid = (yMin + yMax)*0.5;

	if(IsSamePoint(pt[0],line.ptStart) && IsSamePoint(pt[2],line.ptEnd)) return true;
	if(IsSamePoint(pt[0],line.ptEnd) && IsSamePoint(pt[2],line.ptStart)) return true;
	if(IsSamePoint(pt[1],line.ptStart) && IsSamePoint(pt[3],line.ptEnd)) return true;
	if(IsSamePoint(pt[1],line.ptEnd) && IsSamePoint(pt[3],line.ptStart)) return true;

	//if(CLibMesh2d::IsSamePos(pt[0],line.ptStart,0.0000000001) && CLibMesh2d::IsSamePos(pt[2],line.ptEnd,0.0000000001)) return true;
	//if(CLibMesh2d::IsSamePos(pt[0],line.ptEnd,0.0000000001) && CLibMesh2d::IsSamePos(pt[2],line.ptStart,0.0000000001)) return true;
	//if(CLibMesh2d::IsSamePos(pt[1],line.ptStart,0.0000000001) && CLibMesh2d::IsSamePos(pt[3],line.ptEnd,0.0000000001)) return true;
	//if(CLibMesh2d::IsSamePos(pt[1],line.ptEnd,0.0000000001) && CLibMesh2d::IsSamePos(pt[3],line.ptStart,0.0000000001)) return true;
	
	return false;
	
	//return (l < 0.00001);
}


int CGrid::DoIncluding(const LINE_T& line)
{
	int nIndex = 1;

	const double dx1 = fabs(line.ptEnd.x - line.ptStart.x);
	const double dy1 = fabs(line.ptEnd.y - line.ptStart.y);
	snode<POINT_T>* ptr=NULL;
	for(ptr=m_pNode;ptr && ptr->next;ptr=ptr->next,++nIndex)
	{
		const double dx2 = fabs(ptr->next->element.x - ptr->element.x);
		const double dy2 = fabs(ptr->next->element.y - ptr->element.y);
		
		if((0.f == dy1) && (0.f == dy2) && (0.f == fabs(line.ptStart.y - ptr->element.y)))
		{
			const double sign1 = (ptr->element.x - line.ptStart.x ) * (ptr->next->element.x - line.ptStart.x);
			const double sign2 = (ptr->element.x - line.ptEnd.x ) * (ptr->next->element.x - line.ptEnd.x);
			if((sign1 <= 0.f) && (sign2 <= 0.f)) return nIndex;
		}
		if((0.f == dx1) && (0.f == dx2) && (0.f == fabs(line.ptStart.x - ptr->element.x)))
		{
			const double sign1 = (ptr->element.y - line.ptStart.y ) * (ptr->next->element.y - line.ptStart.y);
			const double sign2 = (ptr->element.y - line.ptEnd.y ) * (ptr->next->element.y - line.ptEnd.y);
			if((sign1 <= 0.f) && (sign2 <= 0.f)) return nIndex;
		}
	}
	
	const double dx2 = fabs(ptr->element.x - m_pNode->element.x);
	const double dy2 = fabs(ptr->element.y - m_pNode->element.y);
	if((0.f == dy1) && (0.f == dy2) && (0.f == fabs(line.ptStart.y - ptr->element.y)))
	{
		const double sign1 = (ptr->element.x - line.ptStart.x ) * (m_pNode->element.x - line.ptStart.x);
		const double sign2 = (ptr->element.x - line.ptEnd.x ) * (m_pNode->element.x - line.ptEnd.x);
		if((sign1 <= 0.f) && (sign2 <= 0.f)) return nIndex;
	}
	if((0.f == dx1) && (0.f == dx2) && (0.f == fabs(line.ptStart.x - ptr->element.x)))
	{
		const double sign1 = (ptr->element.y - line.ptStart.y ) * (m_pNode->element.y - line.ptStart.y);
		const double sign2 = (ptr->element.y - line.ptEnd.y ) * (m_pNode->element.y - line.ptEnd.y);
		if((sign1 <= 0.f) && (sign2 <= 0.f)) return nIndex;
	}
	
	return 0;
}

/**	
	@brief	grid의 각 edge와 line과의 교차를 검사하고 교차점을 리턴한다.
		교차점이 edge의 양 끝점이거나 line의 양 끝점일 경우에는 교차하지 않는걸로 간주한다.

	@author	백흠경

	@param	pt	a parameter of type POINT_T&
	@param	line	a parameter of type const LINE_T&

	@return	bool	
*/
bool CGrid::DoIntersection(POINT_T& pt,const LINE_T& line)
{
	LINE_T _line={0,};
	
	snode<POINT_T>* ptr=NULL;
	for(ptr=m_pNode;ptr && ptr->next;ptr=ptr->next)
	{
		_line.ptStart.x = ptr->element.x;
		_line.ptStart.y = ptr->element.y;
		_line.ptEnd.x = ptr->next->element.x;
		_line.ptEnd.y = ptr->next->element.y;
		if(DO_INTERSECTION == CGeometry::IntersectLine2D(pt,_line,line))
		{
			pt.x = _round(pt.x,MAX_OPENSOLID_PRECISION);
			pt.y = _round(pt.y,MAX_OPENSOLID_PRECISION);
			pt.z = _round(pt.z,MAX_OPENSOLID_PRECISION);
			if(	(IsSamePoint(pt,_line.ptStart) || IsSamePoint(pt,_line.ptEnd)) || 
				(IsSamePoint(pt,line.ptStart)  || IsSamePoint(pt,line.ptEnd))) continue;
			else	return true;
		}
	}

	_line.ptStart.x = ptr->element.x;
	_line.ptStart.y = ptr->element.y;
	_line.ptEnd.x   = m_pNode->element.x;
	_line.ptEnd.y   = m_pNode->element.y;
	if(DO_INTERSECTION == CGeometry::IntersectLine2D(pt,_line,line))
	{
		pt.x = _round(pt.x,MAX_OPENSOLID_PRECISION);
		pt.y = _round(pt.y,MAX_OPENSOLID_PRECISION);
		pt.z = _round(pt.z,MAX_OPENSOLID_PRECISION);
		if(	(IsSamePoint(pt,_line.ptStart) || IsSamePoint(pt,_line.ptEnd)) || 
			(IsSamePoint(pt,line.ptStart)  || IsSamePoint(pt,line.ptEnd))) return false;
		else	return true;
	}

	return false;
}

/**	
	@brief	line is a diagonal line.

	@author	BHK

	@date	?

	@param	line	a parameter of type const LINE_T&

	@return	CGrid*	
*/
CGrid* CGrid::Split(const LINE_T& line)
{
	POINT_T ptLeft={0},ptRight={0};
	
	snode<POINT_T>* ptr=NULL;
	for(ptr=m_pNode;ptr;ptr=ptr->next)
	{
		if(IsSamePoint(ptr->element,line.ptStart) || IsSamePoint(ptr->element,line.ptEnd)) continue;
		if(CGeometry::IsLeftSidePoint(ptr->element,line))
		{
			ptLeft = ptr->element;
		}
		else if(CGeometry::IsRightSidePoint(ptr->element,line))
		{
			ptRight = ptr->element;
		}
	}
	
	ptr = m_pNode;
	ptr->element      = line.ptStart;
	ptr->next->element= line.ptEnd;
	ptr->next->next->element = ptLeft;
	ptr->next->next->next    = NULL;
	
	CGrid* pRet=NULL;
	pRet = new CGrid;
	if(pRet)
	{
		snode<POINT_T>* pNode=NULL;
		pNode = (snode<POINT_T>*)calloc(1,sizeof(snode<POINT_T>));
		pNode->element = line.ptEnd;
		pNode->next = (snode<POINT_T>*)calloc(1,sizeof(snode<POINT_T>));
		pNode->next->element = line.ptStart;
		pNode->next->next = (snode<POINT_T>*)calloc(1,sizeof(snode<POINT_T>));
		pNode->next->next->element = ptRight;
		pRet->m_pNode = pNode;
		pRet->CalcRect();
	}
	
	return pRet;
}

/**
	@brief	GRID의 영역(rect)를 계산한다.
	@author	백흠경
*/
void CGrid::CalcRect()
{
	m_rect.x = m_rect.y = 0;
	m_rect.width = m_rect.height = -1;
	if(m_pNode)
	{
		
		_node* ptr = m_pNode;
		double minx = ptr->element.x, miny = ptr->element.y;
		double maxx = ptr->element.x, maxy = ptr->element.y;

		for(ptr = ptr->next;ptr;ptr = ptr->next)
		{
			if(ptr->element.x < minx) minx = ptr->element.x;
			if(ptr->element.x > maxx) maxx = ptr->element.x;
			if(ptr->element.y < miny) miny = ptr->element.y;
			if(ptr->element.y > maxy) maxy = ptr->element.y;
		}

		m_rect.x = minx;
		m_rect.y = miny;
		m_rect.width = maxx - minx;
		m_rect.height= maxy - miny;
	}
}

/**
	@brief	GRID의 영역을 리턴한다.
	@author	백흠경

	@return	RECT_T
*/
RECT_T CGrid::rect() const
{
	return m_rect;
}
