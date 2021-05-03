#include <DGNLib.h>

#include <DGNElement.h>
#include <DGNLine.h>
#include <DGNPointString.h>
#include <DGNCircle.h>
#include <DGNArc.h>
#include <DGNEllipse.h>
#include <DGNCone.h>
#include <DGNText.h>

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}*/


const static struct tagDGNCOLORS{
	int   nDGNColor;
	unsigned char r,g,b;
}DGNCOLORS[]={
	{0,255,255,255},
	{1,0,0,255},
	{2,0,255,0},
	{3,255,0,0},
	{4,255,255,0},
	{5,255,0,255},
	{6,255,255,255},
	{7,0,255,255},
	{8,255,255,255},
	{9,211,211,211},
	{10,0,200,0},
	{11,255,0,0},
	{12,255,255,0},
	{13,255,0,255},
	{14,255,150,0},
	{15,132,117,115},
	{96,211,211,211}
};

const static struct tagDGNSURFACES{
	int   nDGNSurface,nSurface;
}DGNSURFACES[]={
	{0,SWEEP_SURF},
	{1,SWEEP_SURF},
	{2,SWEEP_SURF},
	{3,SWEEP_SURF},
	{4,SWEEP_SURF},
	{5,SWEEP_SURF},
	{6,SWEEP_SURF},
	{7,SWEEP_SURF},
	{8,SURF_OF_REVOLUTION},
	{9,SWEEP_SURF}
};

/**	\brief	The CDGNLib::memory function

	\param	nSize	a parameter of type size_t

	\return	void*	
*/
void* CDGNLib::memory(size_t nSize)
{
	return calloc(1,nSize);
}

void* (*CDGNLib::AllocMemory)(size_t nSize)=CDGNLib::memory;
void (*CDGNLib::FreeMemory)(void* memblock)=free;

/**	\brief
*/
CDGNLib::CDGNLib()
{
	const static char* szSeed="init.dgn";

	_instance = NULL;
	strcpy(m_DGNExport.m_szSeedFilePath,szSeed);
	m_pDoc = NULL;

	m_plstPrimitive = new list<PSQPRIMITIVE>;
	m_plstElement = new list<CDGNElement*>;
}

/**	\brief
*/
CDGNLib::~CDGNLib()
{
	try
	{
		OnFreePrimitive();

		for(list<CDGNElement*>::iterator itr=m_plstElement->begin();itr != m_plstElement->end();)
		{
			delete (*itr);
			m_plstElement->erase(itr++);
		}

		if(m_plstPrimitive) delete m_plstPrimitive;
		if(m_plstElement) delete m_plstElement;
	}
	catch(...)
	{
	}
}

/**	\brief	The CDGNLib::GetInstance function


	\return	CDGNLib*
*/
CDGNLib* CDGNLib::GetInstance()
{
	static CDGNLib theDGNLib;

	if(NULL == theDGNLib._instance)
	{
		CDGNLine* pLine=new CDGNLine;
		if(pLine) theDGNLib.m_plstElement->push_back(pLine);

		CDGNPointString* pPointString = new CDGNPointString;
		if(pPointString) theDGNLib.m_plstElement->push_back(pPointString);

		CDGNCircle*  pCircle=new CDGNCircle;
		if(pCircle) theDGNLib.m_plstElement->push_back(pCircle);

		CDGNArc*  pArc=new CDGNArc;
		if(pArc) theDGNLib.m_plstElement->push_back(pArc);

		CDGNEllipse*  pEllipse=new CDGNEllipse;
		if(pEllipse) theDGNLib.m_plstElement->push_back(pEllipse);

		CDGNCone*  pCone=new CDGNCone;
		if(pCone) theDGNLib.m_plstElement->push_back(pCone);

		CDGNText* pText=new CDGNText;
		if(pText) theDGNLib.m_plstElement->push_back(pText);

		theDGNLib._instance = &theDGNLib;
	}
	return &theDGNLib;
}

