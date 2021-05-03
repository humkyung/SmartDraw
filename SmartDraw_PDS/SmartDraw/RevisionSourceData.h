#pragma once

#include <util/Markup/Markup.h>
#include <RevisionChart.h>
#include <unordered_map>
#include <vector>
using namespace std;

class CRevisionData
{
public:
	CRevisionData();
	~CRevisionData();

	int Save(CMarkup& xml);						/// 2012.03.31 added by humkyung

	STRING_T ToString() const;
	STRING_T& Value(const STRING_T& sKey);		/// 2012.03.31 added by humkyung
	STRING_T Value(const STRING_T& sKey) const;	/// 2012.03.31 added by humkyung
	int Parse(CMarkup &xml);
	
	typedef std::tr1::unordered_map<STRING_T , STRING_T>::const_iterator iterator;
	iterator begin()
	{
		return m_pValueMap->begin();	
	}
	iterator end()
	{
		return m_pValueMap->end();	
	}
private:
	std::tr1::unordered_map<STRING_T , STRING_T>* m_pValueMap;
};

class CRevisionSourceData
{
public:
	CRevisionSourceData(void);
	~CRevisionSourceData(void);

	CRevisionData* GetNextRevDataOf(const STRING_T& sRevNo);	/// 2014.11.16 added by humkyung
	CRevisionData* GetRevDataOf(const STRING_T& sRevNo);		/// 2014.11.16 added by humkyung
	CRevisionData* GetRevDataOf(const int&);					/// 2014.11.16 added by humkyung

	vector<CRevisionData*>* GetRevisionDataList();	/// 2012.03.31 added by humkyung
	int Clear();	/// 2012.03.31 added by humkyung
	int Load(LPCTSTR pFilePath);
	int Save();		/// 2012.03.31 added by humkyung
private:
	STRING_T m_sFilePath;
	vector<CRevisionData*> m_oRevisionDataList;
};
