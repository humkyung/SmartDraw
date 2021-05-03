#include <assert.h>
#include <string.h>
#include "Throwable.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KThrowable::KThrowable (const char* pMessage){
    	assert(msg && "msg is NULL");

	nType   = MSG_WARNING;
	from[0] = '\0';
    	if(msg) strncpy(msg,msg,MAX_MSG_LENGTH - 1);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KThrowable::KThrowable (const KThrowable& rhs){
	nType = rhs.nType;
	strcpy(from,rhs.from);
    	strcpy(msg,rhs.msg);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KThrowable::~KThrowable(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
char* KThrowable::message(){
	return msg;
}
