// DGNCircle.cpp: implementation of the CDGNCircle class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "DGNCircle.h"
#include "DGNLib.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNCircle::CDGNCircle()
{
	m_nType = SQ_CIRCLE;
}

/**	\brief
*/
CDGNCircle::~CDGNCircle()
{

}

/**	\brief	The CDGNCircle::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNCircle::size(PSQPRIMITIVE pPrimitive){
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(pDGNLib){
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		if(2 == nDimension)
			nRet = 37;
		else if(3 == nDimension)
			nRet = 46;
	}

	return nRet;
}

/**	\brief	The CDGNCircle::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T	
*/
VOLUME_T CDGNCircle::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0,};

	if(pPrimitive){
		varRet.x[LOW] = pPrimitive->body.circle.ptOrigin.x - pPrimitive->body.circle.nRadius;
		varRet.y[LOW] = pPrimitive->body.circle.ptOrigin.y - pPrimitive->body.circle.nRadius;
		varRet.z[LOW] = 0;
		varRet.x[HIGH]= pPrimitive->body.circle.ptOrigin.x + pPrimitive->body.circle.nRadius;
		varRet.y[HIGH]= pPrimitive->body.circle.ptOrigin.y + pPrimitive->body.circle.nRadius;
		varRet.z[HIGH]= 1;
	}

	return varRet;
}
/**	\brief	The CDGNCircle::Write function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNCircle::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
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

/**	\brief	The CDGNCircle::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNCircle::Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();

		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
		pDGNHDR->ElmHdr.words = 17 + 18;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		double* d = (double*)(hDGN->abyElem + 36);
		(*d) = (pPrimitive->body.circle.nRadius)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d++;
		(*d) = (pPrimitive->body.circle.nRadius)*nSubUnit;
		(*d) = IEEE2VAX(d);
		long*   l = (long*)(hDGN->abyElem + 52);
		(*l) = 0;
		SWAP_LITTLE32_FOR_VAX(*l);
		d = (double*)(hDGN->abyElem + 56);
		(*d) = (pPrimitive->body.circle.ptOrigin.x)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d++;
		(*d) = (pPrimitive->body.circle.ptOrigin.y)*nSubUnit;
		(*d) = IEEE2VAX(d);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNCircle::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNCircle::Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();

		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
		pDGNHDR->ElmHdr.words = 16 + 28;		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		if(pPrimitive->hdr.attr.u)
		{
			int i=0;
			for(char code=((char*)(pPrimitive->pLinkageData))[i];code;code = ((char*)(pPrimitive->pLinkageData))[i])
			{
				if(FILL_ATTRIBUTE == code)
				{
					pDGNHDR->DispHdr.props.b.a = 1;
					
					short* linkage= (short*)(hDGN->abyElem + pDGNHDR->ElmHdr.words*2 + 4);
					*linkage = (short)0x1007; // linkage TRUE , wtf = 7
					linkage++;
					*linkage = (short)0x0041; // Fill option 65
					linkage++;
					*linkage = (short)0x0802;
					linkage++;
					*linkage = (short)0x0001;
					linkage++;
					*linkage = (short)((char*)pPrimitive->pLinkageData)[i + 2]; // color
					linkage++;
					*linkage = (short)0x0000;
					linkage++;
					*linkage = (short)0x0000;
					linkage++;
					*linkage = (short)0x0000;
					pDGNHDR->ElmHdr.words += 8;
					
					i+= ((char*)pPrimitive->pLinkageData)[i + 1] + 2;
				}else	break;
			}
		}
		
		double* d = (double*)(hDGN->abyElem + 36);
		(*d) = (pPrimitive->body.circle.nRadius)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 44);
		(*d) = (pPrimitive->body.circle.nRadius)*nSubUnit;
		(*d) = IEEE2VAX(d);
		
		// quaternion
		QUAT_T quat=GetQuaternion(pPrimitive);
		long*   l = (long*)(hDGN->abyElem + 52);
		(*l) = (long)(quat.w*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 56);
		(*l) = (long)(quat.x*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 60);
		(*l) = (long)(quat.y*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 64);
		(*l) = (long)(quat.z*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		d = (double*)(hDGN->abyElem + 68);
		(*d) = (pPrimitive->body.circle.ptOrigin.x)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 76);
		(*d) = (pPrimitive->body.circle.ptOrigin.y)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 84);
		(*d) = 0;//(pPrimitive->body.circle.ptOrigin.z)*m_nSubUnit;
		(*d) = IEEE2VAX(d);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNCircle::GetQuaternion function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	QUAT_T	
*/
QUAT_T CDGNCircle::GetQuaternion(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	QUAT_T ret={0.};

	if(pPrimitive){
		SQVECTOR vecAxis=pPrimitive->body.circle.vecNorm;
		CMath::NormalizeVector(vecAxis);
		
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		QUAT_T quat1={0.},quat2={0.};
		
		quat1.w = cos(-beta*0.5);
		quat1.x = sin(-beta*0.5)*0;
		quat1.y = sin(-beta*0.5)*1;
		quat1.z = sin(-beta*0.5)*0;
		quat2.w = cos(-alpha*0.5);
		quat2.x = sin(-alpha*0.5)*1;
		quat2.y = sin(-alpha*0.5)*0;
		quat2.z = sin(-alpha*0.5)*0;
		ret   = quat1*quat2;
	}

	return ret;
}