#ifndef __KTHROWABLE_H__
#define __KTHROWABLE_H__

#define	MSG_WARNING	1
#define MSG_ERROR	2

#define MAX_MSG_LENGTH	256

typedef struct tagTHROWABLEITEM{
	int  nType;
	char from[1024];
	char where[1024];
	char msg[2048];
	char dump[2048];
}THROWABLEITEM;

/*
#define LOG(THROWABLEITEM& item)\
{\
	COPYDATASTRUCT cd={0};\
\
	HWND hWnd=FindWindow("LogViewer","LogViewer Ver1.0");\
	if(hWnd){\
		cd.dwData = 0;\
		cd.cbData = sizeof(THROWABLEITEM);\
		cd.lpData = (void*)&item;\
		::SendMessage(hWnd,WM_COPYDATA,0,(LPARAM)&cd);\
	}\
}
*/

class KThrowable : public THROWABLEITEM{
public:
	KThrowable (const char* pMessage="");
	KThrowable (const KThrowable&);
	virtual ~KThrowable();
public:
	char* message();
};

#endif
