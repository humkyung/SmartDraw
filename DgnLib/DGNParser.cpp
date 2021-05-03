// DGNParser.cpp: implementation of the CDGNParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <math.h>
#include <math\ITKMath.h>
#include <graphics\ITKGeo.h>
#include <graphics\geometry.h>
#include <DGNLib.h>
#include "DGNScanner.h"
#include "DGNParser.h"

#include <DGNLine.h>
#include "DGNLineString.h"
#include <DGNPointString.h>
#include <DGNEllipse.h>
#include <DGNArc.h>
#include <DGNCone.h>
#include "DGNText.h"

static FILE* fp=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNParser::CDGNParser()
{
	m_pDGNScanner = NULL;
}

/**	\brief
*/
CDGNParser::~CDGNParser()
{
	try
	{
		if(m_pDGNScanner) delete m_pDGNScanner;
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CDGNParser::Parse function

	@author	HumKyung

	@date	????.??.??

	@param	hDGN	handle of dgn file

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CDGNParser::Parse(CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	PSQPRIMITIVE pRet=NULL;

	if(hDGN && pDGNLib)
	{
		if(NULL == m_pDGNScanner) m_pDGNScanner = new CDGNScanner;
		if(m_pDGNScanner)
		{
			//PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			for(;hDGN->ReadElement();)
			{
				//if(!pDGNHDR->ElmHdr.deleted){
				//GetHeader(pPrimitive,hDGN);
				PSQPRIMITIVE* ppPrimitiveList = (PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE));
				if(ppPrimitiveList)
				{
					*ppPrimitiveList = NULL;
					ParseElement(ppPrimitiveList,hDGN);
					if(*ppPrimitiveList && (*ppPrimitiveList)->hdr.attr.f)
					{
						if((ENM_GROUP == (*ppPrimitiveList)->hdr.id) && IsElbow(*ppPrimitiveList))
						{
							const static char sz[6]="ELBOW";
							PSQPRIMITIVE pGroup=(*ppPrimitiveList)->body.pPrimitive;
							pGroup = pGroup->next;
							strcpy(pGroup->hdr.szDesc,sz);
						}
						else if((ENM_GROUP == (*ppPrimitiveList)->hdr.id) && IsEquipment(*ppPrimitiveList))
						{
							const static char sz[5]="EQUP";
							strcpy((*ppPrimitiveList)->hdr.szDesc,sz);
						}
						if((*ppPrimitiveList)->hdr.id) pRet = ListPush(pRet,*ppPrimitiveList);
						
					}else if(*ppPrimitiveList)
						pRet = ListPush(pRet,*ppPrimitiveList);
					//else	free(ppPrimitiveList);
				}
			}
		}
		//if(NULL != fp) fclose(fp);
	}

	return pRet;
}

/**	\brief	The CDGNParser::Parse function

	\param	hDGN	handle of dgn file
	\param	pVolume	a parameter of type PSQVOLUME

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CDGNParser::Parse(CDGNFile* hDGN,PSQVOLUME pVolume)
{
	assert(hDGN    && "hDGN is NULL");
	assert(pVolume && "pVolume is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	PSQPRIMITIVE pRet=NULL;

	if(hDGN && pDGNLib)
	{
		if(NULL == m_pDGNScanner) m_pDGNScanner = new CDGNScanner;
		if(m_pDGNScanner)
		{
			for(;hDGN->ReadElement();)
			{
				//if(!pDGNHDR->ElmHdr.deleted){
				PSQPRIMITIVE* ppPrimitiveList = (PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE));
				if(ppPrimitiveList)
				{
					*ppPrimitiveList = NULL;
					ParseElement(ppPrimitiveList,hDGN);
					if(*ppPrimitiveList)
					{
						if((*ppPrimitiveList)->hdr.attr.f && !IsOuterVolume(*pVolume,(*ppPrimitiveList)->hdr.volume))
						{
							if((ENM_GROUP == (*ppPrimitiveList)->hdr.id) && IsElbow(*ppPrimitiveList))
							{
								const static char sz[6]="ELBOW";
								PSQPRIMITIVE pGroup=(*ppPrimitiveList)->body.pPrimitive;
								pGroup = pGroup->next;
								strcpy(pGroup->hdr.szDesc,sz);
							}
							else if((ENM_GROUP == (*ppPrimitiveList)->hdr.id) && IsEquipment(*ppPrimitiveList))
							{
								const static char sz[5]="EQUP";
								strcpy((*ppPrimitiveList)->hdr.szDesc,sz);
							}
							if((*ppPrimitiveList)->hdr.id) pRet = ListPush(pRet,*ppPrimitiveList);
						}
						else if(62 == (*ppPrimitiveList)->hdr.id)
						{
							pRet = ListPush(pRet,*ppPrimitiveList);
						}
						else if((*ppPrimitiveList)->hdr.attr.f && 
							((SQ_TEXT == (*ppPrimitiveList)->hdr.id) || (ENM_ELLIPSE == (*ppPrimitiveList)->hdr.id) || (ENM_LINE == (*ppPrimitiveList)->hdr.id)))
						{
							pRet = ListPush(pRet,*ppPrimitiveList);
						}
					}//else	free(ppPrimitiveList);
				}
				//}
			}
		}
		//if(NULL != fp) fclose(fp);
	}

	return pRet;
}

/**	\brief	The CDGNParser::GetNumOfElements function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int(number of elements)	
*/
int CDGNParser::GetNumOfElements(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	int nRet=1;

	if(pPrimitive)
	{
		switch(pPrimitive->hdr.id)
		{
			case SQ_LOOP:
			{
				for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr = ptr->next,nRet++);
			}
			break;
			case ENM_GROUP:
				nRet += GetNumOfElements(pPrimitive->body.pPrimitive);
				break;
		}
	}

	return nRet;
}

