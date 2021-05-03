#pragma once

#include <util/ado/ADODB.h>
#include <Drawing.h>

class CCableTrayDrawingImpl : public CDrawing
{
public:
	CCableTrayDrawingImpl();
	~CCableTrayDrawingImpl();
	
	int Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile);
protected:
	int Write(const STRING_T& sOutputFilePath);
};