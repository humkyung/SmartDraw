#include <assert.h>
#include "Insert.h"
#include "aDgnV7.h"
#include "utility.h"
#include <mselemen.fdf>

char szBuf[256];

int elmFunc (MSElementDescr *elemDscrP);
/*----------------------------------------------------------------------+
|									|
| name		loadCellLibraryIfNeeded					|
|									|
| author	BSI					6/91		|
|									|
+----------------------------------------------------------------------*/
Public int loadCellLibraryIfNeeded(char *libToLoad)
{
	char libName[MAXFILELENGTH];
	int res = 0;

    	if(*libToLoad && strcmp(libToLoad,tcb->celfilenm))
	{
		res = mdlCell_attachLibrary(libName,libToLoad,NULL,TRUE);
		if(SUCCESS != res)
		{
			sprintf(libName , "res = %d , path = %s,%d" , res , libToLoad , strlen(libToLoad));
			mdlDialog_dmsgsPrint(libName);

			return res;
		}
	}

	return SUCCESS;
}

/**
	@brief	

	@author	HumKyung.Baek

	@date	????.??.??

	@return	boolean
*/
boolean Draw_Insert(Insert *pInsert, int nLayer, char *sExplode)
{	
	int share = 0;
	long location = 0;
	MSElementDescr *cellDescrP=NULL;
	MSElementDescr *cellDP=NULL;
	Dpoint3d  scale,ptsDiff,ptsOrigin,ptsOffset;
	Dvector3d rangeP;
	RotMatrix       rMatrix,rotMatrix;
	Transform       tMatrix;
	int view = 0;
	double unitVal = tcb->uorpersub;
	MSElementDescr *elemDscrP;

	assert(pInsert && "pInsert is NULL");
	mdlRMatrix_fromAngle(&rotMatrix, GetRadian(pInsert->fAngle));
	
	if(pInsert)
	{
		loadCellLibraryIfNeeded(pInsert->psCellLibPath);
		if(0L != (location = mdlCell_getFilePosInLibrary(pInsert->psCellName)))
		{
			mdlCell_getElmDscr (&cellDescrP, NULL, location, NULL, &pInsert->ptScale, &rotMatrix, NULL, 0, share, pInsert->psCellName);

			if(0 == strcmpi(pInsert->sWhere, "CENTER"))
			{
				if( SUCCESS == mdlElement_extractRange(&rangeP,&cellDescrP->el))
				{
					//Find range of cell.
					ptsDiff.x = (rangeP.end.x + rangeP.org.x)/2; //* unitVal;
					ptsDiff.y = (rangeP.end.y + rangeP.org.y)/2; //* unitVal;
					ptsDiff.z = 0;//rangeP.end.z - rangeP.org.z;

					//Find origin of cell.
					mdlCell_extract(&ptsOrigin,NULL,NULL,NULL,NULL,&cellDescrP->el);

					//Find out offset.
					ptsOffset.x = -(ptsDiff.x - ptsOrigin.x);
					ptsOffset.y = -(ptsDiff.y - ptsOrigin.y);
					ptsOffset.z = -(ptsDiff.z - ptsOrigin.z);

					/* --- adjust for position --- */
					mdlTMatrix_getIdentity (&tMatrix);
					mdlTMatrix_translate (&tMatrix, &tMatrix, ptsOffset.x, ptsOffset.y, ptsOffset.z);
					mdlElmdscr_transform (cellDescrP, &tMatrix);
				}
				else
				{
					return FALSE;
				}
			}
			
			if (cellDescrP)
			{
				///mdlElmdscr_duplicate (&cellDP, cellDescrP);
				/* --- adjust for active angle and view rotation --- */
				mdlTMatrix_getIdentity (&tMatrix);
				///mdlRMatrix_fromView (&rMatrix, view, FALSE);
				///mdlRMatrix_invert (&rMatrix, &rMatrix);
				///mdlTMatrix_rotateByRMatrix (&tMatrix, &tMatrix, &rMatrix);
				mdlTMatrix_rotateByAngles (&tMatrix, &tMatrix, 0.0, 0.0,tcb->actangle*fc_piover180);
				mdlElmdscr_transform (cellDescrP, &tMatrix);

				/* --- adjust for active scale --- */
				mdlTMatrix_getIdentity (&tMatrix);
				mdlParams_getActive (&scale, ACTIVEPARAM_SCALE);
				
				//! not necessary below code
				///mdlTMatrix_scale (&tMatrix , &tMatrix , (pInsert->ptScale.x) , (pInsert->ptScale.y) , (pInsert->ptScale.z));
				mdlElmdscr_transform (cellDescrP, &tMatrix);

				/* --- adjust for position --- */
				mdlTMatrix_getIdentity (&tMatrix);
				mdlTMatrix_translate (&tMatrix, &tMatrix, (pInsert->ptInsert.x)*unitVal , (pInsert->ptInsert.y)*unitVal , (pInsert->ptInsert.z)*unitVal);
				mdlElmdscr_transform (cellDescrP, &tMatrix);

				/*mdlElmdscr_add(cellDP);

				if(cellDP)
				{
					mdlElmdscr_freeAll (&cellDP);
					cellDP = NULL;
				}*/
	
				//Explode cell.	
				if(0 == strcmpi(sExplode, "Yes"))
				{
					if(SUCCESS == mdlElmdscr_operation(cellDescrP, elmFunc, NULL, ELMD_ELEMENT))
					{}
					//MSElementDescr *elemDscrP = cellDP->h.firstElem;
					elemDscrP = cellDP->h.firstElem;

					while (elemDscrP)
						{
						elmFunc (elemDscrP);
						elemDscrP = elemDscrP->h.next;
						}
				}
				else
				{
					mdlElmdscr_add(cellDescrP);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
	@brief	

	@author	???

	@date	????.??.??

	@return	int
*/
int elmFunc(MSElementDescr *elemDscrP)
{
	MSElementDescr *newDscrPP;
	ULong filePos=0;
	char msg[256];
	int tt;
	//mdlDialog_openAlert("In func");

	mdlElmdscr_duplicateSingle(&newDscrPP,&elemDscrP);
	//mdlElmdscr_new(&newDscrPP,NULL,element);
	
	filePos = mdlElmdscr_add(newDscrPP);
	mdlElmdscr_display(newDscrPP,NULL,NORMALDRAW);

	sprintf(msg,"%d",mdlElement_getType(&elemDscrP->el));
	PrintError(msg);
	
	mdlElmdscr_freeAll(&newDscrPP);

	return SUCCESS;
}
