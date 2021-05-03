#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

namespace tag_option
{
class ON_TAG_EXT_DLL COutsideOption  
{
public:
	COutsideOption();
	virtual ~COutsideOption();
public:
	const double PipeEqpDist() const;
	const double InsidePipeEqpDist() const;
	const double ElevDist() const;
	const double InsideElevDist() const;

	double GetDistToDimLine(const int& eDimLevel) const;
	double GetOverlineDist();
	bool DistCIsZero();
	bool DistBIsZero();
	void Parse(const STRING_T& pszFilePath);
	
	double m_nDistA;
	double m_nDistB;
	double m_nDistC;
	double m_nDistD;
	double m_nDistE;
	double m_nOverlineDist;
	double m_nElevStartDist;
	double m_nElevOffset;
private:
	double m_dPipeEqpDist , m_dInsidePipeEqpDist;
	double m_dElevEndDist;
};
};