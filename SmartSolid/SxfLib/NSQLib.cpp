// NSQLib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <SQLib.h>
#include "NSQImporter.h"
#include "NSQExporter.h"


class CNSQFile
{
public:
	CNSQFile(){}
	~CNSQFile(){}
public:
	CNSQImporter m_clsImporter;
	CNSQExporter m_clsExporter;
};

CNSQFile theNSQFile;

//------------------------------------------------------------------

/**	\brief	The NSQImportFile function

	\param	pFilePath	a parameter of type const char*

	\return	PSQPRIMITIVE

	
*/
PSQPRIMITIVE NSQImportFile(const char* pFilePath)
{
	assert(pFilePath && "pFilePath is invalid");
	PSQPRIMITIVE pRet=NULL;

	if(pFilePath)
	{
		pRet=theNSQFile.m_clsImporter.OnImportFile(pFilePath);
	}

	return pRet;
}

/**	
	@brief	pPrimitive 리스트의 내용을 파일로 저장한다.
	@author	humkyung
	@param	pFilePath	a parameter of type const char*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@param	nSkipLevel	a parameter of type const int&
	@remarks	엔터티중에서 nSkipLevel에 위치한 엔터티는 파일로 저장되지 않는다.
	@return	void	
*/
void SxfExportFile(const TCHAR* pFilePath , const int& iModelType , PSQPRIMITIVE pPrimitive , const int& nSkipLevel , 
				   const string& sVer , const string& sLastWriteTime , const double& dOffsetX , const double& dOffsetY ,  const double& dOffsetZ)
{
	assert(pFilePath && "pFilePath is NULL");
	assert(pPrimitive&& "pPrimitive is NULL");

	if(pFilePath && pPrimitive)
	{
		theNSQFile.m_clsExporter.m_nSkipLevel = nSkipLevel;
		theNSQFile.m_clsExporter.m_iModelType = iModelType;
		theNSQFile.m_clsExporter.OnExportFile(pFilePath , pPrimitive , sVer , sLastWriteTime , dOffsetX , dOffsetY , dOffsetZ);
	}
}

/**	
	@brief	The FreeNSQPrimitive function
	@author	humkyung
	@param	pPrimitive	a parameter of type PSQPRIMITIVE
	@return	void	
*/
void FreeNSQPrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is invalid");

	if(pPrimitive){
		PSQPRIMITIVE ptrPrev=NULL;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;){
			switch(ptr->hdr.id){
				case SQ_CONESURFACE:
					free((void*)ptr->body.pConeSurf);
					ptr->body.pConeSurf = NULL;
				break;
				case SQ_SPHERESURFACE:
					free((void*)ptr->body.pSphereSurf);
					ptr->body.pSphereSurf = NULL;
				break;
				case SQ_TORUSSURFACE:
					free((void*)ptr->body.pTorusSurf);
					ptr->body.pTorusSurf = NULL;
				break;
			}
			ptrPrev = ptr;
			ptr = ptr->next;
			free((void*)ptrPrev);
		}
	}
}
