#pragma once

#include "../../SmartDrawModule/aDraw_DraFile.h"

#include <vector>
using namespace std;

typedef LPCTSTR (__stdcall *GetDrawingName)();
typedef LPCTSTR (__stdcall *GetHsrType)();
typedef int  (__stdcall *Annotate)(LPCTSTR pOutputFilePath , LPCTSTR pDraFilePath , LPCTSTR pPrjName , LPCTSTR pPrjFolderPath,const double& dDrawingScale,const double*);
typedef HWND (__stdcall *CreatePropertyPage)(LPCTSTR pName , LPCTSTR pIniFilePath , HWND hParent);
typedef int  (__stdcall *SaveSetting)();
typedef void (__stdcall *ClosePropertyPage)(LPCTSTR pName);
typedef int (__stdcall *InitReport)(LPCTSTR pSqliteFilePath);
typedef int (__stdcall *Report)(CaDraw_DraFile*,const CIsPoint3d&,LPCTSTR,LPCTSTR,LPCTSTR);
typedef struct tagPlugin
{
	HMODULE hModule;
	GetDrawingName pfGetDrawingName;
	GetHsrType pfGetHsrType;
	Annotate pfAnnotate;
	CreatePropertyPage pfCreatePropertyPage;
	SaveSetting pfSaveSetting;
	ClosePropertyPage pfClosePropertyPage;
	InitReport pfInitReport;
	Report pfReport;
}Plugin;

class CPluginManager
{
	CPluginManager(void);
	CPluginManager(const CPluginManager&){}
	CPluginManager& operator=(const CPluginManager&){return (*this);}
public:
	static CPluginManager& GetInstance(void);
	~CPluginManager(void);

	Plugin* GetPluginOf(const STRING_T& sDrawingType);	/// 2012.03.17 added by humkyung

	vector<Plugin*> m_oPluginList;
};
