#ifndef __POLY_H__
#define __POLY_H__

#include <graphics/curve3d.h>
#include <list>
using namespace std;

class DLL_EXPORT CPoly : public CCurve3d{
public:
	CPoly();
	virtual ~CPoly();
protected:
	bool m_bClosed;
	
	list<CPoint3d*>* m_plst;
};

#endif
