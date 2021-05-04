#ifndef	_DIMENSION_H_
#define	_DIMENSION_H_

#include <mdl.h>
#include "Utility.h"

#define	DIM_STYLE_NAME_SIZE	16

typedef struct DimStyleTag
{
	char   szName[DIM_STYLE_NAME_SIZE + 1];
	int    nFontName;
	double dTextHeight;
	double dTextWidth;	//! added by HumKyung.Baek 2010.11.08
	int    nTextWeight;
	int    nTextLevel;		/// 2012.03.18 added by humkyung
	int    nTextColor;
	float  fTextOffset;
	int    nTextPrecision;		/// 2012.03.18 added by humkyung
	int		nDimLineColor;		/// 2012.03.18 added by humkyung
	double	fDimLineDistance;	/// 2012.03.18 added by humkyung
	float	fExtLineOffset;
	float	fExtLineExtension;
	int		nExtLineColor;
	char	szArrowType[16];
	float	fArrowWidth , fArrowHeight;
	boolean bAutoSize;			/// 2014.11.16 added by humkyung

	struct DimStyleTag* next;
}DimStyle;

extern DimStyle* Find_DimStyle( char* pDimStyleName );
extern DimStyle* Parse_DimStyle(Tokens* pTokens);
extern Public Parse_Dimension(Tokens* pTokens);
extern int GetColorNo(char* sColor);
extern int GetLineWeight(char* sWeight);
extern Draw_Line(Dpoint3d *ptStart, Dpoint3d *ptEnd, int nColor, int nLayer, int nStyle, float fScale, int nWeight);
extern Public boolean Draw_DimAngular(Dpoint3d *ptCenter,Dpoint3d *ptStart, Dpoint3d *ptEnd, int nLayer, char *pDimStyleName);

#endif