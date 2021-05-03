#ifndef __DGNSTRUCT_H__
#define __DGNSTRUCT_H__

#include "DGNDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DGN_DEBUG

//-----ELEMENT HEADER-----
typedef struct
{
	unsigned short	level:6;	// level element is on   
	unsigned short  r:1;            // reserved   
	unsigned short  complex:1;      // component of complex elem.  
	unsigned short 	type:7;         // type of element   
	unsigned short  deleted:1;      // set if element is deleted   
	unsigned short  words;          // words to follow in element   
	unsigned long   xlow;           // element range - low   
	unsigned long   ylow;
	unsigned long   zlow;
	unsigned long   xhigh;          // element range - high   
	unsigned long   yhigh;
	unsigned long   zhigh;
}ELMHDR,* PELMHDR;

typedef struct{
	unsigned short	style:3;	// line style   
	unsigned short	weight:5;	// line weight   
	unsigned short	color:8;	// color   
}Symbology;				// element symbology word 652   

typedef struct{
	unsigned short	gphgrp;                            
	short		attindx;   	  
	
	union{
		short s;
		struct{
			unsigned short	  _class:4;
			unsigned short    res:4;        
			unsigned short    l:1;       
			unsigned short    n:1;       
			unsigned short    m:1;       
			unsigned short    a:1;       
			unsigned short    r:1;       
			unsigned short    p:1;       
			unsigned short    s:1;       
			unsigned short    h:1;   
		}b;
	} props;
	
	union{
		short		s;
		Symbology	b;
	} symb;
}DISPHDR,* PDISPHDR;

//-----ELEMENET CIRE WHEN READING-----
typedef struct {
	unsigned int offset;
	unsigned int size;

	int	g_cnt;
	int	element_id;	// Element number    
	int	stype;		// Structure type: (DGNST_*)   
	int	level;		// Element Level: 0-63   
	int	type;		// Element type (DGNT_)   
	int	complex;	// Is element complex?   
	int	deleted;	//Is element deleted?   

	int	graphic_group;	// Graphic group number   
	int	properties;	// Properties: ORing of DGNP_ flags   
	int	color;		// Color index (0-255)   
	int	weight;		// Line Weight (0-31)   
	int	style;		// Line Style: One of DGNS_* values   

	int		attr_bytes;	//Bytes of attribute data, usually zero.   
	unsigned char	*attr_data;	// Raw attribute data 
}DGNElemCore;
  
// POINT COORSINATES DOUBLE TYPE ====================================================
//-----Reference file attachment element (type 5, level 9)-----
typedef struct{
	unsigned multi_attach:1;	// multi-attach   
	unsigned one_one_map:1;         // 1:1 mapping   
	unsigned slot_in_use:1;         // slot in use   
	unsigned upd_fildgn:1;          // update on file design   
	unsigned db_diff_mf:1;          // database dif than mas file   
	unsigned snap_lock:1;           // snap lock   
	unsigned locate_lock:1;         // locate lock   
	unsigned missing_file:1;        // missing file   
	unsigned unused:8;              // unused   
}Fb_opts;
                           
typedef struct{
	unsigned view_ovr:1;// view override   
	unsigned display:1;// display   
	unsigned line_width:1;// lines with width   
	unsigned unused:13;// unused   
}Fd_opts;
                                          
// Named view element (type 5, level 3) ==================================================
typedef struct{
	unsigned       fast_curve:1         ;               // fast curve display   
	unsigned       fast_text:1          ;               // fast text   
	unsigned       fast_font:1          ;               // fast font text   
	unsigned       line_wghts:1         ;               // line weights   
	unsigned       patterns:1           ;               // pattern display   
	unsigned       text_nodes:1         ;               // text node display   
	unsigned       ed_fields:1          ;               // enter data field underlines   
	unsigned       on_off:1             ;               // view on or off   
	unsigned       delay:1              ;               // delay on   
	unsigned       grid:1               ;               // grid on   
	unsigned       lev_symb:1           ;               // level symbology   
	unsigned       points:1             ;               // points   
	unsigned       constructs:1         ;               // line constructs   
	unsigned       dimens:1             ;               // dimensioning   
	unsigned       fast_cell:1          ;               // fast cells   
	unsigned       def:1                ;
} Viewflags                             ;           

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////          ELEMENT CORE INCLUDES
//////
////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagDGNBSpline_Surface{
	ELMHDR  ehdr;
	DISPHDR dhdr;
	long    desc_words;
	struct tagFlags{
		unsigned short order:4;		// B-spline U order - 2
		unsigned short curve_display:1;	// surface display flag
		unsigned short poly_display:1;	// polygon display flag
		unsigned short rational:1;	// rationalization flag
		unsigned short closed:1;	// closed U surface flag
		unsigned short curve_type:8;	// surface type
	}flags;
	short num_poles_u;
	short num_knots_u;
	short rule_lines_u;
	struct tagBSurf_Flags{
		unsigned short v_order:4;
		short reversed1:2;
		unsigned short arcSpacing:1;
		unsigned short v_closed:1;
		unsigned short reversed2:8;
	}bsurf_flags;
	short num_poles_v;
	short num_knots_v;
	short rule_lines_v;
	short num_bounds;
}Bspline_surface;

#ifdef __cplusplus
}
#endif

#endif