/**	
	@brief	read a primitive from DGN buffer, and store to pPrimitive

	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long(element sizes in words)
*/
long CDGNParser::ParseElement(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PDGNHDR pDGNHdr = (PDGNHDR)hDGN->abyElem;
	if(hDGN->remainder > 0) hDGN->remainder -= (int)(hDGN->nElemBytes*0.5);
		
	switch(pDGNHdr->ElmHdr.type)
	{
		case DGNT_TCB:
			nWords += ParseTCB(hDGN);
			break;
		case DGNT_LINE:
			nWords += CDGNLine::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_LINE_STRING:case DGNT_SHAPE:case DGNT_CURVE:
			nWords += CDGNLineString::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_POINT_STRING:
			nWords += CDGNPointString::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_ARC:
			nWords += CDGNArc::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_ELLIPSE:
			nWords += CDGNEllipse::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_TEXT:
			nWords += CDGNText::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_CONE:
			nWords += CDGNCone::Parse(ppPrimitiveList,hDGN);
			break;
		case DGNT_COMPLEX_SHAPE_HEADER:case DGNT_COMPLEX_CHAIN_HEADER:
			nWords += ParseComplexShapeOrChain(ppPrimitiveList,hDGN);
			break;
		case DGNT_3D_SOLID_HEADER:
			//nWords += ParseSolid(ppPrimitiveList,hDGN);
			nWords += ParseSurface(ppPrimitiveList,hDGN);
			break;
		case DGNT_SURFACE_HEADER:
			nWords += ParseSurface(ppPrimitiveList,hDGN);
			break;
		case DGNT_B_SPLINE_SURFACE_HEADER:
			nWords += ParseBSplineSurfaceHeader(ppPrimitiveList,hDGN);
			break;
		case DGNT_DIMENSION:
			nWords += ParseDimension(ppPrimitiveList,hDGN);
			break;
		case DGNT_CELL_HEADER:
			nWords += ParseCellHeader(ppPrimitiveList,hDGN);
			break;
		case DGNT_RASTER_HEADER:
			nWords += ParseRasterHeader(ppPrimitiveList,hDGN);
			break;
		case DGNT_RASTER_DATA:
			nWords += (int)(hDGN->nElemBytes*0.5);
			break;
		default:
			if(	((DGNT_GROUP_DATA == pDGNHdr->ElmHdr.type) && (9 == pDGNHdr->ElmHdr.level)) ||
				((DGNT_APPLICATION_ELEM == pDGNHdr->ElmHdr.type) && (5 == pDGNHdr->ElmHdr.level)))
			{
				nWords += ParseGroupData(ppPrimitiveList,hDGN);
			}else if((DGNT_GROUP_DATA == pDGNHdr->ElmHdr.type) && (1 == pDGNHdr->ElmHdr.level)){
				nWords += ParseColorTable(hDGN);
			/*}
			else if(62 == pDGNHdr->ElmHdr.type)
			{
				FILE* fp=fopen("d:\\62.txt","wb");
				for(int i=0;i < 8;i++){
					short* s=(short*)(hDGN->abyElem + i*2);
					fprintf(fp,"%x ",(short)(*s));
				}
				fprintf(fp,"\r\n");
				for(i=8;i < 16;i++){
					short* s=(short*)(hDGN->abyElem + i*2);
					fprintf(fp,"%x ",(short)(*s));
				}
				fprintf(fp,"\r\n");
				for(i=16;i < 18;i++){
					short* s=(short*)(hDGN->abyElem + i*2);
					fprintf(fp,"%x ",(short)(*s));
				}
				fclose(fp);
			*/
			}else{
				PSQPRIMITIVE pPrimitive=NULL;
				CDGNLib* pDGNLib=CDGNLib::GetInstance();
				if(pPrimitive = pDGNLib->NewPrimitive()){
					pPrimitive->hdr.id     = pDGNHdr->ElmHdr.type;
					pPrimitive->hdr.attr.d = hDGN->dimension;
					pPrimitive->hdr.attr.f = 0;
					
					nWords += (int)(hDGN->nElemBytes*0.5);
					int nBytes = hDGN->nElemBytes - 36;
					if(nBytes > 0)
					{
						pPrimitive->body.pData = (char*)CDGNLib::AllocMemory(nBytes);
						memcpy(pPrimitive->body.pData,hDGN->abyElem + 36,nBytes);
						//pPrimitive->hdr.esize = nBytes;
					}
					
					(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
				}
				/*
				if(62 == pPrimitive->hdr.id){
					FILE* fp=fopen("d:\\temp\\62.txt","wb");
					fwrite(hDGN->abyElem,1,hDGN->nElemBytes,fp);
					fclose(fp);
				}
				*/
			}//else	nWords += (int)(hDGN->nElemBytes*0.5);
			break;
	}

	return nWords;
}

/**	
	@brief	The CDGNParser::ParseCellHeader function

	@author	HumKyung.BAEK

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long	
*/
long CDGNParser::ParseCellHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is invalid");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = ENM_GROUP;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);

		long nElementWords=*((unsigned short*)(hDGN->abyElem + 36));
		nElementWords = nElementWords - (int)((hDGN->nElemBytes*0.5 - 19));
		nWords += nElementWords;
		if(2 == hDGN->dimension)
		{
			pPrimitive->hdr.volume.x[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 52)*hDGN->scale;
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->hdr.volume.x[LOW];
			
			pPrimitive->hdr.volume.y[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 56)*hDGN->scale;
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->hdr.volume.y[LOW];
			
			if(pPrimitive->hdr.volume.x[LOW] > SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60)*hDGN->scale) pPrimitive->hdr.volume.x[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60)*hDGN->scale;
			if(pPrimitive->hdr.volume.x[HIGH]< SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60)*hDGN->scale) pPrimitive->hdr.volume.x[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60)*hDGN->scale;
			if(pPrimitive->hdr.volume.y[LOW] > SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale) pPrimitive->hdr.volume.y[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale;
			if(pPrimitive->hdr.volume.y[HIGH]< SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale) pPrimitive->hdr.volume.y[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale;

			pPrimitive->hdr.volume.x[LOW] += SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 84);
			pPrimitive->hdr.volume.x[HIGH]+= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 84);
			pPrimitive->hdr.volume.y[LOW] += SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 88);
			pPrimitive->hdr.volume.y[HIGH]+= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 88);

			PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
		}
		else if(3 == hDGN->dimension)
		{
			pPrimitive->hdr.volume.x[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 52);//*hDGN->scale;
			pPrimitive->hdr.volume.y[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 56);
			pPrimitive->hdr.volume.z[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60);//*hDGN->scale;
			pPrimitive->hdr.volume.x[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64);
			pPrimitive->hdr.volume.y[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 68);//*hDGN->scale;
			pPrimitive->hdr.volume.z[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 72);//pPrimitive->hdr.volume.z[LOW];

			pPrimitive->hdr.ptOrigin.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 112);
			pPrimitive->hdr.ptOrigin.x *= hDGN->scale;

			pPrimitive->hdr.ptOrigin.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 116);
			pPrimitive->hdr.ptOrigin.y *= hDGN->scale;
			
			pPrimitive->hdr.ptOrigin.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 120);
			pPrimitive->hdr.ptOrigin.z *= hDGN->scale;			
			/*
			if(pPrimitive->hdr.volume.x[LOW] > DGN_INT32(hDGN->abyElem + 64)*hDGN->scale) pPrimitive->hdr.volume.x[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale;
			if(pPrimitive->hdr.volume.x[HIGH]< DGN_INT32(hDGN->abyElem + 64)*hDGN->scale) pPrimitive->hdr.volume.x[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64)*hDGN->scale;
			if(pPrimitive->hdr.volume.y[LOW] > DGN_INT32(hDGN->abyElem + 68)*hDGN->scale) pPrimitive->hdr.volume.y[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 68)*hDGN->scale;
			if(pPrimitive->hdr.volume.y[HIGH]< DGN_INT32(hDGN->abyElem + 68)*hDGN->scale) pPrimitive->hdr.volume.y[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 68)*hDGN->scale;
			if(pPrimitive->hdr.volume.z[LOW] > DGN_INT32(hDGN->abyElem + 72)*hDGN->scale) pPrimitive->hdr.volume.z[LOW] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 72)*hDGN->scale;
			if(pPrimitive->hdr.volume.z[HIGH]< DGN_INT32(hDGN->abyElem + 72)*hDGN->scale) pPrimitive->hdr.volume.z[HIGH]= SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 72)*hDGN->scale;
			*/

			PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
		}

		for(;nElementWords > 0;)
		{
			if(m_pDGNScanner->ReadElement(hDGN))
			{
				//PSQPRIMITIVE pElement = pDGNLib->NewPrimitive();
				//if(pElement){
				//	GetHeader(pElement,hDGN);
				PSQPRIMITIVE* ppElement=(PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE));
				if(ppElement)
				{
					*ppElement     = NULL;
					nElementWords -= ParseElement(ppElement,hDGN);
					if(*ppElement) pPrimitive->body.pPrimitive = ListPush(pPrimitive->body.pPrimitive,*ppElement);
				}
				//}else	break;
			}else	break;
		}
		
		if(IsPipe(pPrimitive))				strcpy(pPrimitive->hdr.szDesc, _T("PIPE"));
		else if(IsElbow(pPrimitive))		strcpy(pPrimitive->hdr.szDesc, _T("ELBOW"));
		else if(IsTee(pPrimitive))			strcpy(pPrimitive->hdr.szDesc, _T("TEE"));
		//else if(IsInstrument(pPrimitive))	strcpy(pPrimitive->hdr.szDesc, _T("INSTRUMENT"));
		else if(IsNozzle(pPrimitive))
		{
			/*
			if(NULL == fp) fp = fopen("c:\\nozzle.txt","wb");
			if(fp)
			{
				PDGNATTRDATA pAttrData=NULL;
				for(pAttrData = (PDGNATTRDATA)(pPrimitive->pLinkageData);pAttrData;pAttrData = pAttrData->next)
				{
					if(pAttrData->hdr.u)
					{
						fprintf(fp,"[");
						fwrite(pAttrData->data.pData,1,pAttrData->hdr.wtf,fp);
						fprintf(fp,"]");
						fprintf(fp,"\r\n");
					}
				}
			}
			*/
		
		}
		else if(IsCable(pPrimitive))
		{
			/*
			if(NULL == fp) fp = fopen("c:\\cable.txt","wb");
			if(fp)
			{
				PDGNATTRDATA pAttrData=NULL;
				for(pAttrData = (PDGNATTRDATA)(pPrimitive->pLinkageData);pAttrData;pAttrData = pAttrData->next)
				{
					if(pAttrData->hdr.u)
					{
						fprintf(fp,"[");
						fwrite(pAttrData->data.pData,1,pAttrData->hdr.wtf,fp);
						fprintf(fp,"]");
						fprintf(fp,"\r\n");
					}
				}
			}
			*/
		}
		
		if(pPrimitive->body.pPrimitive)
		{
			pPrimitive->hdr.volume = pPrimitive->body.pPrimitive->hdr.volume;
			for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr = ptr->next)
			{
				if(pPrimitive->hdr.volume.x[0] > ptr->hdr.volume.x[0]) pPrimitive->hdr.volume.x[0] = ptr->hdr.volume.x[0];
				if(pPrimitive->hdr.volume.x[1] < ptr->hdr.volume.x[1]) pPrimitive->hdr.volume.x[1] = ptr->hdr.volume.x[1];
				if(pPrimitive->hdr.volume.y[0] > ptr->hdr.volume.y[0]) pPrimitive->hdr.volume.y[0] = ptr->hdr.volume.y[0];
				if(pPrimitive->hdr.volume.y[1] < ptr->hdr.volume.y[1]) pPrimitive->hdr.volume.y[1] = ptr->hdr.volume.y[1];
				if(pPrimitive->hdr.volume.z[0] > ptr->hdr.volume.z[0]) pPrimitive->hdr.volume.z[0] = ptr->hdr.volume.z[0];
				if(pPrimitive->hdr.volume.z[1] < ptr->hdr.volume.z[1]) pPrimitive->hdr.volume.z[1] = ptr->hdr.volume.z[1];
			}
		}
		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	\brief	The CDGNParser::ParseCore function

	\author	BHK

	\param	hDGN	a parameter of type CDGNFile*
	\param	psElement	a parameter of type DGNElemCore *

	\return	bool	
