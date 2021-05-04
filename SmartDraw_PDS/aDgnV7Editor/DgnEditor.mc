/*----------------------------------------------------------------------+
|                                                                       |
| Copyright (1993) Bentley Systems, Inc., All rights reserved.          |
|                                                                       |
| "MicroStation", "MDL", and "MicroCSL" are trademarks of Bentley       |
|  Systems, Inc. and/or Intergraph Corporation.                         |
|                                                                       |
| Limited permission is hereby granted to reproduce and modify this     |
| copyrighted material provided that the resulting code is used only in |
| conjunction with Bentley Systems products under the terms of the      |
| license agreement provided therein, and that this notice is retained  |
| in its entirety in any such reproduction or modification.             |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   myapp.mc - MyApp source code.                                       |
|              Illustrates basic MDL concepts and functionality for     |
|              use with the introductory MDL class.                     |
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
#include <accudraw.h>
#include    <cexpr.h>

#include "DgnEditor.h"                      
#include "DgnEditorcmd.h"                      
#include "cmdids.h"	/* resource ids */                     
#include <msinput.fdf>
#include <msstate.fdf>         
#include <msdim.fdf>
#include <msmisc.fdf>
#include <msrmatrx.fdf>
#include <msoutput.fdf>
#include <msselect.fdf>
#include <mselemen.fdf>
#include <msvec.fdf>
#include <msassoc.fdf>
#include <mselems.h>
#include <mdlerrs.h>
#include <mselems.h>
#include <global.h>
#include <mdl.h>
#include <tcb.h>
#include <userfnc.h>
#include <mselems.h>

// Global variable declaration

DimEditInfo	dimeditInfo;  /* Tool Settings (slam-down) variables */

typedef struct patterninfo
{
    DPoint3d    _pt;
    char	cPatternName[BUFF_LEN];
    DPoint3d    _distance;
    double      _angle;
    int		_elementcount;
    boolean     _bdrawmode;
    MSElementDescr* pElmDescr;
}PatternInfo;

// ------------------------------------ PATTERN -2 ---------------------------------------------------
DPoint3d	g_startpointofline;		    
DPoint3d	g_pivotpt;
DPoint3d	g_oldrefpt;
DPoint3d	org_points[3];
DPoint3d	new_points[3];
DPoint3d	g_div_line[2];
DPoint3d	g_new_div_line[2];                 
DPoint3d	g_leader_line[2];
DPoint3d	g_nearestpoint;       //                      
int		g_LineCnt = 0;
boolean		g_bOnce = FALSE;
MSElementDescr* g_pElmDescr;
Dpoint3d	g_pt;

// ----------------------------------------------------------------------------------------------------

// ------------------------------------ PATTERN -3 ----------------------------------------------------
int		g_elementpos;
Dpoint3d	g_textshape[5];        
Dpoint3d	g_anchor;
boolean		g_firsttimeonly = FALSE;
//MSElementDescr* g_ptempElmDescr;
ULong g_filePos;
//ULong g_fileNum;
DPoint3d	g_elementpickpt;
double g_oldangle = 0.0;
// ----------------------------------------------------------------------------------------------------

// ------------------------------------ PATTERN -4 ----------------------------------------------------
Dpoint3d	g_centerPt;
Dpoint3d	g_intersectPt;
// ----------------------------------------------------------------------------------------------------

// ------------------------------------ PATTERN -7 ----------------------------------------------------
Dpoint3d	g_flathexagon[7];
Dpoint3d	g_rectangle[5];
boolean		g_bPATTERN_7_LINESTRING;
        
// ----------------------------------------------------------------------------------------------------

// ------------------------------------ PATTERN -8 ----------------------------------------------------

Dpoint3d	g_roundrect[4];       
Dpoint3d	g_linevertices[4];
Dpoint3d	g_lowerline[2];
DPoint3d        g_rotateat; 
DPoint3d	g_temppt1;
DPoint3d	g_temppt2;       
// ----------------------------------------------------------------------------------------------------



Dpoint3d cellOrigin;                /* origin of cell to be placed, 
					set by first data point. */
int cellView	;		    /* view of cell to be placed*/


void moveAnElement();


/*****************************************************************************************
//Function : getPatten(MSElementDescr* pElmDescr)
//Desc     : Get name of the pattern.
//Return   : char*
//****************************************************************************************/
Private  int getPatternName(MSElementDescr* pElmDescr,char * cPattern)
{
    char cPatternName[BUFF_LEN];
    MSElementDescr* pDescr = NULL;
    
    if( pElmDescr != NULL && mdlElement_getType(&pElmDescr->el) == CELL_HEADER_ELM)
    {
	pDescr = pElmDescr->h.firstElem;
	// Get cell header
	if( mdlCell_extract(NULL,NULL,NULL,NULL,cPatternName,&pElmDescr->el) == SUCCESS)
	{
	    strcpy(cPattern,cPatternName);
	    return SUCCESS;
	}
    }
    else
    {
	return !SUCCESS;
    }
    
}
/*****************************************************************************************
//Function : getPatten(MSElementDescr* pElmDescr)
//Desc     : Get total elements present in the cell/pattern
//Return   : char*
//****************************************************************************************/
Private  int getElementCount(MSElementDescr* pElmDescr)
{
    
    MSElementDescr* pDescr;
    int cnt = 1;
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	cnt++;
	pDescr = pDescr->h.next;
    }
    
    return cnt;
    
}
/*****************************************************************************************
//Function : IsValidPattern
//Desc     : Check whether selected pattern is valid for selected operation.
//Return   : Boolean
//****************************************************************************************/
Private boolean IsValidPattern(char* cPattern,int iOptType)
{
    if( iOptType == OPT_TAGROTATE && ( strcmp(cPattern,PATTERN_1) == 0
				       || strcmp(cPattern,PATTERN_2) == 0
                                       || strcmp(cPattern,PATTERN_7) == 0
                                       || strcmp(cPattern,PATTERN_8) == 0 ))
    {                                   
         return TRUE;
    }
    else if ( iOptType == OPT_TAGMOVE && ( strcmp(cPattern,PATTERN_1) == 0
				       || strcmp(cPattern,PATTERN_2) == 0
                                       || strcmp(cPattern,PATTERN_3) == 0
                                       || strcmp(cPattern,PATTERN_4) == 0 
                                       || strcmp(cPattern,PATTERN_5) == 0
                                       || strcmp(cPattern,PATTERN_6) == 0
                                       || strcmp(cPattern,PATTERN_7) == 0
                                       || strcmp(cPattern,PATTERN_8) == 0 ))
    {                                   
         return TRUE;
    }
    else
	return FALSE;                             	    
}

