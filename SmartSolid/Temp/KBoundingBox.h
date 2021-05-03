#ifndef __KBOUNDINGBOX_H__
#define __KBOUNDINGBOX_H__

#include <SQLib.h>

class KBoundingBox{
public:
	KBoundingBox();
	KBoundingBox(KBoundingBox& boundingbox);
	~KBoundingBox();
public:
	bool IsInnerPoint(SQPOINT& pt);
	bool IsStraddle(KBoundingBox& boundingbox);
	void operator+=(SQPOINT& pt);
	bool operator==(KBoundingBox& boundingbox);
private:
	SQPOINT m_ptMin;
	SQPOINT m_ptMax;
};

#endif
