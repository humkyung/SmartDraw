#include <windows.h>
#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "include\DGNLib.h"
#include "DGNExport.h"
#include "include\DGNLib.h"

#include "DGNArc.h"
#include "DGNCone.h"
#include "DGNSolid.h"
#include "DGNSurface.h"

/**	\brief
*/
CDGNExport::CDGNExport(){
	m_nDimension= 3;
	m_nSubUnit  = 1;

	OnCreate();
}

/**	\brief
*/
CDGNExport::~CDGNExport(){
	OnDestroy();
}

/**	\brief	The CDGNExport::OnCreate function


	\return	void	
*/
void CDGNExport::OnCreate(){
}

/**	\brief	The CDGNExport::OnDestroy function


	\return	void	
*/
void CDGNExport::OnDestroy(){
}

/**	
	@brief	The CDGNExport::OnExportFile function

	@author	humkyung

	@param	pExportFilePath	a parameter of type const char*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	void
*/
void CDGNExport::OnExportFile(const char* pExportFilePath,PSQPRIMITIVE pPrimitive)
{
	assert(pExportFilePath && "pExportFilePath is NULL");
	assert(pPrimitive      && "pPrimitive is NULL");

	if(pExportFilePath && pPrimitive)
	{
		CDGNFile* hDGN=NULL;
		if(hDGN = OpenFile(pExportFilePath))
		{
			WritePrimitive(hDGN,pPrimitive);
			CloseFile(hDGN);
			free(hDGN);
		}
	}
}

/**	\brief	The CDGNExport::OpenFile function

	\param	pExportFilePath	a parameter of type const char *

	\return	CDGNFile*
*/
CDGNFile* CDGNExport::OpenFile(const char * pExportFilePath){
	assert(pExportFilePath && "pExportFilePath is NULL");
	CDGNFile* hDGN = NULL;

	if(pExportFilePath){
		if(hDGN = (CDGNFile*)malloc(sizeof(CDGNFile))){
			memset(hDGN,0,sizeof(CDGNFile));
			if(hDGN->fp = fopen(pExportFilePath,"wb")){
				GetEnvironment(hDGN,m_szSeedFilePath);
				DGNCopyFile(hDGN,m_szSeedFilePath);
				fflush(hDGN->fp);
			}else{
				free(hDGN);
				hDGN = NULL;
			}
		}
	}

	return hDGN;
}

/**	\brief	The CDGNExport::CloseFile function

	\param	hDGN	a parameter of type CDGNFile*

	\return	void
*/
void CDGNExport::CloseFile(CDGNFile* hDGN){
	assert(hDGN && "hDGN is NULL");
	const char mark = (const char)0xff;

	if(hDGN){
		putc(mark,hDGN->fp);
		putc(mark,hDGN->fp);
		fflush(hDGN->fp);
		fclose(hDGN->fp);
		hDGN->fp = NULL;
	}
}

/**	\brief	The CDGNExport::DGNCopyFile function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pFilePath	a parameter of type const char*

	\return	bool
*/
bool CDGNExport::DGNCopyFile(CDGNFile* hDGN,const char* pFilePath){
	assert(hDGN      && "hDGN is NULL");
	assert(pFilePath && "pFilePath is NULL");
	bool bRet = false;
	FILE* fp=NULL;

	if(hDGN && hDGN->fp && (fp = fopen(pFilePath,"rb"))){
		while(1){
			if(4 != fread(hDGN->abyElem,1,4,fp)) break;
			if((0xff == hDGN->abyElem[0]) && (0xff == hDGN->abyElem[1])) break;
			unsigned int nWords = hDGN->abyElem[2] + hDGN->abyElem[3]*256;
			if(nWords != fread(hDGN->abyElem + 4,2,nWords,fp)) break;
			
			//<--
			/*
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if((DGNT_GROUP_DATA == pDGNHDR->ElmHdr.type) && (1 == pDGNHDR->ElmHdr.level)){
				int i=0;
				unsigned char* color=(unsigned char*)(hDGN->abyElem + i*3 + 38);
				(*color) = 0;
				color=(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
				(*color) = 0;
				color=(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
				(*color) = 0;
				for(i=1;i < 256;i++){
					unsigned char* color=(unsigned char*)(hDGN->abyElem + i*3 + 38);
					(*color) = 0;
					color=(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
					(*color) = 0;
					color=(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
					(*color) = 255;
				}
			}
			*/
			//-->

			fwrite(hDGN->abyElem,1,nWords*2 + 4,hDGN->fp);
		}
		fclose(fp);

		bRet = true;
	}
		
	return bRet;
}

/**	\brief	The CDGNExport::GetEnvironment function\n
	get color table and uors.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pSeedFilePath	a parameter of type const char*

	\return	void
*/
void CDGNExport::GetEnvironment(CDGNFile* hDGN,const char* pSeedFilePath){
	assert(hDGN          && "hDGN is NULL");
	assert(pSeedFilePath && "pSeedFilePath is NULL");

	if(hDGN && pSeedFilePath){
		FILE* fp=fopen(pSeedFilePath,"rb");
		if(fp){
			bool bFindDimAndSubUnit=false;
			bool bFindColorTable   =false;
			while(!feof(fp) && !(bFindColorTable && bFindDimAndSubUnit)){
				if(4 != fread(hDGN->abyElem,1,4,fp)) break;
			
				// Is this an 0xFFFF endof file marker?
				if((hDGN->abyElem[0] == 0xff) && (hDGN->abyElem[1] == 0xff)) break;
			
				int nWords = hDGN->abyElem[2] + hDGN->abyElem[3]*256;
				// Read the rest of the element data into the working buffer.      
				if((unsigned)nWords != fread(hDGN->abyElem + 4,2,nWords,fp)) break;
			
				PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
				if((false == bFindDimAndSubUnit) && (DGNT_TCB == pDGNHDR->ElmHdr.type)){
					long subunits_per_master = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 1112);
					long uor_per_subunit     = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 1116);
					
					//m_nSubUnit = subunits_per_master*uor_per_subunit;
					m_nSubUnit = 1; // ?

					if(hDGN->abyElem[1214] & 0x40 )
						m_nDimension = 3;
					else	m_nDimension = 2;

					/* Get global origin */
					memcpy(&(hDGN->m_ptOrigin.x),hDGN->abyElem + 1240, 8 );
					memcpy(&(hDGN->m_ptOrigin.y),hDGN->abyElem + 1248, 8 );
					memcpy(&(hDGN->m_ptOrigin.z),hDGN->abyElem + 1256, 8 );
					
					/* Transform to IEEE */
					DGN2IEEEDouble(&(hDGN->m_ptOrigin.x));
					DGN2IEEEDouble(&(hDGN->m_ptOrigin.y));
					DGN2IEEEDouble(&(hDGN->m_ptOrigin.z));

					bFindDimAndSubUnit = true;					
				}else if((DGNT_GROUP_DATA == pDGNHDR->ElmHdr.type) && (1 == pDGNHDR->ElmHdr.level)){
					short screen_flag=*(short*)(hDGN->abyElem + 36);
					
					unsigned char r=0,g=0,b=0;
					for(int i=0;i < 256;i++){
						r = *(unsigned char*)(hDGN->abyElem + i*3 + 38);
						g = *(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
						b = *(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
						
						hDGN->color_table[i][0] = r;
						hDGN->color_table[i][1] = g;
						hDGN->color_table[i][2] = b;
					}
					bFindColorTable = true;
				}
			}
			fclose(fp);
		}
	}
}

/**	\brief	The CDGNExport::SetColorTable function

	\param	hDGN	a parameter of type CDGNFile*

	\return	void
*/
void CDGNExport::SetColorTable(CDGNFile* hDGN){
	assert(hDGN && "hDGN is NULL");

	if(hDGN){
		//long lSavePos=ftell(hDGN->fp);
		//fseek(hDGN->fp,0,SEEK_SET);
		//long lpos=ftell(hDGN->fp);
		
		long lColorTablePos=-1;
		while(!feof(hDGN->fp)){
			int nReads=fread(hDGN->abyElem,1,4,hDGN->fp);
			if(4 != nReads) break;
			
			// Is this an 0xFFFF endof file marker?
			if((hDGN->abyElem[0] == 0xff) && (hDGN->abyElem[1] == 0xff)) break;
			
			int nWords = hDGN->abyElem[2] + hDGN->abyElem[3]*256;
			
			// Read the rest of the element data into the working buffer.
			lColorTablePos       = ftell(hDGN->fp);
			if((unsigned)nWords != fread(hDGN->abyElem + 4,2,nWords,hDGN->fp)) break;
			
			PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
			if((DGNT_GROUP_DATA == pDGNHDR->ElmHdr.type) && (1 == pDGNHDR->ElmHdr.level)){
				for(int i=0;i < 256;i++){
					unsigned char* color=(unsigned char*)(hDGN->abyElem + i*3 + 38);
					(*color) = 0;
					color=(unsigned char*)(hDGN->abyElem + i*3 + 1 + 38);
					(*color) = 0;
					color=(unsigned char*)(hDGN->abyElem + i*3 + 2 + 38);
					(*color) = 255;
				}
				fseek(hDGN->fp,lColorTablePos,SEEK_SET);
				fwrite(hDGN->abyElem,2,nWords + 2,hDGN->fp);
				break;
			}
		}
		//fseek(hDGN->fp,lSavePos,SEEK_SET);
	}
}

