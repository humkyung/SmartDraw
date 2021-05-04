/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:C:\Bentley\Program\MicroStation\mdl\examples\w2Dgn\w2Dgn.h
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
#include <mselems.h>
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
#define PUSHBTNID_WRITE         200000002
#define PUSHBTNID_SELECT        200000001
#define DIALOGID_MAIN           1

#define HOOKID_MAIN		1
#define HOOKITEMID_SELECT	2
#define HOOKITEMID_WRITE	3

#define MACROVAL_0              0
#define TEXTID_FILEPATH		200000001

#define MACROVAL_4              4

#define  RSCID_DataDef_Link	4023
#define  RSCID_PlantSpaceDataDef_Link	4024
#define  SIGNATUREID_TEMP	14

typedef struct _fileinfo
{
	char          filename[128];
} FileInfo;

#if !defined (resource)
typedef struct linkinfo
{
	    unsigned char   tag_Name1[25];
        unsigned char   tagValue1[25];
        unsigned char   tag_Name2[25];
        unsigned char   tagValue2[25];
        unsigned char   tag_Name3[25];
        unsigned char   tagValue3[25];
    
} LinkInfo;

typedef struct linkdata
{
    LinkageHeader  linkHdr;
    LinkInfo       linkageInfo;
} LinkData;

typedef struct tagPlantSpaceDataInfo
{
	unsigned char   szBuf[20];
} PlantSpaceDataInfo;

typedef struct tagPlantSpaceData
{
    LinkageHeader      linkHdr;
    PlantSpaceDataInfo linkageInfo;
}PlantSpaceData;
#endif