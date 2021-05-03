/*****************************************************
Copyright Notice & Disclaimer

Copyright © Alessandro Falappa

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose is hereby granted without
fee, provided that the above copyright notice, author statement
appear in all copies of this software and related documentation.

If you make enhancement or you discover bugs, please let me
know

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
ANY KIND, EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT
LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR FITNESS FOR A
PARTICULAR PURPOSE.

IN NO EVENT SHALL ALESSANDRO FALAPPA BE LIABLE FOR ANY
SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE,
AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

///////////////////////////////////////////////
History

v 1.0: first version
v 1.1: added CGLDispList helper class
	   changed previous disp list service in StockDispList
v 1.2: added CGLTesselator helper class
	   added GLCommands pair
	   added CGLQuadric wrapper class

******************************************************/
// CGLEnabledView.cpp : implementation file of version 1.2
//

#include <math.h>
#include <assert.h>
/*******************
 NOTE ABOUT LIBRARIES INCLUSION:
 - Remember to include the appropriate libraries in the link phase
   (look at Project Settings under the Link tab)
 - If you were lucky enough to get SGI's implementation (at present it's
   not availabl nor supported) you can play with it also, just include
   that libraries. SGI's version is faster if you have no GL acceleration
   and if you own a MMX processor
 - These includes below can be moved to stdafx.h to speed up compilation
********************/
#include "GLEnabledView.h"

//* MS openGL libraries (link with OPENGL32.LIB and GLU32.LIB)
#include "gl\gl.h"
#include "gl\glu.h"
//*/
/* SGI openGL libraries (link with OPENGL.LIB and GLU.LIB)
#include "[path-of-SGI-sdk]\include\gl\gl.h"
#include "[path-of-SGI-sdk]\include\gl\glu.h"
//*/
//#include "afxtempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// used to identify a MCD video driver (partial OGL acceleration)
#define INSTALLABLE_DRIVER_TYPE_MASK  (PFD_GENERIC_ACCELERATED|PFD_GENERIC_FORMAT)

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView

IMPLEMENT_DYNCREATE(CGLEnabledView, CView)

