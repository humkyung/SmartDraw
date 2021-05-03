#ifndef __FORMATFILE_H__
#define __FORMATFILE_H__

#include <stdio.h>
#include "..\SQFormat.h"

class KFormatFile{
public:
	KFormatFile();
	virtual ~KFormatFile();
public:
	virtual bool Open(const char* szFileName) = 0;
//	size_t FWrite( const void *buffer,size_t size,size_t count);
	void Close();
public:
	FILE* m_fp;	// it's position is not good.
};

//	description	:
//	remarks		:
class KWriteFormatFile : public KFormatFile{
public:
	KWriteFormatFile();
	KWriteFormatFile(const char* szFileName);
	~KWriteFormatFile();
public:
	bool Open(const char* szFileName);
	void WritePrimitive(PSQPRIMITIVE pPrimitive);
	
	void SetLinkageDataFunc(void (*pfWriteLinkageData)(),long (*pfGetLinkageDataSize)(PSQPRIMITIVE pPrimitive));
private:
	void SetSizeOfPrimitive(PSQPRIMITIVE pPrimitive);
	unsigned long CalcSizeofContent(PSQPRIMITIVE pPrimitive);
	void WritePrimitiveHeader(PSQPRIMITIVE pPrimitive);
	void WriteContent(PSQPRIMITIVE pPrimitive);
private:
	void (*pfWriteLinkageData)();
	long (*pfGetLinkageDataSize)(PSQPRIMITIVE pPrimitive);

	long m_nOffset;
};

//	description	:
//	remarks		:
class KLoadFormatFile : public KFormatFile{
public:
	KLoadFormatFile();
	KLoadFormatFile(const char* szFileName);
	~KLoadFormatFile();
public:
	bool Open(const char* szFileName);
	PSQPRIMITIVE LoadPrimitive();
	
	void SetLinkageDataFunc(void (*pfLoadLinkageData)());
private:
	void LoadHeader(PSQPRIMITIVE pPrimitive);
	void LoadContent(PSQPRIMITIVE pPrimitive);
private:
	long m_nLength;
};

extern const char* format(const char* fmt,...);

#endif
