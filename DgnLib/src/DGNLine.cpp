// DGNLine.cpp: implementation of the CDGNLine class.
//
//////////////////////////////////////////////////////////////////////

#include <DGNLib.h>
#include <DGNLine.h>
#include "..\DGNParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNLine::CDGNLine()
{
	m_nType = ENM_LINE;
}

/**	\brief
*/
CDGNLine::~CDGNLine()
{

}

/**	\brief	The CDGNLine::Parse function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNLine::Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN){
	assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = ENM_LINE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);

		if(2 == hDGN->dimension)
		{
			pPrimitive->body.line.ptStart.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36);
			pPrimitive->body.line.ptStart.x*= hDGN->scale;
			pPrimitive->body.line.ptStart.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 40);
			pPrimitive->body.line.ptStart.y*= hDGN->scale;
			pPrimitive->body.line.ptStart.z = 0.;
			pPrimitive->body.line.ptEnd.x   = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 44);
			pPrimitive->body.line.ptEnd.x  *= hDGN->scale;
			pPrimitive->body.line.ptEnd.y   = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 48);
			pPrimitive->body.line.ptEnd.y  *= hDGN->scale;
			pPrimitive->body.line.ptEnd.z   = 0.;
			
			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptStart.x;
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptStart.x;
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptStart.y;
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptStart.y;
			pPrimitive->hdr.volume.z[LOW] = pPrimitive->hdr.volume.z[HIGH] = 0.;
			
			if(pPrimitive->hdr.volume.x[LOW] > pPrimitive->body.line.ptEnd.x) pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptEnd.x;
			if(pPrimitive->hdr.volume.x[HIGH]< pPrimitive->body.line.ptEnd.x) pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptEnd.x;
			if(pPrimitive->hdr.volume.y[LOW] > pPrimitive->body.line.ptEnd.y) pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptEnd.y;
			if(pPrimitive->hdr.volume.y[HIGH]< pPrimitive->body.line.ptEnd.y) pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptEnd.y;

			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
		}else if(3 == hDGN->dimension){
			pPrimitive->body.line.ptStart.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36);
			pPrimitive->body.line.ptStart.x*= hDGN->scale;
			pPrimitive->body.line.ptStart.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 40);
			pPrimitive->body.line.ptStart.y*= hDGN->scale;
			pPrimitive->body.line.ptStart.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 44);
			pPrimitive->body.line.ptStart.z*= hDGN->scale;
			pPrimitive->body.line.ptEnd.x   = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 48);
			pPrimitive->body.line.ptEnd.x  *= hDGN->scale;
			pPrimitive->body.line.ptEnd.y   = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 52);
			pPrimitive->body.line.ptEnd.y  *= hDGN->scale;
			pPrimitive->body.line.ptEnd.z   = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 56);
			pPrimitive->body.line.ptEnd.z  *= hDGN->scale;
			
			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptStart.x;
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptStart.x;
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptStart.y;
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptStart.y;
			pPrimitive->hdr.volume.z[LOW] = pPrimitive->body.line.ptStart.z;
			pPrimitive->hdr.volume.z[HIGH]= pPrimitive->body.line.ptStart.z;
			
			if(pPrimitive->hdr.volume.x[LOW] > pPrimitive->body.line.ptEnd.x) pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptEnd.x;
			if(pPrimitive->hdr.volume.x[HIGH]< pPrimitive->body.line.ptEnd.x) pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptEnd.x;
			if(pPrimitive->hdr.volume.y[LOW] > pPrimitive->body.line.ptEnd.y) pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptEnd.y;
			if(pPrimitive->hdr.volume.y[HIGH]< pPrimitive->body.line.ptEnd.y) pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptEnd.y;
			if(pPrimitive->hdr.volume.z[LOW] > pPrimitive->body.line.ptEnd.z) pPrimitive->hdr.volume.z[LOW] = pPrimitive->body.line.ptEnd.z;
			if(pPrimitive->hdr.volume.z[HIGH]< pPrimitive->body.line.ptEnd.z) pPrimitive->hdr.volume.z[HIGH]= pPrimitive->body.line.ptEnd.z;

			PDGNHDR pDGNHDR=(PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
		}
		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	\brief	The CDGNLine::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNLine::size(PSQPRIMITIVE pPrimitive){
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	long nRet=0;

	if(pDGNLib){
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		if(2 == nDimension)
			nRet = 26;
		else if(3 == nDimension)
			nRet = 30;
	}

	return nRet;
}

/**	\brief	The CDGNLine::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T
*/
VOLUME_T CDGNLine::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0,};

	if(pPrimitive && (ENM_LINE == pPrimitive->hdr.id)){
		varRet.x[0] = MIN(pPrimitive->body.line.ptStart.x,pPrimitive->body.line.ptEnd.x);
		varRet.x[1] = MAX(pPrimitive->body.line.ptStart.x,pPrimitive->body.line.ptEnd.x);
		varRet.y[0] = MIN(pPrimitive->body.line.ptStart.y,pPrimitive->body.line.ptEnd.y);
		varRet.y[1] = MAX(pPrimitive->body.line.ptStart.y,pPrimitive->body.line.ptEnd.y);
		varRet.z[0] = MIN(pPrimitive->body.line.ptStart.z,pPrimitive->body.line.ptEnd.z);
		varRet.z[1] = MAX(pPrimitive->body.line.ptStart.z,pPrimitive->body.line.ptEnd.z);
	}

	return varRet;
}

