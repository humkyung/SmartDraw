	/*----------------------------------------------------------------------+
|									|
| Copyright (1993) Bentley Systems, Inc., All rights reserved.		|
|									|
| "MicroStation", "MDL", and "MicroCSL" are trademarks of Bentley	|
|  Systems, Inc. and/or Intergraph Corporation.				|
|									|
| Limited permission is hereby granted to reproduce and modify this	|
| copyrighted material provided that the resulting code is used only in	|
| conjunction with Bentley Systems products under the terms of the	|
| license agreement provided therein, and that this notice is retained	|
| in its entirety in any such reproduction or modification.		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   tageditorstr.r - TagEditor Message list resource   	    	    		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>

#include "tageditor.h"

/*----------------------------------------------------------------------+
|									|
|   Messages List Resource Definition					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|								        |
|   The Message List resource structure is defined as follows:		|
|									|
|   #if defined (resource)						|
|   	typedef struct __messagelist__					|
|   	    {								|
|   	    ULong	1;   # No. expected infoFields per string.	|
|   	    struct messages						|
|   	    	{							|
|   	    	ULong   infoFields[];					|
|       	char    msg[];						|
|       	} Messages [];						|
|	    } MessageList;						|
|									|
+----------------------------------------------------------------------*/
MessageList MESSAGELISTID_Commands =
{
    {
	    {COMMANDID_DimTextMoveLeft, 	"Move Dimension Text to Left"},
	    {COMMANDID_DimTextMoveRight, 	"Move Dimension Text to Right"},
	    {COMMANDID_DimTextMoveCenter, 	"Move Dimension Text to Center"},
	    {COMMANDID_DimTextMoveAbove, 	"Move Dimension Text Above"},
	    {COMMANDID_DimTextMoveInline, 	"Move Dimension Text Inline"},
	    {COMMANDID_DimTextMoveBelow, 	"Move Dimension Text Below"},
	    {COMMANDID_DimTextEdit,		"Edit Dimension Text"},
	    {COMMANDID_DimChangeLeftTerminatorToDot,		"Change Left/Bottom Terminator to Dot"},
	    {COMMANDID_DimChangeRightTerminatorToDot,		"Change Right/Top Terminator to Dot"},
	    {COMMANDID_DimChangeCenterTerminatorToDot,		"Change CTerminator to Dot"},
	    {COMMANDID_RotatePattern, 		"Rotate Tag"},
	    {COMMANDID_MovePattern, 		"Move Tag"},
		{COMMANDID_RotatePattern_R90, 	"Rotate Tag By 90"}
    }
};

MessageList MESSAGELISTID_Prompts =
{
    {
	    {PROMPTID_IdentifyPattern, 		"Identify Pattern"},
	    {PROMPTID_CustomAcceptReject, 		"Accept/Reject(select next input)"},
	    {PROMPTID_SelectFirstDimensionElement, 		"Accept/Reject(select Second input)"},
	    {PROMPTID_SelectSecondDimensionElement, 		"Select SECOND Dimension Element"},
    }

};
MessageList MESSAGELISTID_Msgs =
{
    {
    	{MSGID_LoadCmdTbl,			"Unable to load command table"}
    }
};