*/
bool CDGNParser::ParseCore(CDGNFile* hDGN, DGNElemCore *psElement)
{
	assert(hDGN      && "hDGN is NULL");
	assert(psElement && "psElement is NULL");

	if(hDGN && psElement)
	{
		unsigned char* psData = hDGN->abyElem+0;

		psElement->level   = psData[0] & 0x3f;	// 
		psElement->complex = psData[0] & 0x80;  // 
		psElement->deleted = psData[1] & 0x80;  //
		psElement->type    = psData[1] & 0x7f;  //! element type

		if(hDGN->nElemBytes >= 36 )
		{
			psElement->graphic_group = psData[28] + psData[29] * 256;
			psElement->properties = psData[32] + psData[33] * 256;
			psElement->style = psData[34] & 0x7;
			psElement->weight = (psData[34] & 0xf8) >> 3;
			psElement->color = psData[35];

			if( psElement->properties & DGNPF_ATTRIBUTES )
			{
				int   nAttIndex=0;

				nAttIndex = psData[30] + psData[31] * 256;
				psElement->attr_bytes = hDGN->nElemBytes - nAttIndex*2 - 32;
				assert(!(psElement->attr_bytes%4));
				if(psElement->attr_bytes > 0)
				{
					psElement->attr_data = (unsigned char*)CDGNLib::AllocMemory(sizeof(char)*(psElement->attr_bytes));
					memcpy(psElement->attr_data, psData + nAttIndex*2 + 32,  sizeof(char)*psElement->attr_bytes);
				}
			}
		}
	}
	
	return true;
}