/**	\brief	The CDGNLine::Write function\n
	write line to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNLine::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		int nDimension =pDGNLib->m_DGNExport.GetDimension();

		GetHeader(hDGN,pPrimitive);
		if(2 == nDimension)
			nWords = Write2D(hDGN,pPrimitive);
		else if(3 == nDimension)
			nWords = Write3D(hDGN,pPrimitive);
		fwrite(hDGN->abyElem,2,nWords,hDGN->fp);
	}

	return nWords;
}

/**	\brief	The CDGNLine::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNLine::Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive){
		const double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		pDGNHDR->ElmHdr.type   = DGNT_LINE;
		pDGNHDR->ElmHdr.words  = 24;			
		
		DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx      = 10;
		
		long* l= (long*)(hDGN->abyElem + 36);
		*l     = (long)(m_start.x*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l) = (long)(m_start.y*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l) = (long)(m_end.x*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l) = (long)(m_end.y*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNLine::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNLine::Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive){
		const double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		pDGNHDR->ElmHdr.type    = DGNT_LINE;
		pDGNHDR->ElmHdr.words   = 28;
		pDGNHDR->ElmHdr.level   = 22;
		
		DISPHDR* pDISPHDR       = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx	= 14;
		
		long* l= (long*)(hDGN->abyElem + 36);
		*l     = (long)(pPrimitive->body.line.ptStart.x*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l) = (long)(pPrimitive->body.line.ptStart.y*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l) = (long)(pPrimitive->body.line.ptStart.z*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l)= (long)(pPrimitive->body.line.ptEnd.x*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l)= (long)(pPrimitive->body.line.ptEnd.y*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		*(++l)= (long)(pPrimitive->body.line.ptEnd.z*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**
 * Decode:
 *
 * @param hFile 
 * @return bool 
 */
bool CDGNLine::Decode(CDGNFile *hFile)
{
	assert(hFile && "hFile is NULL");
	bool bRet=false;

	if(hFile){
		CDGNElement::Decode(hFile);
		
		if(2 == hFile->dimension){
			m_start.x = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 36);
			m_start.x*= hFile->scale;
			m_start.y = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 40);
			m_start.y*= hFile->scale;
			m_start.z = 0.;
			m_end.x   = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 44);
			m_end.x  *= hFile->scale;
			m_end.y   = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 48);
			m_end.y  *= hFile->scale;
			m_end.z   = 0.;
			
			//////////////////////////////////////////////////////////////////////////
			/// calculate volume
			m_volume.SetMinPoint(CTyPoint3d(m_start.x,m_start.y,0.f));
			m_volume.SetMaxPoint(CTyPoint3d(m_start.x,m_start.y,0.f));
			CTyPoint3d ptMin = m_volume.GetMinPoint();
			CTyPoint3d ptMax = m_volume.GetMaxPoint();
			if(ptMin.x > m_end.x) ptMin.x = m_end.x;
			if(ptMin.y > m_end.y) ptMin.y = m_end.y;
			if(ptMax.x < m_end.x) ptMax.x = m_end.x;
			if(ptMax.y < m_end.y) ptMax.y = m_end.y;
			m_volume.SetMinPoint(ptMin);
			m_volume.SetMaxPoint(ptMax);
			//////////////////////////////////////////////////////////////////////////

			PDGNHDR pDGNHDR = (PDGNHDR)hFile->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) GetDMRS(hFile);
		}else if(3 == hFile->dimension){
			m_start.x = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 36);
			m_start.x*= hFile->scale;
			m_start.y = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 40);
			m_start.y*= hFile->scale;
			m_start.z = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 44);
			m_start.z*= hFile->scale;
			m_end.x   = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 48);
			m_end.x  *= hFile->scale;
			m_end.y   = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 52);
			m_end.y  *= hFile->scale;
			m_end.z   = SWAP_VAX32PTR_FOR_LITTLE(hFile->abyElem + 56);
			m_end.z  *= hFile->scale;
			
			//////////////////////////////////////////////////////////////////////////
			/// calculate volume
			m_volume.SetMinPoint(m_start);
			m_volume.SetMaxPoint(m_start);
			CTyPoint3d ptMin = m_volume.GetMinPoint();
			CTyPoint3d ptMax = m_volume.GetMaxPoint();
			if(ptMin.x > m_end.x) ptMin.x = m_end.x;
			if(ptMin.y > m_end.y) ptMin.y = m_end.y;
			if(ptMin.z > m_end.z) ptMin.z = m_end.z;
			if(ptMax.x < m_end.x) ptMax.x = m_end.x;
			if(ptMax.y < m_end.y) ptMax.y = m_end.y;
			if(ptMax.z < m_end.z) ptMax.z = m_end.z;
			m_volume.SetMinPoint(ptMin);
			m_volume.SetMaxPoint(ptMax);
			//////////////////////////////////////////////////////////////////////////

			PDGNHDR pDGNHDR=(PDGNHDR)hFile->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) GetDMRS(hFile);
		}
		
		bRet = true;
	}

	return bRet;
}
