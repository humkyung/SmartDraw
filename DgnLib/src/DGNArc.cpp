// DGNArc.cpp: implementation of the CDGNArc class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "DGNLib.h"
#include "DGNArc.h"
#include "..\DGNParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDGNArc::CDGNArc()
{
	m_nType = ENM_ARC;
}

CDGNArc::~CDGNArc()
{

}

/**	\brief	The CDGNArc::size function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNArc::size(PSQPRIMITIVE pPrimitive)
{
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	long nRet=0;

	if(pDGNLib)
	{
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		if(2 == nDimension)
			nRet = 41;
		else if(3 == nDimension)
			nRet = 50;
	}

	return nRet;
}

/**	\brief	The CDGNArc::Volume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	VOLUME_T	
*/
VOLUME_T CDGNArc::Volume(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	SQVOLUME ret={0};

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if(pPrimitive && pDGNLib)
	{
		double dx=pPrimitive->body.arc.ptStart.x - pPrimitive->body.arc.ptOrigin.x;
		double dy=pPrimitive->body.arc.ptStart.y - pPrimitive->body.arc.ptOrigin.y;
		double dz=pPrimitive->body.arc.ptStart.z - pPrimitive->body.arc.ptOrigin.z;
		double nRadius=sqrt(dx*dx + dy*dy + dz*dz);

		VECTOR_T vec[4]={0.,};
		vec[0].dx-= nRadius;vec[0].dy-= nRadius;
		vec[1].dx+= nRadius;vec[1].dy-= nRadius;
		vec[2].dx+= nRadius;vec[2].dy+= nRadius;
		vec[3].dx-= nRadius;vec[3].dy+= nRadius;
		
		QUAT_T quat=GetQuaternion(pPrimitive);
		vec[0] = CGeometry::Rotate(vec[0],quat);
		vec[1] = CGeometry::Rotate(vec[1],quat);
		vec[2] = CGeometry::Rotate(vec[2],quat);
		vec[3] = CGeometry::Rotate(vec[3],quat);

		ret.x[0] = ret.x[1] = vec[0].dx;
		ret.y[0] = ret.y[1] = vec[0].dy;
		ret.z[0] = ret.z[1] = vec[0].dz;
		for(int i=1;i < 4;i++)
		{
			if(ret.x[0] > vec[i].dx) ret.x[0] = vec[i].dx;
			if(ret.x[1] < vec[i].dx) ret.x[1] = vec[i].dx;
			if(ret.y[0] > vec[i].dy) ret.y[0] = vec[i].dy;
			if(ret.y[1] < vec[i].dy) ret.y[1] = vec[i].dy;
			if(ret.z[0] > vec[i].dz) ret.z[0] = vec[i].dz;
			if(ret.z[1] < vec[i].dz) ret.z[1] = vec[i].dz;
		}
		ret.x[0] += pPrimitive->body.arc.ptOrigin.x;
		ret.x[1] += pPrimitive->body.arc.ptOrigin.x;
		ret.y[0] += pPrimitive->body.arc.ptOrigin.y;
		ret.y[1] += pPrimitive->body.arc.ptOrigin.y;
		ret.z[0] += pPrimitive->body.arc.ptOrigin.z;
		ret.z[1] += pPrimitive->body.arc.ptOrigin.z;
	}

	return ret;
}

