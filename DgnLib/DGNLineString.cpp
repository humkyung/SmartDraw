// DGNLineString.cpp: implementation of the CDGNLineString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <DGNLib.h>
#include "DGNLineString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDGNLineString::CDGNLineString()
{

}

CDGNLineString::~CDGNLineString()
{

}

/**
	@brief	Parse:

	@author	humkyung

	@param	ppPrimitiveList 
	@param	hDGN 
	
	@return long 
 */
long CDGNLineString::Parse(PSQPRIMITIVE *ppPrimitiveList, CDGNFile *hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;
	
	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		nWords = (int)(hDGN->nElemBytes*0.5);
		if(pPrimitive->body.pvtPoints = new vector<POINT_T>)
		{
			CDGNElement::ParseHeader(pPrimitive,hDGN);
			pPrimitive->hdr.id     = SQ_POINTS;
			pPrimitive->hdr.attr.d = hDGN->dimension;
			pPrimitive->hdr.attr.f = 1;
			pPrimitive->hdr.attr.continuous = 1;
			
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if(DGNT_SHAPE == pDGNHDR->ElmHdr.type) strcpy(pPrimitive->hdr.szDesc,"LOOP");
			pPrimitive->hdr.attr.continuous = (pDGNHDR->DispHdr.props.b.h) ? 0 : 1;
			if(2 == hDGN->dimension)
			{
				pPrimitive->hdr.volume.x[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 38);
				pPrimitive->hdr.volume.x[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.x[HIGH] = pPrimitive->hdr.volume.x[LOW];
				pPrimitive->hdr.volume.y[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 42);
				pPrimitive->hdr.volume.y[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.y[HIGH] = pPrimitive->hdr.volume.y[LOW];
				unsigned short nVertices = *((unsigned short*)(hDGN->abyElem + 36));
				for(int i = 0;i < nVertices;i++)
				{//pLineString->numverts;i++){
					POINT_T pt={0};
					
					pt.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*8 + 38);
					pt.x*= hDGN->scale;
					pt.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*8 + 42);
					pt.y*= hDGN->scale;
					pt.z = 0.;
					
					if(pPrimitive->hdr.volume.x[LOW] > pt.x) pPrimitive->hdr.volume.x[LOW] = pt.x;
					if(pPrimitive->hdr.volume.x[HIGH]< pt.x) pPrimitive->hdr.volume.x[HIGH]= pt.x;
					if(pPrimitive->hdr.volume.y[LOW] > pt.y) pPrimitive->hdr.volume.y[LOW] = pt.y;
					if(pPrimitive->hdr.volume.y[HIGH]< pt.y) pPrimitive->hdr.volume.y[HIGH]= pt.y;
					
					pPrimitive->body.pvtPoints->push_back(pt);
				}
				
				PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
				if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
			}
			else if(3 == hDGN->dimension)
			{
				pPrimitive->hdr.volume.x[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 38);
				pPrimitive->hdr.volume.x[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.x[HIGH] = pPrimitive->hdr.volume.x[LOW];
				pPrimitive->hdr.volume.y[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 42);
				pPrimitive->hdr.volume.y[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.y[HIGH] = pPrimitive->hdr.volume.y[LOW];
				pPrimitive->hdr.volume.z[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 46);
				pPrimitive->hdr.volume.z[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.z[HIGH]= pPrimitive->hdr.volume.z[LOW];
				pPrimitive->hdr.attr.reversed = (pDGNHDR->DispHdr.props.b.h) ? 0 : 1;
				unsigned short nVertices = *((unsigned short*)(hDGN->abyElem + 36));
				for(int i = 0;i < nVertices;i++)
				{
					POINT_T pt={0};
					
					pt.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*12 + 38);
					pt.x*= hDGN->scale;
					pt.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*12 + 42);
					pt.y*= hDGN->scale;
					pt.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*12 + 46);
					pt.z*= hDGN->scale;
					
					if(pPrimitive->hdr.volume.x[LOW] > pt.x) pPrimitive->hdr.volume.x[LOW] = pt.x;
					if(pPrimitive->hdr.volume.x[HIGH]< pt.x) pPrimitive->hdr.volume.x[HIGH]= pt.x;
					if(pPrimitive->hdr.volume.y[LOW] > pt.y) pPrimitive->hdr.volume.y[LOW] = pt.y;
					if(pPrimitive->hdr.volume.y[HIGH]< pt.y) pPrimitive->hdr.volume.y[HIGH]= pt.y;
					if(pPrimitive->hdr.volume.z[LOW] > pt.z) pPrimitive->hdr.volume.z[LOW] = pt.z;
					if(pPrimitive->hdr.volume.z[HIGH]< pt.z) pPrimitive->hdr.volume.z[HIGH]= pt.z;
					
					pPrimitive->body.pvtPoints->push_back(pt);
				}
				
				PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
				if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
			}
			(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
		}
	}
	
	return nWords;
}
