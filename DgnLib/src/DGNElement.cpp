// DGNElement.cpp: implementation of the CDGNElement class.
//
//////////////////////////////////////////////////////////////////////

#include <DGNLib.h>
#include "DGNElement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CDGNElement::CDGNElement()
{
	m_nType      = DGNT_NONE;
	m_pPrimitive = NULL;
}

/**	\brief
*/
CDGNElement::~CDGNElement()
{

}

/**	\brief	The CDGNElement::GetHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNElement::GetHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet = false;

	if(hDGN && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		if(pPrimitive->hdr.pLayer) pDGNHDR->ElmHdr.level  = atoi(pPrimitive->hdr.pLayer);
		pDGNHDR->ElmHdr.complex= 0;
		pDGNHDR->ElmHdr.deleted= 0;
		
		// write volume to hDGN
		pPrimitive->hdr.volume = Volume(pPrimitive);
		CDGNElement::WriteBounds(hDGN,pPrimitive->hdr.volume);
		//
		
		pDGNHDR->ElmHdr.complex        = (hDGN->bComponentOfComplexElem) ? 1 : 0;
		pDGNHDR->DispHdr.props.b.n     = 1;
		pDGNHDR->DispHdr.props.b._class= (RULE_CRV_T == pPrimitive->hdr.attr.type) ? 4 : 0;
		if(SQ_POINTS == pPrimitive->hdr.id){
			pDGNHDR->DispHdr.props.b.h = pPrimitive->hdr.attr.continuous ? 0 : 1;
		}else	pDGNHDR->DispHdr.props.b.h = pPrimitive->hdr.attr.h;
		pDGNHDR->DispHdr.symb.b.color = pPrimitive->hdr.display.lcolor;
		pDGNHDR->DispHdr.symb.b.style = pPrimitive->hdr.display.ltype;
		pDGNHDR->DispHdr.symb.b.weight= pPrimitive->hdr.display.lweight;
		
		bRet = true;
	}

	return bRet;
}

/**	\brief	The CDGNElement::WriteBounds function\n
	write element's volume boundary to hDGN.

	\param	hDGN	a parameter of type CDGNFile*
	\param	vol	a parameter of type const VOLUME_T

	\return	void

	
*/
void CDGNElement::WriteBounds(CDGNFile* hDGN,const VOLUME_T vol){
	assert(hDGN && "hDGN is NULL");

	if(hDGN){
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		pDGNHDR->ElmHdr.xlow   = (long)(vol.x[0]);
		pDGNHDR->ElmHdr.ylow   = (long)(vol.y[0]);
		pDGNHDR->ElmHdr.zlow   = (long)(vol.z[0]);
		pDGNHDR->ElmHdr.xhigh  = (long)(vol.x[1]);
		pDGNHDR->ElmHdr.yhigh  = (long)(vol.y[1]);
		pDGNHDR->ElmHdr.zhigh  = (long)(vol.z[1]);
		
		long* pl = (long*)(hDGN->abyElem + 4);
		char* pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// x-low
		
		pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// y-low
		
		pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// z-low
		
		pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// x-high
		
		pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// y-high
		
		pc = (char*)pl;
		pc[3] ^= 0x80;
		SWAP_LITTLE32_FOR_VAX(*pl);pl++;	// z-high

/*
		POINT_T ptMin={0,};
		ptMin.x = vol.x[0];ptMin.y = vol.y[0];ptMin.z = vol.z[0];
		CDGNLib::POINT2INT(hDGN->abyElem + 4,&ptMin,hDGN);
		
		POINT_T ptMax={0,};
		ptMax.x = vol.x[1];ptMax.y = vol.y[1];ptMax.z = vol.z[1];
		CDGNLib::POINT2INT(hDGN->abyElem + 16,&ptMax,hDGN);
		
		// convert from twos completement to "binary offset" format.
		hDGN->abyElem[5] ^= 0x80;
		hDGN->abyElem[9] ^= 0x80;
		hDGN->abyElem[13]^= 0x80;
		hDGN->abyElem[17]^= 0x80;
		hDGN->abyElem[21]^= 0x80;
		hDGN->abyElem[25]^= 0x80;
*/
	}
}

