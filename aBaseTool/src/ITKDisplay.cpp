#include <string.h>
#include <ITKDisplay.h>

struct tagCOLORS{
	int   nColor;
	char* szColor;
}COLORS[]={
	{SQ_LINE_BLACK,"BLACK"},
	{SQ_LINE_BLUE,"BLUE"},
	{SQ_LINE_GREEN,"GREEN"},
	{SQ_LINE_CYAN,"CYAN"},
	{SQ_LINE_RED,"RED"},
	{SQ_LINE_MAGENTA,"MAGENTA"},
	{SQ_LINE_BROWN,"BROWN"},
	{SQ_LINE_LIGHTGRAY,"LIGHTGRAY"},
	{SQ_LINE_DARKGRAY,"DARKGRAY"},
	{SQ_LINE_LIGHTBLUE,"LIGHTBLUE"},
	{SQ_LINE_LIGHTGREEN,"LIGHTGREEN"},
	{SQ_LINE_LIGHTCYAN,"LIGHTCYAN"},
	{SQ_LINE_LIGHTRED,"LIGHTRED"},
	{SQ_LINE_LIGHTMAGENTA,"LIGHTMAGENTA"},
	{SQ_LINE_YELLOW,"YELLOW"},
	{SQ_LINE_WHITE,"WHITE"}
};

struct tagSQLINETYPE{
	int   nType;
	short nPattern;
}SQLINETYPE[]={
	{SQ_LINE_SOLID,0xffff},
	{SQ_LINE_DOT,0x0101},
	{SQ_LINE_DASH,0x00ff},
	{SQ_LINE_DASH_DOT,0x1c4f}
};

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
const char* GetColorName(int nColor){
	int nSize=sizeof(COLORS)/sizeof(struct tagCOLORS);

	if((nColor >= 0) && (nColor < nSize)) return COLORS[nColor].szColor;

	return COLORS[SQ_LINE_WHITE].szColor;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
const int GetColorNumber(const char* szColor){
	int nSize=sizeof(COLORS)/sizeof(struct tagCOLORS);

	for(int i=0;i < nSize;i++){
		if(0 == strcmp(szColor,COLORS[i].szColor)) return COLORS[i].nColor;
	}

	return COLORS[SQ_LINE_WHITE].nColor;
}
