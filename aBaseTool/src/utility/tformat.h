#ifndef __TFORMAT_H__
#define __TFORMAT_H__

#include "endian.h"

#ifdef	__BINARY__
	char  tformat(const char c){return little2big(c);}
	short tformat(const short s){return short2big(s);}
	long  tformat(const long l){return long2big(l);}
	float tformat(const float f){return float2big(f);}
#else
	template<class T>
	T tformat(T t){return t}
#endif
