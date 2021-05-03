// HSRBoundingBox.h: interface for the CHSRBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRBOUNDINGBOX_H__252D1569_D2AF_40CD_9771_1F42D25FC0E7__INCLUDED_)
#define AFX_HSRBOUNDINGBOX_H__252D1569_D2AF_40CD_9771_1F42D25FC0E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHSRBoundingBox  
{
public:
	enum{
		X = 0,
		Y = 1,
		Z = 2
	};
public:
	bool Collide(const CHSRBoundingBox &bbox);
	CHSRBoundingBox(const CHSRBoundingBox& bbox);
	void SetEmpty();
	bool IsEmpty();
	CHSRBoundingBox(const double minx,const double miny,const double minz,const double maxx,const double maxy,const double maxz);
	const double GetMax(const int nIndex);
	const double GetMin(const int nIndex);
	bool operator +=(CHSRBoundingBox &bbox);
	CHSRBoundingBox();
	virtual ~CHSRBoundingBox();
private:
	bool m_bEmpty;
	
	double m_nMins[3];
	double m_nMaxs[3];
};

#endif // !defined(AFX_HSRBOUNDINGBOX_H__252D1569_D2AF_40CD_9771_1F42D25FC0E7__INCLUDED_)
