// DGNEllipse.cpp: implementation of the CDGNEllipse class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <DGNLib.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "..\DGNParser.h"
#include "..\include\DGNEllipse.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDGNEllipse::CDGNEllipse()
{
	m_nType = ENM_ELLIPSE;
}

/**	\brief
*/
CDGNEllipse::~CDGNEllipse()
{

}

/**	\brief	The CDGNEllipse::Parse function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNEllipse::Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN){
	assert(hDGN && "hDGN is NULL");
	long nWords=0L;
	PSQPRIMITIVE pPrimitive=NULL;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = ENM_ELLIPSE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;
		nWords = (int)(hDGN->nElemBytes*0.5);

		if(2 == hDGN->dimension){
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.x), hDGN->abyElem + 56, 8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.x) );
			pPrimitive->body.ellipse.ptOrigin.x *= hDGN->scale;
			
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.y), hDGN->abyElem + 64, 8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.y) );
			pPrimitive->body.ellipse.ptOrigin.y *= hDGN->scale;

			memcpy(&(pPrimitive->body.ellipse.nAxis[0]), hDGN->abyElem + 36, 8 );
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[0]));
			pPrimitive->body.ellipse.nAxis[0] *= hDGN->scale;
			memcpy(&(pPrimitive->body.ellipse.nAxis[1]), hDGN->abyElem + 44, 8 );
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[1]));
			pPrimitive->body.ellipse.nAxis[1] *= hDGN->scale;

			pPrimitive->body.ellipse.nStartAngle = 0.;
			pPrimitive->body.ellipse.nSweepAngle = 360.;
			
			PDGNHDR pDGNHDR   = (PDGNHDR)hDGN->abyElem;
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);

			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.ellipse.ptOrigin.x - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.ellipse.ptOrigin.x + pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.ellipse.ptOrigin.y - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.ellipse.ptOrigin.y + pPrimitive->body.ellipse.nAxis[0];
		}else if(3 == hDGN->dimension){
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.x), hDGN->abyElem + 68, 8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.x));
			pPrimitive->body.ellipse.ptOrigin.x *= hDGN->scale;
			
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.y), hDGN->abyElem + 76, 8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.y));
			pPrimitive->body.ellipse.ptOrigin.y *= hDGN->scale;

			memcpy(&(pPrimitive->body.ellipse.ptOrigin.z), hDGN->abyElem + 84, 8);
			DGN2IEEEDouble( &(pPrimitive->body.ellipse.ptOrigin.z) );
			pPrimitive->body.ellipse.ptOrigin.z *= hDGN->scale;

			memcpy(&(pPrimitive->body.ellipse.nAxis[0]), hDGN->abyElem + 36, 8 );
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[0]));
			pPrimitive->body.ellipse.nAxis[0] *= hDGN->scale;
			memcpy(&(pPrimitive->body.ellipse.nAxis[1]), hDGN->abyElem + 44, 8 );
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[1]));
			pPrimitive->body.ellipse.nAxis[1] *= hDGN->scale;

			pPrimitive->body.ellipse.nStartAngle = 0.;
			pPrimitive->body.ellipse.nSweepAngle = 360.;
			
			long quat[4]={0};
			quat[0] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 52);
			quat[1] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 56);
			quat[2] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60);
			quat[3] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64);

			POINT_T pt[16]={0};
			double nStep=PI/8.;
			for(int i=0;i < 16;i++){
				pt[i].x = pPrimitive->body.ellipse.nAxis[0]*cos(i*nStep);
				pt[i].y = pPrimitive->body.ellipse.nAxis[1]*sin(i*nStep);
			}

			double w=quat[0]*hDGN->scale;
			double x=quat[1]*hDGN->scale;
			double y=quat[2]*hDGN->scale;
			double z=quat[3]*hDGN->scale;
			double l=sqrt(x*x + y*y + z*z + w*w);
			x /= l;
			y /= l;
			z /= l;
			w /= l;
			double a11=w*w + x*x - y*y - z*z;
			double a12=2*(x*y + w*z);
			double a13=2*(x*z - w*y);
			double a21=2*(x*y-w*z);
			double a22=w*w - x*x + y*y - z*z;
			double a23=2*(y*z + w*x);
			double a31=2*(x*z + w*y);
			double a32=2*(y*z - w*x);
			double a33=w*w - x*x - y*y + z*z;

			for(int i=0;i < 16;i++)
			{
				double x=pt[i].x;
				double y=pt[i].y;
				double z=pt[i].z;
				pt[i].x = a11*x + a12*y + a13*z;
				pt[i].y = a21*x + a22*y + a23*z;
				pt[i].z = a31*x + a32*y + a33*z;
			}
			
			VECTOR_T vec={0};
			CMath::GetCrossProduct(
				pt[0].x,pt[0].y,pt[0].z,
				pt[2].x,pt[2].y,pt[2].z,
				&(vec.dx),&(vec.dy),&(vec.dz));
			CMath::NormalizeVector(vec);
			pPrimitive->body.ellipse.vecNorm.dx = vec.dx;
			pPrimitive->body.ellipse.vecNorm.dy = vec.dy;
			pPrimitive->body.ellipse.vecNorm.dz = vec.dz;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(pPrimitive->body.ellipse.vecNorm,alpha,beta);
			pt[0] = CGeometry::RotateAboutXYAxis(pt[0],alpha,beta);
			double theta=CGeometry::GetRotatedAngleInXYPlane(pt[0]);
			pPrimitive->body.ellipse.nRotate = theta*180./PI;

			PDGNHDR pDGNHdr = (PDGNHDR)hDGN->abyElem;
			if(pDGNHdr->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);

			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.ellipse.ptOrigin.x - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.ellipse.ptOrigin.x + pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.ellipse.ptOrigin.y - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.ellipse.ptOrigin.y + pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.z[LOW] = pPrimitive->hdr.volume.z[HIGH] = 0.;
		}
		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	\brief	The CDGNEllipse::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNEllipse::size(PSQPRIMITIVE pPrimitive){
	long nRet=0;
	
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(pPrimitive){
		int nDimension =pDGNLib->m_DGNExport.GetDimension();

		if(2 == nDimension)
			nRet = 36;
		else if(3 == nDimension)
			nRet = 46;
	}

	return nRet;
}

/**	\brief	The CDGNEllipse::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T
*/
VOLUME_T CDGNEllipse::Volume(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	VOLUME_T varRet={0,};

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(pPrimitive && pDGNLib){
		VECTOR_T vec[4]={0.,};
		vec[0].dx-= pPrimitive->body.ellipse.nAxis[0];vec[0].dy-= pPrimitive->body.ellipse.nAxis[1];
		vec[1].dx+= pPrimitive->body.ellipse.nAxis[0];vec[1].dy-= pPrimitive->body.ellipse.nAxis[1];
		vec[2].dx+= pPrimitive->body.ellipse.nAxis[0];vec[2].dy+= pPrimitive->body.ellipse.nAxis[1];
		vec[3].dx-= pPrimitive->body.ellipse.nAxis[0];vec[3].dy+= pPrimitive->body.ellipse.nAxis[1];
		
		QUAT_T quat=GetQuaternion(pPrimitive);
		vec[0] = CGeometry::Rotate(vec[0],quat);
		vec[1] = CGeometry::Rotate(vec[1],quat);
		vec[2] = CGeometry::Rotate(vec[2],quat);
		vec[3] = CGeometry::Rotate(vec[3],quat);

		varRet.x[0] = varRet.x[1] = vec[0].dx;
		varRet.y[0] = varRet.y[1] = vec[0].dy;
		varRet.z[0] = varRet.z[1] = vec[0].dz;
		for(int i=1;i < 4;i++){
			if(varRet.x[0] > vec[i].dx) varRet.x[0] = vec[i].dx;
			if(varRet.x[1] < vec[i].dx) varRet.x[1] = vec[i].dx;
			if(varRet.y[0] > vec[i].dy) varRet.y[0] = vec[i].dy;
			if(varRet.y[1] < vec[i].dy) varRet.y[1] = vec[i].dy;
			if(varRet.z[0] > vec[i].dz) varRet.z[0] = vec[i].dz;
			if(varRet.z[1] < vec[i].dz) varRet.z[1] = vec[i].dz;
		}
		varRet.x[0] += pPrimitive->body.ellipse.ptOrigin.x;
		varRet.x[1] += pPrimitive->body.ellipse.ptOrigin.x;
		varRet.y[0] += pPrimitive->body.ellipse.ptOrigin.y;
		varRet.y[1] += pPrimitive->body.ellipse.ptOrigin.y;
		varRet.z[0] += pPrimitive->body.ellipse.ptOrigin.z;
		varRet.z[1] += pPrimitive->body.ellipse.ptOrigin.z;
	}

	return varRet;
}

