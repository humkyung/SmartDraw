#ifndef	__UTILITY_H__
#define	__UTILITY_H__

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

#define	MAX_TOKEN_COUNT	256
#define	TOKEN_BUF_SIZ	2048
#define	LINE_BUF_SIZ	5000

typedef struct
{
	int nCount;
	char buf[MAX_TOKEN_COUNT][TOKEN_BUF_SIZ];
}Tokens;

extern Public void ResetTokens();
extern Tokens* Split( char* pText , char delimiter );
extern Tokens* SplitCSVString( char* pText );
extern int GetLevel(char* sLayer);

extern Public int GetTextJustify(char* sJustify);
extern Public int GetLevel(char* sLayer);
extern Public int GetFontNo(char* sFontname);
extern Public int GetLineType(char* sLineType);
extern Public int GetLineWeight(char* sLineWeight);

extern Public void PrintError(char *errorMsg);

extern Public double GetRadian(double nDegree);
extern Public Dpoint3d Rotate(Dpoint3d *p, double angle);
extern Public Dpoint3d Move(Dpoint3d *p, Dpoint3d *offset); 
extern Public Dpoint3d GetIntPoint(Dpoint3d *p); 

extern Public void PrintError(char *errorMsg);

#endif