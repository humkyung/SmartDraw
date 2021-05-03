/********************************************************************
	created:	2001/08/03
	created:	3:8:2001   10:40
	filename: 	d:\Project\Sqlib\SQStruct.h
	file path:	d:\Project\Sqlib
	file base:	SQStruct
	file ext:	h
	author:		
	
	purpose:	
	update:		rename 'SQPOLYLINE' to 'SQPOINTNODE'
		2002.09.12 - add normal vector to SQTEXT structure
*********************************************************************/

#ifndef __SQSTRUCT_H__
#define __SQSTRUCT_H__

#include <graphics\point.h>
#include <math\vect.h>
#include "include\Text.h"
#include "include\Ref.h"

typedef char* PNAME;
struct tagSQPrimitive;

// 3-by-3 matrix
typedef struct tagSQMatrix{
	double element[3][3];
}SQMATRIX,* PSQMATRIX;

typedef struct tagSQVertex
{
    	struct tagCode
	{
		unsigned char visit : 1;
		unsigned char moveto: 1;
		unsigned char r     : 6;
	}code;	
    	double x,y,z;
}SQVERTEX,* PSQVERTEX;

typedef struct tagSQPointNode
{
	POINT_T pt;
	bool visible;
	struct tagSQPointNode* next;
}SQPOINTNODE,* PPOINT_TNODE;

typedef struct tagSQShape{
	unsigned short  fcolor;	// fill color
	struct _list{
		POINT_T point;
		struct _list* next;
	}* list;
}SQSHAPE,* PSQSHAPE;
#endif
