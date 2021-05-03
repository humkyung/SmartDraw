#include "StdAfx.h"
#include <assert.h>
#include "SmartDrawPDSModuleImpl.h"
#include <IsTools.h>
#include <Tokenizer.h>
#include <IsString.h>
#include <util/SplitPath.h>
#include <util/Markup/Markup.h>

#include <DgnV7Lib.h>

#include <aDraw_Pipe.h>
#include <aDraw_Equipment.h>
#include <aDraw_Structure.h>
#include <aDraw_Nozzle.h>
#include <aDraw_CableTray.h>
#include <aDraw_Platform.h>

#include <SmartHSRImp.h>	/// hsr header file

#include "aDraw_PipeParser.h"
#include "aDraw_GeneralPipingParser.h"
#include "aDraw_InstrumentParser.h"
#include "aDraw_NozzleAttribute.h"
#include "aDraw_CableTrayParser.h"
#include "aDraw_SupportParser.h"
#include "BackgroundFile.h"

#include "../SmartSolid/SmartExchanger/SmartExchanger.h"

#include <fstream>
using namespace std;

CSmartDrawPDSModuleImpl::CSmartDrawPDSModuleImpl(void) : m_nPartitionNo(-1)
{
	m_pModelFilePath = new STRING_T;
	m_pIniFilePath = new STRING_T;
	m_pDatabaseFilePath = new STRING_T;
	m_pEntMap = new map<int,list<CDgnElement*>* >;
	m_uor_per_subunit = 80.f;	/// default value is 80

	m_ptModelOffset.Set(0,0,0);
}

CSmartDrawPDSModuleImpl::CSmartDrawPDSModuleImpl(const CSmartDrawPDSModuleImpl&){}
CSmartDrawPDSModuleImpl& CSmartDrawPDSModuleImpl::operator=(const CSmartDrawPDSModuleImpl&){return (*this);}

