// GLFont.cpp: implementation of the CGLFont class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <assert.h>

#include <math.h>
#include <OpenGL\GLFont.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLFont::CGLFont()
{
	m_FontType = GL_FONT_SOLID;
	m_uiListID = 0;
	
	m_dWidthFactor = 1.f;
	m_dThick = 0.1;
	m_dXOffset = 0.0;     
	m_dYOffset = 0.0;
	m_dZOffset = 0.0;
	m_dXScale = 1.0;
	m_dYScale = 1.0;
	m_dZScale = 1.0;
	m_dXRotate = 0.0;
	m_dYRotate = 0.0;
	m_dZRotate = 0.0;     
	
	m_dMEmission[0] = (float)0.1;
	m_dMEmission[1] = (float)0.1;
	m_dMEmission[2] = (float)0.9;
	m_dMEmission[3] = (float)1.0;
	
	m_dMSpecular[0] = (float)0.9;
	m_dMSpecular[1] = (float)0.1;
	m_dMSpecular[2] = (float)0.1;
	m_dMSpecular[3] = (float)1.0;
	
	m_dMAmbient[0] = (float)0.1;
	m_dMAmbient[1] = (float)0.9;
	m_dMAmbient[2] = (float)0.1;
	m_dMAmbient[3] = (float)1.0;
	
	m_dMDiffuse[0] = (float)0.8;
	m_dMDiffuse[1] = (float)0.8;
	m_dMDiffuse[2] = (float)0.8;
	m_dMDiffuse[3] = (float)1.0;
	
	m_dMShininess = 100.0;
}

CGLFont::~CGLFont()
{
	if(m_uiListID != 0)
		glDeleteLists(m_uiListID, FONTLIST);
}


//////////////////////////////////////////////////////////////////////
// Set the material emission of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::SetEmission(float dMEmission[4])
{
	m_dMEmission[0] = dMEmission[0];
	m_dMEmission[1] = dMEmission[1];
	m_dMEmission[2] = dMEmission[2];
	m_dMEmission[3] = dMEmission[3];
}


//////////////////////////////////////////////////////////////////////
// Get the material emission of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::GetEmission(float dMEmission[4])
{
	dMEmission[0] = m_dMEmission[0];
	dMEmission[1] = m_dMEmission[1];
	dMEmission[2] = m_dMEmission[2];
	dMEmission[3] = m_dMEmission[3];
}


//////////////////////////////////////////////////////////////////////
// Set the material specular of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::SetSpecular(float dMSpecular[4])
{
	m_dMSpecular[0] = dMSpecular[0];
	m_dMSpecular[1] = dMSpecular[1];
	m_dMSpecular[2] = dMSpecular[2];
	m_dMSpecular[3] = dMSpecular[3];
}


//////////////////////////////////////////////////////////////////////
// Get the material specular of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::GetSpecular(float dMSpecular[4])
{
	dMSpecular[0] = m_dMSpecular[0];
	dMSpecular[1] = m_dMSpecular[1];
	dMSpecular[2] = m_dMSpecular[2];
	dMSpecular[3] = m_dMSpecular[3];
}

	
//////////////////////////////////////////////////////////////////////
// Set the material ambient of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::SetAmbient(float dMAmbient[4]) 
{
	m_dMAmbient[0] = dMAmbient[0];
	m_dMAmbient[1] = dMAmbient[1];
	m_dMAmbient[2] = dMAmbient[2];
	m_dMAmbient[3] = dMAmbient[3];
}


//////////////////////////////////////////////////////////////////////
// Get the material ambient of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::GetAmbient(float dMAmbient[4]) 
{
	dMAmbient[0] = m_dMAmbient[0];
	dMAmbient[1] = m_dMAmbient[1];
	dMAmbient[2] = m_dMAmbient[2];
	dMAmbient[3] = m_dMAmbient[3];
}


