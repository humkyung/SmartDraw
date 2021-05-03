// SmartHSR.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "Geo.h"
#include "SmartHSR.h"
#include "HSRReader.h"
#include "HSRNewExport.h"

#include <map>
using namespace std;

HINSTANCE _hDllInstance;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hModule    = hModule;
	_hDllInstance = (HINSTANCE)hModule;
	lpReserved = lpReserved;

	switch(ul_reason_for_call){ 
		case DLL_PROCESS_ATTACH:
		/* Indicates that the DLL is being loaded into the 
		virtual address space of the current process as 
		a result of the process starting up or as a 
		result of a call to LoadLibrary.DLLs can use 
		this opportunity to initialize any instance data 
		or to use the TlsAlloc function to allocate a 
			Thread Local Storage (TLS) index.*/
			CHSRApp::LoadPlugIns();
		break;
			
		case DLL_THREAD_ATTACH:
		/* Indicates that the current process is creating a 
		new thread.When this occurs, the system calls the 
		entry-point function of all DLLs currently attached 
		to the process. The call is made in the context of 
		the new thread. DLLs can use this opportunity 
		to initialize a TLS slot for the thread. A thread 
		calling the DLL entry-point function with 
		DLL_PROCESS_ATTACH does not call the DLL entry-point 
		function with DLL_THREAD_ATTACH. Note that a DLL's 
		entry-point function is called with this value only 
		by threads created after the DLL is loaded by the 
		process. When a DLL is loaded using LoadLibrary, 
		existing threads do not call the entry-point 
			function of the newly loaded DLL.*/
			break;
			
		case DLL_THREAD_DETACH:
		/* Indicates that a thread is exiting cleanly. 
		If the DLL has stored a pointer to allocated 
		memory in a TLS slot, it uses this opportunity 
		to free the memory. The system calls the entry-point 
		function of all currently loaded DLLs with this value. 
			The call is made in the context of the exiting thread.*/
			break;
			
		case DLL_PROCESS_DETACH: 
		/* Indicates that the DLL is being unloaded from 
		the virtual address space of the calling process 
		as a result of either a process exit or a call to 
		FreeLibrary. The DLL can use this opportunity to 
		call the TlsFree function to free any TLS indices 
		allocated by using TlsAlloc and to free any thread 
			local data.*/
			CHSRApp::FreePlugIns();
			CHSRApp::DeleteInstance();
			break;
			
		default:
			break;
	}

    return TRUE;
}

//-->
extern "C" __declspec(dllimport) bool Query(int nAddress,int nQueryPair);

#define ASCII 1
/**	
	@brief	The SmartHSR function

	@author	humkyung

	@param	pDraFilePath	output file path
	@param	pIniFilePath	ini file path
	@param	pPrjFilePath	prject setting file path which have directory setting for model files

	@return	void	
*/
extern "C" __declspec(dllexport)
void __stdcall SmartHSR(LPCTSTR pHsrFilePath , LPCTSTR pDraFilePath , LPCTSTR pIniFilePath , LPCTSTR pSxfFolderPath)
{
	assert(pDraFilePath && "pDraFilePath is NULL");
	assert(pIniFilePath && "pIniFilePath is NULL");
	assert(pSxfFolderPath && "pSxfFolderPath is NULL");

	if(pDraFilePath && pIniFilePath && pSxfFolderPath)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		if(pHSRApp)
		{
			pHSRApp->InitHSRApp();
			
			STRCPY_T(pHSRApp->GetEnv()->m_szOutHideFileName , pHsrFilePath);	
			if(!pHSRApp->GetEnv()->Read(pIniFilePath))
			{
				OutputDebugString( _T("Error occurs in reading ini file!!!") );
				return;
			}
			
			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			if(!GetPrivateProfileString( _T("PROJECT") , _T("OutputStyle") , NULL , szBuf , MAX_PATH , pIniFilePath))
			{
				STRCPY_T(szBuf , _T("V1"));	//! version 1 format is default
			}
			pHSRApp->CreateExporter(szBuf);

			if(!pHSRApp->Hide(pDraFilePath , pSxfFolderPath))
			///if(!pHSRApp->Hide("E:\\DATA\\AutoPLANT\\Tracing_Test\\Working\\DET-CL1-D-0231-2-06-03A.crd"))
			{
				OutputDebugString( _T("Error occurs in HSR processing!!!") );
			}
			
			pHSRApp->m_pViewBoundary->ComputateViewBoundary();
			if(pHSRApp->m_pExporter)
			{
				pHSRApp->m_pExporter->WriteViewBoundary(pHSRApp->m_pViewBoundary);
				pHSRApp->m_pExporter->WritePrimitiveAsAsciiFile(pHSRApp->GetEnv()->m_szOutHideFileName);
			}
			
			CHSRApp::DeleteInstance();
		}
		else
		{
			OutputDebugString(_T("Can't Create a Hidden Application!!!"));
		}
	}
}