/**	\brief	The CDGNLib::GetDoc function


	\return	CDGNDoc*	
*/
CDGNDoc* CDGNLib::GetDoc(){return m_pDoc;}

/**	
	@brief	The CDGNLib::Load function

	@author	HumKyung

	@date	????.??.??

	@param	pFilePath	a parameter of type const char*

	@return	bool
*/
bool CDGNLib::Load(const char* pFilePath)
{
	assert(pFilePath && "pFilePath is NULL");
	bool bRet=false;

	if(pFilePath)
	{
		if(NULL == m_pDoc) m_pDoc = new CDGNDoc;
		if(m_pDoc)
		{
			PSQPRIMITIVE pRet = m_clsImporter.OnImportFile(pFilePath);
			if(pRet)
			{
				m_pDoc->Set(pRet);
				bRet = true;
			}
		}
	}
	return bRet;
}

/**	\brief	The CDGNLib::GetColorValue function

	\param	r	a parameter of type unsigned char&
	\param	g	a parameter of type unsigned char&
	\param	b	a parameter of type unsigned char&
	\param	nDGNColor	a parameter of type const int

	\return	void
*/
void CDGNLib::GetColorValue(unsigned char& r,unsigned char& g,unsigned char& b,const int nDGNColor){
	int nSize=sizeof(DGNCOLORS)/sizeof(struct tagDGNCOLORS);
	
	r = g = b = 255;
	for(int i=0;i < nSize;i++){
		if(DGNCOLORS[i].nDGNColor == nDGNColor){
			r = DGNCOLORS[i].r;
			g = DGNCOLORS[i].g;
			b = DGNCOLORS[i].b;
			
			break;
		}
	}
}

/**	\brief	The CDGNLib::GetDGNColorNumber function

	\param	hDGN	a parameter of type CDGNFile*
	\param	r	a parameter of type unsigned char
	\param	g	a parameter of type unsigned char
	\param	b	a parameter of type unsigned char

	\return	const int
*/
const int CDGNLib::GetDGNColorNumber(CDGNFile* hDGN,unsigned char r,unsigned char g,unsigned char b){
	assert(hDGN && "hDGN is NULL");
	int nColor=0;

	if(hDGN){
		int dr=hDGN->color_table[0][0] - r;
		int dg=hDGN->color_table[0][1] - g;
		int db=hDGN->color_table[0][2] - b;

		int min=dr + dg + db;
		for(int i=1;i < 256;i++){
			dr = hDGN->color_table[i][0] - r;
			dg = hDGN->color_table[i][1] - g;
			db = hDGN->color_table[i][2] - b;

			if(min > (dr + dg + db)){
				min = dr + dg + db;
				nColor = i;
			}
		}
	}
	
	return nColor;
}

/**	\brief	The CDGNLib::POINT2INT function

	\param	paryTarget	a parameter of type unsigned char *
	\param	ppt	a parameter of type POINT_T *
	\param	hDGN	a parameter of type CDGNFile*

	\return	void
*/
void CDGNLib::POINT2INT(unsigned char *paryTarget,POINT_T *ppt,CDGNFile* hDGN){
	assert(paryTarget && "pabyTarget is NULL");
	assert(hDGN && "hDGN is NULL");
	assert(ppt  && "ppt is NULL");

	if(paryTarget && hDGN && ppt){
		double adfCT[3];
		
		adfCT[0] = (ppt->x + hDGN->m_ptOrigin.x);// / hDGN->scale;
		adfCT[1] = (ppt->y + hDGN->m_ptOrigin.y);// / hDGN->scale;
		adfCT[2] = (ppt->z + hDGN->m_ptOrigin.z);// / hDGN->scale;
		
		for(int i = 0; i < 3; i++ ){
			long nCTI;
			unsigned char *pabyCTI = (unsigned char *) &nCTI;
			
			nCTI = (long) MAX(-2147483647,MIN(2147483647,adfCT[i]));
			
//#ifdef WORDS_BIGENDIAN 
//			paryTarget[i*4+0] = pabyCTI[1];
//			paryTarget[i*4+1] = pabyCTI[0];
//			paryTarget[i*4+2] = pabyCTI[3];
//			paryTarget[i*4+3] = pabyCTI[2];
//#else
			paryTarget[i*4+3] = pabyCTI[1];
			paryTarget[i*4+2] = pabyCTI[0];
			paryTarget[i*4+1] = pabyCTI[3];
			paryTarget[i*4+0] = pabyCTI[2];
//#endif        
		}
	}
}

