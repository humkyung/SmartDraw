#include "StdAfx.h"
#include "PluginManager.h"

CPluginManager::CPluginManager(void)
{
}

CPluginManager::~CPluginManager(void)
{
	try
	{
		for(vector<Plugin*>::const_iterator itr = m_oPluginList.begin();itr != m_oPluginList.end();++itr)
		{
			AfxFreeLibrary((*itr)->hModule);
		}
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-12
    @function   GetInstance
    @return     CPluginManager&
    @param      void
    @brief
******************************************************************************/
CPluginManager& CPluginManager::GetInstance(void)
{
	static CPluginManager __instance__;

	return __instance__;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   GetPluginOf
    @return     Plugin*
    @param      const           STRING_T&
    @param      sDrawingType
    @brief
******************************************************************************/
Plugin* CPluginManager::GetPluginOf(const STRING_T& sDrawingType)
{
	for(vector<Plugin*>::iterator itr = m_oPluginList.begin();itr != m_oPluginList.end();++itr)
	{
		if(sDrawingType == (*itr)->pfGetDrawingName())
		{
			return (*itr);
		}
	}

	return NULL;
}