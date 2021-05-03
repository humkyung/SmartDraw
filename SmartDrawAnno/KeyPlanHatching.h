#pragma once

#include "SmartDrawAnnoView.h"
#include "SmartDrawAnnoTextStyle.h"

#include <vector>
using namespace std;
class CKeyPlanHatching
{
	CKeyPlanHatching(const CKeyPlanHatching&){}
	CKeyPlanHatching& operator=(const CKeyPlanHatching&){return (*this);}
public:
	CKeyPlanHatching(void);
	~CKeyPlanHatching(void);

	int LoadIni(LPCTSTR pszIniFilePath);
	int WriteHatching(OFSTREAM_T& ofile,CSmartDrawAnnoView*,const double& dDrawingScale);
	int WriteThisDrawing(OFSTREAM_T& ofile,const double& dDrawingScale) const;
private:
	CIsPoint2d m_ptModelPosition , m_ptKeyPlanPosition;
	double m_dXScale,m_dYScale;
	STRING_T m_sThisDrawingText;
	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oTextProp;
	int m_iAngle;
	CIsPoint2d m_ptTextPosition;
	vector<CIsPoint2d> m_oHatchingPntList;
};