/**	\brief	The CDGNExport::GetElmHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNExport::GetElmHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is invalid");
	assert(pPrimitive && "pPrimitive is invalid");
	bool bRet = false;

	if(hDGN && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		UpdateVolumeBox(pPrimitive);
		{
			// set DGNFILE buffer by 0.
			//memset(hDGN->abyElem,0,65540);
			if(2 == m_nDimension){
				if(pPrimitive->hdr.pLayer) pDGNHDR->ElmHdr.level = atoi(pPrimitive->hdr.pLayer);
				pDGNHDR->ElmHdr.complex= 0;
				pDGNHDR->ElmHdr.deleted= 0;

				//CDGNElement::WriteBounds(hDGN,pPrimitive->hdr.volume);
				pDGNHDR->ElmHdr.xlow   = (long)(pPrimitive->hdr.volume.x[LOW]*m_nSubUnit);
				pDGNHDR->ElmHdr.xhigh  = (long)(pPrimitive->hdr.volume.x[HIGH]*m_nSubUnit);
				pDGNHDR->ElmHdr.ylow   = (long)(pPrimitive->hdr.volume.y[LOW]*m_nSubUnit);
				pDGNHDR->ElmHdr.yhigh  = (long)(pPrimitive->hdr.volume.y[HIGH]*m_nSubUnit);
				pDGNHDR->ElmHdr.zlow   = -1;
				pDGNHDR->ElmHdr.zhigh  = -1;
				
				long* l = (long*)(hDGN->abyElem + 4);
				PDGNLONG pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-low
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-low
				l++;			// z-low
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-high
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-high
				
				DISPHDR* pDISPHDR  = (DISPHDR*)(hDGN->abyElem + 28);
				pDISPHDR->gphgrp   = 117;//pPrimitive->hdr.group;
				pDISPHDR->props.b._class = 3;
				pDISPHDR->props.b.n= 1;
				if(SQ_POINTS == pPrimitive->hdr.id){
					pDISPHDR->props.b.h = pPrimitive->hdr.attr.continuous ? 0 : 1;
				}
				pDISPHDR->symb.b.color  = pPrimitive->hdr.display.lcolor;
				pDISPHDR->symb.b.style  = pPrimitive->hdr.display.ltype;
				pDISPHDR->symb.b.weight = pPrimitive->hdr.display.lweight;
			}else if(3 == m_nDimension){
				if(pPrimitive->hdr.pLayer) pDGNHDR->ElmHdr.level  = atoi(pPrimitive->hdr.pLayer);
				pDGNHDR->ElmHdr.complex= 0;
				pDGNHDR->ElmHdr.deleted= 0;

				CDGNElement::WriteBounds(hDGN,pPrimitive->hdr.volume);
				/*
				pDGNHDR->ElmHdr.xlow   = (long)pPrimitive->hdr.volume.x[LOW]*m_nSubUnit;
				pDGNHDR->ElmHdr.xhigh  = (long)pPrimitive->hdr.volume.x[HIGH]*m_nSubUnit;
				pDGNHDR->ElmHdr.ylow   = (long)pPrimitive->hdr.volume.y[LOW]*m_nSubUnit;
				pDGNHDR->ElmHdr.yhigh  = (long)pPrimitive->hdr.volume.y[HIGH]*m_nSubUnit;
				pDGNHDR->ElmHdr.zlow   = (long)pPrimitive->hdr.volume.z[LOW]*m_nSubUnit;
				pDGNHDR->ElmHdr.zhigh  = (long)pPrimitive->hdr.volume.z[HIGH]*m_nSubUnit;
				
				long* l = (long*)(hDGN->abyElem + 4);
				PDGNLONG pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-low
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-low
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-low
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-high
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-high
				pDGNLong = (PDGNLONG)l;
				pDGNLong->lo += 32768;
				SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-high
				*/
				
				//pDGNHdr->DispHdr.gphgrp= pPrimitive->hdr.group;
				pDGNHDR->ElmHdr.complex        = (hDGN->bComponentOfComplexElem) ? 1 : 0;
				pDGNHDR->DispHdr.props.b.n     = 1;
				pDGNHDR->DispHdr.props.b._class= (RULE_CRV_T == pPrimitive->hdr.attr.type) ? 4 : 0;
				if(SQ_POINTS == pPrimitive->hdr.id){
					pDGNHDR->DispHdr.props.b.h = pPrimitive->hdr.attr.continuous ? 0 : 1;
				}else	pDGNHDR->DispHdr.props.b.h = pPrimitive->hdr.attr.h;
				pDGNHDR->DispHdr.symb.b.color = pPrimitive->hdr.display.lcolor;
				pDGNHDR->DispHdr.symb.b.style = pPrimitive->hdr.display.ltype;
				pDGNHDR->DispHdr.symb.b.weight= pPrimitive->hdr.display.lweight;
			}
		}
		bRet = true;
	}

	return bRet;
}

/**	\brief	The CDGNExport::PutTextHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNExport::PutTextHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");

	if(hDGN && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		long Length_multiplier_tmp = 2666667;
		long Height_multiplier_tmp = 3333333;
		double nScale = 0.1;

		long Length_multiplier = (long)(Length_multiplier_tmp*nScale);
		long Height_multiplier = (long)(Height_multiplier_tmp*nScale);

		int     vert_num = 4;
		long    basic_char_size = 6;
		double  length_mult = Length_multiplier*basic_char_size*0.001;
		double  height_mult = Height_multiplier*basic_char_size*0.001;
		
		long nChars    = strlen(pPrimitive->body.text.pText);
		double nWidth  = pPrimitive->body.text.nWidth;
		double nHeight = height_mult;
	
		// Quaternion To Matrix;
		SQMATRIX mat={0};
		QUAT_T quat;
		quat.w = (2147483647*cos(DEG2RAD(pPrimitive->body.text.nRotate)/2));
		quat.x = 0;
		quat.y = 0;
		quat.z = (2147483647*sin(DEG2RAD(pPrimitive->body.text.nRotate)/2));
		double divide = sqrt(quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z);
		quat.w /= divide;
		quat.x /= divide;
		quat.y /= divide;
		quat.z /= divide;
		CMath::GetMatrixFromQuaternion(&mat,quat);

		// Origin Point
		double ox=0.,oy=0.,oz=0.;
		ox = pPrimitive->body.text.ptOrigin.x;
		oy = pPrimitive->body.text.ptOrigin.y;
		oz = pPrimitive->body.text.ptOrigin.z;

		double height_offset = nHeight*0.5;
		//p1 = origin point
		POINT_T p1={0,},p2={0,};
		POINT_T p3={0,},p4={0,};
		p1.x=-pPrimitive->body.text.nWidth*0.5;
		p1.y=-pPrimitive->body.text.nHeight*0.5;
		p1.z=0.0;
		p2.x=pPrimitive->body.text.nWidth*0.5;
		p2.y=-pPrimitive->body.text.nHeight*0.5;
		p2.z=0.0;
		p3.x=pPrimitive->body.text.nWidth*0.5;
		p3.y=pPrimitive->body.text.nHeight*0.5;
		p3.z=0.;
		p4.x=-pPrimitive->body.text.nWidth*0.5;
		p4.y=pPrimitive->body.text.nHeight*0.5;
		p4.z=0.0;

		double rx1=0.,ry1=0.,rz1=0.,rx2=0.,ry2=0.,rz2=0.;
		double rx3=0.,ry3=0.,rz3=0.,rx4=0.,ry4, rz4=0.;
		POINT_T ret[4]={0};
		rx1 = (p1.x*mat.element[0][0] + p1.y*mat.element[1][0] + p1.z*mat.element[2][0] + ox) ;
		ry1 = (p1.x*mat.element[0][1] + p1.y*mat.element[1][1] + p1.z*mat.element[2][1] + oy) ;
		rz1 = (p1.x*mat.element[0][2] + p1.y*mat.element[1][2] + p1.z*mat.element[2][2] + oz) ;
		ret[0].x = rx1;
		ret[0].y = ry1;
		ret[0].z = rz1;
		
		rx2 = (p2.x*mat.element[0][0] + p2.y*mat.element[1][0] + p2.z*mat.element[2][0] + ox) ;
		ry2 = (p2.x*mat.element[0][1] + p2.y*mat.element[1][1] + p2.z*mat.element[2][1] + oy) ;
		rz2 = (p2.x*mat.element[0][2] + p2.y*mat.element[1][2] + p2.z*mat.element[2][2] + oz) ;
		ret[1].x = rx2;
		ret[1].y = ry2;
		ret[1].z = rz2;
		
		rx3 = (p3.x*mat.element[0][0] + p3.y*mat.element[1][0] + p3.z*mat.element[2][0] + ox) ;
		ry3 = (p3.x*mat.element[0][1] + p3.y*mat.element[1][1] + p3.z*mat.element[2][1] + oy) ;
		rz3 = (p3.x*mat.element[0][2] + p3.y*mat.element[1][2] + p3.z*mat.element[2][2] + oz) ;
		ret[2].x = rx3;
		ret[2].y = ry3;
		ret[2].z = rz3;
		
		rx4 = (p4.x*mat.element[0][0] + p4.y*mat.element[1][0] + p4.z*mat.element[2][0] + ox) ;
		ry4 = (p4.x*mat.element[0][1] + p4.y*mat.element[1][1] + p4.z*mat.element[2][1] + oy) ;
		rz4 = (p4.x*mat.element[0][2] + p4.y*mat.element[1][2] + p4.z*mat.element[2][2] + oz) ;
		ret[3].x = rx4;
		ret[3].y = ry4;
		ret[3].z = rz4;
			
		pPrimitive->hdr.volume.x[LOW] = ret[0].x;
		pPrimitive->hdr.volume.x[HIGH]= ret[0].x;
		pPrimitive->hdr.volume.y[LOW] = ret[0].y;
		pPrimitive->hdr.volume.y[HIGH]= ret[0].y;
		pPrimitive->hdr.volume.z[LOW] = ret[0].z;
		pPrimitive->hdr.volume.z[HIGH]= ret[0].z;
		for(int i = 0;i < 4; i++) {
			if(ret[i].x < pPrimitive->hdr.volume.x[LOW])  pPrimitive->hdr.volume.x[LOW] = ret[i].x;
			if(ret[i].x > pPrimitive->hdr.volume.x[HIGH]) pPrimitive->hdr.volume.x[HIGH]= ret[i].x;
			if(ret[i].y < pPrimitive->hdr.volume.y[LOW])  pPrimitive->hdr.volume.y[LOW] = ret[i].y;
			if(ret[i].y > pPrimitive->hdr.volume.y[HIGH]) pPrimitive->hdr.volume.y[HIGH]= ret[i].y;
			if(ret[i].z < pPrimitive->hdr.volume.z[LOW])  pPrimitive->hdr.volume.z[LOW] = ret[i].z;
			if(ret[i].z > pPrimitive->hdr.volume.z[HIGH]) pPrimitive->hdr.volume.z[HIGH]= ret[i].z;
		} 

		// store DGN Header to DGN File.
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;
		// set DGNFILE buf by 0.
		memset(hDGN->abyElem,0,65540);
		if(2 == m_nDimension){
			if(pPrimitive) pDGNHDR->ElmHdr.level = atoi(pPrimitive->hdr.pLayer);
			pDGNHDR->ElmHdr.complex= 0;
			pDGNHDR->ElmHdr.deleted= 0;
			pDGNHDR->ElmHdr.xlow   = (long)pPrimitive->hdr.volume.x[LOW];
			pDGNHDR->ElmHdr.xhigh  = (long)pPrimitive->hdr.volume.x[HIGH];
			pDGNHDR->ElmHdr.ylow   = (long)pPrimitive->hdr.volume.y[LOW];
			pDGNHDR->ElmHdr.yhigh  = (long)pPrimitive->hdr.volume.y[HIGH];
			pDGNHDR->ElmHdr.zlow   = -1;
			pDGNHDR->ElmHdr.zhigh  = -1;
			
			long* l  = (long*)(hDGN->abyElem + 4);
			PDGNLONG pDGNLong = NULL;
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-low
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-low
			l++;			// z-low
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-high
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-high
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			//pDGNHDR->DispHdr.gphgrp= pPrimitive->hdr.group;
			pDISPHDR->props.b.n    = 1;
			pDISPHDR->symb.b.color = pPrimitive->hdr.display.lcolor;
			pDISPHDR->symb.b.style = pPrimitive->hdr.display.ltype;
			pDISPHDR->symb.b.weight= pPrimitive->hdr.display.lweight;
		}else if(3 == m_nDimension){
			if(pPrimitive->hdr.pLayer) pDGNHDR->ElmHdr.level  = atoi(pPrimitive->hdr.pLayer);
			pDGNHDR->ElmHdr.complex= 0;
			pDGNHDR->ElmHdr.deleted= 0;
			pDGNHDR->ElmHdr.xlow   = (long)pPrimitive->hdr.volume.x[LOW];
			pDGNHDR->ElmHdr.xhigh  = (long)pPrimitive->hdr.volume.x[HIGH];
			pDGNHDR->ElmHdr.ylow   = (long)pPrimitive->hdr.volume.y[LOW];
			pDGNHDR->ElmHdr.yhigh  = (long)pPrimitive->hdr.volume.y[HIGH];
			pDGNHDR->ElmHdr.zlow   = (long)pPrimitive->hdr.volume.z[LOW];
			pDGNHDR->ElmHdr.zhigh  = (long)pPrimitive->hdr.volume.z[HIGH];
			
			long* l = (long*)(hDGN->abyElem + 4);
			PDGNLONG pDGNLong = NULL;
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-low
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-low
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-low
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// x-high
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// y-high
			pDGNLong = (PDGNLONG)l;
			pDGNLong->lo += 32768;
			SWAP_LITTLE32_FOR_VAX(*l);l++;	// z-high

			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->props.b.n    = 1;
			pDISPHDR->symb.b.color = pPrimitive->hdr.display.lcolor;
			pDISPHDR->symb.b.style = pPrimitive->hdr.display.ltype;
			pDISPHDR->symb.b.weight= pPrimitive->hdr.display.lweight;
		}
	}
}

/**	
	@brief	write pPrimitive to file(hDGN->fp).

	@author	humkyung

	@param	hDGN	a parameter of type CDGNFile*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	long
*/
long CDGNExport::WritePrimitive(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	if((hDGN && hDGN->fp) && pPrimitive)
	{
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		
		long nGroup=0;
		for(PSQPRIMITIVE ptr = pPrimitive;ptr;ptr = ptr->next)
		{
			switch(ptr->hdr.id){
				case ENM_LINE:
					{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);
					if(pElement)
					{
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
					     }
				break;
				case ENM_POLYLINE:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

					if(pElement){
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
						 }
					break;
				case SQ_POINTS:
					GetElmHeader(hDGN,ptr);
					nRet += WritePoints(hDGN,ptr);
					nGroup++;
				break;
				case SQ_LOOP:
					GetElmHeader(hDGN,ptr);
					nRet += WriteLoop(hDGN,ptr);
					nGroup++;
				break;
				case SQ_SHAPE:
					GetElmHeader(hDGN,ptr);
					nRet += WriteShape(hDGN,ptr);
					nGroup++;
				break;
				case ENM_ELLIPSE:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

					if(pElement){
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
						}
				break;
				case SQ_CIRCLE:{
						CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

						if(pElement){
							nRet += pElement->Write(hDGN,ptr);
							nGroup++;
						}
					       }
				break;
				case ENM_ARC:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

					if(pElement){
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
					    }
				break;
				case SQ_TEXT:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

					if(pElement){
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
					     }
				break;
				case SQ_COMPLEX:
					GetElmHeader(hDGN,ptr);
					nRet += WriteComplex(hDGN,ptr);
					nGroup++;
					break;
				case SQ_CONE:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);

					if(pElement){
						nRet += pElement->Write(hDGN,ptr);
						nGroup++;
					}
					     }
					break;
				case SQ_SOLID:{
					static CDGNSolid solid;
					solid.StoreHeader(hDGN,ptr);
					nRet += solid.Write(hDGN,ptr);
					nGroup++;
					      }
					break;
				case SQ_SURFACE:{
					static CDGNSurface surface;

					surface.StoreHeader(hDGN,ptr);
					nRet += surface.Write(hDGN,ptr);
					nGroup++;
						}
					break;
				case ENM_GROUP:
					nRet += WriteGroup(hDGN,ptr);
				break;
				default:
					if(NULL != ptr->body.pData){
						PDGNHDR pHdr=(PDGNHDR)(ptr->body.pData);
						///char szMsg[100];
						fwrite(ptr->body.pData,2,(pHdr->ElmHdr.words) + 2,hDGN->fp);
					}
					break;
			}
		}
	}

	return nRet;
}

