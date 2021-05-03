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
|    $Logfile:   J:/mdl/examples/cellpick/cellpick.mcv  $
|   $Workfile:   aCellModelingTool.cpp $
|   $Revision: 1.1 $
|   	$Date: 2010/12/16 09:07:00 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Function -								|
|									|
|	Example MDL program for picking cells				|
|									|
|	- - - - - - - - - - - - - - - - - - - - - - - - - - - - -	|
|									|
|   Public Routine Summary -						|
|									|
|	cellPick_cellButtonHook - Cell button hook			|
|	cellPick_drawCell - Draw cell in window				|
|	cellPick_getCellUorRange - Get cell range block			|
|	cellPick_initCellInfo - Initialize cell information		|
|	cellPick_reloadFunction - Application reload function		|
|	cellPick_showCell - Draw cell in dialog box			|
|	main - main entry point						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <mdl.h>
#include <mdlio.h>
#include <mselems.h>
#include <dlogitem.h>
#include <userfnc.h>
#include <tcb.h>
#include <cmdlist.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aCellModelingToolDlg.h"	    /*  Need to know the dialog id to open */
#include "aCellModelingTool.h"

#include <msfile.fdf>
#include <msview.fdf>
#include <mssystem.fdf>
#include <msrmatrx.fdf>
#include <msrsrc.fdf>
#include <msvec.fdf>
#include <mselmdsc.fdf>
#include <mscell.fdf>
#include <mscnv.fdf>
#include <msscan.fdf>

#define	NULLMDLFUNC		0L

/*----------------------------------------------------------------------+
|									|
|   Local function declarations 					|
|									|
+----------------------------------------------------------------------*/
#include    <dlogman.fdf>
Private RawItemHdr *CellNameListBoxP=NULL;
/*----------------------------------------------------------------------+
|									|
|   Private Global variables						|
|									|
+----------------------------------------------------------------------*/
///Private MSElementDescr   *cellDescrP=NULL;
Private OPTIONBUTTONINFO CellInfo;
Private RADIOBUTTONINFO  RadioButtonInfo;

static char szCellName[MAXFILELENGTH]={0,};
ElectLinkInfo linkageInfo;

/**
	@brief

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void loadCellLibraryIfNeeded(char *libToLoad)
{
	char libName[MAXFILELENGTH];

    	if(*libToLoad && strcmp(libToLoad,tcb->celfilenm))
	{
		mdlCell_attachLibrary(libName,libToLoad,NULL,TRUE);
	}
}

/**
	@brief	select cell library file on file dialog

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int GetFileName(char *buffer   /* <= filename user selected */)
{
	FileOpenParams  fileOpenParams;
	char 	msgBuffer[40], extensionDescr[128],defFileFilter[MAXEXTENSIONLENGTH+3];

     	/* set all members of the structure to NULL */
    	memset (&fileOpenParams, 0, sizeof(FileOpenParams));
    
	/* set-up the structure */
	fileOpenParams.dialogId 	  = 0;
	fileOpenParams.defFileId 	  = 0;
	fileOpenParams.openCreate	  = FILELISTATTR_OPEN;
	fileOpenParams.suggestedFileNameP = NULL;
	fileOpenParams.defaultFilterP = "*.cel";
	fileOpenParams.filterInfoStrP = "*.cel,Cell Library [*.cel]";
	fileOpenParams.defaultDirP    = "MS_CELL";
	fileOpenParams.titleP	      = "FILE OPEN DIALOG";
	fileOpenParams.defFileId      = DEFOPENFILE_ID;

	return mdlDialog_fileOpenExt (buffer, NULL, &fileOpenParams, 0);
}

