#ifndef __IO_H__
#define __IO_H__

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif

namespace itk{
	DLL_EXPORT short little2big(const short sh);
	DLL_EXPORT long  little2big(const long  lo);
       	DLL_EXPORT float little2big(const float fl);
	DLL_EXPORT short big2little(const short sh);
	DLL_EXPORT long  big2little(const long  lo);
       	DLL_EXPORT float big2little(const float fl);
	DLL_EXPORT long  little2vax(const long l);
	DLL_EXPORT long  vax2little(const long l);
};

#endif
