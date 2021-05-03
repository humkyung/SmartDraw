#include "..\include\DGNDimension.h"

long CDGNDimension::Parse(CDGNFile* pFile){
assert(hDGN && "hDGN is NULL");
	long nWords=0;

	PSQPRIMITIVE pPrimitive=NULL;
	KDGNLib* pDGNLib=KDGNLib::GetInstance();
	if(hDGN/* && (pPrimitive = pDGNLib->NewPrimitive())*/){
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

long CDGNDimension::Write(CDGNFile* pFile){
	assert(pFile && "pFile is NULL");

	if(pFile){
	}
}