/*----------------------------------------------------------------------+
|									|
| name		Option_Hook						|
|									|
| author	BSI					6/90		|
|									|
+----------------------------------------------------------------------*/
Private void Option_Hook( DialogItemMessage   *dimP)
{
	DialogItem	*diP = dimP->dialogItemP;
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType){
		case DIALOG_MESSAGE_CREATE:
		break;
		case DIALOG_MESSAGE_INIT:/* initialize dialog items */
			if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_RadioButton,RBUTTONID_Option1,0)){
				boolean state=TRUE;
				mdlDialog_itemSetState(&state,dimP->db,dimP->itemIndex);
				//mdlDialog_itemSetValue(NULL,0,NULL,"Type CellFileName",dimP->db,diP->itemIndex);
			}else{
				//mdlDialog_dmsgsPrint("can't find TEXTID_CellFileName");
			}
		break;
		default:
	    		dimP->msgUnderstood = FALSE;
	    	break;
        }
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void MessageList_Hook( DialogItemMessage *dimP)
{
	static StringList *strListP = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DIALOG_MESSAGE_CREATE:
		{
			strListP = mdlStringList_create(11,1);
			mdlDialog_listBoxSetStrListP(dimP->dialogItemP->rawItemP,strListP,1);
			break;
		}
		case DITEM_MESSAGE_DESTROY:
		{
			///StringList *strListP;

			///strListP = mdlDialog_listBoxGetStrListP(dimP->dialogItemP->rawItemP);
			if(NULL != strListP) mdlStringList_destroy(strListP);
			strListP = NULL;

			break;
		}
		default:
	    		dimP->msgUnderstood = FALSE;
	    	break;
        }
}

/**
	@brief	get cell name list from active cell library

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int loadCellNameList(StringList  *strListP)
{
	ULong        elemAddr[250], eofPos, filePos, actualPos, readPos;
    	int          scanWords, status, i, numAddr, fileNum=CELL_LIB, offset;
    	char         cellName[10], cellDescription[30], msg[132];
    	Scanlist     scanList;
    	MSElement    cellElm;        /* cell element descriptor */

    	mdlScan_initScanlist (&scanList);
    	mdlScan_noRangeCheck (&scanList);

    	scanList.scantype       = ELEMTYPE | NESTCELL;
    	scanList.extendedType   = FILEPOS;
    	scanList.typmask[0]     = TMSK0_CELL_LIB;

    	mdlScan_initialize (CELL_LIB, &scanList);

    	filePos = 0L;
    	actualPos = 0L;

    	/* --- loop through all cell elements in file --- */
	do
	{
		scanWords = sizeof(elemAddr)/sizeof(short);
		status    = mdlScan_file(elemAddr, &scanWords, sizeof(elemAddr), &filePos);
		numAddr   = scanWords / sizeof(short);

		for (i=0; i<numAddr; i++)
		{
	    		/* --- get cell descriptor --- */
	    		if (mdlElement_read (&cellElm, CELL_LIB, elemAddr[i]) != SUCCESS)
				continue;
	    		else
			{ 
				int type;

				type = mdlElement_getType (&cellElm);

				if (type != 1) continue;

				/* --- get cell name and description --- */
				mdlCnv_fromR50ToAscii ( 6, (short *) &cellElm.cell_lib_hdr.name,cellName);
				mdlCnv_fromR50ToAscii (27, cellElm.cell_lib_hdr.descrip,cellDescription);

				/* --- if we have the correct section type add to string list --- */
				mdlStringList_insertMember(&offset, strListP, -1,1);

				/* --- place blank in column 1 --- */
				mdlStringList_setMember (strListP, offset,cellName, NULL);
			} 
		}
	}while(status == BUFF_FULL);

    	/* --- sort the stringlist by cell name --- */
    	mdlStringList_sortByColumn (strListP, -1, TRUE, NULL,1, 1);

	return SUCCESS;
} 

/*----------------------------------------------------------------------+
|                                                                       |
|	name	GetSelectedListItem					|
|                                                                       |
|	author	?							|
|                                                                       |
+----------------------------------------------------------------------*/
Private int GetSelectedListItem(int nRow,StringList *strListP){
	int	topRow, bottomRow, tagged;
    	char    *stringP;
    	long	*infoField;

    	mdlStringList_getMember(&stringP,&infoField, strListP,nRow);
	strcpy(szCellName,stringP);

	return SUCCESS;
} 