/**	\brief	Parse application element.

	\author	BHK

	\param	hDGN	handle of dgn file

	\return	long	
*/
long CDGNParser::ParseApplicationElements(CDGNFile* hDGN)
{
	assert(hDGN      && "hDGN is NULL");
	long nWords=0;

	if(hDGN)
	{
		nWords = (int)(hDGN->nElemBytes*0.5);

		short screen_flag=*(short*)(hDGN->abyElem + 36);
		
		unsigned char r=0,g=0,b=0;
		for(int i=0;i < 256;++i)
		{
			r = *(unsigned char*)(hDGN->abyElem + i*3 + 38);
			g = *(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
			b = *(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
			
			hDGN->color_table[i][0] = r;
			hDGN->color_table[i][1] = g;
			hDGN->color_table[i][2] = b;
		}
	}
	
	return nWords;
}

/**	\brief	The CDGNParser::ParseColorTable function

	\param	hDGN	a parameter of type CDGNFile*

	\return	long	
*/
long CDGNParser::ParseColorTable(CDGNFile* hDGN)
{
	assert(hDGN      && "hDGN is NULL");
	long nWords=0;

	if(hDGN)
	{
		nWords = (int)(hDGN->nElemBytes*0.5);

		short screen_flag=*(short*)(hDGN->abyElem + 36);
		
		unsigned char r=0,g=0,b=0;
		for(int i=0;i < 256;i++)
		{
			r = *(unsigned char*)(hDGN->abyElem + i*3 + 38);
			g = *(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
			b = *(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
			
			hDGN->color_table[i][0] = r;
			hDGN->color_table[i][1] = g;
			hDGN->color_table[i][2] = b;
		}
	}
	
	return nWords;
}

/**	
	@brief	The CDGNParser::ParseDimension function

	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long	
*/
long CDGNParser::ParseDimension(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN)
	{
		nWords = (int)(hDGN->nElemBytes*0.5);

		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		unsigned char  ch=0;
		unsigned short sh=0;
		unsigned long   l=0;
		double d=0;
		
		double nScale=0.;
		memcpy(&nScale,hDGN->abyElem + 36,8);
		DGN2IEEEDouble(&nScale);

		char nDimLineStyle=*(char*)(hDGN->abyElem + 52);
		unsigned char nDimLineColor=*(char*)(hDGN->abyElem + 53);

		double nDimTextWidth=0.;
		nDimTextWidth = *(long*)(hDGN->abyElem + 36 + 20);
		nDimTextWidth/= 7680000;
		double nDimTextHeight=0.;
		nDimTextHeight = *(long*)(hDGN->abyElem + 36 + 24);
		nDimTextHeight/= 7680000;

		char nDimFont   = *(char*)(hDGN->abyElem + 64);
		char nDimColor  = *(char*)(hDGN->abyElem + 65);
		char nDimWeight = (*(char*)(hDGN->abyElem + 67)) & 0x0f;

		double nWitnessLineOffset=0.;
		nWitnessLineOffset = *(long*)(hDGN->abyElem + 72);
		nWitnessLineOffset/= 7680000;
		double nWitnessLineExt=0.;
		nWitnessLineExt = *(long*)(hDGN->abyElem + 76);
		nWitnessLineExt/= 7680000;

		double nLowerMargin=0.;
		nLowerMargin = *(long*)(hDGN->abyElem + 80);
		nLowerMargin/= 7680000;
		double nLeftMargin=0.;
		nLeftMargin = *(long*)(hDGN->abyElem + 84);
		nLeftMargin/= 7680000;

		double nLeader=0.;
		nLeader = *(long*)(hDGN->abyElem + 88);
		nLeader/= 7680000;

		double nTermWidth=0.;
		nTermWidth = *(long*)(hDGN->abyElem + 92);
		nTermWidth/= 7680000;
		double nTermHeight=0.;
		nTermHeight = *(long*)(hDGN->abyElem + 96);
		nTermHeight/= 7680000;

		short nID=0;
		nID = *(short*)(hDGN->abyElem + 116);

		double nValue1=0.;
		nValue1 = *(long*)(hDGN->abyElem + 120);

		double x1 = *(long*)(hDGN->abyElem + 136);
		double y1 = *(long*)(hDGN->abyElem + 140);
		double z1 = *(long*)(hDGN->abyElem + 144);
		double x2 = *(long*)(hDGN->abyElem + 156);
		double y2 = *(long*)(hDGN->abyElem + 160);
		double z2 = *(long*)(hDGN->abyElem + 164);

		char nTextJust=1;
		nTextJust = (*(char*)(hDGN->abyElem + 170)) & 0xf0;
		nTextJust >>= 4;
	}
	
	return nWords;
}

/**	
	@brief	parse group data(type 5 : reference file)

	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long	
*/
long CDGNParser::ParseGroupData(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	static char buf[256]={0};
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		DGNREFFILE* pRef = (DGNREFFILE*)hDGN->abyElem;
		
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = ENM_REFERENCE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		strncpy(pPrimitive->body.reference.szName,pRef->file_spec,pRef->file_chars);
		pPrimitive->body.reference.ptRefOrigin.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 188)*hDGN->scale;
		pPrimitive->body.reference.ptRefOrigin.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 192)*hDGN->scale;
		pPrimitive->body.reference.ptRefOrigin.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 196)*hDGN->scale;

		pPrimitive->body.reference.ptDestOrigin.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 280)*hDGN->scale;
		pPrimitive->body.reference.ptDestOrigin.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 284)*hDGN->scale;
		pPrimitive->body.reference.ptDestOrigin.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 288)*hDGN->scale;
		
		memcpy(&(pPrimitive->body.reference.nTrans[0]),hDGN->abyElem + 200,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[0]));
		memcpy(&(pPrimitive->body.reference.nTrans[1]),hDGN->abyElem + 208,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[1]));
		memcpy(&(pPrimitive->body.reference.nTrans[2]),hDGN->abyElem + 216,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[2]));
		memcpy(&(pPrimitive->body.reference.nTrans[3]),hDGN->abyElem + 224,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[3]));
		memcpy(&(pPrimitive->body.reference.nTrans[4]),hDGN->abyElem + 232,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[4]));
		memcpy(&(pPrimitive->body.reference.nTrans[5]),hDGN->abyElem + 240,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[5]));
		memcpy(&(pPrimitive->body.reference.nTrans[6]),hDGN->abyElem + 248,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[6]));
		memcpy(&(pPrimitive->body.reference.nTrans[7]),hDGN->abyElem + 256,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[7]));
		memcpy(&(pPrimitive->body.reference.nTrans[8]),hDGN->abyElem + 264,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nTrans[8]));
		
		memcpy(&(pPrimitive->body.reference.nScale),hDGN->abyElem + 272,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.nScale));
		
		short* vertices = (short*)(hDGN->abyElem + 492);
		if((*vertices))
		{
			double x=0.,y=0.,xl=0.,xh=0.,yl=0.,yh=0.;
			
			xl = xh = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 494);
			yl = yh = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 498);
			for(int i = 0;i < (*vertices);i++)
			{
				x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 494 + i*8);
				y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 498 + i*8);
				if(xl > x) xl = x;
				if(xh < x) xh = x;
				if(yl > y) yl = y;
				if(yh < y) yh = y;
			}
			pPrimitive->body.reference.sizDest.nWidth = (xh - xl)*hDGN->scale;
			pPrimitive->body.reference.sizDest.nHeight= (yh - yl)*hDGN->scale;
			pPrimitive->body.reference.sizDest.nDepth = 0.;
		}
		
		strncpy(buf,(char*)(hDGN->abyElem + 294),22);
		int i = 0;
		for(i = 0;(i < 22) && (buf[i] != '_');i++);
		buf[i] = '\0';
		pPrimitive->body.reference.nIndex = atoi(buf);

		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/////////////////////////////////OLD VERSION/////////////////////
/**	
	@brief	The CDGNParser::ParseSolid function

	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@remarks	[2003.07.29] - check solid header's total length for reading elements.
	
	@return	long	
*/
long CDGNParser::ParseSolid(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	const static unsigned short nSkip=19;
	long nWords=0L;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		pPrimitive->hdr.id = SQ_SOLID;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);

		PSQPRIMITIVE* ppElements[256]={0,};
		unsigned short nTotalLen=*((unsigned short*)(hDGN->abyElem + 36));
		unsigned short nElements=*((unsigned short*)(hDGN->abyElem + 38));
		for(int i = 0;(i < nElements) && ((nWords - nSkip) < nTotalLen) && (i < 256);i++)
		{
			if(m_pDGNScanner->ReadElement(hDGN) && CDGNLib::IsPartOfComplexElement(hDGN))
			{
				if(ppElements[i] = (PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE)))
				{
					*ppElements[i] = NULL;
					nWords += ParseElement(ppElements[i],hDGN);
					if(*ppElements[i]) pPrimitive->body.pPrimitive = ListPush(pPrimitive->body.pPrimitive,*(ppElements[i]));
				}
			}else	return nWords;
		}
		
		if(pPrimitive->body.pPrimitive)
		{
			for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next)
			{
				if(ptr->hdr.volume.x[LOW] < pPrimitive->hdr.volume.x[LOW])  pPrimitive->hdr.volume.x[LOW] = ptr->hdr.volume.x[LOW];
				if(ptr->hdr.volume.x[HIGH]> pPrimitive->hdr.volume.x[HIGH]) pPrimitive->hdr.volume.x[HIGH]= ptr->hdr.volume.x[HIGH];
				if(ptr->hdr.volume.y[LOW] < pPrimitive->hdr.volume.y[LOW])  pPrimitive->hdr.volume.y[LOW] = ptr->hdr.volume.y[LOW];
				if(ptr->hdr.volume.y[HIGH]> pPrimitive->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.y[HIGH]= ptr->hdr.volume.y[HIGH];
				if(ptr->hdr.volume.z[LOW] < pPrimitive->hdr.volume.z[LOW])  pPrimitive->hdr.volume.z[LOW] = ptr->hdr.volume.z[LOW];
				if(ptr->hdr.volume.z[HIGH]> pPrimitive->hdr.volume.z[HIGH]) pPrimitive->hdr.volume.z[HIGH]= ptr->hdr.volume.z[HIGH];
			}
			(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
		}
	}
	return nWords;
}

