/********************************************************************
	created:	2004/01/27
	created:	27:1:2004   11:12
	filename: 	d:\Project\ITK\include\Enum.h
	file path:	d:\Project\ITK\include
	file base:	Enum
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef	__ITK_ENUM_H__
#define	__ITK_ENUM_H__

typedef	unsigned char BYTE;
typedef	unsigned long ULONG;

enum{
	ENM_1D = 1,
	ENM_2D = 2,
	ENM_3D = 3
};

//	Element types
//------------------------------------------------------------------------------------
enum{
	//	not defined object.
	ENM_RAW		=	0,
	
	//	element id(1 ~ 20)
	SQ_CELL_LIBRARY	=	1,
	SQ_CELL_HEADER	=	2,
	ENM_POINT	=	3,
	ENM_VERTEX	=	4,
	ENM_LINE	=	5,
	ENM_LINEX	=	6,
	ENM_RECT	=	7,
	ENM_POLYLINE	=	8,
	ENM_POLYGON	=	9,	
	ENM_GROUP	=	10,	
	ENM_REFERENCE	=	11,
	SQ_LOOP		=	12,
	SQ_SHAPE	=	13,
	SQ_TEXT_NODE	=	14,	
	SQ_DIGITIZER_SETUP=	15,
	SQ_TCB		=	16,
	SQ_TEXT		=	17,
	SQ_POINT_STRING	=	18,
	SQ_POINTS	=	19,
	SQ_VERTICES	=	20,
	
	//	curve id(21 ~ 30)
	SQ_CIRCLE	=	21,
	ENM_ELLIPSE	=	22,
	ENM_ARC		=	23,
	SQ_CUBIC_SPLINE	=	24,
	SQ_BEZIER	=	25,
	SQ_BSPLINE	=	26,
	SQ_INTCURVE	=	27,
	SQ_NURBS	=	28,
	
	//	solid id(31 ~ 40)
	SQ_SOLID	=	31,
	SQ_CONE		=	32,
	SQ_ELBOW	=	33,
	
	//	surface id(41 ~ 50)
	SQ_SURFACE	=	41,
	SQ_PLANESURFACE	=	42,
	SQ_CONESURFACE	=	43,
	SQ_SPHERESURFACE=	44,
	SQ_TORUSSURFACE	=	45,
	SQ_NURBS_SURFACE=	46,
	
	SQ_COMPLEX	=	51,
	
	//	complex element id(51 ~ )
	SQ_APPLICATION_ELEM=	66,
	SQ_COLUMN	=	101,

	SQ_LAYER	=	200
};

/** for DXF */
struct is_binary
{ /* Binary data chunk structure */
	short clen; /* length of chunk in bytes */
	char *buf; /* binary data */
};

union is_u_val
{
	double rreal;
	double rpoint[3];
	short rint;
	char *rstring;
	long rlname[2];
	long rlong;   
	struct is_binary rbinary;
/* TEMPORARY probably, for internal use only at the moment */
	unsigned char ihandle[8];
}; 

struct is_resbuf
{
	struct is_resbuf *rbnext; /* Allows them to be "linked" */
	short restype;
	union is_u_val resval;
};

struct Xdata
{
	struct Xdata* _next;
	char* appname;
	is_resbuf* resval;
};

/** */
#endif