STRING_T CHSRApp::m_strCurLayer = _T("");

double CHSRApp::TOLERANCE = 1.0;	/// default value
CHSRApp* CHSRApp::m_pInstance=NULL;

/**	
	@brief
*/
CHSRApp::CHSRApp() : m_pExporter(NULL)
{
	m_pHSRReader = NULL;
	m_pViewBoundary = NULL;

	m_volModel.x[LOW] = m_volModel.x[HIGH] = 0.;
	m_volModel.y[LOW] = m_volModel.y[HIGH] = 0.;
	m_volModel.z[LOW] = m_volModel.z[HIGH] = 0.;

	m_nID = 0;

#ifdef _DEBUG
	OutputDebugString("CHSRApp");
#endif
}

/**	
	@brief
*/
CHSRApp::~CHSRApp()
{
	try
	{
		Destroy();
		CHSRApp::m_pInstance = NULL;
	}catch(...){}

#ifdef _DEBUG	
	OutputDebugString("Destructure of CHSRApp");
#endif
}

/**	\brief	The CHSRApp::InitHSRApp function


	\return	bool	
*/
bool CHSRApp::InitHSRApp(){
	m_volModel.x[LOW] = m_volModel.x[HIGH] = 0.;
	m_volModel.y[LOW] = m_volModel.y[HIGH] = 0.;
	m_volModel.z[LOW] = m_volModel.z[HIGH] = 0.;
	m_nID = 0;

	CHSRApp::InitFactories();
	
	return true;
}

/**	\brief	The CHSRApp::GetInstance function


	\return	CHSRApp*	
*/
CHSRApp* CHSRApp::GetInstance(){
	if(NULL == CHSRApp::m_pInstance) CHSRApp::m_pInstance = new CHSRApp;
	if(NULL != CHSRApp::m_pInstance) return CHSRApp::m_pInstance;
	return NULL;
}

/**	
	@brief	The CHSRApp::DeleteInstance function


	@return	void	
*/
void CHSRApp::DeleteInstance()
{
	if(NULL != CHSRApp::m_pInstance)
	{
		delete CHSRApp::m_pInstance;
		CHSRApp::m_pInstance = NULL;
	}
}

/**	
	@brief	The CHSRApp::GetOption function


	@return	CHSROption*	
*/
CHSROption* CHSRApp::GetOption()
{
	return &m_clsOption;
}

/**	
	@brief	clear used memory.

	@return	void	
*/
void CHSRApp::Destroy()
{
	if(NULL != m_pHSRReader)
	{
		delete m_pHSRReader;
		m_pHSRReader = NULL;
	}
	
	if(NULL != m_pViewBoundary)
	{
		delete m_pViewBoundary;
		m_pViewBoundary = NULL;
	}
	m_clsHSRCore.Clear();
	if(m_pExporter)
	{
		m_pExporter->Clear();
		delete m_pExporter;
		m_pExporter = NULL;
	}

	CHSRApp::ClearFactories();
}

/**	
	@brief	allocate new point and vertex


	@return	PVERTEX	
*/
PHSR_VERTEX CHSRApp::NewVertex()
{
	PHSR_VERTEX pRet = NULL;
	
	if(NULL != (pRet = alloc_vertex()))
	{
		if(NULL != (pRet->pt = alloc_point()))
		{
		}else	pRet = NULL;
	}
	
	return pRet;
}

