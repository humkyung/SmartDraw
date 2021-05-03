// DGNLibTestSuit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../include/DGNLib.h"

int main(int argc, char* argv[])
{
	if(2 == argc)
	{
		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		if(pDGNLib && pDGNLib->Load(argv[1]))
		{
			CDGNDoc* pDoc=pDGNLib->GetDoc();
			if(pDoc)
			{
				for(PSQPRIMITIVE ptr = pDoc->GetHead();NULL != ptr;ptr = ptr->next)
				{

				}
			}
		}
	}
	else
	{
		printf("usage : DGNLibTestSuit.exe <dgn file name>\n");
	}

	return 0;
}