/**	\brief	The CDGNEllipse::GetQuaternion function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	QUAT_T
*/
QUAT_T CDGNEllipse::GetQuaternion(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	QUAT_T ret={0.};

	if(pPrimitive){
		VECTOR_T vecAxis=pPrimitive->body.ellipse.vecNorm;
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
		ret     = quat1*quat2;
	}

	return ret;
}

/**	\brief	The CDGNEllipse::Write function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNEllipse::Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
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

/**	\brief	The CDGNEllipse::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNEllipse::Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && pPrimitive)
	{
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		pDGNHDR->ElmHdr.type  = DGNT_ELLIPSE;
		pDGNHDR->ElmHdr.words = 17 + 17;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		double* d = (double*)(hDGN->abyElem + 36);
		(*d) = (pPrimitive->body.ellipse.nAxis[0])*nSubUnit;
		(*d) = IEEE2VAX(d);
		d++;
		(*d) = (pPrimitive->body.ellipse.nAxis[1])*nSubUnit;
		(*d) = IEEE2VAX(d);
		long*   l = (long*)(hDGN->abyElem + 52);
		(*l) = 0;
		SWAP_LITTLE32_FOR_VAX(*l);
		d = (double*)(hDGN->abyElem + 56);
		(*d) = (pPrimitive->body.ellipse.ptOrigin.x)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d++;
		(*d) = (pPrimitive->body.ellipse.ptOrigin.y)*nSubUnit;
		(*d) = IEEE2VAX(d);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNEllipse::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNEllipse::Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && pPrimitive)
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
		(*d) = (pPrimitive->body.ellipse.nAxis[0])*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 44);
		(*d) = (pPrimitive->body.ellipse.nAxis[1])*nSubUnit;
		(*d) = IEEE2VAX(d);
		
		// quaternion
		VECTOR_T vecAxis = pPrimitive->body.ellipse.vecNorm;
		if(CMath::NormalizeVector(vecAxis))
		{
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
			QUAT_T quat1={0.},quat2={0.},quat3={0.};
			
			quat1.w = cos(-beta*0.5);
			quat1.x = sin(-beta*0.5)*0;
			quat1.y = sin(-beta*0.5)*1;
			quat1.z = sin(-beta*0.5)*0;
			quat2.w = cos(-alpha*0.5);
			quat2.x = sin(-alpha*0.5)*1;
			quat2.y = sin(-alpha*0.5)*0;
			quat2.z = sin(-alpha*0.5)*0;
			quat3   = quat1*quat2;
			
			long*   l = (long*)(hDGN->abyElem + 52);
			(*l) = (long)(quat3.w*2147483647);
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 56);
			(*l) = (long)(quat3.x*2147483647);
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 60);
			(*l) = (long)(quat3.y*2147483647);
			SWAP_LITTLE32_FOR_VAX(*l);
			l = (long*)(hDGN->abyElem + 64);
			(*l) = (long)(quat3.z*2147483647);
			SWAP_LITTLE32_FOR_VAX(*l);
		}
		else
		{
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
		}
		
		d = (double*)(hDGN->abyElem + 68);
		(*d) = (pPrimitive->body.ellipse.ptOrigin.x)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 76);
		(*d) = (pPrimitive->body.ellipse.ptOrigin.y)*nSubUnit;
		(*d) = IEEE2VAX(d);
		d = (double*)(hDGN->abyElem + 84);
		(*d) = (pPrimitive->body.ellipse.ptOrigin.z)*nSubUnit;
		(*d) = IEEE2VAX(d);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}
	
	return nRet;
}