// MESHElement.h: interface for the CMESHElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHELEMENT_H__26E106DB_5619_4223_A3DA_E9D8B891FEE0__INCLUDED_)
#define AFX_MESHELEMENT_H__26E106DB_5619_4223_A3DA_E9D8B891FEE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "Element.h"

#define	MAX_MESH_ELEMENT	10

class CHSRReader;
class CMeshElement : public CHSRElement  
{
public:
	CMeshElement();
	virtual ~CMeshElement();
public:
	bool ParseLine(CHSRScanner* pPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CMeshElement);
private:
	int m_nRow,m_nCol;
	POINT_T m_pt[MAX_MESH_ELEMENT][MAX_MESH_ELEMENT];
};

#endif // !defined(AFX_MESHELEMENT_H__26E106DB_5619_4223_A3DA_E9D8B891FEE0__INCLUDED_)