/**	\brief	The CDGNLib::IsPartOfComplexElement function

	\param	hDGN	a parameter of type CDGNFile*

	\return	bool
*/
bool CDGNLib::IsPartOfComplexElement(CDGNFile* hDGN){
	assert(hDGN && "hDGN is NULL");
	bool bRet=false;

	if(hDGN){
		PDGNHDR pDGNHDR = (PDGNHDR)hDGN->abyElem;
		bRet = (pDGNHDR->ElmHdr.complex) ? true : false;
	}

	return bRet;
}

/**	\brief	The CDGNLib::GetSurfaceType function

	\param	nDGNSurface	a parameter of type const int

	\return	const int
*/
const int CDGNLib::GetSurfaceType(const int nDGNSurface){
	int nSize=sizeof(DGNSURFACES)/sizeof(struct tagDGNSURFACES);
	
	if((nDGNSurface >= 0) && (nDGNSurface < nSize))
		return DGNSURFACES[nDGNSurface].nSurface;
	
	return DGNSURFACES[0].nSurface;
}

/**	\brief	The CDGNLib::GetDGNSurfaceType function

	\param	nSurface	a parameter of type const int

	\return	const int
*/
const int CDGNLib::GetDGNSurfaceType(const int nSurface){
	int nSize=sizeof(DGNSURFACES)/sizeof(struct tagDGNSURFACES);
	
	for(int i=0;i < nSize;i++){
		if(nSurface == DGNSURFACES[i].nSurface) return DGNSURFACES[i].nDGNSurface;
	}
	
	return DGNSURFACES[0].nDGNSurface;
}

/**	\brief	The CDGNLib::GetElementPrototype function

	\param	nType	a parameter of type const int

	\return	CDGNElement*
*/
CDGNElement* CDGNLib::GetElementPrototype(const int nType){
	CDGNElement* pRet=NULL;

	for(list<CDGNElement*>::iterator itr=m_plstElement->begin();itr != m_plstElement->end();++itr){
		if(nType == (*itr)->m_nType) return *itr;
	}

	return pRet;
}

/**	@brief	The CDGNLib::OnFreePrimitive function
	@author	백흠경

	\return	void
*/
void CDGNLib::OnFreePrimitive()
{
	/*
	for(list<PSQPRIMITIVE>::iterator itr=m_plstPrimitive->begin();itr != m_plstPrimitive->end();)
	{
		delete (*itr);

		m_plstPrimitive->erase(itr++);
	}
	*/
}

/**	\brief	The CDGNLib::NewPrimitive function


	\return	PSQPRIMITIVE
*/
PSQPRIMITIVE CDGNLib::NewPrimitive()
{
	PSQPRIMITIVE pRet=NULL;

	/*if(!m_quePrimitivePool.empty()){
		pRet = m_quePrimitivePool.front();
		m_quePrimitivePool.pop();

		m_plstPrimitive->push_back(pRet);
		memset(pRet,0,sizeof(SQPRIMITIVE));
		
		return pRet;
	}else{
	*/
	if(pRet=(PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE)))
	{		
		m_plstPrimitive->push_back(pRet);
	}
	//}

	return pRet;
}

