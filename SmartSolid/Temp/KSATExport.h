#ifndef __KSATEXPORT_H__
#define __KSATEXPORT_H__

class KSATExport{
public:
	KSATExport();
	~KSATExport();
public:
	void OnExportFile(const char* pFilePath,list<SATSURFACE>& lstSATSurface);
private:
	void ExportPlaneSurface(FILE* fp,list<PSATLINE>* pSATLine);
};

#endif