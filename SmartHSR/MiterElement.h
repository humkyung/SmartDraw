// MiterElement.h: interface for the CMiterElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MITERELEMENT_H__244457DC_7114_4B4D_94B5_E1B3AC0D877E__INCLUDED_)
#define AFX_MITERELEMENT_H__244457DC_7114_4B4D_94B5_E1B3AC0D877E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"
#include <vector>
using namespace std;

class CMiterElement : public CHSRElement  
{
	typedef struct
	{
		double  radius;
		POINT_T pt[2];
	}CYLINDER_T;
	
	typedef struct
	{
		CHSRFace* pFace;
		POINT_T* pts[4];
		bool render[4];
	}Facet;
public:
	CMiterElement();
	virtual ~CMiterElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);

	DECLARE_HSR_ELEMENT(CMiterElement);
private:
	int CreateCylinderFace(vector<POINT_T>& new_pts , CYLINDER_T cylinder , vector<POINT_T>& pts , CPlane3d& joinplane3d , const int& nID);
	int RemoveCoEdge(Facet* pFacet1 , Facet* pFacet2);
	bool IsEdgeLine(vector<POINT_T>& pts , POINT_T* pt1 , POINT_T* pt2);
	bool IsPositivePlaneOnView(CPlane3d& plane3d);
	int GetIntersectionOf(POINT_T& res , CPlane3d& plane , const POINT_T& pt1 , const POINT_T& pt2);
	int CreateJoinPlane(CPlane3d &plane , POINT_T& pt1 , POINT_T& pt2 , POINT_T& pt3);
	void Volume();
	bool CheckConnection(int conn[2] , const CYLINDER_T& lhs , const CYLINDER_T& rhs , int desconn[2]);
	void AlignCylinder();
	bool CreateHorizontalFace(vector<CYLINDER_T>& CylinderEntry , const long& nID);
	bool CreateVertFace(vector<CYLINDER_T>& CylinderEntry , const long& nID);
	void ClipAxisByElevation(POINT_T &ptStart, POINT_T& ptEnd, bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , const double& zhigh , const double& zlow);
	vector<CYLINDER_T> m_CylinderEntry;
	HSR_PRIMITIVE_E m_eType;
};

#endif // !defined(AFX_MITERELEMENT_H__244457DC_7114_4B4D_94B5_E1B3AC0D877E__INCLUDED_)
