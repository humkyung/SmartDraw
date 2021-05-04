#pragma Version 0:1:1
/*----------------------------------------------------------------------+
|																									|
|	$Workfile:   w2Dgn.mc  $
|	$Revision: 1.31 $
|  	$Date: 2011/07/14 07:32:52 $
|  	$Author: HoSang Jeon , HumKyung Baek , Suchita Malgaonkar
|	$Remark: Part of Hanwha E&C Holeman and DAELIM Project
|																             						|
+----------------------------------------------------------------------*/

#include <mdl.h>
#include <cexpr.h>
#include <rscdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmdlist.h>
#include <dlogitem.h>
#include <system.h>
#include <global.h>
#include <userfnc.h>
#include <mselems.h>
#include <refernce.h>
#include <tcb.h>
#include <scanner.h>
#include <mdldim.h>
#include <dlogman.fdf>
#include <msdim.fdf>
#include <mdlerrs.h>
#include <clipdscr.h>
#include <msfile.fdf>
#include <mssystem.fdf>

#include "aDgnV7.h"
#include "Dimension.h"
#include "Insert.h"
#include "Cloud.h"

//Suhas
#define OCTAGONE_ANGLE	22.5
#define MAX_LENGTH	256

//up to here
#define _CRT
// Global variable
FileInfo FI;
char SZ_BUF[LINE_BUF_SIZ];
MSElementDescr *chainDscrP=NULL;
MSElementUnion groupCell;
Private	int		line, txt, Ntxt, cell;

// 일반 Function
Private void nEnd();         
Private void ProcessCmd(char *filename);
Private void ProcessTag(char *filename);

Public boolean Draw_Line(Dpoint3d *ptStart, Dpoint3d *ptEnd, int nColor, int nLayer, int nStyle, float fScale, int nWeight);
Private boolean Draw_Text(Dpoint3d *ptStart, float fHeight, float fWidth, float fAngle, char* sText, int nJustify, int Obliquing, int nColor, int nLayer, int nStyle, int nWeight);
Private boolean Draw_Rectangle(Dpoint3d *ptStart, Dpoint3d *ptOthe, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth);
Private boolean Draw_Circle(Dpoint3d *ptCenter, float fRadius, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, int nFillMode);
Private boolean Draw_Donut(float fInsideDia, float fOutsideDia, Dpoint3d *ptCenter, int nColor, int nLayer);
Private boolean Draw_Arc(Dpoint3d *ptCenter, float fRadius, double StartAngle, double EndAngle, int nColor, int nLayer, int nStyle,  float fScale, int nWeight);
Private boolean Draw_PLine(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount , int iAddFill);
Private boolean Draw_PointString(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount , int disjoint);
Private boolean Draw_Polygon(Dpoint3d *ptCenter, int nNumSide, float fRadius, float fAngle, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth);
Private boolean Draw_HatchPLine(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fAngle, float fScale , int nWidth, int nLineCount,char* sYesNo, char* cCellLib, char* cCellName);
Private boolean Draw_HatchCircle(Dpoint3d *ptCenter, float fRadius, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fAngle, char* sYesNo, char* cCellLib, char* cCellName);
Private boolean Draw_Leader(Dpoint3d *ptPoint, int nColor, int nLayer, char* sArrowType, double nArrowWidth , double nArrowHeight , int nLineCount , int iLineWeight);
Private boolean Sub_Draw_Polygon(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount, int nFillMode);
/// Suhas
Private boolean Draw_HatchRect(Dpoint3d *ptStart, Dpoint3d *ptEnd, Dpoint3d *ptStart1, Dpoint3d *ptEnd1, int nColor, int nLayer, int nStyle, float fAngle, float fScale, int nWeight, char* sYesNo, char* cCellLib, char* cCellName);
Private boolean Draw_HatchDonut(float fInsideDia, float fOutsideDia, Dpoint3d *ptCenter, int nColor, int nLayer,int nStyle,float fAngle, float fScale, char* sYesNo, char* cCellLib, char* cCellName);
Private boolean Draw_Ellipse(Dpoint3d *ptStart, float fPrimaryAxis, float fScndAxis, float fAngle, int nColor, int nLayer, int nStyle, float fScale, int nWeight);

extern Public int loadCellLibraryIfNeeded(char *libToLoad);
extern int GetCellNameFromCellLib(char* pOutputTextFilePath , char* pstrCellLib);

LinkInfo linkInfo;
//up to here

/**
	@brief	entry point

	@author	

	@return	
*/
Private void main(int argc,	char *argv[])
{	
	Dpoint3d origin;
	RotMatrix rotMatrix;

	if((4 == argc) && (0 == strcmp(argv[3] , "ACS")))
	{
		origin.x = 100.0;
		origin.y = 100.0;
		origin.z = 100.0;
		rotMatrix.form2d[0][0] = 1.0;
		rotMatrix.form2d[0][1] = 0.0;
		rotMatrix.form2d[1][0] = 0.0;
		rotMatrix.form2d[1][1] = 1.0;
		mdlACS_setCurrent (&origin, &rotMatrix,NULL);

		mdlACS_getCurrent (&origin, &rotMatrix,NULL);
		sprintf(SZ_BUF , "%lf,%lf,%lf" , origin.x , origin.y , origin.z);
		mdlDialog_dmsgsPrint(SZ_BUF);
	}
	else if((5 == argc) && (0 == strcmp(argv[4] , "CELL")))
	{
		GetCellNameFromCellLib(argv[2] , argv[3]);
	}
	/// 2014.04.01 added by humkyung
	else if((4 == argc) && ((0 == strcmp(argv[2] , "SCALE")) || (0 == strcmp(argv[2] , "GRAY"))))
	{
		ULong elemAddr[50], eofPos, filePos;
		int scanWords, numChanged=0, status, i, numAddr;
		Scanlist scanList;
		int grayColor = atoi(argv[3]);///32;
		double dScale = atof(argv[3]);
		MSElementUnion el;
		
		mdlScan_initScanlist (&scanList);
		mdlScan_noRangeCheck (&scanList);
		scanList.scantype	    = ELEMTYPE;
		scanList.typmask[0]	    = TMSK0_LINE|TMSK0_SHAPE|TMSK0_LINE_STRING|TMSK0_TEXT_NODE|TMSK0_CELL_HEADER|TMSK0_ARC|TMSK0_ELLIPSE;
		scanList.typmask[1]	    = TMSK1_TEXT;
		scanList.typmask[2]		= TMSK2_DIMENSION|TMSK2_ATTRIBUTE;
		scanList.extendedType = FILEPOS;
		
		mdlScan_initialize (0, &scanList);
		do
		{
			scanWords = sizeof(elemAddr)/sizeof(short);
			status	  = mdlScan_file (elemAddr, &scanWords, sizeof(elemAddr), &filePos);
			numAddr   = scanWords / sizeof(short);

			for (i=0; i<numAddr; i++)
			{
				if(SUCCESS == mdlElement_read( &el, 0, elemAddr[i] ))
				{
					/// scale up/down with given value - 2013.10.18 added by humkyung
					Transform tMatrix;
					mdlTMatrix_getIdentity(&tMatrix);
					mdlTMatrix_scale(&tMatrix,&tMatrix,dScale,dScale,0);
					mdlElement_transform(&el,&el,&tMatrix);
					/// up to here

					if(0 == strcmp(argv[2] , "GRAY"))	/// set element's color to gray when GRAY option is on - 2013.11.19 added by humkyung
					{
						mdlElement_setSymbology(&el , &grayColor , NULL , NULL);
					}
					mdlElement_rewrite(&el , NULL , elemAddr[i]);
				}
			}
		} while (status == BUFF_FULL);
	}
	/// up to here
	else
	{
		int iParam = 0;
		Dpoint3d ptScale;

		//! set area mode to SOLID - by HumKyung.Baek
		mdlParams_setActive(0 , ACTIVEPARAM_AREAMODE);
		/*
		ptScale.x = 1.0;
		ptScale.y = 1.0;
		ptScale.z = 1.0;
		mdlParams_setActive(&ptScale , ACTIVEPARAM_SCALE);
		*/

		ProcessCmd(argv[2]);
	}

	nEnd();
}

Private void nEnd()     
{
	mdlState_clear();
	mdlOutput_prompt("");
	mdlState_startDefaultCommand();
	mdlDialog_cmdNumberQueue (FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), TRUE);
}

/**
	@brief	add linkage data to element

	@author	HumKyung

	@date	2010.12.16

	@param	

	@return
*/
Private int addUserData( MSElementDescr **elP , ULong filePos )
{
	int 		status;
	char		cMsg[100];
	char            linkString[MAXFILELENGTH];
	LinkageHeader   linkHdr;

	memset(&linkHdr, '\0' , sizeof(LinkageHeader));
	linkHdr.info		=0;
	linkHdr.remote		=0;
	linkHdr.modified	=0;
	linkHdr.user		=1;
	linkHdr.class		=0;
	linkHdr.words;
	linkHdr.primaryID	= 901;

	memset(&linkInfo , '\0' , sizeof(LinkInfo));
	strcpy(linkInfo.tag_Name1 , "hello");
	///strcpy(_PlantSpaceDataInfo.szBuf , "hello");
	///memcpy(_PlantSpaceDataInfo.szBuf , 
	///	"HGRPT_ASSY                                          S 0-333-G-WV713-001        486457634 P,LK 2 1/2X2 1/2X1/4,-,20.000 ,--                                                                                                                                                                           BA                                                                                                                                                                                                                     " , 508);
	//! already stored user data to linkageInfo
	///status = mdlLinkage_appendToElement( 
	status = mdlLinkage_appendUsingDescr( 
		elP, 
		&linkHdr, 
		&linkInfo, 
		RSCID_DataDef_Link, 
		NULL,
		FALSE);

	if (status == SUCCESS )
	{        
		///mdlElmdscr_rewrite( elP, NULL, filePos );
		//mdlElement_rewrite( &(elP->el) , NULL, filePos );
	}
	else
	{
		PrintError ("\nError adding user data!");
	}

	return (status == SUCCESS) ? MODIFY_STATUS_REPLACE : MODIFY_STATUS_ABORT;	
}

