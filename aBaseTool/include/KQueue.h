//:	name		: KQueue.h
//	author		: baek hum kyung(baekhk@hanmail.net)
//	date		:
//	description	:
//	update		:

#ifndef	__KQUEUE_H__
#define	__KQUEUE_H__

#include <stdio.h>

template <class T>
class KQueue{
public:
//	constructure and destructure
	KQueue();
	virtual ~KQueue();
    
	bool Push(const T &e);
	bool Pop(T &e);
	void Clear();
	bool IsEmpty();

	class iterator;
	friend class iterator;
private:
	struct cell{
		cell(const T &e, cell *p) : element(e),next(p) {}
		cell *next;
		T element;
        };
	cell *first, *last;

public:
	class iterator{
		public:
			iterator(const KQueue<T> &q) :pc(q.first) { }
			T *next(){
				T *pt = 0;
				if (pc != 0){
					pt = &pc->element;
					pc = pc->next;
				}
				return pt;
			}
		private:
			cell *pc;
	};

};

//--------------------------------------------------------------------------------------------
template <class T>
inline KQueue<T>::KQueue() : first(0), last(0){}

template <class T>
inline KQueue<T>::~KQueue(){
	Clear();
}

//--------------------------------------------------------------------------------------------
// the "implement" part

//	push item to queue
template <class T>
bool KQueue<T>::Push(const T &e){
	cell *p = new cell(e, 0);
    
	if(p == NULL)	return false;
	if (first == 0)	first = p;
	else		last->next = p;
	last = p;

	return true;
}

template <class T>
void KQueue<T>::Clear(){
	cell *p;

	while (first != 0){
		p = first;
		first = first->next;
		delete p;
        }
}

//	pop item from queue
template <class T>
bool KQueue<T>::Pop(T &e){
	if (first == 0)	return false;
	cell *p = first;
	if ((first = first->next) == 0)	last = 0;
	e = p->element;
	delete p;
	
	return true;
}

template <class T>
bool KQueue<T>::IsEmpty(){
	if(first == 0 && last == 0)	return true;
	return false;
}

#endif