/**
	@brief	set active cell which is selected by user on listbox

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int CellNameListHook(DialogItemMessage *dimP)
{
	int 		row, topRow, bottomRow;
	char		*stringP;
	long    	*infoField;
    	StringList	*strListP=NULL;
    	DialogItem      *diP=NULL;

    	dimP->msgUnderstood = TRUE;
    	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON:
	    	{
	    		/* --- If double-clicked select it --- */
	    		if (dimP->u.button.buttonTrans == BUTTONTRANS_UP &&
				dimP->u.button.upNumber == 2 && dimP->u.button.clicked)
			{
				
				if(NULL != CellNameListBoxP)
				{
					mdlDialog_listBoxGetSelectRange(&row, NULL, NULL, NULL, CellNameListBoxP);
				}

				/* --- get stringlist associated with list box --- */
				strListP = mdlDialog_listBoxGetStrListP(CellNameListBoxP);
				GetSelectedListItem(row,strListP);
				if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_CellName,0))
				{
					mdlParams_setActive (szCellName, ACTIVEPARAM_CELLNAME); //! 선택한 CELL을 ACTIVE CELL로 설정한다.
					mdlDialog_itemSetValue(NULL,0,NULL,szCellName,dimP->db,diP->itemIndex);
				}
			} 
	    		break;
		} 

		case DITEM_MESSAGE_CREATE:
		{ 
	    		/* --- set global variable to list box raw item header --- */
	    		CellNameListBoxP = dimP->dialogItemP->rawItemP;

	    		/* --- create empty master stringlist --- */
	    		strListP = mdlStringList_create (0,1);
	    		loadCellNameList(strListP);

	    		/* --- set list box to display string list for cells --- */
	    		mdlDialog_listBoxSetStrListP (CellNameListBoxP, strListP, 1);
	    		break;
		} 

		case DITEM_MESSAGE_DESTROY:
		{ 
	    		/* --- get pointer to string list to free --- */
	    		///stringListP = mdlDialog_listBoxGetStrListP (listBoxP);

	    		///if (stringListP)
			//{
			//	mdlStringList_destroy (stringListP);
			//	stringListP = NULL;
			//	listBoxP==NULL;
			//} 

	    		/* --- unload this command when dialog box closed --- */
	     		//mdlDialog_cmdNumberQueue (FALSE, CMD_MDL_UNLOAD,mdlSystem_getCurrTaskID(), TRUE);

	    		break;
		} 
		default:dimP->msgUnderstood = FALSE;
			break;
	} 
	return SUCCESS;

} 

/**
	@brief	

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void showMessage(DialogItem   *diP,char* msg)
{
	StringList *strListP;

	strListP = mdlDialog_listBoxGetStrListP(diP->rawItemP);
	if(strListP)
	{
		int newIndex;

		mdlStringList_insertMember(&newIndex,strListP,-1,1);
		mdlStringList_setMember(strListP,newIndex,msg,NULL);
		mdlDialog_listBoxNRowsChanged(diP->rawItemP);
		mdlDialog_listBoxDrawContents(diP->rawItemP,-1,-1);
	}
}

/*----------------------------------------------------------------------+
|									|
| name          makeViewDependent                                       |
|									|
+----------------------------------------------------------------------*/
Private int makeViewDependent( MSElement *elP, void *params, int operation, ULong offset, MSElementDescr  *elmDP )
{
	int viewInd = 0;
	
	/* --- make element view dependent --- */
	mdlElement_setProperties (elP, NULL, NULL, NULL, NULL, NULL,
		NULL, &viewInd, NULL);
	return (SUCCESS);
}

/*----------------------------------------------------------------------+
|									|
| name          setCellSymbology                                        |
|									|
+----------------------------------------------------------------------*/
Private int setCellSymbology( MSElement *elP, void *params, int operation, ULong offset, MSElementDescr *elmDP )
{
	int color = 0;
	int weight = 0;
	int style = 0;
	
	/* --- set element's symbology --- */
	mdlParams_getActive (&color,  ACTIVEPARAM_COLOR);
	mdlParams_getActive (&weight, ACTIVEPARAM_LINEWEIGHT);
	mdlParams_getActive (&style,  ACTIVEPARAM_LINESTYLE);
	
	mdlElement_setSymbology (elP, &color, &weight, &style);
	
	return (SUCCESS);
}

