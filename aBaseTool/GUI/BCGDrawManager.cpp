//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2000 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
 //*******************************************************************************
// BCGDrawManager.cpp: implementation of the CBCGDrawManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "globals.h"
#include "BCGDrawManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGDrawManager::CBCGDrawManager(CDC& m_dc) :
	m_dc (m_dc)
{
}
//*************************************************************************************
CBCGDrawManager::~CBCGDrawManager()
{
}
//*************************************************************************************
BOOL CBCGDrawManager::HighlightRect (CRect rect, int nPercentage, COLORREF clrTransparent)
{
	if (nPercentage == 100)
	{
		// Nothing to do
		return TRUE;
	}

	if (globalData.m_nBitsPerPixel <= 8)
	{
		CBCGToolBarImages::FillDitheredRect (&m_dc, rect);
		return TRUE;
	}

	int cx = rect.Width ();
	int cy = rect.Height ();

	//--------------------------------------------
	// Copy screen content into the memory bitmap:
	//--------------------------------------------
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (&m_dc))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	//--------------------------------------------
	// Gets the whole menu and changes the shadow.
	//--------------------------------------------
	CBitmap	bmpMem;
	if (!bmpMem.CreateCompatibleBitmap (&m_dc, cx, cy))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);
	ASSERT (pOldBmp != NULL);

	LPBITMAPINFO lpbi;

	// Fill in the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbi->bmiHeader.biWidth = cx;
	lpbi->bmiHeader.biHeight = cy;
	lpbi->bmiHeader.biPlanes = 1;
	lpbi->bmiHeader.biBitCount = 32;
	lpbi->bmiHeader.biCompression = BI_RGB;
	lpbi->bmiHeader.biSizeImage = cx * cy;
	lpbi->bmiHeader.biXPelsPerMeter = 0;
	lpbi->bmiHeader.biYPelsPerMeter = 0;
	lpbi->bmiHeader.biClrUsed = 0;
	lpbi->bmiHeader.biClrImportant = 0;

	COLORREF* pBits = NULL;
	HBITMAP hmbpDib = CreateDIBSection (
		dcMem.m_hDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
		NULL, NULL);

	if (hmbpDib == NULL || pBits == NULL)
	{
		delete lpbi;
		ASSERT (FALSE);
		return FALSE;
	}

	dcMem.SelectObject (hmbpDib);
	dcMem.BitBlt (0, 0, cx, cy, &m_dc, rect.left, rect.top, SRCCOPY);

	for (int pixel = 0; pixel < cx * cy; pixel++, *pBits++)
	{
		COLORREF color = (COLORREF) *pBits;
		if (color != clrTransparent)
		{
			if (nPercentage == -1)
			{
				//-------------------
				// By Maarten Hoeben:
				//-------------------
				*pBits = RGB (
					GetRValue (color) + ((GetRValue (globalData.clrBtnHilite) -
						GetRValue (color)) / 2 ),
					GetGValue (color) + ((GetGValue(globalData.clrBtnHilite) -
						GetGValue (color)) / 2),
					GetBValue(color) + ((GetBValue(globalData.clrBtnHilite) -
						GetBValue (color)) / 2));
			}
			else
			{
				*pBits = PixelAlpha (*pBits, nPercentage);
			}
		}
	}

	//-------------------------------------------
	// Copy highligted bitmap back to the screen:
	//-------------------------------------------
	m_dc.BitBlt (rect.left, rect.top, cx, cy, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject (pOldBmp);
	DeleteObject (hmbpDib);
	delete lpbi;

	return TRUE;
}
//*************************************************************************************
void CBCGDrawManager::FillGradient (CRect rect, 
									COLORREF colorStart, COLORREF colorFinish, 
									BOOL bHorz/* = TRUE*/)
