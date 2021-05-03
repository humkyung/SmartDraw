#include <assert.h>
#include <string.h>
#include <DGNAttrData.h>

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
short DLL_EXPORT GetAttrDataType(PDGNATTRDATA pAttrData)
{
	assert(pAttrData && "pAttrData is NULL");

	if(pAttrData)
	{
		static short data;
		
		memcpy(&data,&(pAttrData->hdr),sizeof(short));
		short* pData = &data;

		if(DGNLT_DMRS          == (*pData))	return DGNLT_DMRS;
		else if(DGNLT_INFORMIX == (*pData))	return DGNLT_INFORMIX;
		else if(DGNLT_ODBC     == (*pData))	return DGNLT_ODBC;
		else if(DGNLT_ORACLE   == (*pData))	return DGNLT_ORACLE;
		else if(DGNLT_RIS      == (*pData))	return DGNLT_RIS;
		else if(DGNLT_SYBASE   == (*pData))	return DGNLT_SYBASE;
		else if(DGNLT_XBASE    == (*pData))	return DGNLT_XBASE;
	}

	return 0xffff;
}