/**	\brief	The CDGNArc::GetQuaternion function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	QUAT_T	
*/
QUAT_T CDGNArc::GetQuaternion(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	QUAT_T ret={0.};

	if(pPrimitive)
	{
		VECTOR_T vecAxis=pPrimitive->body.arc.vecNorm;
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

/**	\brief	The CDGNArc::Parse function

	\param	ppPrimitiveList	a parameter of type PSQPRIMITIVE*
	\param	hDGN	a parameter of type CDGNFile*

	\remarks\n
	;2002.06.19 - if length of axis is less than DGN_TOLERANCE, set length of axis to zero.

	\return	long	
*/
long CDGNArc::Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN)
{
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

		if(2 == hDGN->dimension)
		{
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.x),hDGN->abyElem + 64,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.x));
			pPrimitive->body.ellipse.ptOrigin.x *= hDGN->scale;
			
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.y),hDGN->abyElem + 72,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.y));
			pPrimitive->body.ellipse.ptOrigin.y *= hDGN->scale;

			memcpy(&(pPrimitive->body.ellipse.nAxis[0]),hDGN->abyElem + 44,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[0]));
			if(fabs(pPrimitive->body.ellipse.nAxis[0]) < DGN_TOLERANCE) pPrimitive->body.ellipse.nAxis[0] = 0.;
			pPrimitive->body.ellipse.nAxis[0] *= hDGN->scale;
			memcpy(&(pPrimitive->body.ellipse.nAxis[1]),hDGN->abyElem + 52,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[1]));
			if(fabs(pPrimitive->body.ellipse.nAxis[1]) < DGN_TOLERANCE) pPrimitive->body.ellipse.nAxis[1] = 0.;
			pPrimitive->body.ellipse.nAxis[1] *= hDGN->scale;

			pPrimitive->body.ellipse.nStartAngle = SWAP_VAX32PTR_FOR_LITTLE( hDGN->abyElem + 36);
			pPrimitive->body.ellipse.nSweepAngle = pPrimitive->body.ellipse.nStartAngle/360000.0;
			
			long nSweepVal = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 40);
			if(nSweepVal & 0x80000000) 
				pPrimitive->body.ellipse.nSweepAngle = -(nSweepVal & 0x7fffffff)/360000.0;
			else if(0 == nSweepVal)
				pPrimitive->body.ellipse.nSweepAngle = 360.0;
			else	pPrimitive->body.ellipse.nSweepAngle = nSweepVal/360000.0;

			PDGNHDR pDGNHDR   = (PDGNHDR)(hDGN->abyElem);
			if(pDGNHDR->DispHdr.props.b.a) CDGNElement::ParseDMRS(pPrimitive,hDGN);

			pPrimitive->hdr.volume.x[LOW] = pPrimitive->body.ellipse.ptOrigin.x - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.x[HIGH]= pPrimitive->body.ellipse.ptOrigin.x + pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[LOW] = pPrimitive->body.ellipse.ptOrigin.y - pPrimitive->body.ellipse.nAxis[0];
			pPrimitive->hdr.volume.y[HIGH]= pPrimitive->body.ellipse.ptOrigin.y + pPrimitive->body.ellipse.nAxis[0];
		}
		else if(3 == hDGN->dimension)
		{
			pPrimitive->body.ellipse.nStartAngle = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 36);
			pPrimitive->body.ellipse.nStartAngle = pPrimitive->body.ellipse.nStartAngle/360000.0;
						
			long nSweepVal = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 40);
			if(nSweepVal & 0x80000000 ) 
				pPrimitive->body.ellipse.nSweepAngle = -(nSweepVal & 0x7fffffff)/360000.0;
			else if(0. == nSweepVal)
				pPrimitive->body.ellipse.nSweepAngle = 360.0;
			else	pPrimitive->body.ellipse.nSweepAngle = nSweepVal / 360000.0;

			memcpy(&(pPrimitive->body.ellipse.nAxis[0]),hDGN->abyElem + 44,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[0]));
			if(fabs(pPrimitive->body.ellipse.nAxis[0]) < DGN_TOLERANCE) pPrimitive->body.ellipse.nAxis[0] = 0.;
			pPrimitive->body.ellipse.nAxis[0] *= hDGN->scale;
			memcpy(&(pPrimitive->body.ellipse.nAxis[1]),hDGN->abyElem + 52,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.nAxis[1]));
			if(fabs(pPrimitive->body.ellipse.nAxis[1]) < DGN_TOLERANCE) pPrimitive->body.ellipse.nAxis[1] = 0.;
			pPrimitive->body.ellipse.nAxis[1] *= hDGN->scale;

			memcpy(&(pPrimitive->body.ellipse.ptOrigin.x),hDGN->abyElem + 76,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.x));
			pPrimitive->body.ellipse.ptOrigin.x *= hDGN->scale;
			
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.y),hDGN->abyElem + 84,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.y));
			pPrimitive->body.ellipse.ptOrigin.y *= hDGN->scale;
			
			memcpy(&(pPrimitive->body.ellipse.ptOrigin.z),hDGN->abyElem + 92,8);
			DGN2IEEEDouble(&(pPrimitive->body.ellipse.ptOrigin.z));
			pPrimitive->body.ellipse.ptOrigin.z *= hDGN->scale;

			long quat[4]={0};
			quat[0] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 60);
			quat[1] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 64);
			quat[2] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 68);
			quat[3] = SWAP_VAX32PTR_FOR_LITTLE(hDGN->abyElem + 72);
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
			pt[0].x = pPrimitive->body.ellipse.nAxis[0];
			pt[0].y = 0.;
			pt[0].z = 0.;
			pt[1].x = 0.;
			pt[1].y = 0.;
			pt[1].z = 1.;
			x = pt[0].x;
			y = pt[0].y;
			z = pt[0].z;
			pt[0].x = x*a11 + a12*y + a13*z;
			pt[0].y = x*a21 + a22*y + a23*z;
			pt[0].z = x*a31 + a32*y + a33*z;
			x = pt[1].x;
			y = pt[1].y;
			z = pt[1].z;
			pt[1].x = x*a11 + a12*y + a13*z;
			pt[1].y = x*a21 + a22*y + a23*z;
			pt[1].z = x*a31 + a32*y + a33*z;
			pPrimitive->body.ellipse.vecNorm.dx = pt[1].x;
			pPrimitive->body.ellipse.vecNorm.dy = pt[1].y;
			pPrimitive->body.ellipse.vecNorm.dz = pt[1].z;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(pPrimitive->body.ellipse.vecNorm,alpha,beta);
			pt[0] = CGeometry::RotateAboutXYAxis(pt[0],alpha,beta);
			double theta=CGeometry::GetRotatedAngleInXYPlane(pt[0]);
			pPrimitive->body.ellipse.nRotate = RAD2DEG(theta);

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