/*****************************************************************************************
//Function : findPivotPoint
//Desc     : Decide pivot and reference point for calculationg angle
//Return   : -
//****************************************************************************************/
void findPivotPoint(DPoint3d* points,char* cPatternName,int elmCnt)
{
  
    DPoint3d temppt1,temppt2,temppoint;
    int nVertexPos,nCnt;
    double dpt1,dpt2;
    
    if(strcmp(cPatternName,PATTERN_2) == 0 )
    {
       
       if(elmCnt == 6)
       {
	   if( mdlVec_pointEqual(&g_leader_line[1],&points[0]) )
	   {
		g_pivotpt = points[0];
		g_oldrefpt = points[1];
	   }
	   else if( mdlVec_pointEqual(&g_leader_line[1],&points[1]))
	   {
		g_pivotpt = points[1];
		g_oldrefpt = points[0];
	   }
       }
       else
       {
	    dpt1 = mdlVec_distance(&g_leader_line[1],&points[0]);
            dpt2 = mdlVec_distance(&g_leader_line[1],&points[1]);
	    
            if(dpt1 < dpt2)
            {
		g_pivotpt = points[0];
		g_oldrefpt = points[1];
            } 
            else 
            {
		g_pivotpt = points[1];
		g_oldrefpt = points[0];
            }
	   
       }
    }
    else if(strcmp(cPatternName,PATTERN_8) == 0)
    {            
        nVertexPos = 0; 
        for(nCnt = 0;nCnt < 4;nCnt++)
        {
            if(nCnt != 0)
            {
	      dpt2 = mdlVec_distance(&g_leader_line[1],&g_linevertices[nCnt]);		
              if(dpt1 > dpt2)
              {
		dpt1 = dpt2;
                temppoint = g_linevertices[nCnt];
                nVertexPos = nCnt;
              }
            }   
            else
            {
		dpt1 = mdlVec_distance(&g_leader_line[1],&g_linevertices[nCnt]);
                temppoint = g_linevertices[nCnt];
                nVertexPos = nCnt;
            }
        }
             		
       if( nVertexPos == 0 )
       {              
	    g_pivotpt = g_linevertices[0];
	    g_oldrefpt = g_linevertices[1];
       }
       else if( nVertexPos == 1 )
       {
	    g_pivotpt = g_linevertices[1];
	    g_oldrefpt = g_linevertices[0];           
       }
       else if( nVertexPos == 2 )
       {
	    g_pivotpt = g_linevertices[2];
	    g_oldrefpt = g_linevertices[3];            
       }
       else if( nVertexPos == 3 )
       {
            g_pivotpt = g_linevertices[3];
	    g_oldrefpt =g_linevertices[2];
       }  
    } 
    else if(strcmp(cPatternName,PATTERN_7) == 0)
    {
       
       if(g_bPATTERN_7_LINESTRING)		            
       {
	   for (nVertexPos = 1; nVertexPos <= 7;nVertexPos++)
	   {
		if(mdlVec_pointEqual(&g_leader_line[1],&g_flathexagon[nVertexPos]))
		{
		    if(nVertexPos == 1)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[6];
			//mdlDialog_openAlert("1");
		    }
		    else if(nVertexPos == 2)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[5];
			//mdlDialog_openAlert("2");
		    }
		    else if(nVertexPos == 3)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[4];
			//mdlDialog_openAlert("3");
		    }
		    else if(nVertexPos == 4)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[3];
			//mdlDialog_openAlert("4");
		    }
		    else if(nVertexPos == 5)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[2];
			//mdlDialog_openAlert("5");
		    }
		    else if(nVertexPos == 6)
		    {
			g_pivotpt  = g_flathexagon[nVertexPos];
			g_oldrefpt = g_flathexagon[1];
			//mdlDialog_openAlert("6");
		    }
		}
	   }
       }
       else
       {
	   for (nVertexPos = 1; nVertexPos <= 5;nVertexPos++)
	   {
		if(mdlVec_pointEqual(&g_leader_line[1],&g_rectangle[nVertexPos]))
		{
		    if(nVertexPos == 1)
		    {
			g_pivotpt  = g_rectangle[nVertexPos];
			g_oldrefpt = g_rectangle[2];
			
		    }
                    else if(nVertexPos == 2)
		    {			    
			g_pivotpt  = g_rectangle[nVertexPos];
			g_oldrefpt = g_rectangle[1];
			
		    }
                    else if(nVertexPos == 3)
		    {
			g_pivotpt  = g_rectangle[nVertexPos];
			g_oldrefpt = g_rectangle[4];
		    }
                    else if(nVertexPos == 4)
		    {
			g_pivotpt  = g_rectangle[nVertexPos];
			g_oldrefpt = g_rectangle[3];
		    }
		}
           }
       }
    }
}

/*****************************************************************************************
//Function : getPatternInfo
//Desc     : It stores initial information of leader line when first time pattern selected
//Return   : -
//****************************************************************************************/
Private  void getPatternInfo(MSElementDescr* pElmDescr,int elementcount)
{
    DPoint3d	    points[2];
    MSElementDescr* pDescr = NULL;
    int 	    iElmType,iLineCnt,iCnt;
    int 	    nVertices;
    char	    cPatternName[BUFF_LEN];
    char	    msg[256];

   
    iLineCnt = 1;
    iCnt     = 1;
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    mdlCell_extract(NULL,NULL,NULL,NULL,cPatternName,&pElmDescr->el);
        
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
        iElmType = mdlElement_getType(&pDescr->el);
        if(strcmp(cPatternName,PATTERN_1) == 0 ) // Pattern -1
        {
	    
	    if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 1)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0]   = points[0];
		    g_leader_line[1]   = points[1];
		    g_pivotpt.x	       = (points[0].x+points[1].x) /2;
                    g_pivotpt.y	       = (points[0].y+points[1].y) /2;
                    g_pivotpt.z	       = (points[0].z+points[1].z) /2;
                    g_div_line[0] =  g_pivotpt;
		    g_div_line[1] = points[1];
                    g_oldrefpt =   points[1];
                }  
            }
            
            iCnt++;        
        }
        else if(strcmp(cPatternName,PATTERN_2) == 0) // Pattern -2
        {   
	    if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 1)
	    {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
		{
		    g_startpointofline = points[0];
		    g_leader_line[0]   = points[0];
		    g_leader_line[1]   = points[1];
		    g_pivotpt	       = points[1];
		    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
		}
	    }
	    else if( mdlElement_getType(&pDescr->el) == LINE_ELM  && ((elementcount == 6 && iCnt == 3) 
			|| (elementcount == 4 && iCnt == 2)))
	    {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
		{
		    findPivotPoint(points,PATTERN_2,elementcount);
		}
	    }
            
            iCnt++;        
        }
        else if(strcmp(cPatternName,PATTERN_3) == 0)   // Pattern -3
        {
	    
            if(mdlElement_getType(&pDescr->el) == LINE_ELM)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0] = points[0];
		    g_leader_line[1] = points[1];
		    g_pivotpt	  = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
                }
            }        
             iCnt++;
        }
        else if(strcmp(cPatternName,PATTERN_4) == 0)   // Pattern - 4
        {
	    
            if(mdlElement_getType(&pDescr->el) == LINE_ELM && iCnt == 2)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0] = points[0];
		    g_leader_line[1] = points[1];
		    g_pivotpt	  = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
                }
            }
             iCnt++;        
        }
         else if(strcmp(cPatternName,PATTERN_5) == 0)   // Pattern -5
        {
	    
            if(mdlElement_getType(&pDescr->el) == LINE_ELM && iCnt == 2)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0] = points[0];
		    g_leader_line[1] = points[1];
		    g_pivotpt	  = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
                }
            }
             iCnt++;        
        }

        else if(strcmp(cPatternName,PATTERN_6) == 0)   // Pattern -6
        {
	    
            if(mdlElement_getType(&pDescr->el) == LINE_ELM && iCnt == 2)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0] = points[0];
		    g_leader_line[1] = points[1];
		    g_pivotpt	  = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
                }
            }
             iCnt++;        
        }
        else if(strcmp(cPatternName,PATTERN_7) == 0) // Pattern -7
        {
	    
	    if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 2)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0] = points[0];
		    g_leader_line[1] = points[1];
		    g_pivotpt	     = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
                    findPivotPoint(points,PATTERN_7,elementcount);
                }
            }
            else if ( iCnt == 1)
            {	 
		if ( mdlElement_getType(&pDescr->el) == LINE_STRING_ELM )
                {
		    mdlLinear_extract(g_flathexagon,NULL,&pDescr->el,MASTERFILE);  	    	    
                    g_bPATTERN_7_LINESTRING = TRUE;
                }
                else
                {
		    mdlLinear_extract(g_rectangle,NULL,&pDescr->el,MASTERFILE);
                    g_bPATTERN_7_LINESTRING = FALSE;		    
                }
            }
            iCnt++;        
        }
        else if(strcmp(cPatternName,PATTERN_8) == 0 ) // Pattern -8
        {
	    
	    if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 5)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_startpointofline = points[0];
		    g_leader_line[0]   = points[0];
		    g_leader_line[1]   = points[1];
		    g_pivotpt	       = points[1];
                    g_div_line[0] = points[0];
		    g_div_line[1] = points[1];
		    
                    findPivotPoint(points,PATTERN_8,elementcount);	
                    
                }
            }
            else if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 1)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_linevertices[0] = points[0];
                    g_linevertices[1] = points[1];
                                    }
            }
            else if(mdlElement_getType(&pDescr->el) == LINE_ELM  && iCnt == 3)
            {
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
                {
		    g_linevertices[2] = points[0];
                    g_linevertices[3] = points[1];
                }
            }

            iCnt++;        
        }
        //if(strcmpi(cPatternName,PATTERN_2) == 0)
        else
        {   
	    if(iElmType == LINE_ELM )
	    {
		 
		if( SUCCESS == mdlLinear_extract(points,&nVertices,&pDescr->el,MASTERFILE))
		{
		   if(iLineCnt ==  1 )
                   {
			g_startpointofline = points[0];						
                        g_leader_line[0] = points[0];
			g_leader_line[1] = points[1];
                        g_pivotpt      = points[1];                         
		   }
                   else if(iLineCnt == 2)
                   {
			
			//g_pivotpt      = points[0];
			g_div_line[0] = points[0];
			g_div_line[1] = points[1];                        
                   }
		}
                
                iLineCnt ++;
	    }  
        }      
	pDescr = pDescr->h.next;
    }

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
//Function : 

