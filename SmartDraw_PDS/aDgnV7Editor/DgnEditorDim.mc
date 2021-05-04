/*----------------------------------------------------------------------+
|                                                                       |
|   DgnEditor.mc - Dimension and Manipulation source code               |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/

#include <mdl.h>        /* MDL Library funcs structures & constants */
#include <tcb.h>        /* MicroStation terminal control block */
#include <global.h>     /* structure definitions for global data areas */
#include <cmdlist.h>    /* MicroStation command numbers */
#include <userfnc.h>
#include <mdldim.h> 

#include <string.h>
#include <stdio.h>

#include <msdim.fdf> /* MDL library for Dimesion functions, structures and constants */
#include <ditemlib.fdf>

#include "DgnEditor.h"                      
#include "DgnEditorcmd.h"                      
#include "cmdids.h"	/* resource ids */

Private     double g_dimTexLift;

extern DimEditInfo	dimeditInfo;  /* Tool Settings (slam-down) variables */
Private     ULong g_dimPos1st;
Private     ULong g_dimPos2nd;

/* Function prototypes */
int dimManipulate_EndCommand(void);
void dgneditor_updateViews(void);
int newItems_unloadFunction(int reasonCode);
void find_TextLiftValue(char * extFilename);
Private void dimValidate();
void dimchangecentertermiantortodot_StartCommand ();

/*****************************************************************************************
//Function  : dimModifyLeftTerminatorToDot()
//Desc      : Modify LEFT terminator of dimension to DOT
//Return    : int
//Author    : RSR dated 09/11/2010
//****************************************************************************************/
Private int dimModifyLeftTerminatorToDot
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamTemplate	infoDimParam;

    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    mdlDim_getParam(&infoDimParam, &dimElm, DIMPARAM_TEMPLATE);
            infoDimParam.firstTerm = 4;	/* Set Left terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            mdlDim_setParam(&dimElm, &infoDimParam, DIMPARAM_TEMPLATE); 
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Modify Left Terminator to Dot, Dimension NOT VALIDATED");
	}
    }
//    dgneditor_updateViews();
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimModifyRightTerminatorToDot()
//Desc      : Modify RIGHT terminator of dimension to DOT
//Return    : int
//Author    : RSR dated 09/11/2010
//****************************************************************************************/
Private int dimModifyRightTerminatorToDot
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamTemplate	infoDimParam;

    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    mdlDim_getParam(&infoDimParam, &dimElm, DIMPARAM_TEMPLATE);
            infoDimParam.rightTerm = 4; /* Set right terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            mdlDim_setParam(&dimElm, &infoDimParam, DIMPARAM_TEMPLATE); 
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Modify Right Terminator to Dot, Dimension NOT VALIDATED");
	}
    }
//    dgneditor_updateViews();
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimModifyBottomTerminatorToDot()
//Desc      : Modify BOTTOM terminator of dimension to DOT
//Return    : int
//Author    : RSR dated 13/11/2010
//****************************************************************************************/
Private int dimModifyBottomTerminatorToDot
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamTemplate	infoDimParam;

    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Get and set dimension parameters to reflect new terminator */
	    mdlDim_getParam(&infoDimParam, &dimElm, DIMPARAM_TEMPLATE);
            infoDimParam.firstTerm = 4; /* Set  bottom terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            mdlDim_setParam(&dimElm, &infoDimParam, DIMPARAM_TEMPLATE); 
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Modify Bottom Terminator to Dot, Dimension NOT VALIDATED");
	}
    }
//    dgneditor_updateViews();
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimModifyTopTerminatorToDot()
//Desc      : Modify TOP terminator of dimension to DOT
//Return    : int
//Author    : RSR dated 13/11/2010
//****************************************************************************************/
Private int dimModifyTopTerminatorToDot
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamTemplate	infoDimParam;

    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Get and set dimension parameters to reflect new terminator */
	    mdlDim_getParam(&infoDimParam, &dimElm, DIMPARAM_TEMPLATE);
            infoDimParam.rightTerm = 4; /* Set top terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            mdlDim_setParam(&dimElm, &infoDimParam, DIMPARAM_TEMPLATE); 
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Modify Top Terminator to Dot, Dimension NOT VALIDATED");
	}
    }
//    dgneditor_updateViews();
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimModifyCenterTerminatorToDot_SelectSecondElement()
//Desc      : Modify Center terminator of dimension to DOT, SELECT SECOND element
//Return    : int
//Author    : RSR dated 10/12/2010
//****************************************************************************************/
Private int dimModifyCenterTerminatorToDot_SelectSecondElement
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm1, dimElm2;
    DimParamTemplate infoDimParam;
    DPoint3d outPoints1[2], outPoints2[2];
    char msg[64];
    boolean b_horizontal1st, b_horizontal2nd;
    
    b_horizontal1st = TRUE;
    b_horizontal2nd = TRUE;
    
    g_dimPos2nd = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);

    /* Get the details about FIRST dimension */
    mdlElement_read (&dimElm1, MASTERFILE, g_dimPos1st);
    if (mdlElement_getType (&dimElm1) == DIMENSION_ELM)
    {
	mdlDim_extractPoints(outPoints1, &dimElm1, MASTERFILE, 0, 2);

	// Check for horizontal or vertical dimension
	if (outPoints1[0].x == outPoints1[1].x)  /* Vertical Dimension */
	{
        b_horizontal1st = FALSE;
	}
	else if (outPoints1[0].y == outPoints1[1].y)  /* Horizontal Dimension */
	{
        b_horizontal1st = TRUE;
	}
    }

    /* Get the details about SECOND dimension */
    mdlElement_read (&dimElm2, MASTERFILE, g_dimPos2nd);
    if (mdlElement_getType (&dimElm2) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm2))
	{
	    mdlDim_extractPoints(outPoints2, &dimElm2, MASTERFILE, 0, 2);
	    // Check for horizontal or vertical dimension
	    if (outPoints2[0].x == outPoints2[1].x)  /* Vertical Dimension */
	    {
	    b_horizontal2nd = FALSE;
	    }
	    else if (outPoints2[0].y == outPoints2[1].y) /* Horizontal Dimension */
	    {
	    b_horizontal2nd = TRUE;
	    }
	}
    }

