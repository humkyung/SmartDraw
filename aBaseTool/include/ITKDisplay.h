/********************************************************************
	created:	2002/06/17
	created:	17:6:2002   10:06
	filename: 	D:\Project\ITK\ITKDisplay.h
	file path:	D:\Project\ITK
	file base:	Display
	file ext:	h
	author:		baek hum kyung(baekhk@daum.net)
	
	purpose:	
*********************************************************************/
#ifndef __ITKDISPLAY_H__
#define	__ITKDISPLAY_H__

#ifndef _COLORS_
#define _COLORS_

//------line color------
enum{
	BLACK		=	0,
	BLUE		=	1,
	GREEN		=	2,
	CYAN		=	3,
	RED		=	4,
	MAGENTA		=	5,
	BROWN		=	6,
	LIGHTGRAY	=	7,
	DARKGRAY	=	8,
	LIGHTBLUE	=	9,
	LIGHTGREEN	=	10,
	LIGHTCYAN	=	11,
	LIGHTRED	=	12,
	LIGHTMAGENTA	=	13,
	YELLOW		=	14,
	WHITE		=	15
};

//------line type------
enum{
	SOLID_LINE	=	0,
	DOT_LINE	=	1,
	DASH_LINE	=	2,
	DASH_DOT_LINE	=	3,
	CENTER_LINE	=	4,
	HIDDEN_LINE	=	5
};

extern const char* GetColorName(int nColor);
extern const int   GetColorNumber(const char* szColor);

#endif

#endif
