// HSRMemManager.h: interface for the CHSRMemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRMEMMANAGER_H__A6E0E00A_5CB2_4B64_BC6F_CB9AE1C2CF18__INCLUDED_)
#define AFX_HSRMEMMANAGER_H__A6E0E00A_5CB2_4B64_BC6F_CB9AE1C2CF18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/pool/object_pool.hpp>

class CHSRMemManager  
{
public:
	CHSRMemManager();
	virtual ~CHSRMemManager();
public:
	boost::pool<>* m_pPointPool;
	boost::pool<>* m_pVertexPool;
	boost::object_pool<CHSRFace>* m_pFacePool;
	boost::pool<>* m_pPrimitivePool;

	boost::pool<>* m_pLayerPool;	/// 2012.01.27 added by humkyung
};

#endif // !defined(AFX_HSRMEMMANAGER_H__A6E0E00A_5CB2_4B64_BC6F_CB9AE1C2CF18__INCLUDED_)
