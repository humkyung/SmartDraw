// Leader.h: interface for the CLeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEADER_H__2C4E49F8_B93D_456E_B5C0_749979C5ABC6__INCLUDED_)
#define AFX_LEADER_H__2C4E49F8_B93D_456E_B5C0_749979C5ABC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartDrawAnnoEntity.h"
#include "Dimension.h"

#include <fstream>
using namespace std;

class CExtDivision;
class CDraftLeader : public CaDraw_TagEntity
{
public:
	typedef struct
	{
		TCHAR szLineStyle[16];
		TCHAR szLineColor[16];
		TCHAR szLineWeight[16];
	}LEADER_ENV_T;
	
	CDraftLeader(CaDraw_TagItem* pTITEM);
	virtual ~CDraftLeader();
public:
	void Write(OFSTREAM_T& file,CExtDivision* pDivision);

	static void ReadEnv(const TCHAR* pszFilePath);
public:
	CaDraw_TagItem* m_pTITEM;
	CIsPoint3d m_ptOrigin;

	static LEADER_ENV_T ENV;
};

#endif // !defined(AFX_LEADER_H__2C4E49F8_B93D_456E_B5C0_749979C5ABC6__INCLUDED_)