/**	
	@brief	The CHSRApp::IsOuterVolume function

	@param	pDest	a parameter of type PSQVOLUME
	@param	pSrc	a parameter of type PSQVOLUME

	@return	bool	
*/
bool CHSRApp::IsOuterVolume(PSQVOLUME pDest, PSQVOLUME pSrc)
{
	assert(pDest && "pDest is NULL");
	assert(pSrc  && "pSrc  is NULL");
	bool ret = false;

	if(pDest && pSrc)
	{
		if(TOP == m_clsView.GetViewPoint())
		{
			if((pSrc->x[HIGH] < pDest->x[LOW])     || 
				(pSrc->x[LOW] > pDest->x[HIGH])||
				(pSrc->y[HIGH]< pDest->y[LOW]) ||
				(pSrc->y[LOW] > pDest->y[HIGH]))
				
				ret = true;
		}
		else if(FRONT == m_clsView.GetViewPoint())
		{
			if((pSrc->y[HIGH] < pDest->y[LOW])     || 
				(pSrc->y[LOW] > pDest->y[HIGH])||
				(pSrc->z[HIGH]< pDest->z[LOW]) ||
				(pSrc->z[LOW] > pDest->z[HIGH]))
				
				ret = true;
		}
	}
		
	return ret;
}

/**	\brief	The CHSRApp::IsParallelPlane function

	\param	pFace	a parameter of type PFACE

	\return	bool	
*/
bool CHSRApp::IsParallelPlane(CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	bool ret = false;

	if(pFace){
		double vtx=0.,vty=0.,vtz=0.;

		if(TOP == m_clsView.GetViewPoint()){
			vtx = 0.;
			vty = 0.;
			vtz = 1.;
		}else if(FRONT == m_clsView.GetViewPoint()){
			vtx = 1.;
			vty = 0.;
			vtz = 0.;
		}
		
		CIsVect3d norm(pFace->Plane.a , pFace->Plane.b , pFace->Plane.c);
		const double dot = norm.DotProduct(CIsVect3d(vtx,vty,vtz));
		///double dot = theGeo.DotProduct(pFace->Plane.a,pFace->Plane.b,pFace->Plane.c,vtx,vty,vtz);
		
		if((dot >= -0.01) && (dot <= 0.01)) ret = true;
	}
	
	return ret;
}

