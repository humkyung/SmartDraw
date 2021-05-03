#include <utility/iolib.h>

short itk::little2big(const short sh)
{
    	short nRet=0;

	char* src=(char*)&sh;
	char* dst=(char*)&nRet;
	*(dst + 1) = *(src);
	*(dst)     = *(src + 1);

	return nRet;
}

long  itk::little2big(const long  lo)
{
    	long nRet=0;

	char* src=(char*)&lo;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

float itk::little2big(const float fl)
{
    	float nRet=0.;

	char* src=(char*)&fl;
	char* dst=(char*)&nRet;
	*(dst + 3) = *(src);
	*(dst + 2) = *(src + 1);
	*(dst + 1) = *(src + 2);
	*(dst)     = *(src + 3);

	return nRet;
}

short itk::big2little(const short sh)
{
    	short nRet=0;

	char* src=(char*)&sh;
	char* dst=(char*)&nRet;
	*(dst + 1) = *(src);
	*(dst)     = *(src + 1);

	return nRet;
}

long  itk::big2little(const long  lo)
{
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
float itk::big2little(const float fl)
{
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
long itk::little2vax(const long l)
{
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
long itk::vax2little(const long l)
{
	long nRet=0L;

	nRet   = (l & 0xffff0000);
	nRet >>= 16;
	nRet  |= ((l & 0x0000ffff) << 16);

	return nRet;
}
