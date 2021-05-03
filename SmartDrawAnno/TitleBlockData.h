#pragma once

#include "SmartDrawAnnoTextStyle.h"
#include "aDraw_EntTagShape.h"

#include <map>
using namespace std;

class ON_TAG_EXT_DLL CTitleBlockData
{
	typedef struct tagTitleBlockData
	{
		CIsPoint2d ptLoc;
		CSmartDrawAnnoTextStyle oTextStyle;
		STRING_T sJustify;
		double dRotation;
		CompProp oProp;
		STRING_T sValue;
	}TitleBlockData;

	CTitleBlockData( const CTitleBlockData& rhs );
public:
	CTitleBlockData(void);
	~CTitleBlockData(void);
public:
	bool Load(LPCTSTR pIniFilePath , LPCTSTR pSdrFileName);
	bool Write(OFSTREAM_T& ofile,const double& dDrawingScale);
private:
	bool LoadSetting(const STRING_T& sIniFilePath , const STRING_T& sCfgFilePath);

	map<STRING_T , TitleBlockData>* m_pDataValueMap;
};
