#include <math.h>
#include <math\ITKMath.h>
#include "DGNLib.h"
#include <DGNText.h>

/**	\brief
*/
CDGNText::CDGNText(){m_nType = SQ_TEXT;};

/**	\brief
*/
CDGNText::~CDGNText(){}

/**	\brief	The CDGNText::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNText::size(PSQPRIMITIVE pPrimitive){
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	long nRet=0L;

	if(pDGNLib){
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		if(2 == nDimension)
			nRet = 41;
		else if(3 == nDimension)
			nRet = 50;
	}

	return nRet;
}

/**	\brief	The CDGNText::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T
*/
VOLUME_T CDGNText::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0,};

	if(pPrimitive && (SQ_TEXT == pPrimitive->hdr.id)){
		POINT_T pt[4]={0};

		pt[0].x = -pPrimitive->body.text.nWidth*0.5;
		pt[0].y = -pPrimitive->body.text.nHeight*0.5;
		pt[1].x = pPrimitive->body.text.nWidth*0.5;
		pt[1].y = -pPrimitive->body.text.nHeight*0.5;
		pt[2].x = pPrimitive->body.text.nWidth*0.5;
		pt[2].y = pPrimitive->body.text.nHeight*0.5;
		pt[3].x = -pPrimitive->body.text.nWidth*0.5;
		pt[3].y = pPrimitive->body.text.nHeight*0.5;
		double rad=DEG2RAD(pPrimitive->body.text.nRotate);
		for(int i=0;i < 4;i++){
			double x=cos(rad)*pt[i].x - sin(rad)*pt[i].y;
			double y=sin(rad)*pt[i].x + cos(rad)*pt[i].y;
			pt[i].x = x;
			pt[i].y = y;
		}
		
		varRet.x[LOW] = varRet.x[HIGH] = pt[0].x;
		varRet.y[LOW] = varRet.y[HIGH] = pt[0].y;
		for(int i = 1;i < 4;i++)
		{
			varRet.x[LOW]  = MIN(pt[i].x,pPrimitive->hdr.volume.x[LOW]);
			varRet.x[HIGH] = MAX(pt[i].x,pPrimitive->hdr.volume.x[HIGH]);
			varRet.y[LOW]  = MIN(pt[i].y,pPrimitive->hdr.volume.y[LOW]);
			varRet.y[HIGH] = MAX(pt[i].y,pPrimitive->hdr.volume.y[HIGH]);
		}
		varRet.x[LOW] += pPrimitive->body.text.ptOrigin.x;
		varRet.y[LOW] += pPrimitive->body.text.ptOrigin.y;
		varRet.x[HIGH]+= pPrimitive->body.text.ptOrigin.x;
		varRet.y[HIGH]+= pPrimitive->body.text.ptOrigin.y;
	}

	return varRet;
}

