#include "KBoundingBox.h"

KBoundingBox::KBoundingBox(){
	m_ptMin.x = 0.;
	m_ptMin.y = 0.;
	m_ptMin.z = 0.;
	m_ptMax.x = 0.;
	m_ptMax.y = 0.;
	m_ptMax.z = 0.;
}

KBoundingBox::KBoundingBox(KBoundingBox& boundingbox){
    	m_ptMin.x = boundingbox.m_ptMin.x;
	m_ptMin.y = boundingbox.m_ptMin.y;
	m_ptMin.z = boundingbox.m_ptMin.z;
	m_ptMax.x = boundingbox.m_ptMax.x;
	m_ptMax.y = boundingbox.m_ptMax.y;
	m_ptMax.z = boundingbox.m_ptMax.z;
}

KBoundingBox::~KBoundingBox(){
}

bool KBoundingBox::IsInnerPoint(SQPOINT& pt){
	bool bRet=false;

	if((pt.x >= m_ptMin.x && pt.x <= m_ptMax.x) && 
		(pt.y >= m_ptMin.y && pt.y <= m_ptMax.y) && 
		(pt.z >= m_ptMin.z && pt.z <= m_ptMax.z)){
		bRet=true;
	}

	return bRet;
}

void KBoundingBox::operator +=(SQPOINT& pt){
	if((0 == m_ptMin.x) && (0 == m_ptMin.y) && (0 == m_ptMin.z) &&
		(0 == m_ptMax.x) && (0 == m_ptMax.y) && (0 == m_ptMax.z)){

		m_ptMin.x = m_ptMax.x = pt.x;
		m_ptMin.y = m_ptMax.y = pt.y;
		m_ptMin.z = m_ptMax.z = pt.z;
	}else{
		m_ptMin.x = (pt.x < m_ptMin.x) ? pt.x : m_ptMin.x;
		m_ptMin.y = (pt.y < m_ptMin.y) ? pt.y : m_ptMin.y;
		m_ptMin.z = (pt.z < m_ptMin.z) ? pt.z : m_ptMin.z;
		m_ptMax.x = (pt.x > m_ptMax.x) ? pt.x : m_ptMax.x;
		m_ptMax.y = (pt.y > m_ptMax.y) ? pt.y : m_ptMax.y;
		m_ptMax.z = (pt.z > m_ptMax.z) ? pt.z : m_ptMax.z;
	}
}

bool KBoundingBox::IsStraddle(KBoundingBox& boundingbox){
    	if((boundingbox.m_ptMax.x < m_ptMin.x) || (boundingbox.m_ptMin.x > m_ptMax.x) || 
		(boundingbox.m_ptMax.y < m_ptMin.y) || (boundingbox.m_ptMin.y > m_ptMax.y) || 
		(boundingbox.m_ptMax.z < m_ptMin.z) || (boundingbox.m_ptMin.z > m_ptMax.z)){
	    	return false;
	}

	return true;
}

bool KBoundingBox::operator==(KBoundingBox& boundingbox){
    	bool bRet=false;

    	if((m_ptMin.x == boundingbox.m_ptMin.x) && (m_ptMin.y == boundingbox.m_ptMin.y) && (m_ptMin.z = boundingbox.m_ptMin.z) &&
		(m_ptMax.x == boundingbox.m_ptMax.x) && (m_ptMax.y == boundingbox.m_ptMax.y) && (m_ptMax.z == boundingbox.m_ptMax.z)){
	    	bRet=true;
	}

	return bRet;
}