// Copyright (C) 1998, 1999 by Cristi Posea
// All rights reserved
{
    // this will make 2^6 = 64 fountain steps
    int nShift = 6;
    int nSteps = 1 << nShift;

    for (int i = 0; i < nSteps; i++)
    {
        // do a little alpha blending
        BYTE bR = (BYTE) ((GetRValue(colorStart) * (nSteps - i) +
                   GetRValue(colorFinish) * i) >> nShift);
        BYTE bG = (BYTE) ((GetGValue(colorStart) * (nSteps - i) +
                   GetGValue(colorFinish) * i) >> nShift);
        BYTE bB = (BYTE) ((GetBValue(colorStart) * (nSteps - i) +
                   GetBValue(colorFinish) * i) >> nShift);

		CBrush br (RGB(bR, bG, bB));

        // then paint with the resulting color
        CRect r2 = rect;
        if (bHorz)
        {
            r2.bottom = rect.bottom - 
                ((i * rect.Height()) >> nShift);
            r2.top = rect.bottom - 
                (((i + 1) * rect.Height()) >> nShift);
            if (r2.Height() > 0)
                m_dc.FillRect(r2, &br);
        }
        else
        {
            r2.left = rect.left + 
                ((i * rect.Width()) >> nShift);
            r2.right = rect.left + 
                (((i + 1) * rect.Width()) >> nShift);
            if (r2.Width() > 0)
                m_dc.FillRect(r2, &br);
        }
    }
}
//*************************************************************************************
BOOL CBCGDrawManager::DrawShadow (CRect rect, int nDepth,
								  int iMinBrightness, int iMaxBrightness,
								  CBitmap* pBmpSaveBottom,
								  CBitmap* pBmpSaveRight)
