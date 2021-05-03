// HSRElementFactory.cpp: implementation of the CHSRElementFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRElementFactory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRElementFactory::CHSRElementFactory()
{
	m_pElementMap = new map<STRING_T , queue<CHSRElement*>* >;
}

CHSRElementFactory::~CHSRElementFactory()
{
	try
	{
		for(map<STRING_T , queue<CHSRElement*>* >::iterator itr = m_pElementMap->begin();itr != m_pElementMap->end();++itr)
		{
			while(!itr->second->empty())
			{
				delete (itr->second->front());
				itr->second->pop();
			}
			delete (itr->second);
		}

		delete m_pElementMap;
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-04
    @function   CreateElement
    @return     CHSRElement*
    @param      const       string&
    @param      sTypeStr
    @brief
******************************************************************************/
CHSRElement* CHSRElementFactory::CreateElement(const STRING_T& sTypeStr)
{
	CHSRElement* res = NULL;
	map<STRING_T , queue<CHSRElement*>* >::iterator where = m_pElementMap->find(sTypeStr);
	if((where != m_pElementMap->end()) && !where->second->empty())
	{
		res = where->second->front();
		where->second->pop();
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @function   ReturnElement
    @return     CHSRElement*
    @param      CHSRElement*    pElm
    @brief
******************************************************************************/
int CHSRElementFactory::ReturnElement(CHSRElement* pElm)
{
	assert(pElm && "pElm is NULL");

	if(pElm)
	{
		//SAFE_DELETE(pElm);
		const STRING_T sTypeStr = pElm->GetTypeString();
		map<STRING_T , queue<CHSRElement*>* >::iterator where = m_pElementMap->find(sTypeStr);
		if(where == m_pElementMap->end())
		{
			m_pElementMap->insert(make_pair(sTypeStr , new queue<CHSRElement*>));
		}

		(*m_pElementMap)[sTypeStr]->push(pElm);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-04
    @function   GetInstance
    @return     CHSRElementFactory&
    @brief
******************************************************************************/
CHSRElementFactory& CHSRElementFactory::GetInstance()
{
	static CHSRElementFactory __instance__;

	return __instance__;
}