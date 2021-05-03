// SurfacePara.h: interface for the CSurfacePara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACEPARA_H__B26D5D7B_BE62_4C3C_BB96_20D82E0166AE__INCLUDED_)
#define AFX_SURFACEPARA_H__B26D5D7B_BE62_4C3C_BB96_20D82E0166AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef	OPENSOLID_EXPORTS
	#define	OPENSOLID_EXT_CLASS	__declspec(dllexport)
#else
	#define	OPENSOLID_EXT_CLASS	__declspec(dllimport)
#endif

class OPENSOLID_EXT_CLASS CSurfacePara  
{
public:
	void Set(const double& u,const double& v);
	double v();
	double u();
	CSurfacePara();
	CSurfacePara(const double& u,const double& v);
	virtual ~CSurfacePara();
private:
	double m_u;
	double m_v;
};

#endif // !defined(AFX_SURFACEPARA_H__B26D5D7B_BE62_4C3C_BB96_20D82E0166AE__INCLUDED_)