// ==================================================================
// 
// FUNCTION :  DrawShadows ()
// 
// * Description : Draws the shadow for a rectangular screen element
// 
// * Authors: [Stas Levin ]
//			  [Timo Hummel], Modified: [8/11/99 5:06:59 PM]
//			  
// * Function parameters : 
// [rect] -		The CRect of the rectangular region to draw the
//			    shadow around (altough the CDC needs to be big enough
//				to hold the shadow)
// ==================================================================
{
	ASSERT (nDepth >= 0);

	if (nDepth == 0 || rect.IsRectEmpty ())
	{
		return TRUE;
	}

	int cx = rect.Width ();
	int cy = rect.Height ();

	if (pBmpSaveRight != NULL && pBmpSaveRight->GetSafeHandle () != NULL &&
		pBmpSaveBottom != NULL && pBmpSaveBottom->GetSafeHandle () != NULL)
	{
		//---------------------------------------------------
		// Shadows are already implemented, put them directly
		// to the DC:
		//---------------------------------------------------
		m_dc.DrawState (CPoint (rect.right, rect.top), 
					CSize (nDepth, cy + nDepth),
					pBmpSaveRight, DSS_NORMAL);

		m_dc.DrawState (CPoint (rect.left, rect.bottom),
					CSize (cx + nDepth, nDepth),
					pBmpSaveBottom, DSS_NORMAL);
		return TRUE;
	}

	ASSERT (pBmpSaveRight == NULL || pBmpSaveRight->GetSafeHandle () == NULL);
	ASSERT (pBmpSaveBottom == NULL || pBmpSaveBottom->GetSafeHandle () == NULL);

	//--------------------------------------------
	// Copy screen content into the memory bitmap:
	//--------------------------------------------
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC (&m_dc))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	//--------------------------------------------
	// Gets the whole menu and changes the shadow.
	//--------------------------------------------
	CBitmap	bmpMem;
	if (!bmpMem.CreateCompatibleBitmap (&m_dc, cx + nDepth, cy + nDepth))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);
	ASSERT (pOldBmp != NULL);

	LPBITMAPINFO lpbi;

	// Fill in the BITMAPINFOHEADER
	lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) ];
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbi->bmiHeader.biWidth = cx + nDepth;
	lpbi->bmiHeader.biHeight = cy + nDepth;
	lpbi->bmiHeader.biPlanes = 1;
	lpbi->bmiHeader.biBitCount = 32;
	lpbi->bmiHeader.biCompression = BI_RGB;
	lpbi->bmiHeader.biSizeImage = (cx + nDepth) * (cy + nDepth);
	lpbi->bmiHeader.biXPelsPerMeter = 0;
	lpbi->bmiHeader.biYPelsPerMeter = 0;
	lpbi->bmiHeader.biClrUsed = 0;
	lpbi->bmiHeader.biClrImportant = 0;

	COLORREF* pBits = NULL;
	HBITMAP hmbpDib = CreateDIBSection (
		dcMem.m_hDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
		NULL, NULL);

	if (hmbpDib == NULL || pBits == NULL)
	{
		delete lpbi;
		ASSERT (FALSE);
		return FALSE;
	}

	dcMem.SelectObject (hmbpDib);
	dcMem.BitBlt (0, 0, cx + nDepth, cy + nDepth, &m_dc, rect.left, rect.top, SRCCOPY);

	//----------------------------------------------------------------------------
	// Process shadowing:
	// For having a very nice shadow effect, its actually hard work. Currently,
	// I'm using a more or less "hardcoded" way to set the shadows (by using a
	// hardcoded algorythm):
	//
	// This algorythm works as follows:
	// 
	// It always draws a few lines, from left to bottom, from bottom to right,
	// from right to up, and from up to left). It does this for the specified
	// shadow width and the color settings.
	//-----------------------------------------------------------------------------

	// For speeding up things, iShadowOffset is the
	// value which is needed to multiply for each shadow step
	int iShadowOffset = (iMaxBrightness - iMinBrightness) / nDepth;

	// Loop for drawing the shadow
	// Actually, this was simpler to implement than I thought
	for (int c = 0; c < nDepth; c++)
	{
		// Draw the shadow from left to bottom
		for (int y = cy; y < cy + (nDepth - c); y++)
		{
			SetAlphaPixel (pBits, rect, c + nDepth, y, 
				iMaxBrightness - ((nDepth  - c) * (iShadowOffset)),nDepth);
		}

		// Draw the shadow from left to right
		for (int x = nDepth + (nDepth - c); x < cx + c; x++)
		{
			SetAlphaPixel(pBits, rect,x, cy + c,
				iMaxBrightness - ((c) * (iShadowOffset)),nDepth);
		}

		// Draw the shadow from top to bottom
		for (int y1 = nDepth + (nDepth - c); y1 < cy + c + 1; y1++)
		{
			SetAlphaPixel(pBits, rect, cx+c, y1, 
				iMaxBrightness - ((c) * (iShadowOffset)),
				nDepth);
		}
		
		// Draw the shadow from top to left
		for (int x1 = cx; x1 < cx + (nDepth - c); x1++)
		{
			SetAlphaPixel (pBits, rect, x1, c + nDepth,
				iMaxBrightness - ((nDepth - c) * (iShadowOffset)),
				nDepth);
		}
	}

	//-----------------------------------------
	// Copy shadowed bitmap back to the screen:
	//-----------------------------------------
	m_dc.BitBlt (rect.left, rect.top, cx + nDepth, cy + nDepth, 
				&dcMem, 0, 0, SRCCOPY);

	//------------------------------------
	// Save shadows in the memory bitmaps:
	//------------------------------------
	if (pBmpSaveRight != NULL)
	{
		pBmpSaveRight->CreateCompatibleBitmap (&m_dc, nDepth + 1, cy + nDepth);
		
		dcMem.SelectObject (pBmpSaveRight);
		dcMem.BitBlt (0, 0, nDepth, cy + nDepth,
						&m_dc, rect.right, rect.top, SRCCOPY);
	}

	if (pBmpSaveBottom != NULL)
	{
		pBmpSaveBottom->CreateCompatibleBitmap (&m_dc, cx + nDepth, nDepth + 1);

		dcMem.SelectObject (pBmpSaveBottom);
		dcMem.BitBlt (0, 0, cx + nDepth, nDepth,
						&m_dc, rect.left, rect.bottom, SRCCOPY);
	}

	dcMem.SelectObject (pOldBmp);
	DeleteObject (hmbpDib);
	delete lpbi;

	return TRUE;
}

