#include <..\include\DGNParser.h>

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CDGNParser::ParseHeader(CElement* pElement){
	assert(pElement && "pElement is NULL");
	assert(pFile    && "pFile is NULL");

	if(pElement && pFile){
		DGNHDR* pDGNHDR=(DGNHDR*)pFile->m_aryElem;

		pElement->hdr.layer    = pDGNHDR->ElmHdr.level;
		pElement->hdr.attr.d   = pFile->m_nDimension;
		pElement->hdr.attr.f   = 1;
		pElement->hdr.attr.u   = pDGNHDR->DispHdr.props.b.a;
		pElement->hdr.attr.type= ((4 == pDGNHDR->DispHdr.props.b._class) || (6 == pDGNHDR->DispHdr.props.b._class)) ? SQ_RULE_T : SQ_PRIMARY_T;
		pElement->hdr.handle   = pFile->m_nElementID - 1;

		if(pFile->m_nElemBytes >= 36){
			//pPrimitive->hdr.group           = pDGNHDR->DispHdr.gphgrp;
			pElement->hdr.display.ltype     = pDGNHDR->DispHdr.symb.b.style;
			pElement->hdr.display.lweight   = pDGNHDR->DispHdr.symb.b.weight;
			pElement->hdr.display.lcolor[0] = pFile->m_aryColorTable[pDGNHDR->DispHdr.symb.b.color + 1][0];
			pElement->hdr.display.lcolor[1] = pFile->m_aryColorTable[pDGNHDR->DispHdr.symb.b.color + 1][1];
			pElement->hdr.display.lcolor[2] = pFile->m_aryColorTable[pDGNHDR->DispHdr.symb.b.color + 1][2];
		}
	}
}

//	paramter	:
//	description	: parse dgn elemnt's buffer.
//	remarks		:
//	returns		: element sizes in word.
CElement* CDGNParser::Parse(long& nWords,CDGNFile* pFile){
	assert(pFile && "pFile is NULL");
	CElement* pRet=NULL;
	
	if(pFile){
		DGNHDR* pDGNHDR = (DGNHDR*)pFile->m_aryElem;
		switch(pDGNHDR->ElmHdr.type){
			case DGNT_TCB:
				nWords += ParseTCB(hDGN);
				break;
			case DGNT_LINE:
				pRet = new CDGNLine;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_LINE_STRING:case DGNT_SHAPE:case DGNT_CURVE:case DGNT_POINT_STRING:
				pRet = new CDGNPolyline;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_ELLIPSE:
				pRet = new CDGNEllipse;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_TEXT:
				pRet = new CDGNText;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_CONE:
				pRet = new CDGNCone;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_COMPLEX_SHAPE_HEADER:case DGNT_COMPLEX_CHAIN_HEADER:
				pRet = new CDGNLoop;
				if(pRet){
					ParseHeader(pRet);
					nWords += pRet->Parse(pFile);
				}
				break;
			case DGNT_3D_SOLID_HEADER:
				pRet = new CDGNSolid;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_SURFACE_HEADER:
				pRet = new CDGNSurface;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_B_SPLINE_SURFACE_HEADER:
				pRet = new CDGNBSplineSurf;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_DIMENSION:
				pRet = new CDGNDimension;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_CELL_HEADER:
				pRet = new CDGNGroup;
				if(pRet){
					ParseHeader(pRet);
					pRet->Parse(pFile);
				}
				break;
			case DGNT_RASTER_HEADER:
				nWords += (long)(m_nElemBytes*0.5);
				break;
			case DGNT_RASTER_DATA:
				nWords += (long)(m_nElemBytes*0.5);
				break;
			case DGNT_GROUP_DATA:
				if(9 == pDGNHDR->ElmHdr.level)
					ParseRefData();
				else if(1 == pDGNHDR->ElmHdr.level){
					nWords += ParseColorTable();
				break;
			case DGNT_APPLICATION_ELEM:
				if(5 == pDGNHdr->ElmHdr.level) ParseRefData();
				break;
			default:break;
		}
		if(pDGNHDR->DispHdr.props.b.a && pRet) ParseDMRS(pRet,pFile);
	}

	return pRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNParser::ParseTCB(CDGNFile* pFile){
	assert(pFile && "p  is NULL");
	static bool _bGotTCB=false;
	long nWords=0;
	
	if(pFile){
		nWords = (pFile->m_nElemBytes)*0.5;

		if(pFile->m_aryElem[1214] & 0x40)	pFile->m_nDimension = CDGNFile::3D;
		else					pFile->m_nDimension = CDGNFile::2D;
			
		pFile->m_szMasterUnit[0] = (char)(pFile->m_aryElem[1120]);
		pFile->m_szMasterUnit[1] = (char)(pFile->m_aryElem[1121]);
		pFile->m_szMasterUnit[2] = '\0';
			
		pFile->m_nSubUnitPerMaster = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + 1112);
		pFile->m_nUORPerSubunit    = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + 1116);
			
		pFile->m_szSubUnit[0] = (char)(pFile->m_aryElem[1122]);
		pFile->m_szSubUnit[1] = (char)(pFile->m_aryElem[1123]);
		pFile->m_szSubUnit[2] = '\0';
			
		if(!_bGotTCB){
			_bGotTCB = true;
			//if((0L != pFile->m_nUORPerSubUnit) && (0L != pFile->m_nSubUnitPerMaster))
			//	hDGN->scale = 1.0 / (psTCB->uor_per_subunit * psTCB->subunits_per_master);
			//hDGN->scale = 1.; // for temparay.
		}
	}
	
	return nWords;
}

