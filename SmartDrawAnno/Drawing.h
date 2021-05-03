#pragma once

#include <IsLine2d.h>
#include <aDraw_DraFile.h>

#include "AnnoTerritory.h"

#include <list>
using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL CDrawing
{
	CDrawing(const CDrawing&){}
	CDrawing& operator=(const CDrawing&){return (*this);}
public:
	CDrawing(void);
	virtual ~CDrawing(void);

	virtual int Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile,const double& dDrawingScale);
protected:
	virtual int WriteKeyPlanHatching(OFSTREAM_T& ofile,const double& dDrawingScale);
	virtual int WriteBorderData(OFSTREAM_T& ofile,const double& dDrawingScale);
	///@brief	write nozzle chart
	int WriteNozzleChart(OFSTREAM_T& ofile,const double& dDrawingScale);
	///@brief	write revision chart
	int WriteRevisionChart(OFSTREAM_T& ofile , const STRING_T& sDraFileName , const double& dDrawingScale);
	virtual int Write(const STRING_T& sOutputFilePath , const STRING_T& sDraFileName, const double& dDrawingScale);
	int ParseHsrTextFile(const STRING_T& rHsrFilePath);

	list<CIsLine2d>* m_pHsrLineList;
	list<CAnnoTerritory*>* m_pAnnoTerritoryList;
public:
	STRING_T m_sIniFilePath;
	STRING_T m_sDraFileName;
};
