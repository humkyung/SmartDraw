#pragma once

#include <map>
#include <util/Markup/Markup.h>
using namespace std;

class CStringTable
{
	CStringTable(void);
	CStringTable(const CStringTable& ){}
	CStringTable& operator=(const CStringTable&){return (*this);}
public:
	static CStringTable& GetInstance();
	~CStringTable(void);

	int Parse(const STRING_T& sFilePath);
	STRING_T GetString(const STRING_T& sName);
private:
	int ParseString(CMarkup& xml,const LCID& lcid);
	map<STRING_T,STRING_T> m_oStringValueMap;
};
