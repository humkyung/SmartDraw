// DGNPolyline.cpp: implementation of the CDGNPointString class.
//
//////////////////////////////////////////////////////////////////////

#include "DGNLib.h"
#include "DGNPointString.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNPointString::CDGNPointString()
{
	m_nType = ENM_POLYLINE;
}

CDGNPointString::~CDGNPointString()
{

}

/**	\brief	The CDGNPointString::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNPointString::size(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(pDGNLib && pPrimitive){
		int nDimension =pDGNLib->m_DGNExport.GetDimension();

		long nSize=0;
		for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next,nSize++);
		if(pPrimitive->hdr.attr.closed) nSize++;
		if(2 == nDimension)
			nRet = nSize*(12)+ 19;
		else if(3 == nDimension)
			nRet = nSize*(14) + 19;
	}

	return nRet;
}

/**	\brief	The CDGNPointString::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T
*/
VOLUME_T CDGNPointString::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0};

	if(pPrimitive){
		varRet.x[0] = pPrimitive->body.polyline->pt.x;
		varRet.y[0] = pPrimitive->body.polyline->pt.y;
		varRet.z[0] = pPrimitive->body.polyline->pt.z;
		varRet.x[1] = pPrimitive->body.polyline->pt.x;
		varRet.y[1] = pPrimitive->body.polyline->pt.y;
		varRet.z[1] = pPrimitive->body.polyline->pt.z;
		for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next){
			if(varRet.x[0] > ptr->pt.x) varRet.x[0] = ptr->pt.x;
			if(varRet.x[1] < ptr->pt.x) varRet.x[1] = ptr->pt.x;
			if(varRet.y[0] > ptr->pt.y) varRet.y[0] = ptr->pt.y;
			if(varRet.y[1] < ptr->pt.y) varRet.y[1] = ptr->pt.y;
			if(varRet.z[0] > ptr->pt.z) varRet.z[0] = ptr->pt.z;
			if(varRet.z[1] < ptr->pt.z) varRet.z[1] = ptr->pt.z;
		}
	}

	return varRet;
}


/**	\brief	The CDGNPointString::Write function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNPointString::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && pPrimitive)
	{
		int nDimension =pDGNLib->m_DGNExport.GetDimension();

		GetHeader(hDGN,pPrimitive);
		if(ENM_2D == nDimension)
			nWords = Write2D(hDGN,pPrimitive);
		else if(ENM_3D == nDimension)
			nWords = Write3D(hDGN,pPrimitive);
		fwrite(hDGN->abyElem,2,nWords,hDGN->fp);
	}

	return nWords;
}

/**	\brief	The CDGNPointString::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNPointString::Write2D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && pPrimitive){
		const double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		long nSize=0;
		for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next,nSize++);
		if(pPrimitive->hdr.attr.closed) nSize++;
		
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;
		pDGNHDR->ElmHdr.type  = DGNT_LINE_STRING;
		pDGNHDR->ElmHdr.words = nSize*(4+8)+ 17;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		short* pNumOfLine     = (short*)(hDGN->abyElem + 36);
		*pNumOfLine           = (short)nSize;
		
		int cnt=0;
		for(PPOINT_TNODE ptr = pPrimitive->body.polyline;ptr;ptr=ptr->next)
		{
			long* l = (long*)(hDGN->abyElem + 38 + cnt);
			*l      = (long)((ptr->pt.x)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((ptr->pt.y)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
		}
		if(pPrimitive->hdr.attr.closed){
			long* l = (long*)(hDGN->abyElem + 38 + cnt);
			*l      = (long)((pPrimitive->body.polyline->pt.x)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((pPrimitive->body.polyline->pt.y)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
		}

		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

/**	\brief	The CDGNPointString::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNPointString::Write3D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && pPrimitive){
		const double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		long nSize=0;
		for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next,nSize++);
		if(pPrimitive->hdr.attr.closed) nSize++;

		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		pDGNHDR->ElmHdr.type  = DGNT_LINE_STRING;
		pDGNHDR->ElmHdr.words = nSize*(6+8) + 17;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		short* pNumOfLine     = (short*)(hDGN->abyElem + 36);
		*pNumOfLine           = (short)nSize;
		
		int cnt=0;
		for(PPOINT_TNODE ptr = pPrimitive->body.polyline;ptr;ptr=ptr->next){
			long* l = (long*)(hDGN->abyElem + 38 + cnt);
			*l      = (long)((ptr->pt.x)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((ptr->pt.y)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((ptr->pt.z)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
		}
		if(pPrimitive->hdr.attr.closed){
			long* l = (long*)(hDGN->abyElem + 38 + cnt);
			*l      = (long)((pPrimitive->body.polyline->pt.x)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((pPrimitive->body.polyline->pt.y)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
			
			l  = (long*)(hDGN->abyElem + 38 + cnt);
			*l = (long)((pPrimitive->body.polyline->pt.z)*nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			cnt += 4;
		}
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

/**	\brief	The CDGNParser::ParsePointString function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long	
*/
long CDGNPointString::Parse(PSQPRIMITIVE *ppPrimitiveList, CDGNFile *hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0L;
	
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
			
			PDGNHDR pDGNHDR   = (PDGNHDR)hDGN->abyElem;
			pPrimitive->hdr.attr.continuous = (pDGNHDR->DispHdr.props.b.h) ? 0 : 1;
			if(2 == hDGN->dimension)
			{
				pPrimitive->hdr.volume.x[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 38);
				pPrimitive->hdr.volume.x[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.x[HIGH] = pPrimitive->hdr.volume.x[LOW];
				pPrimitive->hdr.volume.y[LOW]  = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 42);
				pPrimitive->hdr.volume.y[LOW] *= hDGN->scale;
				pPrimitive->hdr.volume.y[HIGH] = pPrimitive->hdr.volume.y[LOW];
				pPrimitive->hdr.volume.z[LOW]  = pPrimitive->hdr.volume.z[HIGH] = 0.;
				unsigned short nVertices = *((unsigned short*)(hDGN->abyElem + 36));
				for(int i = 0;i < nVertices;i++){//pLineString->numverts;i++){
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
				unsigned short nVertices = *((unsigned short*)(hDGN->abyElem + 36));
				for(int i = 0;i < nVertices;i++)
				{//pLineString->numverts;i++){
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
				long nStartPos =38 + nVertices*12;
				for(int i = 0;i < nVertices;i++)
				{
					long quat[4]={0};
					
					quat[0] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*16 + nStartPos);
					//quat[0]*= hDGN->scale;
					quat[1] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*16 + nStartPos + 4);
					//quat[1]*= hDGN->scale;
					quat[2] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*16 + nStartPos + 8);
					//quat[2]*= hDGN->scale;
					quat[3] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + i*16 + nStartPos + 12);
					//quat[3]*= hDGN->scale;
				}
				PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
				if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
			}
			(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
		}
	}
	
	return nWords;
}
