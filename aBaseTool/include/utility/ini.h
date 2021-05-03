#ifndef	__INI_H__
#define	__INI_H__

#include <windows.h>

#include <list>
using namespace std;

#define	MAX_KEY_LEN	32
#define	MAX_VAL_LEN	64

class CIni  
{
	typedef struct{
		char szKey[MAX_KEY_LEN];
		char szValue[MAX_VAL_LEN];
	}KEY_VALUE;

	typedef struct{
		char szSection[MAX_KEY_LEN];
		list<KEY_VALUE>* plst;
	}SECTION;
public:
	CIni();
	virtual ~CIni();
public:
	bool  Read(const char* pszFilePath);
	char* Value(const char* pszSection,const char* pszKey);
	void  Write();
protected:
	void ParseLine(const char* pszLine);
private:
	char m_szFilePath[MAX_PATH];
	list<SECTION*> m_lstSection;
};

#endif
