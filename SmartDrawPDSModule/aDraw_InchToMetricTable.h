#pragma once

#include <list>
using namespace std;

#ifdef ADRAW_PDSMODULE_EXPORTS
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllexport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern
#else
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllimport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_PDSMODULE_EXT_DLL CaDraw_InchToMetricTable
{
	CaDraw_InchToMetricTable(void);
public:
	typedef struct tagRangeValue
	{
		double more_equal_than , less_than;
		double value;
	}RangeValue;

	static CaDraw_InchToMetricTable& GetInstance(void);
	~CaDraw_InchToMetricTable(void);
private:
	list<RangeValue*> m_RangeValueList;
public:
	CaDraw_InchToMetricTable::RangeValue* Find(const double& nInch);
};