/**	@brief	The CDGNLib::DeleteElement function
	@author	백흠경

	@param	ptrObj	a parameter of type PSQPRIMITIVE

	@return	void
*/
void CDGNLib::DeleteElement(PSQPRIMITIVE ptrObj)
{
	assert(ptrObj && "ptrObj is NULL");

	if(ptrObj && ptrObj->hdr.attr.f)
	{
		if(ptrObj->hdr.attr.u) CDGNLib::DeleteLinkageData(ptrObj);
		if(NULL != ptrObj->hdr.pLayer)
		{
			free((void*)ptrObj->hdr.pLayer);
			ptrObj->hdr.pLayer = NULL;
		}

		switch(ptrObj->hdr.id)
		{
			case SQ_POINTS:
				delete (ptrObj->body.pvtPoints);
				free((void*)ptrObj);
				break;
			case SQ_LOOP:
			{
				PSQPRIMITIVE ptrNext=NULL;
				for(PSQPRIMITIVE ptr=ptrObj->body.pPrimitive;ptr;ptr = ptrNext)
				{
					ptrNext = ptr->next;
					
					if(ptr->hdr.attr.f) CDGNLib::DeleteElement(ptr);
					else free((void*)ptr);
				}
				free((void*)ptrObj);
			}
				break;
			case SQ_SURFACE:
			{
				SURF_T::PSURFELM_T ptrElmNext=NULL;
				for(SURF_T::PSURFELM_T ptrElm=ptrObj->body.pSurface->pElement;ptrElm;ptrElm = ptrElmNext)
				{
					ptrElmNext = ptrElm->next;
					
					PSQPRIMITIVE ptrNext=NULL;
					for(PSQPRIMITIVE ptr=ptrElm->pBoundaryCurve;ptr;ptr = ptrNext)
					{
						ptrNext = ptr->next;
					
						if(ptr->hdr.attr.f) CDGNLib::DeleteElement(ptr);
						else free((void*)ptr);
					}
					for(PSQPRIMITIVE ptr=ptrElm->pRuleCurve;ptr;ptr = ptrNext)
					{
						ptrNext = ptr->next;
						
						if(ptr->hdr.attr.f) CDGNLib::DeleteElement(ptr);
						else free((void*)ptr);
					}
					free((void*)ptrElm);
				}
				free((void*)ptrObj);
			}
				break;
			case SQ_TEXT:
				if(ptrObj->body.text.pFamily) free((void*)ptrObj->body.text.pFamily);
				if(ptrObj->body.text.pText)   free((void*)ptrObj->body.text.pText);
				free((void*)ptrObj);
				break;
			case ENM_GROUP:
			{
				PSQPRIMITIVE ptrNext=NULL;
				for(PSQPRIMITIVE ptr=ptrObj->body.pPrimitive;ptr;ptr = ptrNext)
				{
					ptrNext = ptr->next;
					
					if(ptr->hdr.attr.f) CDGNLib::DeleteElement(ptr);
					else free((void*)ptr);
				}
				free((void*)ptrObj);
			}
				break;
			default:
				free((void*)ptrObj);
				break;
		}
	}else if(ptrObj) free((void*)ptrObj);
}

/**	\brief	The CDGNLib::DeleteLinkageData function

	\param	ptrObj	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNLib::DeleteLinkageData(PSQPRIMITIVE ptrObj)
{
	assert(ptrObj && "ptrObj is NULL");

	if(ptrObj && ptrObj->pLinkageData)
	{
		PDGNATTRDATA ptrNext=NULL;
		for(PDGNATTRDATA ptr=(PDGNATTRDATA)(ptrObj->pLinkageData);ptr;ptr = ptrNext)
		{
			ptrNext = ptr->next;

			if(ptr->hdr.u) free((void*)ptr->data.pData);
			free((void*)ptr);
		}
		ptrObj->pLinkageData = NULL;
	}
}


//----------------------------------------------------------------------------
/**	\brief	The DGNSetSeedFilePath function

	\param	pSeedFilePath	a parameter of type const char *

	\return	bool	
*/
extern "C" __declspec( dllexport ) 
bool DGNSetSeedFilePath(const char * pSeedFilePath){
	assert(pSeedFilePath && "pSeedFilePath is NULL");
	bool bRet = false;

	if(pSeedFilePath){
		CDGNLib* pDGNLib=CDGNLib::GetInstance();

		strcpy(pDGNLib->m_DGNExport.m_szSeedFilePath,pSeedFilePath);

		bRet = true;
	}

	return bRet;
}

