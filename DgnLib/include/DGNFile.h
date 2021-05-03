#ifndef	__DGNFILE_H__
#define	__DGNFILE_H__

#ifndef	__DLL_EXPORT__
#define	__DLL_EXPORT__ __declspec(dllexport)
#endif

#include <stdio.h>
#include <SQLib.h>
class __DLL_EXPORT__ CDGNFile{
public:
	bool ReadElement();
	
	enum{
		modeRead = 0x01,
		modeWrite= 0x02
	};
public:
	SQVOLUME GetVolume();
	unsigned long U_ToLong(unsigned long t_ulong);
	bool WriteElement(unsigned char* pElmBuf);
	void Close();
	bool Open(const char* pFilepath,const int mode);
	CDGNFile();
	virtual ~CDGNFile();
	
	FILE* fp;

	bool bComponentOfComplexElem;
	int nElemBytes;
	unsigned char abyElem[65540];

	int got_tcb;
	int got_bounds;
	int dimension;
	double scale;
	POINT_T m_ptOrigin;
	unsigned short remainder; // for cell data

	int index_built;
	int element_count;

	int got_color_table;
	unsigned char color_table[256][3];

	int next_element_id;
private:
	DGNHDR m_header;
	int m_nOpenMode;
	long m_nPrevFilePos;
};

#endif