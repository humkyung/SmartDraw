#ifndef __KBEZIER_H__
#define __KBEZIER_H__

#include "KCurve.h"

class KBezier : public KCurve{
public:
	KBezier();
	KBezier(int nCount,SQPOINT ptControl[]);
	KBezier(const SQCURVE& bezier);
	KBezier(const KBezier& bezier);
	virtual ~KBezier();
public:
	int GetNumOfCtrlPoints(){return m_crvBezier.nPoints;}
	int GetNumOfDegree(){return m_crvBezier.nDegree;}
	SQPOINT GetCtrlPoint(int nIndex){return m_crvBezier.ppt[nIndex];}
	void ElevateDegree();
	KBezier* SubDivideAt(const double t);
	SQPOINT operator()(double t);
protected:
	void CreateSegments(){};
	void OnDestroy();

	void operator=(const KBezier& bezier);
protected:
	SQCURVE m_crvBezier;
};

#endif
