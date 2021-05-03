#include <time.h>
#include "SQError.h"

SQError* SQError::m_pErrorHandler = NULL;

SQError::SQError(){
	m_bLogged       = false;
    	m_pErrorHandler = this;
}

SQError::~SQError(){
	if(!m_bLogged) Log();
}

//	parameter	:
//	description	: log messages
//	remarks		:
//	returns		:
void SQError::Log(){
    	FILE* fp;
	struct tm *ptime;
	time_t tlog;

	_tzset();
	tlog = time(NULL);
	
	if(NULL != (fp = fopen("log.txt","wb"))){
		int n;

		ptime  = localtime(&tlog);
		fprintf(fp,"month : %d , day : %d , hour : %d , min : %d\r\n",ptime->tm_mon,ptime->tm_mday,ptime->tm_hour,ptime->tm_min);
		fprintf(fp,"--------------------------------------------\r\n");
		if(m_lstError.size()){
	    		list<KThrowable>::iterator i;
			for(n = 0,i = m_lstError.begin();i != m_lstError.end();n++,++i){
		    		fprintf(fp,"%5d : %s\r\n",n,i->message());
			}
		}else{
			fprintf(fp,"Congratulations! You have no error.");
		}
		fclose(fp);
		
		m_bLogged = true;
	}
}

void SQError::Pure(){
	m_lstError.clear();
}

bool SQError::operator +=(const KThrowable& err){
    	m_lstError.push_back(err);
	m_bLogged = false;

	return true;
}

SQError theError;

SQError* GetErrorHandler(){
	return SQError::m_pErrorHandler;
}
