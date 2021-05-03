#pragma once

#include "aDraw_View.h"
#include "aDraw_LogicView.h"
#include "aDraw_Entity.h"

#include <IsTools.h>
#include <IsVolume.h>
#include <list>
#include <string>
using namespace std;

class ON_MODULE_EXT_DLL CaDraw_DraFile
{
	CaDraw_DraFile(const CaDraw_DraFile&){}
	CaDraw_DraFile& operator=(const CaDraw_DraFile&){ return (*this);}
public:
	enum
	{
		LEFT_EXT_SIDE   = 0x01,
		RIGHT_EXT_SIDE  = 0x02,
		TOP_EXT_SIDE    = 0x04,
		BOTTOM_EXT_SIDE = 0x08
	};

	CaDraw_DraFile(const STRING_T& rFilePath);
	~CaDraw_DraFile(void);
private:
	STRING_T *m_pFilePath;
	STRING_T *m_pSettingFileName;
	
	list<CaDraw_View*>* m_pViewList;						/// 2012.02.11 added by humkyung
	list<CaDraw_LogicView*>* m_pLogicViewList;
public:
	STRING_T GetDwgNo() const;								/// 2016.03.26 added by humkyung
	int FillMatchLineData(const CaDraw_DraFile*,const STRING_T&,const STRING_T&);	/// 2016.03.07 added by humkyung
	int ResetMatchLineData(const STRING_T&);										/// 2016.03.07 added by humkyung
	LPCTSTR status() const;
	int GenerateReport(const STRING_T& sReportFilePath);	/// 2016.02.11 added by humkyung
	int Report(const STRING_T& sReportFilePath) const;		/// 2012.02.15 added by humkyung

	int SetViewCoords(const CaDraw_View::COORD_OPTION_E& eCoords);	/// 2012.03.25 added by humkyung
	int SetExtTagSide(const int& iExtTagSide);				/// 2012.03.18 added by humkyung

	STRING_T GetFilePath() const;							/// 2012.02.17 added by humkyung

	CaDraw_View* GetView(const STRING_T& sViewName);		/// 2012.02.12 added by humkyung
	CaDraw_View* AddView(const STRING_T& sViewName);		/// 2012.02.12 added by humkyung
	int GetViewList(list<CaDraw_View*>* pViewList) const;	/// 2012.02.11 added by humkyung

	STRING_T GetFileNameWithoutExt() const;
	int SetSettingFileName(LPCTSTR pSettingFileName);		/// 2012.02.13 added by humkyung
	const TCHAR* GetSettingFileName() const;
	int GetLogicViewList(list<CaDraw_LogicView*>* pLogicViewList) const;
	int ChangeViewName(const STRING_T& rNewViewName, const STRING_T& rViewName);
	int RemoveView(const STRING_T& sViewName);
	CaDraw_View* CopyView(const STRING_T& rNewViewName, const STRING_T& rViewName);
	int GetTotalReferenceFile(list<STRING_T>& TotalReferenceFile);
	bool DoesIncludeVolume(const CIsVolume& volume);
	///int Add(CaDraw_Entity* pEnt);

	int Save();				/// 2012.02.12 added by humkyung
	int CreateLogicView();	/// 2012.02.11 added by humkyung
	int Parse();
private:
	int m_iExtTagSide;
	CaDraw_View::COORD_OPTION_E m_eViewCoords;
	int Clear(void);
};
