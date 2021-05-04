#ifndef	_INSERT_H_
#define	_INSERT_H_

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

typedef struct tagInsert
{
	char *psCellLibPath;
	char *psCellName; 
	
	char sWhere[8];

	Dpoint3d ptInsert , ptScale;
	double fAngle;
}Insert;




extern boolean Draw_Insert(Insert *pInsert, int nLayer, char *sExplode);
extern boolean Draw_Cell(Insert *pInsert, int nLayer, int nColor);

#endif