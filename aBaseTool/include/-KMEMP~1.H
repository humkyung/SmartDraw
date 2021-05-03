#ifndef __KMEMPOOL_H__
#define __KMEMPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>
using namespace std;

#include "windows.h"
//	description	:
//	remarks		:
template<class T>
class KMemPool{
	typedef struct tagPol{
		bool use;
		T*   pol;
		
		struct tagPol *next;
	}POL,* PPOL;
private:
	list<T*> m_lstMemPool;
public:
	KMemPool(){
		m_pPool = NULL;
	}

	virtual ~KMemPool(){
		Pure();
	}
public:
	//	parameter	:
	//	description	: first look for unused memory block, 
	//			: if not exist, allocate memory.
	//	remarks		:
	//	returns		:
	inline T* Pick(){
		T* ptr=(T*)calloc(1,sizeof(T));
		if(ptr) m_lstMemPool.push_back(ptr);

		return ptr;
		//T* content = NULL;
		
		//content = link();
		
		//return content;
	}

	//	paramter	:
	//	description	: compare t with each pol in pool list
	//			: and if address is equal, set use to false ,return true.
	//	remarks		:
	//	return		:
	bool Cast(T* t){
		return true;
		//bool ret = false;
		//t = t;

		//ret = true;
		//return ret;
	}

	//	parameter	:
	//	description	: set use flag to flase.
	//	remarks		:
	//	returns		:
	void CastAll(){
#ifdef _DEBUG
		for(POL* ptr = m_pPool;ptr;ptr = ptr->next) ptr->use = false;
#else
#endif
	}

	//	parameter	:
	//	description	: delete contents in pool,(but later contents should be reused)
	//			: and it's container.
	//	remarks		:
	//	returns		:
	void Pure(){
		for(list<T*>::iterator itr=m_lstMemPool.begin();itr != m_lstMemPool.end();){
			if(*itr){
				free((void*)*itr);
				(*itr) = NULL;
			}
			m_lstMemPool.erase(itr++);
		}
	}
private:
	//	parameter	:
	//	description	: allocate memory, and append it to memory block list.
	//	remarks		:
	//	returns		:
	inline T* link(){
		PPOL container=NULL;
		T*   content  =NULL;

		if(NULL != (container = (PPOL)calloc(1,sizeof(POL)))){
			if(NULL != (content = (T*)calloc(1,sizeof(T)))){
				container->use = true;
				container->pol = content;
				container->next= NULL;
				container->next= m_pPool;
				m_pPool = container;
				//if(m_pPool){
				//	container->next = m_pPool;
				//	m_pPool         = container;
				//}else	m_pPool = container;
			}
		}
				
		return content;
	}
private:
	PPOL m_pPool;
};

#endif