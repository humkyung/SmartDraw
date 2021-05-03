#ifndef	__DGNENUM_H__
#define	__DGNENUM_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

#include <SQLib.h>
#include "..\DGNStruct.h"
#include "..\DGNFloat.h"

// --------------------------------------------------------------------   
//      Element types                                                     
// --------------------------------------------------------------------   
enum{
	DGNT_NONE		=	0,
	DGNT_CELL_LIBRARY	=	1,
	DGNT_CELL_HEADER	=	2,
	DGNT_LINE		=	3,
	DGNT_LINE_STRING	=	4,
	DGNT_GROUP_DATA		=	5,
	DGNT_SHAPE		=	6,
	DGNT_TEXT_NODE		=	7,
	DGNT_DIGITIZER_SETUP	=	8,
	DGNT_TCB		=	9,
	DGNT_LEVEL_SYMBOLOGY	=	10,
	DGNT_CURVE		=	11,
	DGNT_COMPLEX_CHAIN_HEADER=	12,
	DGNT_COMPLEX_SHAPE_HEADER=	14,
	DGNT_ELLIPSE		=	15,
	DGNT_ARC		=	16,
	DGNT_TEXT		=	17,
	DGNT_SURFACE_HEADER	=	18,
	DGNT_3D_SOLID_HEADER	=	19,
	DGNT_B_SPLINE_POLE	=	21,
	DGNT_POINT_STRING	=	22,
	DGNT_CONE		=	23,
	DGNT_B_SPLINE_SURFACE_HEADER=	24,
	DGNT_B_SPLINE_SURFACE_BOUNDARY=	25,
	DGNT_B_SPLINE_KNOT	=	26,
	DGNT_B_SPLINE_WEIGHT_FACTOR=	28,
	DGNT_DIMENSION		=	33,
	DGNT_APPLICATION_ELEM	=	66,
	DGNT_RASTER_HEADER	=	87,
	DGNT_RASTER_DATA	=	88
};

// --------------------------------------------------------------------
//	type of surface
// --------------------------------------------------------------------
enum{
	DGNST_SURF_OF_PROJ	  = 0,
	DGNST_BOUNDED_PLANE	  = 1,
	//DGNST_BOUNDED_PLANE	  = 2,
	DGNST_RIGHT_CIRCULAR_CYLN = 3,
	DGNST_RIGHT_CIRCULAR_CONE = 4,
	DGNST_TABULATED_CYLN	  = 5,
	DGNST_TABULATED_CONE	  = 6,
	DGNST_CONVOLUTE		  = 7,
	DGNST_SURF_OF_REVOLUTION  = 8,
	DGNST_WARPED_SURF	  = 9
};

/////////////////////////////////////////////
typedef struct tagDGNLONG{
	unsigned short hi;
	unsigned short lo;
}DGNLONG,* PDGNLONG;

/////////////////////////////////////////////
// 2d point
typedef struct tagDGNPOINT2D{
	long x;
	long y;
}DGNPOINT2D;

/////////////////////////////////////////////
typedef struct tagDGNHDR{
	ELMHDR	ElmHdr;
	DISPHDR DispHdr;
}DGNHDR,* PDGNHDR;

// Terminal Control Block (header).
//
// The core.stype code is DGNST_TCB.
// Returned for DGNT_TCB(9).
// The first TCB in the file is used to determine the dimension (2D vs. 3D),
// and transformation from UOR (units of resolution) to subunits, and subunits
// to master units.  This is handled transparently within DGNReadElement(), so
// it is not normally necessary to handle this element type at the application
// level, though it can be useful to get the sub_units, and master_units names.
typedef DLL_EXPORT struct {
	DGNElemCore core;
	
	int	dimension;		// Dimension (2 or 3)   
	
	double	origin_x;		// X origin of UOR space in master units(?)  
	double	origin_y;		// Y origin of UOR space in master units(?)  
	double	origin_z;		// Z origin of UOR space in master units(?)  
	
	long	uor_per_subunit;	// UOR per subunit.   
	char	sub_units[3];		// User name for subunits (2 chars)  
	long	subunits_per_master;	// Subunits per master unit.   
	char	master_units[3];	// User name for master units (2 chars)  
}DGNElemTCB;
/////////////////////////////////////////////

#endif
