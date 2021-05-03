// HSRMemManager.cpp: implementation of the CHSRMemManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SQLib.h"
#include "HSR.H"
#include "HSRFace.h"
#include "HSRMemManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRMemManager::CHSRMemManager()
{
	m_pPointPool = new boost::pool<>(sizeof(POINT_T));
	m_pVertexPool	= new boost::pool<>(sizeof(HSR_VERTEX));
	m_pFacePool	= new boost::object_pool<CHSRFace>;
	m_pPrimitivePool= new boost::pool<>(sizeof(SQPRIMITIVE));

	/// 2012.01.27 added by humkyung
	m_pLayerPool = new boost::pool<>(sizeof(char)*(LAYER_NAME_LEN + 1));
}

CHSRMemManager::~CHSRMemManager()
{
	try{
		if(NULL != m_pPointPool){
			m_pPointPool->purge_memory();
			delete m_pPointPool;
		}
		
		if(NULL != m_pVertexPool){
			m_pVertexPool->purge_memory();
			delete m_pVertexPool;
		}

		if(NULL != m_pFacePool){
			delete m_pFacePool;
		}

		if(NULL != m_pPrimitivePool){
			m_pPrimitivePool->purge_memory();
			delete m_pPrimitivePool;
		}

		/// 2012.01.27 added by humkyung
		if(NULL != m_pLayerPool)
		{
			m_pLayerPool->purge_memory();
			delete m_pLayerPool;
		}
		/// up to here
	}catch(...){}

#ifdef _DEBUG
	OutputDebugString("Destructure of CHSRMemManager");
#endif
}
