// DGNCone.cpp: implementation of the CDGNCone class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <DGNCone.h>
#include "..\DGNParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDGNCone::CDGNCone()
{
	m_nType = SQ_CONE;
}

CDGNCone::~CDGNCone()
{

}

/**	\brief	The CDGNCone::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNCone::size(PSQPRIMITIVE pPrimitive)
{
	return 59;
}

/**	\brief	The CDGNCone::CalculateVolumeBox function

	\param	volume	a parameter of type SQVOLUME&
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNCone::CalculateVolumeBox(SQVOLUME& volume,PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		VECTOR_T vec=
		{
			pPrimitive->body.cone.ptOrigin[0].x - pPrimitive->body.cone.ptOrigin[1].x,
			pPrimitive->body.cone.ptOrigin[0].y - pPrimitive->body.cone.ptOrigin[1].y,
			pPrimitive->body.cone.ptOrigin[0].z - pPrimitive->body.cone.ptOrigin[1].z
		};

		CMath::NormalizeVector(vec);
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vec,alpha,beta);

		VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.};
		CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
		CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);

		POINT_T ptEdge[8]={0.,};
		ptEdge[0] = pPrimitive->body.cone.ptOrigin[0];
		ptEdge[0].x += vecX.dx*pPrimitive->body.cone.nRadius[0];
		ptEdge[0].y += vecX.dy*pPrimitive->body.cone.nRadius[0];
		ptEdge[0].z += vecX.dz*pPrimitive->body.cone.nRadius[0];
		ptEdge[1] = pPrimitive->body.cone.ptOrigin[0];
		ptEdge[1].x -= vecX.dx*pPrimitive->body.cone.nRadius[0];
		ptEdge[1].y -= vecX.dy*pPrimitive->body.cone.nRadius[0];
		ptEdge[1].z -= vecX.dz*pPrimitive->body.cone.nRadius[0];
		ptEdge[2] = pPrimitive->body.cone.ptOrigin[0];
		ptEdge[2].x += vecY.dx*pPrimitive->body.cone.nRadius[0];
		ptEdge[2].y += vecY.dy*pPrimitive->body.cone.nRadius[0];
		ptEdge[2].z += vecY.dz*pPrimitive->body.cone.nRadius[0];
		ptEdge[3] = pPrimitive->body.cone.ptOrigin[0];
		ptEdge[3].x -= vecY.dx*pPrimitive->body.cone.nRadius[0];
		ptEdge[3].y -= vecY.dy*pPrimitive->body.cone.nRadius[0];
		ptEdge[3].z -= vecY.dz*pPrimitive->body.cone.nRadius[0];

		ptEdge[4] = pPrimitive->body.cone.ptOrigin[1];
		ptEdge[4].x += vecX.dx*pPrimitive->body.cone.nRadius[1];
		ptEdge[4].y += vecX.dy*pPrimitive->body.cone.nRadius[1];
		ptEdge[4].z += vecX.dz*pPrimitive->body.cone.nRadius[1];
		ptEdge[5] = pPrimitive->body.cone.ptOrigin[1];
		ptEdge[5].x -= vecX.dx*pPrimitive->body.cone.nRadius[1];
		ptEdge[5].y -= vecX.dy*pPrimitive->body.cone.nRadius[1];
		ptEdge[5].z -= vecX.dz*pPrimitive->body.cone.nRadius[1];
		ptEdge[6] = pPrimitive->body.cone.ptOrigin[1];
		ptEdge[6].x += vecY.dx*pPrimitive->body.cone.nRadius[1];
		ptEdge[6].y += vecY.dy*pPrimitive->body.cone.nRadius[1];
		ptEdge[6].z += vecY.dz*pPrimitive->body.cone.nRadius[1];
		ptEdge[7] = pPrimitive->body.cone.ptOrigin[1];
		ptEdge[7].x -= vecY.dx*pPrimitive->body.cone.nRadius[1];
		ptEdge[7].y -= vecY.dy*pPrimitive->body.cone.nRadius[1];
		ptEdge[7].z -= vecY.dz*pPrimitive->body.cone.nRadius[1];

		volume.x[0] = volume.x[1] = ptEdge[0].x;
		volume.y[0] = volume.y[1] = ptEdge[0].y;
		volume.z[0] = volume.z[1] = ptEdge[0].z;

		for(int i=1;i < 8;i++)
		{
			if(volume.x[0] > ptEdge[i].x) volume.x[0] = ptEdge[i].x;
			if(volume.x[1] < ptEdge[i].x) volume.x[1] = ptEdge[i].x;
			if(volume.y[0] > ptEdge[i].y) volume.y[0] = ptEdge[i].y;
			if(volume.y[1] < ptEdge[i].y) volume.y[1] = ptEdge[i].y;
			if(volume.z[0] > ptEdge[i].z) volume.z[0] = ptEdge[i].z;
			if(volume.z[1] < ptEdge[i].z) volume.z[1] = ptEdge[i].z;
		}
	}
}

/**	\brief	The CDGNCone::StoreHeader function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool
*/
bool CDGNCone::StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is invalid");
	assert(pPrimitive && "pPrimitive is invalid");
	bool bRet = false;

	if(hDGN && pPrimitive)
	{
		PDGNHDR  pDGNHdr = (PDGNHDR)hDGN->abyElem;

		CalculateVolumeBox(pPrimitive->hdr.volume,pPrimitive);

		// set DGNFILE buf by 0.
		memset(hDGN->abyElem,0,65540);
		if(pPrimitive->hdr.pLayer) pDGNHdr->ElmHdr.level  = atoi(pPrimitive->hdr.pLayer);
		pDGNHdr->ElmHdr.complex= 0;
		pDGNHdr->ElmHdr.deleted= 0;
		pDGNHdr->ElmHdr.xlow   = (long)pPrimitive->hdr.volume.x[LOW];
		pDGNHdr->ElmHdr.xhigh  = (long)pPrimitive->hdr.volume.x[HIGH];
		pDGNHdr->ElmHdr.ylow   = (long)pPrimitive->hdr.volume.y[LOW];
		pDGNHdr->ElmHdr.yhigh  = (long)pPrimitive->hdr.volume.y[HIGH];
		pDGNHdr->ElmHdr.zlow   = (long)pPrimitive->hdr.volume.z[LOW];
		pDGNHdr->ElmHdr.zhigh  = (long)pPrimitive->hdr.volume.z[HIGH];
		
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
		
		pDGNHdr->DispHdr.props.b.n = 1;
		pDGNHdr->DispHdr.symb.b.color = pPrimitive->hdr.display.lcolor;
		pDGNHdr->DispHdr.symb.b.style = pPrimitive->hdr.display.ltype;
		pDGNHdr->DispHdr.symb.b.weight= pPrimitive->hdr.display.lweight;

		bRet = true;
	}

	return bRet;
}

