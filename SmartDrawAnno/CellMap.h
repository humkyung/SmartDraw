// CellMap.h: interface for the CCellMap class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IsTools.h>
#include <IsEntity.h>
#include <IsPoint3d.h>
#include <IsRect.h>
#include <IsVect2d.h>

//#define	SEARCH_HORIZONTAL	1
//#define	SEARCH_VERTICAL		2
//#define	SEARCH_AROUND		3

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL CCellMap  
{
public:
	typedef enum VALUE
	{
		EMPTY = 0,
		WHITE = 1,
		RED   = 2,
		GREEN = 3,
		BLUE  = 4,
		TAG = 155,
		OUT_OF_INDEX = 0xff
	};

	typedef struct
	{
		int width;
		int height;
	}SIZE_T;

	typedef unsigned char CellType;
	class ON_TAG_EXT_DLL CellIndex
	{
	public:
		CellIndex();
		CellIndex(const long& _x , const long& _y);
		CellIndex(const CellIndex& rhs)
		{
			(*this) = rhs;
		}
		
		CellIndex& operator=(const CellIndex& rhs)
		{
			if(this != &rhs)
			{
				x = rhs.x;
				y = rhs.y;
			}
			
			return (*this);
		}
		
		long x;
		long y;
	};

	class ON_TAG_EXT_DLL CellConversionAxis
	{
	public:
		CellConversionAxis()
		{
			xAxis.Set(1.f , 0.f);
			yAxis.Set(0.f , 1.f);
		}

		CellConversionAxis(const CellConversionAxis& rhs)
		{
			(*this) = rhs;
		}

		CellConversionAxis& operator=(const CellConversionAxis& rhs)
		{
			if(this != &rhs)
			{
				xAxis = rhs.xAxis;
				yAxis = rhs.yAxis;
			}

			return (*this);
		}

		CellIndex operator()(const long& dx , const long& dy) const
		{
			CellIndex cIndex;
			CIsPoint2d res = xAxis * dx + yAxis * dy;
			cIndex.x = int(SAFE_ROUND(res.x() , 0));
			cIndex.y = int(SAFE_ROUND(res.y() , 0));

			return cIndex;
		}
		CIsVect2d xAxis , yAxis;
	};

	class ON_TAG_EXT_DLL SearchRes
	{
	public:
		SearchRes();
		SearchRes(const SearchRes& rhs);
		~SearchRes();
		SearchRes& operator=(const SearchRes& rhs);
		

		TCHAR szString[256];

		CellIndex  cIndex;
		CIsPoint2d ptLoc , ptTagOrigin;
		size_t     occupied;
		double     distance;
		double     radian;

		CIsPoint2d coords[4];
		CIsPoint2d leaders[16];	/// 2012.03.23 added by humkyung
		CellConversionAxis cAxis;
	};

	typedef struct
	{
		CellIndex min;
		CellIndex max;

		CellIndex minPoint() const { return min; }
		CellIndex maxPoint() const { return max; }
	}CellExtents;

	typedef enum SEARCH_AREA
	{
		OVER_AREA	= 0x01,
		INSIDE_AREA	= 0x02
	};

	bool FindBestLoc(CCellMap::SearchRes& res , CCellMap::CellIndex& cOrigin , const CCellMap::CellIndex& tagOrigin , 
		const CCellMap::SIZE_T& sizSearch, const CCellMap::SIZE_T& sizItem, 
		   CCellMap::CellConversionAxis& cAxis, const int& nMinOccupiedWeight ,
		   double (*fnDistanceTo)(CCellMap::SearchRes* , const CCellMap::SIZE_T& , const CCellMap::CellIndex& )=NULL);
	int  Resize(const CIsPoint2d& origin , const double &width , const double &height , const double& scale);
	void Reset(const CellType& value);
	
	int GetOccupiedCountOf(const CCellMap::CellIndex &start, const CCellMap::CellIndex &end);	/// 2012.03.09 added by humkung
	void UpdateLineOf(const CellIndex& start , const CellIndex& end , const char& value);
	int UpdateLineOf(const CIsPoint2d& start , const CIsPoint2d& end , const CellType& value);
	int UpdateRectOf(const CIsPoint2d &min, const CIsPoint2d &max , const char &value);
	int UpdateRectOf(const CellIndex& origin , const long& xlen , const long& ylen , const CellConversionAxis& cAxis , const char &value);
	int FillCircleOf(const CellIndex& origin , const long& lRadius , const TCHAR &value);	/// fille circle by using given value - 2014.04.13 added by humkyung
	int Set(const int & x , const int & y , const CellType& value);
	int Set(const CellIndex& cIndex , const CellType& value);
	CellType Value(const int & x , const int& y) const;
	CIsPoint2d IndexToPoint(const CellIndex &index);
	CellIndex PointToIndex(const CIsPoint2d &pt);
	CCellMap(const CIsPoint2d& origin , const double &width , const double & height , const double& scale);
	virtual ~CCellMap();

	static bool fnResSort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs);	/// 2012.03.20 added by humkyung

	void DumpToBMPFile(const STRING_T& rBmpFilePath);
private:
	CIsPoint2d m_ptOrigin;
	double m_width;
	double m_height;
	double m_scale;
	
	size_t m_xlen;
	size_t m_ylen;
	CellType* m_pCell;
public:
	const double scale() const;
	double width(void) const;
	double height(void) const;
	size_t xlen(void) const;
	size_t ylen(void) const;
};
