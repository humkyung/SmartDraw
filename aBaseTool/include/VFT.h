#ifndef __VFT_H__
#define __VFT_H__

#include "SQLib.h"
#define	VFT_VER "Ver 1.0"

typedef enum{
	MOVETO = 1,
	LINETO = 2
}OPCODE_E;

typedef struct tagvline{
	short    nPoints;
	POINT_T* ptrPoints;
}VLINE;

typedef struct tagvchar{
	short  nCharNum;
	short  nLines;
	VLINE* ptrLines;
}VCHAR;

typedef struct tagVData{
	char   chCode;
	float* ptrFloat;
	
	struct tagVData* next;
}VDATA_T;

typedef struct{
	short    nCharNum;
	VDATA_T *ptrData;
}VCHAR_T;

class CVFT{
public:
	CVFT();
	virtual ~CVFT();
public:
	bool   Read(const char* pFilePath);
	bool   Load(const char* pFilePath);
	bool   Write(const char* pFilePath);
	//VCHAR* GetChar(const char ch);
	VCHAR_T* GetChar(const char ch);
	void   Dump(const char* pFilePath);
protected:
	void  LoadChar(FILE* fp);
	void  ReadChar(FILE* fp);
	void  ReadLine(VLINE* ptrLine,FILE* fp);
	VDATA_T* ReadData(FILE* fp);
	
	short ReadShort(FILE* fp);
	void  WriteShort(FILE* fp,short sh);
	float ReadFloat(FILE* fp);
	void  WriteFloat(FILE* fp,float fl);

	void  WriteChar(FILE* fp,VCHAR* ptrChar);
	void  WriteLine(FILE* fp,VLINE* ptrLine);
protected:
	static char m_szHead[4];

	char   m_szFontName[256];
	float  m_nHeight;
	float  m_nAdvances[256];
	
	VCHAR*   m_ptrData[256];
	VCHAR_T* m_ptrChar[256];
};

#endif
