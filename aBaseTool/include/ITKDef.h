#ifndef __DEF_H__
#define __DEF_H__

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef unix
	#include <endian.h>

	#undef __LITTLE_ENDIAN__
	#undef __BIG_ENDIAN__

	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define __LITTLE_ENDIAN__
	#elif __BYTE_ORDER == __BIG_ENDIAN
		#define __BIG_ENDIAN__
	#else
		#error No byte sex defined
	#endif
#else	// assume this platform is M$-Windows
	#undef __LITTLE_ENDIAN__
	#undef __BIG_ENDIAN__

	#define __LITTLE_ENDIAN__
#endif

#ifndef LOW
	#define	LOW	0
#endif
#ifndef HIGH
	#define	HIGH	1
#endif

#ifndef MAX
#define MAX(lo,hi) (((hi) > (lo)) ? (hi) : (lo))
#endif

#ifndef MIN
#define MIN(lo,hi) (((hi) < (lo)) ? (hi) : (lo))
#endif

#ifndef PI  
#define PI  3.14159265358979323846
#endif

#ifndef DEG2RAD
#define DEG2RAD(deg) ((deg)*PI/180.)
#endif

#ifndef RAD2DEG
#define RAD2DEG(rad) ((rad)*180./PI)
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(array) ((sizeof((array))/sizeof((array[0]))))
#endif

//	little endian long 2 mean endian long;
#define SWAP_LITTLE32_FOR_VAX(l) {\
	long _x=(l);\
\
	(l)   = (_x & 0x0000ffff);\
	(l) <<= 16;\
	(l)  |= ((_x & 0xffff0000) >> 16);\
	}

#define	SWAP_VAX32_FOR_LITTLE(l){\
	long _x=(l);\
\
	(l)   = (_x & 0xffff0000);\
	(l) >>= 16;\
	(l)  |= ((_x & 0x0000ffff) << 16);\
}

#define SWAP_VAX32PTR_FOR_LITTLE(p)\
	((p)[2]	+		\
	 (p)[3]*256 +		\
	 (p)[1]*65536*256 +	\
	 (p)[0]*65536)

#endif
