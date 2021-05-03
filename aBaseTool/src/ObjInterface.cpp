#include "ObjInterface.h"

CObjInterface::~CObjInterface(){}
/**	\brief	The CObjInterface::IsKindOf function

	\param	nId	a parameter of type const long

	\return	bool	
*/
bool CObjInterface::IsKindOf(const long nId)
{
	return (m_nId == (unsigned long)nId) ? true : false;
}
