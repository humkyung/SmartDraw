#include "..\include\DGNFile.h"
#include "..\include\DGNBSplineSurf.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNBSplineSurf::Parse(CDGNFile pFile){
	assert(pFile && "pFile is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	KDGNLib* pDGNLib=KDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive())){
		static double  aryKnot[102]  ={0,};
		static SQPOINT aryPoint[102] ={0,};
		static double  aryWeight[102]={0,};

		pPrimitive->hdr.id     = SQ_GROUP;

		nWords = (int)(hDGN->nElemBytes*0.5);
		long nElementWords=SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + 36);
		
		struct Bspline_surface::tagFlags* pFlags=(struct Bspline_surface::tagFlags*)(pFile->m_aryElem + 40);
		short nDegree  = pFlags->order + 1;
		bool  bRational= pFlags->rational;

		short nUPoints = *((short*)(pFile->m_aryElem + 42));
		short nUKnots  = *((short*)(pFile->m_aryElem + 44));
		short nURules  = *((short*)(pFile->m_aryElem + 46));
		short nVPoints = *((short*)(pFile->m_aryElem + 50));
		short nVKnots  = *((short*)(pFile->m_aryElem + 52));
		short nVRules  = *((short*)(pFile->m_aryElem + 54));
		short nBoundary= *((short*)(pFile->m_aryElem + 56));

		nElementWords = nElementWords - (int)(hDGN->nElemBytes*0.5) + 20;
		nWords += nElementWords;
		bool bExistKnot=false;
		while(nElementWords){
			if(m_pDGNScanner->ReadElement(hDGN)){
				nWords += (int)(hDGN->nElemBytes*0.5);
				nElementWords -= (int)(hDGN->nElemBytes*0.5);
				
				PDGNHDR pDGNHDR = (PDGNHDR)pFile->m_aryElem;
				switch(pDGNHDR->ElmHdr.type){
				case DGNT_B_SPLINE_POLE:
					ParseBSplinePole(aryPoint,pFile);
					break;
				case DGNT_B_SPLINE_KNOT:
					ParseBSplineKnot(aryKnot,pFile,nUKnots);
					bExistKnot = true;
					break;
				case DGNT_B_SPLINE_WEIGHT_FACTOR:{
					ParseBSplineWeightFactor(aryWeight,pFile,nUPoints);
								 }
					break;
				case DGNT_B_SPLINE_SURFACE_BOUNDARY:{
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

//	paramter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNBSplineSurf::ParseBSplinePole(SQPOINT* pPoints,CDGNFile pFile){
	assert(pFile && "pFile is NULL");
	assert(pPoints && "pPoints is NULL");
	long nWords=0;

	if(hDGN && pPoints){
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		PDGNHDR pDGNHDR = (PDGNHDR)pFile->m_aryElem;
		if(CDGNFile::2D == pFile->m_nDimension){
			unsigned short nVertices = *((unsigned short*)(pFile->m_aryElem + 36));
			for(int i = 0;i < nVertices;i++){
				SQPOINT pt={0};

				pt.x = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + i*8 + 38);
				pt.x*= hDGN->scale;
				pt.y = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + i*8 + 42);
				pt.y*= hDGN->scale;
				pt.z = 0.;

				pPoints[i] = pt;
			}
		}else if(CDGNFile::3D == pFile->m_nDimension){
			unsigned short nVertices = *((unsigned short*)(pFile->m_aryElem + 36));
			for(int i = 0;i < nVertices;i++){
				SQPOINT pt={0};

				pt.x = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + i*12 + 38);
				pt.x*= hDGN->scale;
				pt.y = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + i*12 + 42);
				pt.y*= hDGN->scale;
				pt.z = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + i*12 + 46);
				pt.z*= hDGN->scale;

				pPoints[i] = pt;
			}
		}
	}

	return nWords;
}

//	parameter	:
//	description	: parse B-spline knot
//	remarks		:
//	returns		:
long CDGNBSplineSurf::ParseBSplineKnot(double* pKnot,CDGNFile* pFile,const int nPoints){
	assert(pFile && "pFile is NULL");
	assert(pKnot && "pKnot is NULL");

	if(hDGN && pKnot){
		const static long nDivider=2147483648 - 1;/*2^31 - 1*/
		for(int i=0;i < nPoints;i++){
			double nKnot = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + 36 + i*4);
			nKnot   /= nDivider;
			pKnot[i] = nKnot;
		}
	}

	return 1L;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNBSplineSurf::ParseBSplineWeightFactor(double* pWeight,CDGNFile* pFile,const int nPoints){
	assert(hDGN    && "hDGN is NULL");
	assert(pWeight && "pWeight is NULL");

	if(hDGN && pWeight){
		const static long nDivider=2147483648 - 1;/*2^31 - 1*/
		for(int i=0;i < nPoints;i++){
			double nWeight = SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + 36 + i*4);
			nWeight   /= nDivider;
			pWeight[i] = nWeight;
		}
	}

	return 1L;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNBSplineSurf::ParseBSplineSurfaceBoundary(PSQPOINT pPoints,CDGNFile* pFile){
	assert(pFile && "pFile is NULL");
	short nPoints=0;

	if(pFile){
		short nBoundary=*((short*)(pFile->m_aryElem + 36));
		nPoints  =*((short*)(pFile->m_aryElem + 38));
		
		int nIndex=0;
		for(int i=0;i < nPoints;i++){
			long x=SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + nIndex + 40);
			nIndex+=4;

			long y=SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + nIndex + 40);
			nIndex+=4;

			long z=SWAP_VAX32PTR_FOR_LITTLE(pFile->m_aryElem + nIndex + 40);
			nIndex+=4;

			pPoints[i].x = x;
			pPoints[i].y = y;
			pPoints[i].z = z;
		}
	}

	return nPoints;
}

