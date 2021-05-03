#ifndef	__DGNDIMENSION_H__
#define	__DGNDIMENSION_H__

class CDGNFile;
class CDGNDimension : public CDimension{
public:
	long Parse(CDGNFile* pFile);
	long Write(CDGNFile* pFile);
};

#endif