// height : width = 1 : xx
double FONTSCALE[4][100]={
	{0},
	{
	0,0,0.6666,0.1339,0.4174,0.5118,0.5669,0.6693,0.6771,0.4094,
	0.2599,0.2599,0.3859,0.4488,0.1339,0.4488,0.1339,0.5591,0.6614,0.2362,
	0.6693,0.6693,0.7165,0.6693,0.622,0.6771,0.6693,0.622,0.1417,0.1339,
	0.5039,0.4488,0.5039,0.5748,0.6639,0.7715,0.6693,0.7165,0.6693,0.622,
	0.622,0.7165,0.6699,0.3307,0.4803,0.6693,0.5748,0.7638,0.6693,0.7638,
	0.6693,0.7638,0.6693,0.6693,0.6693,0.6693,0.7638,0.9527,0.6693,0.7638,
	0.6693,0.3386,0.6771,0.3386,0.3923,0.7638,0.1575,0.5669,0.5669,0.5748,
	0.5669,0.5669,0.3858,0.5669,0.5196,0.0472,0.2834,0.5196,0.3307,1.0132,
	0.5196,0.6142,0.5669,0.5669,0.3779,0.5196,0.3779,0.5196,0.5669,0.746,
	0.5196,0.5669,0.5196,0,0,0,0,0,0,0
	},
	{0},
	{0,0,0.667,0.4178,0.6458,0.8749,0.6331,0.854,0.8149,0.3333,
	0.3333,0.3333,0.9167,0.8333,0.3837,0.7499,0.3306,0.6666,0.6628,0.3333,
	0.5833,0.5833,0.5833,0.5859,0.6302,0.5833,0.5833,0.5833,0.3837,0.3837,
	0.6666,0.8333,0.6666,0.617,0.7449,0.6666,0.6302,0.6302,0.6364,0.6249,
	0.5833,0.6328,0.5833,0.4528,0.4999,0.5833,0.5833,0.7155,0.6302,0.6707,
	0.6303,0.6718,0.6302,0.6285,0.6666,0.6249,0.6666,0.7161,0.6249,0.6458,
	0.6302,0.4998,0.6666,0.3748,0.3592,0.6742,0.8333,0.6771,0.6718,0.6822,
	0.6718,0.6874,0.6666,0.6718,0.6771,0.401,0.401,0.6822,1,0.8281,
	0.6822,0.7659,0.6718,0.6718,0.6666,0.6666,0.6718,0.6771,0.6718,0.8021,
	0.6822,0.6874,0.6874,0.4868,0.6666,0.5207,0,0,0,0
	}

};

//	parameter	:
//	description	: get length of string.
//	remarks		:
//	returns		:
extern "C" __declspec(dllexport)
double GetDGNTextLength(const char* text/*,int nFontID*/,double nHeight){
	assert(text           && "text is NULL");
	assert((nHeight > 0.) && "nHeight is less than 0., or equal ");
	double nLen = 0.;

	if(text && (nHeight > 0.)){
		//switch(nFontID){
		//	case 0:
		//		nLen=strlen(text)*nHeight;
		//		break;
		//	case 1:{
				double nScale=0;
				nLen = strlen(text)*nHeight;
				nLen-= nHeight - nHeight*FONTSCALE[1][text[strlen(text) - 1] - 30];
				//for(int i=0;text[i];i++){
				//	if((text[i] < 30) || (text[i] >= 130))
				//		nScale = 1.;
				//	else	nScale = FONTSCALE[3][text[i] - 30];
				//	nLen += nScale*nHeight;
				//}
		//	       }
		//		break;
		//	default:
		//		nLen=strlen(text)*nHeight;
		//		break;
		//}
	}

	return nLen;
}

