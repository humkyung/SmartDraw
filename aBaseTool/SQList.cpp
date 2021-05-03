template<class T>
SQList<T>::SQList(){
}
template<class T>
SQList<T>::SQList(const T& val){
	push_back(val);
}
template<class T>
SQList<T>::SQList(const SQList<T>& lst){
	(*this) = lst;
}
template<class T>
SQList<T>::~SQList(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T& SQList<T>::First(){
	SQList<T>::iterator i = begin();

	return (*i);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T& SQList<T>::Second(){
	SQList<T>::iterator i = begin();

	return (*(i+1));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T& SQList<T>::Third(){
	SQList<T>::iterator i = begin();

	return (*(i+2));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T& SQList<T>::Fourth(){
	SQList<T>::iterator i = begin();

	return (*(i+3));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T& SQList<T>::Nth(const int index){
    	int count;
	SQList<T>::iterator i = begin();

	for(i = begin(),count = 0;i != end() && count < index;++i,count++);

	return (*i);
}

//template<class T>
//void SQList<T>::Sort(bool (*pfCompare)(const SQValue&,const SQValue&)){
//	stable_sort(begin(),end(),NULL);
//}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
void SQList<T>::Collect(int count,...){
	if(count >= 1){
		va_list ap;
		
		va_start(ap,count);
		for(int i = 0;i < count;i++) push_back(va_arg(ap,T));
		va_end(ap);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
SQList<T>* SQList<T>::AppendSublist(){
	static T data;

	data.type	  = sqlist;
	data.data.m_pList = new SQList<SQValue>;
	push_back(data);
	data.type	  = sqnull;

	return data.data.m_pList;
}

template<class T>
SQList<T>* SQList<T>::list(){
	return (new SQList<T>);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
SQList<T> operator +(SQList<T>& operand1,SQList<T>& operand2){
	SQList<T> ret;

	ret = operand1;
	ret.merge(operand2);

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
bool SQList<T>::operator =(DList* dlist){
	bool ret = false;
	
	if(dlist){
		SQList<SQValue>* sqlist = NULL;
		DList* p = NULL;

		for(p = dlist;p;p = p->next){
			switch(p->val.type){
				case df_string:
					push_back(VAL(p->val.data.string));
				break;
				case df_integer:
					push_back(VAL(p->val.data.integer));
				break;
				case df_number:
					push_back(VAL(p->val.data.number));
				break;
				case df_point:{
					SQPoint pt(p->val.data.point->x,p->val.data.point->y,p->val.data.point->z);
					push_back(VAL(pt));
					      }
				break;
				case df_list:
					push_back(VAL(sqlist = new SQList<SQValue>));
					(*sqlist) = p->val.data.list;
				break;
			}
		}

		ret = true;
	}

	return ret;
}
