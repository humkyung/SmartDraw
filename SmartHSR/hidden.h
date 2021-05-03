#ifndef __HIDDEN_H__
#define __HIDDEN_H__

#include "HiddenDef.h"
#include "HSRFace.h"

bool  IsEqualPoint(POINT_T pt1,POINT_T pt2);
CHSRFace* AppendFace(CHSRFace* pFace,CHSRFace* pValue);
#endif