/**	\brief	The CDGNArc::Write function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNArc::Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nWords=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		
		GetHeader(hDGN,pPrimitive);
		if(2 == nDimension)
			nWords = Write2D(hDGN,pPrimitive);	
		else if(3 == nDimension)
			nWords = Write3D(hDGN,pPrimitive);
		fwrite(hDGN->abyElem,2,nWords,hDGN->fp);
	}

	return nWords;
}

/**	\brief	The CDGNArc::Write2D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNArc::Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();

		pDGNHDR->ElmHdr.type  = DGNT_ARC;
		pDGNHDR->ElmHdr.words = 17 + 22;
		
		DISPHDR* pDISPHDR     = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx     = pDGNHDR->ElmHdr.words - 14;
		
		long* pl = (long*)(hDGN->abyElem + 36);
		POINT_T ptStart={0};
		ptStart.x=pPrimitive->body.arc.ptStart.x - pPrimitive->body.arc.ptOrigin.x;
		ptStart.y=pPrimitive->body.arc.ptStart.y - pPrimitive->body.arc.ptOrigin.y;
		ptStart.z=pPrimitive->body.arc.ptStart.z - pPrimitive->body.arc.ptOrigin.z;
		double nRadius=sqrt(ptStart.x*ptStart.x + ptStart.y*ptStart.y + ptStart.z*ptStart.z);
		double nStartAngle=CGeometry::GetRotatedAngleInXYPlane(ptStart);
		nStartAngle = RAD2DEG(nStartAngle)*360000.0;
		(*pl) = (long)(nStartAngle);
		SWAP_LITTLE32_FOR_VAX(*pl);
		
		pl = (long*)(hDGN->abyElem + 40);
		(*pl) = (long)(RAD2DEG(pPrimitive->body.arc.nSweep)*360000.0);
		SWAP_LITTLE32_FOR_VAX(*pl);
		
		double* pd=(double*)(hDGN->abyElem + 44);
		(*pd)= (nRadius)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		pd = (double*)(hDGN->abyElem + 52);
		(*pd) = (nRadius)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		// rotation
		pl = (long*)(hDGN->abyElem + 60);
		(*pl) = 0;
		SWAP_LITTLE32_FOR_VAX(*pl);
		
		// origin x
		pd = (double*)(hDGN->abyElem + 64);
		(*pd) = (pPrimitive->body.arc.ptOrigin.x)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		// origin y
		pd = (double*)(hDGN->abyElem + 72);
		(*pd) = (pPrimitive->body.arc.ptOrigin.y)*nSubUnit;
		(*pd) = IEEE2VAX(pd);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNArc::Write3D function

	\param	hDGN	a parameter of type CDGNFile*
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	long	
*/
long CDGNArc::Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)
{
	assert(hDGN       && "hDGN is NULL");
	assert(pPrimitive && "pPrimitive is NULL");
	long nRet=0;

	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	if((hDGN && hDGN->fp) && pPrimitive)
	{
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		int nDimension =pDGNLib->m_DGNExport.GetDimension();
		double nSubUnit=pDGNLib->m_DGNExport.GetSubUnit();
		
		pDGNHDR->ElmHdr.type  = DGNT_ARC;
		pDGNHDR->ElmHdr.words = 16 + 32;
		
		DISPHDR* pDISPHDR = (DISPHDR*)(hDGN->abyElem + 28);
		pDISPHDR->attindx = pDGNHDR->ElmHdr.words - 14;
		
		VECTOR_T vecAxis = pPrimitive->body.arc.vecNorm;
		QUAT_T quat = GetQuaternion(pPrimitive);

		double alpha=0.,beta=0.;
		VECTOR_T vecStart={0.},vecEnd={0.};
		vecStart.dx=pPrimitive->body.arc.ptStart.x - pPrimitive->body.arc.ptOrigin.x;
		vecStart.dy=pPrimitive->body.arc.ptStart.y - pPrimitive->body.arc.ptOrigin.y;
		vecStart.dz=pPrimitive->body.arc.ptStart.z - pPrimitive->body.arc.ptOrigin.z;
		double nAxis=sqrt(vecStart.dx*vecStart.dx + vecStart.dy*vecStart.dy + vecStart.dz*vecStart.dz);
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		vecStart = CGeometry::RotateAboutXYAxis(vecStart,alpha,beta);
		double nStartAngle=CGeometry::GetRotatedAngleInXYPlane(vecStart);
		nStartAngle = RAD2DEG(nStartAngle)*360000.0;
		long* pl = (long*)(hDGN->abyElem + 36);
		(*pl) = (long)(nStartAngle);
		SWAP_LITTLE32_FOR_VAX(*pl);
		
		pl = (long*)(hDGN->abyElem + 40);
		(*pl) = (long)(pPrimitive->body.arc.nSweep*360000.0);
		SWAP_LITTLE32_FOR_VAX(*pl);
				
		double* pd=(double*)(hDGN->abyElem + 44);
		(*pd)= (nAxis)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		pd = (double*)(hDGN->abyElem + 52);
		(*pd) = (nAxis)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
			
		long* l = (long*)(hDGN->abyElem + 60);
		(*l) = (long)(quat.w*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 64);
		(*l) = (long)(quat.x*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 68);
		(*l) = (long)(quat.y*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		l = (long*)(hDGN->abyElem + 72);
		(*l) = (long)(quat.z*2147483647);
		SWAP_LITTLE32_FOR_VAX(*l);
		
		// origin x
		pd = (double*)(hDGN->abyElem + 76);
		(*pd) = (pPrimitive->body.arc.ptOrigin.x)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		// origin y
		pd = (double*)(hDGN->abyElem + 84);
		(*pd) = (pPrimitive->body.arc.ptOrigin.y)*nSubUnit;
		(*pd) = IEEE2VAX(pd);
		
		// origin y
		pd = (double*)(hDGN->abyElem + 92);
		(*pd) = (pPrimitive->body.arc.ptOrigin.z)*nSubUnit;
		(*pd) = IEEE2VAX(pd);

		nRet = pDGNHDR->ElmHdr.words + 2;
	}

	return nRet;
}

/**	\brief	The CDGNArc::GetEndPoint function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	POINT_T	
*/
POINT_T CDGNArc::GetEndPoint(PSQPRIMITIVE pPrimitive)
{
	POINT_T ptRet={0,};

	VECTOR_T vecAxis = pPrimitive->body.arc.vecNorm;
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);

	VECTOR_T vecStart={0.},vecEnd={0.};
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

	ptRet.x = pPrimitive->body.arc.ptOrigin.x + vecEnd.dx;
	ptRet.y = pPrimitive->body.arc.ptOrigin.y + vecEnd.dy;
	ptRet.z = pPrimitive->body.arc.ptOrigin.z + vecEnd.dz;

	return ptRet;
}