/*
double GetDGNTextLength(const char* text,double nHeight){
	assert(text           && "text is NULL");
	assert((nHeight > 0.) && "nHeight is less than 0., or equal ");
	double len = 0.;

	if(text && (nHeight > 0.)){
		double scale = nHeight*10000./4800.;
		for(int i=0;text[i];i++){
			switch (text[i]){ 
				case 'I': len+=(1380.0 * scale); break;
				case 'J': len+=(3000.9 * scale); break;
				case 'M': len+=(3379.8 * scale); break;
				case 'P': len+=(2690.4 * scale); break;
				case 'T': len+=(2897.1 * scale); break;
				case 'V': len+=(2690.4 * scale); break;
				case 'W': len+=(3379.8 * scale); break;
				case '1': len+=(1380.0 * scale); break;
				case '3': len+=(2738.4 * scale); break;
				case '6': len+=(2769.6 * scale); break;
				case '.': len+=(690.6  * scale); break;
				case ',': len+=(690.6  * scale); break;
				case '-': len+=(2138.7 * scale); break;
				case ' ': len+=(2932.2 * scale); break;
				default : len+=(2862.6 * scale); break;
			}
			len+=(1446.9 * scale);
		}
		return (len //+ nHeight//)/10000;
	}

	return len;
}
*/

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
/*
vector<PSQPRIMITIVE>* DGNImportFile(const char* pImportFilePath,PVOLUME_T pVolume){
	assert(pImportFilePath && "pImportFilePath is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	vector<PSQPRIMITIVE>* pRet=NULL;

	if(pImportFilePath && pDGNLib){
		if(pVolume)
			pRet = pDGNLib->m_clsImporter.ImportFile(pImportFilePath,pVolume);
		else	pRet = pDGNLib->m_clsImporter.ImportFile(pImportFilePath);
	}

	return pRet;
}
*/

/**	\brief	The DGNImportFile function

	\param	pImportFilePath	a parameter of type const char*
	\param	pVolume	a parameter of type VOLUME_T*

	\return	PSQPRIMITIVE	
*/
extern "C" __declspec(dllexport)
PSQPRIMITIVE DGNImportFile(const char* pImportFilePath,VOLUME_T* pVolume=NULL)
{
	assert(pImportFilePath && "pImportFilePath is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	PSQPRIMITIVE pRet=NULL;

	if(pImportFilePath && pDGNLib)
	{
		if(pVolume)
			pRet = pDGNLib->m_clsImporter.OnImportFile(pImportFilePath,pVolume);
		else	pRet = pDGNLib->m_clsImporter.OnImportFile(pImportFilePath);
	}

	return pRet;
}

/*
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
extern "C" __declspec(dllexport)
PSQPRIMITIVE DGNImportFile(const char* pImportFilePath,VOLUME_T* pVolume){
	assert(pImportFilePath && "pImportFilePath is NULL");
	assert(pVolume         && "pVolume is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();
	PSQPRIMITIVE pRet=NULL;

	if(pImportFilePath && pVolume && pDGNLib){
		pRet = pDGNLib->m_clsImporter.OnImportFile(pImportFilePath,pVolume);
	}

	return pRet;
}
*/

/**	
	@brief	The DGNExportFile function

	@param	pExportFilePath	a parameter of type const char*
	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	void	
*/
extern "C" __declspec(dllexport)
void DGNExportFile(const char* pExportFilePath,PSQPRIMITIVE pPrimitive)
{
	assert(pExportFilePath && "pExportFilePath is NULL");
	assert(pPrimitive      && "pPrimitive is NULL");
	CDGNLib* pDGNLib=CDGNLib::GetInstance();

	if(pExportFilePath && pPrimitive && pDGNLib)
	{
		pDGNLib->m_DGNExport.OnExportFile(pExportFilePath,pPrimitive);
	}
}

/**	\brief	The DeleteObject function

	\param	ptrObj	a parameter of type PSQPRIMITIVE

	\return	static void	
*/
static void DeleteObject(PSQPRIMITIVE ptrObj){
	assert(ptrObj && "ptrObj is NULL");

	if(ptrObj){
		switch(ptrObj->hdr.id){
			case SQ_POINTS:
				delete (ptrObj->body.pvtPoints);
				free((void*)ptrObj);
				break;
			case SQ_SURFACE:{
				SURF_T::PSURFELM_T ptrElmNext=NULL;
				for(SURF_T::PSURFELM_T ptrElm=ptrObj->body.pSurface->pElement;ptrElm;ptrElm = ptrElmNext){
					ptrElmNext = ptrElm->next;
					free((void*)ptrElm);
				}
				free((void*)ptrObj);
					}
				break;
			case ENM_GROUP:{
				PSQPRIMITIVE ptrNext=NULL;
				for(PSQPRIMITIVE ptr=ptrObj->body.pPrimitive;ptr;ptr = ptrNext){
					ptrNext = ptr->next;
					
					if(ptr->hdr.attr.f) DeleteObject(ptr);
					else free((void*)ptr);
				}
				free((void*)ptrObj);
				      }
				break;
			default:
				free((void*)ptrObj);
				break;
		}
	}
}

/**	\brief	The DGNDeletePrimitive function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void	
*/
extern "C" __declspec(dllexport)
void DGNDeletePrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive){
		PSQPRIMITIVE ptrNext=NULL;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptrNext){
			ptrNext = ptr->next;
			if(ptr->hdr.attr.f) DeleteObject(ptr);
			else	free((void*)ptr);
		}
	}
}

