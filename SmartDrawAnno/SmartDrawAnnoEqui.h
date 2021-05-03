#pragma once
#include <aDraw_Equipment.h>
#include "SmartDrawAnnoentity.h"

#include <list>
using namespace std;

class CaDraw_TagNozzle;
class ON_TAG_EXT_DLL CaDraw_TagEqui : public CaDraw_TagEntity
{
public:
	/// euipment type
	enum
	{
		VERTICAL_VESSEL		= 1,
		HORIZONTAL_VESSEL	= 2,
		PUMP				= 3
	};

	class HATCHING_ENV
	{
		HATCHING_ENV(void);
		HATCHING_ENV(const HATCHING_ENV&);
		HATCHING_ENV& operator=(const HATCHING_ENV&);
	public:
		static HATCHING_ENV& GetInstance();
		int Parse(const STRING_T& sIniFilePath);
		
		~HATCHING_ENV(void);
	public:
		STRING_T m_sCellLibFilePath , m_sCellName;
		int m_iHatchingOffset , m_iHatchingScale;
		CompProp m_oHatchingProp;
	};

	CaDraw_Equipment* equi() const
	{
		return static_cast<CaDraw_Equipment*>(m_pDrawEntity);
	}

	CaDraw_TagEqui(CaDraw_Entity* unnamed);
	~CaDraw_TagEqui(void);

	bool HasNozzle() const;
	int GetTagNozzleList( list<CaDraw_TagNozzle*>& oTagNozzleList );	/// 2012.03.26 added by humkyung
	int AddTagNozzle( CaDraw_TagNozzle* pTagNozzle );					/// 2012.03.26 added by humkyung

	int Annotate();
	int FillSectionMark();						/// 2014.04.19 added by humkyung
	int WriteSectionMark(OFSTREAM_T& ofile,const double& dDrawingScale);	/// 2014.04.18 added by humkyung
	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	void GetTagNo(STRING_T* pTagNo1 , STRING_T* pTagNo2);

	SMARTDRAW_TAG_DECLARE_FUNC()

	TCHAR m_szGridNo[2][16];	/// 2012.03.27 added by humkyung
private:
	void GetCoordinate(STRING_T *pTagNo2);
	void GetDesc(STRING_T *pTagNo2);

	list<CaDraw_TagNozzle*>* m_pTagNozzleList;	/// 2012.03.26 added by humkyung
};