/**
	@brief	add linkage data to element

	@author	HumKyung

	@date	2010.12.16

	@param	

	@return
*/
Private int addUserData( MSElementDescr *elP , ULong filePos )
{
	int 		status;
	char		cMsg[100];
	LinkageHeader	tLinkHdr;
	char            linkString[MAXFILELENGTH];
	LinkageHeader   linkHdr;

	memset (&linkHdr, '\0', sizeof(LinkageHeader));
	linkHdr.primaryID = SIGNATUREID_USERDATAPROJECT;
	linkHdr.user = 1;	//! set user data flag

	//! already stored user data to linkageInfo
	status = mdlLinkage_appendUsingDescr( 
		&elP, 
		&linkHdr, 
		&linkageInfo, 
		RSCID_DataDef_Link, 
		NULL,
		FALSE );

	if (status == SUCCESS )
	{        
		mdlElmdscr_rewrite( elP, NULL, filePos );   
	}

	return (status == SUCCESS) ? MODIFY_STATUS_REPLACE : MODIFY_STATUS_ABORT;	
}

/**
	@brief	place new cell with linkage data

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int generateCell( Dpoint3d *pt, int view, int drawMode, int writeMode )
{
	MSElementDescr  *cellDP = NULL;
	MSElementUnion  el;
	Dpoint3d        scale;
	int             level;
	int             dummy;
	int             status;
	RotMatrix       rMatrix;
	Transform       tMatrix;
	char    activeCell[20];
	ULong		filePos;
	
	//! get cell element from cell library
	mdlParams_getActive (activeCell, ACTIVEPARAM_CELLNAME);
	if(mdlCell_getElmDscr (&cellDP, NULL, 0L, NULL, NULL , NULL , NULL, 0, 0 , activeCell))
	{
		if (cellDP)
		{
			mdlElmdscr_freeAll (&cellDP);
			cellDP = NULL;

			return FALSE;
		}
	}
        
	{		
		/* --- adjust for active angle and view rotation --- */
		mdlTMatrix_getIdentity (&tMatrix);
		mdlRMatrix_fromView (&rMatrix, view, FALSE);
		mdlRMatrix_invert (&rMatrix, &rMatrix);
		mdlTMatrix_rotateByRMatrix (&tMatrix, &tMatrix, &rMatrix);
		mdlTMatrix_rotateByAngles (&tMatrix, &tMatrix, 0.0, 0.0,
			tcb->actangle*fc_piover180);
		mdlElmdscr_transform (cellDP, &tMatrix);
		
		/* --- adjust for active scale --- */
		mdlTMatrix_getIdentity (&tMatrix);
		mdlParams_getActive (&scale, ACTIVEPARAM_SCALE);
		mdlTMatrix_scale (&tMatrix, &tMatrix, scale.x, scale.y, scale.z);
		mdlElmdscr_transform (cellDP, &tMatrix);
		
		/* --- adjust for position --- */
		mdlTMatrix_getIdentity (&tMatrix);
		mdlTMatrix_translate (&tMatrix, &tMatrix, pt->x, pt->y, pt->z);
		mdlElmdscr_transform (cellDP, &tMatrix);
		
		/* --- set level to active level --- */
		mdlParams_getActive (&level, ACTIVEPARAM_LEVEL);
		mdlCell_fixLevels (cellDP, TRUE, level);
		
		/* --- make point cells view dependent --- */
		mdlElmdscr_operation (cellDP, makeViewDependent, &dummy , ELMD_ALL_ONCE);
		
		/* --- set color weight and style to current --- */
		mdlElmdscr_operation (cellDP, setCellSymbology, &dummy, ELMD_ALL_ONCE);
		
		/* --- display cell --- */
		mdlElmdscr_display (cellDP, 0, drawMode);
		if (writeMode)
		{
			filePos = mdlElmdscr_add(cellDP);
			addUserData(cellDP, filePos);
		}
		
		if (cellDP)
		{
			mdlElmdscr_freeAll (&cellDP);
			cellDP = NULL;
		}
        }
	
	return  SUCCESS;
}

