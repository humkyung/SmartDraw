#pragma once

#include <afxcolorbutton.h>

#include <vector>
using namespace std;
// CPluginColorButton

class CPluginColorButton : public CMFCColorButton
{
	DECLARE_DYNAMIC(CPluginColorButton)

	typedef struct tagRGBQuad
	{
		BYTE r,g,b;
	}RGBQuad;
public:
	CPluginColorButton();
	virtual ~CPluginColorButton();

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

	vector<RGBQuad> m_oColorList;
public:
	int FillColors(const CString& rIniFilePath);
};