//////////////////////////////////////////////////////////////////////
// Set the material diffuse of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::SetDiffuse(float dMDiffuse[4])
{
	m_dMDiffuse[0] = dMDiffuse[0];
	m_dMDiffuse[1] = dMDiffuse[1];
	m_dMDiffuse[2] = dMDiffuse[2];
	m_dMDiffuse[3] = dMDiffuse[3];
}


//////////////////////////////////////////////////////////////////////
// Get the material diffuse of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::GetDiffuse(float dMDiffuse[4])
{
	dMDiffuse[0] = m_dMDiffuse[0];
	dMDiffuse[1] = m_dMDiffuse[1];
	dMDiffuse[2] = m_dMDiffuse[2];
	dMDiffuse[3] = m_dMDiffuse[3];
}

	
//////////////////////////////////////////////////////////////////////
// Set the material shininess of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::SetShininess(float dMShininess)  
{
	m_dMShininess = dMShininess;
}


//////////////////////////////////////////////////////////////////////
// Get the material shininess of the font 
//////////////////////////////////////////////////////////////////////
void CGLFont::GetShininess(float* dMShininess)
{
	*dMShininess = m_dMShininess;
}

//////////////////////////////////////////////////////////////////////
// Create the font to display the text 
//////////////////////////////////////////////////////////////////////
BOOL CGLFont::CreateFont(HDC hDC, char* fontname)
{
	m_uiListID = glGenLists(FONTLIST);

	if(hDC == NULL || m_uiListID == 0)
	{
		return FALSE;
	}

	//Create the font to display
	CFont newfont;
	CFont* oldfont;
	BOOL  bresult;
	GLYPHMETRICSFLOAT gmf[FONTLIST];

	if(fontname != NULL){	
		bresult = newfont.CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
			OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			FF_DONTCARE|DEFAULT_PITCH, fontname);
		
		//if new font failed to be created
		if(!bresult){
			return FALSE;
		}
		
		oldfont = (CFont*)::SelectObject(hDC,newfont);
	}else{
		//oldfont = (CFont*)::SelectStockObject(SYSTEM_FONT);
	}

	if(m_FontType == GL_FONT_SOLID)
	{
		bresult = wglUseFontOutlines(hDC, 0, FONTLIST, m_uiListID, 
			0.0f, (float)m_dThick, WGL_FONT_POLYGONS, gmf);	
	}
	else if(m_FontType == GL_FONT_LINE)
	{
		bresult = wglUseFontOutlines(hDC, 0, FONTLIST, m_uiListID, 
			0.0f, (float)m_dThick, WGL_FONT_LINES, gmf);	
	}
	
	::SelectObject(hDC,oldfont);
	newfont.DeleteObject();
	
	if(!bresult){
		return FALSE;
	}

	return TRUE;
}

BOOL CGLFont::CreateFont(CDC* pDrawDC, char* fontname)
{
	m_uiListID = glGenLists(FONTLIST);

	if(pDrawDC == NULL || m_uiListID == 0)
	{
        return FALSE;
	}

	//Create the font to display
	CFont newfont;
	CFont* oldfont;
	BOOL  bresult;
	GLYPHMETRICSFLOAT gmf[FONTLIST];

	if(fontname != NULL)
	{	
        bresult = newfont.CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		       OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
               FF_DONTCARE|DEFAULT_PITCH, fontname);

	    //if new font failed to be created
	    if(!bresult)
		{
		    return FALSE;
		}

	    oldfont = pDrawDC->SelectObject(&newfont);
	}
	else
	{
	    oldfont = (CFont*)pDrawDC->SelectStockObject(SYSTEM_FONT);
	}

	if(m_FontType == GL_FONT_SOLID)
	{
        bresult = wglUseFontOutlines(pDrawDC->m_hDC, 0, FONTLIST, m_uiListID, 
			0.0f, (float)m_dThick, WGL_FONT_POLYGONS, gmf);	
	}
	else if(m_FontType == GL_FONT_LINE)
	{
        bresult = wglUseFontOutlines(pDrawDC->m_hDC, 0, FONTLIST, m_uiListID, 
			0.0f, (float)m_dThick, WGL_FONT_LINES, gmf);	
	}
	
	pDrawDC->SelectObject(oldfont);
	newfont.DeleteObject();
	
	if(!bresult)
	{
		return FALSE;
	}

	return TRUE;
}

