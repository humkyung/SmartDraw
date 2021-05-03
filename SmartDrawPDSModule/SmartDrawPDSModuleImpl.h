#pragma once

#ifdef ADRAW_PDSMODULE_EXPORTS
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllexport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern
#else
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllimport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <util/ado/ADODB.h>

#include <aDraw_DraFile.h>
#include <DgnV7Lib.h>

#include <string>
#include <map>
#include <list>
using namespace std;

class ON_PDSMODULE_EXT_DLL CSmartDrawPDSModuleImpl
{
	typedef enum tagPipingEntityType
	{
		GENERAL		= 0x03,
		TUBING		= 0x05,
		INSTRUMENT	= 0x06,
		SUPPORT		= 0x08
	}PipingEntityType;

	typedef enum tagColumnNameType
	{
		COLUMN_DATA		= 0x01,
		GRID_LINE_DATA	= 0x02
	}ColumnNameType;

	CSmartDrawPDSModuleImpl(const CSmartDrawPDSModuleImpl&);
	CSmartDrawPDSModuleImpl& operator=(const CSmartDrawPDSModuleImpl&);
public:
	CSmartDrawPDSModuleImpl(void);
	virtual ~CSmartDrawPDSModuleImpl(void);
	int GenerateHSR(const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile , const STRING_T& sPrjFolderPath , const STRING_T& sProjectName , const CIsPoint3d&);	/// generate hsr file
	int ExtractEntities(CaDraw_View* pDraView , const CaDraw_Entity::MODEL_TYPE_T& modelType , const STRING_T& rFilePath , const STRING_T& rIniFilePath , const STRING_T& rDatabaseFilePath , const CIsPoint3d&);
	int WriteModelToXml(const STRING_T& sXmlFilePath , const STRING_T& sDgnFilePath , const int& iDiscipline);			/// 2012.05.17 added by humkyung
	int WriteModelToDump(const STRING_T& sDumpFilePath , const STRING_T& sDgnFilePath , const int& iDiscipline = 1);	/// 2014.09.02 added by humkyung
private:
	CaDraw_Entity::MODEL_TYPE_T m_eModelType;
	STRING_T* m_pModelFilePath;
	STRING_T* m_pIniFilePath;
	STRING_T* m_pDatabaseFilePath;
	map<int,list<CDgnElement*>* >* m_pEntMap;
	CaDraw_View* m_pDraView;

	map<int , STRING_T> m_sServerModelFolderMap , m_sLocalModelFolderMap;	/// 2012.03.02 added by humkyung
public:
	int Clear(void);
private:
	int ExtractModelEntities(CADODB* pAdoDB , const CaDraw_Entity::MODEL_TYPE_T& modelType , const STRING_T& rFilePath);	/// extract model entities - 2014.07.29 added by humkyung

	int WriteEquipmentEntitiesToXml(const STRING_T& , map<int,list<CDgnElement*>* >* );		/// write equipment entities to xml file - 2013.11.01 added by humkyung
	int WriteEquipmentModelToXml(const STRING_T& , const STRING_T& );						/// write a equipment model to xml file - 2013.11.01 added by humkyung

	int GetEntityType(long occur[3] , vector<DPoint3d>& pts , CDgnCellHeader* pCell);
	CDgn62Element* GetEquipmentNameEnt(map<int,list<CDgnElement*>* >* _pEntMap , const short& occur);
	long GetEquipmentOccurAndName(short& eType , STRING_T& rName , long& equi_occur , CDgnElement* pEnt);
	int ParsePipeModelEntities(CADODB& adoDB , map<int,list<CDgnElement*>* >* _pEntMap);
	int ParseEquipmentModelEntities(CADODB& adoDB , map<int,list<CDgnElement*>* >* _pEntMap);
	const char* FindColumnTextInGridCircle(CDgnElement* pGridCircle , map<int,list<CDgnElement*>* >* _pEntMap);
	int ParseStructureModelEntities(map<int,list<CDgnElement*>* >* _pEntMap);
	int ParseCableTModelEntities(map<int,list<CDgnElement*>* >* _pEntMap);
	
	int ExtractPipeModelEntities(CADODB& adoDB , const STRING_T& rFilePath);
	int ExtractEquipmentModelEntities(CADODB& adoDB , const STRING_T& rFilePath);
	int ExtractStructureModelEntities(CADODB& adoDB , const STRING_T& rFilePath);
	int ExtractCableTModelEntities(CADODB& adoDB , const STRING_T& rFilePath);
	int ExtractOtherModelEntities(CADODB& adoDB , const STRING_T& rFilePath);

	bool IsStructureType(CDgnElement* pEnt);

	CIsPoint3d m_ptModelOffset;	/// 2014.11.08 added by humkyung
	int m_nPartitionNo;
	double m_uor_per_subunit;
	int ParseEntityAsPlatform(CDgnElement* pEnt , const int& , const int&);
};
