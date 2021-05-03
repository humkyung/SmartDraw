#ifndef __KMEMPOOL_H__
#define __KMEMPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BufferPool.h"

#include <list>
#include <stack>
using namespace std;

#include "windows.h"

/*------------------------------------------------------------------------
operator new()
  
	new "placement" using the buffer pool instade of the real allocation
  
 
   size_t size                  size of the object a must parameter
   CBufferPool * BufferPool     buffer pool that allocates buffers of the object size
	
------------------------------------------------------------------------*/
inline void * operator new(size_t size , CBufferPool * BufferPool)
{
   //allocate the buffer
   return BufferPool->Allocate();
}
/*------------------------------------------------------------------------
operator delete()

  delete placement using the buffer pool instade of the real allocation
  
 DoomedObject - pointer to the buffer holding the object
 CBufferPool * BufferPool - buffer pool that free buffers of the object size
				
------------------------------------------------------------------------*/
inline void   operator delete(void * DoomedObject , CBufferPool * BufferPool)
{   
   //free the buffer
   BufferPool->Free(DoomedObject);
}



template <class T>
class  CObjectPool
{
public:
   bool Create(IN const unsigned int nNumberOfObjectsInSegment,
               IN const unsigned int nNumberOfSegmentsStrat,
               IN const unsigned int nNumberOfSegmentsLowMark,
               IN const unsigned int nNumberOfSegmentsHighMark = -1);
   
   inline T*   Pick();
   
   inline void Cast(IN T * obj);
   
   void        Pure();

private:
   CBufferPool BufferPool;
};


/*------------------------------------------------------------------------
Create()
	
	  Call the create of the buffer pool with the object size

   IN UINT nNumberOfObjectsInSegment - Number of Objects in a segment
   IN UINT nNumberOfSegmentsStart    - Start Number of Segments
   IN UINT nNumberOfSegmentsLowMark  - Minimum Number of Segments
   IN UINT nNumberOfSegmentsHighMark - Maximum Number of Segments
				
------------------------------------------------------------------------*/

template <class T>
bool CObjectPool<T>::Create(IN const unsigned int nNumberOfObjectsInSegment,
                            IN const unsigned int nNumberOfSegmentsStart,
                            IN const unsigned int nNumberOfSegmentsLowMark,
                            IN const unsigned int nNumberOfSegmentsHighMark /*=-1*/)
{
   //create the buffer pool with a buffer size sizeof(T)
   return BufferPool.Create(sizeof(T),
                     nNumberOfObjectsInSegment,
                     nNumberOfSegmentsStart,
                     nNumberOfSegmentsLowMark,
                     nNumberOfSegmentsHighMark);
}

/*------------------------------------------------------------------------
Allocate()

  returnes a pointer to the object allocated using the buffer pool
		
------------------------------------------------------------------------*/

template <class T>
inline T * CObjectPool<T>::Pick()
{ 
   //call the placement new operator
   return new (&BufferPool) T();
}

/*------------------------------------------------------------------------
Free()
	
  returns the object to the pool allocated using the buffer pool

	T * obj - the pointer to the object allocated using the object pool
				
------------------------------------------------------------------------*/


template <class T>
inline void CObjectPool<T>::Cast(IN T * obj)
{
   //call the placement delete operator
   operator delete(obj,&BufferPool);
}


/*------------------------------------------------------------------------
Destroy()

  Destroys the pool
			
------------------------------------------------------------------------*/

template <class T>
void CObjectPool<T>::Pure()
{
   //Destroy buffer pool
   BufferPool.Destroy();
}

/*
//	description	:
//	remarks		:
template<class T>
class KMemPool{
	typedef struct tagPol{
		bool use;
		T*   pol;
		
		struct tagPol *prev;
		struct tagPol *next;
	}POL,* PPOL;
private:
	vector<T*>  m_vecMemPool;
	stack<T*> m_stkUnused;
	map<T*,long> m_mapIndex;
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
		T* ptr=NULL;
		if(!m_stkUnused.empty()){
			ptr=m_stkUnused.top();m_stkUnused.pop();
			m_vecMemPool.push_back(ptr);
			m_mapIndex[ptr] = m_vecMemPool.size() - 1;
		}else{
			ptr=(T*)calloc(1,sizeof(T));
			if(ptr){
				m_vecMemPool.push_back(ptr);
				m_mapIndex[ptr] = m_vecMemPool.size() - 1;
			}
		}

		return ptr;
	}

	//	paramter	:
	//	description	: compare t with each pol in pool list
	//			: and if address is equal, set use to false ,return true.
	//	remarks		:
	//	return		:
	bool Cast(T* t){
		long where=m_mapIndex[t].second;
		T* ptr=m_vecMemPool[where]
		m_stkUnused.push(*ptr);
		m_lstMemPool.erase(ptr);
		
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
*/

#endif