#ifndef __DOC_INTERFACE_H__
#define __DOC_INTERFACE_H__

#include <ObjInterface.h>

#include <list>
using namespace std;

class DLL_EXPORT CDocInterface{
public:
	CDocInterface();
	~CDocInterface();
public:
	virtual bool Append(CObjInterface* pObj)=0;
	virtual list<CObjInterface*>* GetObjList()=0;
protected:
	list<CObjInterface*>* m_plstObj;
};

#endif