/**	\brief	The CDGNCone::GetQuaternion function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	QUAT_T
*/
QUAT_T CDGNCone::GetQuaternion(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	QUAT_T ret={0.};

	if(pPrimitive)
	{
		VECTOR_T vecAxis={0.};
		vecAxis.dx = pPrimitive->body.cone.ptOrigin[1].x - pPrimitive->body.cone.ptOrigin[0].x;
		vecAxis.dy = pPrimitive->body.cone.ptOrigin[1].y - pPrimitive->body.cone.ptOrigin[0].y;
		vecAxis.dz = pPrimitive->body.cone.ptOrigin[1].z - pPrimitive->body.cone.ptOrigin[0].z;
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

/**	\brief	The CDGNCone::Parse function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\return	long
*/
long CDGNCone::Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");
	long nWords=0;
	nWords = (int)(hDGN->nElemBytes*0.5);

	PSQPRIMITIVE pPrimitive=NULL;
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(hDGN && (pPrimitive = pDGNLib->NewPrimitive()))
	{
		CDGNElement::ParseHeader(pPrimitive,hDGN);
		pPrimitive->hdr.id     = SQ_CONE;
		pPrimitive->hdr.attr.d = hDGN->dimension;
		pPrimitive->hdr.attr.f = 1;

		long quat[4]={0,};
		quat[0] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 38);
		quat[1] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 42);
		quat[2] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 46);
		quat[3] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 50);

		memcpy(&(pPrimitive->body.cone.ptOrigin[0].x),hDGN->abyElem + 54,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[0].x));
		pPrimitive->body.cone.ptOrigin[0].x *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.ptOrigin[0].y),hDGN->abyElem + 62,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[0].y));
		pPrimitive->body.cone.ptOrigin[0].y *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.ptOrigin[0].z),hDGN->abyElem + 70,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[0].z));
		pPrimitive->body.cone.ptOrigin[0].z *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.nRadius[0]),hDGN->abyElem + 78,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.nRadius[0]));
		pPrimitive->body.cone.nRadius[0] *= hDGN->scale;

		memcpy(&(pPrimitive->body.cone.ptOrigin[1].x),hDGN->abyElem + 86,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[1].x));
		pPrimitive->body.cone.ptOrigin[1].x *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.ptOrigin[1].y),hDGN->abyElem + 94,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[1].y));
		pPrimitive->body.cone.ptOrigin[1].y *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.ptOrigin[1].z),hDGN->abyElem + 102,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.ptOrigin[1].z));
		pPrimitive->body.cone.ptOrigin[1].z *= hDGN->scale;
		memcpy(&(pPrimitive->body.cone.nRadius[1]),hDGN->abyElem + 110,8);
		DGN2IEEEDouble(&(pPrimitive->body.cone.nRadius[1]));
		pPrimitive->body.cone.nRadius[1] *= hDGN->scale;

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

		POINT_T pt[2]={0};
		pt[0].x = pPrimitive->body.cone.nRadius[0];
		pt[0].y = 0.;
		pt[0].z = 0.;
		pt[1].x = 0.;
		pt[1].y = pPrimitive->body.cone.nRadius[0];
		pt[1].z = 0.;
		for(int i=0;i < 2;i++)
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
			pt[1].x,pt[1].y,pt[1].z,
			&(vec.dx),&(vec.dy),&(vec.dz));
		pPrimitive->body.cone.vecAxis.dx = vec.dx;
		pPrimitive->body.cone.vecAxis.dy = vec.dy;
		pPrimitive->body.cone.vecAxis.dz = vec.dz;

		pPrimitive->body.cone.vecAxis.dx = pPrimitive->body.cone.ptOrigin[1].x - pPrimitive->body.cone.ptOrigin[0].x;
		pPrimitive->body.cone.vecAxis.dy = pPrimitive->body.cone.ptOrigin[1].y - pPrimitive->body.cone.ptOrigin[0].y;
		pPrimitive->body.cone.vecAxis.dz = pPrimitive->body.cone.ptOrigin[1].z - pPrimitive->body.cone.ptOrigin[0].z;

		PDGNHDR pDGNHdr = (PDGNHDR)hDGN->abyElem;
		if(pDGNHdr->DispHdr.props.b.a)
		{
			CDGNElement::ParseDMRS(pPrimitive,hDGN);

			PDGNATTRDATA pAttrData=(PDGNATTRDATA)(pPrimitive->pLinkageData);
			if(pAttrData && (5 == pAttrData->UserID))
			{
				pPrimitive->hdr.handle = pAttrData->data.occur;
				strcpy(pPrimitive->hdr.szDesc,"PIPE");
			}
			else	strcpy(pPrimitive->hdr.szDesc,"CONE");
		}
		
		//--> calculate cone volume
		double nRadius=(pPrimitive->body.cone.nRadius[0] > pPrimitive->body.cone.nRadius[1]) ? pPrimitive->body.cone.nRadius[0] : pPrimitive->body.cone.nRadius[1];
		double dx=pPrimitive->body.cone.ptOrigin[1].x - pPrimitive->body.cone.ptOrigin[0].x;
		double dy=pPrimitive->body.cone.ptOrigin[1].y - pPrimitive->body.cone.ptOrigin[0].y;
		double dz=pPrimitive->body.cone.ptOrigin[1].z - pPrimitive->body.cone.ptOrigin[0].z;
		double nLength=sqrt(dx*dx + dy*dy + dz*dz);

		POINT_T ptVolumeEdge[8]={0};
		ptVolumeEdge[0].x = -nRadius,ptVolumeEdge[0].y = -nRadius,ptVolumeEdge[0].z = -nLength*0.5;
		ptVolumeEdge[1].x =  nRadius,ptVolumeEdge[1].y = -nRadius,ptVolumeEdge[1].z = -nLength*0.5;
		ptVolumeEdge[2].x =  nRadius,ptVolumeEdge[2].y =  nRadius,ptVolumeEdge[2].z = -nLength*0.5;
		ptVolumeEdge[3].x = -nRadius,ptVolumeEdge[3].y =  nRadius,ptVolumeEdge[3].z = -nLength*0.5;
		ptVolumeEdge[4].x = -nRadius,ptVolumeEdge[0].y = -nRadius,ptVolumeEdge[0].z =  nLength*0.5;
		ptVolumeEdge[5].x =  nRadius,ptVolumeEdge[1].y = -nRadius,ptVolumeEdge[1].z =  nLength*0.5;
		ptVolumeEdge[6].x =  nRadius,ptVolumeEdge[2].y =  nRadius,ptVolumeEdge[2].z =  nLength*0.5;
		ptVolumeEdge[7].x = -nRadius,ptVolumeEdge[3].y =  nRadius,ptVolumeEdge[3].z =  nLength*0.5;
		for(int i = 0;i < 8;i++)
		{
			double x=ptVolumeEdge[i].x;
			double y=ptVolumeEdge[i].y;
			double z=ptVolumeEdge[i].z;
			ptVolumeEdge[i].x = a11*x + a12*y + a13*z;
			ptVolumeEdge[i].y = a21*x + a22*y + a23*z;
			ptVolumeEdge[i].z = a31*x + a32*y + a33*z;
		}
		pPrimitive->hdr.volume.x[LOW] = ptVolumeEdge[0].x;
		pPrimitive->hdr.volume.x[HIGH]= ptVolumeEdge[0].x;
		pPrimitive->hdr.volume.y[LOW] = ptVolumeEdge[0].y;
		pPrimitive->hdr.volume.y[HIGH]= ptVolumeEdge[0].y;
		pPrimitive->hdr.volume.z[LOW] = ptVolumeEdge[0].z;
		pPrimitive->hdr.volume.z[HIGH]= ptVolumeEdge[0].z;
		for(int i = 1;i < 8;i++)
		{
			if(pPrimitive->hdr.volume.x[LOW] > ptVolumeEdge[i].x) pPrimitive->hdr.volume.x[LOW] = ptVolumeEdge[i].x;
			if(pPrimitive->hdr.volume.x[HIGH]< ptVolumeEdge[i].x) pPrimitive->hdr.volume.x[HIGH]= ptVolumeEdge[i].x;
			if(pPrimitive->hdr.volume.y[LOW] > ptVolumeEdge[i].y) pPrimitive->hdr.volume.y[LOW] = ptVolumeEdge[i].y;
			if(pPrimitive->hdr.volume.y[HIGH]< ptVolumeEdge[i].y) pPrimitive->hdr.volume.y[HIGH]= ptVolumeEdge[i].y;
			if(pPrimitive->hdr.volume.z[LOW] > ptVolumeEdge[i].z) pPrimitive->hdr.volume.z[LOW] = ptVolumeEdge[i].z;
			if(pPrimitive->hdr.volume.z[HIGH]< ptVolumeEdge[i].z) pPrimitive->hdr.volume.z[HIGH]= ptVolumeEdge[i].z;
		}
		POINT_T ptCenter={0};
		ptCenter.x = (pPrimitive->body.cone.ptOrigin[0].x + pPrimitive->body.cone.ptOrigin[1].x)*0.5;
		ptCenter.y = (pPrimitive->body.cone.ptOrigin[0].y + pPrimitive->body.cone.ptOrigin[1].y)*0.5;
		ptCenter.z = (pPrimitive->body.cone.ptOrigin[0].z + pPrimitive->body.cone.ptOrigin[1].z)*0.5;
		pPrimitive->hdr.volume.x[LOW] += ptCenter.x;
		pPrimitive->hdr.volume.x[HIGH]+= ptCenter.x;
		pPrimitive->hdr.volume.y[LOW] += ptCenter.y;
		pPrimitive->hdr.volume.y[HIGH]+= ptCenter.y;
		pPrimitive->hdr.volume.z[LOW] += ptCenter.z;
		pPrimitive->hdr.volume.z[HIGH]+= ptCenter.z;
		//<--

		(*ppPrimitiveList) = ListPush((*ppPrimitiveList),pPrimitive);
	}

	return nWords;
}

