// DraftCellMap.cpp: implementation of the CCellMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <malloc.h>
#include <inc\ip.h>
#include <inc\img.h>
#include "CellMap.h"

#include <exception>
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

CCellMap::SearchRes::SearchRes()
{
	radian = 0.0;
	occupied = 0;
	memset(szString , '\0' , 256);
}

CCellMap::SearchRes::SearchRes(const CCellMap::SearchRes& rhs)
{
	radian = 0.0;
	occupied = 0;
	memset(szString , '\0' , 256);

	(*this) = rhs;
}

CCellMap::SearchRes::~SearchRes()
{
	try
	{
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-23
    @class      SearchRes
    @function   operator=
    @return     SearchRes&
    @param      const SearchRes& rhs
    @brief
******************************************************************************/
CCellMap::SearchRes& CCellMap::SearchRes::operator=(const CCellMap::SearchRes& rhs)
{
	if(this != &rhs)
	{
		STRNCPY_T(szString , rhs.szString , 255);

		cIndex = rhs.cIndex;
		ptLoc = rhs.ptLoc;
		ptTagOrigin = rhs.ptTagOrigin;
		occupied = rhs.occupied;
		distance = rhs.distance;
		radian = rhs.radian;

		coords[0] = rhs.coords[0];
		coords[1] = rhs.coords[1];
		coords[2] = rhs.coords[2];
		coords[3] = rhs.coords[3];
		cAxis = rhs.cAxis;

		for(int i = 0;i < 16;++i)
		{
			leaders[i] = rhs.leaders[i];
		}
	}

	return (*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCellMap::CellIndex::CellIndex( )
{
	x = y = 0;
}

CCellMap::CellIndex::CellIndex(const long& _x , const long& _y )
{
	x = _x;
	y = _y;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCellMap::CCellMap(const CIsPoint2d& origin , const double &width , const double & height , const double& scale)
 : m_ptOrigin(origin) , m_width(width) , m_height(height) , m_scale(scale)
{
	m_xlen = static_cast<size_t>(m_width * m_scale) + 1;
	m_ylen = static_cast<size_t>(m_height* m_scale) + 1;

	m_pCell = (CCellMap::CellType*)calloc(1 , m_xlen * m_ylen);
}

CCellMap::~CCellMap()
{
	try
	{
		if(NULL != m_pCell)
		{
			free((void*)m_pCell);
			m_pCell = NULL;
		}
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06

	@return	
*/
CCellMap::CellIndex CCellMap::PointToIndex(const CIsPoint2d &pt)
{
	CellIndex res;
    
	const double x = (pt.x() - m_ptOrigin.x()) * m_scale + m_xlen * 0.5;
	const double y = (pt.y() - m_ptOrigin.y()) * m_scale + m_ylen * 0.5;

	res.x = SAFE_ROUND(x,0);
	res.y = SAFE_ROUND(y,0);

	if(res.x < 0) res.x = 0;
	if(res.y < 0) res.y = 0;
	if(res.x >= int(m_xlen)) res.x = m_xlen - 1;
	if(res.y >= int(m_ylen)) res.y = m_ylen - 1;
	
	return res;
}

/**	
	@brief	cell index를 POINT로 변환한다.

	@author	HumKyung.BAEK

	@date	2010.06.06
**/
CIsPoint2d CCellMap::IndexToPoint(const CellIndex &index)
{    
	const double x = (index.x - m_xlen * 0.5) / m_scale + m_ptOrigin.x();
	const double y = (index.y - m_ylen * 0.5) / m_scale + m_ptOrigin.y();

	return CIsPoint2d(x , y);
}


/**
	@brief	x,y에 위치한 CELL의 점유 상태를 리턴한다.

	@author	humkyung

	@return	!0 = 해당 위치의 CELL이 점유 됨 , 0 = 해당 위치의 CELL이 점유 되지 않음. -1 = range error
*/
CCellMap::CellType CCellMap::Value(const int &x, const int &y) const
{
	if((x >= 0) && (x < int(m_xlen)) && (y >= 0) && (y < int(m_ylen)))
	{
		unsigned long index = (y * m_xlen) + x;
		return m_pCell[index];
	}

	return -1;
}

/**
	@brief	x,y에 위치한 CELL의 점유 상태를 설정한다.

	@author	humkyung
  
	@return	
*/
int CCellMap::Set(const int &x, const int &y , const CCellMap::CellType& value)
{
	if((x >= 0) && (x < int(m_xlen)) && (y >=0) && (y < int(m_ylen)))
	{
		unsigned long index = (y * m_xlen) + x;
		(m_pCell[index]) = value;

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	x,y에 위치한 CELL의 점유 상태를 설정한다.

	@author	humkyung
  
	@return	
*/
int CCellMap::Set(const CellIndex& cIndex , const CCellMap::CellType& value)
{
	return this->Set(cIndex.x , cIndex.y , value);
}

/**	
	@brief	양 끝점으로 이루어지는 line으로 cell map을 갱신한다.

	@author	백흠경

	@param	start	a parameter of type const CellIndex &
	@param	end	a parameter of type const CellIndex &
	@param	on	a parameter of type const bool &

	@return	void	
*/
void CCellMap::UpdateLineOf(const CCellMap::CellIndex &start, const CCellMap::CellIndex &end, const char &value)
{
	//int nr, liconst;      // remainder, loop-invariant constant 
	
	int deltax = end.x - start.x;
	int deltay = end.y - start.y;
	
	int i = 0 , j = 0;
	if(start.x == end.x)
	{
		if(start.y < end.y)
		{
			for(int y = start.y; y <= end.y;++y)
			{
				this->Set(start.x , y , value);
			}
		}
		else
		{
			for(int y = end.y; y <= start.y;++y)
			{
				this->Set(start.x , y , value);
			}
		}
	}
	else if(start.y == end.y)
	{
		if(start.x < end.x)
		{
			for(int x = start.x; x <= end.x;x++)
			{
				this->Set(x , start.y , value);
			}
		}
		else
		{
			for(int x = end.x; x <= start.x;x++)
			{
				this->Set(x , start.y , value);
			}
		}
	}
	else
	{
		size_t adx = abs(deltax);
		size_t ady = abs(deltay);
		if(adx > ady)
		{
			CellIndex NewStart(start) , NewEnd(end);
			if(start.x > end.x) swap(NewStart , NewEnd);
			const double ratio = ((double)NewEnd.y - (double)NewStart.y) / ((double)NewEnd.x - (double)NewStart.x);
			{
				for(int x = 0; x <= int(adx);++x)
				{
					this->Set(NewStart.x + x , NewStart.y + (size_t)(ratio * x) , value);
				}
			}
		}
		else
		{
			CellIndex NewStart(start) , NewEnd(end);
			if(start.y > end.y) swap(NewStart , NewEnd);
			const double ratio = ((double)NewEnd.x - (double)NewStart.x) / ((double)NewEnd.y - (double)NewStart.y);
			{
				for(int y = 0; y <= int(ady);++y)
				{
					this->Set(NewStart.x + (size_t)(ratio * y) , NewStart.y + y , value);
				}
			}
		}
	}
}

/**	
	@brief	양 끝점으로 이루어지는 rectangle으로 cell map을 갱신한다.

	@author	백흠경

	@param	start	a parameter of type const CellIndex &
	@param	end	a parameter of type const CellIndex &
	@param	on	a parameter of type const bool &

	@return	void	
*/
int CCellMap::UpdateLineOf(const CIsPoint2d &start, const CIsPoint2d &end, const CellType &value)
{
	CellIndex _start = PointToIndex(start);
	CellIndex _end   = PointToIndex(end);

	if ((_start.x == _end.x) && (_start.y == _end.y))
	{
		this->Set(_start.x , _start.y , value);
	}
	else 
	{
		this->Set(_start.x , _start.y , value);
		this->Set(_end.x , _end.y , value);
		UpdateLineOf(_start, _end, value); 
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06

*/
int CCellMap::UpdateRectOf(const CIsPoint2d& min , const CIsPoint2d& max , const char &value)
{
	CIsPoint2d pt(min(min.x() , max.x()) , min(min.y() , max.y()));
	CellIndex index = PointToIndex(pt);

	const double width = fabs(max.x() - min.x());
	const double height= fabs(max.y() - min.y());

	size_t x1 = index.x;
	size_t y1 = index.y;
	size_t x2 = x1 + (size_t)(width * m_scale);
	size_t y2 = y1 + (size_t)(height* m_scale);
	if(x2 >= m_xlen) x2 = m_xlen - 1;
	if(y2 >= m_ylen) y2 = m_ylen - 1;

	for(int j = y1;j <= int(y2);++j)
	{
		for(int i = x1;i <= int(x2);++i)
		{
			this->Set(i , j , value);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.08
*/
int CCellMap::UpdateRectOf(const CellIndex& origin , const long& xlen , const long& ylen , const CellConversionAxis& cAxis , const char &value)
{
	for(int j = 0;j <= ylen;++j)
	{
		CellIndex start = cAxis(0 , j);
		CellIndex end   = cAxis(xlen , j);
		start.x += origin.x; start.y += origin.y;
		end.x += origin.x; end.y += origin.y;
		this->UpdateLineOf(start , end , value);
	}

	/*for(int i = 0;i <= xlen;++i)
	{
		for(int j = 0;j <= ylen;++j)
		{
			CellIndex cIndex = cAxis(i,j);
			cIndex.x += origin.x;
			cIndex.y += origin.y;
			Set(cIndex , value);
		}
	}*/

	return ERROR_SUCCESS;
}

/**
	@brief	fill circle
	@author	humkyung
	@date	2014.04.13
*/
int CCellMap::FillCircleOf(const CellIndex& origin , const long& lRadius , const TCHAR &value)
{
	for(int j = 0;j <= lRadius;++j)
	{
		CellIndex start , end;
		int i = int(sqrt((double)lRadius*(double)lRadius - (double)j*(double)j));
		start.x = origin.x - i; start.y = origin.y + j;
		end.x   = origin.x + i; end.y   = origin.y + j;
		this->UpdateLineOf(start , end , value);

		start.y = origin.y - j;
		end.y = origin.y - j;
		this->UpdateLineOf(start , end , value);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class      CCellMap
    @function   GetOccupiedCountOf
    @return     int
    @param      const CCellMap::CellIndex &start
	@param		const CCellMap::CellIndex &end
    @brief		return occiped cell count from start to end
******************************************************************************/
int CCellMap::GetOccupiedCountOf(const CCellMap::CellIndex &start, const CCellMap::CellIndex &end)
{
	int res = 0;

	const int dx = end.x - start.x;
	const int dy = end.y - start.y;
	const int len= int(ceil(sqrt(double(dx*dx + dy*dy))));
	
	int i = 0 , j = 0;
	if(start.x == end.x)
	{
		if(start.y < end.y)
		{
			for(int y = start.y; y <= end.y;++y)
			{
				const CCellMap::CellType ch = this->Value(start.x , y);
				(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
			}
		}
		else
		{
			for(int y = end.y; y <= start.y;++y)
			{
				const CCellMap::CellType ch = this->Value(start.x , y);
				(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
			}
		}
	}
	else if(start.y == end.y)
	{
		if(start.x < end.x)
		{
			for(int x = start.x; x <= end.x;x++)
			{
				const CCellMap::CellType ch = this->Value(x , start.y);
				(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
			}
		}
		else
		{
			for(int x = end.x; x <= start.x;x++)
			{
				const CCellMap::CellType ch = this->Value(x , start.y);
				(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
			}
		}
	}
	else
	{
		size_t adx = abs(dx);
		size_t ady = abs(dy);
		if(adx > ady)
		{
			CellIndex NewStart(start) , NewEnd(end);
			if(start.x > end.x) swap(NewStart , NewEnd);
			const double ratio = ((double)NewEnd.y - (double)NewStart.y) / ((double)NewEnd.x - (double)NewStart.x);
			{
				for(int x = 0; x <= int(adx);++x)
				{
					const CCellMap::CellType ch = this->Value(NewStart.x + x , NewStart.y + (int)(ratio * x));
					(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
				}
			}
		}
		else
		{
			CellIndex NewStart(start) , NewEnd(end);
			if(start.y > end.y) swap(NewStart , NewEnd);
			const double ratio = ((double)NewEnd.x - (double)NewStart.x) / ((double)NewEnd.y - (double)NewStart.y);
			{
				for(int y = 0; y <= int(ady);++y)
				{
					const CCellMap::CellType ch = this->Value(NewStart.x + (int)(ratio * y) , NewStart.y + y);
					(CCellMap::OUT_OF_INDEX == ch) ? (res += len*2) : ((CCellMap::EMPTY != ch) ? ((CCellMap::TAG == ch) ? res += 4 : ++res) : res);
				}
			}
		}
	}

	return res;
}

/**
	@brief	cell map을 초기화한다.

	@author	백흠경
*/
void CCellMap::Reset(const CCellMap::CellType& value)
{
	if(m_pCell) memset(m_pCell , value , m_xlen * m_ylen);
}

/**
	@brief	cell map의 크기를 조절한다.

	@author	백흠경
*/
int CCellMap::Resize(const CIsPoint2d& origin , const double &width , const double &height , const double& scale)
{
	m_ptOrigin = origin;
	m_width  = width;
	m_height = height;
	m_scale  = scale;

	m_xlen = static_cast<size_t>(m_width * m_scale) + 1;
	m_ylen = static_cast<size_t>(m_height* m_scale) + 1;

	m_pCell = (CCellMap::CellType*)realloc(m_pCell , m_xlen * m_ylen);
	if(m_pCell)
	{
		memset(m_pCell , CCellMap::EMPTY , m_xlen * m_ylen);
		return ERROR_SUCCESS;
	}

	return ERROR_NOT_ENOUGH_MEMORY;
}

//////////////////////////////////////////////////////////////////////////
typedef struct tagSearchLoc
{
	size_t cnt;
	double length;
	int left , low;
}SearchLoc;

bool CCellMap::fnResSort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs)
{
	/// occupied에 비중을 더 준다.
	const double lhsValue = ((1.0 + lhs.occupied) * 2.0) + (1.0 + lhs.distance);
	const double rhsValue = ((1.0 + rhs.occupied) * 2.0) + (1.0 + rhs.distance);

	return (lhsValue < rhsValue);
}

double fnDefaultDistanceTo(CCellMap::SearchRes* pSearchRes , const CCellMap::SIZE_T& sizItem , const CCellMap::CellIndex& tagOrigin)
{
	assert(pSearchRes && "pSearchRes is NULL");
	double res = DBL_MAX;

	if(pSearchRes)
	{
		CCellMap::CellIndex cIndex = pSearchRes->cAxis(sizItem.width * 0.5 , sizItem.height * 0.5);
		CIsPoint2d at(pSearchRes->cIndex.x + cIndex.x , pSearchRes->cIndex.y + cIndex.y);
		res = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));
	}

	return res;
}

/**	
	@brief	find best location for item

	@author	humkyung

	@date	2010.06.09

	@param

	@return	bool	
*/
bool CCellMap::FindBestLoc(CCellMap::SearchRes& res , CCellMap::CellIndex& cOrigin , const CCellMap::CellIndex& tagOrigin , 
			   const CCellMap::SIZE_T& sizSearch , const CCellMap::SIZE_T& sizItem , 
			   CCellMap::CellConversionAxis& cAxis, const int& nMinOccupiedWeight , 
			   double (*fnDistanceTo)(CCellMap::SearchRes* , const CCellMap::SIZE_T& , const CCellMap::CellIndex& ))
{
	const double TOLER = 0.001;

	list<CCellMap::SearchRes> SearchResList;

	if((cAxis.xAxis.Length() < TOLER) || (cAxis.yAxis.Length() < TOLER)) return false;
	
	for(int j = 0;(j <= sizSearch.height - sizItem.height)/* && (cOrigin.y + j <= m_ylen - nItemHeight)*/ ;++j)
	{
		vector<int> oColumnOverlapList;
		for(int i = 0;(i <= sizSearch.width - sizItem.width)/* && (cOrigin.x + i <= m_xlen - nItemLength)*/; ++i)
		{
			/// get occupied cell count within item area
			int ii = 0 , jj = 0;
			if(0 == i)
			{
				for (ii = i;ii <= (i + sizItem.width); ++ii)
				{
					CCellMap::CellIndex start , end;
					start = cAxis(ii , j);
					start.x += cOrigin.x; start.y += cOrigin.y;
					end = cAxis(ii , j + sizItem.height);
					end.x += cOrigin.x; end.y += cOrigin.y;

					const int res = this->GetOccupiedCountOf(start , end);
					oColumnOverlapList.push_back(res);
				}
			}
			else
			{
				if(!oColumnOverlapList.empty()) oColumnOverlapList.erase(oColumnOverlapList.begin());
				ii = (i + sizItem.width);
				{
					CCellMap::CellIndex start , end;
					start = cAxis(ii , j);
					start.x += cOrigin.x; start.y += cOrigin.y;
					end = cAxis(ii , j + sizItem.height);
					end.x += cOrigin.x; end.y += cOrigin.y;

					const int res = this->GetOccupiedCountOf(start , end);
					oColumnOverlapList.push_back(res);
				}
			}
			/// up to here

			int nOverlapCount = 0;
			for(vector<int>::iterator itr = oColumnOverlapList.begin();itr != oColumnOverlapList.end();++itr)
			{
				nOverlapCount += (*itr);
			}
			if(nOverlapCount < nMinOccupiedWeight)
			{
				CCellMap::SearchRes tmpRes(res);

				tmpRes.cIndex    = cAxis(i,j);
				tmpRes.cIndex.x += cOrigin.x;
				tmpRes.cIndex.y += cOrigin.y;
				tmpRes.ptLoc = IndexToPoint(tmpRes.cIndex);
				tmpRes.occupied = nOverlapCount;
				CCellMap::CellIndex cIndex = cAxis(sizItem.width * 0.5 , sizItem.height * 0.5);
				
				if(NULL == fnDistanceTo)
				{
					tmpRes.distance = fnDefaultDistanceTo(&tmpRes , sizItem , tagOrigin);
				}
				else
				{
					tmpRes.distance = fnDistanceTo(&tmpRes , sizItem , tagOrigin);
				}
				///CIsPoint2d temp(tmpRes.cIndex.x + cIndex.x , tmpRes.cIndex.y + cIndex.y);
				///tmpRes.distance = temp.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));
				
				tmpRes.radian = cAxis.xAxis.angle();
				SearchResList.push_back(tmpRes);
			}
		}
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);  /// result을 sort한다.
		res = SearchResList.front();
		res.cAxis = cAxis;

		return true;
	}
	
	return false;
}

/**
	@brief	cell map의 width를 리턴한다.

	@author	백흠경
**/
double CCellMap::width(void) const
{
	return m_width;
}

/**
	@brief	cell map의 height를 리턴한다.

	@author	백흠경
*/
double CCellMap::height(void) const
{
	return m_height;
}

/**
	@brief	cell map의 xlen를 리턴한다.

	@author	백흠경

	@date	2010.06.07
*/
size_t CCellMap::xlen(void) const
{
	return m_xlen;
}

/**
	@brief	cell map의 ylen를 리턴한다.

	@author	백흠경

	@date	2010.06.07
*/
size_t CCellMap::ylen(void) const
{
	return m_ylen;
}

/**
	@brief	

	@author	백흠경

	@date	2010.06.07
*/
const double CCellMap::scale() const
{
	return m_scale;
}

rgb_quad palette[256];

/******************************************************************************
    @author     humkyung
    @date       2012-02-28
    @class      CCellMap
    @function   DumpToBMPFile
    @return     void
    @param      const STRING_T& rBmpFilePath
    @brief		dump cell data to 256 color bmp file
******************************************************************************/
void CCellMap::DumpToBMPFile(const STRING_T& rBmpFilePath)
{
	const int xlen = this->xlen();
	const int ylen = this->ylen();

	image_type* img = new_image(xlen, ylen);
	image_type* mirror = new_image(xlen , ylen);
	for(int i = 0;i < 256;++i)
	{
		palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = i;
		palette[i].rgbReserved = 0;
		if(CCellMap::WHITE == i)
		{
			palette[i].rgbRed = 255;
			palette[i].rgbGreen = palette[i].rgbBlue = 255;
		}
		else if(CCellMap::RED == i)
		{
			palette[i].rgbRed = 255;
			palette[i].rgbGreen = palette[i].rgbBlue = 0;
		}
		else if(CCellMap::GREEN == i)
		{
			palette[i].rgbGreen = 255;
			palette[i].rgbRed = palette[i].rgbBlue = 0;
		}
		else if(CCellMap::BLUE == i)
		{
			palette[i].rgbBlue = 255;
			palette[i].rgbRed = palette[i].rgbGreen = 0;
		}
		else
		{
			palette[i].rgbBlue = i;
			palette[i].rgbRed = palette[i].rgbGreen = i;
		}
	}

	for(int j = 0;j < int(img->height) ;++j)
	{
		for(int i = 0;i < int(img->width);++i)
		{
			const char value = this->Value(i,j);
			img->data[j*img->width + i] = value;
		}
	}
	y_mirror(img , mirror);

	write_256bmp_file((char*)(rBmpFilePath.c_str()) , mirror , palette);
	free_image(img);
	free_image(mirror);
}