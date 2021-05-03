#pragma once

#include <string>
using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL CSmartDrawAnnoTextStyle
{
public:
	CSmartDrawAnnoTextStyle(void);
	CSmartDrawAnnoTextStyle(const STRING_T& sFontName , const double& dHeight , const double& dWidthFactor);
	~CSmartDrawAnnoTextStyle(void);

	STRING_T GetFontName() const;
	void SetName(const STRING_T& sName);
	STRING_T GetName() const;
	double height() const;
	double& height();
	int weight() const;
	int& weight();
	double GetWidthFactor() const;
	double GetOblique() const;
	double width() const;	/// return m_nHeight*m_nWidthFactor
public:
	friend OSTREAM_T& operator<<(OSTREAM_T& c, const CSmartDrawAnnoTextStyle& t);
	double GetTextLength(const STRING_T& rTextString);
	int Parse(const STRING_T& sTextStyleName , const STRING_T& rIniFilePath);
private:
	STRING_T m_rName;		/// style name
	STRING_T m_rFontName;	/// font no.
	double m_nHeight , m_nWidthFactor , m_nOblique;
	int m_nWeight;
};
