#ifndef __HSR_DEF_H__
#define __HSR_DEF_H__

#include <assert.h>

#pragma warning(push)
#include <math.h>
#pragma warning(disable: 4514) // unreferenced inline function has been removed
#pragma warning(pop)

#include <SQLib.h>

#ifdef VER_01
	#define VER	_T("HSR Ver 0.1")
#elif VER_02
	#define VER	_T("HSR Ver 0.2")
#elif VER_03
	#define VER	_T("HSR Ver 0.3")
#endif

// kinds of render type.
typedef enum
{
	SHOW	    = 0,
	INTERSECTION= 0,
	HIDE	    = 2,
	INVALID     = 3,
	CUT         = 4
}RENDERTYPE;

typedef enum{
	PARTITIONNODE = 'p',
	INNODE  = 'i',
	OUTNODE = 'o'
}NODETYPE;

typedef enum{
	TOP  = 1,
	FRONT= 2
}VIEWPOINT;

#endif