/**
	@brief	Line을 생성한다.

	@author	전호상,HumKyung.Baek

	@param

	@return
*/
Public boolean Draw_Line(Dpoint3d *ptStart, Dpoint3d *ptEnd, int nColor, int nLayer, int nStyle, float fScale, int nWeight)
{
	Dpoint3d  createLinePnt[2];  
	MSElementUnion newElement; 
	int stat;
	MSElementDescr *elDescrP = NULL;
	ULong filePos = 0;

	MSElementUnion cellElm;
	MSElementDescr* cellElP;

	double unitVal = tcb->uorpersub;

	createLinePnt[0].x = ptStart->x * unitVal;
	createLinePnt[0].y = ptStart->y * unitVal;
	createLinePnt[0].z = ptStart->z * unitVal;

	createLinePnt[1].x = ptEnd->x * unitVal;
	createLinePnt[1].y = ptEnd->y * unitVal;
	createLinePnt[1].z = ptEnd->z * unitVal;

	/*stat = mdlCell_create (&cellElm , NULL, &createLinePnt[0] , FALSE);
	stat = mdlElmdscr_new (&cellElP, NULL, &cellElm);*/

	stat = mdlLine_create(&newElement, NULL, createLinePnt); 
	if(SUCCESS != stat) 
	{
		PrintError ("Error creating graphic element!");
		return FALSE;
	}		
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);

	if(chainDscrP == NULL)
	{
		mdlElement_display(&newElement, NORMALDRAW);	
		stat = mdlElement_add(&newElement);	

		if(stat == 0) 
		{
			PrintError ("\nError adding graphic element!");
			return FALSE;
		}
	}
	else
	{
		stat = mdlElmdscr_new(&elDescrP,NULL,&newElement);
		if(stat == 0) 
		{
			//PrintError ("Success creating descriptor !");
		}

		stat = mdlElmdscr_appendDscr(chainDscrP,elDescrP);
		if(stat > 0) 
		{
			//PrintError ("Success appending descriptor !");
		}
	}
	return TRUE;
}

/**
	@brief	

	@author	

	@date	

	@return
*/
Private boolean Draw_Arc(Dpoint3d *ptCenter, float fRadius, double StartAngle, double EndAngle, int nColor, int nLayer, int nStyle,  float fScale, int nWeight)
{
	MSElementUnion newElement;
	int i, stat; 
	double scale;
	MSElementDescr* lineElP = NULL; //Added by suchita for GG

	double unitVal = tcb->uorpersub , dSweepAng = EndAngle-StartAngle;

	ptCenter->x *= unitVal;
	ptCenter->y *= unitVal;
	ptCenter->z *= unitVal;
	if(dSweepAng < 0.0) dSweepAng += 360.;
	stat = 	mdlArc_create(&newElement, NULL, ptCenter, fRadius*unitVal, fRadius*unitVal, NULL, GetRadian(StartAngle), GetRadian(dSweepAng));
	if(stat != SUCCESS) 
	{
		PrintError ("Error creating graphic element!");
		return FALSE;
	}	
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);

	if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);	
		stat = mdlElement_add(&newElement);
		if(stat == 0) 
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}	
	}
	//Added by suchita for GG
	else
	{
		stat = mdlElmdscr_new (&lineElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, lineElP);
	}
	mdlCurrTrans_end();
	//End Addition by suchita for GG
	return TRUE;
}

/**
	@brief	draw a circle

	@author	전호상,HumKyung.Baek

	@date	????.??.??

	@return	boolean
*/
Private boolean Draw_Circle(Dpoint3d *ptCenter, float fRadius, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, int nFillMode)
{
	MSElementDescr *patternEdPP,*edP , *ptr;
	MSElementUnion newElement;
	int i, stat; 
	//Added by suchita for GG
	MSElementDescr* circleElP = NULL;

	double unitVal = tcb->uorpersub;

	ptCenter->x = ptCenter->x * unitVal;
	ptCenter->y = ptCenter->y * unitVal;
	ptCenter->z = ptCenter->z * unitVal;

	stat = mdlEllipse_create(&newElement, NULL, ptCenter, fRadius*unitVal, fRadius*unitVal, NULL, nFillMode);
	if(stat != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}		
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);

	if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);
		stat = mdlElement_add(&newElement);						 
		if(stat == 0) 
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}
	}
	//Added by suchita for GG
	else
	{
		stat = mdlElmdscr_new (&circleElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, circleElP); 	
	}
	
	return TRUE;
}

/**
	@brief	LineString을 생성한다.

	@author	전호상,백흠경

	@date	????.??.??
*/
Private boolean Draw_PLine(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount , int iAddFill)
{
	int res = SUCCESS , stat , i = 0;
	MSElementUnion newElement;
	MSElementDescr* polylineElP = NULL;
	double unitVal = tcb->uorpersub;

	for( i = 0;i < nLineCount;i++)
	{
		ptPLine[i].x = ptPLine[i].x * unitVal;
		ptPLine[i].y = ptPLine[i].y * unitVal;
		ptPLine[i].z = fc_zero;
	}

	res = mdlLineString_create(&newElement , NULL, ptPLine , nLineCount);
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);		
		stat = mdlElement_add(&newElement);						   
		if(stat == 0) 
		{
			PrintError ("\nError adding graphic element!");
			return FALSE;
		}
	}
	//Added by suchita for GG
	else
	{
		stat = mdlElmdscr_new (&polylineElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, polylineElP); 	
	}

	return TRUE;
}

/**
	@brief	create a PointString

	@author	HumKyung.Baek

	@date	2010.11.15

	@return	boolean
*/
Private boolean Draw_PointString(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount , int disjoint)
{
	int res = SUCCESS , stat , i = 0;
	MSElementUnion newElement;
	MSElementDescr* polylineElP = NULL;
	double unitVal = tcb->uorpersub;
	double rMatrices[9];

	for( i = 0;i < nLineCount;i++)
	{
		ptPLine[i].x = ptPLine[i].x * unitVal;
		ptPLine[i].y = ptPLine[i].y * unitVal;
		ptPLine[i].z = fc_zero;
	}

	rMatrices[0] = 1;rMatrices[1] = 0;rMatrices[2] = 0;
	rMatrices[3] = 0;rMatrices[4] = 1;rMatrices[5] = 0;
	rMatrices[6] = 0;rMatrices[7] = 0;rMatrices[8] = 1;

	res = mdlPointString_create(&newElement , NULL, ptPLine , NULL , nLineCount , TRUE);
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	
	//if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);		
		stat = mdlElement_add(&newElement);						   
		if(stat == 0) 
		{
			PrintError ("\nError adding graphic element!");
			return FALSE;
		}
	}
	//Added by suchita for GG
	/*
	else
	{
		stat = mdlElmdscr_new (&polylineElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, polylineElP); 	
	}
	*/
	return TRUE;
}

/**
	@brief	create rectangle by using shape.

	@author	전호상,백흠경
*/
Private boolean Draw_Rectangle(Dpoint3d *ptStart, Dpoint3d *ptOthe, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth)
{	
	Dpoint3d  createLinePnt[5];
	MSElementDescr *patternEdPP,*edP;
	MSElementUnion newElement;
	int stat;

	double unitVal = tcb->uorpersub;

	createLinePnt[0].x = createLinePnt[4].x = ptStart->x * unitVal;
	createLinePnt[0].y = createLinePnt[4].y = ptStart->y * unitVal;
	createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	createLinePnt[1].x = ptStart->x * unitVal;
	createLinePnt[1].y = ptOthe->y * unitVal;
	createLinePnt[1].z = fc_zero;

	createLinePnt[2].x = ptOthe->x * unitVal;
	createLinePnt[2].y = ptOthe->y * unitVal;
	createLinePnt[2].z = fc_zero;

	createLinePnt[3].x = ptOthe->x * unitVal;
	createLinePnt[3].y = ptStart->y * unitVal;
	createLinePnt[3].z = fc_zero;

	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);
	if(stat != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}		
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);		
		stat = mdlElement_add(&newElement);						  
		if(stat == 0) 
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}	
	}
	//Added by suchita for GG
	else
	{
		stat = mdlElmdscr_new (&edP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, edP); 	
	}
	return TRUE;
}

/**
	@brief	create sub polygon by using shape.
	@author	HumKyung.BAEK
	@date	????.??.??
	@return	boolean
*/
Private boolean Sub_Draw_Polygon(Dpoint3d *ptPLine, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount, int nFillMode)
{
	MSElementDescr *patternEdPP,*edP, *lineElP;
	Dpoint3d  *createLinePnt;  
	MSElementUnion newElement; 
	MSElementDescr* polylineElP = NULL;

	int i, stat; 

	double unitVal = tcb->uorpersub;

	createLinePnt = (DPoint3d*)malloc(nLineCount * sizeof(DPoint3d));
	for(i=0; i<nLineCount; i++)
	{					
		createLinePnt[i].x = (ptPLine+i)->x * unitVal;
		createLinePnt[i].y = (ptPLine+i)->y * unitVal;
		createLinePnt[i].z = (ptPLine+i)->z * unitVal;
	}

	stat = mdlShape_create(&newElement, NULL, createLinePnt, nLineCount, nFillMode);
	if(stat != SUCCESS) 
	{
		PrintError ("Error creating graphic element!");
		return FALSE;
	}
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);

	if(chainDscrP == NULL)	//added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);	
		stat = mdlElement_add(&newElement);						 
		if(stat == 0) 
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}	
	}
	//Added by suchita for GG
	else
	{
		stat = mdlElmdscr_new (&polylineElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, polylineElP); 	
	}
	
	return TRUE;
}

/**
	@brief	create polygon

	@author	HoSang.Jeon,HumKyung.Baek

	@date	????.??.??

	@return	boolean
*/
Private boolean Draw_Polygon(Dpoint3d *ptCenter, int nNumSide, float fRadius, float fAngle, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth)
{
	int i;
	float angle;
	Dpoint3d *ptPoint;
	MSElement poly;
	MSElementDescr* polyElP = NULL;
	double unitVal = tcb->uorpersub;

	if(nNumSide<3)
	{
		PrintError("Range of point number is [3~1024]");
		return FALSE;
	}

	fRadius = fRadius * unitVal;
	ptCenter->x = ptCenter->x * unitVal;
	ptCenter->y = ptCenter->y * unitVal;

	ptPoint = (DPoint3d*)malloc((nNumSide+1) * sizeof(DPoint3d));
	for(i=0; i<nNumSide; i++)
	{
		angle = ((fc_pi * 2) / nNumSide * i) + GetRadian(fAngle);
		(ptPoint+i)->x = ptCenter->x + cos(angle) * fRadius;
		(ptPoint+i)->y = ptCenter->y + sin(angle) * fRadius;
	}

	angle = (fc_pi * 2) / nNumSide * i	;
	(ptPoint+i)->x = ptPoint->x;
	(ptPoint+i)->y = ptPoint->y;

	if ( SUCCESS != mdlShape_create(&poly,NULL,ptPoint,nNumSide+1,0) )
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}
	else
	{
		mdlElement_setSymbology(&poly, &nColor, &nWeight, &nStyle);
		mdlElement_setProperties(&poly , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);

		if(chainDscrP == NULL)	//added By Suchita
		{
			mdlElement_display(&poly, NORMALDRAW);		
			mdlElement_add(&poly);	
		}
		//Added by suchita for GG
		else
		{
			mdlElmdscr_new (&polyElP, NULL, &poly);
			mdlElmdscr_appendDscr (chainDscrP, polyElP); 	
		}
	}

	return TRUE;
}