/**
	@brief	

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void placeCell( Dpoint3d *pt, int view )
{
	generateCell (pt, view, NORMALDRAW, 1);
	
	/* --- free element descriptor --- */
	/*
	if (cellDescrP)
        {
		mdlElmdscr_freeAll (&cellDescrP);
		cellDescrP = NULL;
        }
	*/

	mdlState_restartCurrentCommand ();
}

/*----------------------------------------------------------------------+
|									|
| name          dragCell -- dynamics function                           |
|									|
+----------------------------------------------------------------------*/
Private int dragCell( Dpoint3d *pt, int	view, int drawMode )
{
	generateCell (pt, view, drawMode, 0);
	
	return (SUCCESS);
}

/*----------------------------------------------------------------------+
|									|
| name          cleanUp                                                 |
|									|
+----------------------------------------------------------------------*/
Private void cleanUp()
{
	tcb->cugraf=0;
	
	///if (cellDescrP)
	///	mdlElmdscr_freeAll (&cellDescrP);
	
	mdlState_startDefaultCommand();
}

/**
	@brief	start command for placing cell

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
cmdName placeCellCurrentCmd( void )
{
	mdlState_startPrimitive (placeCell, placeCellCurrentCmd, 1, 2);
	
	mdlState_setFunction (STATE_COMPLEX_DYNAMICS, dragCell);
	mdlState_setFunction (STATE_RESET,            cleanUp);
	
	return (SUCCESS);
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int getLinkageDataFromDialog( DialogItemMessage* dimP )
{
	if(NULL != dimP)
	{
		DialogItem* diP  =dimP->dialogItemP;

		memset(&linkageInfo , 0 , sizeof(linkageInfo));

		//! Get Name
		if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Name,0))
		{
			mdlDialog_itemGetValue(NULL,NULL, linkageInfo.cName ,dimP->db,diP->itemIndex, INFO_BUF_SIZ);
			if('\0' == linkageInfo.cName[0])
			{
				mdlDialog_dmsgsPrint("there is no name which is necessary!");
				return FALSE;
			}
		}
		//! Get Desc1
		if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc1,0))
		{
			mdlDialog_itemGetValue(NULL,NULL, linkageInfo.cDesc1 ,dimP->db,diP->itemIndex, INFO_BUF_SIZ);
		}else	return FALSE;
		//! Get Desc2
		if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc2,0))
		{
			mdlDialog_itemGetValue(NULL,NULL, linkageInfo.cDesc2 ,dimP->db,diP->itemIndex, INFO_BUF_SIZ);
		}else	return FALSE;
		//! Get Desc3
		if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc3,0))
		{
			mdlDialog_itemGetValue(NULL,NULL, linkageInfo.cDesc3 ,dimP->db,diP->itemIndex, INFO_BUF_SIZ);
		}else	return FALSE;

		return SUCCESS;
	}

	return FALSE;
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void aCellModelingTool_ButtonHook(DialogItemMessage* dimP)
{
	char* ptr=NULL;
	char  szPrint[MAXFILELENGTH],activeCell[20],szActiveCellName[256];
	int   dummy;
	DialogItem* diP  =dimP->dialogItemP;
	RawItemHdr* rihP =dimP->dialogItemP->rawItemP;
	dimP->msgUnderstood = TRUE;

	switch(dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
			mdlDialog_dmsgsClear();
		break;
		case DITEM_MESSAGE_BUTTON: //! button is pressed
			if((dimP->u.button.buttonNumber != DATAPNT) ||
			   (dimP->u.button.buttonTrans  != BUTTONTRANS_UP))
				break;
			
			if(0 == RadioButtonInfo.state)	//! place new cell
			{
				/*if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_CellFileName,0))
				{
					mdlDialog_itemGetValue(NULL,NULL,szCellFileName,dimP->db,diP->itemIndex,MAXFILELENGTH);
					strcpy(szCellFilePath,szCellFileName);
					strcpy(szNewCellFilePath,szCellFileName);
					if(ptr = strstr(szNewCellFilePath,".")){
						(*ptr) = '\0';
						strcat(ptr,"+.cel");
					}
				}else	return;
				*/

				//! Get Cell Name
				if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_CellName,0))
				{
					mdlDialog_itemGetValue(NULL,NULL,szCellName,dimP->db,diP->itemIndex,MAXFILELENGTH);
					if('\0' == szCellName[0])
					{
						mdlDialog_dmsgsPrint("type in cell name!");
						return;
					}
				}else	return;

				if(SUCCESS == getLinkageDataFromDialog(dimP))
				{
					if(linkageInfo.cName[0])
					{
						placeCellCurrentCmd();
					}
					else	//! show error message
					{
						diP = dimP->dialogItemP;
						if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
						{
							showMessage(diP,"please, input name which is necessary.");
						}else	mdlDialog_dmsgsPrint("Can't show message");
					}
				}
			}
			else if(1 == RadioButtonInfo.state) //! editing
			{
				if(mdlSelect_isActive())
				{
					MSElementDescr* elemDescrPP=NULL;
					ULong filePos = 0;

					filePos = mdlElement_getFilePos(FILEPOS_CURRENT,0);
					if((0xffffffff != filePos) && mdlElmdscr_read(&elemDescrPP, filePos, 0, 0, NULL))
					{
						if(2 == elemDescrPP->el.ehdr.type)	//! it is cell.
						{
							//! delete linkage data
							mdlLinkage_deleteUsingDescr(&elemDescrPP , SIGNATUREID_USERDATAPROJECT , RSCID_DataDef_Link , NULL , NULL , NULL , FALSE);

							if(SUCCESS == getLinkageDataFromDialog(dimP))
							{
								addUserData(elemDescrPP, filePos);
							}
						}
						
						mdlElmdscr_freeAll(&elemDescrPP);
					}
				}
			}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

