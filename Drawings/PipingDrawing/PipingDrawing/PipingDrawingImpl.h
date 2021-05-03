#pragma once

#include <util/ado/ADODB.h>
#include <Drawing.h>

class CPipingDrawingImpl : public CDrawing
{
public:
	CPipingDrawingImpl(const CString&);
	~CPipingDrawingImpl();
	
	int Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile , const double& dDrawingScale);
protected:
	int Write(const STRING_T& sOutputFilePath , const double& dDrawingScale);
private:
	CString m_sOutputFormat;
};