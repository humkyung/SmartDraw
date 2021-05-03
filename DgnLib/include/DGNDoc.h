#ifndef	__DGNDOC_H__
#define	__DGNDOC_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

#include <SQLib.h>
class DLL_EXPORT CDGNDoc{
public:
	CDGNDoc();
	virtual ~CDGNDoc();
public:
	void Set(PSQPRIMITIVE pSet);
	PSQPRIMITIVE GetHead();
	void* CreateObject(size_t nSiz);
private:
	PSQPRIMITIVE m_pHead;
};

#endif