/**
	@brief	get item type from string

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
int getItemTypeFromString(char* pString)
{
	if(NULL != pString)
	{
		if(0 == strcmpi("POWER" , pString))
			return POWER;
		else if(0 == strcmpi("GROUNDING" , pString))
			return GROUNDING;
		else if(0 == strcmpi("LIGHTING" , pString))
			return LIGHTING;
		else if(0 == strcmpi("FIRE ALARM" , pString))
			return FIRE_ALARM;
		else if(0 == strcmpi("COMMUNICATION" , pString))
			return COMMUNICATION;
		else if(0 == strcmpi("COMMUNICATION" , pString))
			return COMMUNICATION;
		else if(0 == strcmpi("ETC" , pString))
			return ETC;
	}

	return LIGHTING;
}

/**
	@brief	extract user data from cell

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
int electinfo_linkFunc( ElectLinkData* linkageP, void* paramsP )
{
	char data[256];
	if(NULL != linkageP)
	{
		memset(&linkageInfo , 0 , sizeof(ElectLinkInfo));
		strcpy(linkageInfo.cType , linkageP->electlinkageInfo.cType);
		strcpy(linkageInfo.cName , linkageP->electlinkageInfo.cName);
		strcpy(linkageInfo.cType , linkageP->electlinkageInfo.cType);
		strcpy(linkageInfo.cDesc1, linkageP->electlinkageInfo.cDesc1); 
		strcpy(linkageInfo.cDesc2, linkageP->electlinkageInfo.cDesc2); 
		strcpy(linkageInfo.cDesc3, linkageP->electlinkageInfo.cDesc3); 
	}	

	return SUCCESS;
}

/**
	@brief	call mdlLinkage_extractUsingDescr to extract linkage data from cell

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private int extractUserData( MSElementDescr *elP )
{
	void	*nextLink = NULL;
	char	data[256];

	if(NULL != elP)
	{
		nextLink = mdlLinkage_extractUsingDescr(NULL, elP, SIGNATUREID_USERDATAPROJECT, 
			RSCID_DataDef_Link, NULL, electinfo_linkFunc, NULL,TRUE);
	}

	return (NULL != nextLink) ? SUCCESS : FALSE;
}

/**
	@brief

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void aCellModelingTool_InfoHook(DialogItemMessage* dimP)
{
	char* ptr=NULL;
	char  szPrint[MAXFILELENGTH],activeCell[20],szActiveCellName[256] , szUserData[128],szBuf[MAXFILELENGTH];
	int   dummy;
	ULong filePosition = 0;
	DialogItem* diP  =dimP->dialogItemP;
	RawItemHdr* rihP =dimP->dialogItemP->rawItemP;
	dimP->msgUnderstood = TRUE;

	switch(dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
			mdlDialog_dmsgsClear();
		break;
		case DITEM_MESSAGE_BUTTON: //! button is pressed
			if((dimP->u.button.buttonNumber != DATAPNT) ||
			   (dimP->u.button.buttonTrans  != BUTTONTRANS_UP))
				break;
			
			if(mdlSelect_isActive())
			{
				MSElementDescr* elemDescrPP=NULL;

				filePosition = mdlElement_getFilePos(FILEPOS_CURRENT,0);
				if((0xffffffff != filePosition) && mdlElmdscr_read(&elemDescrPP,filePosition,0,0,NULL))
				{
					if(2 == elemDescrPP->el.ehdr.type)	//! it is cell.
					{
						if(SUCCESS == extractUserData(elemDescrPP))
						{
							CellInfo.type = getItemTypeFromString(linkageInfo.cType);

							if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Name,0))
							{
								mdlDialog_itemSetValue(NULL,0,NULL,linkageInfo.cName , dimP->db , diP->itemIndex);
							}
							if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc1,0))
							{
								mdlDialog_itemSetValue(NULL,0,NULL,linkageInfo.cDesc1 , dimP->db , diP->itemIndex);
							}
							if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc2,0))
							{
								mdlDialog_itemSetValue(NULL,0,NULL,linkageInfo.cDesc2 , dimP->db , diP->itemIndex);
							}
							if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_Desc3,0))
							{
								mdlDialog_itemSetValue(NULL,0,NULL,linkageInfo.cDesc3 , dimP->db , diP->itemIndex);
							}
							/* --- synch cell library name --- */
							mdlDialog_itemsSynch (dimP->db);
						}
					}
					else
					{
						if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
						{
							showMessage(diP, "selected item is not cell.");
						}
					}

					mdlElmdscr_freeAll(&elemDescrPP);	//! free delement description
				}else	mdlDialog_dmsgsPrint("Can't Read Element");
				///mdlSelect_freeAll();
			}
			else
			{
				if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
				{
					showMessage(diP , "First of all, select a cell.");
				}
			}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