/**	@brief	parse surface element.
	first boundary elemnt appears and second boundary element follows.\n
	and rule elements follow boudary elements immediately.\n
	support 0(surface of projection),4(right circular cone),8(surface of revolution) types
	
	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@remarks	[2003.05.15] - convert right circular cone to cone.\n
	[2003.07.29] - check surface header's total length for reading elements.

	@return	long	
*/
long CDGNParser::ParseSurface(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	const static unsigned short nSkip = 19;
	long nWords = 0L;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = SQ_SURFACE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (long)(hDGN->nElemBytes*0.5);

		pPrimitive->body.pSurface = (PSQSURFACE)CDGNLib::AllocMemory(sizeof(SQSURFACE));
		if(pPrimitive->body.pSurface)
		{
			unsigned short nTotalLen=*((unsigned short*)(hDGN->abyElem + 36));
			unsigned char nSurfType =*((unsigned char*)(hDGN->abyElem + 40));
			pPrimitive->body.pSurface->nType     =pDGNLib->GetSurfaceType(*((unsigned char*)(hDGN->abyElem + 40)));
			pPrimitive->body.pSurface->nBoundary =*((unsigned char*)(hDGN->abyElem + 41)) + 1;
			
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a)
			{
				CDGNElement::ParseDMRS(pPrimitive,hDGN);
				PDGNATTRDATA pAttrData = (PDGNATTRDATA)pPrimitive->pLinkageData;
				if(pAttrData && (1 == pAttrData->hdr.u) && (92 == pAttrData->UserID))
				{
					strncpy(pPrimitive->hdr.szDesc , pAttrData->data.pData + 12 , sizeof(char) * 15);
				}
			}
			
			PSQPRIMITIVE* ppElements[256]={0,};
			unsigned short nElements=*((unsigned short*)(hDGN->abyElem + 38)),nIndex=0;
			int i=0;
			for(i=0;(i < nElements) && ((nWords - nSkip) < nTotalLen) && (nIndex < 256);nIndex++)
			{
				if(hDGN->ReadElement() && CDGNLib::IsPartOfComplexElement(hDGN))
				{
					if(ppElements[nIndex] = (PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE)))
					{
						*ppElements[nIndex] = NULL;
						nWords += ParseElement(ppElements[nIndex],hDGN);
						i      += GetNumOfElements(*ppElements[nIndex]);
					}
				}else	return nWords;
			}
			
			PSQPRIMITIVE pLastRuleCurve = NULL;
			for(i = 0;(i < nIndex) && (i < 256);)
			{
				SQSURFACE::PSURFELM_T pSurfaceElement=(SQSURFACE::PSURFELM_T)CDGNLib::AllocMemory(sizeof(SQSURFACE::SURFELM_T));
				if(pSurfaceElement)
				{
					// store 1'st boundary element.
					if(NULL == pLastRuleCurve)
					{
						for(int j=0;(j < pPrimitive->body.pSurface->nBoundary) && 
							(i < nIndex) && (PRIMARY_CRV_T == (*ppElements[i])->hdr.attr.type);j++)
						{
							pSurfaceElement->pBoundaryCurve = ListPush(pSurfaceElement->pBoundaryCurve,(*ppElements[i++]));
						}
					}
					else
					{
						pSurfaceElement->pBoundaryCurve = CDGNLib::CopyPrimitive(pLastRuleCurve);
						/*
						PSQPRIMITIVE ptr = (PSQPRIMITIVE)CDGNLib::AllocMemory(sizeof(SQPRIMITIVE));
						memcpy(ptr , pLastRuleCurve , sizeof(SQPRIMITIVE));
						pSurfaceElement->pBoundaryCurve = ptr;
						*/
					}
					
					// store 2'nd boundary element.
					for(int j=0;(j < pPrimitive->body.pSurface->nBoundary) && 
						(i < nIndex) && (PRIMARY_CRV_T == (*ppElements[i])->hdr.attr.type);j++)
					{
						pLastRuleCurve = *ppElements[i];
						pSurfaceElement->pBoundaryCurve = ListPush(pSurfaceElement->pBoundaryCurve,(*ppElements[i++]));
					}
					
					// store rule elements
					while((i < nIndex) && (RULE_CRV_T == (*ppElements[i])->hdr.attr.type))
					{
						pSurfaceElement->pRuleCurve = ListPush(pSurfaceElement->pRuleCurve,(*ppElements[i++]));
					}
					
					if(pSurfaceElement->pBoundaryCurve && pSurfaceElement->pRuleCurve)
						pPrimitive->body.pSurface->pElement = ListPush(pPrimitive->body.pSurface->pElement,pSurfaceElement);
					else	break;
				}else	break;
			}
			
			if(pPrimitive->body.pSurface->pElement && pPrimitive->body.pSurface->pElement->pBoundaryCurve)
			{
				pPrimitive->hdr.volume = pPrimitive->body.pSurface->pElement->pBoundaryCurve->hdr.volume;
				for(SQSURFACE::PSURFELM_T ptrElement=pPrimitive->body.pSurface->pElement;ptrElement;ptrElement = ptrElement->next)
				{
					for(PSQPRIMITIVE ptr=ptrElement->pBoundaryCurve;ptr;ptr=ptr->next)
					{
						if(pPrimitive->hdr.volume.x[0] > ptr->hdr.volume.x[0]) pPrimitive->hdr.volume.x[0] = ptr->hdr.volume.x[0];
						if(pPrimitive->hdr.volume.x[1] < ptr->hdr.volume.x[1]) pPrimitive->hdr.volume.x[1] = ptr->hdr.volume.x[1];
						if(pPrimitive->hdr.volume.y[0] > ptr->hdr.volume.y[0]) pPrimitive->hdr.volume.y[0] = ptr->hdr.volume.y[0];
						if(pPrimitive->hdr.volume.y[1] < ptr->hdr.volume.y[1]) pPrimitive->hdr.volume.y[1] = ptr->hdr.volume.y[1];
						if(pPrimitive->hdr.volume.z[0] > ptr->hdr.volume.z[0]) pPrimitive->hdr.volume.z[0] = ptr->hdr.volume.z[0];
						if(pPrimitive->hdr.volume.z[1] < ptr->hdr.volume.z[1]) pPrimitive->hdr.volume.z[1] = ptr->hdr.volume.z[1];
					}
				}
			}

			//--> right circular cone
			if(DGNST_RIGHT_CIRCULAR_CONE == nSurfType)
			{
				pPrimitive->hdr.id = SQ_CONE;

				POINT_T ptOrigin[2]={0,};
				double  nRadius[2]={0,};
				
				SQSURFACE::PSURFELM_T pSurfElm=pPrimitive->body.pSurface->pElement,ptrElmNext=NULL;
				if(pSurfElm->pBoundaryCurve && pSurfElm->pBoundaryCurve->next)
				{
					ptOrigin[0] = pSurfElm->pBoundaryCurve->body.ellipse.ptOrigin;
					nRadius[0]  = pSurfElm->pBoundaryCurve->body.ellipse.nAxis[0];
					ptOrigin[1] = pSurfElm->pBoundaryCurve->next->body.ellipse.ptOrigin;
					nRadius[1]  = pSurfElm->pBoundaryCurve->next->body.ellipse.nAxis[0];
				}
				
				/*for(i = 0;i < nIndex;i++){
					(*ppElements[i])->next = NULL;
					CDGNLib::DeletePrimitive((*ppElements[i]));
				}
				*/
				
				/*
				free((void*)pSurfElm);
				pSurfElm=NULL;
				for(SQSURFACE::PSURFELM_T ptrElm=pSurfElm;ptrElm;ptrElm = ptrElmNext)
				{
					ptrElmNext = ptrElm->next;

					CDGNLib::DeleteElement(ptrElm->pBoundaryCurve);
					CDGNLib::DeleteElement(ptrElm->pRuleCurve);
					CDGNLib::FreeMemory((void*)ptrElm);
				}
				*/

				pPrimitive->body.cone.nRadius[0] = nRadius[0];
				pPrimitive->body.cone.nRadius[1] = nRadius[1];
				pPrimitive->body.cone.ptOrigin[0]= ptOrigin[0];
				pPrimitive->body.cone.ptOrigin[1]= ptOrigin[1];
				pPrimitive->body.cone.vecAxis.dx = ptOrigin[1].x - ptOrigin[0].x;
				pPrimitive->body.cone.vecAxis.dy = ptOrigin[1].y - ptOrigin[0].y;
				pPrimitive->body.cone.vecAxis.dz = ptOrigin[1].z - ptOrigin[0].z;
			}
			//<--
		}
		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	
	@brief	The CDGNParser::ParseBSplineSurfaceHeader function

	@author	HumKyung

	@date	????.??.??

	@param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long
