// DGNSolid.cpp: implementation of the CDGNSolid class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "DGNLib.h"
#include "DGNSolid.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNSolid::CDGNSolid()
{

}

/**	\brief
*/
CDGNSolid::~CDGNSolid()
{

}

/**	\brief	The CDGNSolid::GetNumOfElements function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int
*/
int CDGNSolid::GetNumOfElements(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	int nRet=0;

	if(pPrimitive){
		int nNumOfElements=0;
		for(SQSURFACE::PSURFELM_T pSurfaceElement=pPrimitive->body.pSurface->pElement;pSurfaceElement;pSurfaceElement=pSurfaceElement->next)
		{
			for(PSQPRIMITIVE ptr=pSurfaceElement->pBoundaryCurve;ptr;ptr=ptr->next,nRet++);
			for(PSQPRIMITIVE ptr=pSurfaceElement->pRuleCurve;ptr;ptr=ptr->next,nRet++);
		}
	}

	return nRet;
}

/**	\brief	The CDGNSolid::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNSolid::size(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=21; // not include dmrs linkage or user data linkage size.

	if(pPrimitive){
		for(SQSURFACE::PSURFELM_T ptrSurfaceElement=pPrimitive->body.pSurface->pElement;ptrSurfaceElement;ptrSurfaceElement=ptrSurfaceElement->next){
			for(PSQPRIMITIVE ptr=ptrSurfaceElement->pBoundaryCurve;ptr;ptr=ptr->next){
				if(ENM_LINE == ptr->hdr.id){
					nSize += 30;
				}else if(SQ_CIRCLE == ptr->hdr.id){
					nSize += 92;
				}else if(SQ_LOOP  == ptr->hdr.id){
					int nElements = ptr->body.pvtPoints->size();
					nSize += nElements*6 + 19;
				}else if(SQ_POINTS== ptr->hdr.id){
					int nElements = ptr->body.pvtPoints->size();
					nSize +=nElements*(6 + 8) + 17;
				}else if(SQ_SHAPE == ptr->hdr.id){
					int nPoints=SizeOfList(ptr->body.shape.list);

					nSize += 19 + nPoints*6;
				}
			}
			for(PSQPRIMITIVE ptr = ptrSurfaceElement->pRuleCurve;ptr;ptr=ptr->next)
			{
				if(ENM_LINE == ptr->hdr.id){
					nSize += 30;
				}else if(SQ_CIRCLE == ptr->hdr.id){
					nSize += 92;
				}else if(SQ_LOOP  == ptr->hdr.id){
					int nElements = ptr->body.pvtPoints->size();
					nSize += nElements*6 + 19;
				}else if(SQ_POINTS== ptr->hdr.id){
					int nElements = ptr->body.pvtPoints->size();
					nSize +=nElements*(6 + 8) + 17;
				}
			}
		}
	}

	return nSize;
}

/**	\brief	The CDGNSolid::StoreHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNSolid::StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is invalid");
	assert(pPrimitive && "pPrimitive is invalid");
	bool bRet = false;

	if(hDGN && pPrimitive){
		PDGNHDR  pDGNHdr = (PDGNHDR)hDGN->abyElem;

		//CalculateVolumeBox(pPrimitive->hdr.volume,pPrimitive);

		// set DGNFILE buf by 0.
		memset(hDGN->abyElem,0,65540);
		if(pPrimitive->hdr.pLayer) pDGNHdr->ElmHdr.level = atoi(pPrimitive->hdr.pLayer);
		pDGNHdr->ElmHdr.complex= 0;
		pDGNHdr->ElmHdr.deleted= 0;
		pDGNHdr->ElmHdr.xlow   = (long)pPrimitive->hdr.volume.x[LOW];
		pDGNHdr->ElmHdr.xhigh  = (long)pPrimitive->hdr.volume.x[HIGH];
		pDGNHdr->ElmHdr.ylow   = (long)pPrimitive->hdr.volume.y[LOW];
		pDGNHdr->ElmHdr.yhigh  = (long)pPrimitive->hdr.volume.y[HIGH];
		pDGNHdr->ElmHdr.zlow   = (long)pPrimitive->hdr.volume.z[LOW];
		pDGNHdr->ElmHdr.zhigh  = (long)pPrimitive->hdr.volume.z[HIGH];
		
		long* l = (long*)(hDGN->abyElem + 4);
		PDGNLONG pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-low
		pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-low
		pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-low
		pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-high
		pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-high
		pDGNLong = (PDGNLONG)l;
		pDGNLong->lo += 32768;
		SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-high
		
		pDGNHdr->DispHdr.props.b.n = 1;
		pDGNHdr->DispHdr.symb.b.color = pPrimitive->hdr.display.lcolor;
		pDGNHdr->DispHdr.symb.b.style = pPrimitive->hdr.display.ltype;
		pDGNHdr->DispHdr.symb.b.weight= pPrimitive->hdr.display.lweight;

		bRet = true;
	}

	return bRet;
}

/**	\brief	The CDGNSolid::Write function\n
	parse surface element.\n
	first boundary element appears and second boundary element follows.\n
	and rule elements follow boundary elements immediately.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNSolid::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if(hDGN)
	{
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;

		pDGNHDR->ElmHdr.type     = DGNT_3D_SOLID_HEADER;
		pDGNHDR->ElmHdr.words    = 16 + 3;
		pDGNHDR->DispHdr.props.b.s = 1;
		pDGNHDR->ElmHdr.level    = 10;

		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;

		/*
		pDGNHDR->DispHdr.props.b.a = 1;
		DGNDMRS* pDGNDMRS=(DGNDMRS*)(hDGN->abyElem + 42);
		pDGNDMRS->hdr._class = 0;
		pDGNDMRS->hdr.i      = 1;
		pDGNDMRS->UserID     = 21;
		pDGNDMRS->data.occur = 21;
		
		pDGNDMRS=(DGNDMRS*)(hDGN->abyElem + 50);
		pDGNDMRS->hdr.wtf    = 19;
		pDGNDMRS->hdr._class = 0;
		pDGNDMRS->hdr.u      = 1;
		pDGNDMRS->UserID     = 6;

		unsigned short linkage[18]={
			0x0000,0x0000,0x0000,0x0000,0x7999,
			0x5a82,0x8667,0xa57d,0x0001,0x0000,
			0xdac0,0x0000,0xdac0,0x0000,0x3e80,
			0x0000,0x2020,0x0015
		};
		for(int i=0;i < 18;i++){
			*((unsigned short*)(hDGN->abyElem + 54 + i*2)) = linkage[i];
		}
		pDGNHDR->ElmHdr.words += 24;
		*/

		*((unsigned short*)(hDGN->abyElem + 36)) = size(pPrimitive) - 19;
		int nNumOfElements= GetNumOfElements(pPrimitive);
		*((unsigned short*)(hDGN->abyElem + 38)) = nNumOfElements;
		hDGN->abyElem[40] = 0;
		hDGN->abyElem[41] = 0;

		if(NULL != pPrimitive->pLinkageData)
		{
			long lIndex=42;
			for(PDGNATTRDATA ptr=(PDGNATTRDATA)(pPrimitive->pLinkageData);ptr;ptr = ptr->next)
			{
				memcpy(hDGN->abyElem + lIndex,ptr,(ptr->hdr.wtf + 2)*2);
				pDGNHDR->ElmHdr.words += (ptr->hdr.wtf + 2);
				lIndex += (ptr->hdr.wtf + 2)*2;
			}
		}
		
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
				
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		bool bComponetOfComplexElem   = hDGN->bComponentOfComplexElem;
		hDGN->bComponentOfComplexElem = true;
		SQSURFACE::PSURFELM_T pSurfaceElement=NULL;
		for(pSurfaceElement = pPrimitive->body.pSurface->pElement;pSurfaceElement;pSurfaceElement = pSurfaceElement->next)
		{
			nWords += pDGNLib->m_DGNExport.WritePrimitive(hDGN,pSurfaceElement->pBoundaryCurve);
			nWords += pDGNLib->m_DGNExport.WritePrimitive(hDGN,pSurfaceElement->pRuleCurve);
		}
		hDGN->bComponentOfComplexElem = bComponetOfComplexElem;
	}

	return nWords;
}