/**
	@brief	사용자가 선택한 cell library을 attach시키고 cell이름을 출력한다.

	@author	HumKyung

	@date	2010.12.16

	@param

	@return
*/
Private void GetFileName_Hook(DialogItemMessage* dimP)
{
	DialogItem *diP = dimP->dialogItemP;
	char szFilePath[MAXFILELENGTH];

	dimP->msgUnderstood = TRUE;
	switch(dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON: /* button is pressed */
			if((dimP->u.button.buttonNumber != DATAPNT) || (dimP->u.button.buttonTrans  != BUTTONTRANS_UP))
				break;

			if(0 == GetFileName(szFilePath))
			{
				if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_Text,TEXTID_CellFileName,0))
				{
					mdlDialog_itemSetValue(NULL,0,NULL,szFilePath,dimP->db,diP->itemIndex);
					
					/* --- if cell library has changed reload string list --- */
	    				if(0 != strcmp(szFilePath,tcb->celfilenm))
					{
						StringList *strListP=NULL;

						loadCellLibraryIfNeeded(szFilePath);

						/* --- get stringlist associated with list box --- */
						strListP = mdlDialog_listBoxGetStrListP(CellNameListBoxP);
		
						/* --- delete the contents of the original stringlist --- */
						mdlStringList_deleteMember(strListP, 0, -1);

						/* --- reload string list --- */
						loadCellNameList(strListP);

						/* --- tell dialog manager number of rows have changed --- */
						mdlDialog_listBoxNRowsChanged(CellNameListBoxP);

						/* --- redraw stringlist --- */
						mdlDialog_listBoxDrawContents (CellNameListBoxP, -1, -1);

						/* --- synch cell library name --- */
						mdlDialog_itemsSynch (dimP->db);
					} 
				}
#ifdef	DEBUG				
				mdlDialog_dmsgsPrint(szFilePath);
#endif
			}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

