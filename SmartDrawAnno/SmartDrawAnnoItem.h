#pragma once

#include <IsPoint3d.h>
#include <IsVect3d.h>
#include "Enum.h"
#include "SmartDrawAnnoEntity.h"
#include "SmartDrawAnnoPos.h"

#include <list>
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

using namespace tag_pos;

#define DIM_BOTH_SIDE	0x01
#define DIM_ONE_SIDE	0x02

class CExtDivision;
class ON_TAG_EXT_DLL CaDraw_TagItem
{
public:
	virtual void OnRegisterToDivision(list<CExtDivision*>& DivisionList){};
	CaDraw_TagItem();
	virtual ~CaDraw_TagItem();
public:
	static CaDraw_TagItem* CreateInstance();	/// 2012.03.11 added by humkyung

	void OutputDebugString();
	int SetTypeString(const STRING_T& rTypeString);	/// 2012.02.20 added by humkyung
	const TCHAR* GetTypeString() const;
	virtual int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);

	DITEM_E	       enmType;
	SUBITEM_TYPE_E enmSubType;
	CIsPoint3d m_ptOrigin;		///< tagÇÒ ¸ðµ¨ ÁÂÇ¥¿¡ ´ëÀÀÇÏ´Â ºä ÁÂÇ¥
	CIsPoint3d ptModelOrigin;	///< tagÇÒ ¸ðµ¨ ÁÂÇ¥
	CIsPoint3d ptMin,ptMax;
	double  nBOP,nCL,nTOP;
	CIsVect3d vecDir;
	int nLevel;
	int m_nDimStyle;
	STRING_T* name[2];
	
	CaDraw_TagEntity* m_pEnt;
protected:
	list<CaDraw_TagPos*>* m_pTagPosList;
private:
	TCHAR m_szTypeString[64];
};

class ON_TAG_EXT_DLL CaDraw_PipeTagItem : public CaDraw_TagItem
{
public:
	CaDraw_PipeTagItem();
	virtual ~CaDraw_PipeTagItem();

	void OnRegisterToDivision(list<CExtDivision*>& DivisionList);
	int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);
};

class ON_TAG_EXT_DLL CaDraw_EquiTagItem : public CaDraw_TagItem
{
public:
	CaDraw_EquiTagItem();
	virtual ~CaDraw_EquiTagItem();

	void OnRegisterToDivision(list<CExtDivision*>& DivisionList);
	int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);

	bool m_bWirteCenterline;
};

class ON_TAG_EXT_DLL CaDraw_StruTagItem : public CaDraw_TagItem
{
public:
	CaDraw_StruTagItem();
	virtual ~CaDraw_StruTagItem();

	void OnRegisterToDivision(list<CExtDivision*>& DivisionList);
	int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);
};

class ON_TAG_EXT_DLL CaDraw_TrayTagItem : public CaDraw_TagItem
{
public:
	CaDraw_TrayTagItem();
	virtual ~CaDraw_TrayTagItem();

	void OnRegisterToDivision(list<CExtDivision*>& DivisionList);
	int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);
};

class ON_TAG_EXT_DLL CaDraw_DimTagItem : public CaDraw_TagItem
{
public:
	typedef enum
	{
		DIM_NEAR_SIDE	= 0,
		DIM_LEFT_SIDE	= 1,
		DIM_RIGHT_SIDE	= 2,
		DIM_TOP_SIDE	= 4,
		DIM_BOTTOM_SIDE	= 8
	};

	CaDraw_DimTagItem();
	virtual ~CaDraw_DimTagItem();

	static CaDraw_DimTagItem* CreateInstance();

	void OnRegisterToDivision(list<CExtDivision*>& oDivisionList);
	int OnCreateTagPos(vector<CaDraw_TagPos*>& oTagPosList , CExtDivision *pDivision);
public:
	int m_iTagDir;
};