/**
	@brief	create donut
	@author	HumKyung.Baek
	@date	????.??.??
	@return	boolean
*/
Private boolean Draw_Donut(float fInsideDia, float fOutsideDia, Dpoint3d *ptCenter, int nColor, int nLayer)
{
	MSElementUnion newElement, newInElement, newOutElement;
	int i, stat; 
	int nFillmode = 1;
	int nWeight, nStyle;
	MSElementDescr *OutpatternEdPP = NULL, *InpatternEdPP = NULL, *patternEdPP = NULL;

	double unitVal = tcb->uorpersub;

	ptCenter->x = ptCenter->x * unitVal;
	ptCenter->y = ptCenter->y * unitVal;
	ptCenter->z = ptCenter->z * unitVal;

	stat = mdlEllipse_create(&newOutElement, NULL, ptCenter, fOutsideDia*unitVal, fOutsideDia*unitVal, NULL, nFillmode);
	if(stat != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}		

	mdlElement_setSymbology(&newOutElement, &nColor, NULL, NULL);
	mdlElement_setProperties(&newOutElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	
	if(fInsideDia > 0.0)
	{
		stat = mdlEllipse_create(&newInElement, NULL, ptCenter, fInsideDia*unitVal, fInsideDia*unitVal, NULL, nFillmode);
		if(stat != SUCCESS) 
		{
			PrintError("Error creating graphic element!");
			return FALSE;
		}		

		mdlElement_setSymbology(&newInElement, &nColor, NULL, NULL);
		mdlElement_setProperties(&newInElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);

		mdlElement_display(&newInElement, NORMALDRAW);	
		
		mdlElmdscr_new (&OutpatternEdPP, NULL, &newOutElement);
		mdlElmdscr_new (&InpatternEdPP, NULL, &newInElement);

		/// get difference pattern between two patterns
		mdlElmdscr_differenceShapes(&patternEdPP, NULL, OutpatternEdPP, InpatternEdPP , 0);
		mdlElmdscr_display(patternEdPP, 0, NORMALDRAW);
		mdlElmdscr_add (patternEdPP);
		
		/// free mstn element descriptions
		mdlElmdscr_freeAll(&patternEdPP);
		mdlElmdscr_freeAll(&InpatternEdPP);
		mdlElmdscr_freeAll(&OutpatternEdPP);
	}
	else
	{
		mdlElement_display(&newOutElement, NORMALDRAW);
		stat = mdlElement_add(&newOutElement);
		if(stat == 0) 
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}
	}
	
	return TRUE;
}

/**
	@brief

	@author	HumKyung.Baek,Suchita

	@date	????.??.??

	@param

	@return
*/
Private boolean Draw_Text(Dpoint3d *ptStart, float fHeight, float fWidth, float fAngle, char* sText, int nJustify, int Obliquing, int nColor, int nLayer, int nStyle, int nWeight)
{
	MSElementUnion newElement;
	int i, stat; 
	int nFillmode;
	TextSizeParam txtSize;
	RotMatrix rMatrix; 
	TextParamWide txtParam;
	MSWideChar wString[MAX_LENGTH];
	char txtString[128];
	MSElementDescr* groupElP = NULL;		
	double radian;
	ULong filePos = 0;
	double unitVal = tcb->uorpersub;

	ptStart->x = ptStart->x * unitVal;
	ptStart->y = ptStart->y * unitVal;

	sprintf(txtString,"%s", sText);  
	mdlRMatrix_getIdentity(&rMatrix);

	txtSize.mode       = TXT_BY_TILE_SIZE | TXT_NO_TRANSFORM;
	txtSize.size.width  = fWidth * unitVal;
	txtSize.size.height = fHeight * unitVal;
	txtSize.aspectRatio = 1.0;

	txtParam.font      = nStyle;                  // Font 번호.
	txtParam.just      = nJustify;
	txtParam.style     = -1;
	txtParam.flags.vertical = FALSE;

	txtParam.flags.underline = FALSE;
	txtParam.flags.slant = TRUE;
	txtParam.flags.vertical = FALSE;
	txtParam.slant	= GetRadian(Obliquing);

	mbstowcs(wString, txtString, MAX_LENGTH);
	mdlRMatrix_fromAngle(&rMatrix, GetRadian(fAngle));

	stat = mdlText_createWide(&newElement, NULL, wString, ptStart, &rMatrix , &txtSize, &txtParam , NULL);

	if(stat != SUCCESS) 
	{
		PrintError ("Error creating text element!");
		return FALSE;
	}		
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	if(chainDscrP == NULL)	//! added By Suchita
	{
		mdlElement_display(&newElement, NORMALDRAW);	
		filePos = mdlElement_add(&newElement);						 
		if(0 == filePos) 
		{
			PrintError ("\nError adding text element!");
			return FALSE;
		}
	}
	else
	{
		mdlElmdscr_new (&groupElP, NULL, &newElement);
		mdlElmdscr_appendDscr (chainDscrP, groupElP); 	
	}

	return TRUE;
}

/**
	@brief	Leader를 그린다.
	@author	HoSang.Jeon,HumKyung.Baek
	@date	????.??.??
	@return	boolean
*/
Private boolean Draw_Leader(Dpoint3d *ptPoint, int nColor, int nLayer, char* sArrowType, double nArrowWidth , double nArrowHeight , int nLineCount , int iLineWeight)
{
	Dpoint3d pointS, pointE,ptsLine[2]; 
	Dpoint3d lineVect; 
	Dpoint3d ptArrowHead[4];
	Dpoint3d LeftVec;
	Dpoint3d ptBase;
	float normVect;
	float lineLength;
	float fWidth = 1;		//! Length of an arow
	float fTtheta = 1.0;	//! angle (in radians) at the arrow tip between the two sides of the arrowhead 
	MSElementUnion newElement;
	double unitVal = tcb->uorpersub;
	int i = 0;
	///MSElementDescr *saveChainDscrP=NULL;

	Dpoint3d ptFirst,ptSecond;

	// set first node to terminal point   
	ptArrowHead[0].x = ptArrowHead[3].x = ptPoint[0].x; 
	ptArrowHead[0].y = ptArrowHead[3].y = ptPoint[0].y;
	ptArrowHead[0].z = ptArrowHead[3].z = fc_zero;

	pointS.x = ptPoint[1].x;
	pointS.y = ptPoint[1].y;

	pointE.x = ptPoint[0].x; 
	pointE.y = ptPoint[0].y; 

	// build the line vector
	lineVect.x = pointE.x - pointS.x; 
	lineVect.y = pointE.y - pointS.y; 

	normVect = sqrt(lineVect.x*lineVect.x + lineVect.y*lineVect.y);
	if(0.0 == normVect) return TRUE;
	lineVect.x /= normVect;
	lineVect.y /= normVect;

	// build the arrow base vector - normal to the line
	LeftVec.x = -lineVect.y;
	LeftVec.y = lineVect.x;

	// setup remaining arrow head points
	lineLength = normVect;

	// find the base of the arrow
	ptBase.x = (ptArrowHead[0].x + -nArrowWidth * lineVect.x);
	ptBase.y = (ptArrowHead[0].y + -nArrowWidth * lineVect.y);

	// build the points on the sides of the arrow
	ptArrowHead[1].x = (ptBase.x + nArrowHeight * LeftVec.x * 0.5);
	ptArrowHead[1].y = (ptBase.y + nArrowHeight * LeftVec.y * 0.5);
	ptArrowHead[1].z = fc_zero;
	ptArrowHead[2].x = (ptBase.x + -nArrowHeight * LeftVec.x * 0.5);
	ptArrowHead[2].y = (ptBase.y + -nArrowHeight * LeftVec.y * 0.5);
	ptArrowHead[2].z = fc_zero;

	//Suhas : As changes suggested by beak (16 id on sharepoint)Draw slash as always 45 degree angle.
	ptsLine[0].x = ptPoint[0].x - (nArrowWidth/2); 
	ptsLine[0].y = ptPoint[0].y - (nArrowHeight/2); 
	ptsLine[0].z = 0.0;

	ptsLine[1].x = ptPoint[0].x + (nArrowWidth/2); 
	ptsLine[1].y = ptPoint[0].y + (nArrowHeight/2); 
	ptsLine[1].z = 0.0;
	//up to here.

	///saveChainDscrP = chainDscrP;
	///mdlCell_create(&groupCell, "LEADER" , NULL, FALSE);
	///if(0 == mdlElmdscr_new(&chainDscrP,NULL,&groupCell))
	{
		//! create master line with lines
		for(i = 0;i < nLineCount - 1;i++)
		{
			Draw_Line(&(ptPoint[i]) , &(ptPoint[i + 1]) , nColor, nLayer, GetLineType("ByLayer"), 1.0, iLineWeight);
		}

		if( strncmp( sArrowType, "CLOSE", strlen("CLOSE")) == 0)
		{	
			Sub_Draw_Polygon(ptArrowHead, nColor, nLayer, GetLineType("ByLayer"), 1.0, GetLineWeight("Default"), 1, 4, 1);
		}
		else if( strncmp( sArrowType, "NONE", strlen("NONE")) == 0)
			;
		else if( strncmp( sArrowType, "OPEN30", strlen("OPEN30")) == 0)
		{
			pointS = ptArrowHead[1];
			ptArrowHead[1] = ptArrowHead[0];
			ptArrowHead[0] = pointS;
			Draw_PLine(ptArrowHead , nColor , nLayer , GetLineType("ByLayer") , 1.0 , GetLineWeight("Default") , 1 , 3 , 0);
		}
		else if( strncmp( sArrowType, "DOTSMALL", strlen("DOTSMALL")) == 0)
		{
			Draw_Circle(&pointE, (nArrowWidth/2), nColor, nLayer, GetLineType("ByLayer"), 1.0, GetLineWeight("Default"), 1);
		}
		else if( strncmp( sArrowType, "HOLLOWSMALL", strlen("HOLLOWSMALL")) == 0)
		{
			//Suhas : Circle center point is not leader line start point(id no 14 on sharepoint).
			pointE.x = (pointE.x + nArrowWidth * lineVect.x * 0.5);
			pointE.y = (pointE.y + nArrowWidth * lineVect.y * 0.5);
			//up to here
			Draw_Circle(&pointE, (nArrowWidth/2), nColor, nLayer, GetLineType("ByLayer"), 1.0, GetLineWeight("Default"), 0);
		}
		else if( strncmp( sArrowType, "SLASH", strlen("SLASH")) == 0)
		{
			Draw_PLine(ptsLine , nColor , nLayer , GetLineType("ByLayer") , 1.0 , GetLineWeight("Default") , 1 , 2 , 0);
		}
		else
		{
			///chainDscrP = saveChainDscrP;

			PrintError("Not support Arrow Type!");
			return FALSE;
		}

		/*
		if(NULL == saveChainDscrP)
		{
			mdlElmdscr_add(chainDscrP);
			mdlElmdscr_display(chainDscrP,MASTERFILE,NORMALDRAW);	
		}
		else
		{
			mdlElmdscr_appendDscr (saveChainDscrP, chainDscrP); 	
		}
		mdlElmdscr_freeAll(chainDscrP);
		chainDscrP = NULL;

		chainDscrP = saveChainDscrP;
		*/
	}
	/*else
	{
		PrintError("Group mdlElmdscr_new Failed");
	}
	*/

	return TRUE;
}

