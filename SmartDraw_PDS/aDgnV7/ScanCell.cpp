#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "aDgnV7.h"
#include <mselemen.fdf>
#include <msscan.fdf>

extern int loadCellLibraryIfNeeded(char *libToLoad);
/*----------------------------------------------------------------------+
|                                                                       |
|	name	GetCellNameFromCellLib					|
|									|
|	author	HumKyung.Baek						|
|                                                                       |
+----------------------------------------------------------------------*/
Public int GetCellNameFromCellLib(char* pOutputTextFilePath , char* pstrCellLib)
{
	ULong        elemAddr[250], eofPos, filePos, actualPos, readPos;
    	int          scanWords, status, i,j=0, numAddr, fileNum=CELL_LIB, offset;
    	char         cellName[10], cellDescription[30], msg[132];
    	Scanlist     scanList;
    	MSElement    cellElm;        /* cell element descriptor */
	FILE* fp = NULL;
	
	if(fp = mdlTextFile_open(pOutputTextFilePath , TEXTFILE_WRITE))
	{
		loadCellLibraryIfNeeded(pstrCellLib);

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
					memset(cellName , 0 , sizeof(char)*10);
					mdlCnv_fromR50ToAscii ( 6, (short *) &cellElm.cell_lib_hdr.name,cellName);
					//! trim right
					for(j = strlen(cellName) - 1;(j >= 0) && ((' ' == cellName[j]) || ('\r' == cellName[j]) || ('\n' == cellName[j]));j--);
					cellName[j + 1] = '\0';
					
					memset(cellDescription , 0 , sizeof(char)*30);
					mdlCnv_fromR50ToAscii (27, cellElm.cell_lib_hdr.descrip,cellDescription);
					//! trim right
					for(j = strlen(cellDescription) - 1;(j >= 0) && ((' ' == cellDescription[j]) || ('\r' == cellDescription[j]) || ('\n' == cellDescription[j]));j--);
					cellDescription[j + 1] = '\0';

					//! write cell name and description to output file.
					sprintf(msg , "%s^%s" , cellName , cellDescription);
					mdlTextFile_putString(msg , fp , TEXTFILE_DEFAULT);
				} 
			}
		}while(status == BUFF_FULL);

    		mdlTextFile_close(fp);

		return SUCCESS;
	}

	return FALSE;
} 
