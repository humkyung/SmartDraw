// GLFont.h: interface for the CGLFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLFONT_H__1F38C981_2654_11D3_955D_8422F5C00000__INCLUDED_)
#define AFX_GLFONT_H__1F38C981_2654_11D3_955D_8422F5C00000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <VFT.h>

#include "gl\gl.h"
#include "gl\glu.h"
#include "gl\glaux.h"

#define FONTLIST    256

enum GLFontType
{
	GL_FONT_SOLID,
	GL_FONT_LINE,
};

class CGLFont : public CVFT
{
public:
	void SetWidthFactor(const double& dWidthFactor);
	const char* GetName();
	CGLFont();
	virtual ~CGLFont();
	
	void   SetFontType(GLFontType FontType){m_FontType = FontType;};
	void   GetFontType(GLFontType* FontType){*FontType = m_FontType;};
	
	void   SetFontSize(double dThick){m_dThick = dThick;};
	void   GetFontSize(double* dThick){*dThick = m_dThick;};
	
	void   SetXOffset(double dXOffset){m_dXOffset = dXOffset;};
	double GetXOffset(void){ return m_dXOffset;};
	
	void   SetYOffset(double dYOffset){m_dYOffset = dYOffset;};
	double GetYOffset(void){ return m_dYOffset;};
	
	void   SetZOffset(double dZOffset){m_dZOffset = dZOffset;};
	double GetZOffset(void){ return m_dZOffset;};
	
	void   SetXScale(double dXScale){m_dXScale = dXScale;};
	double GetXScale(void){ return m_dXScale;};
	
	void   SetYScale(double dYScale){m_dYScale = dYScale;};
	double GetYScale(void){ return m_dYScale;};
	
	void   SetZScale(double dZScale){m_dZScale = dZScale;};
	double GetZScale(void){ return m_dZScale;};
	
	void   SetXRotate(double dXRotate){m_dXRotate = dXRotate;};
	double GetXRotate(void){ return m_dXRotate;};
	
	void   SetYRotate(double dYRotate){m_dYRotate = dYRotate;};
	double GetYRotate(void){ return m_dYRotate;};
	
	void   SetZRotate(double dZRotate){m_dZRotate = dZRotate;};
	double GetZRotate(void){ return m_dZRotate;};
	
	void   SetEmission(float dMEmission[4]);
	void   GetEmission(float dMEmission[4]);
	
	void   SetSpecular(float dMSpecular[4]);
	void   GetSpecular(float dMSpecular[4]);
	
	void   SetAmbient(float dMAmbient[4]); 
	void   GetAmbient(float dMAmbient[4]); 
	
	void   SetDiffuse(float dMDiffuse[4]);
	void   GetDiffuse(float dMDiffuse[4]);
	
	void   SetShininess(float dMShininess);  
	void   GetShininess(float* dMShininess);  
	
	void   SetText(const char* strText){m_strText = strText;};
	
	BOOL CreateFont(HDC  pDrawDC, char* fontname);
	BOOL CreateFont(CDC* pDrawDC, char* fontname);
	
	float TextLength(const char* str,const double& nWidthFactor,const double& nScale);

	void GLDrawText(void);
	void GLDrawText(float x,float y,float z,const char* pszText);
	void GLDrawText(CDC* pDC,float x,float y,float z,const char* pszText);
	
	void GLDrawChar(float x,float y,float z,const char ch);
	void GLDrawChar(CDC* pDC,float x,float y,float z,const char ch);
	//Parameters
public:
	double	     m_dWidthFactor;
private:
	GLFontType   m_FontType;     //font type
	UINT         m_uiListID;     //the ID of the displaylist
	double       m_dThick;       //the letters thick 
	double       m_dXOffset;     //the offset in X direction
	double       m_dYOffset;     //the offset in Y direction
	double       m_dZOffset;     //the offset in Z direction
	double       m_dXScale;      //the scale in X direction
	double       m_dYScale;      //the scale in Y direction
	double       m_dZScale;      //the scale in Z direction
	double       m_dXRotate;     //the rotation around X-axis 
	double       m_dYRotate;     //the rotation around Y-axis 
	double       m_dZRotate;     //the rotation around Z-axis 
	float        m_dMEmission[4];//the material emission of the font 
	float        m_dMSpecular[4];//the material specular of the font
	float        m_dMAmbient[4]; //the material ambient of the font
	float        m_dMDiffuse[4]; //the material diffuse of the font 
	float        m_dMShininess;  //the material diffuse of the font 
	CString      m_strText;      //the text for display    
};

#endif // !defined(AFX_GLFONT_H__1F38C981_2654_11D3_955D_8422F5C00000__INCLUDED_)
