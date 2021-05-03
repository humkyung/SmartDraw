#ifndef __HSR_FACE_H__
#define __HSR_FACE_H__

#pragma warning(push)
#include <math.h>
#pragma warning(disable: 4514)
#pragma warning(pop)

#include <graphics\plane3d.h>
#include <IsVolume.h>
#include "Enum.h"
#include "HSRPoint.h"
#include "HSRFace.h"
#include "HSR.h"

#define	LAYER_NAME_LEN	32

class CHSRViewFormat;
class CHSRElement;
//	description	: hidden setted indicate all lines are hiddend
//	remarks		:
typedef class CHSRFace
{
	CHSRFace(const CHSRFace&){}
	CHSRFace& operator=(const CHSRFace&){return (*this);}
public:
	CHSRFace();
	virtual ~CHSRFace();
public:	
	CIsVolume GetRange() const;			/// 2012.10.18 added by humkyung

	STRING_T color() const;				/// 2012.02.08 added by humkyung
	int SetColor(const STRING_T& sColor);	/// 2012.02.08 added by humkyung

	bool SplitFaceLine(CHSRFace* pFace,bool coplane);
	bool SplitLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool selfsplit = false);
	
	bool IsHiddenLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool coplane = false);
	void ExtractHiddenLine(CHSRFace* pDestFace,bool coplane = false);
	bool CalcPlaneEquation();
	static bool CalcPlaneEquationOf(PLANE_T& plane , const int& nCount , POINT_T* ppts);

	bool IsInnerVertex(PPOINT_T pt,bool coplane = false);
	void UnionFaces();
	
	bool IsPerpendicularToViewPoint();
	bool IsParallelToViewPoint();

	bool UnionFace(CHSRFace* pSibling);
	PHSR_VERTEX SplitEdge(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool coplane);
	
	static PHSR_VERTEX CreateVertexList(const int& nCount,POINT_T ptEDGE[] , const bool& merge = true);
	static CHSRFace* CreateFace(const int nCount , POINT_T ptEDGE[] , const bool& mergeEdge = true);
	static int CreateCirclePts(POINT_T pts[] , const POINT_T& ptOrigin,const double& nRadius,const VECTOR_T& vecNorm,const int& nSegments);
	static CHSRFace* CreateCircleFace(POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm,const int& nSegments);
	static PHSR_VERTEX CreateLine(const int nCount,POINT_T ptEDGE[]);
public:
	PHSR_VERTEX HasEdge(const POINT_T& pt1 , const POINT_T& pt2);
	HSR_LAYER_E m_enmLayer;
	HSR_PRIMITIVE_E type;
	unsigned char ele;
	unsigned long id,region;

	CHSRElement* body;	/// body id

	int       nCount; // number of vertices.
	POINT_T   ptCenter,ptCross,ptEdge[2],ptMark[2];
	double    radius;
	bool	  mark[2],symbol[2],hidden;
	PLANE_T   Plane;
	VOLUME_T volume;

	PHSR_VERTEX  pHead,pCenterline,pCrossline,pSymboline;
	CHSRFace*   pSibling;

	CHSRFace *prev,*next;
	CHSRFace *front,*back;
private:
	STRING_T m_sColor;	/// color - 2012.02.08 added by humkyung
}FACE,*PFACE;

typedef struct tagRenderNode
{
	CHSRFace* pFace;
	CHSRFace* pTail;

	tagRenderNode *prev,*next;
}RENDERNODE,* PRENDERNODE;

#endif

