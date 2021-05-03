// DGNSurface.cpp: implementation of the CDGNSurface class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "DGNLib.h"
#include "..\DGNParser.h"
#include "DGNSurface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CDGNSurface::CDGNSurface()
{

}

/**	\brief
*/
CDGNSurface::~CDGNSurface()
{

}

/**	\brief	The CDGNSurface::GetNumOfElements function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int
*/
int CDGNSurface::GetNumOfElements(PSQPRIMITIVE pPrimitive){
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

/**	\brief	The CDGNSurface::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long(size of elements in words)
*/
long CDGNSurface::size(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=21;//45;

	if(pPrimitive){
		for(SQSURFACE::PSURFELM_T ptrSurfaceElement=pPrimitive->body.pSurface->pElement;ptrSurfaceElement;ptrSurfaceElement=ptrSurfaceElement->next){
			for(PSQPRIMITIVE ptr=ptrSurfaceElement->pBoundaryCurve;ptr;ptr=ptr->next){
				if(ENM_LINE == ptr->hdr.id){
					nSize += 30;
				}else if(ENM_ARC == ptr->hdr.id){
					nSize += 50;
				}else if(ENM_ELLIPSE == ptr->hdr.id){
					nSize += 46;
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
				if(ENM_LINE == ptr->hdr.id)
				{
					nSize += 30;
				}else if(ENM_ARC == ptr->hdr.id){
					nSize += 50;
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

/**	\brief	The CDGNSurface::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T
*/
VOLUME_T CDGNSurface::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0,};

	if(pPrimitive && pPrimitive->body.pSurface){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		bool bFirst=true;
		for(SQSURFACE::PSURFELM_T pSurfElement = pPrimitive->body.pSurface->pElement;pSurfElement;pSurfElement = pSurfElement->next){
			for(PSQPRIMITIVE ptr=pSurfElement->pBoundaryCurve;ptr;ptr = ptr->next){
				CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);
				if(pElement){
					pElement->Volume(ptr);
					if(bFirst){
						varRet = ptr->hdr.volume;
						bFirst = false;
					}else{
						varRet.x[0] = MIN(varRet.x[0],ptr->hdr.volume.x[0]);
						varRet.x[1] = MAX(varRet.x[1],ptr->hdr.volume.x[1]);
						varRet.y[0] = MIN(varRet.y[0],ptr->hdr.volume.y[0]);
						varRet.y[1] = MAX(varRet.y[1],ptr->hdr.volume.y[1]);
						varRet.z[0] = MIN(varRet.z[0],ptr->hdr.volume.z[0]);
						varRet.z[1] = MAX(varRet.z[1],ptr->hdr.volume.z[1]);
					}
				}
			}
			
			for(PSQPRIMITIVE ptr=pSurfElement->pRuleCurve;ptr;ptr = ptr->next)
			{
				CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);
				if(pElement){
					pElement->Volume(ptr);
					if(bFirst){
						varRet = ptr->hdr.volume;
						bFirst = false;
					}else{
						varRet.x[0] = MIN(varRet.x[0],ptr->hdr.volume.x[0]);
						varRet.x[1] = MAX(varRet.x[1],ptr->hdr.volume.x[1]);
						varRet.y[0] = MIN(varRet.y[0],ptr->hdr.volume.y[0]);
						varRet.y[1] = MAX(varRet.y[1],ptr->hdr.volume.y[1]);
						varRet.z[0] = MIN(varRet.z[0],ptr->hdr.volume.z[0]);
						varRet.z[1] = MAX(varRet.z[1],ptr->hdr.volume.z[1]);
					}
				}
			}
		}
	}

	return varRet;
}

/**	\brief	The CDGNSurface::StoreHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNSurface::StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is invalid");
	assert(pPrimitive && "pPrimitive is invalid");
	bool bRet = false;

	if(hDGN && pPrimitive){
		//pPrimitive->hdr.volume = volume(pPrimitive);

		PDGNHDR  pDGNHdr = (PDGNHDR)hDGN->abyElem;
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

/**	\brief	The CDGNSurface::Write function\n
	parse surface element.\n
	first boundary elemnt appears and second boundary element follows.\n
	and rule elements follow boudary elements immediately.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNSurface::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
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
		pDGNHDR->ElmHdr.level    = 22;

		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;

		*((unsigned short*)(hDGN->abyElem + 36)) = size(pPrimitive) - 19;
		int nNumOfElements= GetNumOfElements(pPrimitive);
		*((unsigned short*)(hDGN->abyElem + 38)) = nNumOfElements;
		hDGN->abyElem[40] = (SURF_OF_REVOLUTION == pPrimitive->body.pSurface->nType) ? 8 : 0;
		hDGN->abyElem[41] = pPrimitive->body.pSurface->nBoundary - 1;
		if(hDGN->abyElem[41] < 0) hDGN->abyElem[41] = 0;

		if(NULL != pPrimitive->pLinkageData)
		{
			pDGNHDR->DispHdr.props.b.a = 1;
			long lIndex=42;
			for(PDGNATTRDATA ptr=(PDGNATTRDATA)(pPrimitive->pLinkageData);ptr;ptr = ptr->next)
			{
				if(ptr->hdr.u)
				{
					memcpy(hDGN->abyElem + lIndex,ptr,4);
					memcpy(hDGN->abyElem + lIndex + 4,ptr->data.pData,(ptr->hdr.wtf - 1)*2);
					pDGNHDR->ElmHdr.words += (ptr->hdr.wtf + 1);
					lIndex += (ptr->hdr.wtf + 1) * 2;
					bool odd=((ptr->hdr.wtf + 1) % 2) ? true : false;
					if(true == odd)
					{
						short dummy=0;
						memcpy(hDGN->abyElem + lIndex + 4 + (ptr->hdr.wtf - 1)*2,&dummy,sizeof(dummy));
						pDGNHDR->ElmHdr.words += 1;
						lIndex += 2;
					}
				}
				else
				{
					memcpy(hDGN->abyElem + lIndex,ptr,(4)*2);
					pDGNHDR->ElmHdr.words += 4;
					lIndex += (4)*2;
				}
			}
		}

		hDGN->WriteElement(hDGN->abyElem);
		nWords = pDGNHDR->ElmHdr.words + 2;
				
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		bool bComponetOfComplexElem   = hDGN->bComponentOfComplexElem;
		hDGN->bComponentOfComplexElem = true;
		SQSURFACE::PSURFELM_T pSurfaceElement=NULL;
		for(pSurfaceElement = pPrimitive->body.pSurface->pElement;pSurfaceElement;pSurfaceElement = pSurfaceElement->next)
		{
			//for(PSQPRIMITIVE ptr=pSurfaceElement->pBoundaryCurve;ptr;ptr = ptr->next){
				nWords += pDGNLib->m_DGNExport.WritePrimitive(hDGN,pSurfaceElement->pBoundaryCurve);
			//}
			//for(ptr=pSurfaceElement->pRuleCurve;ptr;ptr = ptr->next){
				nWords += pDGNLib->m_DGNExport.WritePrimitive(hDGN,pSurfaceElement->pRuleCurve);
			//}
		}
		hDGN->bComponentOfComplexElem = bComponetOfComplexElem;
	}

	return nWords;
}
