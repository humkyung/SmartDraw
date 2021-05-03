#ifndef	__DGNATTRDATA_H__
#define	__DGNATTRDATA_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT	__declspec(dllexport)
#endif

//	Attribute Data Types
enum
{
	DGNLT_DMRS	=0x0000,
	DGNLT_INFORMIX  =0x3848,
	DGNLT_ODBC	=0x5e62,
	DGNLT_ORACLE	=0x6091,
	DGNLT_RIS	=0x1007,
	DGNLT_SYBASE	=0x4f58,
	DGNLT_XBASE	=0x1971,
};

//	Header of Attribute Data
typedef DLL_EXPORT struct tagDGNATTRDATAHDR
{
	unsigned short  wtf:8;
	unsigned short  _class:4;
	unsigned short 	u:1;
	unsigned short 	m:1;
	unsigned short 	r:1;
	unsigned short 	i:1;
}DGNATTRDATAHDR,* PDGNATTRDATAHDR;

//	Attribute Data
typedef DLL_EXPORT struct tagDGNATTRDATA{
	DGNATTRDATAHDR	hdr;
	unsigned short  UserID; // or entity
	union{
		char*	pData;
		long	occur;
	}data;

	struct tagDGNATTRDATA* next;
}DGNATTRDATA,* PDGNATTRDATA;

extern short DLL_EXPORT GetAttrDataType(PDGNATTRDATA pAttrData);

#endif
