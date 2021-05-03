#ifndef __HIDDENDLL_H__
#define __HIDDENDLL_H__

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__
#endif

#include "HSRFace.h"	// Added by ClassView
#include <time.h>
#include "SQLib.h"

#include "HSRMemManager.h"
#include "HSRElementFactory.h"
#include "HSRView.h"
#include "HSROption.h"
#include "HSRReader.h"
#include "HSRImport.h"
#include "HSRExport.h"
#include "HSRNewExport.h"	//! added by HumKyung.Baek - 2010.11.12
#include "KMemPool.h"
#include "HSRPostProcessor.h"
#include "HSRPreProcessor.h"

#include "HSRCore.h"
#include "ViewBoundary.h"

#include <string>
#include "HSR.H"	// Added by ClassView
using namespace std;

class CHSRApp
{
	typedef enum
	{
		SECTION_FACE = 0,
		NORMAL_FACE  = 1,
		HIDDEN_FACE  = 2
	}FACETYPE;
protected:
	CHSRApp();
	virtual ~CHSRApp();
public:
	bool InitHSRApp();
	void Destroy();

	PHSR_VERTEX NewVertex();
	bool Hide(LPCTSTR pDraFilePath , LPCTSTR pPrjFilePath);
	CHSRFace* UnifyFaceList();
	void FreeFace(CHSRFace* PFace);
	CHSRFace* AppendFace(CHSRFace* pFaceList,CHSRFace* pFace);
	bool IsParallelPlane(CHSRFace* pFace);
	bool IsOuterVolume(PSQVOLUME pDest,PSQVOLUME pSrc);
	CHSROption* GetOption();
public:
	bool IsGreatNumber(double num1,double num2);
	bool IsEqualPointFromViewPoint(POINT_T pt1,POINT_T pt2);
	bool IsEqualNumber(const double& num1,const double& num2);
	bool IsEqualPoint(POINT_T pt1,POINT_T pt2);
	bool IsLessNumber(double num1,double num2);

	static CHSRApp* GetInstance();
	static void DeleteInstance();
private:
	bool IsAdjacentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2,const double nTol);
	void SetCoIncidentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2);

	CHSROption m_clsOption;
public:
	int CreateExporter(const TCHAR* version);
	void delete_hsr_face(CHSRFace* pFace);
	void free_point(PPOINT_T ppt);
	void free_vertex(HSR_VERTEX* pVertex);
	void ClearFactories();
	static void InitFactories();
	static void FreePlugIns();
	static void LoadPlugIns();
	static void RegisterFactory(HINSTANCE hInst,CHSRElementFactory* pFactory);
	static CHSRElementFactory* GetFactory(LPCTSTR pKeyword);
	void Clear();
	__DLL_EXPORT__ PSQPRIMITIVE alloc_primitive();
	__DLL_EXPORT__ CHSRFace* new_hsr_face();
	__DLL_EXPORT__ PPOINT_T alloc_point();
	__DLL_EXPORT__ PHSR_VERTEX alloc_vertex();
	
	CHSROption* GetEnv();
	CHSRView	m_clsView;
	CHSRReader*	m_pHSRReader;
	CViewBoundary*  m_pViewBoundary;

	CHSRMemManager m_clsMemManager;
	CHSRCore m_clsHSRCore;
	
	///CHSRImport m_Import;
	CHSRExport *m_pExporter;				/// export class
	CHSRPostProcessor m_clsPostProcessor;
	CHSRPreProcessor m_clsPreProcessor;
	
	VOLUME_T m_volModel;

	static STRING_T m_strCurLayer;			/// current layer
	static double TOLERANCE;				/// hidden module tolerance
	struct tm* m_pTime;
	time_t m_StartTime,m_EndTime;
private:
	long m_nID;
	static CHSRApp* m_pInstance;

	CHSRFace* m_pFaceList[3];
	list<LINE_T> m_lstLine;		// line of vertical face
	
	friend CHSRExport;
	friend CHSRNewExport;	//! add by HumKyung.Baek - 2010.11.12
};

//////////////////////////////////////////////////////////////////////////
extern CHSRApp* GetHSRApp();

#endif