/**	\brief	The CGLFont::TextLength function

	\param	str	a parameter of type const char*
	\param	nScale	a parameter of type double

	\return	float	
*/
float CGLFont::TextLength(const char* str,const double& nWidthFactor,const double& nScale){
	assert(str && "str is NULL");
	float nRet=0;

	if(str){
		double _nScale=nScale;
		for(char* ptr=(char*)str;*ptr;ptr++){
			nRet += m_nAdvances[*ptr]*_nScale;
		}
		nRet *= nWidthFactor;
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////
// display the text string
//////////////////////////////////////////////////////////////////////
/**	\brief	The CGLFont::GLDrawText function

	\param	x	a parameter of type float
	\param	y	a parameter of type float
	\param	z	a parameter of type float
	\param	pszText	a parameter of type const char*

	\return	void	
*/
void CGLFont::GLDrawText(float x,float y,float z,const char* pszText){
	assert(pszText && "pszText is NULL");

	if(pszText){
		double cosval=cos(DEG2RAD(m_dZRotate));
		double sinval=sin(DEG2RAD(m_dZRotate));

		float xoff=0.;
		float xloc=0.,yloc=0.,nScale=m_dXScale;
		for(char* ptr=(char*)pszText;*ptr;ptr++){
			GLDrawChar(x + xloc,y + yloc,z,*ptr);
			xoff += m_nAdvances[*ptr]*nScale*m_dWidthFactor;
			xloc = cosval*xoff;
			yloc = sinval*xoff;
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLFont::GLDrawText(CDC* pDC,float x,float y,float z,const char* pszText){
	assert(pDC && "pDC is NULL");
	assert(pszText && "pszText is NULL");

	if(pDC && pszText){
		double cosval=cos(DEG2RAD(m_dZRotate));
		double sinval=sin(DEG2RAD(m_dZRotate));

		float xoff=0.;
		float xloc=0.,yloc=0.,nScale=m_dXScale;
		long nLen=strlen(pszText);
		for(int i = 0;i < nLen;i++){
			GLDrawChar(pDC,x + xloc,y + yloc,z,pszText[i]);
			xoff += m_nAdvances[pszText[i]]*nScale;
			xloc = cosval*xoff;
			yloc = sinval*xoff;
		}
	}
}

void CGLFont::GLDrawText()
{
	if (m_uiListID == 0 || m_strText.IsEmpty() ||
		m_dXScale == 0.0 || m_dYScale == 0.0 ||
		m_dZScale == 0.0)
	{
		return;
	}
	
	GLsizei size = m_strText.GetLength();
	
	glPushMatrix();
	//set material mission
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_dMEmission);
	//set material specular
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_dMSpecular);
	//set material ambient
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_dMAmbient);
	//set material diffuse
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dMDiffuse);
	//set material shininess
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_dMShininess);
	
	//Translate 
	glTranslated(m_dXOffset, m_dYOffset, m_dZOffset);
	//Scale
	glScaled(m_dXScale, m_dYScale, m_dZScale); 
	//Rotate around X-axis
	glRotated(m_dXRotate, 1.0, 0.0, 0.0);
	//Rotate around Y-axis
	glRotated(m_dYRotate, 0.0, 1.0, 0.0);
	//Rotate around Z-axis
	glRotated(m_dZRotate, 0.0, 0.0, 1.0);
	
	//display the letter
	glListBase(m_uiListID);
	glCallLists(size, GL_UNSIGNED_BYTE, (const GLvoid*)m_strText.GetBuffer(size));
	
	//restore the original angle around Z-axis
	glRotated(-1.0f * m_dZRotate, 0.0, 0.0, 1.0);
	//restore the original angle around Y-axis
	glRotated(-1.0f * m_dYRotate, 0.0, 1.0, 0.0);
	//restore the original angle around X-axis
	glRotated(-1.0f * m_dXRotate, 1.0, 0.0, 0.0);
	//restore the original scale
	glScaled(1.0/m_dXScale, 1.0/m_dYScale, 1.0/m_dZScale); 
	//restore the original position
	glTranslated(-m_dXOffset, -m_dYOffset, -m_dZOffset);
	
	glPopMatrix();
}

