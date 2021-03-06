#ifndef __SQPAIR_H__
#define __SQPAIR_H__

template<class N,class V>
class CPair{
public:
	CPair();
	CPair(N& name,V& val);
	CPair(const SQPair<N,V>& vpair);
	~CPair();
public:
	bool operator=(const SQPair<N,V>& vpair);
private:
	N m_Name;
	V m_Value;
};

#include "SQPair.cpp"
#endif 