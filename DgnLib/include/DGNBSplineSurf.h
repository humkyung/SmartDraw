#ifndef	__DGNBSPLINESURF_H__
#define	__DGNBSPLINESURF_H__

class CDGNFile;
class CDGNBSplineSurf : public CBSplineSurf{
public:
	long Parse(CDGNFile* pFile);
};

#endif
