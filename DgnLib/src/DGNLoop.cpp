#include "..\include\DGNFile.h"
#include "..\include\DGNLoop.h"

//	parameter	:
//	description	: complex element fromed from a series of elements.
//	remarks		:
//	returns		:
long CDGNLoop::Parse(CDGNFile* pFile){
	assert(pFile && "pFile is NULL");
	long nWords=0L;

	if(pFile){
		nWords = (int)((pFile->m_nElemBytes)*0.5);

		unsigned short nElements=*((unsigned short*)(pFile->m_aryElem + 38));
		for(int i=0;i < nElements;){
			if(pFile->m_pDGNScanner->Read(pFile)){
				CElement* pElem=pFile->m_pParse->Parse(nWords,pFile);
				if(pElem){
					i += pElem->GetElements();
					(*this) += pElem;
				}
			}
		}		

		if(pPrimitive->body.pPrimitive){
			pPrimitive->hdr.volume = pPrimitive->body.pPrimitive->hdr.volume;
			for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive->next;ptr;ptr=ptr->next){
				if(pPrimitive->hdr.volume.x[LOW] > ptr->hdr.volume.x[LOW])  pPrimitive->hdr.volume.x[LOW] = ptr->hdr.volume.x[LOW];
				if(pPrimitive->hdr.volume.x[HIGH]< ptr->hdr.volume.x[HIGH]) pPrimitive->hdr.volume.x[HIGH]= ptr->hdr.volume.x[HIGH];
				if(pPrimitive->hdr.volume.y[LOW] > ptr->hdr.volume.y[LOW])  pPrimitive->hdr.volume.y[LOW] = ptr->hdr.volume.y[LOW];
				if(pPrimitive->hdr.volume.y[HIGH]< ptr->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.y[HIGH]= ptr->hdr.volume.y[HIGH];
				if(pPrimitive->hdr.volume.z[LOW] > ptr->hdr.volume.y[LOW])  pPrimitive->hdr.volume.z[LOW] = ptr->hdr.volume.z[LOW];
				if(pPrimitive->hdr.volume.z[HIGH]< ptr->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.z[HIGH]= ptr->hdr.volume.z[HIGH];
			}
		}
	}

	return nWords;
}


//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNLoop::GetElemSize(const int nDimension){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=0L;

	if(pPrimitive){
		if(SQ_LINE == pPrimitive->hdr.id){
			nSize = (3 == m_nDimension) ? 30 : 26;
		}else if(SQ_CIRCLE == pPrimitive->hdr.id){
			nSize = (3 == m_nDimension) ? 92 : 26;
		}else if(SQ_LOOP  == pPrimitive->hdr.id){
			int nElements = pPrimitive->body.pvtPoints->size();
			nSize = (3 == m_nDimension) ? nElements*6 + 19 : nElements*4 + 19;
		}else if(SQ_POINTS== pPrimitive->hdr.id){
			int nElements = pPrimitive->body.pvtPoints->size();
			nSize = (3 == m_nDimension) ? nElements*(6+8)+ 17 : nElements*(4+8)+ 17;
		}else if(SQ_COMPLEX == pPrimitive->hdr.id){
			nSize = GetComplexSize(pPrimitive);
		}else if(SQ_GROUP == pPrimitive->hdr.id){
			nSize = GetGroupSize(pPrimitive);
		}
	}

	return nSize;
}

//	parameter	:
//	description	: write Loop to the file.
//	remarks		:
//	returns		:
long CDGNLoop::Write(DGNHANDLE hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		KDGNLib* pDGNLib=KDGNLib::GetInstance();
		
		PDGNHDR  pDGNHDR = (PDGNHDR)(pFile->m_aryElem);

		if(CDGNFile::2D == pFile->m_nDimension){
			pDGNHDR->ElmHdr.type   = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words  = 44;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(pFile->m_aryElem + 28);
			pDISPHDR->attindx      = pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(pFile->m_aryElem + 36)) = nCellLength - 19;
			*(short*)(pFile->m_aryElem + 42) = 0;
		}else if(CDGNFile::3D == pFile->m_nDimension){
			pDGNHDR->ElmHdr.type    = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words   = 60;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR       = (DISPHDR*)(pFile->m_aryElem + 28);
			pDISPHDR->attindx	= pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(pFile->m_aryElem + 36)) = nCellLength - 19;
			*(short*)(pFile->m_aryElem + 42) = 0;
		}
		fwrite(pFile->m_aryElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		//fflush(hDGN->fp);
		nWords += pDGNHDR->ElmHdr.words + 2;

		CDGNElement* pElement=NULL;
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			GetElmHeader(hDGN,ptr);
			switch(ptr->hdr.id){
				case SQ_LINE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						//nWords += WriteLine(hDGN,ptr);
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_POINTS:
					nWords += WritePoints(hDGN,ptr);
					break;
				case SQ_ARC:
					if(pElement = pDGNLib->GetElementPrototype(SQ_ARC)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_ELLIPSE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						nWords += pElement->Write(hDGN,ptr);
					}
					//nWords += WriteEllipse(hDGN,ptr);
					break;
				case SQ_SHAPE:
					nWords += WriteShape(hDGN,ptr);
					break;
				case SQ_TEXT:
					nWords += WriteText(hDGN,ptr);
					break;
			}
		}
	}

	return nWords;
}

