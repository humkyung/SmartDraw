// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SmartDrawDoc.h : interface of the CSmartDrawDoc class
//


#pragma once
#include <IsTools.h>
#include <aDraw_DraFile.h>
#include <aDraw_Entity.h>
#include <SmartDrawColorButton.h>
#include "RevisionSourceData.h"

#include <map>
using namespace std;

class CSmartDrawDoc : public CDocument
{
protected: // create from serialization only
	CSmartDrawDoc();
	DECLARE_DYNCREATE(CSmartDrawDoc)

// Attributes
public:
	static CSmartDrawDoc* GetInstance();
	typedef enum eGenerationType{HLR = 0x01 , ANNOTATION = 0x02};
	CString m_rProjectName;
// Operations
public:
	int GetValuesOf(list<STRING_T>& ,const STRING_T& ) const;
	int GetOperatorsOf(list<STRING_T>&,const STRING_T&) const;	/// 2016.03.17 added by humkyung
	int GetPropsOf(list<STRING_T>&,const STRING_T&) const;		/// 2016.03.17 added by humkyung
	bool IsFilterItem(const STRING_T&) const;					/// 2016.03.17 added by humkyung
	int GetGenerationType() const;						/// 2014.08.25 added by humkyung
	int OpenDrawing(const CString& sDrawingFilePath);	/// 2014.06.02 added by humkyung
	int GetSelectedDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList);
// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSmartDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CRevisionSourceData m_oRevisionSource;
protected:
	STRING_T m_sProjectDesc , m_sProjectUOR , m_sDrawingType , m_sOutputFormat;	/// 2012.02.13 added by humkyung
	STRING_T m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ;				/// 2014.11.09 added by humkyung
	STRING_T m_sCADApplication;													/// 2015.03.31 added by humkyung
	STRING_T m_sPassword;
	STRING_T m_sDatabaseFilePath;
	STRING_T m_sRefFilePath;													/// 2016.02.15 added by humkyung
	int m_iGenerationType;
	STRING_T m_sProjectUnit;													/// 2016.01.27 added by humkyung
	STRING_T m_sProjectCode;													///#57: 2016.01.27 added by humkyung

	CString m_sProjectSettingFilePath;		/// 2012.02.12 added by humkyung

	map<CaDraw_Entity::MODEL_TYPE_T , STRING_T> m_oModelFolderPathMap;		/// 2012.02.15 added by humkyung
	map<STRING_T , vector<CSmartDrawColorButton::RGBQuad>* > m_oColorMap;	/// 2012.04.15 added by humkyung
// Generated message map functions
protected:
	afx_msg void OnProjectSetting();
	void OnGenerateDra();					/// 2012.02.12 added by humkyung
	afx_msg void OnEditRevisionData();		/// 2012.03.28 added by humkyung
	afx_msg void OnUpdateGenerateHLR(CCmdUI *pCmdUI);
	afx_msg void OnGenerateHLR();
	afx_msg void OnUpdateGenerateAnnotation(CCmdUI *pCmdUI);
	afx_msg void OnGenerateAnnotation();
	afx_msg void OnUpdateGenerate(CCmdUI *pCmdUI);
	afx_msg void OnGenerate();
	afx_msg void OnUpdateInsertRevNo(CCmdUI *pCmdUI);
	afx_msg void OnInsertRevNo();
	afx_msg void OnUpdateUpdateTitleBlock(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTitleBlock();
	afx_msg void OnUpdateFillMatchlineData(CCmdUI *pCmdUI);
	afx_msg void OnFillMatchlineData();
	afx_msg void OnUpdateGenerateReport(CCmdUI *pCmdUI);
	afx_msg void OnGenerateReport();
	afx_msg void OnEditPipeDiaTable();
	afx_msg void OnRibbonManual();			/// 2014.05.10 added by humkyung
	DECLARE_MESSAGE_MAP()
public:
	STRING_T GetModelFolderPathWith(const CaDraw_Entity::MODEL_TYPE_T& modelType);	/// 2012.02.15 added by humkyung

	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

	int LoadPipeDiaTable() const;	/// 2016.02.16 added by humkyung

	///#57: 2016.02.13 added by humkyung
	STRING_T GetProjectCode() const;
	int SetProjectCode(const STRING_T&);
	/// up to here

	/// 2016.01.27 added by humkyung
	STRING_T GetProjectUnit() const;
	int SetProjectUnit(const STRING_T&);
	/// up to here

	/// 2014.11.09 added by humkyung
	STRING_T GetModelOffsetX() const;
	STRING_T GetModelOffsetY() const;
	STRING_T GetModelOffsetZ() const;
	int SetModelOffsetX(const STRING_T&);
	int SetModelOffsetY(const STRING_T&);
	int SetModelOffsetZ(const STRING_T&);
	/// up to here

	CString GetCADAppPath() const;			/// 2015.04.02 added by humkyung
	void SetCADAppPath(const STRING_T&);	/// 2016.03.10 added by humkyung
	CString GetProjectName(void) const;
	CString GetDefaultExt() const;			/// 2014.06.26 added by humkyung
	bool IsMSTNOutput() const;				/// 2014.06.25 added by humkyung
	bool IsAutoCADOutput() const;			/// 2014.06.25 added by humkyung
	CString GetLineWeightFilePath() const;	/// 2014.07.02 added by humkyung
	CString GetLineStyleFilePath() const;	/// 2014.06.22 added by humkyung
	STRING_T GetOutputFormatString() const;	/// 2014.06.21 added by humkyung
	STRING_T GetDrawingType() const;		/// 2012.03.16 added by humkyung
	CString GetPrjSettingFilePath() const;	/// 2012.04.10 added by humkyung
	vector<CSmartDrawColorButton::RGBQuad>* GetColorListOf(const STRING_T& sIniFilePath , const bool& bReload=false);	/// 2012.04.15 added by humkyung
	STRING_T GetAccessFilePath() const;		/// 2012.09.19 added by humkyung
	STRING_T GetRefFilePath() const;		/// 2016.02.15 added by humkyung
	STRING_T GetProjectDesc() const;		/// 2012.12.27 added by humkyung
	STRING_T GetPassword() const;			/// 2012.12.27 added by humkyung
};


