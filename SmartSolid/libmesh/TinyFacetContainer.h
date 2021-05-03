// TinyFacetContainer.h: interface for the CTinyFacetContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYFACETCONTAINER_H__7C79BB50_8869_4E3E_852D_9CBC78515221__INCLUDED_)
#define AFX_TINYFACETCONTAINER_H__7C79BB50_8869_4E3E_852D_9CBC78515221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef	OPENSOLID_EXPORTS
	#define	OPENSOLID_EXT_CLASS	__declspec(dllexport)
#else
	#define	OPENSOLID_EXT_CLASS	__declspec(dllimport)
#endif

#include "../facet.h"
#include <vector>
using namespace std;

class OPENSOLID_EXT_CLASS CTinyFacetContainer  
{
public:
	bool Add(CFacet* pFacet);
	CFacet* GetAt(const long& nIndex);
	const long Size();
	CTinyFacetContainer();
	virtual ~CTinyFacetContainer();

	static CTinyFacetContainer* CreateInstance(){
		return new CTinyFacetContainer;
	}

	static void DeleteInstance(CTinyFacetContainer* p){
		if(p) delete p;
	}
private:
	vector<CFacet*>* m_pFacetEntry;
};

#endif // !defined(AFX_TINYFACETCONTAINER_H__7C79BB50_8869_4E3E_852D_9CBC78515221__INCLUDED_)
