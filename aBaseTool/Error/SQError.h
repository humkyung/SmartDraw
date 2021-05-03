#ifndef __SQERROR_H__
#define __SQERROR_H__

#include <list>
using namespace std;

#include "Throwable.h"

class SQError{
public:
	SQError();
	~SQError();
public:
	void Log();
	void Pure();
	bool operator +=(const KThrowable& err);
public:
	static SQError*   m_pErrorHandler;
private:
	bool m_bLogged;
	list<KThrowable> m_lstError;
};

extern SQError* GetErrorHandler();
#endif

