#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <Tokenizer.h>
#include <IsLine2d.h>
#include "SmartDrawAnno.h"
#include "AnnoTerritory.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "Drawing.h"

#include <fstream>
using namespace std;

CSmartDrawAnno::CSmartDrawAnno(void)
{
}

CSmartDrawAnno::~CSmartDrawAnno(void)
{
}

/**	
	@brief	annotate
	@author	HumKyung.BAEK
	@date	2010.05.25
	@param	
	@return
*/
int CSmartDrawAnno::Annotate(const TCHAR* pOutputFilePath , const TCHAR* pHsrTextFilePath , const TCHAR* pIniFilePath , CaDraw_DraFile* pDraFile,const double& dDrawingScale)
{
	assert(pDraFile && pIniFilePath && "pDraFile or pIniFilePath is NULL");
	assert(pHsrTextFilePath && "pHiddenTextFilePath is NULL");

	if(pDraFile && pIniFilePath && pHsrTextFilePath)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		env.Parse(STRING_T(pIniFilePath),_T(""));

		auto_ptr<CDrawing> oDrawing(CSmartDrawAnnoModule::CreateDrawingOf(_T("")));
		oDrawing->Annotate(pOutputFilePath , pHsrTextFilePath , pDraFile,dDrawingScale);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}