/**
 * tyCopyDgnFile:
 *
 * @param pOutputFile 
 * @param pInputFile 
 * @return void 
 */
__DLL_EXPORT__ void tyCopyDgnFile(const char* pOutputFile,const char* pInputFile)
{
	assert(pOutputFile && "pOutputFile is NULL");
	assert(pInputFile  && "pInputFile is NULL");

	if(pOutputFile && pInputFile){
		CDGNFile inFile,outFile;

		if(false == inFile.Open(pInputFile,CDGNFile::modeRead)) return;
		if(false == outFile.Open(pOutputFile,CDGNFile::modeWrite)) return;
		for(;inFile.ReadElement();){
			PDGNHDR pHdr=(PDGNHDR)(inFile.abyElem);
			if(62 == pHdr->ElmHdr.type){
				memset(inFile.abyElem + 36,'\0',20);
				memcpy(inFile.abyElem + 36,"9SQ(c)",strlen("9SQ(c)"));
			}
			outFile.WriteElement(inFile.abyElem);
		}
		inFile.Close();
		outFile.Close();
	}
}

/**
	@brief	src의 복사본을 생성한다.
	@author	백흠경

	@return	src의 복사본.
*/
PSQPRIMITIVE CDGNLib::CopyPrimitive(PSQPRIMITIVE src)
{
	assert(src  && "src is NULL");
	PSQPRIMITIVE ret = NULL;

	if(src)
	{
		for(PSQPRIMITIVE ptr = src;NULL != ptr;ptr = ptr->next)
		{
			PSQPRIMITIVE p = (PSQPRIMITIVE)CDGNLib::AllocMemory(sizeof(SQPRIMITIVE));
			if(p)
			{
				memcpy(p , ptr , sizeof(SQPRIMITIVE));
				if(p->hdr.pLayer = (char*)calloc(1,2))
				{
					memcpy(p->hdr.pLayer , ptr->hdr.pLayer , sizeof(char) * 2);
				}

				switch(p->hdr.id)
				{
					case SQ_SURFACE: case SQ_PLANESURFACE: case SQ_CONESURFACE:
					case SQ_SPHERESURFACE: case SQ_TORUSSURFACE:
						p->body.pSurface = NULL;
						break;
					case ENM_GROUP:
						p->body.pPrimitive = NULL;
						break;
				}

				ret = ListPush(ret , p);
			}
		}

		return ret;
	}

	return ret;
}
