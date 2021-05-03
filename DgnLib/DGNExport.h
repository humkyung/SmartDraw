#ifndef __KDGNEXPORT_H__
#define __KDGNEXPORT_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT CDGNExport{
public:
	CDGNExport();
	~CDGNExport();
public:
	int    GetDimension(){return m_nDimension;}
	double GetSubUnit(){return m_nSubUnit;}
	void OnExportFile(const char* pExportFilePath,PSQPRIMITIVE pPrimitive);
	long WritePrimitive(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
protected:
	void OnCreate();
	void OnDestroy();
private:
	CDGNFile* OpenFile(const char* pExportFilePath);
	void CloseFile(CDGNFile* hDGN);
	bool DGNCopyFile(CDGNFile* hDGN,const char* pFilePath);
	void GetEnvironment(CDGNFile* hDGN,const char* pSeedFilePath);
	void SetColorTable(CDGNFile* hDGN);

	bool GetElmHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	void PutTextHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);

	long WriteLineX(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long WriteArrow(CDGNFile* hDGN,const LINE_TX& linex);
	long WritePoints(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long WriteLoop(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long WriteShape(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long WriteCircle(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long WriteText(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long WriteComplex(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long WriteGroup(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);

	void UpdateVolumeBox(PSQPRIMITIVE pPrimitive);
	void UpdateComplexVolumeBox(PSQPRIMITIVE pPrimitive);
	long GetSize(PSQPRIMITIVE pPrimitive);
	long GetGroupSize(PSQPRIMITIVE pPrimitive);
	long GetComplexSize(PSQPRIMITIVE pPrimitive);
	void AsciiToRad50( const char *str, unsigned short *pRad50);
public:
	char m_szSeedFilePath[1024];
private:
	long m_nSubUnit;
	int  m_nDimension;
};

#endif