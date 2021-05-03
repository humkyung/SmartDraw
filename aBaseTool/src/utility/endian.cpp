#include "endian.h"

short little2big(const short sh){
    	short nRet=0;

	char* src=(char*)&sh;
	char* dst=(char*)&nRet;
	*(dst + 1) = *(src);
	*(dst)     = *(src + 1);

	return nRet;
}

long  little2big(const long  lo){
    	long nRet=0;

	char* src=(char*)&lo;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

float little2big(const float fl){
    	float nRet=0.;

	char* src=(char*)&fl;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

short big2little(const short sh){
    	short nRet=0;

	char* src=(char*)&sh;
	char* dst=(char*)&nRet;
	*(dst + 1) = *(src);
	*(dst)     = *(src + 1);

	return nRet;
}

long  big2little(const long  lo){
    	long nRet=0;

	char* src=(char*)&lo;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
float big2little(const float fl){
    	float nRet=0.;

	char* src=(char*)&fl;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

//	parameter	:
//	description	: little endian long 2 mean endian long;
//	remarks		:
//	returns		:
long little2vax(const long l){
	long nRet=0L;

	nRet   = (l & 0x0000ffff);
	nRet <<= 16;
	nRet  |= ((l & 0xffff0000) >> 16);

	return nRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long vax2little(const long l){
	long nRet=0L;

	nRet   = (l & 0xffff0000);
	nRet >>= 16;
	nRet  |= ((l & 0x0000ffff) << 16);

	return nRet;
}
