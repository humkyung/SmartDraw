// RolloverButton.cpp : implementation file
// Heiko Eichhorn
//
/////////////////////////////////////////////////////////////////////////////

// CRolloverButton window

#include "stdafx.h"
#include <gui\RolloverButton.h>

/////////////////////////////////////////////////////////////////////////////
// CRolloverButton

IMPLEMENT_DYNCREATE(CRolloverButton, CButton)

CRolloverButton::CRolloverButton()
{
	m_bColorTab = FALSE;
	m_bDefault = FALSE;
	m_nOldState = 0;
	m_nOldAction = 0;
	m_nImageOffsetFromBorder = 4;
	m_nTextOffsetFromImage = 8;
	m_bUseOffset = TRUE;
	m_bOverControl = m_bTracking = 0;
	m_cHelpTitle = "";
	m_cHelpContent = "";
	m_HasImage = FALSE;
}

CRolloverButton::~CRolloverButton()
{

}

void CRolloverButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}

BEGIN_MESSAGE_MAP(CRolloverButton, CButton)
	//{{AFX_MSG_MAP(CRolloverButton)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()


inline void CRolloverButton::Redraw()
{
	if( m_hWnd != NULL )
	{
		Invalidate();
	}
}

BOOL CRolloverButton::SetBitmaps( UINT nID, int nWidth, int nHeight, COLORREF crTransparentMask )
{
	m_hBitmap.LoadBitmap(nID);
	m_hImages.Create(nWidth, nHeight, ILC_COLORDDB|ILC_MASK, 3, 1);
	m_hImages.Add(&m_hBitmap, crTransparentMask);
	m_nImageWidth = nWidth;
	m_nImageHeight = nHeight;
	m_HasImage = TRUE;
	Redraw();
	return TRUE;
}

void CRolloverButton::SetHelpText(CString cHelpTitle, CString cHelpContent)
{
	m_cHelpTitle = cHelpTitle;
	m_cHelpContent = cHelpContent;
}

void CRolloverButton::SetColorTab(COLORREF crTab)
{	
	m_bColorTab = TRUE;
	m_crColorTab = crTab;
	Redraw();
}

BOOL CRolloverButton::SetDefaultButton( BOOL bState )
{
	CDialog *pDialog = (CDialog *)GetOwner();

	pDialog->SetDefID( GetDlgCtrlID() );
	BOOL bPrevious = m_bDefault;
	m_bDefault = bState;
	Redraw();
	return bPrevious;
}

//	Positioning Functions
int CRolloverButton::SetImageOffset( int nPixels )
{
	int nPrevious = m_nImageOffsetFromBorder;

	m_bUseOffset = TRUE; 
	m_nImageOffsetFromBorder = nPixels; 
	Redraw();
	return nPrevious;
}

int CRolloverButton::SetTextOffset( int nPixels ) 
{ 
	int nPrevious = m_nTextOffsetFromImage;

	m_bUseOffset = TRUE; 
	m_nTextOffsetFromImage = nPixels; 
	Redraw();
	return nPrevious;
}

CPoint CRolloverButton::SetImagePos( CPoint p ) 
{ 
	CPoint pointPrevious = m_pointImage;

	m_bUseOffset = FALSE; 
	m_pointImage = p; 
	Redraw();
	return pointPrevious;
}
CPoint CRolloverButton::SetTextPos( CPoint p ) 
{ 
	CPoint pointPrevious = m_pointText;

	m_bUseOffset = FALSE; 
	m_pointText = p; 
	Redraw();
	return pointPrevious;
}

BOOL CRolloverButton::HasImage()
{
	return (m_HasImage);
}

void CRolloverButton::CheckPointForCentering( CPoint &p, int nWidth, int nHeight )
{
	CRect	rectControl;
	GetClientRect( rectControl );

	if( p.x == ROLLOVERBUTTON_CENTER )
		p.x = ( ( rectControl.Width() - nWidth ) >> 1 );
	if( p.y == ROLLOVERBUTTON_CENTER )
		p.y = ( ( rectControl.Height() - nHeight ) >> 1 );
}

void CRolloverButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{	
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CRect	rectControl( lpDIS->rcItem );
	UINT	nOffset = 0;			
	UINT	nFrameStyle=0;
	BOOL	bDRAWFOCUSONLY = FALSE;	
	int		nStateFlag;					
	UINT	nNewState = lpDIS->itemState;
	UINT	nNewAction = lpDIS->itemAction;
	int		nBmpID;
	
	if ( nNewState & ODS_SELECTED)
	{
		nFrameStyle = DFCS_PUSHED;
		nOffset += 1;
	}

	if( nNewState & ODS_DISABLED )
	{
		nStateFlag = DSS_DISABLED;
		nBmpID = 2;
	}
	else
	{
		nStateFlag = DSS_NORMAL;
		if(m_bOverControl == TRUE)
		{
			nBmpID = 1;
		}
		else
		{
			nBmpID = 0;
		}
	}
	if (nNewAction == ODA_FOCUS )
	{
		bDRAWFOCUSONLY = TRUE;
	}
	if( m_bDefault )
	{
		rectControl.DeflateRect( 1, 1 );
	}
	if( !bDRAWFOCUSONLY )
	{
		if( m_bDefault )
		{
			CPen *pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
							
			pDC->Rectangle( &lpDIS->rcItem );
			pDC->SelectObject( pOldPen );
		}
		pDC->DrawFrameControl(&rectControl, DFC_BUTTON, DFCS_BUTTONPUSH | nFrameStyle);
		if (m_bColorTab)
		{
			CPen penTab; 
			
			#define COLORTABSIZE 8
			if( penTab.CreatePen( PS_SOLID, 1, m_crColorTab) )
			{
				CPen* pOldPen = pDC->SelectObject( &penTab );
							
				int nXOffset = rectControl.left+1 + nOffset;
				int nYOffset = rectControl.top+1 + nOffset;
				for (UINT nStep = 0; nStep < COLORTABSIZE; nStep++)
				{
					pDC->MoveTo( nXOffset, nYOffset + nStep );
					pDC->LineTo( nXOffset + (COLORTABSIZE-nStep)-1, nYOffset + nStep );			
				}
        
				pDC->SelectObject( pOldPen );
			}
		}
		CString		strTitle;
		this->GetWindowText(strTitle);
		if( HasImage() )
		{
			CPoint pt;

			if( m_bUseOffset )
			{
				pt.x = strTitle.IsEmpty() ? ROLLOVERBUTTON_CENTER : rectControl.left + m_nImageOffsetFromBorder;
				pt.y = ROLLOVERBUTTON_CENTER;
			}
			else
				pt = m_pointImage;

			CheckPointForCentering( pt, m_nImageWidth, m_nImageHeight );
			
			pt.Offset( nOffset, nOffset );

			SIZE sz;
			sz.cx = m_nImageWidth;
			sz.cy = m_nImageHeight;
			POINT ptOrigin;

			ptOrigin.x = 0;
			ptOrigin.y = 0;
			m_hImages.DrawIndirect(pDC, nBmpID, pt, sz, ptOrigin, ILD_NORMAL/*CLR_NONE*/, SRCCOPY, CLR_NONE, CLR_DEFAULT );

		}
		if ( !strTitle.IsEmpty() )
		{
			CPoint	pt;
			CSize	sizeText = pDC->GetTextExtent(strTitle);

			if( m_bUseOffset )
			{
				CRect rcClient;
				GetClientRect(rcClient);
				pt.x = !HasImage() ? ROLLOVERBUTTON_CENTER : 
					m_nImageWidth + m_nTextOffsetFromImage + m_nImageOffsetFromBorder +
					(max(0,rcClient.Width()-(m_nImageWidth + m_nTextOffsetFromImage + m_nImageOffsetFromBorder)-sizeText.cx)/2);
				pt.y = ROLLOVERBUTTON_CENTER; 
			}
			else
			{
				pt = m_pointText;
			}
			int nOffsetFixY = pt.y == ROLLOVERBUTTON_CENTER ? -1 : 0;

			CheckPointForCentering( pt, sizeText.cx, sizeText.cy );

			pt.Offset( nOffset, nOffset + nOffsetFixY );
			
			pDC->DrawState( pt, CSize(0,0), strTitle, DST_PREFIXTEXT|nStateFlag, TRUE, 0, (CBrush*)NULL );
		}

	}
	if( !( nNewState & ODS_DISABLED ) )
	{
		if( ( nNewState & ODS_FOCUS ) ^ ( m_nOldState & ODS_FOCUS ) ||
			( !bDRAWFOCUSONLY && ( nNewState & ODS_FOCUS ) ) )
		{

			#define FOCUSOFFSET 3
			CRect rect( rectControl );
			int nDeflate = min( FOCUSOFFSET,
								min( rect.Width(), rect.Height() ) >> 2 );
			rect.DeflateRect( nDeflate, nDeflate);
			pDC->DrawFocusRect(&rect);
		}
	}
	m_nOldAction = nNewAction;
	m_nOldState = nNewState;
}

void CRolloverButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);		
	}	
	if(m_bOverControl == FALSE)
	{
		m_bOverControl=TRUE;
		Redraw();
	}
	CButton::OnMouseMove(nFlags, point);	
}

LRESULT CRolloverButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bOverControl=TRUE;
	Invalidate();
	return 1;
}

LRESULT CRolloverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bOverControl = FALSE;
	Invalidate(FALSE);
	Redraw();
	return 0;
}

void CRolloverButton::OnRButtonDown(UINT nFlags, CPoint point){
	if((m_cHelpTitle == "") || (m_cHelpContent == ""))
	{
		return;
	}
	CPoint mousepoint;
	::GetCursorPos(&mousepoint);
	//CBalloonHelp::LaunchBalloon(m_cHelpTitle, m_cHelpContent, mousepoint, IDI_QUESTION, CBalloonHelp::unCLOSE_ON_RBUTTON_UP|CBalloonHelp::unDISABLE_FADE, NULL, "", 0);
}
