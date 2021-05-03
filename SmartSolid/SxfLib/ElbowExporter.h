// ElbowExporter.h: interface for the CElbowExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELBOWEXPORTER_H__4644974C_42D7_4FA5_95EE_306856ABA558__INCLUDED_)
#define AFX_ELBOWEXPORTER_H__4644974C_42D7_4FA5_95EE_306856ABA558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>

class CElbowExporter  
{
public:
	void WriteElbow(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup , const int& nSkipLevel);
	CElbowExporter();
	virtual ~CElbowExporter();

private:
	void WriteSweepElbow(ofstream& ofile , PSQPRIMITIVE pPrimitive , long &nSeq, long &nGroup , const int& iSkipLevel);
	bool IsSweepSurf(PSQPRIMITIVE pPrimitive) const;
	bool IsInsulationElbow(PSQPRIMITIVE pElement);
};

#endif // !defined(AFX_ELBOWEXPORTER_H__4644974C_42D7_4FA5_95EE_306856ABA558__INCLUDED_)