//Desc     : Rotate elements by angle
//Return   : It returs modify status. 
//****************************************************************************************/
Private int rotateElementByAngle
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *infoP         /* => from params in mdlModify_element... */
)
{
    RotMatrix       outRMatrix;
    Transform	    outTMatrix;	 
    Dpoint3d        invOrigin;
   
    if(strcmp(infoP->cPatternName,PATTERN_1) == 0  
			||( strcmp(infoP->cPatternName,PATTERN_2) == 0 && 
  		                     ( (infoP->_elementcount==4 && g_elementpos == ELM_POS_3)
                                       || g_elementpos == ELM_POS_4 
                                       || g_elementpos == ELM_POS_5 
                                       || g_elementpos == ELM_POS_6)) 
			 ||( strcmp(infoP->cPatternName,PATTERN_7) == 0 && 
  		                     ( g_elementpos == ELM_POS_2  
                                       || g_elementpos == ELM_POS_7))               
                         ||( strcmp(infoP->cPatternName,PATTERN_8) == 0 && 
  		                     ( g_elementpos == ELM_POS_2 
                                       || g_elementpos == ELM_POS_3 
                                       || g_elementpos == ELM_POS_4
                                       || g_elementpos == ELM_POS_5
                                       || g_elementpos == ELM_POS_10)))                                       
                                       
    {
	invOrigin.x = -g_pivotpt.x;
	invOrigin.y = -g_pivotpt.y;
	invOrigin.z = -g_pivotpt.z;

	//Bring it to origin
	mdlTMatrix_getIdentity (&outTMatrix);
	mdlTMatrix_setTranslation(&outTMatrix,&invOrigin);
	mdlElement_transform(el,el,&outTMatrix);

	mdlRMatrix_fromAngle(&outRMatrix,infoP->_angle);
	mdlTMatrix_fromRMatrix(&outTMatrix,&outRMatrix);
	mdlTMatrix_setTranslation (&outTMatrix, &g_pivotpt);
	mdlElement_transform(el,el,&outTMatrix);
        if(infoP->_bdrawmode == DRAW_MODE_DYNAMIC)
        {
	    mdlElement_display(el,TEMPDRAW);
        }    
        g_elementpos++;      
    }
    else
	g_elementpos++;   
        
    /* indicate that we did something to the element */
    return  MODIFY_STATUS_REPLACE	 ;
    
}
/*****************************************************************************************
//Function : getCursorAngle(Dpoint3d *cursorpt,Dpoint3d* ptCellOrg )
//Desc     : Calculates angle beween two points
//Return   : double
//****************************************************************************************/
Private double getCursorAngle(Dpoint3d *cursorpt,Dpoint3d* ptCellOrg )
{
   double  zRotation;
   char    msg[256];
   
//   if (cursorpt->x - ptCellOrg->x == 0.0)
//   {
//      zRotation = fc_piover2;
//      //zRotation = fc_zero;
//   }
//   else
      zRotation=atan2((ptCellOrg->y-cursorpt->y), (ptCellOrg->x-cursorpt->x));

//   if (cursorpt->x - ptCellOrg->x < 0)
//   {
//      zRotation += fc_pi; 
//      //zRotation = fc_pi/3;
//   } 
   //printf("Cursor X = %f\n Y = %f\n Z = %f\n CellOrg X = %f\n Y = %f\n Z = %f\n ZRotation %f\n",cursorpt->x,cursorpt->y,cursorpt->z,ptCellOrg->x,ptCellOrg->y,ptCellOrg->z,zRotation);   
   return(zRotation);

}

/*****************************************************************************************
//Function : rotateArrow
//Desc     : -
//Return   : int
//****************************************************************************************/
Private int rotateArrow 
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *infoP      /* => from params in mdlModify_element... */
)
{
    double	    angle;
    int		    iVertices;
    RotMatrix       outRMatrix;
    Transform	    outTMatrix;	 
    Dpoint3d        invOrigin;
    DPoint3d	    points[4];        /* => Size is defined temporary :) */
    Dpoint3d	    tempPt;
    RotMatrix       rMatrix;
    
           
    if( mdlElement_getType(el) == SHAPE_ELM || mdlElement_getType(el) == LINE_STRING_ELM)
    {    
       
        // Extract Shape/ Line String element
        mdlLinear_extract(points,&iVertices,el,MASTERFILE);
        
        if(g_elementpos == ELM_POS_3 || g_elementpos == ELM_POS_4 || g_elementpos == ELM_POS_7 )
        {
	    if( mdlElement_getType(el) == SHAPE_ELM )
		tempPt = points[0];
	    else if( mdlElement_getType(el) == LINE_STRING_ELM)
	    {
		tempPt = points[1];
	    }
        }
        else
        { 
	    g_elementpos++;
	    return 0;
        }
        
	invOrigin.x = -tempPt.x;
	invOrigin.y = -tempPt.y;
	invOrigin.z = -tempPt.z;

	//Bring it to origin
	mdlTMatrix_getIdentity (&outTMatrix);
	mdlTMatrix_setTranslation(&outTMatrix,&invOrigin);
        mdlElement_transform(el, el,&outTMatrix);

	mdlRMatrix_fromAngle(&outRMatrix,infoP->_angle);        
        mdlTMatrix_fromRMatrix(&outTMatrix,&outRMatrix); 
        mdlTMatrix_setTranslation (&outTMatrix, &tempPt);
        mdlElement_transform(el,el,&outTMatrix);        
        if(infoP->_bdrawmode == DRAW_MODE_DYNAMIC)
	    mdlElement_display(el,TEMPDRAW);
        
    }
    g_elementpos++;    
    /* indicate that we did something to the element */
    return  MODIFY_STATUS_REPLACE;

}
/*****************************************************************************************
//Function : elementModify_withAngle
//Desc     : -
//Return   : int
//****************************************************************************************/
Private int elementModify_withAngle 
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo            *infoP      /* => from params in mdlModify_element... */
)
{
   
    return rotateElementByAngle(el,infoP);

}