//	parameter	:
//	description	: parse reference file(Type 5 : reference file)
//	remarks		:
//	returns		:
long CDGNParser::ParseRefData(){
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	KDGNLib* pDGNLib=KDGNLib::GetInstance();
	static char buf[256]={0};
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive())){
		Ref_file_type5* pRef = (Ref_file_type5*)hDGN->abyElem;
		
		GetHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = SQ_REFERENCE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		strncpy(pPrimitive->body.reference.szName,pRef->file_spec,pRef->file_chars);
		pPrimitive->body.reference.RefOrigin.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 188)*hDGN->scale;
		pPrimitive->body.reference.RefOrigin.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 192)*hDGN->scale;
		pPrimitive->body.reference.RefOrigin.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 196)*hDGN->scale;

		pPrimitive->body.reference.DestOrigin.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 280)*hDGN->scale;
		pPrimitive->body.reference.DestOrigin.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 284)*hDGN->scale;
		pPrimitive->body.reference.DestOrigin.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 288)*hDGN->scale;
		
		memcpy(&(pPrimitive->body.reference.Trans[0]),hDGN->abyElem + 200,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[0]));
		memcpy(&(pPrimitive->body.reference.Trans[1]),hDGN->abyElem + 208,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[1]));
		memcpy(&(pPrimitive->body.reference.Trans[2]),hDGN->abyElem + 216,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[2]));
		memcpy(&(pPrimitive->body.reference.Trans[3]),hDGN->abyElem + 224,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[3]));
		memcpy(&(pPrimitive->body.reference.Trans[4]),hDGN->abyElem + 232,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[4]));
		memcpy(&(pPrimitive->body.reference.Trans[5]),hDGN->abyElem + 240,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[5]));
		memcpy(&(pPrimitive->body.reference.Trans[6]),hDGN->abyElem + 248,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[6]));
		memcpy(&(pPrimitive->body.reference.Trans[7]),hDGN->abyElem + 256,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[7]));
		memcpy(&(pPrimitive->body.reference.Trans[8]),hDGN->abyElem + 264,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Trans[8]));
		
		memcpy(&(pPrimitive->body.reference.Scale),hDGN->abyElem + 272,8);
		DGN2IEEEDouble(&(pPrimitive->body.reference.Scale));
		
		short* vertices = (short*)(hDGN->abyElem + 492);
		if((*vertices)){
			double x=0.,y=0.,xl=0.,xh=0.,yl=0.,yh=0.;
			
			xl = xh = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 494);
			yl = yh = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 498);
			for(int i = 0;i < (*vertices);i++){
				x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 494 + i*8);
				y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 498 + i*8);
				if(xl > x) xl = x;
				if(xh < x) xh = x;
				if(yl > y) yl = y;
				if(yh < y) yh = y;
			}
			pPrimitive->body.reference.DestSize.width = (xh - xl)*hDGN->scale;
			pPrimitive->body.reference.DestSize.height= (yh - yl)*hDGN->scale;
			pPrimitive->body.reference.DestSize.depth = 0.;
		}
		
		strncpy(buf,(char*)(hDGN->abyElem + 294),22);
		for(int i = 0;(i < 22) && (buf[i] != '_');i++);
		buf[i] = '\0';
		pPrimitive->body.reference.Index = atoi(buf);

		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNParser::ParseColorTable(){
	long nWords=0L;

	nWords = (int)(m_nElemBytes*0.5);
	short nScreenFlag=*(short*)(m_aryElem + 36);
		
	unsigned char r=0,g=0,b=0;
	for(int i=0;i < 256;i++){
		r = *(unsigned char*)(m_aryElem + i*3 + 38);
		g = *(unsigned char*)(m_aryElem + i*3 + 1 + 38);
		b = *(unsigned char*)(m_aryElem + i*3 + 2 + 38);
			
		m_aryColorTable[i][0] = r;
		m_aryColorTable[i][1] = g;
		m_aryColorTable[i][2] = b;
	}
	
	return nWords;
}

