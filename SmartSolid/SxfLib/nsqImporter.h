#ifndef __KNSQIMPORT_H__
#define __KNSQIMPORT_H__

#include <SQLib.h>

#include <list>
using namespace std;

class CNSQImporter{
public:
	CNSQImporter();
	~CNSQImporter();
public:
	PSQPRIMITIVE OnImportFile(const char* pFilePath);
private:
	list<char*> m_lstLine;
};

#endif