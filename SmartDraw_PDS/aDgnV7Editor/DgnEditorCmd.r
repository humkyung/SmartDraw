#pragma suppressREQCmds
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
|   myappcmd.r - MyApp Command Table hierarchy 	    	    	    	|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
|									|
|   Local Defines							|
|									|
+----------------------------------------------------------------------*/
#define	CT_NONE			0
#define CT_MAIN			1
#define CT_DLMANIPULATE		2

/*----------------------------------------------------------------------+
|									|
|   Table <tableId> =    						|
|   {									|
|      	{ <no.>, <subTableId>, <cmdClass>, <options>, "<cmdWord>" };	|
|   };									|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   MyApp commands							|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Main (Root) command word table					|
|                                                                       |
+----------------------------------------------------------------------*/
  


Table CT_MAIN = 
{
    {1,CT_DLMANIPULATE,MANIPULATION,REQ,"Daelim"},
    {2,CT_NONE,MANIPULATION,NONE,"EditDmns"},
} 
Table CT_DLMANIPULATE = 
{
    {1, CT_NONE, INHERIT, NONE,"DimJustifyLeft"},
    {2, CT_NONE, INHERIT, NONE,"DimJustifyCenter"},
    {3, CT_NONE, INHERIT, NONE,"DimJustifyRight"},
    {4, CT_NONE, INHERIT, NONE,"DimTextMoveAbove"},
    {5, CT_NONE, INHERIT, NONE,"DimTextMoveInline"},
    {6, CT_NONE, INHERIT, NONE,"DimTextMoveBelow"},
    {7, CT_NONE, INHERIT, NONE,"DimChangeLeftBottomTermiantorToDot"},
    {8, CT_NONE, INHERIT, NONE,"DimChangeCenterTermiantorsToDot"},
    {9, CT_NONE, INHERIT, NONE,"DimChangeRightTopTermiantorToDot"},
    {10, CT_NONE, INHERIT, NONE,"TagMove"},
    {11, CT_NONE, INHERIT, NONE,"TagRotate"},
}

  
