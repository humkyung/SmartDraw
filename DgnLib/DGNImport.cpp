#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <DGNLib.h>
#include "DGNStruct.h"
#include "DGNImport.h"
#include <windows.h>

/**	\brief
*/
CDGNImporter::CDGNImporter()
{
	m_plstImportedPrimitive = new list<PSQPRIMITIVE>;
	m_pDGNParser  = NULL;
}

/**	\brief
*/
CDGNImporter::~CDGNImporter()
{
	try
	{
		if(m_plstImportedPrimitive) delete m_plstImportedPrimitive;
		if(m_pDGNParser)  delete m_pDGNParser;
	}
	catch(...)
	{
	}
}

/**	
	@brief	read primitive from dgn file and clipping by volume.

	@author	HumKyung

	@date	????.??.??

	@param	pImportFilePath	a parameter of type const char*

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CDGNImporter::OnImportFile(const char* pImportFilePath)
{
	assert(pImportFilePath && "pImportFilePath is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	PSQPRIMITIVE pRet=NULL;

	if(pImportFilePath && pDGNLib)
	{	
		CDGNFile* hDGN=NULL;
		if(hDGN = (CDGNFile*)OpenFile(pImportFilePath))
		{
			if(NULL == m_pDGNParser) m_pDGNParser = new CDGNParser;
			if(m_pDGNParser)
			{
				pRet = m_pDGNParser->Parse(hDGN);
			}
			CloseFile(hDGN);
		}
	}

	return pRet;
}

/**	\brief	The CDGNImporter::OnImportFile function

	\param	pImportFilePath	a parameter of type const char*
	\param	pVolume	a parameter of type PSQVOLUME

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CDGNImporter::OnImportFile(const char* pImportFilePath,PSQVOLUME pVolume)
{
	assert(pImportFilePath && "pImportFilePath is NULL");
	assert(pVolume	       && "pVolume is NULL");
	PSQPRIMITIVE pRet=NULL;
	
	if(pImportFilePath && pVolume)
	{
		PSQPRIMITIVE pPrimitive=NULL;
		CDGNFile* hDGN=NULL;
		if(hDGN = (CDGNFile*)OpenFile(pImportFilePath))
		{
			if(NULL == m_pDGNParser) m_pDGNParser = new CDGNParser;
			if(m_pDGNParser)
			{
				pRet = m_pDGNParser->Parse(hDGN,pVolume);
			}
			CloseFile(hDGN);
		}
	}
	
	return pRet;
}

/**	\brief	The CDGNImporter::FreeImportedPrimitive function


	\return	void	
*/
void CDGNImporter::FreeImportedPrimitive()
{
	list<PSQPRIMITIVE>::iterator itr;
	for(itr = m_plstImportedPrimitive->begin();itr != m_plstImportedPrimitive->end();)
	{
		if(*itr) free((void*)*itr);
		m_plstImportedPrimitive->erase(itr++);
	}
}

/**	\brief	The CDGNImporter::OpenFile function

	\param	pszFilePath	a parameter of type const char *

	\return	CDGNFile*	
*/
CDGNFile* CDGNImporter::OpenFile(const char * pszFilePath)
{
	assert(pszFilePath && "pszFilePath is invalid-[DGNFOpen]");
	CDGNFile* hDGN=NULL;

	if(pszFilePath)
	{
		FILE *fp=NULL;
		if(NULL == (fp = fopen(pszFilePath,"rb")))
		{
			return NULL;
		}
		
		// Verify the format ... add later.                                
		unsigned char abyHeader[4]={0};
		fread(abyHeader,1,4,fp);
		fseek(fp,0L,SEEK_SET);
		
		if(NULL != (hDGN = (CDGNFile*)CDGNLib::AllocMemory(sizeof(CDGNFile))))
		{
			hDGN->fp = fp;
			hDGN->next_element_id = 1;
			
			hDGN->got_tcb = false;
			hDGN->dimension= 2;
			hDGN->scale    = 1.0;
			hDGN->m_ptOrigin.x = 0.0;
			hDGN->m_ptOrigin.y = 0.0;
			hDGN->m_ptOrigin.z = 0.0;					       
			
			hDGN->index_built   = false;
			hDGN->element_count = 0;
			//hDGN->element_index = NULL;
			
			hDGN->got_bounds = false;	

			CDGNLib* pDGNLib=CDGNLib::GetInstance();
			for(int i=0;i < 256;i++)
				pDGNLib->GetColorValue(hDGN->color_table[i][0],hDGN->color_table[i][1],hDGN->color_table[i][2],i);
		}
	}

	return hDGN;
}

//////////////////////////////////////////////////////////////////////////
/**	\brief	The CDGNImporter::CloseFile function

	\param	hDGN	a parameter of type CDGNFile*

	\return	void	
*/
void CDGNImporter::CloseFile(CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is invalid-[DGNClose]");

	if(hDGN)
	{
		fclose(hDGN->fp);
		free(hDGN);
	}
}