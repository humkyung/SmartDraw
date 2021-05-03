#ifndef __REF_H__
#define __REF_H__

#include "Size.h"

typedef struct tagReference{
	short	nIndex;
	char	szName[62];
	double	nScale;
	POINT_T	ptDestOrigin;
	TSIZE   sizDest;
	POINT_T	ptRefOrigin;
	double	nTrans[9];
}REFERENCE_T,* PREFERENCE_T;

#endif