/**	\brief	The CDGNCone::Write function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long
*/
long CDGNCone::Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	if(hDGN && pPrimitive)
	{
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;

		StoreHeader(hDGN,pPrimitive);
		pDGNHDR->ElmHdr.type  = DGNT_CONE;
		pDGNHDR->ElmHdr.words = 16 + 41;

		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		pDISPHDR->props.b.n   = 1;
		pDISPHDR->props.b.h   = pPrimitive->hdr.attr.h;

		QUAT_T quat=GetQuaternion(pPrimitive);

		// quaternion
		long*   l = (long*)(hDGN->abyElem + 38);
		(*l) = (long)(quat.w*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 42);
		(*l) = (long)(quat.x*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 46);
		(*l) = (long)(quat.y*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 50);
		(*l) = (long)(quat.z*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		// center1 x
		double* pd = (double*)(hDGN->abyElem + 54);
		(*pd) = (pPrimitive->body.cone.ptOrigin[0].x);
		(*pd) = IEEE2VAX(pd);
		
		// center1 y
		pd = (double*)(hDGN->abyElem + 62);
		(*pd) = (pPrimitive->body.cone.ptOrigin[0].y);
		(*pd) = IEEE2VAX(pd);
		
		// center1 z
		pd = (double*)(hDGN->abyElem + 70);
		(*pd) = (pPrimitive->body.cone.ptOrigin[0].z);
		(*pd) = IEEE2VAX(pd);

		// radius1
		pd=(double*)(hDGN->abyElem + 78);
		(*pd)= (pPrimitive->body.cone.nRadius[0]);
		(*pd) = IEEE2VAX(pd);
		
		// center2 x
		pd = (double*)(hDGN->abyElem + 86);
		(*pd) = (pPrimitive->body.cone.ptOrigin[1].x);
		(*pd) = IEEE2VAX(pd);
		
		// center2 y
		pd = (double*)(hDGN->abyElem + 94);
		(*pd) = (pPrimitive->body.cone.ptOrigin[1].y);
		(*pd) = IEEE2VAX(pd);
		
		// center2 z
		pd = (double*)(hDGN->abyElem + 102);
		(*pd) = (pPrimitive->body.cone.ptOrigin[1].z);
		(*pd) = IEEE2VAX(pd);

		// radius2
		pd=(double*)(hDGN->abyElem + 110);
		(*pd)= (pPrimitive->body.cone.nRadius[1]);
		(*pd) = IEEE2VAX(pd);

		if(NULL != pPrimitive->pLinkageData)
		{
			pDGNHDR->DispHdr.props.b.a = 1;
			long lIndex=118;
			for(PDGNATTRDATA ptr=(PDGNATTRDATA)(pPrimitive->pLinkageData);ptr;ptr = ptr->next)
			{
				if(ptr->hdr.u)
				{
					memcpy(hDGN->abyElem + lIndex,ptr,4);
					memcpy(hDGN->abyElem + lIndex + 4,ptr->data.pData,(ptr->hdr.wtf - 1)*2);
					pDGNHDR->ElmHdr.words += (ptr->hdr.wtf + 1);
					lIndex += (ptr->hdr.wtf + 1) * 2;
					bool odd=((ptr->hdr.wtf + 1) % 2) ? true : false;
					if(true == odd)
					{
						short dummy=0;
						memcpy(hDGN->abyElem + lIndex + 4 + (ptr->hdr.wtf - 1)*2,&dummy,sizeof(dummy));
						pDGNHDR->ElmHdr.words += 1;
						lIndex += 2;
					}
				}
				else
				{
					memcpy(hDGN->abyElem + lIndex,ptr,(4)*2);
					pDGNHDR->ElmHdr.words += 4;
					lIndex += (4)*2;
				}
			}
		}

		fwrite(hDGN->abyElem,2,pDGNHDR->ElmHdr.words + 2,hDGN->fp);
		nWords = pDGNHDR->ElmHdr.words + 2;
	}

	return nWords;
}