/**
 * Decode:
 *
 * @param pElmBuf 
 * @return bool 
 */
bool CDGNElement::Decode(CDGNFile* hFile)
{
	assert(hFile && "hFile is NULL");
	bool bRet=false;

	if(NULL != hFile){
		PDGNHDR pDGNHDR=(PDGNHDR)hFile->abyElem;
		memcpy(&m_header,pDGNHDR,36);
		bRet = true;
	}
	
	return bRet;
}

/**
 * Encode:
 *
 * @param pElmBuf 
 * @return bool 
 */
bool CDGNElement::Encode(CDGNFile* hFile)
{
	assert(hFile && "hFile is NULL");
	bool bRet=false;

	if(NULL != hFile){
		memcpy(hFile->abyElem,&m_header,36);
		bRet = true;
	}
	
	return bRet;
}

/**
 * GetDMRS:
 *
 * @param hFile 
 * @return PDGNATTRDATA 
 */
PDGNATTRDATA CDGNElement::GetDMRS(CDGNFile *hFile)
{
	assert(hFile && "hFile is NULL");
	PDGNATTRDATA pDMRS=NULL;

	if(hFile){
		int nAttIndex = hFile->abyElem[30] + hFile->abyElem[31] * 256;
		int nAttrBytes= hFile->nElemBytes - nAttIndex*2 - 32;
		int size = 0;
		int WTF,nWTFPos = 32 + nAttIndex*2;
		
		PDGNATTRDATA pHead=NULL,pTail= NULL,pData=NULL;
		for(int index = 0;nAttrBytes > 0;){
			WTF = hFile->abyElem[nWTFPos];
			if(pData = (PDGNATTRDATA)calloc(1,sizeof(DGNATTRDATA))){
				memcpy(&(pData->hdr),hFile->abyElem + nWTFPos,2);
				memcpy(&(pData->UserID),hFile->abyElem + nWTFPos + 2,2);
				if(pData->hdr.u && WTF){
					if(pData->data.pData = (char*)calloc(1,WTF*2 - 2 + 1)){
						memcpy(pData->data.pData,hFile->abyElem + nWTFPos + 4,WTF*2 - 2);
						pData->data.pData[WTF*2 - 2] = '\0';
						
						if(NULL == pHead){
							pHead = pTail = pData;
							pDMRS = pHead;
						}else{
							pTail ->next = pData;
							pTail = pTail->next;
						}
					}else	break;
					
					nAttrBytes -= 2*WTF + 2;
					nWTFPos    += 2*WTF + 2;
				}else{
					memcpy(&(pData->data.occur),hFile->abyElem + nWTFPos + 4,4);
					if(NULL == pHead){
						pHead = pTail = pData;
						pDMRS = pHead;
					}else{
						pTail ->next = pData;
						pTail = pTail->next;
					}
					
					nAttrBytes -= 4*2;
					nWTFPos    += 8;
				}
			}else	break;
		}
	}

	return pDMRS;
}

/**
 * AddUserData:
 *
 * @param pUserData 
 * @return void 
 */
void CDGNElement::AddUserData(PDGNATTRDATA pUserData)
{
	assert(pUserData && "pUserData is NULL");

	if(pUserData)
	{
		PDGNATTRDATA ptr = NULL;
		for(ptr=m_pUserData;ptr && ptr->next;ptr = ptr->next);
		if(NULL != ptr) ptr->next = pUserData;
	}
}

