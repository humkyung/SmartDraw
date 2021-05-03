#include <DGNLib.h>
#include "..\include\DGNFile.h"

CDGNFile::CDGNFile()
{
	fp = NULL;
	m_nOpenMode = 0x00;
}

/**
 * ~CDGNFile:
 *
 * @return  
 */
CDGNFile::~CDGNFile()
{
	try
	{
		if(NULL != fp) fclose(fp);
	}
	catch(...)
	{
	}
}


/**	\brief	The CDGNFile::ReadElement function\n
	read one element which deleted flag is not setted from dgn file.


	\return	bool	
*/
bool CDGNFile::ReadElement()
{
	for(;!feof(fp);)
	{
		// save file pointer for PushBackElement();
		m_nPrevFilePos = ftell(fp);
		
		if(4 != fread(abyElem,1,4,fp)) return false;
		
		// Is this an 0xFFFF endof file marker?
		if((0xff == abyElem[0]) && (0xff == abyElem[1])) return false;
		
		int nWords = abyElem[2] + abyElem[3]*256;
		int nType  = abyElem[1] & 0x7f;
		int nLevel = abyElem[0] & 0x3f;
		
		// Read the rest of the element data into the working buffer.      
		if((unsigned)nWords != fread(abyElem + 4,2,nWords,fp)) return false;
		
		nElemBytes = nWords*2 + 4;
		next_element_id++;
		
		PDGNHDR pDGNHDR = (PDGNHDR)abyElem;
		memcpy(&m_header , pDGNHDR , sizeof(DGNHDR));
		m_header.ElmHdr.xlow =  U_ToLong(m_header.ElmHdr.xlow);
		m_header.ElmHdr.ylow =  U_ToLong(m_header.ElmHdr.ylow);
		m_header.ElmHdr.zlow =  U_ToLong(m_header.ElmHdr.zlow);
		m_header.ElmHdr.xhigh = U_ToLong(m_header.ElmHdr.xhigh);
		m_header.ElmHdr.yhigh = U_ToLong(m_header.ElmHdr.yhigh);
		m_header.ElmHdr.zhigh = U_ToLong(m_header.ElmHdr.zhigh);

		if(pDGNHDR->ElmHdr.deleted) continue;

		return true;
	}
	
	return false;
}

/**
 * Open:
 *
 * @param pFilepath 
 * @param mode 
 * @return bool 
 */
bool CDGNFile::Open(const char *pFilepath, const int mode)
{
	assert(pFilepath && "pFilepath is NULL");
	bool bRet=false;

	if(pFilepath)
	{
		if(CDGNFile::modeRead == mode)
		{
			if(NULL != (fp = fopen(pFilepath,"rb")))
			{
				m_nOpenMode = CDGNFile::modeRead;
				bRet = true;
			}
		}
		else if(CDGNFile::modeWrite == mode)
		{
			if(NULL != (fp = fopen(pFilepath,"wb")))
			{
				m_nOpenMode = CDGNFile::modeWrite;
				bRet = true;
			}
		}
	}
	
	return bRet;
}

/**
 * Close:
 *
 * @return void 
 */
void CDGNFile::Close()
{
	if(NULL != fp)
	{
		static const char mark = (const char)0xff;

		putc(mark,fp);
		putc(mark,fp);
		fflush(fp);
		fclose(fp);
	}
	fp = NULL;
	m_nOpenMode = 0x00;
}

/**
 * WriteElement:
 *
 * @param pElmBuf 
 * @return bool 
 */
bool CDGNFile::WriteElement(unsigned char *pElmBuf)
{
	assert(pElmBuf && "pElmBuf is NULL");
	bool bRet=false;

	if((NULL != pElmBuf) && fp/* && (CDGNFile::modeWrite == m_nOpenMode)*/)
	{
		PDGNHDR pHdr=(PDGNHDR)(pElmBuf);
		int count=fwrite(pElmBuf,2,pHdr->ElmHdr.words + 2,fp);
		memset(pElmBuf,'\0',(pHdr->ElmHdr.words + 2)*2);
		
		bRet = (count == pHdr->ElmHdr.words + 2) ? true : false;
	}

	return bRet;
}

/**	
	@brief	convert unsigned long for little endian.

	@author	HumKyung.BAEK

	@date	2010.05.17

	@return	bool	
*/
unsigned long CDGNFile::U_ToLong(unsigned long t_ulong)
{
	unsigned long  tmp_unsigned;
	
	tmp_unsigned = t_ulong;
	tmp_unsigned <<= 16;
	t_ulong >>= 16;
	t_ulong += tmp_unsigned;
	t_ulong += 2147483648;	// set top most bit (0x800000000)
	
	return t_ulong;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.11.03

	@return	VOLUME	
*/
SQVOLUME CDGNFile::GetVolume()
{
	SQVOLUME volume;

	volume.x[0] = (m_header.ElmHdr.xlow);
	volume.x[1] = (m_header.ElmHdr.xhigh);
	volume.y[0] = (m_header.ElmHdr.ylow);
	volume.y[1] = (m_header.ElmHdr.yhigh);
	volume.z[0] = (m_header.ElmHdr.zlow);
	volume.z[1] = (m_header.ElmHdr.zhigh);

	return volume;
}