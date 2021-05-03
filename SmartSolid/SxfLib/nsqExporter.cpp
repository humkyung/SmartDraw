#include <tchar.h>
#include <windows.h>
#include <assert.h>
#include <math.h>
#include <graphics/geometry.h>
#include <graphics/ellipse3d.h>
#include <math\ITKMath.h>
#include "NSQExporter.h"
#include "ElbowExporter.h"
#include "..\SmartSolid.h"

#include <fstream>
#include <string>
#include <sstream>
using namespace std;

const char* CNSQExporter::szFormat[2]=
{
	"%d,LINE,%d,%lf,%lf,%lf,%lf,%lf,%lf\r\n",
	"%d,LINE,%d,%lf,%lf,%lf,%lf,%lf,%lf\r\n"
};

COpenSolid theOpenSolid;

/**	
	@brief
	@author	humkyung
	@date	????.??.??
*/
CNSQExporter::CNSQExporter() : m_nSkipLevel(56)
{
	m_dModelOffset[0] = m_dModelOffset[1] = m_dModelOffset[2] = 0;
}

/**	
	@brief
	@author	humkyung
	@date	????.??.??
*/
CNSQExporter::~CNSQExporter()
{
}

/**	
	@brief	write an element
	@author	humkyung
	@date	2011.05.04
	@param	FILE*		a parameter of type const char*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@return	void	
*/
void CNSQExporter::WriteElement(ofstream& ofile , PSQPRIMITIVE pPrimitive , long &nSeq , long &nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		for(PSQPRIMITIVE ptr = pPrimitive;ptr;ptr = ptr->next)
		{
			if(!ptr->hdr.attr.f) continue;
			switch(ptr->hdr.id)
			{
			case ENM_LINE:
				WriteLine(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case ENM_ELLIPSE:
				WriteEllipse(ofile ,ptr,nSeq , nGroup , true , true);
				break;
			case ENM_ARC:
				WriteArc(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case ENM_POLYLINE:
				WritePolyline(ofile ,ptr,nSeq,nGroup , true);
				break;
			case SQ_POINTS:
				if(0 == strcmp(ptr->hdr.szDesc, _T("LOOP")))
				{
					WritePoints(ofile ,ptr,nSeq,nGroup , true);
				}
				else if(0 == strcmp(ptr->hdr.szDesc, _T("3D_FACE")))
				{
					Write3dFace(ofile ,ptr,nSeq,nGroup);
					nGroup++;
				}
				else if(0 == strcmp(ptr->hdr.szDesc , _T("SWEEP")))
				{
					WriteSweepSurface(ofile , ptr , nSeq , nGroup);
					nGroup++;
				}
				break;
			case SQ_VERTICES:
				WriteVertices(ofile ,ptr,nSeq,nGroup , true);
				break;
			case SQ_LOOP:
				WriteLoop(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_SOLID:
				WriteSolid(ofile ,ptr,nSeq,nGroup);
				//nGroup++;
				break;
			case SQ_CONE:
				WriteCone(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_SURFACE:
				try
				{
					if(0 == strcmp(ptr->hdr.szDesc,"ELBOW"))
						WriteElbowSurf(ofile ,ptr,nSeq,nGroup);
					else if(0 == strcmp(ptr->hdr.szDesc,"DOME"))
						WriteDome(ofile ,ptr,nSeq,nGroup);
					else	WriteSurface(ofile ,ptr,nSeq,nGroup);
					nGroup++;
				}
				catch(...)
				{

				}
				break;
			case SQ_CONESURFACE:
				if(0 == strcmp(ptr->hdr.szDesc,"CONE"))
				{
					if(2 == ptr->body.pConeSurf->nLoop)
					{
						PSQPRIMITIVE pBoundaryCrv[2]={ptr->body.pConeSurf->pBoundary[0],ptr->body.pConeSurf->pBoundary[1]};
						if((ENM_ARC == pBoundaryCrv[0]->hdr.id) && (ENM_ARC == pBoundaryCrv[1]->hdr.id))
						{
							VECTOR_T vec={0.,};
							vec.dx = pBoundaryCrv[0]->body.arc.ptStart.x - pBoundaryCrv[0]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[0]->body.arc.ptStart.y - pBoundaryCrv[0]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[0]->body.arc.ptStart.z - pBoundaryCrv[0]->body.arc.ptOrigin.z;
							double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
							vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
							double radius_b=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
							vec = ptr->body.pConeSurf->vecDelta;

							ofile << _T("CONE,");
							ofile << radius_a << _T(",") << radius_b << _T(",");
							ofile << (pBoundaryCrv[0]->body.arc.ptOrigin.x + m_dModelOffset[0])  << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
							ofile << (pBoundaryCrv[1]->body.arc.ptOrigin.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.z + m_dModelOffset[2] - vec.dz) << _T(",");
							ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
							ofile << int(ptr->hdr.display.lcolor) << std::endl;
						}
						else if((ENM_ARC == pBoundaryCrv[0]->hdr.id) && (ENM_POINT == pBoundaryCrv[1]->hdr.id))
						{
							VECTOR_T vec={0.,};
							vec.dx = pBoundaryCrv[0]->body.arc.ptStart.x - pBoundaryCrv[0]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[0]->body.arc.ptStart.y - pBoundaryCrv[0]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[0]->body.arc.ptStart.z - pBoundaryCrv[0]->body.arc.ptOrigin.z;
							const double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
							/*
							vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
							*/
							const double radius_b = 0.f;
							vec = ptr->body.pConeSurf->vecDelta;

							ofile << _T("CONE,");
							ofile << radius_a << _T(",") << radius_b << _T(",");
							ofile << (pBoundaryCrv[0]->body.arc.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
							ofile << (pBoundaryCrv[1]->body.point.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[1]->body.point.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[1]->body.point.z + m_dModelOffset[2] - vec.dz) << _T(",");
							ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
							ofile << int(ptr->hdr.display.lcolor) << std::endl;
						}
						else if((ENM_POINT == pBoundaryCrv[0]->hdr.id) && (ENM_ARC == pBoundaryCrv[1]->hdr.id))
						{
							VECTOR_T vec={0.,};
							vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
							const double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
							/*
							vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
							vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
							vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
							*/
							const double radius_b = 0.f;
							vec = ptr->body.pConeSurf->vecDelta;

							ofile << _T("CONE,");
							ofile << radius_a << _T(",") << radius_b << _T(",");
							ofile << (pBoundaryCrv[1]->body.arc.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
							ofile << (pBoundaryCrv[0]->body.point.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[0]->body.point.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[0]->body.point.z + m_dModelOffset[2] - vec.dz) << _T(",");
							ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
							ofile << int(ptr->hdr.display.lcolor) << std::endl;
						}
					}
				}
				break;
			case ENM_GROUP:
				if(0 == strcmp(ptr->hdr.szDesc, _T("PIPE")))
				{
					for(PSQPRIMITIVE ptr1=ptr->body.pPrimitive;ptr1;ptr1 = ptr1->next)
					{
						if(ptr1->hdr.pLayer && (m_nSkipLevel == ptr1->hdr.pLayer[0])) continue;

						if(SQ_CONE == ptr1->hdr.id)
						{
							POINT_T pt[2]={0};

							const double nRadius = ptr1->body.cone.nRadius[0];
							pt[0]  = ptr1->body.cone.ptOrigin[0];
							pt[1]  = ptr1->body.cone.ptOrigin[1];

							ofile << _T("PIPE,");
							ofile << nRadius << _T(",");
							ofile << (pt[0].x + m_dModelOffset[0]) << _T(",") << (pt[0].y + m_dModelOffset[1]) << _T(",") << (pt[0].z + m_dModelOffset[2]) << _T(",");
							ofile << (pt[1].x + m_dModelOffset[0]) << _T(",") << (pt[1].y + m_dModelOffset[1]) << _T(",") << (pt[1].z + m_dModelOffset[2]) << _T(",");
							ofile << int(ptr->hdr.display.lcolor) << std::endl;
						}
					}
				}
				else if(0 == strcmp(ptr->hdr.szDesc, _T("ELBOW")))
				{
					CElbowExporter elbowExporter;
					elbowExporter.WriteElbow(ofile ,ptr,nSeq,nGroup , m_nSkipLevel);
					nGroup++;
				}
				else if(0 == strcmp(ptr->hdr.szDesc, _T("TEE")))
				{
					WriteTee(ofile ,ptr,nSeq,nGroup);
					nGroup++;
				}
				else if(IsPlaneSurfWithHole(ptr))
				{
					if(!WritePlaneSurf(ofile ,ptr,nSeq,nGroup))
						OutputDebugString("Can't Write Plane Surface");
				}
				else
				{
					WriteGroup(ofile,ptr,nSeq,nGroup);
				}
				nGroup++;
				break;
			}
		}
	}
}

/**	
	@brief	write given primitives to file
	@author	humkyung
	@date	????.??.??
	@param	pFilePath	a parameter of type const char*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@return	void	
*/
void CNSQExporter::OnExportFile(const TCHAR* pFilePath , PSQPRIMITIVE pPrimitive , const string& sVer , const string& sLastWriteTime , const double& dOffsetX , const double& dOffsetY , const double& dOffsetZ)
{
	assert(pFilePath && "pFilePath is invalid");

	if(pFilePath)
	{
		ofstream ofile(pFilePath);
		if(ofile.is_open())
		{
			ofile.precision( 5 );         /// 소수점 자릿수 5
			ofile.setf(ios_base:: fixed, ios_base:: floatfield); /// 소수점 방식으로 표시됨

			/// 버젼 기록
			ofile << "#" << sVer.c_str() << std::endl;
			/// 모델 파일의 날짜를 기록할 것
			ofile << "#" << sLastWriteTime.c_str() << std::endl;

			long nSeq=0,nGroup=0;
			m_dModelOffset[0] = dOffsetX;
			m_dModelOffset[1] = dOffsetY;
			m_dModelOffset[2] = dOffsetZ;
			WriteElement(ofile , pPrimitive , nSeq , nGroup);

			ofile.close();
			theOpenSolid.Pure();
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteLine function

	@author	humkyung

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@return	void	
*/
void CNSQExporter::WriteLine(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");	

	if(pPrimitive)
	{
		ofile << _T("LINE,") << nGroup << _T(",");
		ofile << (pPrimitive->body.line.ptStart.x + m_dModelOffset[0]) << _T(",") << (pPrimitive->body.line.ptStart.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.line.ptStart.z + m_dModelOffset[2]) << _T(",");
		ofile << (pPrimitive->body.line.ptEnd.x + m_dModelOffset[0]) << _T(",") << (pPrimitive->body.line.ptEnd.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.line.ptEnd.z + m_dModelOffset[2]) << _T(",");
		ofile << _T("1,") << int(pPrimitive->hdr.display.lcolor) << std::endl;
	}
}

/// 2006.12.06
void CNSQExporter::WriteSweepSurface(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(!pPrimitive->hdr.attr.f) return;
		if(pPrimitive->hdr.attr.continuous)
		{
			ofile << _T("SWEEP=") << std::endl;
			ofile << "{" << std::endl;
			ofile << _T("COLOR=") << pPrimitive->hdr.display.lcolor << std::endl;
			size_t count = pPrimitive->body.pvtPoints->size();
			size_t half  = size_t(count * 0.5);
			for(int i = 0;i < int(half);++i)
			{
				POINT_T pt = (*(pPrimitive->body.pvtPoints))[i];
				ofile << "LHS=" << (pt.x + m_dModelOffset[0]) << "," << (pt.y + m_dModelOffset[1]) << "," <<  (pt.z + m_dModelOffset[2]) << std::endl;
			}

			for(int i = int(half);i < int(count);++i)
			{
				POINT_T pt = (*(pPrimitive->body.pvtPoints))[i];
				ofile << "RHS=" << (pt.x + m_dModelOffset[0]) << "," << (pt.y + m_dModelOffset[1]) << "," << (pt.z + m_dModelOffset[2]) << std::endl;
			}
			ofile << "}" << std::endl;
		}
	}
}

/**	
	@brief	write points

	@author	HumKyung Baek

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WritePoints(ofstream& ofile,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(!pPrimitive->hdr.attr.f) return;
		if(pPrimitive->hdr.attr.continuous)
		{
			if(false == isFace)
			{
				for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();(itr+1) != pPrimitive->body.pvtPoints->end();itr++)
				{
					ofile << _T("LINE,") << nGroup << _T(",");
					ofile << (itr->x + m_dModelOffset[0]) << _T(",") << (itr->y + m_dModelOffset[1]) << _T(",") << (itr->z + m_dModelOffset[2]) << _T(",");
					ofile << ((itr+1)->x + m_dModelOffset[0]) << _T(",") << ((itr+1)->y + m_dModelOffset[1]) << _T(",") << ((itr+1)->z + m_dModelOffset[2]) << _T(",");
					ofile << _T("1,") << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
			else
			{
				ofile << _T("FACE=") << std::endl;
				ofile << _T("{") << std::endl;
				ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
				for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();(itr) != pPrimitive->body.pvtPoints->end();++itr)
				{
					ofile << (itr->x + m_dModelOffset[0]) << _T(",") << (itr->y + m_dModelOffset[1]) << _T(",") << (itr->z + m_dModelOffset[2]) << std::endl;
				}
				ofile << _T("}") << std::endl;
			}
		}
	}
}

/**	
	@brief	write polyline

	@author	HumKyung Baek

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WritePolyline(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace)
{
	assert(pPrimitive && "pPrimitive is invalid");

	if(pPrimitive)
	{
		if(false == isFace)
		{
			for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr && ptr->next;ptr=ptr->next)
			{
				ofile << _T("LINE,") << nGroup << _T(",");
				ofile << (ptr->pt.x + m_dModelOffset[0]) << _T(",") << (ptr->pt.y + m_dModelOffset[1]) << _T(",") << (ptr->pt.z + m_dModelOffset[2]) << _T(",");
				ofile << (ptr->next->pt.x + m_dModelOffset[0]) << _T(",") << (ptr->next->pt.y + m_dModelOffset[1]) << _T(",") << (ptr->next->pt.z + m_dModelOffset[2]) << _T(",");
				ofile << ((true == ptr->visible) ? 1 : 0) << _T(",") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			}
		}
		else
		{
			ofile << _T("FACE=") << std::endl;
			ofile << _T("{") << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			for(PPOINT_TNODE ptr=pPrimitive->body.polyline;ptr;ptr=ptr->next)
			{
				ofile << (ptr->pt.x + m_dModelOffset[0]) << _T(",") << (ptr->pt.y + m_dModelOffset[1]) << _T(",") << (ptr->pt.z + m_dModelOffset[2]) << _T(",");
				ofile << ((true == ptr->visible) ? 1 : 0) << std::endl;
			}
			ofile << _T("}") << std::endl;
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteEllipse function

	@author	humkyung

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WriteEllipse(ofstream& ofile , PSQPRIMITIVE pPrimitive , long& nSeq , long nGroup , const bool& isFace , const bool& line_strip)
{
	assert(pPrimitive && "pPrimitive is NULL");	

	if(pPrimitive)
	{
		if( false == line_strip )
		{
			ofile << _T("ELLIPSE,") << nGroup << _T(",");
			ofile << (pPrimitive->body.ellipse.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pPrimitive->body.ellipse.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.ellipse.ptOrigin.z + m_dModelOffset[2]) << _T(",");
			ofile << pPrimitive->body.ellipse.nAxis[0] << _T(",") << pPrimitive->body.ellipse.nAxis[1] << _T(",");
			ofile << pPrimitive->body.ellipse.nStartAngle << _T(",") << pPrimitive->body.ellipse.nSweepAngle << _T(",");
			ofile << pPrimitive->body.ellipse.vecNorm.dx << _T(",") << pPrimitive->body.ellipse.vecNorm.dy << _T(",") << pPrimitive->body.ellipse.vecNorm.dz << _T(",");
			ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
			
			nSeq++;
		}
		else
		{
			CEllipse3d ellipse(pPrimitive->body.ellipse);
			ellipse.CreateSegments();
			const int nPoints = ellipse.GetNumOfPoints();

			if(false == isFace)
			{
				for(int i=0;i < nPoints - 1;i++)
				{
					POINT_T pt1 = CCurve3d::pt(i);
					POINT_T pt2 = CCurve3d::pt(i + 1);

					ofile << _T("LINE,") << nGroup << _T(",");
					ofile << (pt1.x + m_dModelOffset[0]) << _T(",") << (pt1.y + m_dModelOffset[1]) << _T(",") << (pt1.z + m_dModelOffset[2]) << _T(",");
					ofile << (pt2.x + m_dModelOffset[0]) << _T(",") << (pt2.y + m_dModelOffset[1]) << _T(",") << (pt2.z + m_dModelOffset[2]) << _T(",");
					ofile <<  _T("1,") << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
			else
			{
				if(3 != m_iModelType)	/// skip ellipse in structure model - 2015.03.24 added by humkyung
				{
					ofile << _T("FACE=") << std::endl;
					ofile << _T("{") << std::endl;
					ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
					for(int i=0;i < nPoints;++i)
					{
						POINT_T pt1 = CCurve3d::pt(i);
						ofile << (pt1.x + m_dModelOffset[0]) << _T(",") << (pt1.y + m_dModelOffset[1]) << _T(",") << (pt1.z + m_dModelOffset[2]) << std::endl;
					}
					ofile << _T("}") << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteArc function
	@author	humkyung
	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long
	@return	void	
*/
void CNSQExporter::WriteArc(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is invalid");

	if(pPrimitive)
	{
		if((pPrimitive->body.arc.ptOrigin.x == pPrimitive->body.arc.ptStart.x) && (pPrimitive->body.arc.ptOrigin.y == pPrimitive->body.arc.ptStart.y) && (pPrimitive->body.arc.ptOrigin.z == pPrimitive->body.arc.ptStart.z))
		{
			ofile << _T("ARC,") << nGroup << ",";
			ofile << (pPrimitive->body.arc.ptOrigin.x + m_dModelOffset[0]) << "," << (pPrimitive->body.arc.ptOrigin.y + m_dModelOffset[1]) << "," << (pPrimitive->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
			ofile << (pPrimitive->body.arc.ptStart.x + m_dModelOffset[0]) << "," << (pPrimitive->body.arc.ptStart.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.arc.ptStart.z + m_dModelOffset[2]) << _T(",");
			ofile << 0.0 << ",";
			ofile << 0.0 << _T(",") << 0.0 << _T(",") << 1.0 << _T(",");
			ofile << _T("1,1,");	/// primary , secondary axis
			ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
		}
		else
		{
			ofile << _T("ARC,") << nGroup << ",";
			ofile << (pPrimitive->body.arc.ptOrigin.x + m_dModelOffset[0]) << "," << (pPrimitive->body.arc.ptOrigin.y + m_dModelOffset[1]) << "," << (pPrimitive->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
			ofile << (pPrimitive->body.arc.ptStart.x + m_dModelOffset[0]) << "," << (pPrimitive->body.arc.ptStart.y + m_dModelOffset[1]) << "," << (pPrimitive->body.arc.ptStart.z + m_dModelOffset[2]) << ",";
			ofile << pPrimitive->body.arc.nSweep << _T(",");
			ofile << pPrimitive->body.arc.vecNorm.dx << _T(",") << pPrimitive->body.arc.vecNorm.dy << _T(",") << pPrimitive->body.arc.vecNorm.dz << _T(",");
			ofile << _T("1,1,");	/// primary , secondary axis
			ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
		}

		nSeq++;
	}
}

/**	
	@brief	The CNSQExporter::WriteVertices function

	@author	humkyung

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WriteVertices(ofstream& ofile,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup , const bool& isFace)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(false == isFace)
		{
			list<SQVERTEX>::iterator itrTmp=pPrimitive->body.plstVertices->begin();
			for(list<SQVERTEX>::iterator itr=pPrimitive->body.plstVertices->begin();itr != pPrimitive->body.plstVertices->end();itr = itrTmp)
			{
				itrTmp = itr;
				itrTmp++;
				if(itrTmp != pPrimitive->body.plstVertices->end())
				{
					while((itrTmp != pPrimitive->body.plstVertices->end()) && IsSamePoint(itr->x,itr->y,itr->z,itrTmp->x,itrTmp->y,itrTmp->z)) itrTmp++;
					if(itrTmp != pPrimitive->body.plstVertices->end())
					{
						ofile << _T("LINE,") << nGroup << _T(",");
						ofile << (itr->x + m_dModelOffset[0]) << _T(",") << (itr->y + m_dModelOffset[1]) << _T(",") << (itr->z + m_dModelOffset[2]) << _T(",");
						ofile << (itrTmp->x + m_dModelOffset[0]) << _T(",") << (itrTmp->y + m_dModelOffset[1]) << _T(",") << (itrTmp->z + m_dModelOffset[2]) << _T(",");
						ofile << ((itr->code.moveto) ? 0 : 1) << _T(",") << int(pPrimitive->hdr.display.lcolor) << std::endl;
					}
				}else	break;
			}
		}
		else
		{
			ofile << _T("FACE=") << std::endl;
			ofile << _T("{") << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			list<SQVERTEX>::iterator itrTmp=pPrimitive->body.plstVertices->begin();
			for(list<SQVERTEX>::iterator itr=pPrimitive->body.plstVertices->begin();itr != pPrimitive->body.plstVertices->end();itr = itrTmp)
			{
				ofile << (itr->x + m_dModelOffset[0]) << _T(",") << (itr->y + m_dModelOffset[1]) << _T(",") << (itr->z + m_dModelOffset[2]) << _T(",") << ((itr->code.moveto) ? 0 : 1) << std::endl;

				itrTmp = itr;
				itrTmp++;
				if(itrTmp != pPrimitive->body.plstVertices->end())
				{
					/// skip same point
					while((itrTmp != pPrimitive->body.plstVertices->end()) && IsSamePoint(itr->x,itr->y,itr->z,itrTmp->x,itrTmp->y,itrTmp->z)) itrTmp++;
				}else	break;
			}
			ofile << _T("}") << std::endl;
		}
	}
}

/**	
	@brief	write a face

	@author	HumKyung Baek

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WriteFace(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		ofile << _T("FACE=") << std::endl;
		ofile << _T("{") << std::endl;
		ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
		list<SQVERTEX>::iterator itrTmp=pPrimitive->body.plstVertices->begin();
		for(list<SQVERTEX>::iterator itr=pPrimitive->body.plstVertices->begin();itr != pPrimitive->body.plstVertices->end();++itr)
		{
			ofile << (itr->x + m_dModelOffset[0]) << _T(",") << (itr->y + m_dModelOffset[1]) << _T(",") << (itr->z + m_dModelOffset[2]) << _T(",") << ((itr->code.moveto) ? 0 : 1) << std::endl;
		}
		ofile << _T("}") << std::endl;
	}
}

/**	
	@brief	write solid primitive to text file

	@author	HumKyung Baek

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@return	void	
*/
void CNSQExporter::WriteSolid(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");	

	if(pPrimitive)
	{
		const static char sz[39] ="%d,LINE,%d,%lf,%lf,%lf,%lf,%lf,%lf\r\n";
		static POINT_T pt[2][256]={0,};
		int nCount[2]={0,};

		PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive;
		if(ptr && (SQ_POINTS == ptr->hdr.id))
		{
			for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
			{
				pt[0][nCount[0]]=*itr;
				(nCount[0])++;
			}
			ptr = ptr->next;
		}
		if(ptr && (SQ_POINTS == ptr->hdr.id))
		{
			for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
			{
				pt[1][nCount[1]]=*itr;
				(nCount[1])++;
			}
			ptr = ptr->next;
		}

		/// check if count of two shape's points are equal
		if(ptr && (nCount[0] > 0) && (nCount[1] > 0) && (nCount[0] == nCount[1]))
		{
			/// write one shape
			ofile << _T("FACE=") << std::endl;
			ofile << _T("{") << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			for(int i=0;i < nCount[0]/* - 1*/;i++)
			{
				ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
			}
			ofile << (pt[0][0].x + m_dModelOffset[0]) << _T(",") << (pt[0][0].y + m_dModelOffset[1]) << _T(",") << (pt[0][0].z + m_dModelOffset[2]) << _T(",1") << std::endl;
			ofile << _T("}") << std::endl;

			/// write another shape
			ofile << _T("FACE=") << std::endl;
			ofile << _T("{") << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			for(int i = 0;i < nCount[1] - 1;i++)
			{
				ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
			}
			ofile << (pt[1][0].x + m_dModelOffset[0]) << _T(",") << (pt[1][0].y + m_dModelOffset[1]) << _T(",") << (pt[1][0].z + m_dModelOffset[2]) << _T(",1") << std::endl;
			ofile << _T("}") << std::endl;

			/// write shapes which connect two shapes given
			for(int i = 0;i < nCount[0] - 1;i++)
			{
				ofile << _T("FACE=") << std::endl;
				ofile << _T("{") << std::endl;
				ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
				ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
				ofile << (pt[0][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[0][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[0][i+1].z + m_dModelOffset[2]) << _T(",1") << std::endl;
				ofile << (pt[1][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[1][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[1][i+1].z + m_dModelOffset[2]) << _T(",1") << std::endl;
				ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
				ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
				ofile << _T("}") << std::endl;
			}
		}
		else	/// try surface if not satisfied solid condition : added by HumKyung.Baek - 2010.11.12
		{
			WriteSurface(ofile , pPrimitive , nSeq , nGroup , SQ_SOLID);
		}

		++nGroup;
	}
}

/**	
	@brief	The GetPlaneSurfVector function

	@author	HumKyung Baek

	@date	????.??.??

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	VECTOR_T	
*/
VECTOR_T GetPlaneSurfVector(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	VECTOR_T vecNorm={0,};

	if(pPrimitive && (SQ_POINTS == pPrimitive->hdr.id))
	{
		POINT_T pt[3]={0,};

		int nCount=0;
		for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr)
		{
			if(nCount < 3){
				pt[nCount++]=(*itr);
			}else	break;
		}

		if(3 == nCount)
		{
			VECTOR_T vec[2]={0,};

			vec[0].dx = pt[1].x - pt[0].x;
			vec[0].dy = pt[1].y - pt[0].y;
			vec[0].dz = pt[1].z - pt[0].z;
			vec[1].dx = pt[2].x - pt[0].x;
			vec[1].dy = pt[2].y - pt[0].y;
			vec[1].dz = pt[2].z - pt[0].z;
			CMath::NormalizeVector(vec[0]);
			CMath::NormalizeVector(vec[1]);

			vecNorm = CMath::CrossProduct(vec[0],vec[1]);
		}
	}

	return vecNorm;
}

/**	
	@brief	The _copy function
	@author	humkyung
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE _copy(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive && (SQ_POINTS == pPrimitive->hdr.id))
	{
		pRet = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE));
		if(pRet)
		{
			memcpy(&(pRet->hdr),&(pPrimitive->hdr),sizeof(SQHEADER));
			pRet->body.pvtPoints = new vector<POINT_T>;
			if(pRet->body.pvtPoints)
			{
				for(vector<POINT_T>::iterator itr=pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr)
					pRet->body.pvtPoints->push_back(*itr);
			}
		}
	}

	return pRet;
}

/**	
	@brief	write a plane surface

	@author	HumKyung Baek

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@date	????.??.??

	@return	bool	
*/
bool CNSQExporter::WritePlaneSurf(ofstream& ofile , PSQPRIMITIVE pPrimitive , long& nSeq , long& nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive)
	{
		PSQPRIMITIVE pElm=(PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE));
		if(pElm)
		{
			PLANESURF_T* pPlaneSurf = (PLANESURF_T*)calloc(1,sizeof(PLANESURF_T));
			if(pPlaneSurf)
			{
				pPlaneSurf->ptOrigin = pPrimitive->hdr.ptOrigin;
				if((pPlaneSurf->ptOrigin.x < pPrimitive->hdr.volume.x[0]) ||
					(pPlaneSurf->ptOrigin.y < pPrimitive->hdr.volume.y[0]) || 
					(pPlaneSurf->ptOrigin.z < pPrimitive->hdr.volume.z[0]) ||
					(pPlaneSurf->ptOrigin.x > pPrimitive->hdr.volume.x[1]) ||
					(pPlaneSurf->ptOrigin.y < pPrimitive->hdr.volume.y[1]) ||
					(pPlaneSurf->ptOrigin.z < pPrimitive->hdr.volume.z[1]))
				{
					pPlaneSurf->ptOrigin.x = (pPrimitive->hdr.volume.x[0] + pPrimitive->hdr.volume.x[1]) * 0.5;
					pPlaneSurf->ptOrigin.y = (pPrimitive->hdr.volume.y[0] + pPrimitive->hdr.volume.y[1]) * 0.5;
					pPlaneSurf->ptOrigin.z = (pPrimitive->hdr.volume.z[0] + pPrimitive->hdr.volume.z[1]) * 0.5;
				}

				PSQPRIMITIVE pSurfElm=pPrimitive->body.pPrimitive;
				if(pSurfElm)
				{
					pPlaneSurf->vecNorm   = GetPlaneSurfVector(pSurfElm);

					PSQPRIMITIVE pBoundaryNext=NULL;
					for(PSQPRIMITIVE pBoundary=pSurfElm;pBoundary;pBoundary = pBoundaryNext)
					{
						pBoundaryNext   = pBoundary->next;
						pBoundary->next = NULL;

						PSQPRIMITIVE pGroup=(PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE));
						if(pGroup)
						{
							pGroup->hdr.attr = pBoundary->hdr.attr;
							pGroup->body.pPrimitive = pBoundary;
							pPlaneSurf->pBoundary = ListPush(pPlaneSurf->pBoundary,pGroup);
						}
					}
					pElm->body.pPlaneSurf = pPlaneSurf;

					PSQPRIMITIVE pFacets=theOpenSolid.CreatePlaneSurface(pElm);
					for(PSQPRIMITIVE ptr=pFacets;ptr;ptr = ptr->next)
					{
						if(SQ_VERTICES == ptr->hdr.id) WriteFace(ofile ,ptr,nSeq,nGroup++);
					}
					bRet = true;
				}
			}
		}
	}

	return bRet;
}

/**	
	@brief	SURFACE PRIMITIVE의 내용을 파일로 저장한다.
	SURF_OF_REVOLUTION , SWEEP_SURF의 2가지 형태를 지원한다.

	@author	HumKyung.Baek

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@remarks
	;2003.06.09 - write the surface as <CONE> which's one radius is zero.

	@return	void	
*/
void CNSQExporter::WriteSurface(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq , long& nGroup , const int& iType)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(SURF_OF_REVOLUTION == pPrimitive->body.pSurface->nType)
		{
			if(1 == pPrimitive->body.pSurface->nBoundary)
			{
				SQSURFACE::PSURFELM_T pSurf =pPrimitive->body.pSurface->pElement;
				if(NULL == pSurf) return;
				if(IsTORS_EQP(pSurf))
				{
					PSQPRIMITIVE pBoundary=pSurf->pBoundaryCurve;
					POINT_T ptStart   =pBoundary->body.ellipse.ptOrigin;
					double nTubeRadius=pBoundary->body.ellipse.nAxis[0];

					PSQPRIMITIVE ptr=NULL;
					for(ptr=pBoundary;ptr && ptr->next;ptr = ptr->next);
					POINT_T ptEnd = ptr->body.ellipse.ptOrigin;

					PSQPRIMITIVE pRuleCurve=pSurf->pRuleCurve;
					POINT_T ptOrigin   =pRuleCurve->body.ellipse.ptOrigin;
					double dx=ptStart.x - ptOrigin.x;
					double dy=ptStart.y - ptOrigin.y;
					double dz=ptStart.z - ptOrigin.z;
					const double nTorusRadius = sqrt(dx*dx + dy*dy + dz*dz);
					VECTOR_T vecNorm   =pRuleCurve->body.ellipse.vecNorm;
					CMath::NormalizeVector(vecNorm);
					VECTOR_T vecStart={ptStart.x - ptOrigin.x,ptStart.y - ptOrigin.y,ptStart.z - ptOrigin.z};
					CMath::NormalizeVector(vecStart);
					VECTOR_T vecEnd  ={ptEnd.x - ptOrigin.x,ptEnd.y - ptOrigin.y,ptEnd.z - ptOrigin.z};
					CMath::NormalizeVector(vecEnd);

					double nStartAng = DEG2RAD(pRuleCurve->body.ellipse.nStartAngle);
					double nSweepAng = DEG2RAD(pRuleCurve->body.ellipse.nSweepAngle);
					//if(nEndAng > 2*PI) nEndAng -= 2*PI;

					ofile << _T("TORS_EQP,");
					ofile << (ptOrigin.x + m_dModelOffset[0]) << _T(",") << (ptOrigin.y + m_dModelOffset[1]) << _T(",") << (ptOrigin.z + m_dModelOffset[2]) << _T(",");
					ofile << nTorusRadius << _T(",") << nTubeRadius << _T(",");
					ofile << vecNorm.dx << _T(",") << vecNorm.dy << _T(",") << vecNorm.dz << _T(",");
					ofile << vecStart.dx << _T(",") << vecStart.dy << _T(",") << vecStart.dz << _T(",");
					ofile << nStartAng << _T(",") << nSweepAng << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
				else
				{
					PSQPRIMITIVE pSurf=theOpenSolid.CreateSurface(pPrimitive);
					if(0 == strcmp(pSurf->hdr.szDesc, _T("DOME")))
					{
						WriteDome(ofile ,pSurf,nSeq,nGroup);
					}
					else if(0 == strcmp(pSurf->hdr.szDesc, _T("SPHERE")))
					{
						WriteSphere(ofile ,pSurf,nSeq,nGroup);
					}
					else
					{
						for(PSQPRIMITIVE ptr=pSurf;ptr;ptr = ptr->next)
						{
							if(ENM_POLYLINE == ptr->hdr.id) WritePolyline(ofile ,ptr,nSeq , nGroup, true);
						}
					}
				}
			}
			else
			{
				PSQPRIMITIVE pSurf=theOpenSolid.CreateSurface(pPrimitive);
				for(PSQPRIMITIVE ptr=pSurf;ptr;ptr = ptr->next)
				{
					if(ENM_POLYLINE == ptr->hdr.id) WritePolyline(ofile ,ptr,nSeq,nGroup , true);
				}
			}
		}
		else if(SWEEP_SURF == pPrimitive->body.pSurface->nType)
		{
			WriteSweepSurf(ofile , pPrimitive , nSeq , nGroup , iType);
		}
		else
		{
			if(pPrimitive->body.pSurface && pPrimitive->body.pSurface->pElement && pPrimitive->body.pSurface->pElement->pBoundaryCurve)
			{
				for(PSQPRIMITIVE ptr = pPrimitive->body.pSurface->pElement->pBoundaryCurve;ptr;ptr = ptr->next)
				{
					if(!ptr->hdr.attr.f) continue;
					switch(ptr->hdr.id)
					{
					case ENM_LINE:
						WriteLine(ofile ,ptr,nSeq,nGroup);
						nGroup++;
						break;
					case ENM_ELLIPSE:
						WriteEllipse(ofile ,ptr,nSeq , nGroup , true , true);
						break;
					case ENM_ARC:
						WriteArc(ofile ,ptr,nSeq,nGroup);
						nGroup++;
						break;
					case ENM_POLYLINE:
						WritePolyline(ofile ,ptr,nSeq,nGroup , true);
						break;
					case SQ_POINTS:
						if(0 == strcmp(ptr->hdr.szDesc, _T("LOOP")))
						{
							WritePoints(ofile ,ptr,nSeq,nGroup , true);
						}
						else if(0 == strcmp(ptr->hdr.szDesc, _T("3D_FACE")))
						{
							Write3dFace(ofile ,ptr,nSeq,nGroup);
							nGroup++;
						}
						else if(0 == strcmp(ptr->hdr.szDesc , _T("SWEEP")))
						{
							WriteSweepSurface(ofile , ptr , nSeq , nGroup);
							nGroup++;
						}
						break;
					case SQ_VERTICES:
						WriteFace(ofile , ptr , nSeq , nGroup);
						break;
					case SQ_LOOP:
						WriteLoop(ofile ,ptr,nSeq,nGroup);
						nGroup++;
						break;
					case SQ_SOLID:
						WriteSolid(ofile ,ptr,nSeq,nGroup);
						//nGroup++;
						break;
					case SQ_CONE:
						WriteCone(ofile ,ptr,nSeq,nGroup);
						nGroup++;
						break;
					case SQ_SURFACE:
						try
						{
							if(0 == strcmp(ptr->hdr.szDesc, _T("ELBOW")))
								WriteElbowSurf(ofile ,ptr,nSeq,nGroup);
							else if(0 == strcmp(ptr->hdr.szDesc, _T("DOME")))
								WriteDome(ofile ,ptr,nSeq,nGroup);
							else	WriteSurface(ofile ,ptr,nSeq,nGroup);
							nGroup++;
						}
						catch(...)
						{

						}
						break;
					case SQ_CONESURFACE:
						if(0 == strcmp(ptr->hdr.szDesc,"CONE"))
						{
							if(2 == ptr->body.pConeSurf->nLoop)
							{
								PSQPRIMITIVE pBoundaryCrv[2]={ptr->body.pConeSurf->pBoundary[0],ptr->body.pConeSurf->pBoundary[1]};
								if((ENM_ARC == pBoundaryCrv[0]->hdr.id) && (ENM_ARC == pBoundaryCrv[1]->hdr.id))
								{
									VECTOR_T vec={0.,};
									vec.dx = pBoundaryCrv[0]->body.arc.ptStart.x - pBoundaryCrv[0]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[0]->body.arc.ptStart.y - pBoundaryCrv[0]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[0]->body.arc.ptStart.z - pBoundaryCrv[0]->body.arc.ptOrigin.z;
									double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
									vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
									double radius_b=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
									vec = ptr->body.pConeSurf->vecDelta;

									ofile << _T("CONE,");
									ofile << radius_a << _T(",") << radius_b << _T(",");
									ofile << (pBoundaryCrv[0]->body.arc.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
									ofile << (pBoundaryCrv[1]->body.arc.ptOrigin.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.z + m_dModelOffset[2] - vec.dz) << _T(",");
									ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
									ofile << int(ptr->hdr.display.lcolor) << std::endl;
								}
								else if((ENM_ARC == pBoundaryCrv[0]->hdr.id) && (ENM_POINT == pBoundaryCrv[1]->hdr.id))
								{
									VECTOR_T vec={0.,};
									vec.dx = pBoundaryCrv[0]->body.arc.ptStart.x - pBoundaryCrv[0]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[0]->body.arc.ptStart.y - pBoundaryCrv[0]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[0]->body.arc.ptStart.z - pBoundaryCrv[0]->body.arc.ptOrigin.z;
									const double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
									/*
									vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
									*/
									const double radius_b = 0.f;
									vec = ptr->body.pConeSurf->vecDelta;

									ofile << _T("CONE,");
									ofile << radius_a << _T(",") << radius_b << _T(",");
									ofile << (pBoundaryCrv[0]->body.arc.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[0]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
									ofile << (pBoundaryCrv[1]->body.point.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[1]->body.point.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[1]->body.point.z + m_dModelOffset[2] - vec.dz) << _T(",");
									ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
									ofile << int(ptr->hdr.display.lcolor) << std::endl;
								}
								else if((ENM_POINT == pBoundaryCrv[0]->hdr.id) && (ENM_ARC == pBoundaryCrv[1]->hdr.id))
								{
									VECTOR_T vec={0.,};
									vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
									const double radius_a=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
									/*
									vec.dx = pBoundaryCrv[1]->body.arc.ptStart.x - pBoundaryCrv[1]->body.arc.ptOrigin.x;
									vec.dy = pBoundaryCrv[1]->body.arc.ptStart.y - pBoundaryCrv[1]->body.arc.ptOrigin.y;
									vec.dz = pBoundaryCrv[1]->body.arc.ptStart.z - pBoundaryCrv[1]->body.arc.ptOrigin.z;
									*/
									const double radius_b = 0.f;
									vec = ptr->body.pConeSurf->vecDelta;

									ofile << _T("CONE,");
									ofile << radius_a << _T(",") << radius_b << _T(",");
									ofile << (pBoundaryCrv[1]->body.arc.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pBoundaryCrv[1]->body.arc.ptOrigin.z + m_dModelOffset[2]) << _T(",");
									ofile << (pBoundaryCrv[0]->body.point.x + m_dModelOffset[0] - vec.dx) << _T(",") << (pBoundaryCrv[0]->body.point.y + m_dModelOffset[1] - vec.dy) << _T(",") << (pBoundaryCrv[0]->body.point.z + m_dModelOffset[2] - vec.dz) << _T(",");
									ofile << vec.dx << _T(",") << vec.dy << _T(",") << vec.dz << _T(",");
									ofile << int(ptr->hdr.display.lcolor) << std::endl;
								}
							}
						}
						break;
					case ENM_GROUP:
						if(0 == strcmp(ptr->hdr.szDesc, _T("PIPE")))
						{
							for(PSQPRIMITIVE ptr1=ptr->body.pPrimitive;ptr1;ptr1 = ptr1->next)
							{
								if(ptr1->hdr.pLayer && (m_nSkipLevel == ptr1->hdr.pLayer[0])) continue;

								if(SQ_CONE == ptr1->hdr.id)
								{
									POINT_T pt[2]={0};

									double nRadius = ptr1->body.cone.nRadius[0];
									pt[0]  = ptr1->body.cone.ptOrigin[0];
									pt[1]  = ptr1->body.cone.ptOrigin[1];

									ofile << _T("PIPE,");
									ofile << nRadius << _T(",");
									ofile << (pt[0].x + m_dModelOffset[0]) << _T(",") << (pt[0].y + m_dModelOffset[1]) << _T(",") << (pt[0].z + m_dModelOffset[2]) << _T(",");
									ofile << (pt[1].x + m_dModelOffset[0]) << _T(",") << (pt[1].y + m_dModelOffset[1]) << _T(",") << (pt[1].z + m_dModelOffset[2]) << _T(",");
									ofile << int(ptr->hdr.display.lcolor) << std::endl;
								}
							}
						}
						else if(0 == strcmp(ptr->hdr.szDesc, _T("ELBOW")))
						{
							CElbowExporter elbowExporter;
							elbowExporter.WriteElbow(ofile ,ptr,nSeq,nGroup , m_nSkipLevel);
							nGroup++;
						}
						else if(0 == strcmp(ptr->hdr.szDesc, _T("TEE")))
						{
							WriteTee(ofile ,ptr,nSeq,nGroup);
							nGroup++;
						}
						else if(IsPlaneSurfWithHole(ptr))
						{
							if(!WritePlaneSurf(ofile ,ptr,nSeq,nGroup))
								OutputDebugString("Can't Write Plane Surface");
						}
						else
						{
							WriteGroup(ofile ,ptr,nSeq,nGroup);
						}
						nGroup++;
						break;
					}
				}
			}
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteSphere function
	@author	humkyung
	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long
	@return	void	
*/
void CNSQExporter::WriteSphere(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");	

	if(pPrimitive)
	{
		if(0)
		{
			ofile << _T("SPHERE,") << nGroup << ",";
			ofile << (pPrimitive->body.pSphereSurf->ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pPrimitive->body.pSphereSurf->ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.pSphereSurf->ptOrigin.z + m_dModelOffset[2]) << _T(",");
			ofile << pPrimitive->body.pSphereSurf->nRadius << _T(",") << int(pPrimitive->hdr.display.lcolor) << std::endl;
		}
		else
		{
			POINT_T ptOrigin={0,};
			double nRadius=0.;

			PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;
			if(pPrimitive && (SQ_SURFACE == pPrimitive->hdr.id) && (SURF_OF_REVOLUTION == pPrimitive->body.pSurface->nType))
			{
				SQSURFACE::PSURFELM_T pSurfElm=pPrimitive->body.pSurface->pElement;

				if(pSurfElm->pBoundaryCurve && (ENM_ELLIPSE == pSurfElm->pBoundaryCurve->hdr.id))
				{
					nRadius = pSurfElm->pBoundaryCurve->body.ellipse.nAxis[0];
				}
				if(pSurfElm->pRuleCurve && (ENM_ELLIPSE == pSurfElm->pRuleCurve->hdr.id))
				{
					ptOrigin = pSurfElm->pRuleCurve->body.ellipse.ptOrigin;
				}

				if(nRadius > 0.)
				{
					ofile << _T("SPHERE,");
					ofile << nRadius << _T(",");
					ofile << (ptOrigin.x + m_dModelOffset[0]) << _T(",") << (ptOrigin.y + m_dModelOffset[1]) << _T(",") << (ptOrigin.z + m_dModelOffset[2]) << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	write torus attributes

	@author	hummkyung

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WriteTorus(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");	

	if(pPrimitive)
	{
		ofile << _T("TORUS,") << nGroup << _T(",");
		ofile << (pPrimitive->body.pTorusSurf->ptOrigin.x + m_dModelOffset[0]) << _T(",") << (pPrimitive->body.pTorusSurf->ptOrigin.y + m_dModelOffset[1]) << _T(",") << (pPrimitive->body.pTorusSurf->ptOrigin.z + m_dModelOffset[2]) << _T(",");
		ofile << pPrimitive->body.pTorusSurf->nTorusRadius << _T(",");
		ofile << pPrimitive->body.pTorusSurf->nTubeRadius << _T(",");
		ofile << pPrimitive->body.pTorusSurf->vecNorm.dx << _T(",") << pPrimitive->body.pTorusSurf->vecNorm.dy << _T(",") << pPrimitive->body.pTorusSurf->vecNorm.dz << _T(",");
		ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
	}
}

/**	
	@brief	The CNSQExporter::WriteLoop function

	@author	HumKyung.Baek

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long

	@return	void	
*/
void CNSQExporter::WriteLoop(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(!pPrimitive->hdr.attr.f) return;
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next)
		{
			switch(ptr->hdr.id)
			{
			case ENM_LINE:
				WriteLine(ofile ,ptr,nSeq,nGroup);
				break;
			case ENM_ELLIPSE:
				WriteEllipse(ofile , ptr , nSeq , nGroup , false , true);
				break;
			case ENM_POLYLINE:
				WritePolyline(ofile ,ptr,nSeq,nGroup , ptr->hdr.attr.closed);
				if(ptr->hdr.attr.closed) nGroup++;
				break;
			case ENM_ARC:
				WriteArc(ofile ,ptr,nSeq,nGroup);
				break;
			case SQ_POINTS:
				if(0 == strcmp(ptr->hdr.szDesc, _T("LOOP")))
				{
					WritePoints(ofile ,ptr,nSeq,nGroup , true);
				}
				else
				{
					WritePoints(ofile ,ptr,nSeq,nGroup , false);
				}
				break;
			case SQ_VERTICES:
				WriteVertices(ofile ,ptr,nSeq,nGroup , false);
				break;
			case SQ_CONE:
				WriteCone(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_SURFACE:
				WriteSurface(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			}
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteGroup function
	@author	humkyung
	@date	????.??.??
	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&
	@return	void	
*/
void CNSQExporter::WriteGroup(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is invalid");	

	if(pPrimitive)
	{
		if(!pPrimitive->hdr.attr.f) return;
		/// write layer
		if(pPrimitive->hdr.szDesc[0])
		{
			ofile << _T("[") << pPrimitive->hdr.szDesc << _T("]") << std::endl;
		}
		/// up to here
		for(PSQPRIMITIVE ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next)
		{
			switch(ptr->hdr.id)
			{
			case ENM_LINE:
				WriteLine(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case ENM_ELLIPSE:
				WriteEllipse(ofile , ptr , nSeq , nGroup , true , false);
				break;
			case ENM_POLYLINE:
				WritePolyline(ofile ,ptr,nSeq,nGroup , ptr->hdr.attr.closed);
				if(ptr->hdr.attr.closed) nGroup++;
				break;
			case ENM_ARC:
				WriteArc(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_POINTS:	///< points data를 write한다.
				if(0 == strcmp(ptr->hdr.szDesc, _T("LOOP")))
				{
					WritePoints(ofile ,ptr,nSeq,nGroup , true);
				}
				else
				{
					WritePoints(ofile ,ptr,nSeq,nGroup , true);
				}
				break;
			case SQ_VERTICES:
				WriteVertices(ofile ,ptr,nSeq,nGroup , true);
				break;
			case SQ_LOOP:
				WriteLoop(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_CONE:
				WriteCone(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case SQ_SURFACE:
				WriteSurface(ofile ,ptr,nSeq,nGroup);
				nGroup++;
				break;
			case ENM_GROUP:
				WriteGroup(ofile ,ptr,nSeq,nGroup);
				break;
			default:
				nGroup++;
				break;
			}
		}
		/// write layer
		if(pPrimitive->hdr.szDesc[0])
		{
			ofile << _T("[/LAYER]") << std::endl;
		}
		/// up to here
	}
}

/**	
	@brief	The CNSQExporter::WriteCone function

	@author	humkyung

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@return	void	
*/
void CNSQExporter::WriteCone(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is invalid");	

	if(pPrimitive)
	{
		if(pPrimitive->hdr.pLayer && (m_nSkipLevel == pPrimitive->hdr.pLayer[0])) return;

		if(0 == strcmp(pPrimitive->hdr.szDesc, _T("PIPE")))
		{
			POINT_T pt[2]={0};
			double  radius={0.};

			radius = pPrimitive->body.cone.nRadius[0];
			pt[0]  = pPrimitive->body.cone.ptOrigin[0];
			pt[1]  = pPrimitive->body.cone.ptOrigin[1];

			ofile << _T("PIPE,");
			ofile << radius << _T(",");
			ofile << (pt[0].x + m_dModelOffset[0]) << _T(",") << (pt[0].y + m_dModelOffset[1]) << _T(",") << (pt[0].z + m_dModelOffset[2]) << _T(",");
			ofile << (pt[1].x + m_dModelOffset[0]) << _T(",") << (pt[1].y + m_dModelOffset[1]) << _T(",") << (pt[1].z + m_dModelOffset[2]) << _T(",");
			ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
		}
		else
		{
			POINT_T ptCenter[2]={0};
			double  nRadius[2]={0.};

			nRadius[0] = pPrimitive->body.cone.nRadius[0];
			nRadius[1] = pPrimitive->body.cone.nRadius[1];
			ptCenter[0]= pPrimitive->body.cone.ptOrigin[0];
			ptCenter[1]= pPrimitive->body.cone.ptOrigin[1];

			if(fabs(nRadius[0] - nRadius[1]) < 0.00001)
			{
				ofile << _T("CYLN,");
				ofile << nRadius[0] << _T(",");
				ofile << (ptCenter[0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0].z + m_dModelOffset[2]) << _T(",");
				ofile << (ptCenter[1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1].z + m_dModelOffset[2]) << _T(",");
				ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
			}
			else
			{
				VECTOR_T V , axis;
				V.dx = ptCenter[1].x - ptCenter[0].x;
				V.dy = ptCenter[1].y - ptCenter[0].y;
				V.dz = ptCenter[1].z - ptCenter[0].z;
				axis = pPrimitive->body.cone.vecAxis;
				if(CMath::NormalizeVector(axis))
				{
					const double length = CMath::DotProduct(V , axis);

					VECTOR_T sideAxis;
					sideAxis.dx = V.dx - length * axis.dx;
					sideAxis.dy = V.dy - length * axis.dy;
					sideAxis.dz = V.dz - length * axis.dz;

					ofile << _T("CONE,");
					ofile << nRadius[0] << _T(",") << nRadius[1] << _T(",");
					ofile << (ptCenter[0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[1].x + m_dModelOffset[0] - sideAxis.dx) << _T(",") << (ptCenter[1].y + m_dModelOffset[1] - sideAxis.dy) << _T(",") << (ptCenter[1].z + m_dModelOffset[2] - sideAxis.dz) << _T(",");
					ofile << sideAxis.dx << _T(",") << sideAxis.dy << _T(",") << sideAxis.dz << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteElbowSurf function
	@author	humkyung
	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@return	void	
*/
void CNSQExporter::WriteElbowSurf(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
#ifdef CADWORX
		POINT_T pt[2]={0,};
		PSQPRIMITIVE pBoundaryCurve = pPrimitive->body.pSurface->pElement->pBoundaryCurve;
		pt[0] = pBoundaryCurve->body.arc.ptOrigin;
		pt[1] = pBoundaryCurve->next->body.arc.ptOrigin;

		POINT_T ptOrigin = pPrimitive->body.pSurface->pElement->pRuleCurve->body.ellipse.ptOrigin;

		double dx[2]={0,},dy[2]={0,},dz[2]={0,};
		dx[0] = pt[0].x - ptOrigin.x;
		dy[0] = pt[0].y - ptOrigin.y;
		dz[0] = pt[0].z - ptOrigin.z;
		dx[1] = pt[1].x - ptOrigin.x;
		dy[1] = pt[1].y - ptOrigin.y;
		dz[1] = pt[1].z - ptOrigin.z;

		POINT_T ptCross={0,};
		ptCross.x = ptOrigin.x + dx[0] + dx[1];
		ptCross.y = ptOrigin.y + dy[0] + dy[1];
		ptCross.z = ptOrigin.z + dz[0] + dz[1];

		dx[0] = pt[0].x - ptOrigin.x;dy[0] = pt[0].y - ptOrigin.y;dz[0] = pt[0].z - ptOrigin.z;
		double nTorusRadius = sqrt(dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]);
		dx[0] = pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptStart.x - 
			pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptOrigin.x;
		dy[0] = pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptStart.y - 
			pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptOrigin.y;
		dz[0] = pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptStart.z - 
			pPrimitive->body.pSurface->pElement->pBoundaryCurve->body.arc.ptOrigin.z;
		double nTubeRadius = sqrt(dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]);
		fprintf(fp,"<ELBO>,"
			"%.5lf,%.5lf,%.5lf,"
			"%.5lf,%.5lf,%.5lf,"
			"%.5lf,%.5lf,%.5lf,"
			"%.5lf,%.5lf,%.5lf,"
			"%.5lf,"
			"%.5lf,\r\n",
			pt[0].x,pt[0].y,pt[0].z,
			pt[1].x,pt[1].y,pt[1].z,
			ptCross.x,ptCross.y,ptCross.z,
			ptOrigin.x,ptOrigin.y,ptOrigin.z,
			nTorusRadius,
			nTubeRadius);
#endif
	}
}

/**	
	@brief	The CNSQExporter::WriteDome function

	@author	HumKyung.BAEK

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@return	void	
*/
void CNSQExporter::WriteDome(ofstream& ofile ,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		POINT_T ptOrigin={0,} , ptTop={0,};
		VECTOR_T vecNorm={0,},vecBoundaryNorm={0,};
		double nRotate=0.f , nStartAng=0.;
		double nRadius=0.,nHeight=0.;

		PSQPRIMITIVE pElement=pPrimitive->body.pPrimitive;
		if(pPrimitive && (SQ_SURFACE == pPrimitive->hdr.id) && (SURF_OF_REVOLUTION == pPrimitive->body.pSurface->nType))
		{
			SQSURFACE::PSURFELM_T pSurfElm=pPrimitive->body.pSurface->pElement;

			if(pSurfElm->pBoundaryCurve && (ENM_ELLIPSE == pSurfElm->pBoundaryCurve->hdr.id))
			{
				VECTOR_T vecStart = {pSurfElm->pBoundaryCurve->body.ellipse.nAxis[0] , 0 , 0};
				VECTOR_T vecEnd = vecStart;

				nHeight = pSurfElm->pBoundaryCurve->body.ellipse.nAxis[1];

				double alpha=0.,beta=0.;
				CGeometry::GetRotatedAngleOfAxis(pSurfElm->pBoundaryCurve->body.ellipse.vecNorm,alpha,beta);
				///vecStart = CGeometry::RotateAboutXYAxis(vecStart,alpha,beta);
				///vecEnd   = CGeometry::RotateAboutXYAxis(vecEnd,alpha,beta);
				const double nRotate = pSurfElm->pBoundaryCurve->body.ellipse.nRotate;
				vecStart = CGeometry::RotateAboutZAxis(vecStart , DEG2RAD(nRotate + pSurfElm->pBoundaryCurve->body.ellipse.nSweepAngle));
				vecEnd   = CGeometry::RotateAboutZAxis(vecEnd , DEG2RAD(nRotate));
				vecStart = CGeometry::RotateAboutYXAxis(vecStart , -beta , -alpha);
				vecEnd   = CGeometry::RotateAboutYXAxis(vecEnd , -beta , -alpha);
				ptTop.x  = pSurfElm->pBoundaryCurve->body.ellipse.ptOrigin.x + (vecStart.dx + vecEnd.dx)*0.5;
				ptTop.y  = pSurfElm->pBoundaryCurve->body.ellipse.ptOrigin.y + (vecStart.dy + vecEnd.dy)*0.5;
				ptTop.z  = pSurfElm->pBoundaryCurve->body.ellipse.ptOrigin.z + (vecStart.dz + vecEnd.dz)*0.5;

				nStartAng = pSurfElm->pBoundaryCurve->body.ellipse.nStartAngle + pSurfElm->pBoundaryCurve->body.ellipse.nRotate;
				vecBoundaryNorm = pSurfElm->pBoundaryCurve->body.ellipse.vecNorm;

			}
			else if(pSurfElm->pBoundaryCurve && (ENM_ARC == pSurfElm->pBoundaryCurve->hdr.id))
			{
				VECTOR_T vecStart;
				vecStart.dx=pSurfElm->pBoundaryCurve->body.arc.ptStart.x - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.x;
				vecStart.dy=pSurfElm->pBoundaryCurve->body.arc.ptStart.y - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.y;
				vecStart.dz=pSurfElm->pBoundaryCurve->body.arc.ptStart.z - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.z;
				nHeight = sqrt(vecStart.dx*vecStart.dx + vecStart.dy*vecStart.dy + vecStart.dz*vecStart.dz)/(pSurfElm->pBoundaryCurve->body.arc.nRatio);

				double alpha=0.,beta=0.;
				CGeometry::GetRotatedAngleOfAxis(pSurfElm->pBoundaryCurve->body.arc.vecNorm,alpha,beta);
				vecStart = CGeometry::RotateAboutXYAxis(vecStart,alpha,beta);
				vecStart = CGeometry::RotateAboutZAxis(vecStart , DEG2RAD(pSurfElm->pBoundaryCurve->body.arc.nSweep));
				vecStart = CGeometry::RotateAboutYXAxis(vecStart , -beta , -alpha);
				ptTop.x  = pSurfElm->pBoundaryCurve->body.arc.ptOrigin.x + vecStart.dx;
				ptTop.y  = pSurfElm->pBoundaryCurve->body.arc.ptOrigin.y + vecStart.dy;
				ptTop.z  = pSurfElm->pBoundaryCurve->body.arc.ptOrigin.z + vecStart.dz;

				nStartAng = RAD2DEG(CGeometry::GetRotatedAngleInXYPlane(vecStart));
				vecBoundaryNorm = pSurfElm->pBoundaryCurve->body.arc.vecNorm;
			}

			if(pSurfElm->pRuleCurve && (ENM_ELLIPSE == pSurfElm->pRuleCurve->hdr.id))
			{
				ptOrigin = pSurfElm->pRuleCurve->body.ellipse.ptOrigin;
				vecNorm = pSurfElm->pRuleCurve->body.ellipse.vecNorm;
				nRadius = pSurfElm->pRuleCurve->body.ellipse.nAxis[0];
			}
			else if(pSurfElm->pRuleCurve && (ENM_ARC == pSurfElm->pRuleCurve->hdr.id))
			{
				ptOrigin = pSurfElm->pRuleCurve->body.arc.ptOrigin;
				vecNorm = pSurfElm->pRuleCurve->body.arc.vecNorm;
				double dx=pSurfElm->pRuleCurve->body.arc.ptStart.x - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.x;
				double dy=pSurfElm->pRuleCurve->body.arc.ptStart.y - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.y;
				double dz=pSurfElm->pRuleCurve->body.arc.ptStart.z - pSurfElm->pBoundaryCurve->body.arc.ptOrigin.z;
				nRadius = sqrt(dx*dx + dy*dy + dz*dz);
			}

			if((nRadius > 0.) && (nHeight > 0.))
			{
				if(((1-fabs(vecNorm.dx)) < 0.001) && (vecNorm.dx > 0.))
				{
					if((nStartAng >= 90) && (nStartAng <= 180))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
				}
				else if(((1-fabs(vecNorm.dx)) < 0.001) && (vecNorm.dx < 0.))
				{
					if((nStartAng >= 270) && (nStartAng <= 360))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
				}
				else if(((1-fabs(vecNorm.dy)) < 0.001) && (vecNorm.dy > 0.))
				{
					if((fabs(1 - vecBoundaryNorm.dz) < 0.001) && (nStartAng >= 180) && (nStartAng <= 270))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
					else if((fabs(1 - fabs(vecBoundaryNorm.dx)) < 0.001) && (nStartAng >= 180) && (nStartAng <= 270))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
				}
				else if(((1-fabs(vecNorm.dy)) < 0.001) && (vecNorm.dy < 0.))
				{
					if((fabs(1 - vecBoundaryNorm.dz) < 0.001) && (nStartAng >= 0) && (nStartAng <= 90))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
					else if((fabs(1 - fabs(vecBoundaryNorm.dx)) < 0.001) && (nStartAng >= 0) && (nStartAng <= 90))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
				}
				else if(((1-fabs(vecNorm.dz)) < 0.001) && (vecNorm.dz > 0.))
				{
					if((nStartAng >= 90) && (nStartAng <= 180))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
					else
					{
						const double d = -(vecNorm.dx*ptOrigin.x + vecNorm.dy*ptOrigin.y + vecNorm.dz*ptOrigin.z);

						const double h = vecNorm.dx*ptTop.x + vecNorm.dy*ptTop.y + vecNorm.dz*ptTop.z + d;
						if(h < 0)
						{
							vecNorm.dx = -vecNorm.dx;
							vecNorm.dy = -vecNorm.dy;
							vecNorm.dz = -vecNorm.dz;
						}
					}
				}
				else if(((1-fabs(vecNorm.dz)) < 0.001) && (vecNorm.dz < 0.))
				{
					if((nStartAng >= 270) && (nStartAng <= 360))
					{
						vecNorm.dx = -vecNorm.dx;vecNorm.dy = -vecNorm.dy;vecNorm.dz = -vecNorm.dz;
					}
					else
					{
						const double d = -(vecNorm.dx*ptOrigin.x + vecNorm.dy*ptOrigin.y + vecNorm.dz*ptOrigin.z);

						const double h = vecNorm.dx*ptTop.x + vecNorm.dy*ptTop.y + vecNorm.dz*ptTop.z + d;
						if(h < 0)
						{
							vecNorm.dx = -vecNorm.dx;
							vecNorm.dy = -vecNorm.dy;
							vecNorm.dz = -vecNorm.dz;
						}
					}
				}
				else
				{
					const double d = -(vecNorm.dx*ptOrigin.x + vecNorm.dy*ptOrigin.y + vecNorm.dz*ptOrigin.z);

					const double h = vecNorm.dx*ptTop.x + vecNorm.dy*ptTop.y + vecNorm.dz*ptTop.z + d;
					if(h < 0)
					{
						vecNorm.dx = -vecNorm.dx;
						vecNorm.dy = -vecNorm.dy;
						vecNorm.dz = -vecNorm.dz;
					}
				}

				ofile << _T("DOME,") << nRadius << _T(",") << nHeight << _T(",");
				ofile << (ptOrigin.x + m_dModelOffset[0]) << _T(",") << (ptOrigin.y + m_dModelOffset[1]) << _T(",") << (ptOrigin.z + m_dModelOffset[2]) << _T(",");
				ofile << vecNorm.dx << _T(",") << vecNorm.dy << _T(",") << vecNorm.dz << _T(",") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			}
		}
	}
}

/**	
	@brief	The CNSQExporter::WriteTee function

	@author	humkyung

	@date	????.??.??

	@param	fp	a parameter of type FILE*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSeq	a parameter of type long&
	@param	nGroup	a parameter of type long&

	@remarks
	;2005-12-15 - write TEE SW consisting of 5 CONEs

	@return	void	
*/
void CNSQExporter::WriteTee(ofstream& ofile,PSQPRIMITIVE pPrimitive,long& nSeq,long& nGroup)
{
	assert(pPrimitive && "pPrimitive is invalid");	

	if(pPrimitive)
	{
		POINT_T ptCenter[6][2]={0,};
		double  nRadius[6]={0.};

		PSQPRIMITIVE pElm=pPrimitive->body.pPrimitive;
		pElm = pElm->next; //! skip point string

		int nPipes=0;
		for(;pElm && (nPipes < 6);pElm = pElm->next)
		{
			if(pElm->hdr.pLayer && (m_nSkipLevel == pElm->hdr.pLayer[0])) continue;

			if(SQ_CONE == pElm->hdr.id)
			{
				nRadius[nPipes] = pElm->body.cone.nRadius[0];
				ptCenter[nPipes][0] = pElm->body.cone.ptOrigin[0];
				ptCenter[nPipes][1] = pElm->body.cone.ptOrigin[1];
				nPipes++;
			}else	break;
		}

		if(2 == nPipes)	/// TEE BW
		{			
			if((nRadius[0] > 0.) && (nRadius[1] > 0.))
			{
				if(!IsSamePoint(ptCenter[0][0],ptCenter[0][1]) && !IsSamePoint(ptCenter[1][0],ptCenter[1][1]))
				{
					ofile << _T("TEE,");
					ofile << nRadius[0] << _T(",");
					ofile << (ptCenter[0][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[0][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[1] << _T(",");
					ofile << (ptCenter[1][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[1][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][1].z + m_dModelOffset[2]) << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}
		else if(3 == nPipes)	/// for strainer(SAMSUNG)
		{
			if((nRadius[0] > 0.) && (nRadius[1] > 0.) && (nRadius[2] > 0.))
			{
				if(IsSamePoint(ptCenter[0][1],ptCenter[1][0]) && IsSamePoint(ptCenter[0][1],ptCenter[2][0]))
				{
					ofile << _T("TEE,");
					ofile << nRadius[0] << _T(",");
					ofile << (ptCenter[0][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[1][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[2] << _T(",");
					ofile << (ptCenter[2][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[2][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][1].z + m_dModelOffset[2]) << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}
		else if(4 == nPipes)
		{
			if((nRadius[2] > 0.) && (nRadius[3] > 0.))
			{
				if(!IsSamePoint(ptCenter[2][0],ptCenter[2][1]) && !IsSamePoint(ptCenter[3][0],ptCenter[3][1]))
				{
					ofile << _T("TEE,");
					ofile << nRadius[2] << _T(",");
					ofile << (ptCenter[2][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[2][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[3] << _T(",");
					ofile << (ptCenter[3][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[3][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][1].z + m_dModelOffset[2]) << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}
		else if(5 == nPipes)	/// TEE SW - 2005-12-15
		{
			ofile << _T("TEE,");
			ofile << nRadius[1] << _T(",");
			ofile << (ptCenter[1][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][0].z + m_dModelOffset[2]) << _T(",");
			ofile << (ptCenter[1][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][1].z + m_dModelOffset[2]) << _T(",");
			ofile << nRadius[3] << _T(",");
			ofile << (ptCenter[3][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][0].z + m_dModelOffset[2]) << _T(",");
			ofile << (ptCenter[3][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][1].z + m_dModelOffset[2]) << _T(",");
			ofile << nRadius[0] << _T(",");
			ofile << (ptCenter[0][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][0].z + m_dModelOffset[2]) << _T(",");
			ofile << (ptCenter[0][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][1].z + m_dModelOffset[2]) << _T(",");
			ofile << nRadius[2] << _T(",");
			ofile << (ptCenter[2][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][0].z + m_dModelOffset[2]) << _T(",");
			ofile << (ptCenter[2][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][1].z + m_dModelOffset[2]) << _T(",");
			ofile << nRadius[4] << _T(",");
			ofile << (ptCenter[4][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[4][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[4][0].z + m_dModelOffset[2]) << _T(",");
			ofile << (ptCenter[4][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[4][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[4][1].z + m_dModelOffset[2]) << _T(",");
			ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
		}
		else if(6 == nPipes)	/// TEE SW
		{
			if((nRadius[0] > 0.) && (nRadius[1] > 0.))
			{
				if(!IsSamePoint(ptCenter[0][0],ptCenter[0][1]) && !IsSamePoint(ptCenter[1][0],ptCenter[1][1]))
				{
					ofile << _T("TEE,");
					ofile << nRadius[1] << _T(",");
					ofile << (ptCenter[1][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[1][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[1][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[2][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[2][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[2][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[0] << _T(",");
					ofile << (ptCenter[0][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[0][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[0][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[0][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[3] << _T(",");
					ofile << (ptCenter[3][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[3][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[3][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[3][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[4] << _T(",");
					ofile << (ptCenter[4][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[4][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[4][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[4][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[4][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[4][1].z + m_dModelOffset[2]) << _T(",");
					ofile << nRadius[5] << _T(",");
					ofile << (ptCenter[5][0].x + m_dModelOffset[0]) << _T(",") << (ptCenter[5][0].y + m_dModelOffset[1]) << _T(",") << (ptCenter[5][0].z + m_dModelOffset[2]) << _T(",");
					ofile << (ptCenter[5][1].x + m_dModelOffset[0]) << _T(",") << (ptCenter[5][1].y + m_dModelOffset[1]) << _T(",") << (ptCenter[5][1].z + m_dModelOffset[2]) << _T(",");
					ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
				}
			}
		}

		//! if still element remains, write remained elements -  2011.05.04 added by humkyung
		if(NULL != pElm)
		{
			WriteElement(ofile , pElm , nSeq , nGroup);
		}
	}
}

/**	
	@brief	The CNSQExporter::IsPlaneSurfWithHole function

	@author	humkyung

	@param	pSurf	a parameter of type PSQPRIMITIVE

	@return	bool	
*/
bool CNSQExporter::IsPlaneSurfWithHole(PSQPRIMITIVE pSurf)
{
	assert(pSurf && "pSurf is NULL");
	bool bRet=false;

	if(pSurf && (ENM_GROUP == pSurf->hdr.id))
	{
		for(PSQPRIMITIVE ptr=pSurf->body.pPrimitive;ptr;ptr = ptr->next)
		{
			if(SQ_POINTS != ptr->hdr.id) return false;
			if(ptr->hdr.attr.h) bRet = true;
		}
	}
	return bRet;
}

/**	
	@brief	The CNSQExporter::IsTORS_EQP function

	@author	humkyung

	@param	pSurf	a parameter of type SQSURFACE::PSURFELM_T

	@return	bool	
*/
bool CNSQExporter::IsTORS_EQP(SQSURFACE::PSURFELM_T pSurf)
{
	assert(pSurf && "pSurf is NULL");
	bool bRet=false;

	if(pSurf)
	{
		for(PSQPRIMITIVE ptr=pSurf->pBoundaryCurve;ptr;ptr = ptr->next)
		{
			if(!((ENM_ELLIPSE == ptr->hdr.id) && 
				(0.     == ptr->body.ellipse.nStartAngle) &&
				(360.   == ptr->body.ellipse.nSweepAngle))) return false;
		}

		for(PSQPRIMITIVE ptr=pSurf->pRuleCurve;ptr;ptr = ptr->next)
		{
			if(ENM_ELLIPSE != ptr->hdr.id) return false;
		}

		return true;
	}

	return true;
}

/**	
	@brief	check if two point are same

	@author	humkyung

	@date	????.??.??

	@param	pt1	a parameter of type const POINT_T&
	@param	pt2	a parameter of type const POINT_T&

	@return	bool	
*/
bool CNSQExporter::IsSamePoint(const POINT_T& pt1,const POINT_T& pt2)
{
	double dx=pt2.x - pt1.x;
	double dy=pt2.y - pt1.y;
	double dz=pt2.z - pt1.z;
	double l =sqrt(dx*dx + dy*dy + dz*dz);

	return (l < 0.0000000001);
}

/**	
	@brief	The CNSQExporter::IsSamePoint function
	@author	humkyung
	@param	x1	a parameter of type double
	@param	y1	a parameter of type double
	@param	z1	a parameter of type double
	@param	x2	a parameter of type double
	@param	y2	a parameter of type double
	@param	z2	a parameter of type double

	@return	bool	
*/
bool CNSQExporter::IsSamePoint(double x1,double y1,double z1,double x2,double y2,double z2)
{
	double dx=x2 - x1;
	double dy=y2 - y1;
	double dz=z2 - z1;
	double l =sqrt(dx*dx + dy*dy + dz*dz);

	if(l < 0.0000000001) return true;
	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-07
    @class      CNSQExporter
    @function   Write3dFace
    @return     void
    @param      ofstream&       ofile
    @param      PSQPRIMITIVE    pPrimitive
    @param      long            &nSeq
    @param      long            nGroup
    @brief
******************************************************************************/
void CNSQExporter::Write3dFace(ofstream& ofile , PSQPRIMITIVE pPrimitive, long &nSeq, long nGroup)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		if(!pPrimitive->hdr.attr.f) return;
		if(pPrimitive->hdr.attr.continuous)
		{
			ofile << _T("FACE=") << std::endl;
			ofile << _T("{") << std::endl;
			ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
			vector<POINT_T>* pvt=pPrimitive->body.pvtPoints;			
			for(vector<POINT_T>::iterator itr=pvt->begin();itr != pvt->end();itr++)
			{
				ofile << _T(",") << ((*itr).x + m_dModelOffset[0]) << _T(",") << ((*itr).y + m_dModelOffset[1]) << _T(",") << ((*itr).z + m_dModelOffset[2]) << std::endl;
			}
			ofile << _T("}") << std::endl;
		}
	}
}

/**
	@brief	SWEEP_SURF의 내용을 저장한다.

	@author	HumKyung.Baek

	@param	fp = file pointer
	@param	pPrimitive = SWEEP_SURF의 내용을 담고 있는 DATA STRUCT.
	@param	nSeq = 
	@param	nGroup = 

	@return
*/
bool CNSQExporter::WriteSweepSurf(ofstream& ofile , PSQPRIMITIVE pPrimitive, long &nSeq, long &nGroup , const int& iType)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		static POINT_T pt[2][256]={0};
		int nCount[2]={0};

		const int nBoundary = pPrimitive->body.pSurface->nBoundary;
		if(1 == nBoundary)
		{
			for(SQSURFACE::PSURFELM_T ptrElement=pPrimitive->body.pSurface->pElement;ptrElement;ptrElement = ptrElement->next)
			{
				PSQPRIMITIVE ptr=ptrElement->pBoundaryCurve;
				if(ptr && (SQ_POINTS == ptr->hdr.id) && (ptr->next && (SQ_POINTS == ptr->next->hdr.id)))
				{
					if(ptr && (SQ_POINTS == ptr->hdr.id))
					{
						for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
						{
							pt[0][nCount[0]]=*itr;
							(nCount[0])++;
						}
						ptr = ptr->next;
					}
					if(ptr && (SQ_POINTS == ptr->hdr.id))
					{
						for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
						{
							pt[1][nCount[1]]=*itr;
							(nCount[1])++;
						}
						ptr = ptr->next;
					}

					if(nCount[0] == nCount[1])
					{
						ofile << _T("FACE=") << std::endl;
						ofile << _T("{") << std::endl;
						ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
						for(int i = 0;i < nCount[0];++i)
						{
							ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << std::endl;
						}
						ofile << _T("}") << std::endl;

						ofile << _T("FACE=") << std::endl;
						ofile << _T("{") << std::endl;
						ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
						for(int i=0;i < nCount[1] - 1;i++)
						{
							ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << std::endl;
						}
						ofile << _T("}") << std::endl;

						for(int i=0;i < nCount[0] - 1;i++)
						{
							ofile << _T("FACE=") << std::endl;
							ofile << _T("{") << std::endl;
							ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;

							ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << std::endl;
							ofile << (pt[0][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[0][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[0][i+1].z + m_dModelOffset[2]) << std::endl;					
							ofile << (pt[1][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[1][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[1][i+1].z + m_dModelOffset[2]) << std::endl;
							ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << std::endl;
							
							ofile << _T("}") << std::endl;
						}
					}
				}
				else if(ptr && (ENM_ELLIPSE == ptr->hdr.id) && (ptr->next && (ENM_ELLIPSE == ptr->next->hdr.id)))
				{
					if(	((0. == ptr->body.ellipse.nAxis[0])       && (0. == ptr->body.ellipse.nAxis[1])) || 
						((0. == ptr->next->body.ellipse.nAxis[0]) && (0. == ptr->next->body.ellipse.nAxis[1])))
					{
						ofile << _T("CONE,");
						ofile << ptr->body.ellipse.nAxis[0] << _T(",") << ptr->next->body.ellipse.nAxis[0] << _T(",");
						ofile << (ptr->body.ellipse.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (ptr->body.ellipse.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (ptr->body.ellipse.ptOrigin.z + m_dModelOffset[2]) << _T(",");
						ofile << (ptr->next->body.ellipse.ptOrigin.x + m_dModelOffset[0]) << _T(",") << (ptr->next->body.ellipse.ptOrigin.y + m_dModelOffset[1]) << _T(",") << (ptr->next->body.ellipse.ptOrigin.z + m_dModelOffset[2]) << _T(",");
						ofile << int(pPrimitive->hdr.display.lcolor) << std::endl;
					}
					else
					{
						/// create two curves
						if(ptr && (ENM_ELLIPSE == ptr->hdr.id))
						{
							CEllipse3d ellipse(ptr->body.ellipse);
							ellipse.CreateSegments();
							int nPoints = ellipse.GetNumOfPoints();

							for(int i=0;i < nPoints;i++)
							{
								pt[0][nCount[0]] = CCurve3d::pt(i);
								(nCount[0])++;
							}
							ptr = ptr->next;
						}
						if(ptr && (ENM_ELLIPSE == ptr->hdr.id))
						{
							CEllipse3d ellipse(ptr->body.ellipse);
							ellipse.CreateSegments();
							int nPoints = ellipse.GetNumOfPoints();

							for(int i=0;i < nPoints;i++)
							{
								pt[1][nCount[1]] = CCurve3d::pt(i);
								(nCount[1])++;
							}
							ptr = ptr->next;
						}

						if(nCount[0] == nCount[1])
						{
							for(int i = 0;i < nCount[0] - 1;i++)
							{
								ofile << _T("FACE=") << std::endl;
								ofile << _T("{") << std::endl;
								ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;

								ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",");
								ofile << 1 << std::endl;
								
								ofile << (pt[0][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[0][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[0][i+1].z + m_dModelOffset[2]) << _T(",");
								ofile << (((nCount[0] - 1) == (i + 1)) ? 1 : 0) << std::endl;
								
								ofile << (pt[1][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[1][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[1][i+1].z + m_dModelOffset[2]) << _T(",");
								ofile << 1 << std::endl;

								ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << _T(",");
								ofile << ((0 == i) ? 1 : 0) << std::endl;
								
								ofile << _T("}") << std::endl;
							}
						}
					}
				}
				else if(ptr && (SQ_LOOP == ptr->hdr.id) && (ptr->next && (SQ_LOOP == ptr->next->hdr.id)))
				{
					PSQPRIMITIVE pRuleCurve = ptrElement->pRuleCurve;
					if(pRuleCurve && (ENM_LINE == pRuleCurve->hdr.id))
					{
						ofile << _T("EXTR=") << std::endl;
						ofile << _T("{") << std::endl;
						ofile << _T("LOOP=") << std::endl;
						ofile << _T("{") << std::endl;
						WriteLoop(ofile , ptr , nSeq , nGroup);
						ofile << _T("}") << std::endl;

						double dx = pRuleCurve->body.line.ptEnd.x - pRuleCurve->body.line.ptStart.x;
						double dy = pRuleCurve->body.line.ptEnd.y - pRuleCurve->body.line.ptStart.y;
						double dz = pRuleCurve->body.line.ptEnd.z - pRuleCurve->body.line.ptStart.z;
						double height = sqrt(dx*dx + dy*dy + dz*dz);
						ofile << _T("HEIGHT=") << height << std::endl;
						ofile << _T("AXIS=") << dx/height << _T(",") << dy/height << _T(",") << dz/height << std::endl;
						ofile << _T("DESC=") << pPrimitive->hdr.szDesc << std::endl;
						if(SQ_SURFACE == iType)
						{
							ofile << _T("TYPE=SURFACE") << std::endl;
						}
						else
						{
							ofile << _T("TYPE=SOLID") << std::endl;
						}
						ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
						ofile << _T("}") << std::endl;
					}
				}

				return true;
			}
		}
		/// 2011.08.18 - added by humkyung
		else if(nBoundary > 1)
		{
			for(SQSURFACE::PSURFELM_T ptrElement=pPrimitive->body.pSurface->pElement;ptrElement;ptrElement = ptrElement->next)
			{
				memset(pt[0] , '\0' , sizeof(POINT_T)*256);
				memset(pt[1] , '\0' , sizeof(POINT_T)*256);
				nCount[0] = nCount[1] = 0;

				int nIndex = 0 , i = 0;
				for(PSQPRIMITIVE ptr = ptrElement->pBoundaryCurve;ptr != NULL;ptr = ptr->next , ++i)
				{
					if(ENM_ELLIPSE == ptr->hdr.id)
					{
						CEllipse3d ellipse(ptr->body.ellipse);
						ellipse.CreateSegments();
						const int nPoints = ellipse.GetNumOfPoints();

						if(0 == nCount[nIndex])
						{
							pt[nIndex][nCount[nIndex]] = CCurve3d::pt(0);
							(nCount[nIndex])++;
						}
						else if((nCount[nIndex] > 0) && !IsSamePoint(pt[nIndex][nCount[nIndex] - 1] , CCurve3d::pt(0)))
						{
							pt[nIndex][nCount[nIndex]] = CCurve3d::pt(0);
							(nCount[nIndex])++;
						}
						for(int j = 1;j < nPoints;++j)
						{
							pt[nIndex][nCount[nIndex]] = CCurve3d::pt(j);
							(nCount[nIndex])++;
						}
					}
					else if(ENM_LINE == ptr->hdr.id)
					{
						if(0 == nCount[nIndex])
						{
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptStart;
							(nCount[nIndex])++;
						}
						else if((nCount[nIndex] > 0) && !IsSamePoint(pt[nIndex][nCount[nIndex] - 1] , ptr->body.line.ptStart))
						{
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptStart;
							(nCount[nIndex])++;
						}
						pt[nIndex][nCount[nIndex]] = ptr->body.line.ptEnd;
						(nCount[nIndex])++;
					}
					else if(SQ_POINTS == ptr->hdr.id)
					{
						if(!ptr->body.pvtPoints->empty())
						{
							if(0 == nCount[nIndex])
							{
								pt[nIndex][nCount[nIndex]] = (*(ptr->body.pvtPoints))[0];
								(nCount[nIndex])++;
							}
							else if((nCount[nIndex] > 0) && !IsSamePoint(pt[nIndex][nCount[nIndex] - 1] , (*(ptr->body.pvtPoints))[0]))
							{
								pt[nIndex][nCount[nIndex]] = (*(ptr->body.pvtPoints))[0];
								(nCount[nIndex])++;
							}
							for(int j = 1;j < int(ptr->body.pvtPoints->size());++j)
							{
								pt[nIndex][nCount[nIndex]] = (*(ptr->body.pvtPoints))[j];
								(nCount[nIndex])++;
							}
						}
					}

					if((0 == nIndex) && ((i + 1) >= nBoundary)) ++nIndex;
				}

				if(nCount[0] == nCount[1])
				{
					ofile << _T("FACE=") << std::endl;
					ofile << _T("{") << std::endl;
					ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
					for(int i = 0;i < nCount[0];++i)
					{
						ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
					}
					ofile << _T("}") << std::endl;

					ofile << _T("FACE=") << std::endl;
					ofile << _T("{") << std::endl;
					ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;
					for(int i = 0;i < nCount[1];++i)
					{
						ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
					}
					ofile << _T("}") << std::endl;

					for(int i = 0;i < nCount[0] - 1;++i)
					{
						ofile << _T("FACE=") << std::endl;
						ofile << _T("{") << std::endl;
						ofile << _T("COLOR=") << int(pPrimitive->hdr.display.lcolor) << std::endl;

						ofile << (pt[0][i].x + m_dModelOffset[0]) << _T(",") << (pt[0][i].y + m_dModelOffset[1]) << _T(",") << (pt[0][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
						ofile << (pt[0][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[0][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[0][i+1].z + m_dModelOffset[2]) << _T(",1") << std::endl;
						ofile << (pt[1][i+1].x + m_dModelOffset[0]) << _T(",") << (pt[1][i+1].y + m_dModelOffset[1]) << _T(",") << (pt[1][i+1].z + m_dModelOffset[2]) << _T(",1") << std::endl;
						ofile << (pt[1][i].x + m_dModelOffset[0]) << _T(",") << (pt[1][i].y + m_dModelOffset[1]) << _T(",") << (pt[1][i].z + m_dModelOffset[2]) << _T(",1") << std::endl;
						
						ofile << _T("}") << std::endl;
					}
				}
			}

			return true;
		}
		/// up to here
	}

	return false;
}