//    if ((b_horizontal1st == TRUE && b_horizontal2nd == TRUE) || (b_horizontal1st == FALSE && b_horizontal2nd == FALSE)) /* Action item for Horizontal dimensions */
    if ((b_horizontal1st == TRUE && b_horizontal2nd == TRUE)) /* Action item for Horizontal dimensions */
    {
//	if (outPoints1[1].x == outPoints2[0].x || outPoints1[1].y == outPoints2[0].y)
	if (outPoints1[1].x == outPoints2[0].x)
	{
	    /* Change first dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm1, DIMPARAM_TEMPLATE);
	    infoDimParam.rightTerm = 4;     /* Set Rithg terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm1, ERASE);

	    mdlDim_setParam(&dimElm1, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm1, NULL, g_dimPos1st);
	    mdlElement_display (&dimElm1, drawMode);

	    /* Change second dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm2, DIMPARAM_TEMPLATE);
	    infoDimParam.firstTerm = 4;     /* Set Left terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm2, ERASE);

	    mdlDim_setParam(&dimElm2, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm2, NULL, g_dimPos2nd);
	    mdlElement_display (&dimElm2, drawMode);
	}
//	else if (outPoints1[0].x == outPoints2[1].x || outPoints1[0].y == outPoints2[1].y)
	else if (outPoints1[0].x == outPoints2[1].x)
	{
	    /* Change first dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm1, DIMPARAM_TEMPLATE);
	    infoDimParam.firstTerm = 4;     /* Set Left terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm1, ERASE);

	    mdlDim_setParam(&dimElm1, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm1, NULL, g_dimPos1st);
	    mdlElement_display (&dimElm1, drawMode);

	    /* Change second dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm2, DIMPARAM_TEMPLATE);
	    infoDimParam.rightTerm = 4;     /* Set Rithg terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm2, ERASE);

	    mdlDim_setParam(&dimElm2, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm2, NULL, g_dimPos2nd);
	    mdlElement_display (&dimElm2, drawMode);
	}
    }
    else if (b_horizontal1st == FALSE && b_horizontal2nd == FALSE) /* Action item for Vertical dimensions */
    {
	if (outPoints1[1].y == outPoints2[0].y)
	{
	    /* Change first dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm1, DIMPARAM_TEMPLATE);
	    infoDimParam.rightTerm = 4;     /* Set Left terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm1, ERASE);

	    mdlDim_setParam(&dimElm1, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm1, NULL, g_dimPos1st);
	    mdlElement_display (&dimElm1, drawMode);

	    /* Change second dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm2, DIMPARAM_TEMPLATE);
	    infoDimParam.firstTerm = 4;     /* Set Rithg terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm2, ERASE);

	    mdlDim_setParam(&dimElm2, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm2, NULL, g_dimPos2nd);
	    mdlElement_display (&dimElm2, drawMode);
	}
	else if (outPoints1[0].y == outPoints2[1].y)
	{
	    /* Change first dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm1, DIMPARAM_TEMPLATE);
	    infoDimParam.firstTerm = 4;     /* Set Left terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm1, ERASE);

	    mdlDim_setParam(&dimElm1, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm1, NULL, g_dimPos1st);
	    mdlElement_display (&dimElm1, drawMode);

	    /* Change second dimension terminator to dot */
	    mdlDim_getParam(&infoDimParam, &dimElm2, DIMPARAM_TEMPLATE);
	    infoDimParam.rightTerm = 4;     /* Set Rithg terminal to dot */

	    /* Erase old element display */
	    mdlElement_display (&dimElm2, ERASE);

	    mdlDim_setParam(&dimElm2, &infoDimParam, DIMPARAM_TEMPLATE);
	    mdlElement_rewrite(&dimElm2, NULL, g_dimPos2nd);
	    mdlElement_display (&dimElm2, drawMode);
	}
	
    }
    
    mdlState_clear()	;
    dimchangecentertermiantortodot_StartCommand();
//    mdlLocate_init () ;
//    mdlState_restartCurrentCommand();

    return SUCCESS;
    }


/*****************************************************************************************
//Function  : dimModifyCenterTerminatorToDot_SelectFirstElement()
//Desc      : Modify Center terminator of dimension to DOT, SELECT FIRST element
//Return    : int
//Author    : RSR dated 3/12/2010
//****************************************************************************************/
Private int dimModifyCenterTerminatorToDot_SelectFirstElement
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    g_dimPos1st = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);

    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimModifyCenterTerminatorToDot_SelectFirstElement, dimModifyCenterTerminatorToDot_SelectSecondElement, dimValidate, NULL, NULL, 
		COMMANDID_DimChangeCenterTerminatorToDot, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlOutput_prompt(DIM_SELECT_SECOND_ELEMENT);

    return SUCCESS;
    }

        
