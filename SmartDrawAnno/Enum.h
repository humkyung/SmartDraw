#ifndef __TAG_ENUM_H__
#define __TAG_ENUM_H__

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <string>
using namespace std;

#define	X_DIR		1
#define	Y_DIR		2
#define Z_DIR		3
#define	XY_DIR		4

#define	BOTTOM_SIDE	0
#define	LEFT_SIDE	1
#define	TOP_SIDE	2
#define	RIGHT_SIDE	3

#define	PREFIX_BUF_SIZ	16

typedef enum
{
	MM	= 0x08 + 1,
	CM	= 0x08 + 2,
	M	= 0x08 + 3,
	INCH	= 0x04 + 1,
	FEET	= 0x04 + 2
}UINT_TYPE_E;

typedef struct tagRect
{
	double x,y;		// x,y - corner of rectangle
	double width,height;	// width,height of rectangle
}RECT_T,* PRECT_T;

/// type of entity
typedef enum
{
	NONE		= 0,
	DRAFT_STR	= 1,
	DRAFT_EQP	= 2,
	DRAFT_PIPE	= 3,
	DRAFT_ETIM	= 4,
	DRAFT_DIM	= 5,
	BOP		= 6,
	DRAFT_CABLE	= 7,
	DRAFT_SLIPPER	= 8
}DITEM_E;

typedef enum
{
	VERTICAL   = 1,
	SLOPE      = 2,
	HORIZONTAL = 3,
	CABLE_T    = 4
}SUBITEM_TYPE_E;

typedef enum
{
	LINE_NO_EXTERNAL= 0x01,
	LINE_NO_INTERNAL= 0x02,
	LINE_NO_MIXED	= 0x03
}LINE_NO_TAG_AREA;

// external pipe tag type
typedef enum
{
	SINGLE_LINENO      = 0,
	SINGLE_LINENO_ELEV = 1,
	DOUBLELINE         = 2
}PIPE_TAG_T;

// external pipe elev type
typedef enum
{
	ELEV_CL		= 1,
	ELEV_BOP	= 2,
	ELEV_TOP	= 3,
	ELEV_MIXIN	= 4,
	ELEV_BYSPEC	= 5
}PIPE_ELEV_T;

typedef enum
{
	ONLY_ELEV	= 0x01,
	ELEV_COORD	= 0x02	/// elevation°ú coordinateÀÇ È¥ÇÕÇü
}PIPE_SECOND_TAG_TYPE_T;

typedef enum tagEqpTag
{
	EQP_EXTERNAL	= 0x01,
	EQP_INTERNAL	= 0x02,
	EQP_MIXED		= 0x03
}EQP_TAG_E;

typedef enum
{
	STR_LEFT = 1,
	STR_RIGHT= 2,
	STR_UP   = 4,
	STR_DOWN = 8
}STR_TAG_DIR_E;

typedef enum tagGlobalDim
{
	NONE_GLOBAL_DIMENSION		= 0,
	ALL_GLOBAL_DIMENSION		= 1,
	TOP_LEFT_GLOBAL_DIMENSION	= 2
}GLOBAL_DIMENSION_E;

//->
typedef enum
{
	LINE_NO_NULL	= 0x00,
	MAIN_SIZE	= 0x01,
	LINE_NO		= 0x02,
	SERVICE		= 0x03,
	SPEC		= 0x04,
	AREA		= 0x05,
	INSULATION	= 0x06,
	INSTHICK	= 0x07,
	LINE_NO_DASH	= 0x08,
	LINE_NO_OPEN	= 0x09,
	LINE_NO_CLOSE	= 0x0a,
	LINE_NO_BLANK	= 0x0b,
	UNIT_NO		= 0x0c,
	TRACING		= 0x0d
}LINE_NO_CFG_ENV_E;

typedef enum
{
	NIL_NAME2     = 0,
	FULL_NAME     = 1,
	ONLY_LINE_NUM = 2
}LINE_NO_NAME2_OPT;

typedef enum
{
	OUTSIDE_ALIGN = 1,
	INSIDE_ALIGN  = 2
}TEXT_ALIGN_E;

#endif