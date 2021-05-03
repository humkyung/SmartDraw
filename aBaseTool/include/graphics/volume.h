/********************************************************************
	created:	2001/08/03
	created:	3:8:2001   10:40
	filename: 	d:\Project\ITK\volume.h
	file path:	d:\Project\ITK
	file base:	volume
	file ext:	h
	author:		
	
	purpose:	
	update :
*********************************************************************/

#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <graphics\point.h>

#ifndef	LOW
#define	LOW	0
#endif

#ifndef	HIGH
#define	HIGH	0
#endif

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

typedef struct tagVolume{
	double x[2];		// x[0] - low of x, x[1] - high of x
	double y[2];		// y[0] - low of y, y[1] - high of y
	double z[2];		// z[0] - low of z, z[1] - high of z
}VOLUME_T,* PVOLUME_T,SQVOLUME,* PSQVOLUME;

DLL_EXPORT bool IsPointInVolume(PPOINT_T pPoint,PVOLUME_T pVolume);
DLL_EXPORT bool IsOuterVolume(const VOLUME_T& destvolume,const VOLUME_T& volume);
DLL_EXPORT bool IsInnerVolume(const VOLUME_T& destvolume,const VOLUME_T& volume);
DLL_EXPORT bool IsStraddleVolume(const VOLUME_T& destvolume,const VOLUME_T& volume);
	
#endif
