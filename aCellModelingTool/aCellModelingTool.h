#include <mselems.h>

//! cell type
#define	POWER		0
#define	GROUNDING	1
#define	LIGHTING	2
#define	FIRE_ALARM	3
#define	FIRE_ALARM	3
#define	COMMUNICATION	4
#define	ETC		5

#define  RSCID_DataDef_Link  		2
#define SIGNATUREID_USERDATAPROJECT	14

#define	INFO_BUF_SIZ	24
typedef struct tagElectlinkinfo
{
	unsigned char  cType[INFO_BUF_SIZ];
	unsigned char  cName[INFO_BUF_SIZ];
	unsigned char  cDesc1[INFO_BUF_SIZ];
	unsigned char  cDesc2[INFO_BUF_SIZ];
	unsigned char  cDesc3[INFO_BUF_SIZ];

}ElectLinkInfo;

typedef struct electlinkdata
{
    LinkageHeader	linkHdr;
    ElectLinkInfo       electlinkageInfo;
} ElectLinkData;