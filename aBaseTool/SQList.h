#ifndef __SQLIST_H__
#define __SQLIST_H__

#include <stdarg.h>

#include <list>
#include <vector>
#include <stack>
#include <algorithm>
#include <functional>

#include "htypes.h"
using namespace std;

#define VAL(s) static_cast<SQValue>(s)
template<class T>
class SQList : public list<T>{
public:
	SQList();
	SQList(const T& val);
	SQList(const SQList<T>& lst);
	~SQList();
public:
	T& First(); 
	T& Second();
	T& Third();
	T& Fourth();
	T& Nth(const int index);

//	void Sort(bool (*)(const SQValue&,const SQValue&));
	void Collect(int count,...);
	SQList<T>* AppendSublist();

	bool operator =(DList* dlist);

	SQList<T>* list();
	friend SQList<T> operator +(SQList<T>& operand1,SQList<T>& operand2);
};

#include "SQList.cpp"

#endif
