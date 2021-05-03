/********************************************************************
	created:	2002/06/03
	created:	3:6:2002   14:54
	filename: 	D:\Project\OpenSolid\DGNLib\DGNScanner.cpp
	file path:	D:\Project\OpenSolid\DGNLib
	file base:	DGNScanner
	file ext:	cpp
	author:		
	
	purpose:	read a raw element from dng file.
*********************************************************************/
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DGNScanner.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDGNScanner::CDGNScanner()
{
	m_nFilePos = 0;
}

CDGNScanner::~CDGNScanner()
{
}

/**	
	@brief	Read the first four bytes to get the level, type, and word,count.

	@author	HumKyung.BAEK

	@param	hDGN	a parameter of type CDGNFile*

	@return	bool	
*/
bool CDGNScanner::ReadElement(CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");

	if(hDGN && !feof(hDGN->fp))
	{
		// save file pointer for PushBackElement();
		m_nFilePos = ftell(hDGN->fp);

		int nType=0, nWords=0, nLevel=0;
		if(4 != fread(hDGN->abyElem,1,4,hDGN->fp)) return false;
		
		// Is this an 0xFFFF endof file marker?
		if( hDGN->abyElem[0] == 0xff && hDGN->abyElem[1] == 0xff ) return false;
		
		nWords = hDGN->abyElem[2] + hDGN->abyElem[3]*256;
		nType  = hDGN->abyElem[1] & 0x7f;
		nLevel = hDGN->abyElem[0] & 0x3f;
		
		// Read the rest of the element data into the working buffer.      
		if((unsigned)nWords != fread(hDGN->abyElem + 4,2,nWords,hDGN->fp)) return false;
		
		hDGN->nElemBytes = nWords*2 + 4;
		hDGN->next_element_id++;
		
		return true;
	}

	return false;
}

/**	\brief	The CDGNScanner::PushBackElement function

	\param	hDGN	a parameter of type CDGNFile*

	\return	void	
*/
void CDGNScanner::PushBackElement(CDGNFile* hDGN)
{
	assert(hDGN && "hDGN is NULL");

	if(hDGN)
	{
		fseek(hDGN->fp,m_nFilePos,SEEK_SET);
	}
}