// ==================================================================
// 
// FUNCTION :  static void SetAlphaPixel ()
// 
// * Description : Draws an alpha blended pixel
// 
// * Author : [Timo Hummel], Created : [8/11/99 5:04:38 PM]
// 
// * Function parameters : 
// [pBits] -	The DIB bits
// [x] -		X-Coordinate
// [y] -		Y-Coordinate
// [percent] -	Percentage to blit (100 = hollow, 0 = solid)
// 
// ==================================================================
inline void CBCGDrawManager::SetAlphaPixel (COLORREF* pBits, CRect rect, int x, int y, int percent, int m_iShadowSize)
{
	// Our direct bitmap access swapped the y coordinate...
	y = (rect.Height()+m_iShadowSize)- y;

	COLORREF* pColor = (COLORREF*) (pBits + (rect.Width () + m_iShadowSize) * y + x);
	*pColor = PixelAlpha (*pColor, percent);
}

// ==================================================================
// 
// FUNCTION :  PixelAlpha ()
// 
// * Description : Shades a color value with a specified percentage
// 
// * Author : [Timo Hummel], Created : [8/11/99 2:37:04 PM]
// 
// * Returns : [COLORREF] - The result pixel
// 
// * Function parameters : 
// [srcPixel] - The source pixel
// [percent] -  Percentage (amount of shadow)
//
// Example: percent = 10    makes the pixel around 10 times darker
//          percent = 50    makes the pixel around 2 times darker
// 
// ==================================================================
COLORREF CBCGDrawManager::PixelAlpha (COLORREF srcPixel, int percent)
{
	// My formula for calculating the transpareny is as
	// follows (for each single color):
	//
	//							   percent
	// destPixel = sourcePixel * ( ------- )
	//                               100
	//
	// This is not real alpha blending, as it only modifies the brightness,
	// but not the color (a real alpha blending had to mix the source and
	// destination pixels, e.g. mixing green and red makes yellow).
	// For our nice "menu" shadows its good enough.

	COLORREF clrFinal = RGB ( min (255, (GetRValue (srcPixel) * percent) / 100), 
							  min (255, (GetGValue (srcPixel) * percent) / 100), 
							  min (255, (GetBValue (srcPixel) * percent) / 100));

	return (clrFinal);

}

static inline int AdjustChannel (int nValue, double nPercent)
{
	int nNewValue = (int) (.5 + nPercent * nValue);
	if (nValue == 0 && nPercent > 1.)
	{
		nNewValue = (int) (.5 + (nPercent - 1.) * 255);
	}

	return min (nNewValue, 255);
}

COLORREF CBCGDrawManager::PixelAlpha (COLORREF srcPixel, double percentR, double percentG, double percentB)
{
	COLORREF clrFinal = RGB ( AdjustChannel (GetRValue (srcPixel), percentR), 
							  AdjustChannel (GetGValue (srcPixel), percentG), 
							  AdjustChannel (GetBValue (srcPixel), percentB));

	return (clrFinal);

}

// ==================================================================
// 
// FUNCTION :  PixelAlpha ()
// 
// * Description : Shades a color value with a specified percentage
// 
// * Author : [Guillaume Nodet]
// 
// * Returns : [COLORREF] - The result pixel
// 
// * Function parameters : 
// [srcPixel] - The source pixel
// [dstPixel] - The destination pixel
// [percent] -  Percentage (amount of shadow)
//
// ==================================================================
COLORREF CBCGDrawManager::PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent)
{
	int ipercent = 100 - percent;
	COLORREF clrFinal = RGB ( (GetRValue (srcPixel) * percent + GetRValue (dstPixel) * ipercent) / 100, 
							  (GetGValue (srcPixel) * percent + GetGValue (dstPixel) * ipercent) / 100, 
							  (GetBValue (srcPixel) * percent + GetBValue (dstPixel) * ipercent) / 100);

	return (clrFinal);

}
