#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__ __declspec(dllexport)
#endif

__DLL_EXPORT__ short little2big(const short sh);
__DLL_EXPORT__ long  little2big(const long  lo);
__DLL_EXPORT__ float little2big(const float fl);
__DLL_EXPORT__ short big2little(const short sh);
__DLL_EXPORT__ long  big2little(const long  lo);
__DLL_EXPORT__ float big2little(const float fl);
__DLL_EXPORT__ long  little2vax(const long l);
__DLL_EXPORT__ long  vax2little(const long l);

#endif
