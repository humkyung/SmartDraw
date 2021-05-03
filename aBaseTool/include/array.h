//:	name		: KArray.h
//	author		: 백흠경(baek hum kyung)
//	date		: 99.?.?
//	description	:
//	update		:

#ifndef __KARRAY_H__
#define __KARRAY_H__

//#include <iostream.h>
#include <assert.h>

//	m_nLength는 배열의 크기를 나타낸다.
template<class T>
class KArray{
public:
   	KArray(int length = 0);
 	KArray(int length,const T& value);
	KArray(int length,const T *data);
   	KArray(const KArray<T>& array);
  	virtual ~KArray();
public:

	KArray<T>& operator=(const KArray<T>& array);
  	bool operator==(const KArray<T>& array) const;
  	int operator!=(const KArray<T>& array) const;

  	int Length() const { return m_nLength; }
  	const T& operator[](int index) const;
    	T& operator[](int index);
private:
  	int m_nLength;
  	T*  m_pValue;
};

template<class T> 
inline KArray<T>::KArray(int length) : m_nLength(length){
	assert( m_nLength >= 0 );

	m_pValue = new T[m_nLength];
}
template<class T> 
inline KArray<T>::KArray(int length, const T& value) : m_nLength(length){
	assert( m_nLength >= 0 );

	m_pValue = new T[m_nLength];
  	for( int n = 0; n < m_nLength; n++ )	m_pValue[n] = value;
}
template<class T>
inline KArray<T>::KArray(int length,const T *data) : m_nLength(length){
   	assert(m_nLength >= 0);

	m_pValue = new T[m_nLength];
	for(int i = 0;i < m_nLength;i++)	m_pValue[i] = data[i];
}
template<class T> 
inline KArray<T>::KArray(const KArray<T>& array) : m_nLength(array.m_nLength){
	assert( m_nLength >= 0 );

	m_pValue = new T[m_nLength];
  	for( int n = 0; n < m_nLength; n++ )	m_pValue[n] = array.m_pValue[n];
}
template<class T> 
inline KArray<T>::~KArray(){
  	delete [] m_pValue;
}

template<class T> 
inline KArray<T>& KArray<T>::operator=(const KArray<T>& array){
  	if( this != &array ){
		delete [] m_pValue;
   		
		m_nLength = array.m_nLength;
   		m_pValue  = new T[m_nLength];
   		for( int n = 0; n < m_nLength; n++ )	m_pValue[n] = array.m_pValue[n];
  	}

	return *this;
}

//	비교 연산자.
template<class T> 
inline bool KArray<T>::operator==(const KArray<T>& array) const {
   	bool ret;

	ret = (m_nLength == array.m_nLength) ? true : false;
  	for(int n = 0;ret == true && ( n < m_nLength ); n++ )
   		ret = (m_pValue[n] == array.m_pValue[n]) ? true : false;

	return true;
}

template<class T> 
inline int KArray<T>::operator!=(const KArray<T>& array) const {
  	return !operator==(array);
}

//	인덱스에 해당하는 값을 리턴한다.
template<class T>
inline const T& KArray<T>::operator[](int index) const {
  	assert(index >= 0);
  	assert(index < m_nLength);

	return m_pValue[index];
}
template<class T> 
inline T& KArray<T>::operator[](int index){
  	assert( index >= 0 );
  	assert( index < m_nLength );

	return m_pValue[index];
}

//	배열의 내용을 표준 출력 장치로 출력한다.
//template<class T> 
//inline ostream& operator<<(ostream &stream,const KArray<T> &array){
//  	for(int n = 0;n < array.Length();n++)	stream << array[n] << endl;

//	return stream;
//}

#endif





