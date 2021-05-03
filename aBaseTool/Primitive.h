/********************************************************************
	created:	2001/08/03
	created:	3:8:2002   10:32
	filename: 	d:\Project\Sqlib\SQPrimitive.h
	file path:	d:\Project\Sqlib
	file base:	SQPrimitive
	file ext:	h
	author:		baek hum kyung
	
	purpose:	base structure
	update:		rename 'SQPRIMITIVETYPE' to 'SQPRIMITIVEATTR'
			[2003.06.18] - add 'layer'
*********************************************************************/
#ifndef __SQPRIMITIVE_H__
#define __SQPRIMITIVE_H__

#include <ITKDef.h>
#include "include\Enum.h"
#include "SQStruct.h"
#include "SQCurve.h"
#include <graphics/rect.h>
#include <graphics/graphics.h>
#include <graphics/surf.h>
#include "include\Cone.h"
#include "include\Elbow.h"
#include "include\Layer.h"
#include "include\Ref.h"

#include <ITKDisplay.h>
#include <graphics/ITKGeo.h>
#include "include\Attr.h"

#define SQ_NULL			0

#include <list>
#include <vector>
using namespace std;

//	primitive types
//------------------------------------------------------------------------------------
typedef enum{
	PRIMARY_CRV_T = 1,
	COMPLEX_CRV_T = 2,
	RULE_CRV_T    = 3
}PRIMITIVE_T;

typedef struct tagPrimtiveAttr{
	unsigned short u:1;		// exist user data?
	unsigned short f:1;		// is supportting format?
	unsigned short d:2;		// dimension(2 - 2D , 3 - 3D)
	unsigned short h:1;		// hole or solid
	unsigned short reversed:1;
	unsigned short continuous:1;	// for SQ_POINTS.
	unsigned short closed:1;	// for SQ_POLYLINE
	unsigned short del:1;		// is deleted?
	unsigned short r:3;
	unsigned short type:4;		//
}PRIMITIVEATTR_T,* PPRIMITIVEATTR_T;

typedef struct tagSQHeader{
	unsigned char	id;		// type of element
	char*		pLayer;		// layer name which element is in
	PRIMITIVEATTR_T	attr;
	unsigned long	handle;
	SQVOLUME	volume;
	struct{
		short	ltype;			// line type
		short	lweight;		// line weight
		unsigned char lcolor;	// color index
		unsigned char rgb[3];	// color rgb value
		double	lscale;			// line scale
	}display;
	char	szDesc[16];
	POINT_T ptOrigin;			// for group
}SQHEADER,* PSQHEADER;

typedef struct tagSQPrimitive{
	SQHEADER hdr;
	union{
		POINT_T	    		point;
		SQVERTEX		vertex;
		LINE_T      		line;
		LINE_TX			linex;
		RECT_T			rect;
		PPOINT_TNODE 		polyline;
		SQSHAPE			shape;
		TEXT_T			text;
		REFERENCE_T		reference;
		list<SQVERTEX>*		plstVertices;
		vector<POINT_T>*	pvtPoints;

		//--> curve
		CIRCLE_T    		circle;
		ARC_T			arc;
		ELLIPSE_T        	ellipse;
		SQCURVE			curve;
		SQINTCURVE		intcurve;
		SQBSPLINE		bspline;
		PSQNURBS		pNurbs;
		//<--

		CONE_T			cone;
		//SQELBOW			elbow;

		//--> surface
		PSQSURFACE		pSurface;
		PPLANESURF_T		pPlaneSurf;
		PCONESURF_T		pConeSurf;
		PSQSPHERESURFACE	pSphereSurf;
		PSQTORUSSURFACE		pTorusSurf;
		//<--

		PLAYER_T		pLayer;

		struct tagSQPrimitive*	pPrimitive;
		char*			pData;
	}body;
	void* pLinkageData;

	struct tagSQPrimitive *prev,*next;
}SQPRIMITIVE,* PSQPRIMITIVE;

extern PSQPRIMITIVE NewPrimitive();
extern void DeletePrimitive(PSQPRIMITIVE pPrimitive);
extern void InitPrimitive(PSQPRIMITIVE pPrimitive);
extern int  GetPrimitiveType(PSQPRIMITIVE pPrimitive);
extern bool IsSupportedType(PSQPRIMITIVE pPrimitive);
extern bool ExistUserData(PSQPRIMITIVE pPrimitive);
extern PSQPRIMITIVE AppendPrimitive(PSQPRIMITIVE primitive,PSQPRIMITIVE append);
extern "C" __declspec(dllexport) PPOINT_TNODE  PolylinePush(PPOINT_TNODE polyline,PPOINT_T point);
extern PSQPRIMITIVE ClippingPrimitiveByVolume(PSQPRIMITIVE pDest,PSQPRIMITIVE pSrc,PVOLUME_T pVolume);

#endif
