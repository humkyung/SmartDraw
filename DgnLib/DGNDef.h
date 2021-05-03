#ifndef __DGNDEF_H__
#define __DGNDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	DGN_TOLERANCE		0.00001
// --------------------------------------------------------------------   
//      Structure types                                                   
// --------------------------------------------------------------------   
#define DGNST_CORE		1 
#define DGNST_MULTIPOINT	2 
#define DGNST_COLORTABLE	3 
#define DGNST_TCB		4 
#define DGNST_ARC		5 
#define DGNST_TEXT		6 
#define DGNST_CELL_HEADER	7
#define DGNST_CELL_LIBRARY	8
#define DGNST_SURFACE_HEADER	18
#define DGNST_3D_SOLID_HEADER	19
#define DGNST_POINT_STRING	22
#define DGNST_GROUP_DATA	51

// --------------------------------------------------------------------   
//      Line Styles                                                       
// --------------------------------------------------------------------   
#define DGNS_SOLID			0
#define DGNS_DOTTED			1
#define DGNS_MEDIUM_DASH		2
#define DGNS_LONG_DASH			3
#define DGNS_DOT_DASH			4
#define DGNS_SHORT_DASH			5
#define DGNS_DASH_DOUBLE_DOT		6
#define DGNS_LONG_DASH_SHORT_DASH	7

// --------------------------------------------------------------------   
//      Class                                                             
// --------------------------------------------------------------------   
#define DGNC_PRIMARY			0
#define DGNC_PATTERN_COMPONENT		1
#define DGNC_CONSTRUCTION_ELEMENT	2
#define DGNC_DIMENSION_ELEMENT	        3
#define DGNC_PRIMARY_RULE_ELEMENT       4
#define DGNC_LINEAR_PATTERNED_ELEMENT   5
#define DGNC_CONSTRUCTION_RULE_ELEMENT  6

// --------------------------------------------------------------------   
//      Group Data level numbers.                                         
//                                                                        
//      These are symbolic values for the typ 5 (DGNT_GROUP_DATA)         
//      level values that have special meanings.                          
// --------------------------------------------------------------------   
#define DGN_GDL_COLOR_TABLE     1
#define DGN_GDL_NAMED_VIEW      3
#define DGN_GDL_REF_FILE        9

// --------------------------------------------------------------------   
//      Word 17 property flags.                                           
// --------------------------------------------------------------------   
#define DGNPF_HOLE		0x8000
#define DGNPF_SNAPPABLE		0x4000
#define DGNPF_PLANAR		0x2000
#define DGNPF_ORIENTATION	0x1000
#define DGNPF_ATTRIBUTES	0x0800
#define DGNPF_MODIFIED		0x0400
#define DGNPF_NEW		0x0200
#define DGNPF_LOCKED		0x0100
#define DGNPF_CLASS		0x000f

// --------------------------------------------------------------------   
//      Justifications                                                    
// --------------------------------------------------------------------   
#define DGNJ_LEFT_TOP		0
#define DGNJ_LEFT_CENTER	1
#define DGNJ_LEFT_BOTTOM	2
#define DGNJ_CENTER_TOP		3
#define DGNJ_CENTER_CENTER	4
#define DGNJ_CENTER_BOTTOM	5
#define DGNJ_RIGHT_TOP		6
#define DGNJ_RIGHT_CENTER	7
#define DGNJ_RIGHT_BOTTOM	8

#ifdef __cplusplus
}
#endif

#endif