/**	\brief	The CDGNExport::WriteLineX function\n
	write line to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long

	
*/
long CDGNExport::WriteLineX(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;

		if(2 == pPrimitive->hdr.attr.d){
			pDGNHDR->ElmHdr.type   = DGNT_LINE;
			pDGNHDR->ElmHdr.words  = 24;			
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx      = 10;
			
			long* l= (long*)(hDGN->abyElem + 36);
			*l     = (long)(pPrimitive->body.linex.ptStart.x*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l) = (long)(pPrimitive->body.linex.ptStart.y*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l) = (long)(pPrimitive->body.linex.ptEnd.x*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l) = (long)(pPrimitive->body.linex.ptEnd.y*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);

			fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
			nWords = pDGNHDR->ElmHdr.words + 2;

			// write arrow head.
			POINT_T pt={0};
			pt.x = pPrimitive->body.linex.ptEnd.x - pPrimitive->body.linex.ptStart.x;
			pt.y = pPrimitive->body.linex.ptEnd.y - pPrimitive->body.linex.ptStart.y;
			double alpha=CGeometry::GetRotatedAngleInXYPlane(pt);
			
			POINT_T ptArrow[4]={{3,0,0},{0,2,0},{-3,0,0},{0,-2,0}};
			for(int i=0;i < 4;i++){
				double x=ptArrow[i].x*cos(alpha) - ptArrow[i].y*sin(alpha);
				double y=ptArrow[i].x*sin(alpha) + ptArrow[i].y*cos(alpha);
				ptArrow[i].x = x;
				ptArrow[i].y = y;
			}
			double dx=pPrimitive->body.linex.ptEnd.x - ptArrow[0].x;
			double dy=pPrimitive->body.linex.ptEnd.y - ptArrow[0].y;
			
			PSQPRIMITIVE pArrowPrimitive=NULL;
			pArrowPrimitive = (PSQPRIMITIVE)CDGNLib::AllocMemory(sizeof(SQPRIMITIVE));
			if(pArrowPrimitive){
				pArrowPrimitive->hdr.id     = SQ_POINTS;
				pArrowPrimitive->hdr.attr.d = 2;
				pArrowPrimitive->hdr.attr.continuous = 1;

				pArrowPrimitive->body.pvtPoints = new vector<POINT_T>;
				pt.x = ptArrow[3].x + dx;
				pt.y = ptArrow[3].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[0].x + dx;
				pt.y = ptArrow[0].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[1].x + dx;
				pt.y = ptArrow[1].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[3].x + dx;
				pt.y = ptArrow[3].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);

				GetElmHeader(hDGN,pArrowPrimitive);
				nWords += WritePoints(hDGN,pArrowPrimitive);

				for(vector<POINT_T>::iterator itr=pArrowPrimitive->body.pvtPoints->begin();itr != pArrowPrimitive->body.pvtPoints->end();++itr){
					pArrowPrimitive->body.pvtPoints->erase(itr--);
				}
				dx=pPrimitive->body.linex.ptStart.x - ptArrow[2].x;
				dy=pPrimitive->body.linex.ptStart.y - ptArrow[2].y;
				pt.x = ptArrow[1].x + dx;
				pt.y = ptArrow[1].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[2].x + dx;
				pt.y = ptArrow[2].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[3].x + dx;
				pt.y = ptArrow[3].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				pt.x = ptArrow[1].x + dx;
				pt.y = ptArrow[1].y + dy;
				pArrowPrimitive->body.pvtPoints->push_back(pt);
				
				GetElmHeader(hDGN,pArrowPrimitive);
				nWords += WritePoints(hDGN,pArrowPrimitive);

				delete pArrowPrimitive->body.pvtPoints;
				free((void*)pArrowPrimitive);
			}

		}else if(3 == pPrimitive->hdr.attr.d){
			pDGNHDR->ElmHdr.type    = DGNT_LINE;
			pDGNHDR->ElmHdr.words   = 28;
			
			DISPHDR* pDISPHDR       = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx	= 14;
			
			long* l= (long*)(hDGN->abyElem + 36);
			*l     = (long)(pPrimitive->body.linex.ptStart.x*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l) = (long)(pPrimitive->body.linex.ptStart.y*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l) = (long)(pPrimitive->body.linex.ptStart.z*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l)= (long)(pPrimitive->body.linex.ptEnd.x*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l)= (long)(pPrimitive->body.linex.ptEnd.y*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);
			*(++l)= (long)(pPrimitive->body.linex.ptEnd.z*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);

			fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
			nWords = pDGNHDR->ElmHdr.words + 2;
		}
	}

	return nWords;
}