/**
	@brief	Leader를 그린다.
	@author	HumKyung.Baek
	@date	????.??.??
	@return	boolean
*/
boolean Draw_HatchPoint(Dpoint3d *ptStart, Dpoint3d *ptEnd, Dpoint3d *ptInst,int nColor, int nLayer, int nStyle,  float fScale, int nWeight)
{
	Dpoint3d  createLinePnt[5];
	MSElementDescr *patternEdPP,*edP, *holeEdp, *chainHole; //*chainEdp;
	MSElementUnion newElement;
	ClipDescr	clipPP;
	int stat;
	char msg[252];

	holeEdp = NULL;
	chainHole= NULL;

	mdlInput_sendKeyin("HATCH ICON ", 1, 0, NULL);
	//mdlSystem_journalAccessStrByTaskId(mdlSystem_getCurrTaskID(),accessStrP,dataType, dataMask, dataP);
	mdlSystem_journalDataPoint(ptInst, NULL, 0, 0, 0);
	mdlSystem_journalDataPoint(ptInst, NULL, 0, 0, 0);
	//createLinePnt[0].x = createLinePnt[4].x = -10.0 ;//* unitVal;
	//createLinePnt[0].y = createLinePnt[4].y = -10.0 ;//* unitVal;
	//createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	//createLinePnt[1].x = 10.0 ;//* unitVal;
	//createLinePnt[1].y = -10.0 ;//* unitVal;
	//createLinePnt[1].z = fc_zero;

	//	createLinePnt[2].x = 10.0 ;//* unitVal;
	//createLinePnt[2].y = 10.0 ;//* unitVal;
	//createLinePnt[2].z = fc_zero;

	//createLinePnt[3].x = -10.0 ;//* unitVal;
	//createLinePnt[3].y = 10.0 ;//* unitVal;
	//createLinePnt[3].z = fc_zero;

	//	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);

	//	if(stat != SUCCESS) 
	//{
	//	PrintError("Error creating graphic element!");
	//	return FALSE;
	//}		
	//
	//stat = mdlElement_add(&newElement);
	////create fence using given points
	////mdlFence_fromShape(&newElement);
	//mdlElmdscr_new(&edP, NULL, &newElement);

	//createLinePnt[0].x = createLinePnt[4].x = -8.0 ;//* unitVal;
	//createLinePnt[0].y = createLinePnt[4].y = -8.0 ;//* unitVal;
	//createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	//createLinePnt[1].x = 0.0 ;//* unitVal;
	//createLinePnt[1].y = -8.0 ;//* unitVal;
	//createLinePnt[1].z = fc_zero;

	//	createLinePnt[2].x = 0.0 ;//* unitVal;
	//createLinePnt[2].y = 0.0 ;//* unitVal;
	//createLinePnt[2].z = fc_zero;

	//createLinePnt[3].x = -8.0 ;//* unitVal;
	//createLinePnt[3].y = 0.0 ;//* unitVal;
	//createLinePnt[3].z = fc_zero;

	//	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);
	//if(stat != SUCCESS) 
	//{
	//	PrintError("Error creating graphic element!");
	//	return FALSE;
	//}		
	//else
	//{
	//	stat = mdlElement_add(&newElement);
	//	mdlElmdscr_new(&holeEdp, NULL, &newElement);
	//	if (chainHole == NULL)
	//		mdlElmdscr_new(&chainHole,NULL,&newElement);
	//	//else
	//	{
	//		createLinePnt[0].x = createLinePnt[4].x = 8.0 ;//* unitVal;
	//		createLinePnt[0].y = createLinePnt[4].y = 8.0 ;//* unitVal;
	//		createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	//		createLinePnt[1].x = 0.5 ;//* unitVal;
	//		createLinePnt[1].y = 8.0 ;//* unitVal;
	//		createLinePnt[1].z = fc_zero;

	//			createLinePnt[2].x = 0.5 ;//* unitVal;
	//		createLinePnt[2].y = 0.5 ;//* unitVal;
	//		createLinePnt[2].z = fc_zero;

	//		createLinePnt[3].x = 8.0 ;//* unitVal;
	//		createLinePnt[3].y = 0.5 ;//* unitVal;
	//		createLinePnt[3].z = fc_zero;

	//	 	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);
	//		stat = mdlElement_add(&newElement);		
	//		mdlElmdscr_new(&holeEdp,NULL,&newElement);
	//		mdlElmdscr_appendDscr (&chainHole, &holeEdp);
	//		//mdlElmdscr_addToChain (&chainHole, &holeEdp);
	//	}
	//}
	//	//stat = mdlElmdscr_add(&holeEdp);
	////scane Fence elements
	////mdlClip_getFence(&clipPP);


	//createLinePnt[0].x = createLinePnt[4].x = -10.0 ;//* unitVal;
	//createLinePnt[0].y = createLinePnt[4].y = -10.0 ;//* unitVal;
	//createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	//createLinePnt[1].x = 10.0 ;//* unitVal;
	//createLinePnt[1].y = -10.0 ;//* unitVal;
	//createLinePnt[1].z = fc_zero;

	//	createLinePnt[2].x = 10.0 ;//* unitVal;
	//createLinePnt[2].y = 10.0 ;//* unitVal;
	//createLinePnt[2].z = fc_zero;

	//createLinePnt[3].x = -10.0 ;//* unitVal;
	//createLinePnt[3].y = 10.0 ;//* unitVal;
	//createLinePnt[3].z = fc_zero;

	//	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);

	//	if(stat != SUCCESS) 
	//{
	//	PrintError("Error creating graphic element!");
	//	return FALSE;
	//}		
	//
	//stat = mdlElement_add(&newElement);
	////create fence using given points
	//mdlFence_fromShape(&newElement);
	//mdlElmdscr_new(&edP, NULL, &newElement);
	//fenceFunction();
	//stat = mdlPattern_hatch(&patternEdPP, edP, chainHole, NULL, 0, 1, 0, TRUE, &createLinePnt[0]);
	////stat = mdlPattern_hatch(&patternEdPP, edP, &holeEdp, NULL, 0, 1, 0, TRUE, &ptInst);
	//
	//if(stat != SUCCESS) 
	//{
	//	sprintf(msg, "status = %d", stat); 
	//	PrintError(msg);		
	//	return FALSE;
	//}	
	//else
	//stat = mdlElmdscr_add(patternEdPP);
	////mdlClip_isElemInside(TRUE, );
	return TRUE;
}

