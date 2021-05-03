#include <graphics/enum.h>
#include <graphics/rect.h>

CRect2d::CRect2d(){
	m_nId = ENM_ITK_RECT;

	x = y = width = height = 0;
}

CRect2d::~CRect2d(){}

/**	\brief	The CRect2d::IsInnerPoint function

	\param	pt	a parameter of type const POINT_T&

	\return	bool	
*/
bool CRect2d::IsInnerPoint(const POINT_T& pt){
	return ((pt.x >= x) && (pt.x <= x + width) && (pt.y >= y) && (pt.y <= y + height));
}

/**	\brief	The CRect2d::IsOverlapped function

	\param	rect	a parameter of type const CRect2d&

	\return	bool	
*/
bool CRect2d::IsOverlapped(const CRect2d& rect){
	if((x > (rect.x + rect.width)) || ((x + width) < rect.x) || (y > (rect.y + rect.height)) || ((y + height) < rect.y))
		return false;

	return true;
		
}