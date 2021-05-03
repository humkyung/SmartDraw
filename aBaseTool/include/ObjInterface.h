#ifndef __OBJ_INTERFACE_H__
#define __OBJ_INTERFACE_H__

#ifndef	DLL_EXPORT 
	#define DLL_EXPORT __declspec(dllexport) 
#endif

class DLL_EXPORT CObjInterface{
public:
	virtual ~CObjInterface();
public:
	bool IsKindOf(const long nId);
protected:
	unsigned long m_nId;
};

#endif
