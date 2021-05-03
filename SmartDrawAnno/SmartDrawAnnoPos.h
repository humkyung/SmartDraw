#ifndef __CaDraw_TagPos_H__
#define __CaDraw_TagPos_H__

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <IsPoint3d.h>
#include "Enum.h"

#include <fstream>
using namespace std;

typedef enum tagDimLevel
{
	DIM_LEVEL_X	= 0x00,
	DIM_LEVEL_0	= 0x01,
	DIM_LEVEL_1	= 0x02,
	DIM_LEVEL_2	= 0x04
}DIM_LEVEL_E;

#define DIM_LEVEL_0_ON	0x01
#define DIM_LEVEL_1_ON	0x02
#define DIM_LEVEL_2_ON	0x04

typedef enum
{
	DOUBLE_TAG=1,
	SINGLE_TAG=2,
	DIM_TAG   =3
}TAGTYPE_E;

typedef enum
{
	GROUP_START = 1,
	GROUP_IN    = 2,
	GROUP_END   = 3
}GROUP_CODE;

class CaDraw_TagItem;
class CDimLevel;
class CExtDivision;
//! this class is for tag or dimension.
namespace tag_pos
{
class ON_TAG_EXT_DLL CaDraw_TagPos
{
public:
	virtual void Write(OFSTREAM_T& file,CExtDivision* pDivision,const double& dDrawingScale){};
	virtual bool OnRegisterToDimension(CDimLevel* pDimLevel);
	CaDraw_TagPos(CaDraw_TagItem* pTagItem = NULL);
	virtual ~CaDraw_TagPos();
public:
	TAGTYPE_E   type;
	CIsPoint3d  m_ptStart,m_ptEnd;
	RECT_T      rect;		//! area occupied by object
	int	    nLevel;		//! 
	char	    nLevelOn;
	double      nPrevSpace,nNextSpace;
	int         nOverlap;
	bool        bFixed;		//! tagpos is fixed?
	int	    nGroup;
	GROUP_CODE  gcode;
	CaDraw_TagItem*    pTITEM;	//! pointer to tag item
					
	CaDraw_TagPos* next;		//! pointer to pipes have same elevation.
};

class ON_TAG_EXT_DLL CaDraw_PipeTagPos : public CaDraw_TagPos
{
public:
	CaDraw_PipeTagPos(CaDraw_TagItem* pTagItem) : CaDraw_TagPos(pTagItem){}
	virtual ~CaDraw_PipeTagPos(){}

	bool OnRegisterToDimension(CDimLevel* pDimLevel);
	void Write(OFSTREAM_T& file,CExtDivision* pDivision,const double& dDrawingScale);
private:
	void WriteSinglePipeTag(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale);
	void WriteElevTag(OFSTREAM_T& file , CExtDivision* pDivision,const double& dDrawingScale);
};

class ON_TAG_EXT_DLL CaDraw_EquiTagPos : public CaDraw_TagPos
{
public:
	CaDraw_EquiTagPos(CaDraw_TagItem* pTagItem) : CaDraw_TagPos(pTagItem){}
	virtual ~CaDraw_EquiTagPos(){}

	void Write(OFSTREAM_T& file,CExtDivision* pDivision,const double& dDrawingScale);
};
};

#endif