CGLEnabledView::CGLEnabledView() : m_nAspectRatio(1.0)/*,m_nFont2D(0)*/{
	
	m_nDispListIndex   = 0;
	
	m_rectClient.left  = 0;
	m_rectClient.top   = 0;
	m_rectClient.right = 0;
	m_rectClient.bottom= 0;
	m_ptClientCenter.x = 0;
	m_ptClientCenter.y = 0;

	m_nScaleX = m_nScaleY = m_nScaleZ = 1.;

	m_bLButtonDown = m_bMButtonDown = m_bRButtonDown = false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CGLEnabledView::~CGLEnabledView()
{
}


BEGIN_MESSAGE_MAP(CGLEnabledView, CView)
	//{{AFX_MSG_MAP(CGLEnabledView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView drawing
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::OnDraw(CDC* pDC){
	// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pDC->GetSafeHdc(),m_hRC);
	
	// clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// call the virtual drawing procedure (to be overridden by user)
	glPushMatrix();
	glEnable(GL_LINE_STIPPLE);
	OnDrawGL();
	glDisable(GL_LINE_STIPPLE);
	glPopMatrix();
	
	// execute OGL commands (flush the OGL graphical pipeline)
	glFinish();
	// if double buffering is used it's time to swap the buffers
	SwapBuffers(m_pDC->GetSafeHdc());
		
	// free the target DeviceContext (window)
	wglMakeCurrent(NULL,NULL);
}

//	parameter	:
//	description	: default drawing routine
//	remarks		:
//	returns		:
void CGLEnabledView::OnDrawGL()
{
	// draw carthesian axes
	glBegin(GL_LINES);
	// red x axis
	glColor3f(1.f,0.f,0.f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glVertex3f(0.9f,0.1f,0.0f);
	glVertex3f(1.0f,0.0f,0.0f);
	glVertex3f(0.9f,-0.1f,0.0f);
	// green y axis
	glColor3f(0.f,1.f,0.f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(0.1f,0.9f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glVertex3f(-0.1f,0.9f,0.0f);
	// blue z axis
	glColor3f(0.f,0.f,1.f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.1f,0.9f);
	glVertex3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,-0.1f,0.9f);
	glEnd();
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView diagnostics

#ifdef _DEBUG
void CGLEnabledView::AssertValid() const
{
	CView::AssertValid();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
	// dump some infos
	CString str;
	GetWindowText(str);
	afxDump<<"\nView Parameters\n\tClient Rectangle :"<<m_rectClient<<"\n\tAspect Ratio :"<<m_nAspectRatio<<"\n";
	afxDump<<"\nWindowTitle :"<<str<<"\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////
// CGLEnabledView Constants

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView initialization and palette helpers

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
BOOL CGLEnabledView::SetupPixelFormat(){
// define default desired video mode (pixel format)
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
        1,								// version number
        PFD_DRAW_TO_WINDOW |			// support window
        PFD_SUPPORT_OPENGL |			// support OpenGL
        PFD_DOUBLEBUFFER,				// double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        16,                             // 32-bit z-buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };
// let the user change some parameters if he wants
	BOOL bDoublBuf;
	ColorsNumber cnum;
	ZAccuracy zdepth;
	VideoMode(cnum,zdepth,bDoublBuf);
//set the changes
	if(bDoublBuf) pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
	else pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	switch(cnum)
	{
	case INDEXED: pfd.cColorBits=8;
	case THOUSANDS: pfd.cColorBits=16;
	case MILLIONS: pfd.cColorBits=24;
	case MILLIONS_WITH_TRANSPARENCY: pfd.cColorBits=32;
	};
	switch(zdepth)
	{
	case NORMAL: pfd.cDepthBits=16;
	case ACCURATE: pfd.cDepthBits=32;
	};

	// ask the system for such video mode
	ASSERT(m_pDC != NULL);
	int pixelformat;
	if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
	{
		AfxMessageBox("ChoosePixelFormat failed");
		return FALSE;
	}
	
	// try to set this video mode    
	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		// the requested video mode is not available so get a default one
		pixelformat = 1;	
		if (DescribePixelFormat(m_pDC->GetSafeHdc(), pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd)==0)
		{
			// neither the requested nor the default are available: fail
			AfxMessageBox("SetPixelFormat failed (no OpenGL compatible video mode)");
			return FALSE;
		}
	}
	
	return TRUE;
}

// these are used to construct an equilibrated 256 color palette
static unsigned char _threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

static unsigned char _twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

static unsigned char _oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

// Windows Default Palette
static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::CreateRGBPalette()
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int n, i;
	
	// get the initially choosen video mode
	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);
	
	// if is an indexed one...
	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		// ... construct an equilibrated palette (3 red bits, 3 green bits, 2 blue bits)
		// NOTE: this code is integrally taken from MFC example Cube
		n = 1 << pfd.cColorBits;
		pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];
		
		ASSERT(pPal != NULL);
		
		pPal->palVersion = 0x300;
		pPal->palNumEntries = n;
		for (i=0; i<n; i++)
		{
			pPal->palPalEntry[i].peRed=ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
			pPal->palPalEntry[i].peGreen=ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
			pPal->palPalEntry[i].peBlue=ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
			pPal->palPalEntry[i].peFlags=0;
		}
		
		// fix up the palette to include the default Windows palette
		if ((pfd.cColorBits == 8)                           &&
			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
			)
		{
			for (i = 1 ; i <= 12 ; i++)
				pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
		}
		
		m_CurrentPalette.CreatePalette(pPal);
		delete pPal;
		
		// set the palette
		m_pOldPalette=m_pDC->SelectPalette(&m_CurrentPalette, FALSE);
		m_pDC->RealizePalette();
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
unsigned char CGLEnabledView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
	unsigned char val;
	
	val = (unsigned char) (i >> shift);
	switch (nbits) 
	{
		
	case 1:
		val &= 0x1;
		return _oneto8[val];
	case 2:
		val &= 0x3;
		return _twoto8[val];
	case 3:
		val &= 0x7;
		return _threeto8[val];
		
	default:
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView message handlers and overridables

int CGLEnabledView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;
	
	// initialize the private member
	m_pDC= new CClientDC(this);
	
	// choose the requested video mode
	if (!SetupPixelFormat()) return 0;
	
	// ask the system if the video mode is supported
	int n=::GetPixelFormat(m_pDC->GetSafeHdc());
	
	PIXELFORMATDESCRIPTOR pfd={0};
	::DescribePixelFormat(m_pDC->GetSafeHdc(),n,sizeof(pfd),&pfd);
	
	// create a palette if the requested video mode has 256 colors (indexed mode)
	CreateRGBPalette();
	
	// link the Win Device Context with the OGL Rendering Context
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	
	// specify the target DeviceContext (window) of the subsequent OGL calls
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	
	// performs default setting of rendering mode,etc..
	OnCreateGL();
	
	// free the target DeviceContext (window)
	wglMakeCurrent(NULL,NULL);
	
	return 0;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::OnCreateGL()
{
	GLfloat ambientLight[] = {0.3f,0.3f,0.3f,1.0f};
	GLfloat diffuseLight[] = {0.7f,0.7f,0.7f,1.0f};
	//GLfloat lightPos[]     = {-50.0f,50.0f,100.0f,1.0f};
	GLfloat lightPos[4] = {0, 0, -1000000, 1};
	
	// perform hidden line/surface removal (enabling Z-Buffer)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	
	// set background color to black
	glClearColor(0.f,0.f,0.f,1.0f );

	// set clear Z-Buffer value
	glClearDepth(1.0f);

	m_GLFont.CreateFont(m_pDC,"Romans Black");
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::OnDestroy() 
{
	//if(m_nFont2D != 0) glDeleteLists(m_nFont2D,96);
	
	// specify the target DeviceContext (window) of the subsequent OGL calls
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	
	// release definitely OGL Rendering Context
	if (m_hRC!=NULL) ::wglDeleteContext(m_hRC);
	
	if(m_nDispListIndex) glDeleteLists(m_nDispListIndex,1);
	m_nDispListIndex = 0;

	// Select our palette out of the dc
	CPalette palDefault;
	palDefault.CreateStockObject(DEFAULT_PALETTE);
	m_pDC->SelectPalette(&palDefault, FALSE);
	
	// destroy Win Device Context
	if(m_pDC) delete m_pDC;
	
	// finally call the base function
	CView::OnDestroy();	
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
BOOL CGLEnabledView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// these styles are requested by OpenGL
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	// these styles are meant for a use of this class in a MDI application
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW);
	
	return CView::PreCreateWindow(cs);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
BOOL CGLEnabledView::OnEraseBkgnd(CDC* pDC) 
{
	// OGL has his own background erasing so tell Windows to skip
	return TRUE;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// when called with a nonzero window:
	if ( 0 < cx && 0 < cy ){
		// update the rect and the aspect ratio
		m_rectClient.right  = cx;
		m_rectClient.bottom = cy;
		m_nAspectRatio=double(cx)/double(cy);
				
		// call the virtual sizing procedure (to be overridden by user)
		m_rectClient.right = cx;
		m_rectClient.bottom= cy;
		m_ptClientCenter.x = (int)(cx*0.5);
		m_ptClientCenter.y = (int)(cy*0.5);
		
		OnSizeGL(cx,cy);				
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::OnSizeGL(int cx, int cy)
{
	wglMakeCurrent(m_pDC->m_hDC,m_hRC);

	//Set viewport dimensions
	::glViewport(0, 0, cx, cy);
	// select the viewing volumn
	::glMatrixMode ( GL_PROJECTION );
	::glLoadIdentity();
	
	if(cx > cy)
		::glOrtho(-cx*0.5,cx*0.5,-cy*0.5,cy*0.5,-cx*100,cx*100);
	else	::glOrtho(-cx*0.5,cx*0.5,-cy*0.5,cy*0.5,-cy*100,cy*100);
	// switch back to the modelview matrix and clear it 
	::glMatrixMode( GL_MODELVIEW ); 
	::glLoadIdentity();
}

/////////////////////////////////////////////////////////////////////////////
// CGLEnabledView public members
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
GLuint CGLEnabledView::FontCreateBitmaps(HDC   hdc,		/* I - Device Context */
						char  *typeface,	/* I - Font specification */
						int   height,		/* I - Font height/size in pixels */
						int   weight,		/* I - Weight of font (bold, etc) */
						DWORD italic)		/* I - Text is italic */
{
	GLuint		base;			/* Base display list for font */
	HFONT		font;			/* Windows font ID */
  
	if ((base = glGenLists(96)) == 0)
		return (0);

	if (stricmp(typeface, "symbol") == 0)
		font = CreateFont(height, 0, 0, 0, weight, italic, FALSE, FALSE,
						  SYMBOL_CHARSET, OUT_TT_PRECIS,
						  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
						  DEFAULT_PITCH, typeface);
	else
		font = CreateFont(height, 0, 0, 0, weight, italic, FALSE, FALSE,
						  ANSI_CHARSET, OUT_TT_PRECIS,
						  CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
						  DEFAULT_PITCH, typeface);
  
	SelectObject(hdc, font);
  
	wglUseFontBitmaps(hdc, 32, 96, base);

	return (base);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::DeleteDisplayList(){
	if(m_nDispListIndex) glDeleteLists(m_nDispListIndex,1);
	m_nDispListIndex = 0;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CGLEnabledView::TextOutGL(	float x,float y,float z,
				float nScale,float nRotate,	/* I - Font to use */
				const char *str)		/* I - String to display */
{
	assert(str && "str is NULL");

	if(str){
		m_GLFont.SetXScale(nScale*0.8);
		m_GLFont.SetYScale(nScale);
		m_GLFont.SetZScale(nScale);
		m_GLFont.SetZRotate(nRotate);
		m_GLFont.GLDrawText(x,y,z,str);
	}
}

void CGLEnabledView::VideoMode(ColorsNumber &c, ZAccuracy &z, BOOL &dbuf)
{
// set default videomode
	c=MILLIONS;
	z=NORMAL;
	dbuf=TRUE;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
const CString CGLEnabledView::GetInformation(InfoField type)
{
	PIXELFORMATDESCRIPTOR pfd;
	CString str("Not Available");
	
	// Get information about the DC's current pixel format 
	::DescribePixelFormat( m_pDC->GetSafeHdc(), ::GetPixelFormat(m_pDC->GetSafeHdc()),sizeof(PIXELFORMATDESCRIPTOR), &pfd ); 
	
	// specify the target DeviceContext of the subsequent OGL calls
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	
	switch(type)
	{
		// Derive driver information
	case ACCELERATION: if( 0==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Fully Accelerated (ICD)"; // fully in hardware (fastest)
		else if (INSTALLABLE_DRIVER_TYPE_MASK==(INSTALLABLE_DRIVER_TYPE_MASK & pfd.dwFlags) ) str="Partially Accelerated (MCD)"; // partially in hardware (pretty fast, maybe..)
		else str="Not Accelerated (Software)";	// software
		break;
		// get the company name responsible for this implementation
	case VENDOR:str=(char*)::glGetString(GL_VENDOR);
		if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
		break;
		// get the renderer name; this is specific of an hardware configuration
	case RENDERER:str=(char*)::glGetString(GL_RENDERER);
		if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
		break;
		// get the version
	case VERSION:str=(char*)::glGetString(GL_VERSION);
		if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
		break;
		// return a space separated list of extensions
	case EXTENSIONS: str=(char*)::glGetString(GL_EXTENSIONS);
		if ( ::glGetError()!=GL_NO_ERROR) str.Format("Not Available");// failed!
		break;
	};
	
	// free the target DeviceContext (window) and return the result
	wglMakeCurrent(NULL,NULL);
	return str;
}