/*
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long CDGNExport::WriteArrow(,const SQLINEX& linex){
	POINT_T pt={0};

	pt.x = linex.ptEnd.x - linex.ptStart.x;
	pt.y = linex.ptEnd.y - linex.ptStart.y;
	double alpha=CGeometry::GetRotatedAngle(pt);

	POINT_T ptArrow[4]={{3,0,0},{0,2,0},{-3,0,0},{0,-2,0}};
	for(int i=0;i < 4;i++){
		double x=ptArrow[i].x*cos(alpha) - ptArrow[i].y*sin(alpha);
		double y=ptArrow[i].x*sin(alpha) + ptArrow[i].y*cos(alpha);
		ptArrow[i].x = x;
		ptArrow[i].y = y;
	}

	double dx=linex.ptEnd.x - ptArrow[0].x;
	double dy=linex.ptEnd.y - ptArrow[0].y;
	::MoveToEx(hDC,(int)(ptArrow[3].x + dx),(int)(ptArrow[3].y + dy),NULL);
	::LineTo(hDC,(int)(ptArrow[0].x + dx),(int)(ptArrow[0].y + dy));
	::LineTo(hDC,(int)(ptArrow[1].x + dx),(int)(ptArrow[1].y + dy));
	::LineTo(hDC,(int)(ptArrow[3].x + dx),(int)(ptArrow[3].y + dy));

	dx=linex.ptStart.x - ptArrow[2].x;
	dy=linex.ptStart.y - ptArrow[2].y;
	::MoveToEx(hDC,(int)(ptArrow[1].x + dx),(int)(ptArrow[1].y + dy),NULL);
	::LineTo(hDC,(int)(ptArrow[2].x + dx),(int)(ptArrow[2].y + dy));
	::LineTo(hDC,(int)(ptArrow[3].x + dx),(int)(ptArrow[3].y + dy));
	::LineTo(hDC,(int)(ptArrow[1].x + dx),(int)(ptArrow[1].y + dy));
}
*/

/**	\brief	The CDGNExport::WritePoints function\n
	write point string to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNExport::WritePoints(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.pvtPoints && "pPrimitive->body.pvtPoints is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive && pPrimitive->body.pvtPoints){
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;
		long nSize=pPrimitive->body.pvtPoints->size();

		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_POINT_STRING;
			pDGNHDR->ElmHdr.words = nSize*(4+8)+ 17;
			pDGNHDR->DispHdr.props.b.h = pPrimitive->hdr.attr.continuous ? 0 : 1;
			
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			short* pNumOfPoints   = (short*)(hDGN->abyElem + 36);
			*pNumOfPoints         = (short)nSize;

			int cnt=0;
			for(vector<POINT_T>::iterator itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(itr->x*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->y*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
			}
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_POINT_STRING;
			pDGNHDR->ElmHdr.words = nSize*(6+8) + 17;
			
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			short* pNumOfPoints   = (short*)(hDGN->abyElem + 36);
			*pNumOfPoints         = (short)nSize;
			
			int cnt=0;
			for(vector<POINT_T>::iterator itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(itr->x*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
				
				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->y*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->z*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
			}
			for(int i=0;i < nSize;i++){
				long* l= (long*)(hDGN->abyElem + 38 + cnt);
				*l     = (1*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (0*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (0*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (1*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
			}
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

/**	\brief	The CDGNExport::WriteLoop function\n
	write Loop to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::WriteLoop(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		
		PDGNHDR  pDGNHDR = (PDGNHDR)(hDGN->abyElem);

		if(2 == pPrimitive->hdr.attr.d){
			pDGNHDR->ElmHdr.type   = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words  = 44;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx      = pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(hDGN->abyElem + 36)) = nCellLength - 19;
			*(short*)(hDGN->abyElem + 42) = 0;
		}else if(3 == pPrimitive->hdr.attr.d){
			pDGNHDR->ElmHdr.type    = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words   = 60;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR       = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx	= pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(hDGN->abyElem + 36)) = nCellLength - 19;
			*(short*)(hDGN->abyElem + 42) = 0;
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		//fflush(hDGN->fp);
		nWords += pDGNHDR->ElmHdr.words + 2;

		CDGNElement* pElement=NULL;
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			GetElmHeader(hDGN,ptr);
			switch(ptr->hdr.id){
				case ENM_LINE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						//nWords += WriteLine(hDGN,ptr);
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_POINTS:
					nWords += WritePoints(hDGN,ptr);
					break;
				case ENM_ARC:
					if(pElement = pDGNLib->GetElementPrototype(ENM_ARC)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case ENM_ELLIPSE:
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
/*
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.pvtPoints && "pPrimitive->body.pvtPoints is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive && pPrimitive->body.pvtPoints){
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;

		//if(2 == pPrimitive->hdr.attr.d){
		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_LINE_STRING;
			pDGNHDR->ElmHdr.words = pPrimitive->body.pvtPoints->size()*4 + 17;
			
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			short* s = (short*)(hDGN->abyElem + 36);
			*s= (short)(pPrimitive->body.pvtPoints->size());

			int cnt=0;
			for(vector<POINT_T>::iterator itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(itr->x*m_nSubUnit);
				DGNSWAP32PTR4VAX(l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->y*m_nSubUnit);
				DGNSWAP32PTR4VAX(l);
				cnt += 4;
			}
		//}else if(3 == pPrimitive->hdr.attr.d){
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_LINE_STRING;
			pDGNHDR->ElmHdr.words = pPrimitive->body.pvtPoints->size()*6 + 17;
			
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			short* s = (short*)(hDGN->abyElem + 36);
			*s = (short)(pPrimitive->body.pvtPoints->size());
			
			int cnt=0;
			for(vector<POINT_T>::iterator itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(itr->x*m_nSubUnit);
				DGNSWAP32PTR4VAX(l);
				cnt += 4;
				
				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->y*m_nSubUnit);
				DGNSWAP32PTR4VAX(l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(itr->z*m_nSubUnit);
				DGNSWAP32PTR4VAX(l);
				cnt += 4;
			}
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
*/
}

