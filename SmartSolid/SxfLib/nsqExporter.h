#ifndef __KNSQEXPORT_H__
#define __KNSQEXPORT_H__

#include <SQLib.h>

enum{
	LINE_FORMAT = 1
};

class CNSQExporter
{
public:
	CNSQExporter();
	~CNSQExporter();
public:
	void Write3dFace(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void OnExportFile(const char* pFilePath,PSQPRIMITIVE pPrimitive , const string& sVer , const string& sLastWriteTime , const double& , const double& , const double&);
private:
	void WriteElement(ofstream& ofile , PSQPRIMITIVE pPrimitive , long &nSeq , long &nGroup);	//! 2011.05.04
	void WriteFace(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WriteSweepSurface(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WriteLine(ofstream& ofile,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	void WriteEllipse(ofstream& ofile,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace , const bool& line_strip);
	void WriteArc(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WritePolyline(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace);
	void WritePoints(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace);
	void WriteVertices(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace);
	void WriteLoop(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WriteSolid(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	bool WritePlaneSurf(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	void WriteSurface(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup , const int& iType = SQ_SURFACE);
	void WriteConeSurface(FILE* fp,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WriteSphere(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup);
	void WriteTorus(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long Group);
	void WriteGroup(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);

	void WriteCone(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	void WriteElbowSurf(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	void WriteDome(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);
	void WriteTee(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup);

	bool IsPlaneSurfWithHole(PSQPRIMITIVE pSurf);
	bool IsTORS_EQP(SQSURFACE::PSURFELM_T pSurf);
	bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2);
	bool IsSamePoint(double x1,double y1,double z1,double x2,double y2,double z2);
public:
	int m_nSkipLevel;	/// skip level
	int m_iModelType;	/// model type
private:
	bool WriteSweepSurf(ofstream& ofile , PSQPRIMITIVE pPrimitive , long& nSeq , long& nGroup , const int& iType);
	const static char* szFormat[2];
	char m_szType[16];

	double m_dModelOffset[3];	/// 2014.11.08 added by humkyung
};

#endif