*/
long CDGNParser::ParseBSplineSurfaceHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		static double  aryKnot[102]  ={0};
		static POINT_T aryPoint[102] ={0};
		static double  aryWeight[102]={0};

		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = ENM_GROUP;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;

		if(401 == pPrimitive->hdr.handle)
		{
			int i = 1;
		}

		nWords = (int)(hDGN->nElemBytes*0.5);
		long nElementWords=SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36);
		
		struct Bspline_surface::tagFlags* pFlags=(struct Bspline_surface::tagFlags*)(hDGN->abyElem + 40);
		short nDegree  = pFlags->order + 1;
		bool  bRational= pFlags->rational;

		short nUPoints = *((short*)(hDGN->abyElem + 42));
		short nUKnots  = *((short*)(hDGN->abyElem + 44));
		short nURules  = *((short*)(hDGN->abyElem + 46));
		short nVPoints = *((short*)(hDGN->abyElem + 50));
		short nVKnots  = *((short*)(hDGN->abyElem + 52));
		short nVRules  = *((short*)(hDGN->abyElem + 54));
		short nBoundary= *((short*)(hDGN->abyElem + 56));

		nElementWords = nElementWords - (int)(hDGN->nElemBytes*0.5) + 20;
		nWords += nElementWords;
		bool bExistKnot=false;
		while(nElementWords)
		{
			if(m_pDGNScanner->ReadElement(hDGN))
			{
				nWords += (int)(hDGN->nElemBytes*0.5);
				nElementWords -= (int)(hDGN->nElemBytes*0.5);
				
				PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
				switch(pDGNHDR->ElmHdr.type)
				{
					case DGNT_B_SPLINE_POLE:
						ParseBSplinePole(aryPoint,hDGN);
						break;
					case DGNT_B_SPLINE_KNOT:
						ParseBSplineKnot(hDGN,aryKnot,nUKnots);
						bExistKnot = true;
						break;
					case DGNT_B_SPLINE_WEIGHT_FACTOR:
					{
						ParseBSplineWeightFactor(hDGN,aryWeight,nUPoints);
						
						/*
						PSQPRIMITIVE pElement=NULL;
						if(pElement = (PSQPRIMITIVE)CDGNLib::AllocMemory(sizeof(SQPRIMITIVE)))
						{
							//GetHeader(pRet,hDGN);
							pElement->hdr.id     = SQ_NURBS;
							pElement->hdr.attr.d = hDGN->dimension;
							pElement->hdr.attr.f = 1;
							
							pElement->body.pNurbs= (PSQNURBS)CDGNLib::AllocMemory(sizeof(NURBS));
							if(pElement->body.pNurbs)
							{
								pElement->body.pNurbs->nDegree = nDegree;
								pElement->body.pNurbs->nPoints = nUPoints;
								pElement->body.pNurbs->pKnots  = (double*)CDGNLib::AllocMemory(sizeof(double)*(nUKnots+2*nDegree));
								memcpy(pElement->body.pNurbs->pKnots + nDegree,aryKnot,sizeof(double)*nUKnots);
								for(int i=0;i < nDegree;i++) pElement->body.pNurbs->pKnots[i] = aryKnot[0];
								for(i = 0;i < nDegree;i++)   pElement->body.pNurbs->pKnots[nUPoints + i + 1] = aryKnot[nUKnots - 1];
								pElement->body.pNurbs->ppt     = (NURBS::WPOINT*)CDGNLib::AllocMemory(sizeof(NURBS::WPOINT)*nUPoints);
								for(i=0;i < nUPoints;i++)
								{
									pElement->body.pNurbs->ppt[i].weight = aryWeight[i];
									pElement->body.pNurbs->ppt[i].pt     = aryPoint[i];
								}
								pPrimitive->body.pPrimitive = ListPush(pPrimitive->body.pPrimitive,pElement);
							}
						}
						*/
					}
						break;
					case DGNT_B_SPLINE_SURFACE_BOUNDARY:
					{
						int nPoints=ParseBSplineSurfaceBoundary(aryPoint,hDGN);
					}
					break;
					default:
						break;
				}
			}else	break;
		}
		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	
	@brief	The CDGNParser::ParseBSplinePole function

	@author	HumKyung

	@
	@param	pPoints	a parameter of type POINT_T*
	@param	hDGN	a parameter of type CDGNFile*

	@return	long
*/
long CDGNParser::ParseBSplinePole(POINT_T* pPoints,CDGNFile* hDGN)
{
	assert(hDGN    && "hDGN is NULL");
	assert(pPoints && "pPoints is NULL");
	long nWords=0;

	if(hDGN && pPoints)
	{
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		if(2 == hDGN->dimension)
		{
			unsigned short nVertices = *((unsigned short*)(hDGN->abyElem + 36));
			for(int i = 0;i < nVertices;i++)
			{
				POINT_T pt={0};

				pt.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*8 + 38);
				pt.x*= hDGN->scale;
				pt.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*8 + 42);
				pt.y*= hDGN->scale;
				pt.z = 0.;

				pPoints[i] = pt;
			}
		}
		else if(3 == hDGN->dimension)
		{
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

				pPoints[i] = pt;
			}
		}
	}

	return nWords;
}

/**	\brief	The CDGNParser::ParseBSplineKnot function\n
	parse b-spline knot.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pKnot	a parameter of type double*
	\param	nPoints	a parameter of type const int

	\return	long
*/
long CDGNParser::ParseBSplineKnot(CDGNFile* hDGN,double* pKnot,const int nPoints)
{
	assert(hDGN  && "hDGN is NULL");
	assert(pKnot && "pKnot is NULL");

	if(hDGN && pKnot)
	{
		const long divider=2147483648 - 1;/*2^31 - 1*/
		for(int i=0;i < nPoints;i++)
		{
			double knot = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36 + i*4);
			knot /= divider;
			pKnot[i] = knot;
		}
	}

	return 1L;
}

/**	\brief	The CDGNParser::ParseBSplineWeightFactor function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pWeight	a parameter of type double*
	\param	nPoints	a parameter of type const int

	\return	long
*/
long CDGNParser::ParseBSplineWeightFactor(CDGNFile* hDGN,double* pWeight,const int nPoints)
{
	assert(hDGN    && "hDGN is NULL");
	assert(pWeight && "pWeight is NULL");

	if(hDGN && pWeight)
	{
		const long divider=2147483648 - 1;/*2^31 - 1*/
		for(int i=0;i < nPoints;i++)
		{
			double weight = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36 + i*4);
			weight /= divider;
			pWeight[i] = weight;
		}
	}

	return 1L;
}