/**	\brief	The CDGNExport::WriteShape function\n
	write shape to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::WriteShape(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.shape.list && "pPrimitive->body.shape.list is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive && pPrimitive->body.shape.list){
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		int nPoints=SizeOfList(pPrimitive->body.shape.list);

		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_SHAPE;
			pDGNHDR->ElmHdr.words = nPoints*4 + 17;
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;

			/*
			if(pPrimitive->body.shape.fcolor >= 0){
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
				*linkage = (short)pPrimitive->body.shape.fcolor; // color
				linkage++;
				*linkage = (short)0x0000;
				linkage++;
				*linkage = (short)0x0000;
				linkage++;
				*linkage = (short)0x0000;
				pDGNHDR->ElmHdr.words += 8;
			}
			*/
			short* s = (short*)(hDGN->abyElem + 36);
			*s= (short)(nPoints);

			int cnt=0;
			for(struct SQSHAPE::_list* ptr = pPrimitive->body.shape.list;ptr;ptr = ptr->next){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(ptr->point.x*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(ptr->point.y*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
			}
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_SHAPE;
			pDGNHDR->ElmHdr.words = nPoints*6 + 17;

			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			/*
			if((char)pPrimitive->body.shape.fcolor >= 0){
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
				*linkage = (short)pPrimitive->body.shape.fcolor; // color
				linkage++;
				*linkage = (short)0x0000;
				linkage++;
				*linkage = (short)0x0000;
				linkage++;
				*linkage = (short)0x0000;
				pDGNHDR->ElmHdr.words += 8;
			}
			*/
			short* s = (short*)(hDGN->abyElem + 36);
			*s = (short)(nPoints);
						
			int cnt=0;
			for(struct SQSHAPE::_list* ptr = pPrimitive->body.shape.list;ptr;ptr = ptr->next){
				long* l = (long*)(hDGN->abyElem + 38 + cnt);
				*l      = (long)(ptr->point.x*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(ptr->point.y*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;

				l  = (long*)(hDGN->abyElem + 38 + cnt);
				*l = (long)(ptr->point.z*m_nSubUnit);
				SWAP_LITTLE32_FOR_VAX(*l);
				cnt += 4;
			}
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

/**	\brief	The CDGNExport::WriteCircle function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::WriteCircle(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.shape.list && "pPrimitive->body.shape.list is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;

		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
			pDGNHDR->ElmHdr.words = 17 + 18;
			
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
			
			double* d = (double*)(hDGN->abyElem + 36);
			(*d) = (pPrimitive->body.circle.nRadius)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			d++;
			(*d) = (pPrimitive->body.circle.nRadius)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			long*   l = (long*)(hDGN->abyElem + 52);
			(*l) = 0;
			SWAP_LITTLE32_FOR_VAX(*l);
			d = (double*)(hDGN->abyElem + 56);
			(*d) = (pPrimitive->body.circle.ptOrigin.x)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			d++;
			(*d) = (pPrimitive->body.circle.ptOrigin.y)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
			pDGNHDR->ElmHdr.words = 16 + 28;		
			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;

			if(pPrimitive->hdr.attr.u){
				int i=0;
				for(char code=((char*)(pPrimitive->pLinkageData))[i];code;code = ((char*)(pPrimitive->pLinkageData))[i]){
					if(FILL_ATTRIBUTE == code){
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
			(*d) = (pPrimitive->body.circle.nRadius)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			d = (double*)(hDGN->abyElem + 44);
			(*d) = (pPrimitive->body.circle.nRadius)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			
			long*   l = (long*)(hDGN->abyElem + 52);
			(*l) = (long)2147483647;
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 56);
			(*l) = 0;
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 60);
			(*l) = 0;
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 64);
			(*l) = 0;
			SWAP_LITTLE32_FOR_VAX(*l);
			
			d = (double*)(hDGN->abyElem + 68);
			(*d) = (pPrimitive->body.circle.ptOrigin.x)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			d = (double*)(hDGN->abyElem + 76);
			(*d) = (pPrimitive->body.circle.ptOrigin.y)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
			d = (double*)(hDGN->abyElem + 84);
			(*d) = 0;//(pPrimitive->body.circle.ptOrigin.z)*m_nSubUnit;
			(*d) = IEEE2VAX(d);
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
//DEL long CDGNExport::WriteEllipse(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
//DEL 	assert(hDGN       && "hDGN is NULL");
//DEL 	assert(pPrimitive && "pPrimitive is NULL");
//DEL 	long nWords=0;
//DEL 
//DEL 	if((hDGN && hDGN->fp) && pPrimitive){
//DEL 		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
//DEL 
//DEL 		if(2 == m_nDimension){
//DEL 			pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
//DEL 			pDGNHDR->ElmHdr.words = 17 + 17;
//DEL 			
//DEL 			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
//DEL 			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
//DEL 			
//DEL 			double* d = (double*)(hDGN->abyElem + 36);
//DEL 			(*d) = (pPrimitive->body.ellipse.nAxis[0])*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			d++;
//DEL 			(*d) = (pPrimitive->body.ellipse.nAxis[1])*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			long*   l = (long*)(hDGN->abyElem + 52);
//DEL 			(*l) = 0;
//DEL 			DGNSWAP32PTR4VAX(l);
//DEL 			d = (double*)(hDGN->abyElem + 56);
//DEL 			(*d) = (pPrimitive->body.ellipse.ptOrigin.x)*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			d++;
//DEL 			(*d) = (pPrimitive->body.ellipse.ptOrigin.y)*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 		}else if(3 == m_nDimension){
//DEL 			pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
//DEL 			pDGNHDR->ElmHdr.words = 16 + 28;		
//DEL 			DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
//DEL 			pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
//DEL 
//DEL 			if(pPrimitive->hdr.attr.u){
//DEL 				int i=0;
//DEL 				for(char code=((char*)(pPrimitive->pLinkageData))[i];code;code = ((char*)(pPrimitive->pLinkageData))[i]){
//DEL 					if(FILL_ATTRIBUTE == code){
//DEL 						pDGNHDR->DispHdr.props.b.a = 1;
//DEL 						
//DEL 						short* linkage= (short*)(hDGN->abyElem + pDGNHDR->ElmHdr.words*2 + 4);
//DEL 						*linkage = (short)0x1007; // linkage TRUE , wtf = 7
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0041; // Fill option 65
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0802;
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0001;
//DEL 						linkage++;
//DEL 						*linkage = (short)((char*)pPrimitive->pLinkageData)[i + 2]; // color
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0000;
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0000;
//DEL 						linkage++;
//DEL 						*linkage = (short)0x0000;
//DEL 						pDGNHDR->ElmHdr.words += 8;
//DEL 
//DEL 						i+= ((char*)pPrimitive->pLinkageData)[i + 1] + 2;
//DEL 					}else	break;
//DEL 				}
//DEL 			}
//DEL 			
//DEL 			double* d = (double*)(hDGN->abyElem + 36);
//DEL 			(*d) = (pPrimitive->body.ellipse.nAxis[0])*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			d = (double*)(hDGN->abyElem + 44);
//DEL 			(*d) = (pPrimitive->body.ellipse.nAxis[1])*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			
//DEL 			// quaternion
//DEL 			SQVECTOR vecAxis = pPrimitive->body.ellipse.vecNorm;
//DEL 			if(CMath::NormalizeVector(vecAxis)){
//DEL 				double alpha=0.,beta=0.;
//DEL 				CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
//DEL 				QUAT_T quat1={0.},quat2={0.},quat3={0.};
//DEL 
//DEL 				quat1.w = cos(-beta*0.5);
//DEL 				quat1.x = sin(-beta*0.5)*0;
//DEL 				quat1.y = sin(-beta*0.5)*1;
//DEL 				quat1.z = sin(-beta*0.5)*0;
//DEL 				quat2.w = cos(-alpha*0.5);
//DEL 				quat2.x = sin(-alpha*0.5)*1;
//DEL 				quat2.y = sin(-alpha*0.5)*0;
//DEL 				quat2.z = sin(-alpha*0.5)*0;
//DEL 				quat3 = CMath::operator *(quat1,quat2);
//DEL 
//DEL 				long*   l = (long*)(hDGN->abyElem + 52);
//DEL 				(*l) = (long)(quat3.w*2147483647);
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 56);
//DEL 				(*l) = (long)(quat3.x*2147483647);
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 60);
//DEL 				(*l) = (long)(quat3.y*2147483647);
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 64);
//DEL 				(*l) = (long)(quat3.z*2147483647);
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 			}else{
//DEL 				long*   l = (long*)(hDGN->abyElem + 52);
//DEL 				(*l) = (long)2147483647;
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 56);
//DEL 				(*l) = 0;
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 60);
//DEL 				(*l) = 0;
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 				l = (long*)(hDGN->abyElem + 64);
//DEL 				(*l) = 0;
//DEL 				DGNSWAP32PTR4VAX(l);
//DEL 			}
//DEL 
//DEL 			d = (double*)(hDGN->abyElem + 68);
//DEL 			(*d) = (pPrimitive->body.ellipse.ptOrigin.x)*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			d = (double*)(hDGN->abyElem + 76);
//DEL 			(*d) = (pPrimitive->body.ellipse.ptOrigin.y)*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 			d = (double*)(hDGN->abyElem + 84);
//DEL 			(*d) = (pPrimitive->body.ellipse.ptOrigin.z)*m_nSubUnit;
//DEL 			(*d) = IEEE2VAX(d);
//DEL 		}
//DEL 		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
//DEL 		nWords = pDGNHDR->ElmHdr.words + 2;
//DEL 	}
//DEL 
//DEL 	return nWords;
//DEL }

/**	\brief	The GetEndPoint function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	POINT_T
*/
POINT_T GetEndPoint(PSQPRIMITIVE pPrimitive){
	POINT_T ptRet={0,};

	SQVECTOR vecAxis = pPrimitive->body.arc.vecNorm;
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);

	SQVECTOR vecStart={0.},vecEnd={0.};
	vecStart.dx=pPrimitive->body.arc.ptStart.x - pPrimitive->body.arc.ptOrigin.x;
	vecStart.dy=pPrimitive->body.arc.ptStart.y - pPrimitive->body.arc.ptOrigin.y;
	vecStart.dz=pPrimitive->body.arc.ptStart.z - pPrimitive->body.arc.ptOrigin.z;
	vecStart = CGeometry::RotateAboutXYAxis(vecStart,alpha,beta);
	vecStart.dz = 0;

	double nSweep=DEG2RAD(pPrimitive->body.arc.nSweep);
	vecEnd.dx = vecStart.dx*cos(nSweep) - vecStart.dy*sin(nSweep);
	vecEnd.dy = vecStart.dx*sin(nSweep) + vecStart.dy*cos(nSweep);
	vecEnd.dz = 0;
	
	vecEnd = CGeometry::RotateAboutYXAxis(vecEnd,-beta,-alpha);

	ptRet.x = vecEnd.dx;
	ptRet.y = vecEnd.dy;
	ptRet.z = vecEnd.dz;

	return ptRet;
}

/**	\brief	The CDGNExport::WriteText function\n
	write text to the file\n
	when calculate element's words 1 is added to length's of text.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNExport::WriteText(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.text.pText && "pPrimitive->body.text.pText is NULL");
	long nWords=0;
	int basic_char_size = 6;

	if((hDGN && hDGN->fp) && (pPrimitive && pPrimitive->body.text.pText)){
		PDGNHDR  pDGNHDR = (PDGNHDR)hDGN->abyElem;
		int len = strlen(pPrimitive->body.text.pText);

		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_TEXT;
			pDGNHDR->ElmHdr.words = (((len+1) + 60)>>1) - 2;
			
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


			int nChars=strlen(pPrimitive->body.text.pText);
			long multiplier=(long)CMath::round(m_nSubUnit*1000./6.,1);
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
			(*l) = (long)((pPrimitive->body.text.ptOrigin.x + x)*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);

			l    = (long*)(hDGN->abyElem + 54);
			(*l) = (long)((pPrimitive->body.text.ptOrigin.y + y)*m_nSubUnit);
			SWAP_LITTLE32_FOR_VAX(*l);

			byte    = (char*)(hDGN->abyElem + 58);
			(*byte) = (char)len;

			byte    = (char*)(hDGN->abyElem + 59);
			(*byte) = 0; // ???

			byte = (char*)(hDGN->abyElem + 60);
			memcpy(byte,pPrimitive->body.text.pText,len);
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type  = DGNT_TEXT;
			pDGNHDR->ElmHdr.words = (((len+1) + 76)>>1) - 2;
			
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

			int nChars=strlen(pPrimitive->body.text.pText);
			long  multiplier=(long)CMath::round(m_nSubUnit*1000./6.,1);
			long* l = (long*)(hDGN->abyElem + 38);	// length multiplier
			double nLengthMultiplier=pPrimitive->body.text.nWidth/nChars*multiplier;
			(*l)    = (long)((long)nLengthMultiplier);//pPrimitive->body.text.height*multiplier);
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
			(*l) = (long)(pPrimitive->body.text.ptOrigin.x + x)*m_nSubUnit;
			SWAP_LITTLE32_FOR_VAX(*l);

			l    = (long*)(hDGN->abyElem + 66);
			(*l) = (long)(pPrimitive->body.text.ptOrigin.y + y)*m_nSubUnit;
			SWAP_LITTLE32_FOR_VAX(*l);

			l    = (long*)(hDGN->abyElem + 70);
			(*l) = (long)(pPrimitive->body.text.ptOrigin.z);
			SWAP_LITTLE32_FOR_VAX(*l);

			byte    = (char*)(hDGN->abyElem + 74);
			(*byte) = len;

			byte    = (char*)(hDGN->abyElem + 75);
			(*byte) = 0; // ???

			byte = (char*)(hDGN->abyElem + 76);
			memcpy(byte,pPrimitive->body.text.pText,len);

			/*
			byte = (char*)(hDGN->abyElem + 76 + len);
			(*byte) = pPrimitive->body.text.pText[0];

			byte = (char*)(hDGN->abyElem + 76 + len + 1);
			(*byte) = len;

			byte = (char*)(hDGN->abyElem + 76 + len + 2);
			(*byte) = 1;

			pDGNHDR->ElmHdr.words += 2;
			*/

		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}

/**	\brief	The CDGNExport::WriteComplex function\n
	write Loop to the file.

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::WriteComplex(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		
		PDGNHDR  pDGNHDR = (PDGNHDR)(hDGN->abyElem);

		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type   = DGNT_COMPLEX_CHAIN_HEADER;
			pDGNHDR->ElmHdr.words  = 16 + 6;
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx      = pDGNHDR->ElmHdr.words - 14;

			if(pPrimitive->body.pPrimitive){
				long nSize = GetSize(pPrimitive->body.pPrimitive);
				*((unsigned short*)(hDGN->abyElem + 36)) = nSize - 19;

				int nElements=0;
				for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr=ptr->next,nElements++);
				
				*((unsigned short*)(hDGN->abyElem + 38)) = nElements;
				*(short*)(hDGN->abyElem + 40) = 0;
				*(short*)(hDGN->abyElem + 42) = 0;
				*(short*)(hDGN->abyElem + 44) = 0;
				*(short*)(hDGN->abyElem + 46) = 0;
			}
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type    = DGNT_COMPLEX_CHAIN_HEADER;
			pDGNHDR->ElmHdr.words   = 16 + 6;
			
			DISPHDR* pDISPHDR       = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx	= pDGNHDR->ElmHdr.words - 14;

			if(pPrimitive->body.pPrimitive){
				long nSize = GetSize(pPrimitive);
				*((unsigned short*)(hDGN->abyElem + 36)) = nSize - 19;

				int nElements=0;
				for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr=ptr->next,nElements++);
				
				*((unsigned short*)(hDGN->abyElem + 38)) = nElements;
				*(short*)(hDGN->abyElem + 40) = 0;
				*(short*)(hDGN->abyElem + 42) = 0;
				*(short*)(hDGN->abyElem + 44) = 0;
				*(short*)(hDGN->abyElem + 46) = 0;
			}
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords += pDGNHDR->ElmHdr.words + 2;

		CDGNElement* pElement=NULL;
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			GetElmHeader(hDGN,ptr);
			switch(ptr->hdr.id){
				case ENM_LINE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_POINTS:
					nWords += WritePoints(hDGN,ptr);
					break;
				case SQ_CIRCLE:
					nWords += WriteCircle(hDGN,ptr);
					break;
				case ENM_ARC:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case ENM_ELLIPSE:
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

/**	\brief	The CDGNExport::WriteGroup function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::WriteGroup(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive){
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if((hDGN && hDGN->fp) && pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		
		memset(hDGN->abyElem,'\0',65540);
		PDGNHDR  pDGNHDR = (PDGNHDR)(hDGN->abyElem);
		unsigned char aryLevelsOccuring[8] = {0,0,0,0,0,0,0,0};

		bool bFirstElm=true;
		for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			UpdateVolumeBox(ptr);
			if(bFirstElm){
				pPrimitive->hdr.volume = pPrimitive->body.pPrimitive->hdr.volume;
				bFirstElm = false;
			}else{
				if(pPrimitive->hdr.volume.x[0] > ptr->hdr.volume.x[0]) pPrimitive->hdr.volume.x[0] = ptr->hdr.volume.x[0];
				if(pPrimitive->hdr.volume.x[1] < ptr->hdr.volume.x[1]) pPrimitive->hdr.volume.x[1] = ptr->hdr.volume.x[1];
				if(pPrimitive->hdr.volume.y[0] > ptr->hdr.volume.y[0]) pPrimitive->hdr.volume.y[0] = ptr->hdr.volume.y[0];
				if(pPrimitive->hdr.volume.y[1] < ptr->hdr.volume.y[1]) pPrimitive->hdr.volume.y[1] = ptr->hdr.volume.y[1];
				if(pPrimitive->hdr.volume.z[0] > ptr->hdr.volume.z[0]) pPrimitive->hdr.volume.z[0] = ptr->hdr.volume.z[0];
				if(pPrimitive->hdr.volume.z[1] < ptr->hdr.volume.z[1]) pPrimitive->hdr.volume.z[1] = ptr->hdr.volume.z[1];
			}
			int nLevel=0;
			if(ptr->hdr.pLayer) nLevel = atoi(ptr->hdr.pLayer) - 1;
			aryLevelsOccuring[nLevel >> 3] |= (0x0001 << (nLevel & 0x7));
		}
		memcpy(hDGN->abyElem + 44,aryLevelsOccuring,8);
		GetElmHeader(hDGN,pPrimitive);
		
		if(2 == m_nDimension){
			pDGNHDR->ElmHdr.type   = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words  = 44;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR      = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx      = pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(hDGN->abyElem + 36)) = nCellLength - 19;
			////////////////////////////////////////////////////////
			// Radix 50 name
			AsciiToRad50(pPrimitive->hdr.szDesc,(unsigned short *)(hDGN->abyElem + 38) );
			if(strlen(pPrimitive->hdr.szDesc) > 3 )
				AsciiToRad50(pPrimitive->hdr.szDesc + 3,(unsigned short *)(hDGN->abyElem + 40));
			////////////////////////////////////////////////////////
			*(short*)(hDGN->abyElem + 42) = 0x0001;

			double nMidX=(pPrimitive->hdr.volume.x[0] + pPrimitive->hdr.volume.x[1])*0.5;
			double nMidY=(pPrimitive->hdr.volume.y[0] + pPrimitive->hdr.volume.y[1])*0.5;

			long* l = (long*)(hDGN->abyElem + 52);
			*l      = (long)(pPrimitive->hdr.volume.x[0] - nMidX);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 56);
			*l      = (long)(pPrimitive->hdr.volume.y[0] - nMidY);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 60);
			*l      = (long)(pPrimitive->hdr.volume.x[1] - nMidX);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 64);
			*l      = (long)(pPrimitive->hdr.volume.y[1] - nMidY);
			SWAP_LITTLE32_FOR_VAX(*l);

			l	= (long*)(hDGN->abyElem + 68);
			*l      = (long)(2147483648/10000);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 72);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 76);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 80);
			*l      = (long)(2147483648/10000);
			SWAP_LITTLE32_FOR_VAX(*l);

			l = (long*)(hDGN->abyElem + 84);
			*l      = (long)((pPrimitive->hdr.volume.x[0] + pPrimitive->hdr.volume.x[1])*0.5);
			SWAP_LITTLE32_FOR_VAX(*l);

			l = (long*)(hDGN->abyElem + 88);
			*l      = (long)((pPrimitive->hdr.volume.y[0] + pPrimitive->hdr.volume.y[1])*0.5);
			SWAP_LITTLE32_FOR_VAX(*l);
		}else if(3 == m_nDimension){
			pDGNHDR->ElmHdr.type       = DGNT_CELL_HEADER;
			pDGNHDR->ElmHdr.words      = 60;
			pDGNHDR->DispHdr.props.b.h = 0;
			
			DISPHDR* pDISPHDR       = (DISPHDR*)(hDGN->abyElem + 28);
			pDISPHDR->attindx	= pDGNHDR->ElmHdr.words - 14;

			long nCellLength = GetSize(pPrimitive);
			*((unsigned short*)(hDGN->abyElem + 36)) = nCellLength - 19;
			////////////////////////////////////////////////////////
			// Radix 50 name
			AsciiToRad50(pPrimitive->hdr.szDesc,(unsigned short *)(hDGN->abyElem + 38) );
			if(strlen(pPrimitive->hdr.szDesc) > 3 )
				AsciiToRad50(pPrimitive->hdr.szDesc + 3,(unsigned short *)(hDGN->abyElem + 40));
			////////////////////////////////////////////////////////
			*(short*)(hDGN->abyElem + 42) = 0x0001;

			double nMidX=(pPrimitive->hdr.volume.x[0] + pPrimitive->hdr.volume.x[1])*0.5;
			double nMidY=(pPrimitive->hdr.volume.y[0] + pPrimitive->hdr.volume.y[1])*0.5;
			double nMidZ=(pPrimitive->hdr.volume.z[0] + pPrimitive->hdr.volume.z[1])*0.5;

			long* l = (long*)(hDGN->abyElem + 52);
			*l      = (long)(pPrimitive->hdr.volume.x[0] - nMidX);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 56);
			*l      = (long)(pPrimitive->hdr.volume.y[0] - nMidY);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 60);
			*l      = (long)(pPrimitive->hdr.volume.z[0] - nMidZ);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 64);
			*l      = (long)(pPrimitive->hdr.volume.x[1] - nMidX);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 68);
			*l      = (long)(pPrimitive->hdr.volume.y[1] - nMidY);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 72);
			*l      = (long)(pPrimitive->hdr.volume.z[1] - nMidZ);
			SWAP_LITTLE32_FOR_VAX(*l);

			l	= (long*)(hDGN->abyElem + 76);
			*l      = (long)(2147483648/10000);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 80);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 84);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 88);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 92);
			*l      = (long)(2147483648/10000);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 96);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 100);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 104);
			*l      = (long)(0);
			SWAP_LITTLE32_FOR_VAX(*l);
			l	= (long*)(hDGN->abyElem + 108);
			*l      = (long)(2147483648/10000);
			SWAP_LITTLE32_FOR_VAX(*l);

			l	= (long*)(hDGN->abyElem + 112);
			*l      = (long)(nMidX);
			SWAP_LITTLE32_FOR_VAX(*l);

			l	= (long*)(hDGN->abyElem + 116);
			*l      = (long)(nMidY);
			SWAP_LITTLE32_FOR_VAX(*l);

			l	= (long*)(hDGN->abyElem + 120);
			*l      = (long)(nMidZ);
			SWAP_LITTLE32_FOR_VAX(*l);
		}
		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords += pDGNHDR->ElmHdr.words + 2;

		hDGN->bComponentOfComplexElem=1;
		CDGNElement* pElement=NULL;
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next)
		{
			GetElmHeader(hDGN,ptr);
			switch(ptr->hdr.id)
			{
				case ENM_LINE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id))
					{
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_POINTS:
					nWords += WritePoints(hDGN,ptr);
					break;
				case SQ_CIRCLE:
					{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(ptr->hdr.id);
					
					if(pElement) nWords += pElement->Write(hDGN,ptr);
					       }
					break;
				case ENM_ARC:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case ENM_ELLIPSE:
					if(pElement = pDGNLib->GetElementPrototype(ptr->hdr.id)){
						nWords += pElement->Write(hDGN,ptr);
					}
					break;
				case SQ_LOOP:
					nWords += WriteLoop(hDGN,ptr);
					break;
				case SQ_SHAPE:
					nWords += WriteShape(hDGN,ptr);
					break;
				case SQ_TEXT:
					nWords += WriteText(hDGN,ptr);
					break;
				case ENM_GROUP:
					nWords += WriteGroup(hDGN,ptr);
					break;
			}
		}
		hDGN->bComponentOfComplexElem=0;
	}

	return nWords;
}

/**	\brief	The CDGNExport::UpdateComplexVolumeBox function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNExport::UpdateComplexVolumeBox(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive && pPrimitive->body.pPrimitive)
	{
		for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr=ptr->next)
		{
			UpdateVolumeBox(ptr);
		}

		pPrimitive->hdr.volume = pPrimitive->body.pPrimitive->hdr.volume;
		for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive->next;ptr;ptr=ptr->next)
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

/**	\brief	The CDGNExport::UpdateVolumeBox function\n
	calculate element's volume.

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNExport::UpdateVolumeBox(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		if(ENM_3D == m_nDimension){
			switch(pPrimitive->hdr.id){
				case ENM_LINE:
					if(pPrimitive->body.line.ptStart.x < pPrimitive->body.line.ptEnd.x){
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptStart.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptEnd.x;
					}else{
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptEnd.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptStart.x;
					}

					if(pPrimitive->body.line.ptStart.y < pPrimitive->body.line.ptEnd.y){
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptStart.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptEnd.y;
					}else{
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptEnd.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptStart.y;
					}

					if(pPrimitive->body.line.ptStart.z < pPrimitive->body.line.ptEnd.z){
						pPrimitive->hdr.volume.z[LOW] = pPrimitive->body.line.ptStart.z;
						pPrimitive->hdr.volume.z[HIGH]= pPrimitive->body.line.ptEnd.z;
					}else{
						pPrimitive->hdr.volume.z[LOW] = pPrimitive->body.line.ptEnd.z;
						pPrimitive->hdr.volume.z[HIGH]= pPrimitive->body.line.ptStart.z;
					}
					//pPrimitive->hdr.volume.z[LOW] = 0.;
					//pPrimitive->hdr.volume.z[HIGH] = 1.;
				break;
				case ENM_POLYLINE:{
					pPrimitive->hdr.volume.x[0] = pPrimitive->body.polyline->pt.x;
					pPrimitive->hdr.volume.y[0] = pPrimitive->body.polyline->pt.y;
					pPrimitive->hdr.volume.z[0] = pPrimitive->body.polyline->pt.z;
					pPrimitive->hdr.volume.x[1] = pPrimitive->body.polyline->pt.x;
					pPrimitive->hdr.volume.y[1] = pPrimitive->body.polyline->pt.y;
					pPrimitive->hdr.volume.z[1] = pPrimitive->body.polyline->pt.z;
					for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next){
						if(pPrimitive->hdr.volume.x[0] > ptr->pt.x) pPrimitive->hdr.volume.x[0] = ptr->pt.x;
						if(pPrimitive->hdr.volume.x[1] < ptr->pt.x) pPrimitive->hdr.volume.x[1] = ptr->pt.x;
						if(pPrimitive->hdr.volume.y[0] > ptr->pt.y) pPrimitive->hdr.volume.y[0] = ptr->pt.y;
						if(pPrimitive->hdr.volume.y[1] < ptr->pt.y) pPrimitive->hdr.volume.y[1] = ptr->pt.y;
						if(pPrimitive->hdr.volume.z[0] > ptr->pt.z) pPrimitive->hdr.volume.z[0] = ptr->pt.z;
						if(pPrimitive->hdr.volume.z[1] < ptr->pt.z) pPrimitive->hdr.volume.z[1] = ptr->pt.z;
					}
						 }
					break;
				case SQ_CIRCLE:
					pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.circle.ptOrigin.x - pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.circle.ptOrigin.y - pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.z[LOW] = 0;
					pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.circle.ptOrigin.x + pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.circle.ptOrigin.y + pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.z[HIGH]= 1;
					break;
				case ENM_ELLIPSE:
					pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.ellipse.ptOrigin.x - pPrimitive->body.ellipse.nAxis[0];
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.ellipse.ptOrigin.y - pPrimitive->body.ellipse.nAxis[1];
					pPrimitive->hdr.volume.z[LOW] = pPrimitive->body.ellipse.ptOrigin.z;
					pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.ellipse.ptOrigin.x + pPrimitive->body.ellipse.nAxis[0];
					pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.ellipse.ptOrigin.y + pPrimitive->body.ellipse.nAxis[1];
					pPrimitive->hdr.volume.z[HIGH]= pPrimitive->body.ellipse.ptOrigin.z;
					break;
				case ENM_ARC:{
					CDGNElement* pElement=pDGNLib->GetElementPrototype(pPrimitive->hdr.id);
					if(pElement){
						pPrimitive->hdr.volume = pElement->Volume(pPrimitive);
					}
					    }
					break;
				case SQ_POINTS:{
					POINT_T pt={0};
					double xMin=0,xMax=0,yMin=0,yMax=0;

					pt = pPrimitive->body.pvtPoints->front();
					xMin = xMax = pt.x;
					yMin = yMax = pt.y;
					for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
						pt = (*itr);
						if(itr->x < xMin) xMin = itr->x;
						if(itr->x > xMax) xMax = itr->x;
						if(itr->y < yMin) yMin = itr->y;
						if(itr->y > yMax) yMax = itr->y;
					}
					pPrimitive->hdr.volume.x[LOW] = xMin;
					pPrimitive->hdr.volume.x[HIGH]= xMax;
					pPrimitive->hdr.volume.y[LOW] = yMin;
					pPrimitive->hdr.volume.y[HIGH]= yMax;
				}	
				break;
				case SQ_SHAPE:{
					struct SQSHAPE::_list* ptr=pPrimitive->body.shape.list;
					if(ptr){
						pPrimitive->hdr.volume.x[0] = pPrimitive->hdr.volume.x[1] = ptr->point.x;
						pPrimitive->hdr.volume.y[0] = pPrimitive->hdr.volume.y[1] = ptr->point.y;
						pPrimitive->hdr.volume.z[0] = pPrimitive->hdr.volume.z[1] = ptr->point.z;
						for(ptr = ptr->next;ptr;ptr = ptr->next){
							if(pPrimitive->hdr.volume.x[0] > ptr->point.x) pPrimitive->hdr.volume.x[0] = ptr->point.x;
							if(pPrimitive->hdr.volume.x[1] < ptr->point.x) pPrimitive->hdr.volume.x[1] = ptr->point.x;
							if(pPrimitive->hdr.volume.y[0] > ptr->point.y) pPrimitive->hdr.volume.y[0] = ptr->point.y;
							if(pPrimitive->hdr.volume.y[1] < ptr->point.y) pPrimitive->hdr.volume.y[1] = ptr->point.y;
							if(pPrimitive->hdr.volume.z[0] > ptr->point.z) pPrimitive->hdr.volume.z[0] = ptr->point.z;
							if(pPrimitive->hdr.volume.z[1] < ptr->point.z) pPrimitive->hdr.volume.z[1] = ptr->point.z;
						}
					}
					      }
					break;
				case SQ_TEXT:{
					POINT_T pt[4]={0};

					/*
					double nWidth=strlen(pPrimitive->body.text.pText)*pPrimitive->body.text.height*0.9;
					pt[0].x = -nWidth*0.5;
					pt[0].y = -pPrimitive->body.text.height*0.5;
					pt[1].x = nWidth*0.5;
					pt[1].y = -pPrimitive->body.text.height*0.5;
					pt[2].x = nWidth*0.5;
					pt[2].y = pPrimitive->body.text.height*0.5;
					pt[3].x = -nWidth*0.5;
					pt[3].y = pPrimitive->body.text.height*0.5;
					*/
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

					pPrimitive->hdr.volume.x[LOW] = pPrimitive->hdr.volume.x[HIGH] = pt[0].x;
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->hdr.volume.y[HIGH] = pt[0].y;
					for(int i = 1;i < 4;i++)
					{
						if(pt[i].x < pPrimitive->hdr.volume.x[LOW])
						{
							pPrimitive->hdr.volume.x[LOW] = pt[i].x;
						}
						if(pt[i].x > pPrimitive->hdr.volume.x[HIGH])
						{
							pPrimitive->hdr.volume.x[HIGH] = pt[i].x;
						}
						if(pt[i].y < pPrimitive->hdr.volume.y[LOW])
						{
							pPrimitive->hdr.volume.y[LOW] = pt[i].y;
						}
						if(pt[i].y > pPrimitive->hdr.volume.y[HIGH]){
							pPrimitive->hdr.volume.y[HIGH] = pt[i].y;
						}
					}
					pPrimitive->hdr.volume.x[LOW] += pPrimitive->body.text.ptOrigin.x;
					pPrimitive->hdr.volume.y[LOW] += pPrimitive->body.text.ptOrigin.y;
					pPrimitive->hdr.volume.x[HIGH]+= pPrimitive->body.text.ptOrigin.x;
					pPrimitive->hdr.volume.y[HIGH]+= pPrimitive->body.text.ptOrigin.y;
					     }
				break;
				case SQ_COMPLEX:
					UpdateComplexVolumeBox(pPrimitive);
					break;
			}
		}else if(2 == m_nDimension){
			switch(pPrimitive->hdr.id){
				case ENM_LINE:
					if(pPrimitive->body.line.ptStart.x < pPrimitive->body.line.ptEnd.x){
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptStart.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptEnd.x;
					}else{
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.line.ptEnd.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.line.ptStart.x;
					}

					if(pPrimitive->body.line.ptStart.x < pPrimitive->body.line.ptEnd.x){
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptStart.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptEnd.y;
					}else{
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.line.ptEnd.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.line.ptStart.y;
					}
				break;
				case ENM_LINEX:
					if(pPrimitive->body.linex.ptStart.x < pPrimitive->body.line.ptEnd.x){
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.linex.ptStart.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.linex.ptEnd.x;
					}else{
						pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.linex.ptEnd.x;
						pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.linex.ptStart.x;
					}

					if(pPrimitive->body.linex.ptStart.x < pPrimitive->body.linex.ptEnd.x){
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.linex.ptStart.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.linex.ptEnd.y;
					}else{
						pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.linex.ptEnd.y;
						pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.linex.ptStart.y;
					}
				break;
				case SQ_POINTS:{
					POINT_T pt={0};
					double xMin=0,xMax=0,yMin=0,yMax=0;

					pt = pPrimitive->body.pvtPoints->front();
					xMin = xMax = pt.x;
					yMin = yMax = pt.y;
					for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
						pt = (*itr);
						if(itr->x < xMin) xMin = itr->x;
						if(itr->x > xMax) xMax = itr->x;
						if(itr->y < yMin) yMin = itr->y;
						if(itr->y > yMax) yMax = itr->y;
					}
					pPrimitive->hdr.volume.x[LOW] = xMin;
					pPrimitive->hdr.volume.x[HIGH]= xMax;
					pPrimitive->hdr.volume.y[LOW] = yMin;
					pPrimitive->hdr.volume.y[HIGH]= yMax;
				}	
				break;
				case SQ_SHAPE:{
					struct SQSHAPE::_list* ptr=pPrimitive->body.shape.list;
					if(ptr){
						pPrimitive->hdr.volume.x[0] = pPrimitive->hdr.volume.x[1] = ptr->point.x;
						pPrimitive->hdr.volume.y[0] = pPrimitive->hdr.volume.y[1] = ptr->point.y;
						for(ptr = ptr->next;ptr;ptr = ptr->next){
							if(pPrimitive->hdr.volume.x[0] > ptr->point.x) pPrimitive->hdr.volume.x[0] = ptr->point.x;
							if(pPrimitive->hdr.volume.x[1] < ptr->point.x) pPrimitive->hdr.volume.x[1] = ptr->point.x;
							if(pPrimitive->hdr.volume.y[0] > ptr->point.y) pPrimitive->hdr.volume.y[0] = ptr->point.y;
							if(pPrimitive->hdr.volume.y[1] < ptr->point.y) pPrimitive->hdr.volume.y[1] = ptr->point.y;
						}
					}
					      }
					break;
				case SQ_CIRCLE:
					pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.circle.ptOrigin.x - pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.circle.ptOrigin.y - pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.circle.ptOrigin.x + pPrimitive->body.circle.nRadius;
					pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.circle.ptOrigin.y + pPrimitive->body.circle.nRadius;
				break;
				case ENM_ARC:{
					SQVECTOR vec={	pPrimitive->body.arc.ptStart.x - pPrimitive->body.arc.ptOrigin.x,
							pPrimitive->body.arc.ptStart.y - pPrimitive->body.arc.ptOrigin.y,
							pPrimitive->body.arc.ptStart.z - pPrimitive->body.arc.ptOrigin.z
					};

					double nRadius=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
					pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.arc.ptOrigin.x - nRadius;
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.arc.ptOrigin.y - nRadius;
					pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.arc.ptOrigin.x + nRadius;
					pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.arc.ptOrigin.y + nRadius;
					    }
					break;
				case ENM_ELLIPSE:
					pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.ellipse.ptOrigin.x - pPrimitive->body.ellipse.nAxis[0];
					pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.ellipse.ptOrigin.y - pPrimitive->body.ellipse.nAxis[1];
					pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.ellipse.ptOrigin.x + pPrimitive->body.ellipse.nAxis[0];
					pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.ellipse.ptOrigin.y + pPrimitive->body.ellipse.nAxis[1];
					break;
				case SQ_TEXT:{
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

					pPrimitive->hdr.volume.x[LOW] = pt[0].x;
					pPrimitive->hdr.volume.x[HIGH]= pt[0].y;
					for(int i = 1;i < 4;i++)
					{
						if(pt[i].x < pPrimitive->hdr.volume.x[LOW])
						{
							pPrimitive->hdr.volume.x[LOW] = pt[i].x;
						}
						if(pt[i].x > pPrimitive->hdr.volume.x[HIGH]){
							pPrimitive->hdr.volume.x[HIGH] = pt[i].x;
						}
						if(pt[i].y < pPrimitive->hdr.volume.y[LOW]){
							pPrimitive->hdr.volume.y[LOW] = pt[i].y;
						}
						if(pt[i].y > pPrimitive->hdr.volume.y[HIGH]){
							pPrimitive->hdr.volume.y[HIGH] = pt[i].y;
						}
					}
					pPrimitive->hdr.volume.x[LOW] += pPrimitive->body.text.ptOrigin.x;
					pPrimitive->hdr.volume.y[LOW] += pPrimitive->body.text.ptOrigin.y;
					pPrimitive->hdr.volume.x[HIGH]+= pPrimitive->body.text.ptOrigin.x;
					pPrimitive->hdr.volume.y[HIGH]+= pPrimitive->body.text.ptOrigin.y;
					     }
				break;
				case ENM_GROUP:{
					double xMin=0,xMax=0,yMin=0,yMax=0;

					PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;
					if(ptr){
						UpdateVolumeBox(ptr);
						xMin = ptr->hdr.volume.x[LOW];
						xMax = ptr->hdr.volume.x[HIGH];
						yMin = ptr->hdr.volume.y[LOW];
						yMax = ptr->hdr.volume.y[HIGH];
					}
					for(ptr=ptr->next;ptr;ptr = ptr->next){
						UpdateVolumeBox(ptr);
						if(ptr->hdr.volume.x[LOW] < xMin) xMin = ptr->hdr.volume.x[LOW];
						if(ptr->hdr.volume.x[HIGH]> xMax) xMax = ptr->hdr.volume.x[HIGH];
						if(ptr->hdr.volume.y[LOW] < yMin) yMin = ptr->hdr.volume.y[LOW];
						if(ptr->hdr.volume.y[HIGH]> yMax) yMax = ptr->hdr.volume.y[HIGH];
					}
					pPrimitive->hdr.volume.x[LOW] = xMin;
					pPrimitive->hdr.volume.x[HIGH]= xMax;
					pPrimitive->hdr.volume.y[LOW] = yMin;
					pPrimitive->hdr.volume.y[HIGH]= yMax;
					      }
				break;
				case SQ_COMPLEX:
					UpdateComplexVolumeBox(pPrimitive);
					break;
			}
		}
	}
}