/*****************************************************************************************
//Function  : dimjustifyleft_AcceptDimension()
//Desc      : Move dimension text to LEFT
//Return    : int
//Author    : RSR dated 09/11/2010
//****************************************************************************************/
Private int dimjustifyleft_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamMisc	info;
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            /* Move dimension text to LEFT*/
            mdlDim_setTextOffset(&dimElm, 0, DIMTEXT_LEFT, 0);
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, NORMALDRAW);
	}
	else
	{
	    mdlDialog_openAlert ("In Left Justify, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimjustifyright_AcceptDimension()
//Desc      : Move dimension text to RIGHT
//Return    : int
//Author    : RSR dated 09/11/2010
//****************************************************************************************/
Private int dimjustifyright_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamMisc	info;
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            /* Move dimension text to RIGHT*/
            mdlDim_setTextOffset(&dimElm, 0, DIMTEXT_RIGHT, 0);
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, NORMALDRAW);
	}
	else
	{
	    mdlDialog_openAlert ("In Right Justify, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimjustifycenter_AcceptDimension()
//Desc      : Move dimension text to CENTER
//Return    : int
//Author    : RSR dated 09/11/2010
//****************************************************************************************/
Private int dimjustifycenter_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamMisc	info;
    char msg[64];
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Erase old element display */
	    mdlElement_display (&dimElm, ERASE);

            /* Move dimension text to CENTER*/
	    mdlDim_setTextOffset(&dimElm, 0, DIMTEXT_CENTER, 0);
            mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, NORMALDRAW);
	}
	else
	{
	    mdlDialog_openAlert ("In Center Justify, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimtextmoveabove_AcceptDimension()
//Desc      : Move dimension text ABOVE line
//Return    : int
//Author    : RSR dated 12/11/2010
//****************************************************************************************/
Private int dimtextmoveabove_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong dimPos;    	/* position of dimension in design file */
    char msg[64];
    DimParamGeometry	info;
    DimParamText dimTextParams;
    DimParamFlags	dimParamFlags;
    int dimTextLift;
   
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Find Dimension Text Lift (Dimension line to bottom of text) */
	    mdlDim_getParam(&info, &dimElm, DIMPARAM_GEOMETRY);

	    /* Find Dimension Text height */
	    mdlDim_getParam(&dimTextParams, &dimElm, DIMPARAM_TEXT);
	    
	    /* Find Dimension Parameters */
	    mdlDim_getParam(&dimParamFlags, &dimElm, DIMPARAM_FLAGS);

	    /* Check if this is an INLINE dimension text */
	    if (dimParamFlags.embedded == TRUE)
	    {
		/* Set new text lift */
		info.textLift = (dimTextParams.height * 0.2);

		/* Set the below parameter for Non INLINE dimension text */
		dimParamFlags.embedded = FALSE;

		/* Erase old element display */
		mdlElement_display (&dimElm, ERASE);

		mdlDim_setParam(&dimElm, &dimParamFlags, DIMPARAM_FLAGS);
	    }
	    else if (info.textLift < 0) /* Only text below dimension line should move above dimension line */
	    {
		/* Set new text lift to +ve */
		info.textLift = (abs(info.textLift) - dimTextParams.height);

		/* Erase old element display */
		mdlElement_display (&dimElm, ERASE);
	    }
	    /* Set new parameters to dimension */
	    mdlDim_setParam(&dimElm, &info, DIMPARAM_GEOMETRY);
	    mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Dim Text move ABOVE, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimtextmoveinline_AcceptDimension()
//Desc      : Move dimension text INLINE with dimension line
//Return    : int
//Author    : RSR dated 12/11/2010
//****************************************************************************************/
Private int dimtextmoveinline_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamFlags	info;
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    mdlDim_getParam(&info, &dimElm, DIMPARAM_FLAGS);
	    if (info.embedded == FALSE)
	    {
		/* Erase old element display */
		mdlElement_display (&dimElm, ERASE);

		/* Set the parameter to embedded */
		info.embedded = TRUE;

		/* Set new dimension parameters */
		mdlDim_setParam(&dimElm, &info, DIMPARAM_FLAGS);
		mdlElement_rewrite(&dimElm, NULL, dimPos);
		mdlElement_display (&dimElm, drawMode);
	    }
	}
	else
	{
	    mdlDialog_openAlert ("In Dim Text move INLINE, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimtextmovebelow_AcceptDimension()
//Desc      : Move dimension text BELOW dimension line
//Return    : int
//Author    : RSR dated 12/11/2010
//****************************************************************************************/
Private int dimtextmovebelow_AcceptDimension
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {
    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimParamGeometry	info;
    DimParamText dimTextParams;
    DimParamFlags	dimParamFlags;
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    /* Find Dimension Text Lift (Dimension line to bottom of text) */
	    mdlDim_getParam(&info, &dimElm, DIMPARAM_GEOMETRY);

	    /* Find Dimension Text height */
	    mdlDim_getParam(&dimTextParams, &dimElm, DIMPARAM_TEXT);

	    /* Find Dimension Parameters */
	    mdlDim_getParam(&dimParamFlags, &dimElm, DIMPARAM_FLAGS);

	    /* Check if this is an INLINE dimension text */
	    if (dimParamFlags.embedded == TRUE)
	    {
		/* Set new text lift */
		info.textLift = (dimTextParams.height * 0.2);

		/* Set new text lift to -ve */
		info.textLift = -(info.textLift + dimTextParams.height);

		/* Set the below parameter for Non INLINE dimension text */
		dimParamFlags.embedded = FALSE;

		/* Erase old element display */
		mdlElement_display (&dimElm, ERASE);

		mdlDim_setParam(&dimElm, &dimParamFlags, DIMPARAM_FLAGS);
	    }
	    else if (info.textLift > 0) /* Only text above dimension line should move below dimension line */
	    {	    
		/* Set new text lift to -ve */
		info.textLift = -(info.textLift + dimTextParams.height);

		/* Erase old element display */
		mdlElement_display (&dimElm, ERASE);
	    }
	    /* Set new dimension parameters */
	    mdlDim_setParam(&dimElm, &info, DIMPARAM_GEOMETRY);
	    mdlElement_rewrite(&dimElm, NULL, dimPos);
	    mdlElement_display (&dimElm, drawMode);
	}
	else
	{
	    mdlDialog_openAlert ("In Dim Text move BELOW, Dimension NOT VALIDATED");
	}
    }
    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }

/*****************************************************************************************
//Function  : dimEditText_Accept()
//Desc      : Move dimension text BELOW dimension line
//Return    : int
//Author    : RSR dated 12/11/2010
//****************************************************************************************/
Private int dimEditText_Accept
(
Dpoint3d	*ptP,
int	  view,		     /* => Cursor view			       */
int	  drawMode,	     /* => Current draw / erase mode	       */
int    	  writeMode	     /* => If TRUE, add elements to file       */
)
    {

    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    DimStrings	dimStringInfo;
    DimStringConfig dimStringConfig;
    int	lastAction;
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) == DIMENSION_ELM)
    {
	if (SUCCESS == mdlDim_validate(&dimElm))
	{
	    if (mdlDim_getStrings (&dimStringInfo, &dimStringConfig, &dimElm, 0)==SUCCESS)
	    {
		if (dimStringConfig.tolerance  == FALSE)
		{
		    if (strlen(dimStringInfo.primary.single) > 0)
			strcpy (dimeditInfo.existingDimText, dimStringInfo.primary.single);
		    else
			strcpy (dimeditInfo.existingDimText, "*");

		    /* open child modal dialog box */
		    if (!mdlDialog_openModal (&lastAction, NULL, DIALOGID_EditDimension))
		    {
			if (lastAction == ACTIONBUTTON_OK)
			{
			    if (strlen (dimeditInfo.newDimText) > 0)
			    {
				strcpy (dimStringInfo.primary.single, dimeditInfo.newDimText);
				if (strlen(dimStringInfo.primary.single) > 0)
				{
				    mdlElement_display (&dimElm, ERASE);
				    if (mdlDim_setStrings (&dimElm, 0, &dimStringInfo) == SUCCESS)
				    {
					if (mdlDim_validate (&dimElm) == SUCCESS)
					{
					    mdlElement_rewrite(&dimElm, NULL, dimPos);
					    mdlElement_display (&dimElm, drawMode);
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
	else
	    mdlDialog_openAlert("Dim not validated...");
    }

    mdlLocate_init () ;
    mdlState_restartCurrentCommand();

    return SUCCESS;
    }
    
/*****************************************************************************************
//Function  : dgneditor_updateViews()
//Desc      : Update all the views in dgn file
//Return    : void
//Author    : RSR dated 12/11/2010
//****************************************************************************************/
Private void dgneditor_updateViews
(
void
)
    {
    long    asViewList[ MAX_VIEWS ];
    int     nNdx;

    for ( nNdx = 0; nNdx < MAX_VIEWS; ++nNdx )
	asViewList[ nNdx ] = -1;

    mdlView_updateMulti ( asViewList, FALSE, NORMALDRAW, NULL, TRUE );
    }

/*****************************************************************************************
//Function  : dimManipulate_EndCommand()
//Desc      : Clears the command at the end of manipulation command
//Return    : int
//Author    : RSR dated 28/10/2010
//****************************************************************************************/
Private int dimManipulate_EndCommand
(
void
)
    {
    mdlState_restartCurrentCommand();
    return (SUCCESS);
    }

/*****************************************************************************************
//Function  : dimjustifyleft_StartCommand()
//Desc      : Command to set dimension Text to LEFT
//Return    : void
//Author    : RSR dated 28/10/2010
//****************************************************************************************/

cmdName void    dimjustifyleft_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMJUSTIFYLEFT	
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimjustifyleft_StartCommand, dimjustifyleft_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveLeft, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimjustifyright_StartCommand()
//Desc      : Command to set dimension Text to RIGHT
//Return    : void
//Author    : RSR dated 09/11/2010
//****************************************************************************************/

cmdName void    dimjustifyright_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMJUSTIFYRIGHT
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimjustifyright_StartCommand, dimjustifyright_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveRight, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimjustifycenter_StartCommand()
//Desc      : Command to set dimension Text to CENTER
//Return    : void
//Author    : RSR dated 09/11/2010
//****************************************************************************************/

cmdName void    dimjustifycenter_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMJUSTIFYCENTER
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimjustifycenter_StartCommand, dimjustifycenter_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveCenter, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimtextmoveabove_StartCommand()
//Desc      : Command to set dimension Text ABOVE dimension line
//Return    : void
//Author    : RSR dated 11/11/2010
//****************************************************************************************/

cmdName void    dimtextmoveabove_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMTEXTMOVEABOVE
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimtextmoveabove_StartCommand, dimtextmoveabove_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveAbove, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimtextmoveinline_StartCommand()
//Desc      : Command to set dimension Text INLINE with dimension line
//Return    : void
//Author    : RSR dated 11/11/2010
//****************************************************************************************/

cmdName void    dimtextmoveinline_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMTEXTMOVEINLINE
{  	

//    mdlDialog_openAlert ("Dimension Text move INLINE - Develpment is in progress...");

    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimtextmoveinline_StartCommand, dimtextmoveinline_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveInline, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimtextmovebelow_StartCommand()
//Desc      : Command to set dimension Text BELOW dimension line
//Return    : void
//Author    : RSR dated 11/11/2010
//****************************************************************************************/

cmdName void    dimtextmovebelow_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMTEXTMOVEBELOW
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimtextmovebelow_StartCommand, dimtextmovebelow_AcceptDimension, dimValidate, NULL, NULL, 
		COMMANDID_DimTextMoveBelow, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimchangelefttermiantortodot_StartCommand()
//Desc      : Command to change LEFT or BOTTOM terminator to DOT
//Return    : void
//Author    : RSR dated 11/11/2010
//****************************************************************************************/

cmdName void    dimchangeleftbottomtermiantortodot_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMCHANGELEFTBOTTOMTERMIANTORTODOT
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimchangeleftbottomtermiantortodot_StartCommand, dimModifyLeftTerminatorToDot, dimValidate, NULL, NULL, 
		COMMANDID_DimChangeLeftTerminatorToDot, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimchangecentertermiantortodot_StartCommand()
//Desc      : Command to change CENTER terminator to DOT
//Return    : void
//Author    : RSR dated 13/11/2010
//****************************************************************************************/

cmdName void    dimchangecentertermiantortodot_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMCHANGECENTERTERMIANTORSTODOT
{  	
//    mdlDialog_openAlert ("Change Center Termainator to Dot - Development is in progress.");

    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimchangecentertermiantortodot_StartCommand, dimModifyCenterTerminatorToDot_SelectFirstElement, dimValidate, NULL, NULL, 
		COMMANDID_DimChangeCenterTerminatorToDot, PROMPTID_SelectFirstDimensionElement, FALSE, 1);
    mdlOutput_prompt(DIM_SELECT_FIRST_ELEMENT);
}

/*****************************************************************************************
//Function  : dimchangerighttermiantortodot_StartCommand()
//Desc      : Command to change RIGHT or TOP terminator to DOT
//Return    : void
//Author    : RSR dated 13/11/2010
//****************************************************************************************/

cmdName void    dimchangerighttoptermiantortodot_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMCHANGERIGHTTOPTERMIANTORTODOT
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimchangerighttoptermiantortodot_StartCommand, dimModifyRightTerminatorToDot, dimValidate, NULL, NULL, 
		COMMANDID_DimChangeRightTerminatorToDot, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimchangetoptermiantortodot_StartCommand()
//Desc      : Command to change TOP terminator to DOT
//Return    : void
//Author    : RSR dated 13/11/2010
//****************************************************************************************/

/*
cmdName void    dimchangetoptermiantortodot_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMCHANGETOPTERMIANTORSTODOT
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimchangetoptermiantortodot_StartCommand, dimModifyTopTerminatorToDot, NULL, NULL, NULL, 
		COMMANDID_DimChangeTopTerminatorToDot, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}
*/

/*****************************************************************************************
//Function  : dimchangebottomtermiantortodot_StartCommand()
//Desc      : Command to change BOTTOM terminator to DOT
//Return    : void
//Author    : RSR dated 13/11/2010
//****************************************************************************************/

/*
cmdName void    dimchangebottomtermiantortodot_StartCommand
(
    void
)
cmdNumber       CMD_DAELIM_DIMCHANGEBOTTOMTERMIANTORSTODOT
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimchangebottomtermiantortodot_StartCommand, dimModifyBottomTerminatorToDot, NULL, NULL, NULL, 
		COMMANDID_DimChangeBottomTerminatorToDot, PROMPTID_CustomAcceptReject, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}
*/

/*****************************************************************************************
//Function  : dimeditText_StartCommand()
//Desc      : Command to edit dimension text
//Return    : void
//Author    : RSR dated 23/11/2010
//****************************************************************************************/

cmdName void    dimeditText_StartCommand
(
    void
)
cmdNumber       CMD_EDITDMNS
{  	
    mdlLocate_allowLocked () ;
    mdlLocate_init () ;

    mdlState_startModifyCommand(dimeditText_StartCommand, dimEditText_Accept, dimValidate, NULL, NULL, COMMANDID_CommandName, PROMPTID_Accept, FALSE, 0);
    mdlState_setFunction(STATE_RESET, dimManipulate_EndCommand);
}

/*****************************************************************************************
//Function  : dimValidate()
//Desc      : Simple dynamic function which check validity for the right dimension element
//Return    : void
//Author    : RSR dated 30/11/2010
//****************************************************************************************/

Private void dimValidate()
{

    MSElement dimElm;
    ULong    	    	dimPos;    	/* position of dimension in design file */
    
    dimPos = mdlElement_getFilePos (FILEPOS_CURRENT, NULL);
    mdlElement_read (&dimElm, MASTERFILE, dimPos);
    if (mdlElement_getType (&dimElm) != DIMENSION_ELM)
    {
	mdlOutput_prompt (INVALID_Dimension);
    }
}

/*****************************************************************************************
//Function  : daelim_InsertEditorMenu()
//Desc      : Function to add MicroStation Editor pull down menu
//Return    : void
//Author    : RSR dated 13/11/2010
//****************************************************************************************/
Private void daelim_InsertEditorMenu
(
void
)
    {
    RawItemHdr              *menuBarP;      /* pointer to menu bar header   */
    DItem_PulldownMenu      *mbarMenuP;     /* pointer to pull down menu    */

    menuBarP = mdlDialog_menuBarGetCmdWinP ();
    if (!menuBarP)
        {
	    mdlDialog_openAlert ("Failed to get Main Menu Bar");
        }
    else
    {
	    mdlDialog_openAlert ("Found Main Menu Bar");
	    mdlDialog_menuBarInsertMenu(menuBarP, RTYPE_PulldownMenu, MENUBARID_BarOptions, NULL);
    }
    }

/*****************************************************************************************
//Function  : newItems_unloadFunction()
//Desc      : Remove editor pulldown menu on application unload
//Return    : int
//Author    : RSR dated 17/11/2010
//****************************************************************************************/
Private int newItems_unloadFunction
(
int reasonCode
)
    {
    RawItemHdr              *menuBarP;      /* pointer to menu bar header   */
    DItem_PulldownMenu      *mbarMenuP;     /* pointer to pull down menu    */
    DItem_PulldownMenuItem  *menuItemP;         /* <= found menuItem */

    menuBarP = mdlDialog_menuBarGetCmdWinP ();

    mdlDialog_openAlert ("Unloading Application.....");
    mbarMenuP = mdlDialog_menuBarFindMenu (menuBarP, RTYPE_PulldownMenu, MENUBARID_BarOptions);
    mdlDialog_menuBarDeleteAllItems (mbarMenuP);
    if (mdlDialog_menuBarFindItem (&menuItemP, NULL, menuBarP, RTYPE_PulldownMenu, PULLDOWNMENUID_CTools, MENUBARID_BarOptions) == TRUE)
    {
       mdlDialog_openAlert ("Pull down item found");
       mdlDialog_menuBarDeleteItem (menuItemP); 
       //mdlDialog_menuBarUnloadApp (menuItemP, NULL, PULLDOWNMENUID_CTools);
    }

    return (FALSE);
    }

/*****************************************************************************************
//Function  : find_TextLiftValue()
//Desc      : Funtion to find out demension text lift value from .ext (external) ASCII file
//Return    : void
//Author    : RSR dated 17/11/2010
//****************************************************************************************/
Private void find_TextLiftValue
(
char *filename
)
    {
    FILE *hFile=NULL;
    char buffer[255 + 1], msg[256];
    char *token, *cp;

    hFile = mdlTextFile_open( filename, TEXTFILE_READ );
    if( hFile != NULL )
    {
	while( !feof( hFile ) )
	{
	    memset(buffer, 0, 255);
	    mdlTextFile_getString( buffer, sizeof( buffer ), hFile, TEXTFILE_DEFAULT ); //! NEW LINE을 제거한다.

	    token = strtok ( buffer , "|");
	    if(NULL == token)
	    {
	        mdlDialog_openAlert ("pToken is NULL");
	    }
	    else
	    {
		if(strcmp(token,"<DIMSTYLE>") == 0)
		{
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");
			token = strtok ( NULL , "^");

			/* Convert value to double and assign it to global text lift */
			g_dimTexLift = atof (token);
			
			break;
		}
		else
		{
		    	mdlDialog_openAlert ("Group NOT Found");
		}
	    }

	}
	mdlTextFile_close( hFile );
    }
    else
    {
	sprintf (msg, "External ASCII file %s not found", filename);
	mdlDialog_openAlert (msg);
    }
    }