/**	
	@brief	append pFace to pFaceList's tail or head

	@author	BHK

	@date	?

	@param	pFaceList	a parameter of type PFACE
	@param	pFace	a parameter of type PFACE

	@remarks
	;set face to HIDDEN or INVALID if from view point exits other faces with co-axis.(face is CIRCLE)\n
	;2002.05.29 - when append fact to face list, occur self reference.\n
	;because of this occurrence do infinite loop.

	@return	PFACE	
*/
CHSRFace* CHSRApp::AppendFace(CHSRFace* pFaceList, CHSRFace* pFace)
{
	assert(pFace        && "face is NULL");
	assert(pFace->pHead && "face->pHead is NULL");

	if(pFace && pFace->pHead)
	{
		if((SYMBOL == pFace->ele) || (SECTION == pFace->ele))
		{
			bool hiddenface     = false;
			CHSRFace* pFaceNodeNext = NULL;
			for(CHSRFace* pFaceNode = m_pFaceList[SECTION_FACE];pFaceNode;pFaceNode = pFaceNodeNext){
				pFaceNodeNext = pFaceNode->next;
				if(IsEqualPointFromViewPoint(pFaceNode->ptCenter,pFace->ptCenter)){
					if(IsGreatNumber(pFaceNode->ptCenter.z,pFace->ptCenter.z)){
						if(IsGreatNumber(pFaceNode->radius,pFace->radius)){
							for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							hiddenface = true;
						}else if(IsEqualNumber(pFace->radius,pFaceNode->radius)){
							for(PHSR_VERTEX ptr = pFace->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							hiddenface = true;
						}
					}else if(IsGreatNumber(pFace->ptCenter.z,pFaceNode->ptCenter.z)){
						if(IsGreatNumber(pFace->radius,pFaceNode->radius)){
							for(PHSR_VERTEX ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							pFaceNode->hidden = true;

							/*
							if(pFaceNode->prev){
								pFaceNode->prev->next = pFaceNode->next;
								if(pFaceNode->next) pFaceNode->next->prev = pFaceNode->prev;
							}else{
								m_pFaceList[SECTION_FACE] = pFaceNode->next;
								if(m_pFaceList[SECTION_FACE]) m_pFaceList[SECTION_FACE]->prev = NULL;
							}
							pFace->next = m_pFaceList[HIDDEN_FACE];
							pFace->prev = NULL;
							if(m_pFaceList[HIDDEN_FACE]) m_pFaceList[HIDDEN_FACE]->prev = pFace;
							m_pFaceList[HIDDEN_FACE] = pFace;
							*/
						}else if(IsEqualNumber(pFace->radius,pFaceNode->radius)){
							for(PHSR_VERTEX ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							pFaceNode->hidden = true;
							
							/*
							if(pFaceNode->prev){
								pFaceNode->prev->next = pFaceNode->next;
								if(pFaceNode->next) pFaceNode->next->prev = pFaceNode->prev;
							}else{
								m_pFaceList[SECTION_FACE] = pFaceNode->next;
								if(m_pFaceList[SECTION_FACE]) m_pFaceList[SECTION_FACE]->prev = NULL;
							}
							pFaceNode->next = m_pFaceList[HIDDEN_FACE];
							pFaceNode->prev = NULL;
							if(m_pFaceList[HIDDEN_FACE]) m_pFaceList[HIDDEN_FACE]->prev = pFaceNode;
							m_pFaceList[HIDDEN_FACE] = pFaceNode;
							*/
						}
					}
				}
			}
			if(hiddenface){
				pFace->next = m_pFaceList[HIDDEN_FACE];
				pFace->prev = NULL;
				if(m_pFaceList[HIDDEN_FACE]) m_pFaceList[HIDDEN_FACE]->prev = pFace;
				m_pFaceList[HIDDEN_FACE] = pFace;
			}else{
				pFace->next = m_pFaceList[SECTION_FACE];
				pFace->prev = NULL;
				if(m_pFaceList[SECTION_FACE]) m_pFaceList[SECTION_FACE]->prev = pFace;
				m_pFaceList[SECTION_FACE] = pFace;
			}
		}
		else
		{
			//! add face to facelist's header
			pFace->next = m_pFaceList[NORMAL_FACE];
			pFace->prev = NULL;
			if(m_pFaceList[NORMAL_FACE]) m_pFaceList[NORMAL_FACE]->prev = pFace;
			m_pFaceList[NORMAL_FACE] = pFace;
		}
	}

	return pFaceList;
}

/**	
	@brief	if num1 and num2 is closer, assume two variable is equal.

	@author	BHK

	@date	?

	@param	num1	a parameter of type double
	@param	num2	a parameter of type double

	@return	bool	
*/
bool CHSRApp::IsEqualNumber(const double& num1, const double& num2)
{
	bool   ret = false;
	double dnum= num2 - num1; 

	if(fabs(dnum) <= 0.001) ret = true;

	return ret;
}

/**	\brief	The CHSRApp::IsEqualPoint function\n
	check pt1 and pt2 is equal from view point.

	\param	pt1	a parameter of type POINT_T
	\param	pt2	a parameter of type POINT_T

	\return	bool	
*/
bool CHSRApp::IsEqualPoint(POINT_T pt1, POINT_T pt2)
{
	bool   ret = false;
	double dx,dy,dz;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if((fabs(dx) <= 0.001) && (fabs(dy) <= 0.001) && (fabs(dz) <= 0.001)) ret = true;

	return ret;
}

/**	\brief	The CHSRApp::IsEqualPointFromViewPoint function\n
	check pt1 and pt2 is equal from view point.

	\param	pt1	a parameter of type POINT_T
	\param	pt2	a parameter of type POINT_T

	\return	bool	
*/
bool CHSRApp::IsEqualPointFromViewPoint(POINT_T pt1, POINT_T pt2)
{
	bool   ret = false;
	double dx,dy,dz;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if(TOP == m_clsView.GetViewPoint()){
		if((fabs(dx) <= 0.001) && (fabs(dy) <= 0.001)) ret = true;
	}else if(FRONT == m_clsView.GetViewPoint()){
		if((fabs(dy) <= 0.001) && (fabs(dz) <= 0.001)) ret = true;
	}

	return ret;
}

/**	\brief	The CHSRApp::IsGreatNumber function\n
	if num1 is greater than num2 return ture.

	\param	num1	a parameter of type double
	\param	num2	a parameter of type double

	\return	bool	
*/
bool CHSRApp::IsGreatNumber(double num1, double num2)
{
	bool   ret = false;
	double dnum= num2 - num1; 

	if((num1 - num2 - 0.001) >= 0.) ret = true;

	return ret;
}

/**	\brief	The CHSRApp::FreeFace function\n
	throw pFace to momory pool

	\param	pFace	a parameter of type PFACE

	\return	void	
*/
void CHSRApp::FreeFace(CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	
	if(pFace){
		PHSR_VERTEX pTmp=NULL;
		for(PHSR_VERTEX pVt = pFace->pHead;pVt;)
		{
			pTmp = pVt;
			pVt  = pVt->next;

			m_clsMemManager.m_pVertexPool->free(pTmp);
		}

		for(PHSR_VERTEX pVt = pFace->pCenterline;pVt;)
		{
			pTmp = pVt;
			pVt  = pVt->next;

			m_clsMemManager.m_pVertexPool->free(pTmp);
		}

		m_clsMemManager.m_pFacePool->destroy(pFace);
	}
}


/**	\brief	The CHSRApp::IsAdjacentView function

	\param	pView1	a parameter of type CPreViewFormat*
	\param	pView2	a parameter of type CPreViewFormat*
	\param	nTol	a parameter of type const double

	\return	bool	
*/
bool CHSRApp::IsAdjacentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2,const double nTol){
	assert(pView1 && "pView1 is NULL");
	assert(pView2 && "pView2 is NULL");
	bool bRet=false;

	if(pView1 && pView2){
		if(!pView1->IsIn(pView2) && !pView2->IsIn(pView1)){
			POINT_T ptOrigin[2]={0};
			ptOrigin[0] = pView1->GetViewOrigin();
			ptOrigin[1] = pView2->GetViewOrigin();
			
			double dx=fabs(ptOrigin[0].x - ptOrigin[1].x);
			double dy=fabs(ptOrigin[0].y - ptOrigin[1].y);
			double nWidth =(pView1->GetViewWidth()*0.5)  + (pView2->GetViewWidth()*0.5)  + nTol;
			double nHeight=(pView1->GetViewHeight()*0.5) + (pView2->GetViewHeight()*0.5) + nTol;
			if((dx < nWidth) && (dy < nHeight)){
				bRet = true;
			}
		}
	}

	return bRet;
}

/**	\brief	The CHSRApp::SetCoIncidentView function

	\param	pView1	a parameter of type CPreViewFormat*
	\param	pView2	a parameter of type CPreViewFormat*

	\return	void	
*/
void CHSRApp::SetCoIncidentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2){
	assert(pView1 && "pView1 is NULL");
	assert(pView2 && "pView2 is NULL");
	const static double nTol=0.00001;

	if(pView1 && pView2){
		POINT_T ptOrigin[2]={0,};
		ptOrigin[0] = pView1->GetViewOrigin();
		ptOrigin[1] = pView2->GetViewOrigin();
		
		double dx=fabs(ptOrigin[0].x - ptOrigin[1].x);
		double dy=fabs(ptOrigin[0].y - ptOrigin[1].y);
		double nWidth =(pView1->GetViewWidth())*0.5 + (pView2->GetViewWidth())*0.5;
		double nHeight=(pView1->GetViewHeight())*0.5+ (pView2->GetViewHeight())*0.5;
		if(dx >= nWidth){
			double nGap=dx - nWidth;
			
			double nDir=(ptOrigin[1].x - ptOrigin[0].x)/(fabs(ptOrigin[1].x - ptOrigin[0].x));
			ptOrigin[0].x += nDir*(nGap*0.5 + nTol);
			pView1->SetViewOrigin(ptOrigin[0]);
			
			nDir=(ptOrigin[0].x - ptOrigin[1].x)/(fabs(ptOrigin[0].x - ptOrigin[1].x));
			ptOrigin[1].x += nDir*(nGap*0.5 + nTol);
			pView2->SetViewOrigin(ptOrigin[1]);
		}

		if(dy >= nHeight){
			double nGap=dy - nHeight;
			
			double nDir=(ptOrigin[1].y - ptOrigin[0].y)/(fabs(ptOrigin[1].y - ptOrigin[0].y));
			ptOrigin[0].y += nDir*(nGap*0.5 + nTol);
			pView1->SetViewOrigin(ptOrigin[0]);
			
			nDir=(ptOrigin[0].y - ptOrigin[1].y)/(fabs(ptOrigin[0].y - ptOrigin[1].y));
			ptOrigin[1].y += nDir*(nGap*0.5 + nTol);
			pView2->SetViewOrigin(ptOrigin[1]);
		}
	}
}

/**	
	@brief	
	1. Extract faces\n
	2. Construct BSP Tree\n
	3. CreateRenderList\n
	4. ExtractHiddenLines\n
	5. MergeFace\n
	6. View Clipping\n
	7. PostProcessor Running\n
	8. Write FaceList to Primitive

	@author	humkyung

	@date	????.??.??

	@param	pDraFilePath	a parameter of type const char*
	@param	pPrjFilePath	project setting file

	@remarks\n
	;2004.03.29 - pure current layer name

	@return	bool	
*/
bool CHSRApp::Hide(LPCTSTR pDraFilePath , LPCTSTR pSxfFolderPath)
{
	assert(pDraFilePath && "pDraFilePath is NULL");
	assert(pSxfFolderPath && "pSxfFolderPath is NULL");
	bool bRet = false;

	if(pDraFilePath && pSxfFolderPath)
	{
		CHSRApp::m_strCurLayer= _T(""); /// pure current layer name

		if(NULL == m_pHSRReader)
		{
			m_pHSRReader = new CHSRReader;
		}
		m_pHSRReader->ParseDraFile(pDraFilePath);	/// 2012.03.05 added by humkyung

		if(NULL == m_pViewBoundary) m_pViewBoundary = new CViewBoundary;
		if(m_pHSRReader && m_pViewBoundary)
		{
			m_pHSRReader->Read(pSxfFolderPath);	/// read model data
			
			list<CHSRViewFormat*>* plstViewFormat=m_pHSRReader->GetViewFormatList();
			
			/// check views are adjacent. if so, attach views.
			for(list<CHSRViewFormat*>::iterator itr=plstViewFormat->begin();itr != plstViewFormat->end();++itr)
			{
				list<CHSRViewFormat*>::iterator itrNext=itr;
				for(itrNext++;itrNext != plstViewFormat->end();++itrNext)
				{
					if(IsAdjacentView(*itr,*itrNext,CHSRApp::TOLERANCE))
					{
						SetCoIncidentView(*itr,*itrNext);
					}
				}
			}

			for(list<CHSRViewFormat*>::iterator itr = plstViewFormat->begin();itr != plstViewFormat->end();++itr)
			{
				// clear all face lists.
				m_volModel.x[LOW] = m_volModel.x[HIGH] = 0.f;
				m_volModel.y[LOW] = m_volModel.y[HIGH] = 0.f;
				m_volModel.z[LOW] = m_volModel.z[HIGH] = 0.f;
				
				m_pFaceList[SECTION_FACE] = NULL;
				m_pFaceList[NORMAL_FACE]  = NULL;
				m_pFaceList[HIDDEN_FACE]  = NULL;
								
				m_nID = 0;
				CHSRFace* pFaceList = (*itr)->CreateFace(m_nID);
				
				(*m_pViewBoundary) += (*itr);
				// union separated faces.
				pFaceList = UnifyFaceList();
				////
				
				//checking for input data.
				//m_Export.WriteFaceListToPrimitiveList(pFaceList);
				assert(pFaceList && "pFaceList is NULL");
				if(pFaceList)
				{
					CBSPNode* pBSPNode=NULL;
					try
					{
						pBSPNode  = m_clsHSRCore.m_BSPTree.Construct(&(pFaceList));
					}
					catch(...)
					{
						return false;
					}
					
					m_clsHSRCore.CreateRenderList(pBSPNode);
					m_clsHSRCore.ExtractHiddenLines();
					m_clsHSRCore.RenderList2FaceList();
					
					if(m_clsHSRCore.m_pMergeFace)
					{
						m_clsHSRCore.UnionFaces();
						m_clsPostProcessor.Run(m_clsHSRCore.m_pMergeFace);
					
						m_clsView.Clipping(m_clsHSRCore.m_pMergeFace);
						if(m_pExporter) m_pExporter->WriteFaceListToPrimitiveList(m_clsHSRCore.m_pMergeFace);
						
						m_clsHSRCore.m_pRenderList = NULL;
						m_clsHSRCore.m_pMergeFace  = NULL;
						
						bRet = true;
					}
				}
			}
		}
	}

	return bRet;
}

/**	
	@brief	unify a face list with m_pFaceList[SECTION_FACE],m_pFaceList[NORMAL_FACE]
	
	@author	BHK

	@date	?

	@param

	@return	PFACE	
*/
CHSRFace* CHSRApp::UnifyFaceList()
{
	CHSRFace* ptr = NULL;
	for(ptr = m_pFaceList[SECTION_FACE];ptr && ptr->next;ptr = ptr->next);
	if(ptr){
		ptr->next = m_pFaceList[NORMAL_FACE];
		if(m_pFaceList[NORMAL_FACE]) m_pFaceList[NORMAL_FACE]->prev = ptr;

		return m_pFaceList[SECTION_FACE];
	}else{
		return m_pFaceList[NORMAL_FACE];
	}

	return NULL;
}

/**	
	@brief	if num1 is less than num2 return ture.

	@author	BHK

	@date	?

	@param	num1	a parameter of type double
	@param	num2	a parameter of type double

	@return	bool	
*/
bool CHSRApp::IsLessNumber(double num1, double num2)
{
	bool ret = false;

	if((num1 - num2) < -0.001) ret = true;

	return ret;
}

CHSROption* CHSRApp::GetEnv()
{
	return &m_clsOption;
}

#define MEM_TEST
PPOINT_T CHSRApp::alloc_point()
{
	POINT_T* ppt=(PPOINT_T)m_clsMemManager.m_pPointPool->malloc();
	memset(ppt,'\0',sizeof(POINT_T));
	return ppt;
}

PHSR_VERTEX CHSRApp::alloc_vertex()
{
	PHSR_VERTEX pRet=(PHSR_VERTEX)(m_clsMemManager.m_pVertexPool->malloc());
	memset(pRet,'\0',sizeof(HSR_VERTEX));
	pRet->hider = NULL;	/// 2012.01.12 added by humkyung

	return pRet;
}

CHSRFace* CHSRApp::new_hsr_face()
{
	CHSRFace* pRet=m_clsMemManager.m_pFacePool->construct();
	return pRet;
}

/**	
	@brief	The CHSRApp::alloc_primitive function


	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRApp::alloc_primitive()
{
	PSQPRIMITIVE pRet=(PSQPRIMITIVE)(m_clsMemManager.m_pPrimitivePool->malloc());
	memset(pRet,'\0',sizeof(SQPRIMITIVE));

	return pRet;
}

/**	
	@brief	The CHSRApp::Clear function

	@return	void	
*/
void CHSRApp::Clear()
{	
	if(NULL != m_pViewBoundary)
	{
		delete m_pViewBoundary;
		m_pViewBoundary = NULL;
	}
	m_clsHSRCore.Clear();
	if(m_pExporter) m_pExporter->Clear();
}

//////////////////////////////////////////////////////////////////////////
typedef struct{
	HINSTANCE _hInst;
	void *(*DllInitFactory)();
	void *(*DllClearFactory)();
	CHSRElementFactory* pHSRElementFactory;
}HSR_PLUGINS;

map<STRING_T,HSR_PLUGINS> _theFactoryMap;
CHSRElementFactory* CHSRApp::GetFactory(LPCTSTR pKeyword)
{
	map<STRING_T,HSR_PLUGINS>::iterator where=_theFactoryMap.find(pKeyword);
	if(_theFactoryMap.end() != where) return _theFactoryMap[pKeyword].pHSRElementFactory;

	return NULL;
}

void CHSRApp::RegisterFactory(HINSTANCE hInst,CHSRElementFactory *pFactory)
{
	assert(pFactory && "pFactory is NULL");

	if(NULL != pFactory)
	{
		HSR_PLUGINS plugins;
		plugins._hInst = NULL;
		plugins.pHSRElementFactory = pFactory;

		void *(*pDllInitFactory)()=NULL;
		pDllInitFactory = (void *(*)())GetProcAddress(hInst,"DllInitFactory");
		void *(*pDllClearFactory)()=NULL;
		pDllClearFactory = (void *(*)())GetProcAddress(hInst,"DllClearFactory");
		plugins.DllInitFactory = pDllInitFactory;
		plugins.DllClearFactory= pDllClearFactory;
		
		_theFactoryMap[pFactory->GetKeyword()] = plugins;
	}
}

void CHSRApp::InitFactories()
{
	for(map<STRING_T,HSR_PLUGINS>::iterator itr=_theFactoryMap.begin();itr != _theFactoryMap.end();++itr)
	{
		(*itr).second.DllInitFactory();
	}
}

void CHSRApp::ClearFactories()
{
	for(map<STRING_T,HSR_PLUGINS>::iterator itr=_theFactoryMap.begin();itr != _theFactoryMap.end();++itr)
	{
		(*itr).second.DllClearFactory();
	}
}

extern void hsr_add_raw_line(HSR_LAYER_E layer,PSQPRIMITIVE ptr,CHSRViewFormat* pHSRViewFormat);
void CHSRApp::LoadPlugIns()
{
	const static TCHAR _filter[]={ _T("*.hsr") };
	static TCHAR _szPlugInsFilter[MAX_PATH]={0,};
	static TCHAR _szPlugInsPath[MAX_PATH]={0,};
	static TCHAR _szPath[MAX_PATH]={0};
	
	::GetModuleFileName(_hDllInstance,_szPath,MAX_PATH);
	const int len = STRLEN_T(_szPath);
	int i = 0;
	for(i=len - 1;(i>=0) && ('\\' != _szPath[i]);i--);
	if('\\' == _szPath[i]) _szPath[i+ 1] = '\0';
	if(_szPath[len - 1] != '\\') STRCAT_T(_szPath, _T("\\"));
	STRCPY_T(_szPlugInsFilter,_szPath);
	STRCAT_T(_szPlugInsFilter,_filter);
	
	/*	
	typedef struct _WIN32_FIND_DATA { 
		DWORD dwFileAttributes; 
		FILETIME ftCreationTime; 
		FILETIME ftLastAccessTime; 
		FILETIME ftLastWriteTime; 
		DWORD nFileSizeHigh; 
		DWORD nFileSizeLow; 
		DWORD dwOID; 
		TCHAR cFileName[MAX_PATH]; 
	} WIN32_FIND_DATA; 
	*/
	WIN32_FIND_DATA FindFileData={0};
	
	HANDLE hFindFile = FindFirstFile(_szPlugInsFilter,&FindFileData);
	if(hFindFile != INVALID_HANDLE_VALUE){
		BOOL bTrue = TRUE;
		do{
			STRCPY_T(_szPlugInsPath,_szPath);
			const int len = STRLEN_T(_szPlugInsPath);
			if(_szPlugInsPath[len - 1] != '\\') STRCAT_T(_szPlugInsPath, _T("\\"));
			STRCAT_T(_szPlugInsPath,FindFileData.cFileName);
			HINSTANCE hInstDll = LoadLibrary(_szPlugInsPath);
			if(hInstDll)
			{
				CHSRElementFactory *(*pDllGetFunc)()=NULL;
				pDllGetFunc = (CHSRElementFactory *(*)())GetProcAddress(hInstDll,"DllGetHSRElementFactory");
				CHSRElementFactory* pHSRElementFactory=pDllGetFunc();
				if(NULL != pHSRElementFactory)
				{
					void (*pSetFuncPtr)(void (*)(HSR_LAYER_E,PSQPRIMITIVE,CHSRViewFormat*))=NULL;
					pSetFuncPtr = (void (*)(void (*)(HSR_LAYER_E,PSQPRIMITIVE,CHSRViewFormat*)))GetProcAddress(hInstDll,"DllSetAddRawLineFuncPtr");
					pSetFuncPtr(hsr_add_raw_line);
					
					RegisterFactory(hInstDll,pHSRElementFactory);
				}
			}
			bTrue = FindNextFile(hFindFile,&FindFileData);
		}while(bTrue == TRUE);
		FindClose(hFindFile);
	}
	
}


void CHSRApp::FreePlugIns()
{
	for(map<STRING_T,HSR_PLUGINS>::iterator itr=_theFactoryMap.begin();itr != _theFactoryMap.end();itr++)
	{
		::FreeLibrary((*itr).second._hInst);
	}
}

void CHSRApp::free_vertex(HSR_VERTEX *pVertex)
{
	m_clsMemManager.m_pVertexPool->free(pVertex);
}

void CHSRApp::free_point(PPOINT_T ppt)
{
	m_clsMemManager.m_pPointPool->free(ppt);
}

void CHSRApp::delete_hsr_face(CHSRFace* pFace)
{
	m_clsMemManager.m_pFacePool->destroy(pFace);
}

/**
	@brief	create export class according to version
	@author	HumKyung.Baek
	@date	2010.11.12
	@return	int
*/
int CHSRApp::CreateExporter(const TCHAR* version)
{
	assert(version && "version is NULL");

	if(version)
	{
		if((NULL != m_pExporter) && (STRING_T(version) != m_pExporter->GetVersion()))
		{
			delete m_pExporter;
			m_pExporter = NULL;
		}

		if(NULL == m_pExporter)
		{
			if(0 == STRICMP_T(version , _T("V1")))
			{
				m_pExporter = new CHSRExport;
			}
			else if(0 == STRICMP_T(version , _T("V2")))
			{
				m_pExporter = new CHSRNewExport;
			}
		}
	}

	return ERROR_SUCCESS;
}