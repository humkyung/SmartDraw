// KRule.h: interface for the KRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRULE_H__5558AB8E_652B_4ECD_8EE1_49292A074AB0__INCLUDED_)
#define AFX_KRULE_H__5558AB8E_652B_4ECD_8EE1_49292A074AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <..\Primitive.h>
#include "HSRFace.h"

class CHSRRule
{
	CHSRRule& operator=(const CHSRRule&){}
	CHSRRule(const CHSRRule&){}
public:
	CHSRRule();
	~CHSRRule();
public:
	PSQPRIMITIVE Rule(PSQPRIMITIVE ret,CHSRFace* face);
private:
	PSQPRIMITIVE AppendWeldElbowMark(PSQPRIMITIVE ret,CHSRFace* pFace);
	PSQPRIMITIVE RunForSWElbow(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace);
	PSQPRIMITIVE RunForElbow(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace);
	PSQPRIMITIVE RunForTee(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace);
	PSQPRIMITIVE RunForTeeX(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace);
	PSQPRIMITIVE AppendSWElbowMark(PSQPRIMITIVE ret,CHSRFace* pFace);
};

#endif // !defined(AFX_KRULE_H__5558AB8E_652B_4ECD_8EE1_49292A074AB0__INCLUDED_)