/**
	@brief	Shape을 생성한 후 패턴을 생성한다.
	@author	HumKyung Baek,Suchita
	@date	????.??.??
	@return	boolean
*/
Private boolean Draw_HatchPLine(Dpoint3d *ptPLine, int nLayer, int nColor, int nStyle,  float fAngle, float fScale , int nWeight , int nLineCount, char* sYesNo, char* cCellLib, char* cCellName)
{  
	MSElementDescr *patternEdPP,*edP , *ptr;
	Dpoint3d  *createLinePnt;  
	MSElementUnion newElement; 
	ULong filePos;
	int i, stat,iSuccess = -1 , fillMode = 0; 

	double unitVal = tcb->uorpersub;

	createLinePnt = (DPoint3d*)malloc(nLineCount * sizeof(DPoint3d));
	for(i=0; i<nLineCount; i++)
	{					
		createLinePnt[i].x = (ptPLine[i]).x * unitVal;
		createLinePnt[i].y = (ptPLine[i]).y * unitVal;
		createLinePnt[i].z = fc_zero;
	}

	/// check cell library and cell name
	if((0 == strcmpi(cCellLib , "NULL")) && (0 == strcmpi(cCellName , "SOLID")))
	{
		fillMode = 1;
	}

	stat = mdlShape_create(&newElement, NULL, createLinePnt, nLineCount, fillMode);
	if(stat != SUCCESS) 
	{
		PrintError ("Error creating graphic element!");
		return FALSE;
	}
	mdlElement_setSymbology(&newElement  , &nColor, &nWeight, &nStyle);
	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	if((1 == fillMode) || (0 == strcmpi(sYesNo, "No")))
	{
		if(chainDscrP == NULL)
		{
			mdlElement_display(&newElement, NORMALDRAW);
			/// display outline when sYesNo is "No"
			stat = mdlElement_add(&newElement);
			if(stat == 0) 
			{
				PrintError ("\nError adding graphic element!");
				return FALSE;
			}
		}
		else
		{
			MSElementDescr* outlineElP = NULL;
			stat = mdlElmdscr_new(&outlineElP, NULL, &newElement);
			mdlElmdscr_appendDscr (chainDscrP, outlineElP);
			////mdlElmdscr_freeAll (&outlineElP);
		}
	}

	if((0 == fillMode) && (SUCCESS == mdlElmdscr_new(&edP, NULL, &newElement)))
	{
		stat = SUCCESS;
		if ((0 != strcmpi(cCellLib, "NULL")) && (SUCCESS == loadCellLibraryIfNeeded(cCellLib)))
		{
			stat = mdlPattern_area(&patternEdPP,edP,NULL,NULL,cCellName,fScale ,GetRadian(fAngle),fScale * unitVal,fScale * unitVal,0,FALSE,NULL);   
		}	///up to here
		else
		{
			stat = mdlPattern_hatch(&patternEdPP, edP, NULL, NULL, GetRadian(fAngle), fScale * unitVal, 0, FALSE, FALSE);
		}

		if(SUCCESS != stat)
		{
			sprintf(SZ_BUF , "Error creating mdlPattern_hatch! - %d" , stat);
			PrintError (SZ_BUF);
			return FALSE;
		}
		else
		{
			mdlElmdscr_setSymbology(patternEdPP,&nColor,&nStyle,&nWeight,NULL);
			mdlElmdscr_setProperties(patternEdPP , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
			//if(chainDscrP == NULL)
			{	
				mdlElmdscr_add(patternEdPP);
			}
			/*else
			{
				mdlElmdscr_appendDscr (chainDscrP, patternEdPP);
			}*/
		}

		/// delete outline when flag is "Yes" - 2014.04.15 added by humkyung
		if((fillMode == 0) && (strcmpi(sYesNo, "Yes") == 0))
		{
			mdlElement_undoableDelete(&edP->el,stat,TRUE);
		}
		/// up to here

		/// free used element description
		mdlElmdscr_freeAll (&patternEdPP);
		mdlElmdscr_freeAll (&edP);
	}
	
	return TRUE;
}

/**
	@brief	create hatch circle
	@author	HumKyung.BAEK
*/
Private boolean Draw_HatchCircle(Dpoint3d *ptCenter, float fRadius, int nColor, int nLayer, int nStyle, float fScale, int nWeight, float fAngle, char* sYesNo, char *cCellLib, char* cCellName)
{
	MSElementUnion newElement;
	MSElementDescr* patternEdPP = NULL;
	MSElementDescr* edP = NULL;
	MSElementDescr* circleDscrPP = NULL;
	int i, stat;
	int fillMode = 0,iSuccess = -1;
	MSElementDescr*  elDescrP=NULL;
	double unitVal = tcb->uorpersub;

	ptCenter->x = ptCenter->x * unitVal;
	ptCenter->y = ptCenter->y * unitVal;
	ptCenter->z = ptCenter->z * unitVal;

	if((0 == strcmpi(cCellLib , "NULL")) && (0 == strcmpi(cCellName , "SOLID")))
	{
		fillMode = 1;
	}

	stat = mdlEllipse_create(&newElement, NULL, ptCenter, fRadius * unitVal, fRadius * unitVal, NULL, fillMode);
	if(stat != SUCCESS) 
	{
		PrintError ("Error creating graphic element!");
		return FALSE;
	}		

	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, &nStyle);
	if((fillMode == 1) || (strcmpi(sYesNo, "No") == 0))
	{
		if(chainDscrP == NULL)
		{
			mdlElement_display(&newElement, NORMALDRAW);	
			stat = mdlElement_add(&newElement);
			if(stat == 0)
			{
				PrintError ("\nError adding graphic element!");
				return FALSE;
			}
		}
		else
		{
			stat = mdlElmdscr_new (&circleDscrPP, NULL, &newElement);
			mdlElmdscr_appendDscr (chainDscrP, circleDscrPP);
			//mdlElmdscr_freeAll (&edP);
		}
	}
	
	if(mdlElmdscr_new(&edP, NULL, &newElement) == SUCCESS)
	{
		///Suhas: Add hatching to element.(modified by HumKyung.Baek)
		if (((0 != strcmpi(cCellLib , "NULL")) && loadCellLibraryIfNeeded(cCellLib)) && (strcmpi(cCellName, "NULL") != 0))
		{
			stat= mdlPattern_area(&patternEdPP,edP,NULL,NULL,cCellName,fScale,GetRadian(fAngle),fScale * unitVal,fScale * unitVal,0,FALSE,NULL);   
		}	
		else
		{
			stat= mdlPattern_hatch(&patternEdPP, edP, NULL, NULL, GetRadian(fAngle), fScale*unitVal, 0, FALSE, FALSE);
		}

		if(SUCCESS != stat) 
		{
			PrintError ("Error creating mdlPattern_hatch!");
			return FALSE;
		}
		else
		{
			mdlElmdscr_setSymbology(patternEdPP,&nColor,&nStyle,&nWeight,NULL);
			mdlElmdscr_setProperties(patternEdPP , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
			//if(chainDscrP == NULL)
			{	
				mdlElmdscr_add(patternEdPP);
			}
			//else
			//{
			//	/*MSElementDescr* outDscrPP = NULL;
			//	mdlPattern_getElementDescr(&outDscrPP , patternEdPP,0,0,0);
			//	mdlElmdscr_appendDscr (chainDscrP, outDscrPP);*/
			//}
		}

		///suhas: Delete element if delete source Yes.
		if((fillMode == 0) && (strcmp(sYesNo, "Yes") == 0))
		{
			mdlElement_undoableDelete(&edP->el,stat,TRUE);
		}
		

		mdlElmdscr_freeAll (&patternEdPP);
		mdlElmdscr_freeAll (&edP);
	}
	else
		PrintError ("Error creating graphic element!");

	return TRUE;
}

/**
	@brief	HATCHPLINE을 읽어 ITEM을 생성한다.

	@author	HumKyung.Baek
*/
Private int ProcessHatchPLine( Tokens* pToken )
{
	int nLineNum = 0 , i;
	int nColor, nLayer, nPattern , nWeight = 0;
	float fAngle, fScale;
	Tokens *pToken1 , *pToken2;
	Dpoint3d *ptPLine, *ptCenter;
	char* sYesNo, *cCellLib, *cCellName;
	nLineNum = 0;

	pToken1 = SplitCSVString( pToken->buf[1] );
	if(pToken1 && (pToken1->nCount > 0))
	{
		nLineNum = pToken1->nCount * 0.5;

		ptPLine = (DPoint3d*)malloc(nLineNum * sizeof(DPoint3d));
		if(ptPLine == NULL) 
			mdlDialog_openAlert("Memory allocate error!");

		for(i=0; i < nLineNum; i++)
		{
			ptPLine[i].x = atof( pToken1->buf[i * 2 ] );
			ptPLine[i].y = atof( pToken1->buf[i * 2 + 1] );
			ptPLine[i].z = fc_zero;
		}
	}

	pToken2 = SplitCSVString( pToken->buf[2] );
	if(pToken2 && (7 == pToken2->nCount))
	{
		nLayer = GetLevel( pToken2->buf[0] );
		nColor = GetColorNo(pToken2->buf[1]);
		cCellLib = pToken2->buf[2];

		/// pattern is a cell name
		cCellName = pToken2->buf[3];
		/// nPattern = GetLineType(pToken2->buf[3]);
		nPattern = 0;

		fAngle   = atof(pToken2->buf[4]);

		if(strcmp(pToken2->buf[5], "Default") == 0)
			fScale = 0;
		else	fScale = atof(pToken2->buf[5]);
		sYesNo = pToken2->buf[6];
	}

	if(Draw_HatchPLine(ptPLine, nLayer, nColor , nPattern , fAngle , fScale , nWeight , nLineNum, sYesNo, cCellLib, cCellName) == FALSE)
		PrintError("<HATCHPLINE> drawing fail");

	return 0;
}

Private void ProcessCmd(char *filename)
{
	FILE *hFile=NULL;
	char buffer[LINE_BUF_SIZ + 1];
	ULong   fileMask[8];
	int open_status = -1;

	hFile = mdlTextFile_open( filename, TEXTFILE_READ );
	if( hFile != NULL )
	{
		while( !feof( hFile ) )
		{
			memset(buffer, 0, LINE_BUF_SIZ);
			mdlTextFile_getString( buffer, sizeof( buffer ), hFile,	TEXTFILE_DEFAULT ); /// NEW LINE을 제거한다.
			if('#' == buffer[0]) continue;

			if(0 == strncmp(buffer , "NEWFILE" , 7))
			{
				/// close a file which is opended
				if(SUCCESS == open_status)
				{
					fileMask[0] = fileMask[1] = fileMask[2] = fileMask[3] = fileMask[4] = fileMask[5] = fileMask[6] = fileMask[7] = 0xffffffff;
					mdlView_fit(0,fileMask);
					mdlInput_sendKeyin("FILEDESIGN",TRUE,0,NULL);
					mdlSystem_saveDesignFile();
					open_status = -1;
				}

				if ((open_status = mdlSystem_newDesignFile ( buffer + 8 )) == SUCCESS)
				{
				}
			}
			else if(0 == strncmp(buffer , "SCALE" , 5))
			{
				ULong elemAddr[50], eofPos, filePos;
				int scanWords, numChanged=0, status, i, numAddr;
				Scanlist scanList;
				MSElementUnion el;
				double dScale = atof(buffer + 6);
				
				mdlScan_initScanlist (&scanList);
				mdlScan_noRangeCheck (&scanList);
				scanList.scantype	    = ELEMTYPE;
				scanList.typmask[0]	    = TMSK0_LINE|TMSK0_SHAPE|TMSK0_LINE_STRING|TMSK0_TEXT_NODE|TMSK0_CELL_HEADER|TMSK0_ARC|TMSK0_ELLIPSE;
				scanList.typmask[1]	    = TMSK1_TEXT;
				scanList.typmask[2]		= TMSK2_DIMENSION|TMSK2_ATTRIBUTE;
				scanList.extendedType = FILEPOS;

				mdlScan_initialize (0, &scanList);
				do
				{
					scanWords = sizeof(elemAddr)/sizeof(short);
					status	  = mdlScan_file (elemAddr, &scanWords, sizeof(elemAddr), &filePos);
					numAddr   = scanWords / sizeof(short);

					for (i=0; i<numAddr; i++)
					{
						if(SUCCESS == mdlElement_read( &el, 0, elemAddr[i] ))
						{
							/// scale up/down with given value - 2013.10.18 added by humkyung
							Transform tMatrix;
							mdlTMatrix_getIdentity(&tMatrix);
							mdlTMatrix_scale(&tMatrix,&tMatrix,dScale,dScale,0);
							mdlElement_transform(&el,&el,&tMatrix);
							/// up to here
							mdlElement_rewrite(&el , NULL , elemAddr[i]);
						}
					}
				} while (status == BUFF_FULL);
			}
			else if(0 == strncmp(buffer , "MACRO" , 5))
			{
				mdlInput_sendKeyin(buffer,TRUE,0,NULL);
			}
			else
			{
				if(SUCCESS == open_status) ProcessTag(buffer);
			}
		}
		if(SUCCESS == open_status)
		{
			fileMask[0] = fileMask[1] = fileMask[2] = fileMask[3] = fileMask[4] = fileMask[5] = fileMask[6] = fileMask[7] = 0xffffffff;
			mdlView_fit(0,fileMask);
			mdlInput_sendKeyin("FILEDESIGN",TRUE,0,NULL);
			mdlSystem_saveDesignFile();
			open_status = -1;
		}

		mdlTextFile_close( hFile );
	}
}

/**
	@brief	tag를 적는 main routine
	@author	HoSang.Jeon,HumKyung.Baek
	@date	????.??.??
	@return	
*/
Private void ProcessTag(char *filename)
{
	FILE *hFile=NULL;
	char buffer[LINE_BUF_SIZ + 1], Subbuffer1[LINE_BUF_SIZ + 1], Subbuffer2[LINE_BUF_SIZ + 1], Tmpbuffer[LINE_BUF_SIZ + 1];
	char *ptr, *subptr;
	char *sYesNo, *sTagDesc, *sGroupNo, *cCellLib, *cCellName;
	char *sText, *sArrowType, *sArrowSize , *sDirection, *sTextDirection, *sCellLibPath, *pDimStyleName;

	Dpoint3d ptStart, ptEnd, ptLoc, ptCenter, ptStart1, ptEnd1, ptInst;
	Insert insert;
	Dpoint3d *ptPLine,*ptCloud;
	Tokens *pToken = NULL , *pToken1 = NULL , *pToken2 = NULL;

	int nLineNum=0 , iLineWeight = 0 , iSolidHole = 0;
	int nColor, nLayer, nStyle, nWeight, nJustify, nNumSide, Obliquing, i;
	float fHeight, fAngle, fScale, fWidth, fRadius;
	float fFirst, fNext, fInsideDia, fOutsideDia,fInRadius, fOutRadius;
	float fStartAngle, fEndAngle;
	float fPrimaryAxis,fScndAxis;
	float fArcMin,fArcMax;
	ULong mask[8], filePos;
	double nArrowWidth , nArrowHeight;
	char msg[256]; 
	MSElementUnion       el;

	hFile = mdlTextFile_open( filename, TEXTFILE_READ );
	if( hFile != NULL )
	{
		while( !feof( hFile ) )
		{
			memset(buffer, 0, LINE_BUF_SIZ);
			mdlTextFile_getString( buffer, sizeof( buffer ), hFile,	TEXTFILE_DEFAULT ); /// NEW LINE을 제거한다.
			if('#' == buffer[0]) continue;

			ResetTokens();
			pToken = Split( buffer , '|');

			if(NULL == pToken) 
			{
				PrintError("pToken is NULL");
				continue;
			}
			if(0 == pToken->nCount)
			{
				if(strcmp(buffer,"</GROUP>") == 0)
				{
					strcpy(pToken->buf[0],buffer);
				}
				else
				{
					continue;
				}
			}

			if(strcmp(pToken->buf[0], "<TEXT>") == 0)
			{	
				pToken1 = SplitCSVString( pToken->buf[1]);
				if(pToken1 && (9 == pToken1->nCount))
				{
					ptStart.x = atof(pToken1->buf[0]);
					ptStart.y = atof(pToken1->buf[1]);
					ptStart.z = atof(pToken1->buf[2]);

					fHeight = atof(pToken1->buf[3]);
					fWidth = atof(pToken1->buf[4]);
					fAngle = atof(pToken1->buf[5]);

					sText = pToken1->buf[6];

					nJustify = GetTextJustify(pToken1->buf[7]);

					Obliquing = atof(pToken1->buf[8]);
				}
				else
				{
					sprintf(SZ_BUF , "<TEXT> : Parsing Error(%s)" , pToken->buf[1]);
					PrintError(SZ_BUF);
					continue;
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (4 == pToken2->nCount))
				{
					nLayer = GetLevel(pToken2->buf[0]);
					nColor = GetColorNo(pToken2->buf[1]);
					nStyle = GetFontNo(pToken2->buf[2]);
					nWeight = atof(pToken2->buf[3]);
				}

				if(0 != sText[0])
				{
					if(Draw_Text(&ptStart, fHeight, fWidth, fAngle, sText, nJustify, Obliquing, nColor, nLayer, nStyle, nWeight) == FALSE)
					{
						PrintError(buffer);
					}
				}
			}
			else if(strcmp(pToken->buf[0], "<LINE>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (6 == pToken1->nCount))
				{
					ptStart.x = atof(pToken1->buf[0]);
					ptStart.y = atof(pToken1->buf[1]);
					ptStart.z = atof(pToken1->buf[2]);

					ptEnd.x = atof(pToken1->buf[3]);
					ptEnd.y = atof(pToken1->buf[4]);
					ptEnd.z = atof(pToken1->buf[5]);
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel(pToken2->buf[0]);
					nColor = GetColorNo(pToken2->buf[1]);
					nStyle = GetLineType(pToken2->buf[2]);
					fScale = atof(pToken2->buf[3]);
					nWeight= GetLineWeight( pToken2->buf[4] );
				}	

				if(Draw_Line(&ptStart, &ptEnd, nColor, nLayer, nStyle, fScale, nWeight) == FALSE)
					PrintError("<LINE> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<CIRCLE>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (4 == pToken1->nCount))
				{
					ptCenter.x = atof( pToken1->buf[0] );
					ptCenter.y = atof( pToken1->buf[1] );
					ptCenter.z = atof( pToken1->buf[2] );

					fRadius = atof( pToken1->buf[3] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{				
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmpi( pToken2->buf[4] , "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi( pToken2->buf[4] );
				}

				if(Draw_Circle(&ptCenter, fRadius, nColor, nLayer, nStyle, fScale, nWeight, 0) == FALSE)
					PrintError("<CIRCLE> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<ARC>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (6 == pToken1->nCount))
				{
					ptCenter.x = atof( pToken1->buf[0] );
					ptCenter.y = atof( pToken1->buf[1] );
					ptCenter.z = atof( pToken1->buf[2] );

					fRadius = atof( pToken1->buf[3] );
					fStartAngle = atof( pToken1->buf[4] );
					fEndAngle = atof(pToken1->buf[5]);
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp( pToken2->buf[4] , "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi( pToken2->buf[4] );
				}

				if(Draw_Arc(&ptCenter, fRadius, fStartAngle, fEndAngle, nColor, nLayer, nStyle, fScale, nWeight) == FALSE)
					PrintError("<ARC> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<PLINE>") == 0)
			{
				nLineNum = 0;
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (pToken1->nCount > 1))
				{
					nLineNum = pToken1->nCount * 0.5;
					ptPLine = (DPoint3d*)malloc(nLineNum * sizeof(DPoint3d));
					if(ptPLine == NULL) 
						PrintError("Memory allocate error!");

					for(i = 0;i < nLineNum;i++)
					{
						ptPLine[i].x = atof(pToken1->buf[i* 2]);
						ptPLine[i].y = atof(pToken1->buf[i* 2 + 1]);
						ptPLine[i].z = 0;
					}
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel(pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp(pToken2->buf[4], "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi(pToken2->buf[4]);
				}

				if(Draw_PLine(ptPLine, nColor, nLayer, nStyle, fScale, nWeight, fWidth, nLineNum , 0) == FALSE)
					PrintError("<PLINE> drawing fail");

				if(ptPLine) free(ptPLine);
			}
			else if(strcmp(pToken->buf[0], "<CLOUD>") == 0)
			{
				ptCloud = NULL;
				nLineNum = 0;
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (pToken1->nCount > 1))
				{
					nLineNum = pToken1->nCount * 0.5;
					ptCloud = (DPoint3d*)malloc(nLineNum * sizeof(DPoint3d));
					if(ptCloud == NULL) 
						PrintError("Memory allocate error!");

					for(i = 0;i < nLineNum;i++)
					{
						ptCloud[i].x = atof(pToken1->buf[i*2]);
						ptCloud[i].y = atof(pToken1->buf[i*2 + 1]);
						ptCloud[i].z = 0;
					}
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (7 == pToken2->nCount))
				{
					nLayer = GetLevel(pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp(pToken2->buf[4], "Default") == 0)
						nWeight = 0;
					else	
						nWeight = atoi(pToken2->buf[4]);

					fArcMin = atof( pToken2->buf[5] );
					fArcMax	= atof( pToken2->buf[6] );
				}

				if(Draw_Cloud(ptCloud, nColor, nLayer, nStyle, fScale, nWeight, fWidth, nLineNum, fArcMin, fArcMax) == FALSE)
					PrintError("<Cloud> drawing fail");

				if(ptCloud) free(ptCloud);
			}
			else if(strcmp(pToken->buf[0], "<RECTANGLE>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (4 == pToken1->nCount))
				{
					ptStart.x = atof( pToken1->buf[0] );
					ptStart.y = atof( pToken1->buf[1] );

					ptEnd.x = atof( pToken1->buf[2] );
					ptEnd.y = atof( pToken1->buf[3] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp(pToken2->buf[4], "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi(pToken2->buf[4]);

					fWidth = atof(pToken2->buf[5]);
				}

				if(Draw_Rectangle(&ptStart, &ptEnd, nColor, nLayer, nStyle, fScale, nWeight, fWidth) == FALSE)
					PrintError("<RECTANGLE> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<ELLIPSE>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (5 == pToken1->nCount))
				{
					ptStart.x = atof( pToken1->buf[0] );
					ptStart.y = atof( pToken1->buf[1] );

					fPrimaryAxis = atof( pToken1->buf[2] );
					fScndAxis = atof( pToken1->buf[3] );
					fAngle = atof( pToken1->buf[4] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp(pToken2->buf[4], "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi(pToken2->buf[4]);
				}

				if(Draw_Ellipse(&ptStart, fPrimaryAxis, fScndAxis, fAngle, nColor, nLayer, nStyle, fScale, nWeight) == FALSE)
					PrintError("<RECTANGLE> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<POLYGON>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (5 == pToken1->nCount))
				{
					nNumSide = atoi( pToken1->buf[0] );
					ptCenter.x = atof( pToken1->buf[1] );
					ptCenter.y = atof( pToken1->buf[2] );
					fRadius = atof( pToken1->buf[3] );
					fAngle = atof( pToken1->buf[4] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					nStyle = GetLineType( pToken2->buf[2] );
					fScale = atof( pToken2->buf[3] );
					if(strcmp( pToken2->buf[4] , "Default") == 0)
						nWeight = 0;
					else	nWeight = atoi( pToken2->buf[4] );
				}

				if(Draw_Polygon(&ptCenter, nNumSide, fRadius, fAngle, nColor, nLayer, nStyle, fScale, nWeight, fWidth) == FALSE)
					PrintError("<POLYGON> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<DONUT>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (4 == pToken1->nCount))
				{
					fInsideDia = atof( pToken1->buf[0] );
					fOutsideDia = atof( pToken1->buf[1]);

					ptCenter.x = atof( pToken1->buf[2] );
					ptCenter.y = atof( pToken1->buf[3] );
					ptCenter.z = fc_zero;
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (2 == pToken2->nCount))
				{	
					nLayer = GetLevel( pToken2->buf[0] ) ;
					nColor = GetColorNo( pToken2->buf[1] );
				}

				if(Draw_Donut(fInsideDia, fOutsideDia, &ptCenter, nColor, nLayer) == FALSE)
					PrintError(buffer);
			}
			else if(strcmp(pToken->buf[0], "<LEADER>") == 0)
			{
				nLineNum = 0;
				nArrowWidth  = 0.5;
				nArrowHeight = 1.5;

				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (pToken1->nCount > 0))
				{
					nLineNum = pToken1->nCount * 0.5;

					ptPLine = (DPoint3d*)malloc(nLineNum * sizeof(DPoint3d));
					if(ptPLine==NULL) 
						mdlDialog_openAlert("Memory allocate error!");

					for(i = 0;i < nLineNum;i++)
					{
						ptPLine[i].x = atof( pToken1->buf[i * 2] );
						ptPLine[i].y = atof( pToken1->buf[i * 2 + 1] );
						ptPLine[i].z = 0.;
					}
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && ((5 == pToken2->nCount) || (6 == pToken2->nCount)))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					sArrowType = pToken2->buf[2];
					nArrowWidth = atof(pToken2->buf[3]);
					nArrowHeight= atof(pToken2->buf[4]);
					if(6 == pToken2->nCount)
					{
						iLineWeight = atoi(pToken2->buf[5]);
					}
					else
					{
						iLineWeight = 0;
					}
				}

				if(Draw_Leader(ptPLine, nColor, nLayer, sArrowType , nArrowWidth , nArrowHeight , nLineNum , iLineWeight) == FALSE)
				{
					///PrintError("<LEADER> drawing fail"); 
					PrintError(buffer);
				}

				if(ptPLine) free((void*)ptPLine);
			}
			else if(strcmp(pToken->buf[0], "<INSERT>") == 0)
			{
				memset(&insert , 0 , sizeof(Insert));
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (8 == pToken1->nCount))
				{
					insert.psCellLibPath = pToken1->buf[0];
					insert.psCellName    = pToken1->buf[1];

					insert.ptInsert.x = atof(pToken1->buf[2]);
					insert.ptInsert.y = atof(pToken1->buf[3]);

					strncpy(insert.sWhere , pToken1->buf[4] , 7);

					insert.ptScale.x = atof(pToken1->buf[5]);
					insert.ptScale.y = atof(pToken1->buf[6]);
					insert.ptScale.z = fc_zero;

					insert.fAngle = atof(pToken1->buf[7]);
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (2 == pToken2->nCount))
				{
					nLayer = GetLevel(pToken2->buf[0]);
					subptr = pToken2->buf[1];
				}

				if(FALSE == Draw_Insert(&insert , nLayer, subptr))
					PrintError("<INSERT> drawing fail");
			}
			else if(0 == strcmp(pToken->buf[0], "<DIMSTYLE>"))
			{
				Parse_DimStyle( pToken );
			}
			else if(strcmp(pToken->buf[0], "<DIMENSION>") == 0)
			{
				Parse_Dimension( pToken );
			}
			else if(strcmp(pToken->buf[0], "<DIMANGULAR>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (6 == pToken1->nCount))
				{
					ptCenter.x = atof( pToken1->buf[0] );
					ptCenter.y = atof( pToken1->buf[1] );

					ptStart.x = atof( pToken1->buf[2] );
					ptStart.y = atof( pToken1->buf[3] );

					ptEnd.x = atof( pToken1->buf[4] );
					ptEnd.y = atof( pToken1->buf[5] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (2 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					pDimStyleName =  pToken2->buf[1];
				}
				if(Draw_DimAngular(&ptCenter,&ptStart, &ptEnd, nLayer, pDimStyleName) == FALSE)
					PrintError("<DIMANGULAR> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<HATCHPOINT>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (6 == pToken1->nCount))
				{
					ptInst.x = atof( pToken1->buf[0] );
					ptInst.y = atof( pToken1->buf[1] );

					ptStart.x = atof( pToken1->buf[2] );
					ptStart.y = atof( pToken1->buf[3] );

					ptEnd.x = atof( pToken1->buf[4] );
					ptEnd.y = atof( pToken1->buf[5] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (5 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					
					cCellLib =  pToken2->buf[2];

					//pattern is a cell name
					cCellName = pToken2->buf[3];
					//nStyle = GetLineType( pToken2->buf[3] );
					nStyle = 0;

					fAngle = atof( pToken2->buf[4]);
					fScale = atof( pToken2->buf[5]);
				}

				if(Draw_HatchPoint(&ptStart, &ptEnd, &ptInst, nColor, nLayer, nStyle,  fScale, nWeight) == FALSE) 
					PrintError("<HATCHPOINT> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<HATCHRECT>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (8 == pToken1->nCount))
				{
					ptStart.x = atof( pToken1->buf[0] );
					ptStart.y = atof( pToken1->buf[1] );

					ptEnd.x = atof( pToken1->buf[2] );
					ptEnd.y = atof( pToken1->buf[3] );

					ptStart1.x = atof( pToken1->buf[4] );
					ptStart1.y = atof( pToken1->buf[5] );

					ptEnd1.x = atof( pToken1->buf[6] );
					ptEnd1.y = atof( pToken1->buf[7] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (7 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					
					cCellLib =  pToken2->buf[2];

					//pattern is a cell name
					cCellName = pToken2->buf[3];
					//nStyle = GetLineType( pToken2->buf[3] );
					nStyle = 0;

					fAngle = atof( pToken2->buf[4]);

					fScale = atof( pToken2->buf[5]);
					sYesNo = pToken2->buf[6];
					nWeight = 0;
				}

				if(Draw_HatchRect(&ptStart, &ptEnd, &ptStart1, &ptEnd1, nColor, nLayer, nStyle, fAngle, fScale,nWeight,sYesNo,cCellLib,cCellName) == FALSE)
					PrintError("<HATCHRECT> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<HATCHDONUT>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (4 == pToken1->nCount))
				{
					ptCenter.x = atof( pToken1->buf[0] );
					ptCenter.y = atof( pToken1->buf[1] );
					ptCenter.z = 0; 
					fInRadius = atof( pToken1->buf[2] );

					fOutRadius = atof( pToken1->buf[3] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (7 == pToken2->nCount))
				{	
					nLayer = GetLevel( pToken2->buf[0] ) ;
					nColor = GetColorNo( pToken2->buf[1] );
					
					cCellLib =  pToken2->buf[2];

					///pattern is a cell name
					cCellName = pToken2->buf[3];
					///nStyle = GetLineType( pToken2->buf[3] );
					nStyle = 0;

					fAngle = atof( pToken2->buf[4]);
					fScale = atof( pToken2->buf[5]);
					sYesNo = pToken2->buf[6];
					nWeight = 0;
				}

				if(Draw_HatchDonut(fInRadius, fOutRadius, &ptCenter, nColor, nLayer,nStyle,fAngle,fScale,sYesNo,cCellLib,cCellName) == FALSE)
					PrintError("<HATCHDONUT> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<HATCHPLINE>") == 0)
			{
				ProcessHatchPLine( pToken );
			}
			else if(strcmp(pToken->buf[0], "<HATCHCIRCLE>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				if(pToken1 && (3 == pToken1->nCount))
				{
					ptCenter.x = atof( pToken1->buf[0] );
					ptCenter.y = atof( pToken1->buf[1] );

					fRadius = atof( pToken1->buf[2] );
				}

				pToken2 = SplitCSVString( pToken->buf[2] );
				if(pToken2 && (7 == pToken2->nCount))
				{
					nLayer = GetLevel( pToken2->buf[0] );
					nColor = GetColorNo( pToken2->buf[1] );
					
					cCellLib =  pToken2->buf[2];

					//pattern is a cell name
					cCellName = pToken2->buf[3];
					//nStyle = GetLineType( pToken2->buf[3] );
					nStyle = 0;

					fAngle = atof( pToken2->buf[4]);

					fScale = atof( pToken2->buf[5]);
					sYesNo = pToken2->buf[6];
					nWeight = 0;
				}

				if(Draw_HatchCircle(&ptCenter, fRadius, nColor, nLayer, nStyle, fScale, nWeight,fAngle,sYesNo,cCellLib,cCellName) == FALSE)
					PrintError("<HATCHCIRCLE> drawing fail");
			}
			else if(strcmp(pToken->buf[0], "<GROUP>") == 0)
			{
				pToken1 = SplitCSVString( pToken->buf[1] );
				
				mdlCell_create(&groupCell, pToken1->buf[2], NULL, FALSE);
				mdlElement_setProperties(&groupCell , NULL , NULL , NULL , NULL , NULL , NULL , NULL , &iSolidHole);
				if (mdlElmdscr_new(&chainDscrP,NULL,&groupCell) != 0)
					PrintError("Group mdlElmdscr_new Failed");
			}
			else if(strcmp(pToken->buf[0], "</GROUP>") == 0)
			{
				if (chainDscrP != NULL)
				{
					//! add user data to element
					///filePos = mdlElmdscr_add(chainDscrP);
					///addUserData(&chainDscrP , filePos);

					filePos = mdlElmdscr_add(chainDscrP);

					mdlElmdscr_display(chainDscrP,MASTERFILE,NORMALDRAW);
					mdlElmdscr_freeAll(chainDscrP);
					chainDscrP = NULL;
				}
				else
				{
					PrintError("chainDscrP is NULL");
				}
			}
			else if(strcmp(pToken->buf[0], "<Rev>") == 0 || strcmp(pToken->buf[0], "<UGMODEL>") == 0 || strcmp(pToken->buf[0], "<CABLE>") == 0 || 
				strcmp(pToken->buf[0], "<FIXED>") == 0 || strcmp(pToken->buf[0], "<Road>") == 0)
			{
				ptr = strtok( NULL, "|");
				strcpy(Subbuffer1, ptr);

				sTagDesc = strtok( NULL, "|");

				sYesNo = strtok( Subbuffer1, "^");
				if(strcmp(sYesNo, "YES") == 0)
				{
					sGroupNo = strtok( NULL, "^");

					mdlLevel_addGroup(sTagDesc, sGroupNo, 0);
				}
			}			
		}
		mdlTextFile_close( hFile );
	}
	/*mdlCurrTrans_end();
	mdlCurrTrans_clear();*/

	mask[0] = 3;
	mdlView_fit(0,mask);
	mdlView_updateSingle(0);
}

Private boolean Draw_HatchRect(Dpoint3d *ptStart, Dpoint3d *ptOthe, Dpoint3d *ptStart1, Dpoint3d *ptOthe1, int nColor, int nLayer, int nStyle, float fAngle, float fScale, int nWeight, char* sYesNo, char* cCellLib, char* cCellName)
{
	Dpoint3d  createLinePnt[5],createLinePnt1[5];
	MSElementDescr *patternEdPP,*edP=NULL,*edP1=NULL;
	MSElementDescr     *cellDscrPP;
	MSElementUnion newElement,newElement1;  
	int stat,stat1,iSuccess = -1;
	ULong fPos = 0;
	char msg[256];

	double unitVal = tcb->uorpersub;

	createLinePnt[0].x = createLinePnt[4].x = ptStart->x * unitVal;
	createLinePnt[0].y = createLinePnt[4].y = ptStart->y * unitVal;
	createLinePnt[0].z = createLinePnt[4].z = fc_zero;

	createLinePnt[1].x = ptStart->x * unitVal;
	createLinePnt[1].y = ptOthe->y * unitVal;
	createLinePnt[1].z = fc_zero;

	createLinePnt[2].x = ptOthe->x * unitVal;
	createLinePnt[2].y = ptOthe->y * unitVal;
	createLinePnt[2].z = fc_zero;

	createLinePnt[3].x = ptOthe->x * unitVal;
	createLinePnt[3].y = ptStart->y * unitVal;
	createLinePnt[3].z = fc_zero;

	//For another rectangle
	createLinePnt1[0].x = createLinePnt1[4].x = ptStart1->x * unitVal;
	createLinePnt1[0].y = createLinePnt1[4].y = ptStart1->y * unitVal;
	createLinePnt1[0].z = createLinePnt1[4].z = fc_zero;

	createLinePnt1[1].x = ptStart1->x * unitVal;
	createLinePnt1[1].y = ptOthe1->y * unitVal;
	createLinePnt1[1].z = fc_zero;

	createLinePnt1[2].x = ptOthe1->x * unitVal;
	createLinePnt1[2].y = ptOthe1->y * unitVal;
	createLinePnt1[2].z = fc_zero;

	createLinePnt1[3].x = ptOthe1->x * unitVal;
	createLinePnt1[3].y = ptStart1->y * unitVal;
	createLinePnt1[3].z = fc_zero;

	stat = mdlShape_create(&newElement, NULL, createLinePnt, 5, 0);
	if(stat != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}	

	stat1 = mdlShape_create(&newElement1, NULL, createLinePnt1, 5, 0);
	if(stat1 != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}

	mdlElement_setProperties(&newElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement, &nColor, &nWeight, NULL);

	mdlElement_setProperties(&newElement1 , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_setSymbology(&newElement1, &nColor, &nWeight, NULL);

	mdlElement_display(&newElement, NORMALDRAW);		
	stat = mdlElement_add(&newElement);	

	mdlElement_display(&newElement1, NORMALDRAW);		
	stat1 = mdlElement_add(&newElement1);	

	if(stat == 0 || stat1 == 0) 
	{
		PrintError("Error adding graphic element!");
		return FALSE;
	}	
	stat1 = 0;
	if(mdlElmdscr_new(&edP, NULL, &newElement) == SUCCESS && mdlElmdscr_new(&edP1, NULL, &newElement1) == SUCCESS)
	{
		//Suhas: Add hatching to element.(modified by HumKyung.Baek)
		if(((0 != strcmpi(cCellLib , "NULL")) && loadCellLibraryIfNeeded(cCellLib)) && (strcmpi(cCellName, "NULL") != 0 ))
		{
			stat = mdlPattern_area(&patternEdPP,edP,edP1,NULL,cCellName,fScale,GetRadian(fAngle),fScale* unitVal,fScale* unitVal,0,FALSE,NULL);   
		}	//up to here
		else
		{
			stat = mdlPattern_hatch(&patternEdPP, edP, edP1, NULL, GetRadian(fAngle), fScale* unitVal, 0, FALSE, FALSE); 
		}

		if(SUCCESS != stat) 
		{
			PrintError ("Error creating mdlPattern_hatch!");
			return FALSE;
		}
		else
		{
			mdlElmdscr_setSymbology(patternEdPP, &nColor, NULL, NULL, NULL);
			mdlElmdscr_setProperties(patternEdPP , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
			mdlElmdscr_add(patternEdPP);
		}
	}
	else
		PrintError ("Error creating graphic element!");

	if(strcmp(sYesNo, "Yes") == 0)
	{
		mdlElement_undoableDelete(&edP->el,stat,TRUE);
		mdlElement_undoableDelete(&edP1->el,stat1,TRUE);
	}

	mdlElmdscr_freeAll (&patternEdPP);
	mdlElmdscr_freeAll (&edP);
	mdlElmdscr_freeAll (&edP1);

	return TRUE;
}

/**
	@brief	draw a donut with hatch
	@author	humkyung
	@date
	@return
*/
Private boolean Draw_HatchDonut(float fInsideDia, float fOutsideDia, Dpoint3d *ptCenter, int nColor, int nLayer,int nStyle,float fAngle, float fScale, char* sYesNo, char* cCellLib, char* cCellName)
{
	MSElementDescr *InpatternEdPP, *OutpatternEdPP, *edP, *edP1, *patternEdPP,*elemDscrP;
	MSElementUnion newElement, newInElement, newOutElement;
	int i, stat, stat1; 
	int nFillmode;
	int nWeight = 0, iSuccess = -1;
	MSElementDescr  *pNextDscrFirst = NULL;
	MSElementDescr* elmDesc = NULL; ///Added by suchita for GG

	double unitVal = tcb->uorpersub;

	ptCenter->x = ptCenter->x * unitVal;
	ptCenter->y = ptCenter->y * unitVal;
	ptCenter->z = ptCenter->z * unitVal;

	stat = mdlEllipse_create(&newOutElement, NULL, ptCenter, fOutsideDia*unitVal, fOutsideDia*unitVal, NULL, 0);
	if(stat != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}		

	mdlElement_setSymbology(&newOutElement, &nColor, &nWeight, &nStyle);
	mdlElement_setProperties(&newOutElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	if(0 == strcmp(sYesNo, "No"))
	{
		if(chainDscrP == NULL)	/// added By Suchita
		{
			mdlElement_display(&newOutElement, NORMALDRAW);	
			stat = mdlElement_add(&newOutElement);
			if(stat == 0) 
			{
				PrintError("Error adding graphic element!");
				return FALSE;
			}
		}
		else
		{
			stat = mdlElmdscr_new (&elmDesc, NULL, &newOutElement);
			//mdlElmdscr_add(elmDesc);
			mdlElmdscr_appendDscr (chainDscrP, elmDesc); 	
		}
	}

	stat1 = mdlEllipse_create(&newInElement, NULL, ptCenter, fInsideDia*unitVal, fInsideDia*unitVal, NULL, 0);
	if(stat1 != SUCCESS) 
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}		

	mdlElement_setSymbology(&newInElement, &nColor, &nWeight, &nStyle);
	mdlElement_setProperties(&newInElement , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	if(0 == strcmp(sYesNo, "No"))
	{
		if(chainDscrP == NULL)	//added By Suchita
		{	
			mdlElement_display(&newInElement, NORMALDRAW);	
			stat1 = mdlElement_add(&newInElement);
			if(stat1 == 0) 
			{
				PrintError("Error adding graphic element!");
				return FALSE;
			}
		}
		else
		{
			stat = mdlElmdscr_new (&elmDesc, NULL, &newInElement);
			//mdlElmdscr_add(elmDesc);
			mdlElmdscr_appendDscr (chainDscrP, elmDesc); 	
			//mdlCell_setRange(chainDscrP);
		}
	}

	if(mdlElmdscr_new (&edP, NULL, &newOutElement) == SUCCESS && mdlElmdscr_new (&edP1, NULL, &newInElement) == SUCCESS)
	{
		//Suhas: Add hatching to element.(modified by HumKyung.Baek)
		if (((0 != strcmpi(cCellLib , "NULL")) && (SUCCESS == loadCellLibraryIfNeeded(cCellLib))) && (strcmpi(cCellName, "NULL") != 0 ))
		{
			stat = mdlPattern_area(&patternEdPP,edP1,edP,NULL,cCellName,fScale,GetRadian(fAngle),fScale* unitVal,fScale* unitVal,0,FALSE,NULL);
		}	
		else
		{
			stat = mdlPattern_hatch(&patternEdPP, edP1, edP, NULL, GetRadian(fAngle), fScale* unitVal, 0, FALSE, FALSE); 
		}

		if(SUCCESS != stat) 
		{
			PrintError ("Error creating mdlPattern_hatch!");
			return FALSE;
		}
		else
		{
			mdlElmdscr_setSymbology(patternEdPP,&nColor,&nStyle,&nWeight,NULL);
			mdlElmdscr_setProperties(patternEdPP , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
			if(chainDscrP == NULL)	///added By Suchita
			{
				mdlElmdscr_add(patternEdPP);
			}
			else
			{
				//mdlElmdscr_appendDscr (chainDscrP, patternEdPP); 	
				mdlElmdscr_add(patternEdPP);
			}
		}
	}else	PrintError ("Error creating graphic element!");	

	/*
	if(strcmpi(sYesNo, "No") == 0)
	{
		stat = mdlElement_add(&edP->el);
		stat1 = mdlElement_add(&edP1->el);
		if((0 == stat) || (0 == stat1))
		{
			PrintError("Error adding graphic element!");
			return FALSE;
		}
		
		///mdlElement_undoableDelete(&edP->el,stat,TRUE);
		///mdlElement_undoableDelete(&edP1->el,stat1,TRUE);
	}
	*/

	mdlElmdscr_freeAll (&patternEdPP);
	mdlElmdscr_freeAll (&edP);
	mdlElmdscr_freeAll (&edP1);

	return TRUE;
}

Private boolean Draw_Ellipse(Dpoint3d *ptStart, float fPrimaryAxis, float fScndAxis, float fAngle, int nColor, int nLayer, int nStyle, float fScale, int nWeight)
{
	MSElement out,*elm;
	double dAngle=0;
	RotMatrix rotMatrix; 
	double unitVal = tcb->uorpersub;
	MSElementDescr* lineElP = NULL;

	fAngle = GetRadian(fAngle);
	mdlRMatrix_fromAngle(&rotMatrix,fAngle);

	ptStart->x = ptStart->x * unitVal; 
	ptStart->y = ptStart->y * unitVal;
	ptStart->z = 0;

	if ( SUCCESS != mdlEllipse_create(&out,NULL,ptStart,fPrimaryAxis * unitVal,fScndAxis * unitVal,&rotMatrix,0) )
		//if ( SUCCESS != mdlEllipse_create(&out, NULL, ptStart, fPrimaryAxis, fScndAxis, &rotMatrix, 0) )
	{
		PrintError("Error creating graphic element!");
		return FALSE;
	}
	else
	{
		mdlElement_setProperties(&out , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
		mdlElement_setSymbology(&out, &nColor, &nWeight, &nStyle);

		if(chainDscrP == NULL)	//added By Suchita
		{
			mdlElement_display(&out, NORMALDRAW);		
			mdlElement_add(&out);
		}
		//Added by suchita for GG
		else
		{
			mdlElmdscr_new (&lineElP, NULL, &out);
			mdlElmdscr_appendDscr (chainDscrP, lineElP); 	
		}
	}

	return TRUE;	
}
