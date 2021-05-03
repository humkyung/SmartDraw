#include <graphics/poly.h>
#include <graphics/enum.h>

/**	\brief
*/
CPoly::CPoly(){
	m_nId = ENM_ITK_POLY;
	
	m_bClosed = false;
	m_plst = NULL;
}

/**	\brief
*/
CPoly::~CPoly(){
	if(NULL != m_plst){
		for(list<CPoint3d*>::iterator itr=m_plst->begin();itr != m_plst->end();){
			delete (*itr);
			m_plst->erase(itr++);
		}
		delete m_plst;
	}
}