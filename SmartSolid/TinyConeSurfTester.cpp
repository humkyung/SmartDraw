// TinyConeSurfTester.cpp: implementation of the CTinyConeSurfTester class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TinyConeSurfTester.h"

#include <sstream>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTinyConeSurfTester::CTinyConeSurfTester(bool bOutput , const int& surf_num) : m_surf_num(surf_num)
{
	m_bOutput = bOutput;
}

CTinyConeSurfTester::~CTinyConeSurfTester()
{

}

void CTinyConeSurfTester::OutputTestString(list<CGrid*>& lstGrid , snode<LINE_T>* pEdge)
{
	assert(pEdge && "pEdge is NULL");

	if((true == m_bOutput) && pEdge)
	{
		FILE* fp=NULL;
		
		ostringstream oss;
		oss << "d:\\opensolid_" << m_surf_num << ".txt";

		if(fp = fopen(oss.str().c_str() ,"wb"))
		{
			try
			{
				fprintf(fp , "[GRID]\r\n");
				for(list<CGrid*>::iterator itr = lstGrid.begin();itr != lstGrid.end();++itr)
				{
					snode<POINT_T>* ptr = NULL;
					for(ptr = (*itr)->m_pNode;ptr && ptr->next;ptr = ptr->next)
					{
						fprintf(fp,"L %.9lf,%.9lf %.9lf,%.9lf \r\n",
							ptr->element.x       , ptr->element.y,
							ptr->next->element.x , ptr->next->element.y);
					}
					if(ptr)
					{
						fprintf(fp,"L %.9lf,%.9lf %.9lf,%.9lf \r\n",
							ptr->element.x       , ptr->element.y,
							(*itr)->m_pNode->element.x , (*itr)->m_pNode->element.y);
					}
				}
				fprintf(fp , "\r\n");
			}
			catch(...)
			{
			}
			
			fprintf(fp , "[EDGE]\r\n");
			for(snode<LINE_T>* ptr=pEdge;ptr;ptr = ptr->next)
			{
				fprintf(fp,"L %.9lf,%.9lf %.9lf,%.9lf \r\n",
					ptr->element.ptStart.x,ptr->element.ptStart.y,
					ptr->element.ptEnd.x,  ptr->element.ptEnd.y);
			}
			fprintf(fp , "\r\n");
			fclose(fp);
		}
	}
}
