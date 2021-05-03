#pragma once

#include <afxcolorbutton.h>
#include "SmartDrawRscImp.h"

#include <vector>
using namespace std;
// CSmartDrawColorButton

class ON_EXT_SMARTDRAWRSC CSmartDrawColorButton : public CMFCColorButton
{
	DECLARE_DYNAMIC(CSmartDrawColorButton)
public:
	typedef struct tagRGBQuad
	{
		BYTE r,g,b;
	}RGBQuad;

	CSmartDrawColorButton();
	virtual ~CSmartDrawColorButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
private:
	CString m_rIniFilePath;
	CPalette m_Palette;
public:
	int SetColorIndex(const int& nIndex);
	int GetColorIndex(void);
protected:
	virtual void PreSubclassWindow();

	vector<RGBQuad>* m_pColorList;
public:
	int FillColors(const CString& rIniFilePath , const CString& sOutputFormat);
	int FillColors(vector<CSmartDrawColorButton::RGBQuad>* pColorList);	/// 2012.04.15 added by humkyung
};


