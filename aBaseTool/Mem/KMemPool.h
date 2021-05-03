#ifndef __KMEMPOOL_H__
#define __KMEMPOOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

template<class T>
class KMemPool{
public:
	class node{
	public:
		~node(){
			delete data;
		}
	private:
		T* data;
		class node* next;
		friend KMemPool;
	};
public:
	KMemPool(){
		m_pUsedData = m_pUnusedData = NULL;
	}
	virtual ~KMemPool(){
		DestroyAll();
	}
public:
	T* Pick(){
		if(m_pUnusedData){
			node* ptr = m_pUnusedData;
			m_pUnusedData = m_pUnusedData->next;
			
			ptr->next     = m_pUsedData;
			m_pUsedData   = ptr;

			return ptr->data;
		}else{
			node* ptr = NULL;
//			if(ptr = new node){
//				if(ptr->data = new T){
			if(NULL != (ptr = (node*)::GlobalAlloc(GMEM_FIXED,sizeof(node)))){
				if(NULL != (ptr->data = (T*)::GlobalAlloc(GMEM_FIXED,sizeof(T)))){
					memset(ptr->data,0,sizeof(T));
					ptr->next  = m_pUsedData;
					m_pUsedData= ptr;

					return ptr->data;
				}
			}
		}

		return NULL;
	}

	//	parameter	: 
	//	description	: unuse memory
	//	remarks		:
	//	returns		:
	bool Cast(T* data){
		assert(data);

		if(data){
			node *ptr = NULL,*prev = NULL;
			for(ptr = m_pUsedData;ptr;ptr = ptr->next){
				if(data == ptr->data){ // compare memory address
					if(prev){
						prev->next = ptr->next;
						
						ptr->next     = m_pUnusedData;
						m_pUnusedData = ptr;
					}else{
						m_pUsedData = ptr->next;

						ptr->next     = m_pUnusedData;
						m_pUnusedData = ptr;
					}
					return true;
				}
				prev = ptr;
			}
		}
		
		return false;
	}

	//	parameter	: 
	//	description	: unuse all memories
	//	remarks		:
	//	returns		:
	bool CastAll(){
		node *ptr = NULL,*prev = NULL;
		for(ptr = m_pUsedData;ptr && ptr->next;ptr = ptr->next);
		if(ptr){
			ptr->next     = m_pUnusedData;
			m_pUnusedData = m_pUsedData;

			m_pUsedData = NULL;
			
			return true;
		}
		
		return false;
	}

	//	parameter	: 
	//	description	: destroy all allocated memories
	//	remarks		:
	//	returns		:
	void DestroyAll(){
		node *ptr = NULL,*prev = NULL;
		for(ptr = m_pUnusedData;ptr;){
			prev = ptr;
			ptr  = ptr->next;

			//delete prev;
			::GlobalFree((HGLOBAL)prev->data);
			::GlobalFree((HGLOBAL)prev);
			prev = NULL;
		}
		m_pUnusedData= NULL;
	}
private:
	node* m_pUsedData;
	node* m_pUnusedData;
};

#endif