/**	\brief	The CDGNText::Parse function

	\param	ppPrimitive	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNText::Parse(PSQPRIMITIVE* ppPrimitive,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0L;
	
	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = SQ_TEXT;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);
		
		if(2 == hDGN->dimension)
		{
			pPrimitive->body.text.ptOrigin.x = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 50);
			pPrimitive->body.text.ptOrigin.x*= hDGN->scale;
			pPrimitive->body.text.ptOrigin.y = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 54);
			pPrimitive->body.text.ptOrigin.y*= hDGN->scale;
			
			if(pPrimitive->body.text.pFamily = (char*)CDGNLib::AllocMemory(sizeof(char)*2))
			{
				pPrimitive->body.text.pFamily[0] = hDGN->abyElem[36];
				pPrimitive->body.text.pFamily[1] = '\0';
			}
			pPrimitive->body.text.nJust = hDGN->abyElem[37];
			
			pPrimitive->body.text.nWidth  = (SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 38))*6.0;///1000.0;
			pPrimitive->body.text.nWidth *= hDGN->scale;
			pPrimitive->body.text.nHeight = (SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 42))*6.0;///1000.0;
			pPrimitive->body.text.nHeight*= hDGN->scale;
			
			int NumOfChars = hDGN->abyElem[58];
			pPrimitive->body.text.pText = (char*)CDGNLib::AllocMemory(sizeof(char)*(NumOfChars + 1));
			memcpy(pPrimitive->body.text.pText, hDGN->abyElem + 60, NumOfChars);
			pPrimitive->body.text.pText[NumOfChars] = '\0';
			
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
			
			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.text.ptOrigin.x - pPrimitive->body.text.nWidth*0.5;
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.text.ptOrigin.x + pPrimitive->body.text.nWidth*0.5;
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.text.ptOrigin.y - pPrimitive->body.text.nHeight*0.5;
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.text.ptOrigin.y + pPrimitive->body.text.nHeight*0.5;
		}
		else if(3 == hDGN->dimension)
		{
			pPrimitive->body.text.ptOrigin.x = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 62 );
			pPrimitive->body.text.ptOrigin.y = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 66 );
			pPrimitive->body.text.ptOrigin.z = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 70 );
			pPrimitive->hdr.volume.x[0] += pPrimitive->body.text.ptOrigin.x;
			pPrimitive->hdr.volume.x[1] += pPrimitive->body.text.ptOrigin.x;
			pPrimitive->hdr.volume.y[0] += pPrimitive->body.text.ptOrigin.y;
			pPrimitive->hdr.volume.y[1] += pPrimitive->body.text.ptOrigin.y;
			pPrimitive->hdr.volume.z[0] += pPrimitive->body.text.ptOrigin.z;
			pPrimitive->hdr.volume.z[1] += pPrimitive->body.text.ptOrigin.z;
			
			if(pPrimitive->body.text.pFamily = (char*)CDGNLib::AllocMemory(sizeof(char)*2))
			{
				pPrimitive->body.text.pFamily[0] = hDGN->abyElem[36];
				pPrimitive->body.text.pFamily[0] = '\0';
			}
			pPrimitive->body.text.nJust = hDGN->abyElem[37];
			
			pPrimitive->body.text.nWidth = (SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 38 ))* 6.0/1000.0;
			pPrimitive->body.text.nHeight= (SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 42 ))* 6.0/1000.0;
			
			QUAT_T quat;
			quat.w = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 46);
			quat.x = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 50);
			quat.y = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 54);
			quat.z = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 58);
			/*
			long quat[4]={0};
			quat[0] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 46);
			quat[1] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 50);
			quat[2] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 54);
			quat[3] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 58);
			*/
			double divide = sqrt(quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z);
			quat.w /= divide;
			quat.x /= divide;
			quat.y /= divide;
			quat.z /= divide;
			SQMATRIX mat={0};
			CMath::GetMatrixFromQuaternion(&mat,quat);
			SQVECTOR vecNorm={0.,0.,1.};
			double tmp[3]={0.,};
			//tmp[0]=mat.element[0][0]*vecNorm.dx+mat.element[0][1]*vecNorm.dy+mat.element[0][2]*vecNorm.dz;
			//tmp[1]=mat.element[1][0]*vecNorm.dx+mat.element[1][1]*vecNorm.dy+mat.element[1][2]*vecNorm.dz;
			//tmp[2]=mat.element[2][0]*vecNorm.dx+mat.element[2][1]*vecNorm.dy+mat.element[2][2]*vecNorm.dz;
			tmp[0]=mat.element[0][0]*vecNorm.dx+mat.element[1][0]*vecNorm.dy+mat.element[2][0]*vecNorm.dz;
			tmp[1]=mat.element[0][1]*vecNorm.dx+mat.element[1][1]*vecNorm.dy+mat.element[2][1]*vecNorm.dz;
			tmp[2]=mat.element[0][2]*vecNorm.dx+mat.element[1][2]*vecNorm.dy+mat.element[2][2]*vecNorm.dz;
			vecNorm.dx = tmp[0];vecNorm.dy = tmp[1];vecNorm.dz = tmp[2];
			pPrimitive->body.text.vecNorm = vecNorm;
			
			int NumOfChars = hDGN->abyElem[74];
			pPrimitive->body.text.pText = (char*)CDGNLib::AllocMemory(sizeof(char)*(NumOfChars + 1));
			memcpy(pPrimitive->body.text.pText, hDGN->abyElem + 76, NumOfChars);
			pPrimitive->body.text.pText[NumOfChars] = '\0';
			
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);
			
			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.text.ptOrigin.x - pPrimitive->body.text.nWidth*0.5;
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.text.ptOrigin.x + pPrimitive->body.text.nWidth*0.5;
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.text.ptOrigin.x - pPrimitive->body.text.nHeight*0.5;
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.text.ptOrigin.x + pPrimitive->body.text.nHeight*0.5;
			pPrimitive->hdr.volume.z[LOW] = pPrimitive->hdr.volume.z[HIGH] = pPrimitive->body.text.ptOrigin.z;
		}
		(*ppPrimitive) = ListPush((*ppPrimitive),pPrimitive);
	}
	
	return nWords;
}