/**	\brief	The CDGNExport::GetSize function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::GetSize(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=0;

	if(pPrimitive){
		if(ENM_LINE == pPrimitive->hdr.id){
			nSize = (ENM_3D == m_nDimension) ? SIZE_OF_LINE : 26;
		}else if(SQ_CIRCLE == pPrimitive->hdr.id){
			nSize = (ENM_3D == m_nDimension) ? 92 : 26;
		}else if(SQ_LOOP  == pPrimitive->hdr.id){
			int nElements = pPrimitive->body.pvtPoints->size();
			nSize = (ENM_3D == m_nDimension) ? nElements*6 + 19 : nElements*4 + 19;
		}else if(SQ_POINTS== pPrimitive->hdr.id){
			int nElements = pPrimitive->body.pvtPoints->size();
			nSize = (ENM_3D == m_nDimension) ? nElements*(6+8)+ 17 : nElements*(4+8)+ 17;
		}else if(SQ_SHAPE == pPrimitive->hdr.id){
			int nPoints = SizeOfList(pPrimitive->body.shape.list);
			nSize = (ENM_3D == m_nDimension) ? nPoints*6 + 17 + 2 : nPoints*4 + 17 + 2;
			if(pPrimitive->body.shape.fcolor >= 0) nSize += 8;
		}else if(SQ_TEXT == pPrimitive->hdr.id){
			int nLen=strlen(pPrimitive->body.text.pText);
			nSize = (ENM_3D == m_nDimension) ? ((nLen+1) + 76)>>1 : ((nLen+1) + 60)>>1;
		}else if(SQ_COMPLEX == pPrimitive->hdr.id){
			nSize = GetComplexSize(pPrimitive);
		}else if(ENM_GROUP == pPrimitive->hdr.id){
			nSize = GetGroupSize(pPrimitive);
		}
	}

	return nSize;
}

/**	\brief	The CDGNExport::GetGroupSize function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::GetGroupSize(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=0;

	if(pPrimitive){
		nSize = (3 == m_nDimension) ? 62 : 46;
		for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			nSize += GetSize(ptr);
		}
	}

	return nSize;
}

/**	\brief	The CDGNExport::GetComplexSize function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNExport::GetComplexSize(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	long nSize=0;

	if(pPrimitive){
		nSize = 24;
		for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
			nSize += GetSize(ptr);
		}
	}

	return nSize;
}

/**	\brief	The CDGNExport::AsciiToRad50 function

	\param	str	a parameter of type const char *
	\param	pRad50	a parameter of type unsigned short *

	\return	void
*/
void CDGNExport::AsciiToRad50( const char *str, unsigned short *pRad50 ){
	unsigned short rad50 = 0;
	
	for(int i = 0; i < 3; i++ ){
		unsigned short value;
		
		if( i >= (int) strlen(str) ){
			rad50 = rad50 * 40;
			continue;
		}
		
		if( str[i] == '$' )
			value = 27;
		else if( str[i] == '.' )
			value = 28;
		else if( str[i] == ' ' )
			value = 29;
		else if( str[i] >= '0' && str[i] <= '9' )
			value = str[i] - '0' + 30;
		else if( str[i] >= 'a' && str[i] <= 'z' )
			value = str[i] - 'a' + 1;
		else if( str[i] >= 'A' && str[i] <= 'Z' )
			value = str[i] - 'A' + 1;
		else
			value = 0;
		
		rad50 = rad50 * 40 + value;
	}
	
	*pRad50 = rad50;
}