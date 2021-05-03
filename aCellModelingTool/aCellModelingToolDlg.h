/*----------------------------------------------------------------------+
|									|
|  Copyright (1995) Bentley Systems, Inc., All rights reserved.		|
|									|
|  "MicroStation" is a registered trademark and "MDL" and "MicroCSL"	|
|  are trademarks of Bentley Systems, Inc.			    	|
|									|
|  Limited permission is hereby granted to reproduce and modify this	|
|  copyrighted material provided that the resulting code is used only 	|
|  in conjunction with Bentley Systems products under the terms of the	|
|  license agreement provided therein, and that this notice is retained	|
|  in its entirety in any such reproduction or modification.		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|    $Logfile:   J:/mdl/examples/cellpick/cellpick.h_v  $
|   $Workfile:   cellpick.h  $
|   $Revision: 1.1 $
|   	$Date: 2010/12/16 09:07:01 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Function -								|
|									|
|	Dialog Box IDs							|
|									|
+----------------------------------------------------------------------*/
#if !defined (__cellpickH__)
#define	__cellpickH__

#define USERATTR_SIGNATURE			20246

#define DIALOGID_CellPicker			1	

/*----------------------------------------------------------------------+
|									|
|    Generic IDs							|
|									|
+----------------------------------------------------------------------*/
#define CELLBUTTON_PlaceActive			1
#define CELLBUTTON_PlaceActiveRelative		2
#define PUSHBUTTON_GetFileName			3
#define CELLBUTTON_EditCell			4
#define CELLBUTTON_Info				5
#define CELLBUTTON_Search			6

/*----------------------------------------------------------------------+
|									|
|    Resource IDs							|
|									|
+----------------------------------------------------------------------*/
#define RSCID_User_Data				1

/*----------------------------------------------------------------------+
|									|
|   Hook Id's								|
|									|
+----------------------------------------------------------------------*/
#define HOOKDIALOGID_CellPicker		1
#define	HOOKITEMID_CellButton		2	
#define	HOOKITEMID_OptionButton		3
#define	HOOKITEMID_GetFileName		4
#define	HOOKITEMID_EditCell		5
#define	HOOKITEMID_Option		6
#define HOOKITEMID_MessageList		7
#define HOOKITEMID_CellNameList		8
#define HOOKITEMID_Info			9
#define HOOKITEMID_Search		10

#define	TEXTID_CellFileName		200000000
#define	TEXTID_NewCellFileName		200000001
#define	TEXTID_CellName			200000002
#define	TEXTID_Name			200000003
#define	TEXTID_Desc1			200000004
#define	TEXTID_Desc2			200000001
#define	TEXTID_Desc3			200000005
#define	PUSHBTNID_Button		200000005	
#define OPTIONBUTTONID_OptionType	200000006
#define	LISTID_MessageList		200000007
/* Macro: RBUTTONLISTID_Option is possibly not being used */
#define	RBUTTONLISTID_Option		200000008
#define	RBUTTONID_Option1		2000000009
#define	RBUTTONID_Option2		2000000010
#define	LISTID_CellNameList		2000000011

#define	PIPE_MAINLINE			200000100
#define	PIPE_LATERAL			200000101
#define DEFOPENFILE_ID              		200000102

/* --------------------------- data ------------------------------ */
typedef struct tagOptionButtonInfo
{
	long type;
}OPTIONBUTTONINFO;

typedef struct tagRadioButtonInfo
{
    	long state;
}RADIOBUTTONINFO;

/*
typedef struct tagUserData
{
	unsigned char	data[128];
}USERDATA;
*/

#endif
#define RADIOBUTTONID_Edit            	    2000000010
#define HOOKITEMID_Edit               	    6
/* Macro: RADIOBUTTONLISTID_200000009 is possibly not being used */
#define RADIOBUTTONLISTID_200000009   	    200000009
/* Macro: RADIOBUTTONLISTID_200000010 is possibly not being used */
#define RADIOBUTTONLISTID_200000010   	    200000010

/* Macro: RADIOBUTTONLISTID_200000008 is possibly not being used */
#define RADIOBUTTONLISTID_200000008   	    200000008
/* Macro: RADIOBUTTONLISTID_200000010 is possibly not being used */
#define RADIOBUTTONLISTID_200000010   	    200000010
#define RADIOBUTTONLISTID_200000008   	    200000008
#define RADIOBUTTONLISTID_200000007   	    200000007
/* Macro: SYNONYMID_200000007 is possibly not being used */
#define SYNONYMID_200000007           	    200000007
/* Macro: SYNONYMID_200000008 is possibly not being used */
#define SYNONYMID_200000008           	    200000008

/* Macro: SYNONYMID_200000017 is possibly not being used */
#define SYNONYMID_200000017           	    200000017
#define SYNONYMID_200000108           	    200000108
#define PUSHBTNID_Button1             	    200000002
///#define PUSHBTNID_Button              	    200000001
#define HOOKITEMID_10                 	    10
#define HOOKITEMID_9                  	    9
