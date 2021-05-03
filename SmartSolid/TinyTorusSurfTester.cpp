// TinyTorusSurfTester.cpp: implementation of the CTinyTorusSurfTester class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "surface3d.h"
#include "TinyTorusSurfTester.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTinyTorusSurfTester::CTinyTorusSurfTester(bool bOutput) : m_bOutput(bOutput)
{
}

CTinyTorusSurfTester::~CTinyTorusSurfTester()
{

}

void CTinyTorusSurfTester::OutputTestString()
{	
	if(true == m_bOutput)
	{
		FILE* fp=NULL;
		if(fp = fopen("d:\\opensolid.txt","wb"))
		{
			fprintf(fp,"[POINT]\r\n");
			for(int i = 0;i < m_nSegments + 1;i++)
			{
				for(int j = 0;j < m_nSegments + 1;j++)
				{
					POINT_T pt={0,};
					
					pt.x = CSurface3D::para[m_nRows*i + j].u();
					pt.y = CSurface3D::para[m_nRows*i + j].v();
					fprintf(fp,"%.9lf,%.9lf\r\n",pt.x,pt.y);
				}
			}
			fprintf(fp,"[EDGE]\r\n");
			for(int i = 0;i < m_nBoundaries;i++)
			{
				fprintf(fp,"%.9lf,%.9lf,%.9lf,%.9lf\r\n",
					CLibMesh2d::lnBoundary[i].ptStart.x,CLibMesh2d::lnBoundary[i].ptStart.y,
					CLibMesh2d::lnBoundary[i].ptEnd.x,CLibMesh2d::lnBoundary[i].ptEnd.y
					);
			}
			fclose(fp);
		}
	}
}