Private void   resetAction(Dpoint3d *pPoint, int view )
{
    dgneditor_updateViews();
    mdlState_clear(); 
    mdlState_startDefaultCommand();	
}

/*****************************************************************************************
//Function : elementModify_withAngle
//Desc     : -
//Return   : int
//****************************************************************************************/
Private void  dynamicRotateWithAngle 
(
    MSElementDescr*	pElmDescr,           /* <> element to be modified */
    PatternInfo         *infoP      /* => from params in mdlModify_element... */
)
{    
 
    MSElementDescr* pDescr;
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	rotateElementByAngle(&pDescr->el,infoP);      
	pDescr = pDescr->h.next;
    }
   
 }
 
/*****************************************************************************************
//Function : showElement(Dpoint3d *pntP,int view)
//Desc     : Show element dynamically(temporary).
//Return   : None
//****************************************************************************************/
Private void   dynamicRotation(Dpoint3d *pPoint, int view )
{
    char		*cPatternP = NULL;
    int			fileNum;
    int			modifyFlags = MODIFY_ORIG;
    int			elementcount;
    double		angle = 0.0,tempangle = 0.0;
    ULong		filePos;
    MSElementDescr*	pElmDescr = NULL;
    PatternInfo		info;
    char		cPatternName[BUFF_LEN];
    
    
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT, &fileNum);
  
    if (filePos != 0 && mdlElmdscr_read(&pElmDescr, filePos, NULL, FALSE, NULL) != 0)
    {   
	if(!mdlSelect_isActive())
        {     
	    if(getPatternName(pElmDescr,cPatternName) == SUCCESS && IsValidPattern(cPatternName,OPT_TAGROTATE))
	    {
		strcpy(info.cPatternName,cPatternName);
		if(strcmpi(info.cPatternName,PATTERN_1) == 0
				|| strcmpi(info.cPatternName,PATTERN_2) == 0
				|| strcmpi(info.cPatternName,PATTERN_7) == 0
				|| strcmpi(info.cPatternName,PATTERN_8) == 0 )
		{
		    elementcount = getElementCount(pElmDescr);
		    getPatternInfo(pElmDescr,elementcount);

		    angle =  getCursorAngle(pPoint,&g_pivotpt);
		    tempangle = getCursorAngle(&g_oldrefpt,&g_pivotpt);

		    angle	    = angle-tempangle;
		    info._angle     = angle;
		    info._bdrawmode = DRAW_MODE_DYNAMIC;
                    info._elementcount = elementcount;
		    g_elementpos= 2;
		    dynamicRotateWithAngle(pElmDescr,&info);
		    g_elementpos= 1;
		}
	    }
	    else
	    {
		mdlOutput_prompt(INVALID_PATTERN);
	    }
        }
        else
        {
	    mdlSelect_freeAll();
            mdlLocate_init () ;
            mdlOutput_prompt(SELECTION_SET);		
        }
    }        
}

/*****************************************************************************************
//Function : dynRotation(Dpoint3d *pPoint, int view )
//Desc     : Rotate pattern
//Return   : None
//****************************************************************************************/
Private void   dynRotation(Dpoint3d *pPoint, int view )
{
    char		*cPatternP = NULL;
    int			fileNum;
    int			elementcount;
    ULong		filePos;
    MSElementDescr*	pElmDescr = NULL;
    PatternInfo		info;
    char		cPatternName[BUFF_LEN];
    
    
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT, &fileNum);
  
    if (filePos != 0 && mdlElmdscr_read(&pElmDescr, filePos, NULL, FALSE, NULL) != 0)
    {   
	if(getPatternName(pElmDescr,cPatternName) == SUCCESS && IsValidPattern(cPatternName,OPT_TAGROTATE))
        {
	    strcpy(info.cPatternName,cPatternName);
            if(strcmpi(info.cPatternName,PATTERN_2) == 0 || strcmpi(info.cPatternName,PATTERN_1) == 0
            ||  strcmpi(info.cPatternName,PATTERN_7) == 0 ||  strcmpi(info.cPatternName,PATTERN_8) == 0) 
	    {
		getPatternInfo(pElmDescr,elementcount);
	    }
         
	    mdlState_setAccudrawContext  (
		  ACCUDRAW_SetModePolar|ACCUDRAW_SetOrigin,   /* force mode polar coords*/
		  &g_pivotpt,			  /* default origin OK	    */
		  NULL, 		  /* delta		    */
		  NULL, 		  /* distance		    */
		  NULL, 		  /* angle		    */
		  NULL			  /* default rotation OK    */
		  );
        }
	else
        {
	    mdlOutput_prompt(INVALID_PATTERN);
        }
            
    }
   
    mdlState_setFunction(STATE_COMPLEX_DYNAMICS,dynamicRotation);
}

/*****************************************************************************************
//Function : rotatePattern_accept(Dpoint3d *pPoint,int view )	
//Desc     : Rotate pattern
//Return   : None
//****************************************************************************************/
void  rotatePattern_accept(Dpoint3d *pPoint,int view )	
{
    int			modifyFlags = MODIFY_ORIG	;
    int			fileNum,elementcount;
    double		angle = 0.0;
    double		tempangle=0.0;
    char		cPatternName[BUFF_LEN];
    char		msg[BUFF_LEN];	    
    ULong		filePos;
    MSElementDescr*	pElmDescr = NULL;
    RotMatrix		rotMatrix;
    PatternInfo		info;
    
    
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT,&fileNum);
    if( mdlElmdscr_read (&pElmDescr, filePos, NULL, FALSE,NULL) != 0)
    {
       if(!mdlSelect_isActive())
       {        
	   if(getPatternName(pElmDescr,cPatternName) == SUCCESS && IsValidPattern(cPatternName,OPT_TAGROTATE))
	   {
		strcpy(info.cPatternName,cPatternName);

		if(strcmp(info.cPatternName,PATTERN_1) == 0 || strcmp(info.cPatternName,PATTERN_2) ==0
		 || strcmp(info.cPatternName,PATTERN_7) ==0 || strcmp(info.cPatternName,PATTERN_8) ==0 )
		{

		    elementcount = getElementCount(pElmDescr);
		    getPatternInfo(pElmDescr,elementcount);
		    angle     = getCursorAngle(pPoint,&g_pivotpt);
		    tempangle = getCursorAngle(&g_oldrefpt,&g_pivotpt);
		    angle     = angle-tempangle;
                     
		    strcpy(info.cPatternName,cPatternName);
		    info._angle = angle;
                    info._elementcount = elementcount;
		    mdlString_fromAngle(msg,tempangle,FALSE,FALSE,FALSE,FALSE);
		   //mdlDialog_openAlert(msg);
		    mdlString_fromAngle(msg,angle,FALSE,FALSE,FALSE,FALSE);
		   //mdlDialog_openAlert(msg);
		    g_elementpos = 1;
		    mdlModify_elementMulti(fileNum, filePos, MODIFY_REQUEST_HEADERS,
					modifyFlags, elementModify_withAngle, &info, 1);
		    g_elementpos = 1;
		}

		mdlElmdscr_freeAll(&pElmDescr) ;
		dgneditor_updateViews();
	   }
	   else
	   {
		mdlOutput_prompt(INVALID_PATTERN);
	    }				
        }
        
        mdlState_setFunction(STATE_RESET,resetAction);
	mdlState_restartCurrentCommand();
    }
    
    g_LineCnt = 0;
        
}
/*****************************************************************************************
//Function : rotatePattern
//Desc     : Rotate cell element - PATTERNS
//Return   : None
//****************************************************************************************/
cmdName void    rotatePattern
(
    char        *unparsedP
)
cmdNumber CMD_DAELIM_TAGROTATE
{
    StatusInt   status = SUCCESS;
    int		gFilePos = 0L;

    mdlLocate_init();
    mdlLocate_normal ();
    mdlState_startModifyCommand (rotatePattern,
				 rotatePattern_accept,
				 dynRotation, 
                                 NULL, 
                                 NULL,
				 COMMANDID_RotatePattern, 
                                 PROMPTID_CustomAcceptReject,
				 TRUE, 
                                 1);
                                 
    
                                                                       
    mdlState_setFunction(STATE_RESET,resetAction);	
}
     