/**
	@brief	cell type을 선택했을때 호출된다.
	
	@author	HumKyung

	@date	20.10.12.16

	@param

	@return
*/
Private void OptionButton_Hook( DialogItemMessage   *dimP)
{
	char szBuf[16];
	DialogItem *diP;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DIALOG_MESSAGE_INIT:/* initialize dialog items */
			strncpy(linkageInfo.cType , "LIGHTING" , INFO_BUF_SIZ - 1);
		break;
		case DITEM_MESSAGE_STATECHANGED:
			if(dimP->u.stateChanged.reallyChanged)
			{
				if(POWER == CellInfo.type)
				{
					strncpy(linkageInfo.cType , "POWER" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select POWER");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
				else if(GROUNDING == CellInfo.type)
				{
					strncpy(linkageInfo.cType , "GROUNDING" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select GROUNDING");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
				else if(LIGHTING == CellInfo.type)
				{
				    	strncpy(linkageInfo.cType , "LIGHTING" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select LIGHTING");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
				else if(FIRE_ALARM == CellInfo.type)
				{
				    	strncpy(linkageInfo.cType , "FIRE ALARM" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select FIRE ALARM");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
				else if(COMMUNICATION == CellInfo.type)
				{
				    	strncpy(linkageInfo.cType , "COMMUNICATION" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select COMMUNICATION");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
				else if(ETC == CellInfo.type)
				{
				    	strncpy(linkageInfo.cType , "ETC" , INFO_BUF_SIZ - 1);

					diP = dimP->dialogItemP;
					if(diP = mdlDialog_itemGetByTypeAndId(dimP->db,RTYPE_ListBox,LISTID_MessageList,0))
					{
						showMessage(diP,"Select ETC");
					}else	mdlDialog_dmsgsPrint("Can't show message");
				}
			}
			mdlDialog_synonymsSynch(NULL,DIALOGID_CellPicker,NULL);
		break;
		case DIALOG_MESSAGE_PREBUTTON:
			mdlDialog_dmsgsPrint("Press Button");
		break;
		default:
	    		dimP->msgUnderstood = FALSE;
	    	break;
        }
}

/*----------------------------------------------------------------------+
|									|
| name		main							|
|									|
| author	Hum-Kyung Baek     			8/89		|
|									|
+----------------------------------------------------------------------*/
static DialogHookInfo uHooks[] =
{
	{HOOKITEMID_CellButton  ,aCellModelingTool_ButtonHook},
	{HOOKITEMID_GetFileName ,GetFileName_Hook},
	{HOOKITEMID_OptionButton,OptionButton_Hook},
	{HOOKITEMID_MessageList ,MessageList_Hook},
	{HOOKITEMID_CellNameList,CellNameListHook},
	{HOOKITEMID_Info        , aCellModelingTool_InfoHook},
};

/**
	@brief	

	@author	HumKyung

	@date	2010.12.16

	@pram

	@return
*/
Public int  main (int argc,char *argv[]) 
{
	char	    *setP=NULL;
    	RscFileHandle   rfHandle;
	void* rscTypeInfo;

	
    	//! Open our file for access to dialog
    	mdlResource_openFile (&rfHandle, NULL, FALSE);

    	//! Publish the dialog item hooks
    	mdlDialog_hookPublish (sizeof(uHooks)/sizeof(DialogHookInfo), uHooks);
	CellInfo.type = 2;
	
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX,0,FALSE);
	rscTypeInfo = (void *)mdlCExpression_typeFromRsc(setP,"tagElectlinkinfo",NULL);
	mdlCExpression_symbolPublish(setP,"linkageInfo",SYMBOL_CLASS_VAR,(CType *)rscTypeInfo,(void *)&linkageInfo);
	
 	mdlDialog_publishComplexVariable(setP,"tagOptionButtonInfo","CellInfo",&CellInfo);
	RadioButtonInfo.state = 0;
	mdlDialog_publishComplexVariable(setP,"tagRadioButtonInfo","RadioButtonInfo",&RadioButtonInfo);

	if(NULL == mdlDialog_open(NULL,DIALOGID_CellPicker))
	{
		mdlDialog_dmsgsPrint("can't open dialog");
	}
	
    	return SUCCESS;
}
