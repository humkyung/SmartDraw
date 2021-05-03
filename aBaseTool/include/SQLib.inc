#include <assert.h>

namespace node{

	// linked list node
	template <class T>
	struct snode{
		void operator()(T ele){
			memcpy(&element,&ele,sizeof(T));
		}
		
		T element;
		snode<T>* next;
	};

	template <class T>
	struct dnode{
		void operator()(T ele){
			memcpy(&element,&ele,sizeof(T));
		}
		
		T element;
		dnode<T> *prev,*next;
	};

	// quad-tree node
	template <class T>
	struct qtnode{
		void operator()(T ele){
			memcpy(&element,&ele,sizeof(T));
		}

		T element;
		qtnode<T> *ul,*ur,*ll,*lr;
	};

	//	parameter	:
	//	description	:
	//	remarks		:
	//	returns		:
	template<class T>
	size_t SizeOfList(T* pList){
		assert(pList && "pList is NULL");
		size_t size=0;

		if(pList){
			for(T* ptr = pList;ptr/* && (ptr != pList)*/;ptr = ptr->next,size++);
		}

		return size;
	}

	//	parameter	:
	//	description	:
	//	remarks		:
	//	returns		:
	template<class T>
	T* EndOfList(T* pList){
		assert(pList && "pList is NULL");
		T* pEndOfList=NULL;

		if(pList){
			for(T* ptr = pList;ptr && (ptr != pList);pEndOfList=ptr,ptr -> next);
		}

		return pEndOfList;
	}

	//	parameter	:
	//	description	: unique each of contents
	//	remarks		: end of pSrc must be NULL.
	//	returns		:
	template<class T>
	T* ListPush(T* pDest,T* pSrc)
	{
		assert(pSrc && "pSrc is NULL");

		if(pSrc)
		{
			if(pDest)
			{
				T* ptr = pDest;
				for(ptr = pDest;ptr && ptr->next;ptr = ptr->next)
				{
					assert(ptr != pSrc);
				}
				ptr->next = pSrc;
				//pSrc->prev= ptr;
				//pSrc->next= NULL;

				return pDest;
			}
			else
			{
				pDest = pSrc;
				//pDest->prev = pDest->next = NULL;
				
				return pSrc;
			}
		}

		return pDest;
	}

	//	parameter	:
	//	description	: push pValue at front of pList.
	//	remarks		:
	//	returns		:
	template<class T>
	T* ListPushFront(T* pList,T* pValue){
		assert(pValue && "value is invalid");

		if(pList && pValue){
			T* ptr = NULL;
			for(ptr=pValue;ptr && ptr->next;ptr = ptr->next);
			if(ptr){
				ptr->next = pList;
				//pValue->prev = NULL;
				//pList->prev  = ptr;
			}

			return pValue;
		}else if(pValue){
			//pValue->prev = NULL;
			//pValue->next = NULL;

			return pValue;
		}

		return pList;
	}

	//	parameter	:
	//	description	:
	//	remarks		:
	//	returns		:
	template<class T>
	void MakeCircularList(T* pList){
		assert(pList && "pList is NULL");
		
		if(T* pEndOfList=EndOfList(pList)){
			pEndOfList->next = pList;
		}
	}

	//	parameter	:
	//	description	:
	//	remarks		:
	//	returns		:
	template<class T>
	void MakePlaneList(T* pList){
		assert(pList && "pList is NULL");
		
		if(T* pEndOfList=EndOfList(pList)){
			pEndOfList->next = NULL;
		}
	}
}