/*****************************************************************************************
//Function : elementModify_shape
//Desc     : -
//Return   : int
//****************************************************************************************/
Private int elementModify_shape 
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *infoP      /* => from params in mdlModify_element... */
)
{

    return rotateArrow(el,infoP);
    
} 

/*****************************************************************************************
//Function : connectToNearestPoint(DPoint3d* fixedPtP,DPoint3d* ptArrP)
//Desc     : PATTERN-2 Change the leader line.
//Return   : void
//****************************************************************************************/
Private void findNearestPoint(DPoint3d* points,PatternInfo* info)
{

    double      dpt1,dpt2;
    int		nCnt =  0;
    DPoint3d    temppoint;
    
    points[0] = g_startpointofline;
    
    if(strcmp(info->cPatternName,PATTERN_2) == 0 )  // Pattern -2
    {
	for(nCnt = 0;nCnt < 2;nCnt++)
        {
            if(nCnt != 0)
            {
	      dpt2 = mdlVec_distance(&g_startpointofline,&g_div_line[nCnt]);		
              if(dpt1 > dpt2)
              {
		dpt1 = dpt2;
                temppoint = g_div_line[nCnt];
              }
            }   
            else
            {
		dpt1 = mdlVec_distance(&g_startpointofline,&g_div_line[nCnt]);
                temppoint = g_div_line[nCnt];
            }
        }
	g_nearestpoint = temppoint;
        points[1] = temppoint; 
    }    
    else if(strcmp(info->cPatternName,PATTERN_3) == 0)  // Pattern -3
    {
	for(nCnt = 0;nCnt < 5;nCnt++)
        {
            if(nCnt != 0)
            {
	      dpt2 = mdlVec_distance(&g_startpointofline,&g_textshape[nCnt]);		
              if(dpt1 > dpt2)
              {
		dpt1 = dpt2;
                temppoint = g_textshape[nCnt];
              }
            }   
            else
            {
		dpt1 = mdlVec_distance(&g_startpointofline,&g_textshape[nCnt]);
                temppoint = g_textshape[nCnt];
            }
        }
	g_nearestpoint = temppoint;
        points[1] = temppoint; 
    }
    else if(strcmp(info->cPatternName,PATTERN_7) == 0) // Pattern -7
    {
	for(nCnt = 0;nCnt < 7;nCnt++)
        {
            if(nCnt != 0)
            {
	      dpt2 = mdlVec_distance(&g_startpointofline,&g_flathexagon[nCnt]);		
              if(dpt1 > dpt2)
              {
		dpt1 = dpt2;
                temppoint = g_flathexagon[nCnt];
              }
            }   
            else
            {
		dpt1 = mdlVec_distance(&g_startpointofline,&g_flathexagon[nCnt]);
                temppoint = g_flathexagon[nCnt];
            }
        }
	g_nearestpoint = temppoint;
        points[1] = temppoint; 
    }
    else if(strcmp(info->cPatternName,PATTERN_8) == 0 ) // Pattern -8
    {
	for(nCnt = 0;nCnt < 4;nCnt++)
        {
            if(nCnt != 0)
            {
	      dpt2 = mdlVec_distance(&g_startpointofline,&g_roundrect[nCnt]);		
              if(dpt1 > dpt2)
              {
		dpt1 = dpt2;
                temppoint = g_roundrect[nCnt];
              }
            }   
            else
            {
		dpt1 = mdlVec_distance(&g_startpointofline,&g_roundrect[nCnt]);
                temppoint = g_roundrect[nCnt];
            }
        }
	g_nearestpoint = temppoint;
        points[1] = temppoint; 
    }
    else
    {
	dpt1 = mdlVec_distance(&g_startpointofline,&g_div_line[0]);
	dpt2 = mdlVec_distance(&g_startpointofline,&g_div_line[1]);

	if(dpt1 < dpt2)
	    points[1] = g_div_line[0];
	else
	    points[1] = g_div_line[1];

	g_nearestpoint = points[1];
    }
}   
 


/*****************************************************************************************
//Function : moveElement
//Desc     : Move individual element.
//Return   : It retursn modify status. 
//****************************************************************************************/
Private int moveElement
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *info,         /* => from params in mdlModify_element... */
    int			fileNum,
    boolean		bDrawMode
)