/**	\brief	The CDGNParser::ParseBSplineSurfaceBoundary function

	\param	pPoints	a parameter of type PPOINT_T
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNParser::ParseBSplineSurfaceBoundary(PPOINT_T pPoints,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	short nPoints=0;

	if(hDGN)
	{
		short nBoundary=*((short*)(hDGN->abyElem + 36));
		nPoints  =*((short*)(hDGN->abyElem + 38));
		
		int nIndex=0;
		for(int i=0;i < nPoints;i++)
		{
			long x=SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + nIndex + 40);
			nIndex+=4;

			long y=SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + nIndex + 40);
			nIndex+=4;

			long z=SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + nIndex + 40);
			nIndex+=4;

			pPoints[i].x = x*hDGN->scale;
			pPoints[i].y = y*hDGN->scale;
			pPoints[i].z = z*hDGN->scale;
		}
	}

	return nPoints;
}

/**	\brief	The CDGNParser::ParseComplexShapeOrChain function\n
	complex element formed from a series of elements.

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNParser::ParseComplexShapeOrChain(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = SQ_LOOP;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);

		unsigned short nElements=*((unsigned short*)(hDGN->abyElem + 38));
		for(int i=0;i < nElements;i++)
		{
			PSQPRIMITIVE* ppElement=NULL;
			if(/*(ptr = pDGNLib->NewPrimitive()) && */(m_pDGNScanner->ReadElement(hDGN)))
			{
				//GetHeader(ptr,hDGN);
				if(ppElement = (PSQPRIMITIVE*)CDGNLib::AllocMemory(sizeof(PSQPRIMITIVE)))
				{
					*ppElement = NULL;
					nWords += ParseElement(ppElement,hDGN);
					if(*ppElement) pPrimitive->body.pPrimitive = ListPush(pPrimitive->body.pPrimitive,*ppElement);
				}
			}
		}		
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);

		if(pPrimitive->body.pPrimitive)
		{
			pPrimitive->hdr.volume = pPrimitive->body.pPrimitive->hdr.volume;
			for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive->next;ptr;ptr=ptr->next)
			{
				if(pPrimitive->hdr.volume.x[LOW] > ptr->hdr.volume.x[LOW])  pPrimitive->hdr.volume.x[LOW] = ptr->hdr.volume.x[LOW];
				if(pPrimitive->hdr.volume.x[HIGH]< ptr->hdr.volume.x[HIGH]) pPrimitive->hdr.volume.x[HIGH]= ptr->hdr.volume.x[HIGH];
				if(pPrimitive->hdr.volume.y[LOW] > ptr->hdr.volume.y[LOW])  pPrimitive->hdr.volume.y[LOW] = ptr->hdr.volume.y[LOW];
				if(pPrimitive->hdr.volume.y[HIGH]< ptr->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.y[HIGH]= ptr->hdr.volume.y[HIGH];
				if(pPrimitive->hdr.volume.z[LOW] > ptr->hdr.volume.y[LOW])  pPrimitive->hdr.volume.z[LOW] = ptr->hdr.volume.z[LOW];
				if(pPrimitive->hdr.volume.z[HIGH]< ptr->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.z[HIGH]= ptr->hdr.volume.z[HIGH];
			}
		}

		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	
	@brief	parse terminal control block.

	@author	humkyung

	@param	hDGN	a parameter of type CDGNFile*

	@return	long
*/
long CDGNParser::ParseTCB(CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0L;
	
	if(hDGN)
	{
		DGNElemTCB *psTCB=NULL;
		if(psTCB = (DGNElemTCB *)CDGNLib::AllocMemory(sizeof(DGNElemTCB)))
		{
			DGNElemCore *psElement = (DGNElemCore *) psTCB;
			psElement->stype = DGNST_TCB;
			nWords = (int)(hDGN->nElemBytes*0.5);

			ParseCore(hDGN,psElement );
			
			if(hDGN->abyElem[1214] & 0x40 ) psTCB->dimension = 3;
			else				psTCB->dimension = 2;
			
			psTCB->master_units[0] = (char) hDGN->abyElem[1120];
			psTCB->master_units[1] = (char) hDGN->abyElem[1121];
			psTCB->master_units[2] = '\0';
			
			psTCB->subunits_per_master = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 1112);
			psTCB->uor_per_subunit     = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 1116);
			
			psTCB->sub_units[0] = (char) hDGN->abyElem[1122];
			psTCB->sub_units[1] = (char) hDGN->abyElem[1123];
			psTCB->sub_units[2] = '\0';
			
			/* NOTDEF: Add origin extraction later */
			if(!hDGN->got_tcb )
			{
				hDGN->got_tcb  = true;
				hDGN->dimension= psTCB->dimension;
				hDGN->m_ptOrigin.x = psTCB->origin_x;
				hDGN->m_ptOrigin.y = psTCB->origin_y;
				hDGN->m_ptOrigin.z = psTCB->origin_z;
				
				if(psTCB->uor_per_subunit != 0 && psTCB->subunits_per_master != 0)
				{
					hDGN->scale = 1.0 / (psTCB->uor_per_subunit);/// * psTCB->subunits_per_master);
				}
			}
		}
	}
	
	return nWords;
}

/**	\brief	The CDGNParser::ParseRasterHeader function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNParser::ParseRasterHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;
	
	if(hDGN)
	{
		nWords = (int)(hDGN->nElemBytes*0.5);
	}

	return nWords;
}

/**	\brief	The CDGNParser::ClippingByVolume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE
	\param	pVolume	a parameter of type PSQVOLUME

	\return	PSQPRIMITIVE
*/
PSQPRIMITIVE CDGNParser::ClippingByVolume(PSQPRIMITIVE pPrimitive,PSQVOLUME pVolume)
{
	assert(pPrimitive   && "pPrimitive is invalid");
	assert(pVolume      && "pVolume is invaid");
	PSQPRIMITIVE pRet = NULL;

	if(pPrimitive && pVolume)
	{
		if(pPrimitive->hdr.attr.f)
		{
			switch(pPrimitive->hdr.id)
			{
				case ENM_LINE:
					pRet = ClippingPrimitiveByVolume(pRet,pPrimitive,pVolume);
				break;
				case SQ_POINTS:
				{
					pRet = ClippingPrimitiveByVolume(pRet,pPrimitive,pVolume);
				}
				break;
				case ENM_GROUP:
					if(!IsOuterVolume((*pVolume),pPrimitive->hdr.volume))
					{
						pRet = pPrimitive;
					}
				break;
				case SQ_SOLID:
					if(!IsOuterVolume((*pVolume),pPrimitive->hdr.volume))
					{
						pRet = pPrimitive;
					}
				break;
				case ENM_ARC:case ENM_ELLIPSE:case SQ_TEXT:
					pRet = pPrimitive;
				break;
			}
		}else	pRet = pPrimitive;
	}

	return pRet;
}

