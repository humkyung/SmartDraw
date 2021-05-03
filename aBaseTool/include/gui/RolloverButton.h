// RolloverButton.h : implementation file
// Heiko Eichhorn
//
/////////////////////////////////////////////////////////////////////////////

// CRolloverButton window

#ifndef _ROLLOVERBUTTON_H
#define _ROLLOVERBUTTON_H

#define	ROLLOVERBUTTON_CENTER	-1

//#include "BalloonHelp.h"

#ifndef	DLL_EXPORT
	#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT CRolloverButton : public CButton{
	DECLARE_DYNCREATE(CRolloverButton)

	//Construction, Destruction
public:
	CRolloverButton();
	virtual ~CRolloverButton();
	//Methods
public:
	int			SetImageOffset(int nPixels); 
	int			SetTextOffset(int nPixels);
	CPoint			SetImagePos(CPoint p);
	CPoint			SetTextPos(CPoint p);
	BOOL			SetBitmaps(UINT nID, int nWidth, int nHeight, COLORREF crTransparentMask);
	void			SetHelpText(CString cHelpTitle, CString cHelpContent);
	void			SetColorTab(COLORREF crTab);
	BOOL			SetDefaultButton(BOOL bState = TRUE);
	//Properties
private:
	BOOL			m_bUseOffset;				
	CPoint			m_pointImage;
	CPoint			m_pointText;
	int				m_nImageOffsetFromBorder;
	int				m_nTextOffsetFromImage;					
	CBitmap			m_hBitmap;
	CImageList		m_hImages;
	int				m_nImageWidth, m_nImageHeight;
	char			m_bColorTab;				
	COLORREF		m_crColorTab;
	BOOL			m_bDefault;
	UINT			m_nOldAction;
	UINT			m_nOldState;
	BOOL			m_bOverControl;
	BOOL			m_bTracking;
	//CBalloonHelp	m_hBalloonHelp;
	CString			m_cHelpTitle;
	CString			m_cHelpContent;
	BOOL			m_HasImage;
private:
	void			CheckPointForCentering(CPoint &p, int nWidth, int nHeight);
	void			Redraw();
	BOOL			HasImage();
private:
	//{{AFX_VIRTUAL(CRolloverButton)
	private:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	private:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
private:
	LRESULT afx_msg OnMouseLeave(WPARAM wparam, LPARAM lparam);
	LRESULT afx_msg OnMouseHover(WPARAM wparam, LPARAM lparam);
	void	afx_msg OnMouseMove(UINT nFlags, CPoint point);
	void	afx_msg OnRButtonDown( UINT nFlags, CPoint point);
	//{{AFX_MSG(CRolloverButton)
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
