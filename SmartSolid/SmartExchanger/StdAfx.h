// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9B9C3923_A58D_486B_8F1E_90D7D7EBC9A3__INCLUDED_)
#define AFX_STDAFX_H__9B9C3923_A58D_486B_8F1E_90D7D7EBC9A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <SQLib.h>

extern void SxfExportFile(const TCHAR* pFilePath , const int& iModelType , PSQPRIMITIVE pPrimitive , const int& nSkipLevel , 
						  const string& sVer , const string& sLastWriteTime , const double&,const double&,const double&);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9B9C3923_A58D_486B_8F1E_90D7D7EBC9A3__INCLUDED_)