{
    
    DPoint3d    points[2];
    DPoint3d    shapepoints[7];
    DPoint3d    startoffset,tempdist,ptlineceter;
    char	msg[256];
    MSElementDescr* pTempElmDescr = NULL,*pTempLineDescr= NULL;
    MSElement   elm,el_shape; 
    DPoint3d    ptElm1,ptElm2;
    int         iElmType,nvertices;
        
    
    iElmType = mdlElement_getType(el);
        
    if( strcmp(info->cPatternName,PATTERN_1) == 0 ||
	strcmp(info->cPatternName,PATTERN_2) == 0 ||
        strcmp(info->cPatternName,PATTERN_3) == 0 ||
        strcmp(info->cPatternName,PATTERN_4) == 0 ||
        strcmp(info->cPatternName,PATTERN_5) == 0 ||
        strcmp(info->cPatternName,PATTERN_6) == 0 ||
        strcmp(info->cPatternName,PATTERN_7) == 0 ||
        strcmp(info->cPatternName,PATTERN_8) == 0
        )       
    {
	/* Dont move following elements Shape/Line string - Arrows*/
        
        if( /* Pattern -2 */
	    (strcmp(info->cPatternName,PATTERN_2) == 0 && 
		    (g_elementpos == ELM_POS_2||(g_elementpos == ELM_POS_3 && 
				    (iElmType == LINE_STRING_ELM || iElmType == SHAPE_ELM)))) ||
            /* Pattern -3 */        
	    (strcmp(info->cPatternName,PATTERN_3) == 0 && 
		    (g_elementpos == ELM_POS_3))||
            /* Pattern -4 */        
            (strcmp(info->cPatternName,PATTERN_4) == 0 && 
	   	    (g_elementpos == ELM_POS_3 || g_elementpos == ELM_POS_4))||
	    /* Pattern -5 & Pattern - 6*/        
            ((strcmp(info->cPatternName,PATTERN_5) == 0 || 
              strcmp(info->cPatternName,PATTERN_6) == 0) && 
                    g_elementpos == ELM_POS_4)||
	    /* Pattern -7 */                                                         
            (strcmp(info->cPatternName,PATTERN_7) == 0 && 
                    (g_elementpos == ELM_POS_3 || g_elementpos == ELM_POS_4
		    || g_elementpos == ELM_POS_5 || g_elementpos == ELM_POS_6))|| 
            /* Pattern -8 */        
            (strcmp(info->cPatternName,PATTERN_8) == 0 && 
                    (g_elementpos == ELM_POS_6 || g_elementpos == ELM_POS_7 
	            || g_elementpos == ELM_POS_8 || g_elementpos == ELM_POS_9)) 
                    
          )  
	{  
            g_elementpos ++;
	    return MODIFY_STATUS_REPLACE;
	}
        
        /* Offset project point */
        if( strcmp(info->cPatternName,PATTERN_2) == 0 ||
	   // strcmp(info->cPatternName,PATTERN_4) == 0 ||
           // strcmp(info->cPatternName,PATTERN_5) == 0 ||
           // strcmp(info->cPatternName,PATTERN_6) == 0 ||
            strcmp(info->cPatternName,PATTERN_7) == 0 ||
            strcmp(info->cPatternName,PATTERN_8) == 0

          )
        { 
	    mdlVec_subtractPoint (&startoffset,&g_anchor,&g_leader_line[1]);
	    mdlElement_offset (el, el, &startoffset);
	    mdlElement_display (el, ERASE);
        }
        
        /* Offset elemenent by given distance*/
        /* For each pattern the calling location of this function is different*/
	if( strcmp(info->cPatternName,PATTERN_3) != 0 )
        {
	    if (mdlElement_offset (el, el, &info->_distance))
	    {
		return MODIFY_STATUS_ERROR     ;
	    }
        }
        
        if((strcmp(info->cPatternName,PATTERN_2) == 0 && (g_elementpos == ELM_POS_4 || g_elementpos == ELM_POS_3)))
		
	{
	    mdlLinear_extract(points,NULL,el,MASTERFILE);
	    g_div_line[0] = points[0];
	    g_div_line[1] = points[1];
	}
        
        /* Pattern -3 */
        if(  (strcmp(info->cPatternName,PATTERN_3) == 0 && g_elementpos == ELM_POS_4))
	{            
	    mdlText_extractShape(g_textshape,NULL,el,FALSE,0);
            if(mdlElement_getType(el) == TEXT_ELM)
            {
		mdlShape_create(&el_shape,NULL,g_textshape,4,0); 
                mdlElmdscr_new(&pTempElmDescr,NULL,&el_shape);
            }
            
            mdlMeasure_areaProperties(NULL,NULL,NULL,&g_centerPt,NULL,NULL,NULL,NULL,NULL,NULL,pTempElmDescr,1000);
            
            mdlVec_subtractPoint (&startoffset,&g_anchor,&g_centerPt);
            mdlElement_offset (el, el, &startoffset);
            mdlElement_offset (el, el, &info->_distance);	    
	    mdlText_extractShape(g_textshape,NULL,el,FALSE,0);
            
            mdlElmdscr_freeAll(&pTempElmDescr);

            if(mdlElement_getType(el) == TEXT_ELM)
            {
		mdlShape_create(&el_shape,NULL,g_textshape,4,0); 
                mdlElmdscr_new(&pTempElmDescr,NULL,&el_shape);
            }
	        
	    mdlMeasure_areaProperties(NULL,NULL,NULL,&g_centerPt,NULL,NULL,NULL,NULL,NULL,NULL,pTempElmDescr,1000);
            
            // To find out instersect point draw temp line element
            points[0] = g_startpointofline;
	    points[1] = g_centerPt;            
            
            mdlLine_create (&elm, NULL, points);
            mdlElmdscr_new(&pTempLineDescr,NULL,&elm);

            mdlIntersect_closestBetweenElms(&ptElm1,&ptElm2,NULL,pTempLineDescr,pTempElmDescr,NULL,&g_centerPt,1000);
            g_intersectPt = ptElm1; 
            mdlElmdscr_freeAll(&pTempElmDescr);
            mdlElmdscr_freeAll(&pTempLineDescr);

	}
        /* Pattern -4 */
        if( (strcmp(info->cPatternName,PATTERN_4) == 0 || strcmp(info->cPatternName,PATTERN_5) == 0) 
		    && g_elementpos == ELM_POS_2) 
	{
	    mdlElmdscr_new(&pTempElmDescr,NULL,el);
	    mdlMeasure_areaProperties(NULL,NULL,NULL,&g_centerPt,NULL,NULL,NULL,NULL,NULL,NULL,pTempElmDescr,1000);
         
            // To find out instersect point draw temp line element
            points[0] = g_startpointofline;
	    points[1] = g_centerPt;            
            mdlLine_create (&elm, NULL, points);
            mdlElmdscr_new(&pTempLineDescr,NULL,&elm);

            mdlIntersect_closestBetweenElms(&ptElm1,&ptElm2,NULL,pTempLineDescr,pTempElmDescr,NULL,&g_centerPt,1000);
            g_intersectPt = ptElm1; 
           
            mdlElmdscr_freeAll(&pTempElmDescr);
            mdlElmdscr_freeAll(&pTempLineDescr);
            
	}
        /* Pattern - 6 */
        if( strcmp(info->cPatternName,PATTERN_6) == 0 && g_elementpos == ELM_POS_2) // Elispe element - extract the center point
	{
	    if(mdlElement_getType(el) != SHAPE_ELM)
            {
		mdlLinear_extract(shapepoints,&nvertices,el,MASTERFILE);
		mdlShape_create(&el_shape,NULL,shapepoints,nvertices,0); 
                mdlElmdscr_new(&pTempElmDescr,NULL,&el_shape);
            }
	    else
		mdlElmdscr_new(&pTempElmDescr,NULL,el);
                
	    mdlMeasure_areaProperties(NULL,NULL,NULL,&g_centerPt,NULL,NULL,NULL,NULL,NULL,NULL,pTempElmDescr,1000);
            
            // To find out instersect point draw temp line element
            points[0] = g_startpointofline;
	    points[1] = g_centerPt;            
            mdlLine_create (&elm, NULL, points);
            mdlElmdscr_new(&pTempLineDescr,NULL,&elm);

            mdlIntersect_closestBetweenElms(&ptElm1,&ptElm2,NULL,pTempLineDescr,pTempElmDescr,NULL,&g_centerPt,1000);
            g_intersectPt = ptElm1; 
            mdlElmdscr_freeAll(&pTempElmDescr);
            mdlElmdscr_freeAll(&pTempLineDescr);
            
	}
        
        /* Pattern -7*/
        if(strcmp(info->cPatternName,PATTERN_7) == 0 && g_elementpos == ELM_POS_2) // Text element - extract the range information
	{
	    mdlLinear_extract(g_flathexagon,NULL,el,MASTERFILE);	    
        }
        /* Pattern -8*/
        if(strcmp(info->cPatternName,PATTERN_8) == 0  && g_elementpos == ELM_POS_2) // Upper line element
	{
	    mdlLinear_extract(points,NULL,el,MASTERFILE);	
            g_roundrect[0] = points[0];
            g_roundrect[1] = points[1];
        }
        if( strcmp(info->cPatternName,PATTERN_8) == 0 && g_elementpos == ELM_POS_4) // Lower line element
	{
	    mdlLinear_extract(points,NULL,el,MASTERFILE);	
            g_roundrect[2] = points[0];
            g_roundrect[3] = points[1];
        }
        
	/* Show element temporary in dynamic mode*/ 
        if(bDrawMode == DRAW_MODE_DYNAMIC)
        {
	   if(   strcmp(info->cPatternName,PATTERN_1) == 0 ||
	         strcmp(info->cPatternName,PATTERN_2) == 0 ||
                 strcmp(info->cPatternName,PATTERN_8) == 0 ||
               ( strcmp(info->cPatternName,PATTERN_3) == 0 && g_elementpos != ELM_POS_2)||
               ((strcmp(info->cPatternName,PATTERN_4) == 0 ||
                 strcmp(info->cPatternName,PATTERN_5) == 0 ||
                 strcmp(info->cPatternName,PATTERN_6) == 0 ||
                 strcmp(info->cPatternName,PATTERN_7) == 0 ) && g_elementpos != ELM_POS_3)
             )
	    
	    mdlElement_display(el,TEMPDRAW);
        }
            
        g_elementpos++;      		
    }
    
    return  MODIFY_STATUS_REPLACE	 ;    
    
  
}

/*****************************************************************************************
//Function : elementModify_move
//Desc     : This is call back function it calls for all elements present in the cell desc-
//	     iptor.
//Return   : Returns modify status.
//****************************************************************************************/
Private int elementModify_move
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *info,         /* => from params in mdlModify_element... */
    int			fileNum
)
{
    return moveElement(el,info,fileNum,FALSE);
}