/**	\brief	The CDGNText::Write function\n
	write text to the file\n
	when calculate element's words 1 is added to length's of text.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNText::Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.text.pText && "pPrimitive->body.text.pText is NULL");
	long nWords=0L;
	int basic_char_size = 6;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && (pPrimitive && pPrimitive->body.text.pText))
	{
		int nDim=pDGNLib->m_DGNExport.GetDimension();
		
		GetHeader(hDGN,pPrimitive);
		if(2 == nDim)
			nWords = Write2D(hDGN,pPrimitive);
		else if(3 == nDim)
			nWords = Write3D(hDGN,pPrimitive);

		fwrite(hDGN->abyElem,2,nWords,hDGN->fp);
	}

	return nWords;
}

/**	\brief	The CDGNText::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNText::Write2D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0L;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive){
		PDGNHDR  pDGNHDR = (PDGNHDR)(hDGN->abyElem);
		
		pDGNHDR->ElmHdr.type  = DGNT_TEXT;
		
		int nLen = strlen(pPrimitive->body.text.pText);
		pDGNHDR->ElmHdr.words = (((nLen+1) + 60)>>1) - 2;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		char* byte = (char*)(hDGN->abyElem + 36);
		(*byte) = 3;
		//--> justification
		byte       = (char*)(hDGN->abyElem + 37);
		switch(pPrimitive->body.text.nJust){
		case JUST_LEFT_BOT:	(*byte) = (char)0x02;break;
		case JUST_RIGHT_BOT:	(*byte) = (char)0x0e;break;
		case JUST_CENTER_CENTER:(*byte) = (char)0x07;break;
		default:(*byte) = (char)0x03;break;
		}
		//<--
		
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		int nChars=strlen(pPrimitive->body.text.pText);
		long multiplier=(long)CMath::round(nSubUnit*1000./6.,1);
		double nLengthMultiplier=(pPrimitive->body.text.nWidth/nChars)*multiplier;
		long* l = (long*)(hDGN->abyElem + 38);
		(*l)    = (long)(nLengthMultiplier);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 42);
		(*l) = (long)(pPrimitive->body.text.nHeight*multiplier);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 46);
		(*l) = (long)((pPrimitive->body.text.nRotate)*360000.0);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		POINT_T pt={0};
		double length=GetDGNTextLength(pPrimitive->body.text.pText,pPrimitive->body.text.nHeight);
		pt.x = -length*0.5;
		pt.y = -pPrimitive->body.text.nHeight*0.5;
		double rad=DEG2RAD(pPrimitive->body.text.nRotate);
		double x=cos(rad)*pt.x - sin(rad)*pt.y;
		double y=sin(rad)*pt.x + cos(rad)*pt.y;
		
		l    = (long*)(hDGN->abyElem + 50);
		(*l) = (long)((pPrimitive->body.text.ptOrigin.x + x)*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 54);
		(*l) = (long)((pPrimitive->body.text.ptOrigin.y + y)*nSubUnit);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		byte    = (char*)(hDGN->abyElem + 58);
		(*byte) = (char)nLen;
		
		byte    = (char*)(hDGN->abyElem + 59);
		(*byte) = 0; // ???
		
		byte = (char*)(hDGN->abyElem + 60);
		memcpy(byte,pPrimitive->body.text.pText,nLen);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNText::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNText::Write3D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0L;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive){
		PDGNHDR  pDGNHDR = (PDGNHDR)(hDGN->abyElem);

		pDGNHDR->ElmHdr.type  = DGNT_TEXT;

		int nLen = strlen(pPrimitive->body.text.pText);
		pDGNHDR->ElmHdr.words = (((nLen+1) + 76)>>1) - 2;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		char* byte = (char*)(hDGN->abyElem + 36);
		if(pPrimitive->body.text.pFamily)
			(*byte) = (char)atoi(pPrimitive->body.text.pFamily);
		else	(*byte) = 3;
		//--> justification
		byte       = (char*)(hDGN->abyElem + 37);
		switch(pPrimitive->body.text.nJust){
		case JUST_LEFT_BOT:	(*byte) = (char)0x02;break;
		case JUST_RIGHT_BOT:	(*byte) = (char)0x0e;break;
		case JUST_CENTER_CENTER:(*byte) = (char)0x07;break;
		default:(*byte) = (char)0x03;break;
		}
		//<--
		
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		int nChars=strlen(pPrimitive->body.text.pText);
		long  multiplier=(long)CMath::round(nSubUnit*1000./6.,1);
		long* l = (long*)(hDGN->abyElem + 38);	// length multiplier
		double nLengthMultiplier=pPrimitive->body.text.nWidth/nChars*multiplier;
		(*l)    = (long)((long)nLengthMultiplier);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 42);	// height multiplier
		(*l) = (long)(pPrimitive->body.text.nHeight*multiplier);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 46);
		double nRad = DEG2RAD(pPrimitive->body.text.nRotate);
		(*l) = (long)(2147483647*cos(nRad*0.5));
		SWAP_LITTLE32_FOR_VAX(*l);
		
		// x axis
		l    = (long*)(hDGN->abyElem + 50);
		(*l) = 0;
		SWAP_LITTLE32_FOR_VAX(*l);
		
		// y axis
		l    = (long*)(hDGN->abyElem + 54);
		(*l) = 0;
		SWAP_LITTLE32_FOR_VAX(*l);
		
		// z axis
		l    = (long*)(hDGN->abyElem + 58);
		(*l) = (long)(2147483647*sin(-nRad*0.5));
		//(*l) = 0;
		SWAP_LITTLE32_FOR_VAX(*l);
		
		POINT_T pt={0};
		double length=GetDGNTextLength(pPrimitive->body.text.pText,pPrimitive->body.text.nHeight);
		//pt.x = -length*0.5;
		//pt.y = -pPrimitive->body.text.height*0.5;
		pt.x = -pPrimitive->body.text.nWidth*0.5;
		pt.y = -pPrimitive->body.text.nHeight*0.5;
		double x=cos(nRad)*pt.x - sin(nRad)*pt.y;
		double y=sin(nRad)*pt.x + cos(nRad)*pt.y;
		if(	(JUST_LEFT_BOT == pPrimitive->body.text.nJust) || 
			(JUST_RIGHT_BOT== pPrimitive->body.text.nJust)){
			x = y = 0.;
		}
		
		l    = (long*)(hDGN->abyElem + 62);
		(*l) = (long)(pPrimitive->body.text.ptOrigin.x + x)*nSubUnit;
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 66);
		(*l) = (long)(pPrimitive->body.text.ptOrigin.y + y)*nSubUnit;
		SWAP_LITTLE32_FOR_VAX(*l);
		
		l    = (long*)(hDGN->abyElem + 70);
		(*l) = (long)(pPrimitive->body.text.ptOrigin.z);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		byte    = (char*)(hDGN->abyElem + 74);
		(*byte) = nLen;
		
		byte    = (char*)(hDGN->abyElem + 75);
		(*byte) = 0; // ???
		
		byte = (char*)(hDGN->abyElem + 76);
		memcpy(byte,pPrimitive->body.text.pText,nLen);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}