CSmartDrawPDSModuleImpl::~CSmartDrawPDSModuleImpl(void)
{
	try
	{
		if(m_pModelFilePath) SAFE_DELETE(m_pModelFilePath);
		if(m_pIniFilePath) SAFE_DELETE(m_pIniFilePath);
		if(m_pDatabaseFilePath) SAFE_DELETE(m_pDatabaseFilePath);
		if(m_pEntMap)
		{
			for(map<int,list<CDgnElement*>* >::iterator itr = m_pEntMap->begin();itr != m_pEntMap->end();++itr)
			{
				SAFE_DELETE(itr->second);
			}
			SAFE_DELETE(m_pEntMap);
		}
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      CSmartDrawPDSModuleImpl
    @function   GenerateHSR
    @return     int
    @param      const STRING_T& sHsrFilePath
	@param		CaDraw_DraFile* pDraFile
	@param		const STRING_T& sIniFilePath
    @param      const STRING_T& sPrjFilePath
    @brief
******************************************************************************/
int CSmartDrawPDSModuleImpl::GenerateHSR(const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile , const STRING_T& sPrjFolderPath ,
										 const STRING_T& sProjectName , const CIsPoint3d& ptModelOffset)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};

		const STRING_T sPrjFilePath = sPrjFolderPath + _T("\\Setting\\") + sProjectName + _T(".prj");
		const STRING_T sIniFilePath = sPrjFolderPath + _T("\\Setting\\") + pDraFile->GetSettingFileName() + _T(".ini");

		m_ptModelOffset = ptModelOffset;

		/// read folder setting
		m_sServerModelFolderMap.clear();
		m_sLocalModelFolderMap.clear();
		if(GetPrivateProfileString(_T("Folder") , _T("Drawing") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::DRAWING_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Pipe") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::PIPE_MODEL_T , szBuf));
			m_sLocalModelFolderMap.insert(make_pair(CaDraw_Entity::PIPE_MODEL_T , sPrjFolderPath + _T("\\Model\\Pipe")));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Equipment") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::EQUIPMENT_MODEL_T , szBuf));
			m_sLocalModelFolderMap.insert(make_pair(CaDraw_Entity::EQUIPMENT_MODEL_T , sPrjFolderPath + _T("\\Model\\Equipment")));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Structure") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::STRUCTURE_MODEL_T , szBuf));
			m_sLocalModelFolderMap.insert(make_pair(CaDraw_Entity::STRUCTURE_MODEL_T , sPrjFolderPath + _T("\\Model\\Structure")));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("CableT") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::CABLE_T_MODEL_T , szBuf));
			m_sLocalModelFolderMap.insert(make_pair(CaDraw_Entity::CABLE_T_MODEL_T , sPrjFolderPath + _T("\\Model\\CableT")));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Misc") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			m_sServerModelFolderMap.insert(make_pair(CaDraw_Entity::MISC_MODEL_T , szBuf));
			m_sLocalModelFolderMap.insert(make_pair(CaDraw_Entity::MISC_MODEL_T , sPrjFolderPath + _T("\\Model\\Misc")));
		}
		/// up to here

		STRING_T sHsrType(_T("Import"));	/// convert pds hidden file to hsr file
		if(GetPrivateProfileString(_T("HLR_OPTIONS") , _T("Type") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			sHsrType = szBuf;
		}

		if(_T("Import") == sHsrType)
		{
			const UINT iHiddenLevel = GetPrivateProfileInt(_T("HLR_OPTIONS") , _T("Import hidden level") , 62 , sIniFilePath.c_str());

			const STRING_T sFileNameWithoutExt = pDraFile->GetFileNameWithoutExt();
			STRING_T sDrawingModelFolderPath;
			map<int , STRING_T>::iterator where = m_sServerModelFolderMap.find(CaDraw_Entity::DRAWING_MODEL_T);
			if(where != m_sServerModelFolderMap.end())
			{
				sDrawingModelFolderPath = where->second;
			}
			
			//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(_T("Import background file...")));
			CBackgroundFile oBackgroundFile;
			if(ERROR_SUCCESS != oBackgroundFile.Generate(sHsrFilePath.c_str() , STRING_T(sDrawingModelFolderPath + _T("\\") + sFileNameWithoutExt + _T(".dgn")).c_str() , iHiddenLevel))
			{
				/// show messge that there is no backdrawing file!!!
			}
		}
		else if(_T("Generate") == sHsrType)
		{
			list<CaDraw_View*> oViewList;
			pDraFile->GetViewList(&oViewList);
			for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
			{
				list<CaDraw_View::MODEL_T>* pModelList = (*itr)->GetModelList();
				for(list<CaDraw_View::MODEL_T>::iterator jtr = pModelList->begin();jtr != pModelList->end();++jtr)
				{
					STRING_T sModelFilePath = m_sServerModelFolderMap[jtr->modelType] + _T("\\") + jtr->rModelName;
					STRING_T sSxfFilePath = m_sLocalModelFolderMap[jtr->modelType] + _T("\\") + jtr->rModelName;
					STRING_T::size_type at = sSxfFilePath.rfind(_T("."));
					if(STRING_T::npos != at)
					{
						sSxfFilePath = sSxfFilePath.substr(0 , at) + _T(".sxf");
					}

					CString str;
					str.Format(_T("Convert dgn(%s) to shx file(%s)...") , sModelFilePath.c_str() , sSxfFilePath.c_str());
					//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(str.operator LPCTSTR()));
					/// convert .dgn file to .sxf file
					Dgn2Sxf(sSxfFilePath.c_str() , jtr->modelType , sModelFilePath.c_str() , sIniFilePath.c_str() , m_ptModelOffset.x() , m_ptModelOffset.y() , m_ptModelOffset.z());
				}
			}

			/// generate .hsr file
			CString str;
			str.Format(_T("Generate hsr file(%s)") , sHsrFilePath.c_str());
			//AfxGetApp()->GetMainWnd()->SendMessage(SD_LOG , WPARAM(str.operator LPCTSTR()));
			SmartHSR(sHsrFilePath.c_str() , pDraFile->GetFilePath().c_str() , sIniFilePath.c_str() , STRING_T(sPrjFolderPath + _T("\\Model")).c_str());
		}
	
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**	
	@brief	extract model entities
	@author	humkyung
	@date	2014.07.29
	
	@param	pAdoDB
	@param	modelType
	@param	rFilePath

	@return	int
*/
int CSmartDrawPDSModuleImpl::ExtractModelEntities(CADODB* pAdoDB , const CaDraw_Entity::MODEL_TYPE_T& modelType , const STRING_T& rFilePath)
{
	m_eModelType = modelType;
	switch(modelType)
	{
	case CaDraw_Entity::PIPE_MODEL_T:
		{
			this->ExtractPipeModelEntities(*pAdoDB , rFilePath);
		}
		break;
	case CaDraw_Entity::EQUIPMENT_MODEL_T:
		{
			this->ExtractEquipmentModelEntities(*pAdoDB , rFilePath);
		}
		break;
	case CaDraw_Entity::STRUCTURE_MODEL_T:
		{
			this->ExtractStructureModelEntities(*pAdoDB , rFilePath);
		}
		break;
	case CaDraw_Entity::CABLE_T_MODEL_T:
		{
			this->ExtractCableTModelEntities(*pAdoDB , rFilePath);
		}
		break;
	case CaDraw_Entity::MISC_MODEL_T:
		{
			this->ExtractOtherModelEntities(*pAdoDB , rFilePath);
		}
		break;
	default:
		throw exception("unexpected model type");
		break;
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	extract entities and put the entities to dra file.

	@author	humkyung

	@date	2010.05.18

	@param	pDraFile
	@param	rModelFilePath
	@param	rDatabaseFilePath(Database Type is DRV File if rDatabaseFilePath is empty)

	@return
*/
int CSmartDrawPDSModuleImpl::ExtractEntities(CaDraw_View* pDraView , const CaDraw_Entity::MODEL_TYPE_T& modelType , const STRING_T& rFilePath , const STRING_T& rIniFilePath , const STRING_T& rDatabaseFilePath , const CIsPoint3d& ptModelOffset)
{
	assert(pDraView && "pDraView is NULL");

	if(pDraView)
	{
		m_ptModelOffset = ptModelOffset;

		m_pDraView = pDraView;
		(*m_pModelFilePath) = rFilePath;
		(*m_pIniFilePath) = rIniFilePath;
		(*m_pDatabaseFilePath) = rDatabaseFilePath;
		
		if(!m_pDatabaseFilePath->empty())
		{
			CADODB adoDB;
			const CString rConnectionString = CString(PROVIDER) + CString(_T("Data Source=")) + m_pDatabaseFilePath->c_str() + DB_PASSWORD;
			if(TRUE == adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
			{
				CString rFileSpec(m_pModelFilePath->c_str());
				const int at = rFileSpec.ReverseFind('\\');
				rFileSpec = rFileSpec.Right(rFileSpec.GetLength() - at - 1);
				CString rSql;
				rSql.Format(_T("SELECT partition_no FROM PDTABLE_113 WHERE model_file_spec='%s'") , rFileSpec);
				adoDB.OpenQuery(rSql.operator LPCTSTR());
				LONG lRecordCount = 0L;
				adoDB.GetRecordCount(&lRecordCount);

				if(1 == lRecordCount)
				{
					STRING_T rPartitionNo;
					adoDB.GetFieldValue(0 , _T("partition_no") , &rPartitionNo);
					m_nPartitionNo = ATOI_T(rPartitionNo.c_str());
					
					try
					{
						ExtractModelEntities(&adoDB , modelType , rFilePath);
					}
					catch(...)
					{
						throw;
					}
					
				}else	throw exception("can't get partition number.");
			}else	throw exception("can't open database.");
		}
		else	/// Database type is 'DRV File'
		{
			m_nPartitionNo = -1;
			ExtractModelEntities(NULL , modelType , rFilePath);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		Write model file to xml file
	@author     humkyung
    @date       2012-05-17
    @class      CSmartDrawPDSModuleImpl
    @function   WriteModelToXml
    @return     int
    @param      const       STRING_T&
    @param      rFilePath
    @param		iDiscipline	discipline number
******************************************************************************/
int CSmartDrawPDSModuleImpl::WriteModelToXml(const STRING_T& sXmlFilePath , const STRING_T& sDgnFilePath , const int& iDiscipline)
{
	if(1 == iDiscipline)	/// piping
	{
		map<int,list<CDgnElement*>* > _EntMap;

		CDgnFile dgnFile;
		CDgnParser parser(NULL);
		parser.AddParsingType(LINE_STRING_ELM);
		parser.AddParsingType(CELL_HEADER_ELM);
		parser.AddParsingType(DGNT_CONE);
		if(true == dgnFile.Open(sDgnFilePath.c_str() , CDgnFile::modeRead))
		{
			for(;dgnFile.ReadElement();)
			{
				const int nType = dgnFile.GetType();
				if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
				{
					CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
					m_uor_per_subunit = pTCB->uor_per_subunit;
				}
				else if((CELL_HEADER_ELM == nType) || (LINE_STRING_ELM == nType) || (DGNT_CONE == nType))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}

			CMarkup xml;
			xml.SetDoc( _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n") );
			xml.AddElem( _T("SmartDraw") );

			map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(CELL_HEADER_ELM);
			if(where != _EntMap.end())
			{
				for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
				{
					CDgnCellHeader* pCell = (CDgnCellHeader*)(*itr);
					long occur[3] = {-1,-1,-1};
					vector<DPoint3d> oPointList;
					const int nEntityType = (GetEntityType(occur , oPointList , pCell));

					xml.AddChildElem( _T("Item") );
					xml.IntoElem();
					xml.AddAttrib(_T("DgnType") , _T("CellHeader"));

					switch(nEntityType)
					{
					case GENERAL:
						{
							xml.AddAttrib(_T("Discipline") , _T("34"));
						}
						break;
					case TUBING:
						{
							xml.AddAttrib(_T("Discipline") , _T("50"));
						}
						break;
					case INSTRUMENT:
						{
							xml.AddAttrib(_T("Discipline") , _T("67"));
						}
						break;
					case SUPPORT:
						{
							xml.AddAttrib(_T("Discipline") , _T("80"));
						}
						break;
					default:
						{
							OSTRINGSTREAM_T oss;
							oss << nEntityType;
							xml.AddAttrib(_T("Discipline") , oss.str().c_str());
						}
						break;
					}
					OSTRINGSTREAM_T oss;
					oss << occur[0];
					xml.AddAttrib(_T("occur1") , oss.str().c_str());
					oss.str(_T(""));
					oss << occur[1];
					xml.AddAttrib(_T("occur2") , oss.str().c_str());
					oss.str(_T(""));
					oss << occur[1];
					xml.AddAttrib(_T("occur3") , oss.str().c_str());

					oss.str(_T(""));
					for(vector<DPoint3d>::iterator jtr = oPointList.begin();jtr != oPointList.end();++jtr)
					{
						oss << std::endl << (jtr->x/m_uor_per_subunit) << _T(",") << (jtr->y/m_uor_per_subunit) << _T(",") << (jtr->z/m_uor_per_subunit);
					}
					oss << std::endl;
					xml.SetData(oss.str());

					xml.OutOfElem();
				}
			}

			where = _EntMap.find(DGNT_CONE);
			if(where != _EntMap.end())
			{
				for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
				{
					CDgnCone* pEnt = (CDgnCone*)(*itr);

					xml.AddChildElem( _T("Item") );
					xml.IntoElem();

					xml.AddAttrib(_T("DgnType") , _T("Cone"));
					xml.AddAttrib(_T("Discipline") , _T("50"));

					int iCount = 1;
					PDGNATTRDATA pAttr = pEnt->attribute();
					for(PDGNATTRDATA ptr = pAttr;ptr;ptr = ptr->next)
					{
						if(!(ptr->hdr.u))
						{
							const long occur = ptr->data.occur;
							OSTRINGSTREAM_T oss[2];
							oss[0] << _T("occur") << iCount++;
							oss[1] << occur;
							xml.AddAttrib(oss[0].str() , oss[1].str());
						}
					}

					OSTRINGSTREAM_T oss;
					oss << std::endl << (pEnt->bottom().x/m_uor_per_subunit) << _T(",") << (pEnt->bottom().y/m_uor_per_subunit) << _T(",") << (pEnt->bottom().z/m_uor_per_subunit);
					oss << (pEnt->top().x/m_uor_per_subunit) << _T(",") << (pEnt->top().y/m_uor_per_subunit) << _T(",") << (pEnt->top().z/m_uor_per_subunit);
					oss << std::endl;
					xml.SetData( oss.str() );

					xml.OutOfElem();
				}
			}

			xml.Save(sXmlFilePath.c_str());
		}
	}
	else if(2 == iDiscipline)	/// Equipment
	{
		WriteEquipmentModelToXml(sXmlFilePath , sDgnFilePath);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		Write model file to dump file
	@author     humkyung
    @date       2014-09-02
    @class      CSmartDrawPDSModuleImpl
    @function   WriteModelToDump
    @return     int
    @param      const       STRING_T&
    @param      rFilePath
    @param		iDiscipline	discipline number
******************************************************************************/
int CSmartDrawPDSModuleImpl::WriteModelToDump(const STRING_T& sDumpFilePath , const STRING_T& sDgnFilePath , const int& iDiscipline)
{
	if(1 == iDiscipline)	/// piping
	{
		map<int,list<CDgnElement*>* > _EntMap;

		CDgnFile dgnFile;
		CDgnParser parser(NULL);
		parser.AddParsingType(LINE_STRING_ELM);
		parser.AddParsingType(CELL_HEADER_ELM);
		parser.AddParsingType(DGNT_CONE);
		if(true == dgnFile.Open(sDgnFilePath.c_str() , CDgnFile::modeRead))
		{
			for(;dgnFile.ReadElement();)
			{
				const int nType = dgnFile.GetType();
				if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
				{
					CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
					m_uor_per_subunit = pTCB->uor_per_subunit;
				}
				else if((CELL_HEADER_ELM == nType) || (LINE_STRING_ELM == nType) || (DGNT_CONE == nType))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}

			OFSTREAM_T ofile(sDumpFilePath.c_str());
			if(ofile.is_open())
			{
				ofile.precision(5);
				ofile.setf(ios_base:: fixed, ios_base:: floatfield);	/// 소수점 방식으로 표시
				map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(CELL_HEADER_ELM);
				if(where != _EntMap.end())
				{
					for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
					{
						CDgnCellHeader* pCell = (CDgnCellHeader*)(*itr);
						long occur[3] = {-1,-1,-1};
						vector<DPoint3d> oPointList;
						const int nEntityType = (GetEntityType(occur , oPointList , pCell));
						switch(nEntityType)
						{
						case GENERAL:
							{
								ofile << _T("34,");
							}
							break;
						case TUBING:
							{
								ofile << _T("50,");
							}
							break;
						case INSTRUMENT:
							{
								ofile << _T("67,");
							}
							break;
						case SUPPORT:
							{
								ofile << _T("80,");
							}
							break;
						default:
							{
								ofile << nEntityType << _T(",");
							}
							break;
						}
						ofile << occur[0] << _T(",");
						ofile << occur[1] << _T(",");
						ofile << occur[2] << _T(",");
						for(vector<DPoint3d>::iterator jtr = oPointList.begin();jtr != oPointList.end();++jtr)
						{
							ofile << (jtr->x/m_uor_per_subunit) << _T(",") << (jtr->y/m_uor_per_subunit) << _T(",") << (jtr->z/m_uor_per_subunit);
							if((jtr + 1) != oPointList.end()) ofile << _T(",");
						}
						ofile << std::endl;
					}
				}

				where = _EntMap.find(DGNT_CONE);
				if(where != _EntMap.end())
				{
					for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
					{
						CDgnCone* pEnt = (CDgnCone*)(*itr);

						ofile << _T("50,");
						
						int iCount = 1;
						PDGNATTRDATA pAttr = pEnt->attribute();
						for(PDGNATTRDATA ptr = pAttr;ptr;ptr = ptr->next)
						{
							if(!(ptr->hdr.u))
							{
								const long occur = ptr->data.occur;
								ofile << occur << _T(",");
							}
						}

						ofile << (pEnt->bottom().x/m_uor_per_subunit) << _T(",") << (pEnt->bottom().y/m_uor_per_subunit) << _T(",") << (pEnt->bottom().z/m_uor_per_subunit) << _T(",");
						ofile << (pEnt->top().x/m_uor_per_subunit) << _T(",") << (pEnt->top().y/m_uor_per_subunit) << _T(",") << (pEnt->top().z/m_uor_per_subunit) << std::endl;
					}
				}
				ofile.close();

				return ERROR_SUCCESS;
			}
		}
		/*else if(2 == iDiscipline)	/// Equipment
		{
			WriteEquipmentModelToXml(sDumpFilePath , sDgnFilePath);
		}*/

		return ERROR_FILE_NOT_FOUND;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::Clear(void)
{
	if(m_pEntMap)
	{
		for(map<int,list<CDgnElement*>* >::iterator itr = m_pEntMap->begin();itr != m_pEntMap->end();++itr)
		{
			SAFE_DELETE(itr->second);
		}
		m_pEntMap->clear();
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	occur에 해당하는 equipment 이름을 찾습니다.
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
CDgn62Element* CSmartDrawPDSModuleImpl::GetEquipmentNameEnt(map<int,list<CDgnElement*>* >* _pEntMap , const short& occur)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(DGNT_62);
		if(where != _pEntMap->end())
		{
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{
				CDgn62Element* pEnt = (CDgn62Element*)(*itr);
				if((occur == pEnt->occur()) || (occur == pEnt->occur2()))
				{
					return pEnt;
				}
			}
		}
	}

	return NULL;
}

/**	
	@brief	Equipment occurence를 리턴한다.(dmrs의 type이 21이고 information이 setting되었다면 equipment이다.)

	@author	HumKyung.BAEK

	@date	2010.05.19

	@param	

	@return	return -1 if this is not equipment entity.
*/
long CSmartDrawPDSModuleImpl::GetEquipmentOccurAndName(short& eType , STRING_T& rName , long& equi_occur , CDgnElement* pEnt)
{
	assert(pEnt && "pEnt is NULL");
	long occur = -1;

	const UInt32 uiType = pEnt->elementType();
	const int iMinSize = (uiType == CELL_HEADER_ELM) ? 76 : 0;
	if(pEnt && (pEnt->header().ElmHdr.words > iMinSize))	//! Equipment나 Nozzle이 될수 있는 최소 size보다 커야 한다.
	{
		eType = -1;
		PDGNATTRDATA pAttr = pEnt->attribute();
		for(PDGNATTRDATA ptr = pAttr;ptr;ptr = ptr->next)
		{
			if(/*ptr->hdr.i && */(21 == ptr->UserID))
			{
				equi_occur = ptr->data.occur;
				if(-1 == eType)
				{
					eType = 21;

					rName.clear();
					if(ptr->next && ptr->next->next && ptr->next->next->hdr.u)
					{
						char buf[6] = {'\0',};
						memcpy(buf , ptr->next->next->data.pData + 6 , sizeof(char)*5);
#ifdef	UNICODE
						rName = IsString::mbs_to_wcs(string(buf));
#else
						rName.assign(buf);
#endif
					}
					occur = (ptr->data.occur);
				}
			}
			else if(/*(ptr->hdr.i) && */(22 == ptr->UserID))
			{
				if(-1 == eType)
				{
					eType = 22;
					occur = (ptr->data.occur);

					rName.clear();
					if(ptr->next && ptr->next->next && ptr->next->next->next && ptr->next->next->next->hdr.u)
					{
						char buf[10] = {'\0',};
						memcpy(buf , ptr->next->next->next->data.pData , sizeof(char)*9);
						
						int i = 0;
						for(i = strlen(buf) - 1;(' ' == buf[i]) && (i >= 0);--i);
						buf[i + 1] = '\0';
#ifdef	UNICODE
						rName = IsString::mbs_to_wcs(string(buf));
#else
						rName.assign(buf);
#endif

					}
				}
			}
		}
	}

	return occur;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   GetEquipmentOrigin
    @return     int
    @param      DPoint3d&       origin
    @param      CDgnCellHeader* pCell
    @brief
******************************************************************************/
int GetEquipmentOrigin(DPoint3d& origin , CDgnCellHeader* pCell)
{
	assert(pCell && "pCell is NULL");
	if(pCell)
	{
		int iCount = pCell->GetEntityCount();
		for(int i = 0;i < iCount;++i)
		{
			CDgnElement* pElm = pCell->GetEntityAt(i);
			if(pElm->IsKindOf(CDgnPointString::TypeString()))
			{
				CDgnPointString* pPointStringElm = static_cast<CDgnPointString*>(pElm);
				if(pPointStringElm->GetPointCount() > 0)
				{
					origin = pPointStringElm->GetPointAt(0);
					return ERROR_SUCCESS;
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	register cylinder shape to equipment
	@author	humkyung
	@date	2014.04.18
*/
static int RegisterShapeToEquipment(CaDraw_Equipment* pEqui , CDgnElement* pDgnElm , const double& uor_per_subunit , const CIsPoint3d& ptModelOffset)
{
	assert(pEqui && pDgnElm && "pEqui or pDgnElm is NULL");

	if(pEqui && pDgnElm)
	{
		if(pDgnElm->IsKindOf(CDgnCellHeader::TypeString()))
		{
			CDgnCellHeader* pCellHeader = static_cast<CDgnCellHeader*>(pDgnElm);
			for(int i = 0;i < int(pCellHeader->GetEntityCount());++i)
			{
				if(pCellHeader->GetEntityAt(i)->IsKindOf(CDgnCone::TypeString()))
				{
					CDgnCone* pCone = static_cast<CDgnCone*>(pCellHeader->GetEntityAt(i));
					if(pCone->topRadius() == pCone->bottomRadius())
					{
						CaDraw_Equipment::CylinderShape cylinder;
						{
							cylinder.top.Set((pCone->top().x/uor_per_subunit) + ptModelOffset.x() , (pCone->top().y/uor_per_subunit) + ptModelOffset.y() , (pCone->top().z/uor_per_subunit) + ptModelOffset.z());
							cylinder.bottom.Set((pCone->bottom().x/uor_per_subunit) + ptModelOffset.x() , (pCone->bottom().y/uor_per_subunit) + ptModelOffset.y() , (pCone->bottom().z/uor_per_subunit) + ptModelOffset.z());
							cylinder.radius = pCone->topRadius()/uor_per_subunit;
						}
						pEqui->AppendShape(cylinder);
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	parse equipment
	@author	humkyung
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ParseEquipmentModelEntities(CADODB& adoDB , map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		map<long , CaDraw_Equipment*> oEqpMap;	/// temporary map for equipment
		map<long , CaDraw_Nozzle*> oNozzleMap;

		int iTypes[2] = {CELL_HEADER_ELM , DGNT_3D_SOLID_HEADER};
		for(int i = 0;i < sizeof(iTypes)/sizeof(iTypes[0]);++i)
		{
			map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(iTypes[i]);
			if(where != _pEntMap->end())
			{
				for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
				{
					CDgnElement* pEnt = *itr;
					
					CIsPoint3d ptMin(pEnt->volume().minx() / m_uor_per_subunit , pEnt->volume().miny() / m_uor_per_subunit , pEnt->volume().minz() / m_uor_per_subunit);
					ptMin += m_ptModelOffset;
					CIsPoint3d ptMax(pEnt->volume().maxx() / m_uor_per_subunit , pEnt->volume().maxy() / m_uor_per_subunit , pEnt->volume().maxz() / m_uor_per_subunit);
					ptMax += m_ptModelOffset;
				
					CIsVolume volume;
					volume.Add(ptMin); volume.Add(ptMax);

					short eType = -1;
					long equi_occur = -1; STRING_T rName;
					const long occur = this->GetEquipmentOccurAndName(eType , rName , equi_occur , *itr);
					if(-1 != occur)
					{
						if(21 == eType)		/// equipment type
						{
							/// find a equipment which has same occur number
							/*map<long , CaDraw_Equipment*>::iterator where = oEqpMap.find(occur);
							if(where != oEqpMap.end()) continue;	/// skip this case because it seems modeling error!!!
							{
								CIsVolume vol = where->second->GetVolume();
								vol += volume;
								where->second->SetVolume( vol );
								continue;
							}*/
							/// up to here

							CDgn62Element* p62Ent = GetEquipmentNameEnt(_pEntMap , occur);
							if(p62Ent)
							{
#ifdef	UNICODE
								STRING_T name(IsString::mbs_to_wcs(string(p62Ent->name())));
#else
								STRING_T name(p62Ent->name());
#endif
								
								CaDraw_Equipment* pEqui = m_pDraView->FindEquipmentByName(name);
								if(NULL == pEqui)
								{
									/// create a equipment instance and then store
									pEqui = (CaDraw_Equipment*)CaDraw_Equipment::CreateInstance();
									
									pEqui->SetModelType(m_eModelType);
									pEqui->SetModelName(*(m_pModelFilePath));
									pEqui->SetVolume(volume);
									if((*itr)->IsKindOf(CDgnCellHeader::TypeString()))
									{
										CDgnCellHeader* pCell = static_cast<CDgnCellHeader*>(*itr);
										DPoint3d origin = pCell->origin();
										(ERROR_SUCCESS == GetEquipmentOrigin(origin , pCell));
										pEqui->SetOrigin(CIsPoint3d(origin.x / m_uor_per_subunit , origin.y / m_uor_per_subunit , origin.z / m_uor_per_subunit) + m_ptModelOffset);
									}
									else
									{
										pEqui->SetOrigin(volume.center() + m_ptModelOffset);
									}
									pEqui->SetName(name);
									pEqui->SetName2(rName);

									oEqpMap.insert(make_pair(occur , pEqui));
								}
								else
								{
									if((*itr)->IsKindOf(CDgnCellHeader::TypeString()))
									{
										CDgnCellHeader* pCell = static_cast<CDgnCellHeader*>(*itr);
										DPoint3d origin = pCell->origin();
										(ERROR_SUCCESS == GetEquipmentOrigin(origin , pCell));
										pEqui->SetOrigin(CIsPoint3d(origin.x / m_uor_per_subunit , origin.y / m_uor_per_subunit , origin.z / m_uor_per_subunit) + m_ptModelOffset);
									}

									CIsVolume vol = pEqui->GetVolume();
									vol += volume;
									pEqui->SetVolume( vol );
								}

								if(NULL != pEqui) RegisterShapeToEquipment(pEqui , *itr , m_uor_per_subunit , m_ptModelOffset);	/// 2014.04.18 added by humkyung
							}
						}
						else if(22 == eType)	/// nozzle type
						{
							CDgn62Element* p62Ent = GetEquipmentNameEnt(_pEntMap , equi_occur);
							if(p62Ent)
							{
#ifdef	UNICODE
								STRING_T equi_name(IsString::mbs_to_wcs(string(p62Ent->name())));
#else
								STRING_T equi_name(p62Ent->name());
#endif

								CaDraw_NozzleAttribute attribute;
								attribute.GetAttribute(adoDB , m_nPartitionNo , occur , equi_occur);
								{
									/// store nozzle
									/// nozzle 관련 data를 database에서 읽어와야 한다.
									CDgnCellHeader* pCell = static_cast<CDgnCellHeader*>(*itr);
									CaDraw_Nozzle *pDrawNozz = (CaDraw_Nozzle*)CaDraw_Nozzle::CreateInstance();
									if(pDrawNozz)
									{
										pDrawNozz->SetModelType(m_eModelType);
										pDrawNozz->SetModelName(*(m_pModelFilePath));
										pDrawNozz->SetVolume(volume);
										pDrawNozz->SetOrigin(CIsPoint3d(pCell->origin().x / m_uor_per_subunit , pCell->origin().y / m_uor_per_subunit , pCell->origin().z / m_uor_per_subunit) + m_ptModelOffset);
										pDrawNozz->SetName(rName);
										pDrawNozz->SetEquiName(equi_name);

										pDrawNozz->SetNominalPipeDia(attribute.m_nominal_piping_dia);
										pDrawNozz->SetRating(attribute.m_rating);
										pDrawNozz->SetNozzleCode(attribute.m_nozzle_code);
										pDrawNozz->SetNozzleType(attribute.m_nozzle_type);
										pDrawNozz->SetProjection(attribute.m_projection);
										pDrawNozz->SetOrientation(attribute.m_orientation);

										m_pDraView->Add(pDrawNozz);
									}
								}
							}
						}
					}
				}
			}
		}

		/// equipment origin(x,y) must be located in view volume(zlow - zhigh is enough to be locaed through view volume)
		CIsVolume oViewVolume = m_pDraView->GetVolume();
		for(map<long , CaDraw_Equipment*>::iterator itr = oEqpMap.begin();itr != oEqpMap.end();)
		{
			const CIsPoint3d origin = itr->second->origin();
			CIsVolume vol = itr->second->GetVolume();
			const bool exclude = 
				((origin.x() < oViewVolume.minx()) || (origin.x() > oViewVolume.maxx()) || 
				(origin.y() < oViewVolume.miny()) || (origin.y() > oViewVolume.maxy()) || 
				(vol.minz() > oViewVolume.maxz()) || (vol.maxz() < oViewVolume.minz()));
			if(false == exclude)
			{
				m_pDraView->Add(itr->second);	
			}
			else
			{
				SAFE_DELETE(itr->second);
				itr = oEqpMap.erase(itr);
				continue;
			}
			++itr;
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write equipment entities to xml file
	@author	humkyung
	@date	2013.11.01
	@param	sXmlFilePath	
	@param	_pEntMap
	@return
*/
int CSmartDrawPDSModuleImpl::WriteEquipmentEntitiesToXml(const STRING_T& sXmlFilePath , map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		map<long , CaDraw_Equipment*> oEqpMap;	/// temporary map for equipment
		map<long , CaDraw_Nozzle*> oNozzleMap;

		CMarkup xml;
		xml.SetDoc( _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n") );
		xml.AddElem( _T("SmartDraw") );

		const int iTypes[3] = {CELL_HEADER_ELM , DGNT_3D_SOLID_HEADER , DGNT_62};
		for(int i = 0;i < sizeof(iTypes)/sizeof(iTypes[0]);++i)
		{
			map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(iTypes[i]);
			if(where != _pEntMap->end())
			{
				for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
				{
					CDgnElement* pEnt = *itr;
					if(DGNT_62 == pEnt->header().ElmHdr.type)
					{
						CDgn62Element* p62Elm = static_cast<CDgn62Element*>(pEnt);

						xml.AddChildElem( _T("Item") );
						xml.IntoElem();
						xml.AddAttrib(_T("DgnType") , _T("DGNT62"));
						xml.AddAttrib(_T("Discipline") , _T("21"));
						{
							OSTRINGSTREAM_T oss;
							oss <<  p62Elm->occur();
							xml.AddAttrib(_T("occur1") , oss.str().c_str());
							oss.str(_T(""));
							oss <<  p62Elm->occur2();
							xml.AddAttrib(_T("occur2") , oss.str().c_str());
						}
						xml.AddAttrib(_T("Name") , p62Elm->name());
						xml.AddAttrib(_T("Desc1") , p62Elm->descr1());
						xml.AddAttrib(_T("Desc2") , p62Elm->descr2());

						xml.OutOfElem();

						continue;
					}
					
					CIsPoint3d ptMin(pEnt->volume().minx() / m_uor_per_subunit , pEnt->volume().miny() / m_uor_per_subunit , pEnt->volume().minz() / m_uor_per_subunit);
					CIsPoint3d ptMax(pEnt->volume().maxx() / m_uor_per_subunit , pEnt->volume().maxy() / m_uor_per_subunit , pEnt->volume().maxz() / m_uor_per_subunit);
				
					CIsVolume volume;
					volume.Add(ptMin); volume.Add(ptMax);

					short eType = -1;
					long equi_occur = -1; STRING_T rName;
					const long occur = this->GetEquipmentOccurAndName(eType , rName , equi_occur , *itr);
					if(-1 != occur)
					{
						if(21 == eType)		/// equipment type
						{
							CDgn62Element* p62Ent = GetEquipmentNameEnt(_pEntMap , occur);
							if(p62Ent)
							{
#ifdef	UNICODE
								STRING_T name(IsString::mbs_to_wcs(string(p62Ent->name())));
#else
								STRING_T name(p62Ent->name());
#endif
								
								xml.AddChildElem( _T("Item") );
								xml.IntoElem();
								xml.AddAttrib(_T("DgnType") , (CELL_HEADER_ELM == iTypes[i]) ?  _T("CellHeader") : _T("SolidHeader"));
								xml.AddAttrib(_T("Discipline") , _T("21"));
								{
									OSTRINGSTREAM_T oss;
									oss << occur;
									xml.AddAttrib(_T("occur1") , oss.str().c_str());
								}
								xml.AddAttrib(_T("Name") , name.c_str());
								xml.AddAttrib(_T("Desc") , rName.c_str());

								xml.OutOfElem();
							}
						}
						else if(22 == eType)	/// nozzle type
						{
							CDgn62Element* p62Ent = GetEquipmentNameEnt(_pEntMap , equi_occur);
							if(p62Ent)
							{
#ifdef	UNICODE
								STRING_T equi_name(IsString::mbs_to_wcs(string(p62Ent->name())));
#else
								STRING_T equi_name(p62Ent->name());
#endif
								xml.AddChildElem( _T("Item") );
								xml.IntoElem();
								xml.AddAttrib(_T("DgnType") , (CELL_HEADER_ELM == iTypes[i]) ?  _T("CellHeader") : _T("SolidHeader"));
								xml.AddAttrib(_T("Discipline") , _T("22"));
								{
									OSTRINGSTREAM_T oss;
									oss << occur;
									xml.AddAttrib(_T("occur1") , oss.str().c_str());

									oss.str(_T(""));
									oss << equi_occur;
									xml.AddAttrib(_T("eqp_occur") , oss.str().c_str());
								}
								xml.AddAttrib(_T("Name") , rName.c_str());

								xml.OutOfElem();
							}
						}
					}
				}
			}
		}

		xml.Save(sXmlFilePath.c_str());
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	get entity type from cell
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::GetEntityType(long occur[3] , vector<DPoint3d>& pts , CDgnCellHeader* pCell)
{
	assert(pCell && "pCell is NULL");
	int nRet = -1;

	if(pCell)
	{
		int iCount = 0;
		const size_t nCount = pCell->GetEntityCount();
		for(int i = 0;i < int(nCount);++i)
		{
			CDgnElement* pEnt = pCell->GetEntityAt(i);
			if(pEnt->IsKindOf(CDgnPointString::TypeString()))
			{
				PDGNATTRDATA pAttr = pEnt->attribute();
				for(PDGNATTRDATA ptr = pAttr;ptr;ptr = ptr->next)
				{
					if(!(ptr->hdr.u) && (0 == ptr->hdr.wtf) && (iCount < 3))
					{
						if(0 == iCount) nRet = (ptr->UserID);
						occur[iCount++] = ptr->data.occur;
						
/*
						if((GENERAL == nRet) && (ptr->next && ptr->next->next && ptr->next->next->next))
						{
							occur[1] = ptr->next->next->data.occur;
							occur[2] = ptr->next->next->next->data.occur;
						}
						break;
*/
					}
				}

				CDgnPointString* pPointString = static_cast<CDgnPointString*>(pEnt);
				vector<DPoint3d> oTemp = pPointString->GetPointStringExceptSamePoint();
				pts.insert(pts.end() , oTemp.begin() , oTemp.end());

				break;
			}
		}
	}

	return nRet;
}

/**	
	@brief	parse pipe entity from model file and then create a aDraw_Pipe object.
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	map<int,list<CDgnElement*>* >* _pEntMap
	@return
*/
int CSmartDrawPDSModuleImpl::ParsePipeModelEntities(CADODB& adoDB , map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(CELL_HEADER_ELM);
		if(where != _pEntMap->end())
		{
			for(list<CDgnElement*>::const_iterator itr = where->second->begin();itr != where->second->end();++itr)
			{
				CDgnCellHeader* pCell = (CDgnCellHeader*)(*itr);
				long occur[3] = {-1,-1,-1};
				vector<DPoint3d> oPointList;
				const int nEntityType = (GetEntityType(occur , oPointList , pCell));
				switch(nEntityType)
				{
					case GENERAL:
					{
						CaDraw_GeneralPipingParser parser(m_eModelType , *(m_pModelFilePath) , m_uor_per_subunit);
						parser.m_occur[0] = occur[0];
						parser.m_occur[1] = occur[1];
						parser.m_occur[2] = occur[2];

						parser.Parse(m_pDraView , pCell , adoDB , m_nPartitionNo , m_ptModelOffset);
					}
						break;
					case INSTRUMENT:
					{
						CaDraw_InstrumentParser parser(m_eModelType , *(m_pModelFilePath) , m_uor_per_subunit);
						parser.m_occur = occur[0];

						parser.Parse(m_pDraView , pCell , adoDB , m_nPartitionNo , m_ptModelOffset);
					}
						break;
					case SUPPORT:
					{
						CaDraw_SupportParser parser(m_eModelType , *(m_pModelFilePath) , m_uor_per_subunit);
						parser.m_occur[0] = occur[0];
						parser.m_occur[1] = occur[1];

						parser.Parse(m_pDraView , pCell , adoDB , m_nPartitionNo , m_ptModelOffset);
					}
						break;
				}
			}
		}

		where = _pEntMap->find(LINE_STRING_ELM);
		if(where != _pEntMap->end())
		{
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{
				CDgnLineString* pEnt = (CDgnLineString*)(*itr);

				CaDraw_PipeParser pipeParser;
				pipeParser.m_eModelType = m_eModelType;
				pipeParser.m_rModelName = *(m_pModelFilePath);
				pipeParser.m_uor_per_subunit = m_uor_per_subunit;
				pipeParser.Parse(m_pDraView , pEnt , adoDB , m_nPartitionNo , m_ptModelOffset);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ExtractPipeModelEntities(CADODB& adoDB , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	parser.AddParsingType(LINE_STRING_ELM);
	parser.AddParsingType(CELL_HEADER_ELM);
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
			}
			else if((CELL_HEADER_ELM == nType) || (LINE_STRING_ELM == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.x());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}
		}
	}

	ParsePipeModelEntities(adoDB , &_EntMap);

	return ERROR_SUCCESS;
}

/**	
	@brief	extract equipment entities from a model file
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ExtractEquipmentModelEntities(CADODB& adoDB , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	{
		parser.AddParsingType(CELL_HEADER_ELM);
		parser.AddParsingType(DGNT_3D_SOLID_HEADER);
		parser.AddParsingType(DGNT_62);
	}
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
			}
			else if(DGNT_62 == nType)
			{
				CDgn62Element* pEnt = (CDgn62Element*)parser.ParseElement(&dgnFile);
				map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(DGNT_62);
				if(where == _EntMap.end())
				{
					list<CDgnElement*>* plst = new list<CDgnElement*>;
					plst->push_back(pEnt);
					_EntMap.insert(make_pair(nType , plst));
				}
				else
				{
					where->second->push_back(pEnt);
				}
			}
			else if((CELL_HEADER_ELM == nType) || (DGNT_3D_SOLID_HEADER == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);
					
					/*CDgnVolume dgnVolume = pEnt->volume();
					dgnVolume.Set(
						(dgnVolume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (dgnVolume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (dgnVolume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
						(dgnVolume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (dgnVolume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (dgnVolume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());*/

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}
		}
	}

	/// cell header has first priority, so the solid header is removed if it has same same occur
	map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(CELL_HEADER_ELM);
	if(where != _EntMap.end())
	{
		map<int,list<CDgnElement*>* >::iterator where2 = _EntMap.find(DGNT_3D_SOLID_HEADER);
		if(where2 != _EntMap.end())
		{
			short eType[2] = {-1,-1};
			STRING_T sName[2];
			long equi_occur[2] = {-1,-1};
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{				
				const long occur1 = this->GetEquipmentOccurAndName(eType[0] , sName[0] , equi_occur[0] , *itr);
				if(21 == eType[0])
				{
					for(list<CDgnElement*>::iterator jtr = where2->second->begin();jtr != where2->second->end();)
					{
						const long occur2 = this->GetEquipmentOccurAndName(eType[1] , sName[1] , equi_occur[1] , *itr);
						if(21 == eType[1])
						{
							if(occur1 == occur2)
							{
								SAFE_DELETE(*jtr);
								jtr = where2->second->erase(jtr);
								continue;
							}
						}
						++jtr;
					}
				}
			}
		}
	}
	/// up to here

	ParseEquipmentModelEntities(adoDB , &_EntMap);
	
	/// clear
	for(map<int,list<CDgnElement*>* >::iterator itr = _EntMap.begin();itr != _EntMap.end();++itr)
	{
		for(list<CDgnElement*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			CDgnElement::DeleteInstance(*jtr);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write equipment model to xml file
	@author	humkyung
	@date	2013.11.01
	@param	sXmlFilePath	xml file
	@param	rFilePath		equipment model file path
	@return	int
*/
int CSmartDrawPDSModuleImpl::WriteEquipmentModelToXml(const STRING_T& sXmlFilePath , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	{
		parser.AddParsingType(CELL_HEADER_ELM);
		parser.AddParsingType(DGNT_3D_SOLID_HEADER);
		parser.AddParsingType(DGNT_62);
	}
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
			}
			else if(DGNT_62 == nType)
			{
				CDgn62Element* pEnt = (CDgn62Element*)parser.ParseElement(&dgnFile);
				map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(DGNT_62);
				if(where == _EntMap.end())
				{
					list<CDgnElement*>* plst = new list<CDgnElement*>;
					plst->push_back(pEnt);
					_EntMap.insert(make_pair(nType , plst));
				}
				else
				{
					where->second->push_back(pEnt);
				}
			}
			else if((CELL_HEADER_ELM == nType) || (DGNT_3D_SOLID_HEADER == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					volume.minx() / m_uor_per_subunit , volume.miny() / m_uor_per_subunit , volume.minz() / m_uor_per_subunit ,
					volume.maxx() / m_uor_per_subunit , volume.maxy() / m_uor_per_subunit , volume.maxz() / m_uor_per_subunit);
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);
					
					CDgnVolume dgnVolume = pEnt->volume();
					dgnVolume.Set(
						dgnVolume.minx() / m_uor_per_subunit , dgnVolume.miny() / m_uor_per_subunit , dgnVolume.minz() / m_uor_per_subunit ,
						dgnVolume.maxx() / m_uor_per_subunit , dgnVolume.maxy() / m_uor_per_subunit , dgnVolume.maxz() / m_uor_per_subunit);

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}
		}
	}

	/// cell header has first priority, so the solid header is removed if it has same same occur
	map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(CELL_HEADER_ELM);
	if(where != _EntMap.end())
	{
		map<int,list<CDgnElement*>* >::iterator where2 = _EntMap.find(DGNT_3D_SOLID_HEADER);
		if(where2 != _EntMap.end())
		{
			short eType[2] = {-1,-1};
			STRING_T sName[2];
			long equi_occur[2] = {-1,-1};
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{				
				long occur1 = this->GetEquipmentOccurAndName(eType[0] , sName[0] , equi_occur[0] , *itr);
				if(21 == eType[0])
				{
					for(list<CDgnElement*>::iterator jtr = where2->second->begin();jtr != where2->second->end();)
					{
						long occur2 = this->GetEquipmentOccurAndName(eType[1] , sName[1] , equi_occur[1] , *itr);
						if(21 == eType[1])
						{
							if(occur1 == occur2)
							{
								SAFE_DELETE(*jtr);
								jtr = where2->second->erase(jtr);
								continue;
							}
						}
						++jtr;
					}
				}
			}
		}
	}
	/// up to here

	WriteEquipmentEntitiesToXml(sXmlFilePath , &_EntMap);
	
	/// clear
	for(map<int,list<CDgnElement*>* >::iterator itr = _EntMap.begin();itr != _EntMap.end();++itr)
	{
		for(list<CDgnElement*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			CDgnElement::DeleteInstance(*jtr);
		}
	}
	/// up to here

	return ERROR_SUCCESS;
}

/**	
	@brief	get text string which located inside given circle(ignore elevation)
	@author	humkyung
	@date	2010.05.20
	@param	
	@return	const char*
*/
const char* CSmartDrawPDSModuleImpl::FindColumnTextInGridCircle(CDgnElement* pGridCircle , map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(pGridCircle && "pGridCircle is NULL");

	if(pGridCircle)
	{
		CIsVolume volume(pGridCircle->volume().minx() , pGridCircle->volume().miny() , 0.0 , pGridCircle->volume().maxx() , pGridCircle->volume().maxy() , 0.0);
		
		map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(TEXT_ELM);
		if(where != _pEntMap->end())
		{
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{
				CDgnText* pText = (CDgnText*)(*itr);
				CIsVolume _volume;
				DPoint3d origin = pText->origin();
				_volume.Add(CIsPoint3d(origin.x , origin.y , 0.0) + m_ptModelOffset);
				if(volume.CollideWith(_volume))
				{
					return pText->textString();
				}
			}
		}
	}

	return NULL;
}

/**	
	@brief	structure model file을 분석한다.
	@author	HumKyung.BAEK
	@date	2010.05.20
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ParseStructureModelEntities(map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		/// get options
		const int iGratingLevel = GetPrivateProfileInt(_T("ANNOTATION_OPTIONS") , _T("Grating_Platform_Level") , 21 , m_pIniFilePath->c_str());
		const int iConcreteLevel= GetPrivateProfileInt(_T("ANNOTATION_OPTIONS") , _T("Concrete_Platform_Level"), 0 , m_pIniFilePath->c_str());

		TCHAR szBuf[MAX_PATH + 1] = {'\0'};
		STRING_T sDelimiter(_T("-"));
		if(GetPrivateProfileString(_T("Structure") , _T("Delimiter") , _T("-") , szBuf , MAX_PATH , m_pIniFilePath->c_str()))
		{
			sDelimiter.assign(szBuf);
		}

		/// get column name type - 2014.12.26 added by humkyung
		ColumnNameType eColumnNameType = COLUMN_DATA;
		GetPrivateProfileString(_T("Structure") , _T("Column Name Type") , _T("Column Data") , szBuf , MAX_PATH , m_pIniFilePath->c_str());
		eColumnNameType = (STRING_T(_T("Column Data")) == szBuf) ? COLUMN_DATA : GRID_LINE_DATA;
		/// up to here
		/// up to here

		list<CDgnElement*> StruEntList;
		map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(CELL_HEADER_ELM);
		if(where != _pEntMap->end()) StruEntList.insert(StruEntList.end() , where->second->begin() , where->second->end());
		where = _pEntMap->find(DGNT_3D_SOLID_HEADER);
		if(where != _pEntMap->end()) StruEntList.insert(StruEntList.end() , where->second->begin() , where->second->end());
		where = _pEntMap->find(DGNT_SURFACE_HEADER);
		if(where != _pEntMap->end()) StruEntList.insert(StruEntList.end() , where->second->begin() , where->second->end());

		list<CDgnElement*> GridCircleEntList;
		where = _pEntMap->find(ARC_ELM);
		if(where != _pEntMap->end()) GridCircleEntList.insert(GridCircleEntList.end() , where->second->begin() , where->second->end());
		where = _pEntMap->find(ELLIPSE_ELM);
		if(where != _pEntMap->end()) GridCircleEntList.insert(GridCircleEntList.end() , where->second->begin() , where->second->end());
		for(list<CDgnElement*>::iterator itr = StruEntList.begin();itr != StruEntList.end();++itr)
		{
			CDgnElement* pEnt = (*itr);
			if(IsStructureType(pEnt))
			{
				CIsVolume volume(
					(pEnt->volume().minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (pEnt->volume().miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (pEnt->volume().minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(pEnt->volume().maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (pEnt->volume().maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (pEnt->volume().maxz() / m_uor_per_subunit) + m_ptModelOffset.z());

				vector<STRING_T> oResult;
				if(COLUMN_DATA == eColumnNameType)
				{
					PDGNATTRDATA pAttr = pEnt->attribute();
					if(pAttr && (NULL != pAttr->data.pData))
					{
						TCHAR buf[17] = {'\0',};
						memcpy(buf , pAttr->data.pData + 12, sizeof(char)*16);
						CTokenizer<CIsFromString>::Tokenize(oResult , buf , CIsFromString(sDelimiter));
					}
				}
				else if(GRID_LINE_DATA == eColumnNameType)
				{
					/// find structure name from grid
					const char* pNames[2] = {NULL,NULL};

					CIsPoint3d ptCenter;
					ptCenter.Set(
						(pEnt->volume().minx() + pEnt->volume().maxx()) * 0.5 ,
						(pEnt->volume().miny() + pEnt->volume().maxy()) * 0.5 ,
						(pEnt->volume().minz() + pEnt->volume().maxz()) * 0.5);
					const double dx = pEnt->volume().maxx() - pEnt->volume().minx();
					const double dy = pEnt->volume().maxy() - pEnt->volume().miny();
					for(list<CDgnElement*>::iterator jtr = GridCircleEntList.begin();jtr != GridCircleEntList.end();++jtr)
					{
						CIsPoint3d origin;
						if((*jtr)->IsKindOf(CDgnArc::TypeString()))
						{
							CDgnArc* pArc = (CDgnArc*)(*jtr);
							origin.Set(pArc->origin().x , pArc->origin().y , pArc->origin().z);
						}
						else if((*jtr)->IsKindOf(CDgnEllipse::TypeString()))
						{
							CDgnEllipse* pEllipse = (CDgnEllipse*)(*jtr);
							origin.Set(pEllipse->origin().x , pEllipse->origin().y , pEllipse->origin().z);
						}

						if(fabs(ptCenter.x() - origin.x()) <= dx*0.5)
						{
							const char* pName = FindColumnTextInGridCircle(*jtr , _pEntMap);
							if(pName) pNames[0] = pName;
						}
						else if(fabs(ptCenter.y() - origin.y()) <= dy*0.5)
						{
							const char* pName = FindColumnTextInGridCircle(*jtr , _pEntMap);
							if(pName) pNames[1] = pName;
						}
					}

					if((NULL != pNames[0]) && (NULL != pNames[1]))
					{
#ifdef	_UNICODE
						oResult.push_back(IsString::mbs_to_wcs(string(pNames[0])));
						oResult.push_back(IsString::mbs_to_wcs(string(pNames[1])));
#else
						oResult.push_back(STRING_T(pNames[1]));
						oResult.push_back(STRING_T(pNames[0]));
#endif
					}
				}

				if(2 == oResult.size())
				{
					CaDraw_Structure *pDrawStru = (CaDraw_Structure*)CaDraw_Structure::CreateInstance();
					if(pDrawStru)
					{
						pDrawStru->SetModelType(m_eModelType);
						pDrawStru->SetModelName(*(m_pModelFilePath));
						pDrawStru->SetVolume(volume);
						pDrawStru->SetOrigin(
							CIsPoint3d((volume.minx() + volume.maxx()) * 0.5 , (volume.miny() + volume.maxy()) * 0.5 , (volume.minz() + volume.maxz()) * 0.5) + m_ptModelOffset
							);

						pDrawStru->SetName1(oResult[1]);
						pDrawStru->SetName2(oResult[0]);

						if(ERROR_SUCCESS != m_pDraView->Add(pDrawStru))
						{
							SAFE_DELETE(pDrawStru);
						}
					}
				}
			}
			else if(pEnt->IsKindOf(CDgnCellHeader::TypeString()) || 
				(((iGratingLevel == pEnt->header().ElmHdr.level) || (iConcreteLevel == pEnt->header().ElmHdr.level)) && 
				 (pEnt->IsKindOf(CDgnSolidHeader3d::TypeString()) || pEnt->IsKindOf(CDgnSurfHeader3d::TypeString())))
				 )
			{
				ParseEntityAsPlatform(pEnt , iGratingLevel , iConcreteLevel);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	extract entities from structure model file.
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ExtractStructureModelEntities(CADODB& adoDB , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	{
		parser.AddParsingType(TEXT_ELM);
		parser.AddParsingType(ARC_ELM);
		parser.AddParsingType(ELLIPSE_ELM);
		parser.AddParsingType(DGNT_3D_SOLID_HEADER);
		parser.AddParsingType(DGNT_SURFACE_HEADER);
		parser.AddParsingType(CELL_HEADER_ELM);
		parser.AddParsingType(DGNT_SHAPE);	//! for platform
	}
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			CDgnElement* pEnt = NULL;

			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
				pEnt = pTCB;
			}
			else if((CELL_HEADER_ELM == nType) || (DGNT_3D_SOLID_HEADER == nType) || (DGNT_SURFACE_HEADER == nType) || (DGNT_SHAPE == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					pEnt = parser.ParseElement(&dgnFile);
				}
			}
			else if((ARC_ELM == nType) || (ELLIPSE_ELM == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					pEnt = parser.ParseElement(&dgnFile);
				}
			}
			else if(TEXT_ELM == nType)
			{
				pEnt = parser.ParseElement(&dgnFile);
			}else	continue;

			if(NULL != pEnt)
			{
				map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
				if(where == _EntMap.end())
				{
					list<CDgnElement*>* plst = new list<CDgnElement*>;
					plst->push_back(pEnt);
					_EntMap.insert(make_pair(nType , plst));
				}
				else
				{
					where->second->push_back(pEnt);
				}
			}
		}
	}

	ParseStructureModelEntities(&_EntMap);
	/// clear
	for(map<int,list<CDgnElement*>* >::iterator itr = _EntMap.begin();itr != _EntMap.end();++itr)
	{
		for(list<CDgnElement*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			CDgnElement::DeleteInstance(*jtr);
		}
	}

	return ERROR_SUCCESS;
}

#include <boost/regex.hpp>

#ifdef _UNICODE
typedef boost::wregex CBoostRegEx;
#else
typedef boost::regex CBoostRegEx;
#endif

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ParseCableTModelEntities(map<int,list<CDgnElement*>* >* _pEntMap)
{
	assert(_pEntMap && "_pEntMap is NULL");

	if(_pEntMap)
	{
		CBoostRegEx regex[2];
		STRING_T sFileNameExp[2] , sLabel[2];
		{
			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS") , _T("Elec_Cable_Tray_FileName") , _T("") , szBuf , MAX_PATH , m_pIniFilePath->c_str()))
			{
				sFileNameExp[0] = szBuf;
				regex[0].assign( szBuf );
			}
			regex[0].assign( _T("112ae(.)*") );
			if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS") , _T("Elec_Cable_Tray_Label") , _T("") , szBuf , MAX_PATH , m_pIniFilePath->c_str()))
			{
				sLabel[0] = szBuf;
			}
			if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS") , _T("Inst_Cable_Tray_FileName") , _T("") , szBuf , MAX_PATH , m_pIniFilePath->c_str()))
			{
				sFileNameExp[1] = szBuf;
				regex[1].assign( szBuf );
			}
			regex[1].assign( _T("112ai(.)*") );
			if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS") , _T("Inst_Cable_Tray_Label") , _T("") , szBuf , MAX_PATH , m_pIniFilePath->c_str()))
			{
				sLabel[1] = szBuf;
			}
		}
		
		map<int,list<CDgnElement*>* >::iterator where = _pEntMap->find(CELL_HEADER_ELM);
		if(where != _pEntMap->end())
		{
			for(list<CDgnElement*>::iterator itr = where->second->begin();itr != where->second->end();++itr)
			{
				CDgnCellHeader* pDgnCell = static_cast<CDgnCellHeader*>((*itr));

				PDGNATTRDATA pAttr = pDgnCell->attribute();
				if(DGNLT_RIS == pDgnCell->GetAttrDataType(pAttr))
				{
					CIsPoint3d ptMin(pDgnCell->volume().minx() / m_uor_per_subunit , pDgnCell->volume().miny() / m_uor_per_subunit , pDgnCell->volume().minz() / m_uor_per_subunit);
					ptMin += m_ptModelOffset;
					CIsPoint3d ptMax(pDgnCell->volume().maxx() / m_uor_per_subunit , pDgnCell->volume().maxy() / m_uor_per_subunit , pDgnCell->volume().maxz() / m_uor_per_subunit);
					ptMax += m_ptModelOffset;

					CIsVolume volume;
					volume.Add(ptMin); volume.Add(ptMax);

					/// this is tray created by race-way
					CaDraw_CableTrayParser parser(m_uor_per_subunit);
					parser.Parse(pDgnCell);
					
					CaDraw_CableTray *pCableT = (CaDraw_CableTray*)CaDraw_CableTray::CreateInstance();
					if(pCableT)
					{
						pCableT->SetModelType(m_eModelType);
						pCableT->SetModelName(*(m_pModelFilePath));
						pCableT->SetVolume(volume);
						pCableT->SetStartPoint(CIsPoint3d(parser.m_pt[0].x , parser.m_pt[0].y , parser.m_pt[0].z) + m_ptModelOffset);
						pCableT->SetEndPoint(CIsPoint3d(parser.m_pt[1].x , parser.m_pt[1].y , parser.m_pt[1].z) + m_ptModelOffset);
						pCableT->Set3rdPoint(CIsPoint3d(parser.m_pt[2].x , parser.m_pt[2].y , parser.m_pt[2].z) + m_ptModelOffset);
						pCableT->SetWidth1(parser.m_width[0]);
						pCableT->SetWidth2(parser.m_width[1]);
						pCableT->SetHeight(parser.m_height);
						
						pCableT->SetType(parser.m_eType);
						pCableT->SetSubType(parser.m_eSubType);
						
						pCableT->SetLengthDir(parser.m_lengthDir);
						pCableT->SetWidthDir(parser.m_widthDir);
						pCableT->SetHeightDir(parser.m_heightDir);
						if(CaDraw_CableTray::ELBOW_T == parser.m_eType)
						{
							pCableT->SetAngle(parser.m_nAngle);
						}
						else if(CaDraw_CableTray::REDUCE_T == parser.m_eType)
						{
							vector<CIsPoint3d> oEdgeList;
							for(vector<DPoint3d>::iterator jtr = parser.m_ptEdgeList.begin();jtr != parser.m_ptEdgeList.end();++jtr)
							{
								oEdgeList.push_back(CIsPoint3d(jtr->x , jtr->y , jtr->z) + m_ptModelOffset);
							}
							pCableT->SetEdgeList(oEdgeList);
						}

						///TODO: set cable tray name
						try
						{
							CSplitPath path(m_pModelFilePath->c_str());
							STRING_T sFileName = path.GetFileName().operator LPCTSTR();

							boost::match_results<const TCHAR *> what;
							/// This is a full match check, not a partial match
							if(true == boost::regex_match (static_cast<const TCHAR *>(sFileName.c_str()), what, regex[0] , boost::match_default))
							{
								pCableT->SetName(sLabel[0]);
								pCableT->SetCategory(CaDraw_CableTray::ELECTRIC);
							}
							else if(true == boost::regex_match (static_cast<const TCHAR *>(sFileName.c_str()), what, regex[1] , boost::match_default))
							{
								pCableT->SetName(sLabel[1]);
								pCableT->SetCategory(CaDraw_CableTray::INSTRUMENT);
							}
						}
						catch(const std::exception &e)
						{
							UNUSED_ALWAYS(e);
							pCableT->SetName(_T("ELEC CABLE TRAY"));
						}
						catch(...)
						{
							pCableT->SetName(_T("ELEC CABLE TRAY"));
						}

						m_pDraView->Add(pCableT);
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ExtractCableTModelEntities(CADODB& adoDB , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	{
		parser.AddParsingType(CELL_HEADER_ELM);
	}
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
			}
			else if((CELL_HEADER_ELM == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);
					if(pEnt->IsKindOf(CDgnCellHeader::TypeString()))
					{
						CDgnCellHeader* pDgnCell = static_cast<CDgnCellHeader*>(pEnt);
						if(0 == pDgnCell->GetEntityCount()) continue;

						map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
						if(where == _EntMap.end())
						{
							list<CDgnElement*>* plst = new list<CDgnElement*>;
							plst->push_back(pEnt);
							_EntMap.insert(make_pair(nType , plst));
						}
						else
						{
							where->second->push_back(pEnt);
						}
					}
				}
			}
		}
	}

	ParseCableTModelEntities(&_EntMap);
	/// clear
	for(map<int,list<CDgnElement*>* >::iterator itr = _EntMap.begin();itr != _EntMap.end();++itr)
	{
		for(list<CDgnElement*>::iterator jtr = itr->second->begin();jtr != itr->second->end();++jtr)
		{
			CDgnElement::DeleteInstance(*jtr);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ExtractOtherModelEntities(CADODB& adoDB , const STRING_T& rFilePath)
{
	map<int,list<CDgnElement*>* > _EntMap;

	CDgnFile dgnFile;
	CDgnParser parser(NULL);
	parser.AddParsingType(LINE_STRING_ELM);
	parser.AddParsingType(CELL_HEADER_ELM);
	if(true == dgnFile.Open(rFilePath.c_str() , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
			{
				CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
				m_uor_per_subunit = pTCB->uor_per_subunit;
			}
			else if((CELL_HEADER_ELM == nType) || (LINE_STRING_ELM == nType))
			{
				CIsVolume volume;
				dgnFile.GetVolume(volume);
				volume.Set(
					(volume.minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
					(volume.maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (volume.maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (volume.maxz() / m_uor_per_subunit) + m_ptModelOffset.z());
				if(m_pDraView->DoesIncludeVolume(volume))
				{
					CDgnElement* pEnt = parser.ParseElement(&dgnFile);

					map<int,list<CDgnElement*>* >::iterator where = _EntMap.find(nType);
					if(where == _EntMap.end())
					{
						list<CDgnElement*>* plst = new list<CDgnElement*>;
						plst->push_back(pEnt);
						_EntMap.insert(make_pair(nType , plst));
					}
					else
					{
						where->second->push_back(pEnt);
					}
				}
			}
		}
	}

	///ParsePipeModelEntities(&_EntMap);

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.20
	@param	
	@return
*/
bool CSmartDrawPDSModuleImpl::IsStructureType(CDgnElement* pEnt)
{
	assert(pEnt && "pEnt is NULL");

	if(pEnt)
	{
		return ((32 == pEnt->header().ElmHdr.level) || (36 == pEnt->header().ElmHdr.level));
	}

	return false;
}

/**	
	@brief	parse entity which is in list as platform
	@author	HumKyung.BAEK
	@date	2010.06.06
	@param	
	@return
*/
int CSmartDrawPDSModuleImpl::ParseEntityAsPlatform(CDgnElement* pEnt , const int& iGratingLevel , const int& iConcreteLevel)
{
	assert(pEnt && "pEnt is NULL");

	if(pEnt)
	{
		CIsVolume volume(
				(pEnt->volume().minx() / m_uor_per_subunit) + m_ptModelOffset.x() , (pEnt->volume().miny() / m_uor_per_subunit) + m_ptModelOffset.y() , (pEnt->volume().minz() / m_uor_per_subunit) + m_ptModelOffset.z() ,
				(pEnt->volume().maxx() / m_uor_per_subunit) + m_ptModelOffset.x() , (pEnt->volume().maxy() / m_uor_per_subunit) + m_ptModelOffset.y() , (pEnt->volume().maxz() / m_uor_per_subunit) + m_ptModelOffset.z());

		CDgnSurfElement* pDgnSurfElem = NULL;
		if(pEnt->IsKindOf(CDgnSolidHeader3d::TypeString()))
		{
			CDgnSolidHeader3d* pDgnSolid = static_cast<CDgnSolidHeader3d*>(pEnt);
			const int iSurfType = pDgnSolid->surf_type();
			if((CDgnElement::SWEEP_SURF == iSurfType) || (CDgnElement::SURF_OF_PROJECTION == iSurfType))
			{
				pDgnSurfElem = static_cast<CDgnSurfElement*>(pDgnSolid->GetSurfElemAt(0));
			}
		}
		else if(pEnt->IsKindOf(CDgnSurfHeader3d::TypeString()))
		{
			CDgnSurfHeader3d* pDgnSurf = static_cast<CDgnSurfHeader3d*>(pEnt);
			if(CDgnElement::SWEEP_SURF == pDgnSurf->surf_type())
			{
				pDgnSurfElem = static_cast<CDgnSurfElement*>(pDgnSurf->GetSurfElemAt(0));
			}
		}
		else if(pEnt->IsKindOf(CDgnCellHeader::TypeString()))
		{
			CDgnCellHeader* pCell = static_cast<CDgnCellHeader*>(pEnt);
			for(int i = 0;i < int(pCell->GetEntityCount());++i)
			{
				CDgnElement* pCellElm = pCell->GetEntityAt(i);
				if(pCellElm->IsKindOf(CDgnLineString::TypeString()) && ((iGratingLevel == pCellElm->level()) || (iConcreteLevel == pCellElm->level())))
				{
					CDgnLineString* pLineString3d = static_cast<CDgnLineString*>(pCellElm);

					CaDraw_Platform *pPlatform = (CaDraw_Platform*)CaDraw_Platform::CreateInstance();
					if(pPlatform)
					{
						pPlatform->SetModelType(m_eModelType);
						pPlatform->SetModelName(*(m_pModelFilePath));
						pPlatform->SetVolume(volume);

						for(int i = 0;i < int(pLineString3d->GetVertexCount());++i)
						{
							DPoint3d pt = pLineString3d->GetVertexAt(i);
							pPlatform->Add(CIsPoint3d(pt.x/m_uor_per_subunit , pt.y/m_uor_per_subunit , pt.z/m_uor_per_subunit) + m_ptModelOffset);
						}
						m_pDraView->Add(pPlatform);
					}
					break;
				}
			}
		}

		if((NULL != pDgnSurfElem) && pDgnSurfElem->m_pBoundaryCurve && (2 == pDgnSurfElem->m_pBoundaryCurve->size()))
		{
			CDgnElement* pBoundaryElem = pDgnSurfElem->m_pBoundaryCurve->front();
			if(pBoundaryElem && pBoundaryElem->IsKindOf(CDgnLineString::TypeString()))
			{
				CDgnLineString* pLineString3d = static_cast<CDgnLineString*>(pBoundaryElem);

				CDgnElement* pRuleElem = pDgnSurfElem->m_pRuleCurve->front();
				if(pRuleElem && pRuleElem->IsKindOf(CDgnLine::TypeString()))
				{
					CDgnLine *pLine = static_cast<CDgnLine*>(pRuleElem);
					const DPoint3d start = pLine->GetStartPoint();
					const DPoint3d end = pLine->GetEndPoint();
					CIsVect3d dir = CIsPoint3d(end.x , end.y , end.z) - CIsPoint3d(start.x , start.y , start.z);

					CaDraw_Platform *pPlatform = (CaDraw_Platform*)CaDraw_Platform::CreateInstance();
					if(pPlatform)
					{
						pPlatform->SetModelType(m_eModelType);
						pPlatform->SetModelName(*(m_pModelFilePath));
						pPlatform->SetVolume(volume);
						
						for(int i = 0;i < int(pLineString3d->GetVertexCount());++i)
						{
							DPoint3d pt = pLineString3d->GetVertexAt(i);
							pPlatform->Add(CIsPoint3d(pt.x/m_uor_per_subunit , pt.y/m_uor_per_subunit , pt.z/m_uor_per_subunit) + m_ptModelOffset);
						}
						pPlatform->SetHeight(dir.Length());
						dir.Normalize();
						pPlatform->SetDir(dir);

						m_pDraView->Add(pPlatform);
					}
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
