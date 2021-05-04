#ifndef	_CLOUD_H_
#define	_CLOUD_H_

#include <mdl.h>
#include <cexpr.h>
#include <rscdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmdlist.h>
#include <dlogitem.h>
#include <system.h>
#include <global.h>
#include <userfnc.h>
#include <mselems.h>
#include <refernce.h>
#include <tcb.h>
#include <scanner.h>
#include <mdldim.h>
#include <dlogman.fdf>
#include <msdim.fdf>
#include <mdlerrs.h>
#include <mstagdat.fdf>
#include <mslinkge.fdf>

extern boolean Draw_Cloud(Dpoint3d *ptCloud, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount, float fArcMin, float fArcMax);

#endif