/**
	@brief	dgn element의 header 부분을 파싱하여 저장한다.

	@author	백흠경
*/
void CDGNElement::ParseHeader(PSQPRIMITIVE pPrimitive, CDGNFile *hDGN)
{
	assert(pPrimitive && "pPrimitive is NULL");
	assert(hDGN       && "hDGN is NULL");
	
	if(pPrimitive && hDGN)
	{
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		
		PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
		
		if(pPrimitive->hdr.pLayer=(char*)calloc(1,2)) pPrimitive->hdr.pLayer[0] = pDGNHDR->ElmHdr.level;
		pPrimitive->hdr.attr.d   = hDGN->dimension;
		pPrimitive->hdr.attr.f   = 1;
		pPrimitive->hdr.attr.u   = pDGNHDR->DispHdr.props.b.a;
		pPrimitive->hdr.attr.h   = pDGNHDR->DispHdr.props.b.h;
		pPrimitive->hdr.attr.type= ((4 == pDGNHDR->DispHdr.props.b._class) || (6 == pDGNHDR->DispHdr.props.b._class)) ? RULE_CRV_T : PRIMARY_CRV_T;
		pPrimitive->hdr.handle   = hDGN->next_element_id - 1;
		
		if(hDGN->nElemBytes >= 36)
		{
			//pPrimitive->hdr.group           = pDGNHDR->DispHdr.gphgrp;
			pPrimitive->hdr.display.ltype     = pDGNHDR->DispHdr.symb.b.style;
			pPrimitive->hdr.display.lweight   = pDGNHDR->DispHdr.symb.b.weight;
			pPrimitive->hdr.display.lcolor = pDGNHDR->DispHdr.symb.b.color;
			pPrimitive->hdr.display.rgb[0] = hDGN->color_table[pDGNHDR->DispHdr.symb.b.color + 1][0];
			pPrimitive->hdr.display.rgb[1] = hDGN->color_table[pDGNHDR->DispHdr.symb.b.color + 1][1];
			pPrimitive->hdr.display.rgb[2] = hDGN->color_table[pDGNHDR->DispHdr.symb.b.color + 1][2];
		}
	}
}

/**
 * ParseDMRS:
 *
 * @param pPrimitive 
 * @param hDGN 
 * @return void 
 */
void CDGNElement::ParseDMRS(PSQPRIMITIVE pPrimitive, CDGNFile *hDGN)
{
	assert(pPrimitive && "pPrimitive is NULL");
	assert(hDGN       && "hDGN is NULL");
	
	if(pPrimitive && hDGN){
		int nAttIndex = hDGN->abyElem[30] + hDGN->abyElem[31] * 256;
		int nAttrBytes= hDGN->nElemBytes - nAttIndex*2 - 32;
		int size = 0;
		int WTF,nWTFPos = 32 + nAttIndex*2;
		PDGNATTRDATA pAttrData  = NULL;
		
		for(int index = 0;nAttrBytes > 0;)
		{
			WTF   = hDGN->abyElem[nWTFPos];
			if(pAttrData = (PDGNATTRDATA)CDGNLib::AllocMemory(sizeof(DGNATTRDATA)))
			{
				memcpy(&(pAttrData->hdr),hDGN->abyElem + nWTFPos,2);
				memcpy(&(pAttrData->UserID),hDGN->abyElem + nWTFPos + 2,2);
				if(pAttrData->hdr.u && WTF)
				{
					if(pAttrData->data.pData = (char*)CDGNLib::AllocMemory(WTF*2 - 2 + 1))
					{
						memcpy(pAttrData->data.pData,hDGN->abyElem + nWTFPos + 4,WTF*2 - 2);
						pAttrData->data.pData[WTF*2 - 2] = '\0';
												
						pPrimitive->pLinkageData = ListPush((PDGNATTRDATA)pPrimitive->pLinkageData,(PDGNATTRDATA)pAttrData);
					}else	break;
					
					nAttrBytes -= 2*WTF + 2;
					nWTFPos    += 2*WTF + 2;
				}
				else
				{
					memcpy(&(pAttrData->data.occur),hDGN->abyElem + nWTFPos + 4,4);
					pPrimitive->pLinkageData = ListPush((PDGNATTRDATA)pPrimitive->pLinkageData,pAttrData);
					
					nAttrBytes -= 4*2;
					nWTFPos    += 8;
				}
			}else	break;
		}
	}
}
