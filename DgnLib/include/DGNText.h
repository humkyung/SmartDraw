#ifndef	__DGNTEXT_H__
#define	__DGNTEXT_H__

#include <DGNElement.h>

enum{
	JUST_LEFT = 1,
	JUST_RIGHT= 2
};

class DLL_EXPORT CDGNText : public CDGNElement
{
public:
	CDGNText();
	virtual ~CDGNText();
public:
	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);

	static long Parse(PSQPRIMITIVE* ppPrimitive,CDGNFile* hDGN);
private:
	long Write2D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
};

#endif