//	paramter	:
//	description	: parse dmrs linkage.
//	remarks		:
//	returns		:
void CDGNParser::ParseDMRS(CElement* pElement,CDGNFile pFile){
	assert(pElement && "pElement is NULL");
	assert(pFile    && "pFile is NULL");

	if(pElement && pFile){
		int nAttIndex = hDGN->abyElem[30] + hDGN->abyElem[31] * 256;
		int nAttrBytes= hDGN->nElemBytes - nAttIndex*2 - 32;
		int size = 0;
		int nWTF,nWTFPos = 32 + nAttIndex*2;
		PDGNDMRS pDMRS  = NULL;

		for(int index = 0;nAttrBytes > 0;){
			nWTF = hDGN->abyElem[nWTFPos];
			if(pDMRS = (PDGNDMRS)KDGNLib::AllocMemory(sizeof(DGNDMRS))){
				memcpy(&(pDMRS->hdr),hDGN->abyElem + nWTFPos,2);
				memcpy(&(pDMRS->UserID),hDGN->abyElem + nWTFPos + 2,2);
				if(pDMRS->hdr.u && nWTF){
					if(pDMRS->data.pData = (char*)CDGNLib::AllocMemory(WTF*2 - 2 + 1))
					{
						memcpy(pDMRS->data.pData,hDGN->abyElem + nWTFPos + 4,WTF*2 - 2);
						pDMRS->data.pData[WTF*2 - 2] = '\0';
						
						pPrimitive->pLinkageData = ListPush((PDGNDMRS)pPrimitive->pLinkageData,(PDGNDMRS)pDMRS);
					}else	break;
					
					nAttrBytes -= 2*WTF + 2;
					nWTFPos    += 2*WTF + 2;
				}else{
					memcpy(&(pDMRS->data.occur),hDGN->abyElem + nWTFPos + 4,4);
					pPrimitive->pLinkageData = ListPush((PDGNDMRS)pPrimitive->pLinkageData,pDMRS);
					
					nAttrBytes -= 4*2;
					nWTFPos    += 8;
				}
			}else	break;
		}
	}
}

