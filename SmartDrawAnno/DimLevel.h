#pragma once

#include "SmartDrawAnnoPos.H"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dimension.h"

#include <vector>
using namespace std;

class CExtDivision;
class CDimLevel  
{
public:
	CDimLevel(const DIM_LEVEL_E& nLevel);
	virtual ~CDimLevel();
public:
	int SetLevel(const DIM_LEVEL_E&);
	DIM_LEVEL_E GetLevel() const;
	void SetBaseLine(const CIsLine3d& lnBaseLine);
	void SetLowerLine(const CIsLine3d& lnLowerLine);
	bool operator+=(CaDraw_TagPos* pTagPos);
	const long GetItemCount();
	virtual void Run(CExtDivision* pDivision);

	CIsLine3d GetBaseLine(){return m_lnBase;}
	vector<CaDraw_TagPos*>* GetDITEMVector(){return m_pvtTagPos;}
private:
	void StoreDITEM(CExtDivision* pDivision);
protected:
	DIM_LEVEL_E m_nLevel;
	CIsLine3d m_lnBase,m_lnLower;
	vector<CaDraw_TagPos*>* m_pvtTagPos;
};
