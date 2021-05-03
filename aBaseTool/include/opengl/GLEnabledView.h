#if !defined(AFX_GLENABLEDVIEW_H__59CED13F_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_)
#define AFX_GLENABLEDVIEW_H__59CED13F_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView.h : header file
//
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "gl/gl.h"
#include "gl/glu.h"

#include "GLFont.h"
/////////////////////////////////////////////////////////////////////////////
// Global type definitions
	enum InfoField {VENDOR,RENDERER,VERSION,ACCELERATION,EXTENSIONS};
	enum ColorsNumber{INDEXED,THOUSANDS,MILLIONS,MILLIONS_WITH_TRANSPARENCY};
	enum ZAccuracy{NORMAL,ACCURATE};

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView view
#define MAX_LISTS 1

class CGLEnabledView : public CView
{
protected:
	CGLEnabledView();// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGLEnabledView)	
// Attributes
public:

// Operations
public:
/* Stock Display lists functions
DESC.: these display lists are internally organized in a vector (20 max),
       you have control on definition and redrawing only. 
       use them for background elements which are to be drawn everytime
       all together.
NOTE: between BeginStockDispList and EndStockDispList should be present OpenGL calls only (see documentation for which are allowed and how are them treated)
*/
// Information retrieval function
	const CString GetInformation(InfoField type);
// Mouse cursor function
	void SetMouseCursor(HCURSOR mcursor=NULL);
// Attribute retrieval function
	double GetAspectRatio() {return m_nAspectRatio;};

// Overridables
	virtual void OnCreateGL();		// override to set bg color, activate z-buffer, and other global settings
	virtual void OnSizeGL(int cx, int cy);	// override to adapt the viewport to the window
	virtual void OnDrawGL();		// override to issue drawing functions
	void DeleteDisplayList();
	virtual void VideoMode(ColorsNumber &c,ZAccuracy &z,BOOL &dbuf); // override to specify some video mode parameters
protected:
	GLuint FontCreateBitmaps(HDC hdc,	char  *typeface,	/* I - Font specification */
						int   height,		/* I - Font height/size in pixels */
						int   weight,		/* I - Weight of font (bold, etc) */
						DWORD italic);
	void TextOutGL(float x,float y,float z,float nScale,float nRotate,const char *str);

// Overrides
// NOTE: these have been declared private because they shouldn't be
//		 overridden, use the provided virtual functions instead.
private:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLEnabledView)
	public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGLEnabledView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
// NOTE: these have been declared private because they shouldn't be
//		 overridden, use the provided virtual functions instead.
private:
	//{{AFX_MSG(CGLEnabledView)
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnDestroy();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// member variables
protected:
	RECT	m_rectClient;    // client area size
	POINT	m_ptClientCenter;
	double	m_nAspectRatio;    // aspect
	double	m_nScaleX,m_nScaleY,m_nScaleZ;

	bool	m_bLButtonDown,m_bMButtonDown,m_bRButtonDown;

	int	m_nDispListIndex;// Internal stock display list vector

	CDC*	m_pDC;	// WinGDI Device Context
	CGLFont m_GLFont;
private:
	HGLRC	m_hRC;	// OpenGL Rendering Context
	HCURSOR m_hMouseCursor;	// mouse cursor handle for the view
	CPalette m_CurrentPalette; // palettes
	CPalette* m_pOldPalette;
// initialization helper functions
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	void CreateRGBPalette();
	BOOL SetupPixelFormat();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLENABLEDVIEW_H__59CED13F_E584_11D1_ACB3_E52ED8AC9002__INCLUDED_)
