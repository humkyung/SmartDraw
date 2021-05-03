//Tokenizer.h

#pragma once

#pragma warning(disable:4786)

#include <functional>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>

using namespace std;

#ifdef	_UNICODE
typedef TCHAR	_CHAR;
typedef wstring	STRING;
#define	TCONV	_T
#else
typedef	char	_CHAR;
typedef string	STRING;
#define	TCONV
#endif

//For the case the default is a space.
//This is the default predicate for the Tokenize() function.
class CIsSpace : public unary_function<_CHAR, bool>{
public:
	bool operator()(_CHAR c) const;
};

inline bool CIsSpace::operator()(_CHAR c) const
{
	//isspace<char> returns true if c is a white-space character (0x09-0x0D or 0x20)
#if (_MSC_VER >= 1300) 
	return isspace(c) ? true : false;
#else
	return isspace<_CHAR>(c) ? true : false;
#endif
}

//For the case the separator is a comma
class CIsComma : public unary_function<_CHAR, bool>
{
public:
	bool operator()(_CHAR c) const;
};

inline bool CIsComma::operator()(_CHAR c) const
{
#ifdef	_UNICODE	
	return (_T(',') == c);
#else
	return (',' == c);		
#endif
}

//For the case the separator is a character from a set of characters given in a string
class CIsFromString : public unary_function<_CHAR, bool>
{
public:
	//Constructor specifying the separators
	CIsFromString::CIsFromString(STRING const& rostr) : m_ostr(rostr) {}
	bool operator()(_CHAR c) const;
	
private:
	STRING m_ostr;
};

inline bool CIsFromString::operator()(_CHAR c) const
{
	int iFind = m_ostr.find(c);
	if(iFind != STRING::npos)
		return true;
	else
		return false;
}

//String Tokenizer
template <class Pred=CIsSpace>
class CTokenizer
{
public:
	//The predicate should evaluate to true when applied to a separator.
	static void Tokenize(vector<STRING>& roResult, STRING const& rostr, Pred const& roPred=Pred());
};

//The predicate should evaluate to true when applied to a separator.
template <class Pred>
inline void CTokenizer<Pred>::Tokenize(vector<STRING>& roResult, STRING const& rostr, Pred const& roPred)
{
	//First clear the results vector
	roResult.clear();

	if(rostr.end() == find_if(rostr.begin() , rostr.end() , roPred))
	{
		roResult.push_back(rostr);
		return;
	}

	STRING::const_iterator it = rostr.begin();
	STRING::const_iterator itTokenEnd = rostr.begin();
	while(it != rostr.end())
	{
		//! Eat seperators
		int is_null = roResult.empty();
		while((it != rostr.end()) && roPred(*it)) //! 끝에 도달하면 종료.
		{
			if(is_null) roResult.push_back(STRING(TCONV("")));
			it++;
			is_null++;
		}
		if(it == rostr.end())
		{
			//! seperator is located at end
			roResult.push_back(STRING(TCONV("")));
			break;
		}

		//Find next token
		itTokenEnd = find_if(it, rostr.end(), roPred);
		//Append token to result
		if(it < itTokenEnd)
			roResult.push_back(STRING(it, itTokenEnd));
		it = itTokenEnd;
	}
}

/**
 * 	author  : BHK
 * 	purpose : 주어진 문자열을 delimiter로 분리한다.
 * 	date    : 2009-05-12 14:02:04 
*/
inline void StringSplit( vector<string>& results , string str, const string& delim)
{
	results.clear();

	int cutAt = 0;
	while( (cutAt = str.find(delim)) != str.npos )
	{
		if(cutAt > 0)
		{
			results.push_back(str.substr(0,cutAt));
		}
		str = str.substr(cutAt+delim.length());
	}
	if(str.length() > 0)
	{
		results.push_back(str);
	}
}