/*****************************************************************************************
//Function : modifyLeaderLine
//Desc     :
//Return   : int
//****************************************************************************************/
Private int modifyLeaderLine
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *info,         /* => from params in mdlModify_element... */
    int			fileNum,
    boolean		bDrawMode
)
{
    int			iVertices;
    DPoint3d		points[2];
    
    
    if( ((strcmp(info->cPatternName,PATTERN_2) ==0 || strcmp(info->cPatternName,PATTERN_3) ==0) 
			&& g_elementpos == ELM_POS_2)
                        || (strcmp(info->cPatternName,PATTERN_7) ==0 && g_elementpos == ELM_POS_3) 
			|| (strcmp(info->cPatternName,PATTERN_8) ==0 && g_elementpos == ELM_POS_6))                                     
    {
    
	mdlLinear_extract(points,NULL,el,MASTERFILE);
	findNearestPoint(points,info);

	mdlLine_create (el, el, points);
	g_new_div_line[0] = points[0];
	g_new_div_line[1] = points[1];
        
	if(bDrawMode == DRAW_MODE_DYNAMIC)
	    mdlElement_display(el,TEMPDRAW);
     
    }
           
    else if( (strcmp(info->cPatternName,PATTERN_3) ==0 && g_elementpos == ELM_POS_2)
		|| ((strcmp(info->cPatternName,PATTERN_4) == 0 || 
		     strcmp(info->cPatternName,PATTERN_5) == 0 || 
                     strcmp(info->cPatternName,PATTERN_6) == 0)  && g_elementpos == ELM_POS_3) )
    {
	mdlLinear_extract(points,NULL,el,MASTERFILE);

        //mdlElement_display(el,ERASE);

	points[0] = g_startpointofline;
	points[1] = g_intersectPt;
	mdlLine_create (el, el, points);
	g_new_div_line[0] = points[0];
	g_new_div_line[1] = points[1];

	if(bDrawMode == DRAW_MODE_DYNAMIC)
        {
	    //mdlElement_display(el,ERASE);
	    mdlElement_display(el,TEMPDRAW);
        }
           }
    
    g_elementpos ++;
        
    /* indicate that we did something to the element */
    return  MODIFY_STATUS_REPLACE	 ;
    
   
}

/*****************************************************************************************
//Function : elementModify_move
//Desc     : Move selected pattern
//Return   : int
//****************************************************************************************/
Private int elementModify_coneectLeaderLine
(
    MSElementUnion      *el,           /* <> element to be modified */
    PatternInfo         *info,         /* => from params in mdlModify_element... */
    int			fileNum,
    MSElementDescr      *pDescr
)
{
    return modifyLeaderLine(el,info,fileNum,FALSE);
}

/*****************************************************************************************
//Function : 

//Desc     : Get distance
//Return   : int
//****************************************************************************************/
Private void getMoveDistance

(

    Dpoint3d         *distance,            /* <= distance from anchor */
    Dpoint3d         *pt            /* => current point */

)

{
      
    Dpoint3d anchor;
    mdlCnv_IPointToDPoint(&anchor, statedata.pointstack);	
    g_elementpickpt = anchor;
    g_anchor = anchor;
    
    mdlVec_subtractPoint (distance, pt, &g_elementpickpt)	;

}
                   
/*****************************************************************************************
//Function : movePatterns(uLong filePos,char* cPatternName)
//Desc     : -
//Return   : void 
//****************************************************************************************/
Private void movePatterns(MSElementDescr *pElmDescr,ULong filePos,int fileNum,DPoint3d* pt)
{    
    int 	    modifyFlags = MODIFY_ORIG	;
    int		    elementcount;
    double	    angle,tempangle;
    char	    cPatternName[BUFF_LEN];
    char            msg[256];
    char            ctempAngle[256];
    char            cAngle[256];
    char            cdiffAngle[256];

    Dpoint3d	    distance;
    PatternInfo     patterninfo;
      
    if(getPatternName(pElmDescr,cPatternName) == SUCCESS)
    {    
	/* Get the distance by which to move each element. */
        getMoveDistance (&distance, pt)	 ;
        
	strcpy(patterninfo.cPatternName,cPatternName);
        patterninfo._distance     = distance;
	patterninfo._pt 	  = *pt;
	patterninfo._elementcount = elementcount;


	if (!mdlSelect_isActive())
	  modifyFlags |= MODIFY_DRAWINHILITE	    ;

	getPatternInfo(pElmDescr,elementcount);

	tempangle = getCursorAngle(&g_div_line[1],&g_startpointofline);

	// Pattern -1
	if(strcmp(patterninfo.cPatternName,PATTERN_1) == 0)   // Pattern -1
	{
	    /* Now move each element the user accepted. */
	    g_elementpos = 1;
	    mdlModify_elementMulti (fileNum, filePos, MODIFY_REQUEST_HEADERS,
				    modifyFlags, elementModify_move, &patterninfo, 1);
	}
	// Pattern -2
	else
	{
	   ///////////////////////////////////////////////////////////////////////
//	     /* Now move each element the user accepted. */
//	    g_elementpos = 1;
//	    mdlModify_elementMulti (fileNum, filePos, MODIFY_REQUEST_HEADERS,
//				    modifyFlags, elementModify_offset, &patterninfo, 1);
//	    g_elementpos = 1; // Reset to 1
//
//           
           //////////////////////////////////////////////////////////////////////
           
            /* Now move each element the user accepted. */
	    g_elementpos = 1;
	    mdlModify_elementMulti (fileNum, filePos, MODIFY_REQUEST_HEADERS,
				    modifyFlags, elementModify_move, &patterninfo, 1);
	    g_elementpos = 1; // Reset to 1

	    /* Now move each element the user accepted. */
	    mdlModify_elementMulti (fileNum, filePos, MODIFY_REQUEST_HEADERS,
				    modifyFlags, elementModify_coneectLeaderLine, &patterninfo, 1);
	    g_elementpos = 1; // Reset to 1

	    angle = getCursorAngle(&g_new_div_line[1],&g_startpointofline);
	    angle = angle-tempangle;
	    patterninfo._angle = angle;
	    mdlModify_elementMulti (fileNum, filePos, MODIFY_REQUEST_HEADERS,
				    modifyFlags, elementModify_shape, &patterninfo, 1);
	    g_elementpos = 1; // Reset to 1
	}

	/* Reload the dynamic buffer with the new element. */
	mdlState_restartCurrentCommand();
    }
    else 
    {
	mdlOutput_prompt(INVALID_PATTERN);   
    }
              
}
 

/*****************************************************************************************
//Function : offsetElement
//Desc     : Show movement of pattern dynamically.
//Return   : int
//****************************************************************************************/
Private void dynamicConnectLeaders(MSElementDescr* pElmDescr, PatternInfo* info,ULong fileNum)
{
    MSElementDescr* pDescr = NULL;
   
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	modifyLeaderLine(&pDescr->el,info,fileNum,TRUE);        
        pDescr = pDescr->h.next;
    }    
}

/*****************************************************************************************
//Function : offsetElement
//Desc     : Show movement of pattern dynamically.
//Return   : int
//****************************************************************************************/
Private void dynamicMoveAll(MSElementDescr* pElmDescr, PatternInfo* info,ULong fileNum)
{
    MSElementDescr* pDescr = NULL;

    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	moveElement(&pDescr->el,info,fileNum,TRUE);        
        pDescr = pDescr->h.next;
    }    
} 