/**	\brief	The CDGNParser::ClippingPrimitiveByVolume function\n
	clipping pSrc by pVolume and store to pDest.

	\param	pDest	a parameter of type PSQPRIMITIVE
	\param	pSrc	a parameter of type PSQPRIMITIVE
	\param	pVolume	a parameter of type PSQVOLUME

	\return	PSQPRIMITIVE
*/
PSQPRIMITIVE CDGNParser::ClippingPrimitiveByVolume(PSQPRIMITIVE pDest,PSQPRIMITIVE pSrc,PSQVOLUME pVolume)
{
	assert(pSrc    && "source is invalid");
	assert(pVolume && "volume is invaid");
	POINT_T ret[8],pt={0};
	LINE_T  line={0};

	if(pSrc && pVolume)
	{
		switch(pSrc->hdr.id)
		{
			case ENM_LINE:
			{
				PSQPRIMITIVE pLine = NULL;

				line.ptStart.x = pSrc->body.line.ptStart.x;
				line.ptStart.y = pSrc->body.line.ptStart.x;
				line.ptStart.z = pSrc->body.line.ptStart.x;
				line.ptEnd.x = pSrc->body.line.ptEnd.x;
				line.ptEnd.y = pSrc->body.line.ptEnd.x;
				line.ptEnd.z = pSrc->body.line.ptEnd.x;
								
				ITKGeo::IntersectLineToVolume(&line,pVolume,ret);
				pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
				pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
				pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
				if(IsPointInVolume(&pt,pVolume))
				{
					pSrc->body.line.ptStart.x = line.ptStart.x;
					pSrc->body.line.ptStart.y = line.ptStart.y;
					pSrc->body.line.ptStart.z = line.ptStart.z;
					
					pSrc->body.line.ptEnd.x = line.ptEnd.x;
					pSrc->body.line.ptEnd.y = line.ptEnd.y;
					pSrc->body.line.ptEnd.z = line.ptEnd.z;
					
					pDest = ListPush(pDest,pSrc);
				}
			}
			break;
			case SQ_POINTS:
			{
				PSQPRIMITIVE pPolyline = NULL;

				vector<POINT_T>::iterator itr;
				for(itr = pSrc->body.pvtPoints->begin();(itr + 1) != pSrc->body.pvtPoints->end();++itr)
				{
					line.ptStart.x = itr->x;
					line.ptStart.y = itr->y;
					line.ptStart.z = itr->z;
					line.ptEnd.x = (itr + 1)->x;
					line.ptEnd.y = (itr + 1)->y;
					line.ptEnd.z = (itr + 1)->z;
					
					ITKGeo::IntersectLineToVolume(&line,pVolume,ret);
					pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
					pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
					pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
					if(IsPointInVolume(&pt,pVolume))
					{
						if(pPolyline == NULL)
						{
							pPolyline = new SQPRIMITIVE;
							memset(pPolyline,0,sizeof(SQPRIMITIVE));
							pPolyline->body.pvtPoints = new vector<POINT_T>;
							
							pPolyline->hdr.id     = SQ_POINTS;
							int nLen=strlen(pSrc->hdr.pLayer);
							if(pPolyline->hdr.pLayer=(char*)CDGNLib::AllocMemory(nLen + 1)) strcpy(pPolyline->hdr.pLayer,pSrc->hdr.pLayer);
							pPolyline->hdr.attr.f = 1;
							pPolyline->hdr.handle = pSrc->hdr.handle;
							pPolyline->pLinkageData      = pSrc->pLinkageData;
						}
						
						if(pPolyline && pPolyline->body.pvtPoints)
						{
							pPolyline->body.pvtPoints->push_back(line.ptStart);
							pPolyline->body.pvtPoints->push_back(line.ptEnd);
						}
					}
					else if(pPolyline)
					{
						vector<POINT_T>::iterator itr;
						for(itr = pPolyline->body.pvtPoints->begin();(itr != pPolyline->body.pvtPoints->end()) && ((itr+1) != pPolyline->body.pvtPoints->end());++itr)
						{
							if((itr->x == (itr+1)->x) && (itr->y == (itr+1)->y) && (itr->z == (itr+1)->z))
							{
								pPolyline->body.pvtPoints->erase(itr--);
							}
						}
						pDest = ListPush(pDest,pPolyline);
						pPolyline = NULL;
					}
				}

				if(pPolyline)
				{
					pDest = ListPush(pDest,pPolyline);
				}
				delete pSrc;
			}
			break;
			case ENM_GROUP:
			{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;)
				{
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
			}
			break;
			case SQ_SOLID:
			{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;)
				{
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
			}
			break;
		}
	}

	return pDest;
}

/**	\brief	The CDGNParser::IsElbow function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\remarks	[2003.07.31] - check user data linkage for elbow.
	\return	bool
*/
bool CDGNParser::IsElbow(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (NULL == pPrimitive->pLinkageData)))
	{
		PSQPRIMITIVE pGroup=pPrimitive->body.pPrimitive;

		if(pGroup && (SQ_POINTS == pGroup->hdr.id))
		{
			PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pGroup->pLinkageData);
			if((DGNLT_DMRS == GetAttrDataType(pAttrData)) && (3 == pAttrData->UserID))
			{
				pAttrData = pAttrData->next;
				if(pAttrData) 	// User Data Linkage
				{
					if(2 == pAttrData->data.pData[4])
					{
						bRet = true;
					}
				}
			}
		}
	}

	return bRet;
}

/**	
	@brief	The CDGNParser::IsTee function
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@remarks\n
	;2003.06.12 - check user data linkage for tee.
	@return	bool
*/
bool CDGNParser::IsTee(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (NULL == pPrimitive->pLinkageData)))
	{
		PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;

		if(pElement && (SQ_POINTS == pElement->hdr.id))
		{
			PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pElement->pLinkageData);
			if((DGNLT_DMRS == GetAttrDataType(pAttrData)) && (3 == pAttrData->UserID))
			{
				pAttrData = pAttrData->next;
				if(pAttrData){	// User Data Linkage
					if(5 == pAttrData->data.pData[4])
					{
						/*
						pElement = pElement->next;
						if(pElement && (SQ_CONE == pElement->hdr.id)){
							double nTol=pElement->body.cone.nRadius[1] - pElement->body.cone.nRadius[0];
							if(fabs(nTol) > 0.00001) return false;
							
							pElement = pElement->next;
							if((pElement && (NULL == pElement->next)) && (SQ_CONE == pElement->hdr.id)){
								double nTol=pElement->body.cone.nRadius[1] - pElement->body.cone.nRadius[0];
								if(fabs(nTol) > 0.00001) return false;
								
								bRet = true;
							}
						}
						*/
						bRet = true;
					}
				}
			}
		}
	}

	return bRet;
}

/**	\brief	The CDGNParser::IsPipe function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNParser::IsPipe(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (NULL == pPrimitive->pLinkageData)))
	{
		PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;

		if(pElement && (SQ_POINTS == pElement->hdr.id))
		{
			PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pElement->pLinkageData);
			if((DGNLT_DMRS == GetAttrDataType(pAttrData)) && (5 == pAttrData->UserID))
			{
				pPrimitive->hdr.handle = pAttrData->data.occur;
				bRet = true;
			}
		}
	}
	else if(pPrimitive && (SQ_CONE == pPrimitive->hdr.id))
	{
		PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pPrimitive->pLinkageData);
		if((DGNLT_DMRS == GetAttrDataType(pAttrData)) && (5 == pAttrData->UserID))
		{
			pPrimitive->hdr.handle = pAttrData->data.occur;

			bRet = true;
		}
	}

	return bRet;
}

/**	
	@brief	The CDGNParser::IsEquipment function
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@return	bool
*/
bool CDGNParser::IsEquipment(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (pPrimitive->pLinkageData)))
	{
		PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pPrimitive->pLinkageData);
		if(pAttrData && (21 == pAttrData->UserID))
		{
			bRet = true;
		}
	}

	return bRet;
}

/**
 * IsNozzle:
 *
 * @param pPrimitive 
 * @return bool 
 */
bool CDGNParser::IsNozzle(PSQPRIMITIVE pPrimitive)
{
	static const int NOZZLE_ID = 22;

	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (pPrimitive->pLinkageData)))
	{
		PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pPrimitive->pLinkageData);
		if(pAttrData && (NOZZLE_ID == pAttrData->UserID))
		{
			bRet = true;
		}
	}

	return bRet;
}

/**
	@breif	return true if user id is 6
	@author	humkyung
	@param pPrimitive 
	@return bool 
 */
bool CDGNParser::IsInstrument(PSQPRIMITIVE pPrimitive) const
{
	assert(pPrimitive && "pPrimitive is NULL");
	static const int INSTRUMENT_ID = 6;

	bool bRet=false;
	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (NULL == pPrimitive->pLinkageData)))
	{
		PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;
		if(pElement && (SQ_POINTS == pElement->hdr.id))
		{
			PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pElement->pLinkageData);
			if(pAttrData && (INSTRUMENT_ID == pAttrData->UserID))
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

bool CDGNParser::IsCable(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;
	
	if(pPrimitive && ((ENM_GROUP == pPrimitive->hdr.id) && (NULL != pPrimitive->pLinkageData)))
	{
		PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pPrimitive->pLinkageData);
		if(DGNLT_RIS == GetAttrDataType(pAttrData))
		{
			bRet = true;
		}
	}
	
	return bRet;
}