/**	\brief	The CGLFont::GLDrawChar function

	\param	x	a parameter of type float
	\param	y	a parameter of type float
	\param	z	a parameter of type float
	\param	ch	a parameter of type const char

	\return	void	
*/
void CGLFont::GLDrawChar(float x,float y,float z,const char ch){
	VCHAR_T* ptrChar=m_ptrChar[ch];
	if(ptrChar){
		const double cosval=cos(DEG2RAD(m_dZRotate));
		const double sinval=sin(DEG2RAD(m_dZRotate));

		float xloc=0.,yloc=0.,nScale=m_dXScale;
		for(VDATA_T* ptr=ptrChar->ptrData;ptr;ptr=ptr->next){
			if(MOVETO == ptr->chCode){
				glEnd();
				glBegin(GL_LINE_STRIP);
				xloc = cosval*ptr->ptrFloat[0]*m_dWidthFactor - sinval*ptr->ptrFloat[1];
				yloc = sinval*ptr->ptrFloat[0]*m_dWidthFactor + cosval*ptr->ptrFloat[1];
				glVertex3f(x + xloc*nScale,y + yloc*nScale,0);
			}else if(LINETO == ptr->chCode){
				xloc = cosval*ptr->ptrFloat[0]*m_dWidthFactor - sinval*ptr->ptrFloat[1];
				yloc = sinval*ptr->ptrFloat[0]*m_dWidthFactor + cosval*ptr->ptrFloat[1];
				glVertex3f(x + xloc*nScale,y + yloc*nScale,0);
			}
		}
		glEnd();
	}
}

/**	\brief	The CGLFont::GLDrawChar function

	\param	pDC	a parameter of type CDC*
	\param	x	a parameter of type float
	\param	y	a parameter of type float
	\param	z	a parameter of type float
	\param	ch	a parameter of type const char

	\return	void	
*/
void CGLFont::GLDrawChar(CDC* pDC,float x,float y,float z,const char ch){
	assert(pDC && "pDC is NULL");

	if(pDC){
		VCHAR_T* ptrChar=m_ptrChar[ch];
		if(ptrChar){
			double cosval=cos(DEG2RAD(m_dZRotate));
			double sinval=sin(DEG2RAD(m_dZRotate));

			float xloc=0.,yloc=0.,nScale=m_dXScale;
			for(VDATA_T* ptr=ptrChar->ptrData;ptr;ptr=ptr->next){
				if(MOVETO == ptr->chCode){
					xloc = cosval*ptr->ptrFloat[0] - sinval*ptr->ptrFloat[1];
					yloc = sinval*ptr->ptrFloat[0] + cosval*ptr->ptrFloat[1];
					pDC->MoveTo(x + xloc*nScale*m_dWidthFactor,-y - yloc*nScale);
				}else if(LINETO == ptr->chCode){
					xloc = cosval*ptr->ptrFloat[0] - sinval*ptr->ptrFloat[1];
					yloc = sinval*ptr->ptrFloat[0] + cosval*ptr->ptrFloat[1];
					pDC->LineTo(x + xloc*nScale*m_dWidthFactor,-y - yloc*nScale);
				}
			}
		}
	}
}

/**	\brief	The CGLFont::GetName function


	\return	const char*	
*/
const char* CGLFont::GetName()
{
	return m_szFontName;
}

void CGLFont::SetWidthFactor(const double &dWidthFactor)
{
	m_dWidthFactor = dWidthFactor;
}
