#ifndef __HSR_H__
#define __HSR_H__

#include <SQLib.h>

// HSR primitive id
typedef enum
{
	NO_PRIMITIVE= 0,
	CYLINDER    = 1,
	BOX         = 2,
	ELBOW       = 3,
	CONE        = 4,
	TORUS       = 5,
	PIPE        = 6,
	TEE         = 7,
	FLANGE      = 8,
	DOME        = 9,
	SW_ELBOW    = 10,
	EQP_CYLINDER= 11,
	EQP_CONE    = 12,
	PIPEX       = 13,
	TEEX        = 14,
	PIPE_BEND   = 15,
	FACE_T      = 16,
	HFACE_T	    = 17,
	CTOR	    = 18,	// circular torus
	RTOR	    = 19,	// rectangular torus
	SCTN	    = 20,
	EQP         = 21,	// equipment type
	STRU        = 22,	// structure type
	OTHER       = 23,	// other type
	MITER	    = 24,
}HSR_PRIMITIVE_E;

typedef enum
{
	SIDEFACE   = 0x04,
	TOPFACE    = 0x05,
	BOTTOMFACE = 0x06,
	SIDEFACEX  = 0x07,
	TOPFACEX   = 0x08,
	BOTTOMFACEX= 0x09,
	SYMBOL	   = 0x0a,
	SECTION    = 0x0b,
	NORMAL     = 0x0c
}HSR_ELEMENT_T;

typedef enum
{
	HSR_RECTANGLE_SHAPE = 0x10,
	HSR_CIRCLE_SHAPE    = 0x20,
	HSR_ARC_SHAPE       = 0x30,
	HSR_POLYGON_SHAPE   = 0x40
}HSR_SHAPE_T;

class CHSRElement;
//	pt	: pointer to the point
//	render	: visible or in-visible flag
//	id	:
typedef struct tagVertex
{
	PPOINT_T   pt;
	int        render;
	int        id;
	
	CHSRElement* hider;	/// body id which hides this face

	struct tagVertex *prev,*next;
}HSR_VERTEX,*PHSR_VERTEX;

#define HSR_TOLER 0.000000001
#define FSIGN(f) (((f) < -HSR_TOLER) ? NEGATIVE : ((f) > HSR_TOLER ? POSITIVE : ZERO))
#define ISEQUAL(a,b) ((fabs((double)(a)-(b)) >= (double)HSR_TOLER) ? 0 : 1)
#define ISADJACENT(a,b) ((fabs((double)(a) - (b)) >= (double)HSR_TOLER) ? 0 : 1)

#define	IS_EQUAL_VERTEX(vt1,vt2) \
	(ISEQUAL((vt1)->pt->x,(vt2)->pt->x) && \
	 ISEQUAL((vt1)->pt->y,(vt2)->pt->y) && \
	 ISEQUAL((vt1)->pt->z,(vt2)->pt->z))

#define ISADJACENTVERTEX(v1,v2) \
	(ISADJACENT((v1)->pt->x,(v2)->pt->x) || \
	ISADJACENT((v1)->pt->y,(v2)->pt->y))

///#define MAXINT (0xffffffff)

#define HSR_ELEMENT_MARK(e)	((e) & 0x0f)
#define HSR_SHAPE_MARK(e)	((e) & 0xf0)

#endif
