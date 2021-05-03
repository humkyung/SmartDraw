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


class ON_PDSMODULE_EXT_DLL CaDraw_PipeDiaTable
{
	CaDraw_PipeDiaTable(void);
public:
	typedef struct ON_PDSMODULE_EXT_DLL tagRangeValue
	{
		double more_equal_than , less_than;
		double value;
	}RangeValue;

	static CaDraw_PipeDiaTable& GetInstance(void);
	~CaDraw_PipeDiaTable(void);

	typedef list<RangeValue*>::const_iterator iterator;
	iterator begin()
	{
		return m_pRangeValueList->begin();	
	}
	iterator end()
	{
		return m_pRangeValueList->end();	
	}

	int Reset();
	int Add(const double& more_equal_than , const double& less_than , const double& value);
	int Save();
private:
	list<RangeValue*>* m_pRangeValueList;
public:
	CaDraw_PipeDiaTable::RangeValue* Find(const double& dSize) const;
};
