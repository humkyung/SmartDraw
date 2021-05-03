#pragma once

#include <util/ado/ADODB.h>
#include <Drawing.h>

class COpeningHoleDrawingImpl : public CDrawing
{
public:
	COpeningHoleDrawingImpl();
	~COpeningHoleDrawingImpl();
	
	int Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile);
protected:
	int Write(const STRING_T& sOutputFilePath);
};