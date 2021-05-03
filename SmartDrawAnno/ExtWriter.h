#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExtDivision.h"

#include <fstream>
using namespace std;

#define EXT_BUF_SIZE	1024

class CExtWriter
{
public:
	CExtWriter(CExtDivision* pDivision);
	virtual ~CExtWriter();
public:
	void Write(OFSTREAM_T& file,const double& dDrawingScale);
protected:
	void	      WriteMatchLineData(OFSTREAM_T& file,const double& dDrawingScale);
	void          WriteDimension(OFSTREAM_T& filem,const double& dDrawingScale);
	void          WriteSinglePipeTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale);
	void	      WriteElevTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale);
	virtual void  WriteStruTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale);
	virtual void  WriteETIMTag(OFSTREAM_T& file,CaDraw_TagPos* pTagPos,const double& dDrawingScale);
	void  WriteViewCoord(OFSTREAM_T& file,const double& dDrawingScale);
private:
	TCHAR m_szBuf[EXT_BUF_SIZE];	/// buffer to store text string

	CExtDivision *m_pDivision;
};