/*****************************************************************************************
//Function : offsetElement
//Desc     : Show movement of pattern dynamically.
//Return   : int
//****************************************************************************************/
Private void dynamicRotateArrow(MSElementDescr* pElmDescr, PatternInfo* info)
{
    int iLineCnt,elmtype;
    MSElementDescr* pDescr = NULL;
    
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	rotateArrow(&pDescr->el,info);        
        pDescr = pDescr->h.next;
    }    
}

/*****************************************************************************************
//Function : offsetElement
//Desc     : Show movement of pattern dynamically.
//Return   : int
//****************************************************************************************/
Private void dynamicMove(Dpoint3d *pt, int view )
{
    char            cPatternName[256];  
    int		    iCnt;
    double	    angle,tempangle;
    int 	    modifyFlags = MODIFY_ORIG;
    
    ULong	    fileNum, filePos;
    MSElementDescr  *pElmDescr;
    Dpoint3d	    distance,initialoffset;    
    PatternInfo info;
    
	    
    filePos = mdlElement_getFilePos (FILEPOS_CURRENT,&fileNum);

    if(filePos != 0 && mdlElmdscr_read(&pElmDescr, filePos, NULL, FALSE, NULL) != 0)
    { 
        if(pElmDescr != NULL)
        {
	    
            if(!mdlSelect_isActive())
            {
		// Get Pattern Name
		if( getPatternName(pElmDescr,cPatternName)== SUCCESS && IsValidPattern(cPatternName,OPT_TAGMOVE))
		{
		    // Get Pattern information
		    getPatternInfo(pElmDescr,iCnt);
		    // Get offset distance
		    getMoveDistance (&distance, pt);

		    tempangle = getCursorAngle(&g_div_line[1],&g_startpointofline);

		    info._distance = distance;
		    strcpy(info.cPatternName,cPatternName);
		    info._pt = *pt;
		    info._bdrawmode = DRAW_MODE_DYNAMIC;
		    //g_elementpos = 2;

		   //dynamicOffsetElements(pElmDescr,&info,fileNum);

		    g_elementpos = 2;
		    dynamicMoveAll(pElmDescr,&info,fileNum);

		    g_elementpos = 2;
		    dynamicConnectLeaders(pElmDescr,&info,fileNum);

		    angle = getCursorAngle(&g_new_div_line[1],&g_startpointofline);
		    angle = angle-tempangle;
		    info._angle = angle;

		    g_elementpos = 2;
		    dynamicRotateArrow(pElmDescr,&info);
		    g_elementpos = 1;

		 }
		 else
		 {
		    mdlLocate_init () ;
		    mdlOutput_prompt(INVALID_PATTERN);
		 }
            }
            else
            {
		mdlSelect_freeAll();
		mdlLocate_init () ;
                mdlOutput_prompt(SELECTION_SET);		
            }

	   
        } 
     }  
}

/*****************************************************************************************
//Function : dynamicShift(Dpoint3d *pt, int view )
//Desc     : Dynamic function
//Return   : void 
//****************************************************************************************/
Private void dynamicShift(Dpoint3d *pt, int view )
{
    mdlState_setFunction(STATE_COMPLEX_DYNAMICS,dynamicMove);
}

/*****************************************************************************************
//Function :moveElement_accept
//Desc     : -
//Return   : void 
//****************************************************************************************/
Private void moveElement_accept

(                                                                  

Dpoint3d         *pt      /* => final point for move element */

)

{

    ULong	    filePos;
    ULong	    fileNum;
    MSElementDescr  *pElmDescr = NULL;
   
    if(!mdlSelect_isActive())
    {

	/* Get the file position and file number of the element to move. */
	filePos = mdlElement_getFilePos (FILEPOS_CURRENT,&fileNum); // commented temporary
	mdlElmdscr_read(&pElmDescr,filePos, NULL, FALSE,NULL);	   // commented temporary

	movePatterns(pElmDescr,filePos,fileNum,pt);

	// update all the views in dgn file
	dgneditor_updateViews();

	mdlState_setFunction(STATE_RESET,resetAction);


	mdlElmdscr_freeAll(&pElmDescr) ;

	mdlState_restartCurrentCommand();
     }
        
 }

/*****************************************************************************************
//Function : movePattern()
//Desc     : Use state functions to identify and modify element.
//Return   : void 
//****************************************************************************************/
cmdName void    movePattern
(
    char            *unparsedP
)
cmdNumber       CMD_DAELIM_TAGMOVE
{  	
   
    mdlLocate_init();
    mdlLocate_normal (); 

    mdlState_startModifyCommand ( movePattern,
				  moveElement_accept,
				  dynamicShift,
				  NULL,
				  NULL,
				  COMMANDID_MovePattern,
				  PROMPTID_CustomAcceptReject,
				  TRUE,
				  2);
                                     
   
    mdlState_setFunction(STATE_RESET,resetAction);   
     
}



////////////////////////////////////////////////////////////////////////////////////////////////////

Private void offsetElement(MSElementDescr* pElmDescr,DPoint3d* pt)
{
   
    MSElementDescr* pDescr = NULL;
    MSElement       el;
    DPoint3d	    startoffset;
    DPoint3d	    points[2];    
    DPoint3d        distance;              
    // Get cell header
    pDescr = pElmDescr->h.firstElem;
    // Traverse through cell descriptor
    while (pDescr != NULL)
    {
	if(mdlElement_getType(&pDescr->el) == TEXT_ELM)
	{	    
	    mdlVec_subtractPoint (&startoffset,&g_anchor,&g_leader_line[1]);
	    mdlElement_offset (&pDescr->el, &pDescr->el, &startoffset);
            getMoveDistance(&distance,pt);
            mdlElement_offset (&pDescr->el, &pDescr->el, &distance);
            mdlElement_display(&pDescr->el,TEMPDRAW);
	}
        else if(mdlElement_getType(&pDescr->el) == LINE_ELM)
	{	 
	    mdlVec_subtractPoint (&startoffset,&g_anchor,&g_leader_line[1]);
            mdlLinear_extract(points,NULL,&pDescr->el,MASTERFILE);
            points[1] = *pt;
	    mdlLine_create(&pDescr->el,&pDescr->el,points);
	    mdlElement_display(&pDescr->el,TEMPDRAW);
	}
        
	pDescr = pDescr->h.next;
    }    
}


/*****************************************************************************************
//Function : main()
//Desc     : Entry point of program
//Return   : int 
//****************************************************************************************/

Public  int     main
(
    int             argc,
    char            *argv[]
)
{
    int 	rscFileH;   /* a resource file handle */
    char	    *setP;

//    mdlSystem_enterDebug();

    if (mdlParse_loadCommandTable (NULL) == NULL)
    {
	mdlOutput_rscPrintf (MSG_ERROR, NULL, MESSAGELISTID_Msgs, MSGID_LoadCmdTbl);
	mdlDialog_cmdNumberQueue (FALSE, CMD_MDL_UNLOAD,
				      mdlSystem_getCurrTaskID(), TRUE);
    }

    if (mdlResource_openFile (&rscFileH, NULL, 0) != SUCCESS)
    {
	return (ERROR);
    }

    /* Publish vatiables used for dimension edit */
    setP = mdlCExpression_initializeSet (VISIBILITY_DIALOG_BOX, 0, 0);
    mdlDialog_publishComplexVariable (setP, "dimeditinfo",
				      "dimeditInfo", &dimeditInfo);

    //
    /*
    // mdlState_registerStringIds () takes two arguments. The first is the
    // id of the stringlist holding the Command messages, the second is the id
    // of the stringlist holding the prompt messages.
    */
    mdlState_registerStringIds (MESSAGELISTID_Commands, MESSAGELISTID_Prompts);

    /* Open MicroStation editor icon pallette */
    mdlDialog_open (NULL, DIALOGID_PWDgnEditor);

    return (SUCCESS);
}

