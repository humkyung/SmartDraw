// TinyTorusSurfTester.h: interface for the CTinyTorusSurfTester class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYTORUSSURFTESTER_H__B431CC9C_AF42_4FDE_AE4B_A0038AC656E1__INCLUDED_)
#define AFX_TINYTORUSSURFTESTER_H__B431CC9C_AF42_4FDE_AE4B_A0038AC656E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTinyTorusSurfTester  
{
public:
	bool m_bOutput;
	int m_nRows;
	int m_nBoundaries;
	int m_nSegments;
	void OutputTestString();
	CTinyTorusSurfTester(bool bOutput);
	virtual ~CTinyTorusSurfTester();

};

#endif // !defined(AFX_TINYTORUSSURFTESTER_H__B431CC9C_AF42_4FDE_AE4B_A0038AC656E1__INCLUDED_)
