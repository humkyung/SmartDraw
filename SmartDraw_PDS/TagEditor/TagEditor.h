/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:c:\projects\tageditor\tageditor.h
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|                                                                       |
|                                                                       |
|									|
|									|
|									|
|									|
|									|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   c:\projects\tageditor\tageditor.h
|                                                                       |
+----------------------------------------------------------------------*/
#define DIALOGID_PWTagEditor		    1010
#define DIALOGID_EditDimension        	    1011

#define PUSHBTNID_OK                  	    200000001
#define PUSHBTNID_Cancel              	    200000002

#define TEXTID_ExistingText           	    200000001
#define TEXTID_NewText                	    200000002

#define ICONCMDFRAMEID_Frame		    1

#define ICONCMDSMALLID_MoveLeft       	    1
#define ICONCMDSMALLID_MoveCenter     	    2
#define ICONCMDSMALLID_MoveRight      	    3
#define ICONCMDSMALLID_MoveAbove       	    4
#define ICONCMDSMALLID_MoveInline     	    5
#define ICONCMDSMALLID_MoveBelow       	    6
#define ICONCMDSMALLID_LeftBottomDot	    7
#define ICONCMDSMALLID_CenterDot      	    8
#define ICONCMDSMALLID_RightTopDot          9
#define ICONCMDSMALLID_TagMove        	    10
#define ICONCMDSMALLID_TagRotate      	    11
#define ICONCMDSMALLID_TagRotate_R90		12
#define ICONCMDSMALLID_PWEditDimension	    13

#define MENUBARID_BarOptions                1

#define PULLDOWNMENUID_CTools               1

#define	DRAW_MODE_DYNAMIC		    1					
#define BUFF_LEN			    256
#define PATTERN_SIZE_2			    3

#define OPT_TAGMOVE			    1
#define OPT_TAGROTATE			    2

#define PATTERN_1			    "PAT 01"
#define PATTERN_2			    "PAT 02"
#define PATTERN_3			    "PAT 03"
#define PATTERN_4			    "PAT 04"
#define PATTERN_5			    "PAT 05"
#define PATTERN_6			    "PAT 06"
#define PATTERN_7			    "PAT 07"
#define PATTERN_8			    "PAT 08"

#define ELM_POS_1			    1
#define ELM_POS_2			    2
#define ELM_POS_3			    3
#define ELM_POS_4			    4    
#define ELM_POS_5			    5
#define ELM_POS_6			    6
#define ELM_POS_7			    7
#define ELM_POS_8			    8
#define ELM_POS_9			    9
#define ELM_POS_10			    10

#define SELECTION_SET			    "Can not select multiple elements for this operation.Reset to cancel this operation"    
#define INVALID_PATTERN			    "Invalid element/pattern is selected. Reset to restore operation mode."    
#define INVALID_Dimension		    "Invalid element/dimension selected. (Reset and select next input)"    
#define CMD_MSG				    "Accept/Reject(select next input)"
#define DIM_SELECT_FIRST_ELEMENT	    "Select FIRST Dimension Element"
#define DIM_SELECT_SECOND_ELEMENT	    "Select SECOND Dimension Element"



/*----------------------------------------------------------------------+
|									|
|   Command list IDs							|
|									|
+----------------------------------------------------------------------*/

#define	    COMMANDID_DimTextMoveLeft			1
#define	    COMMANDID_DimTextMoveRight			2
#define	    COMMANDID_DimTextMoveCenter			3
#define	    COMMANDID_DimTextMoveAbove			4
#define	    COMMANDID_DimTextMoveInline			5
#define	    COMMANDID_DimTextMoveBelow			6
#define	    COMMANDID_DimTextEdit			7
#define	    COMMANDID_DimChangeLeftTerminatorToDot      8
#define	    COMMANDID_DimChangeRightTerminatorToDot     9
#define	    COMMANDID_DimChangeTopTerminatorToDot       10
#define	    COMMANDID_DimChangeBottomTerminatorToDot	11
#define     COMMANDID_DimChangeCenterTerminatorToDot    12
#define	    COMMANDID_RotatePattern			13
#define	    COMMANDID_MovePattern			14
#define	    COMMANDID_RotatePattern_R90		15

/*----------------------------------------------------------------------+
|									|
|   Message list IDs							|
|									|
+----------------------------------------------------------------------*/
#define	    MESSAGELISTID_Commands	0
#define	    MESSAGELISTID_Prompts	1
#define	    MESSAGELISTID_Msgs		2

/*----------------------------------------------------------------------+
|									|
|   Prompt IDs - used in the Message list definition for command prompts|
|									|
+----------------------------------------------------------------------*/
#define	    PROMPTID_IdentifyPattern	1
#define	    PROMPTID_CustomAcceptReject	2
#define	    PROMPTID_SelectFirstDimensionElement	3
#define	    PROMPTID_SelectSecondDimensionElement	4

#define     MSGID_LoadCmdTbl		34567

/*----------------------------------------------------------------------+
|									|
|   Local Structure Definitions 					|
|									|
+----------------------------------------------------------------------*/
typedef struct dimeditinfo
    {
    char     existingDimText[16];
    int     newDimText[16];
    } DimEditInfo;
