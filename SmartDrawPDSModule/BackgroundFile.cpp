#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <DgnV7Lib.h>
#include "BackgroundFile.h"

#include <fstream>
#include <map>
using namespace std;

CBackgroundFile::CBackgroundFile(void)
{
}

CBackgroundFile::~CBackgroundFile(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CBackgroundFile
    @function   WriteHidFile
    @return     void
    @param      LPCTSTR             pExportFilePath
    @param      list<CDgnElement*>& dgnElementList
    @param      const               double&
    @param      uor                 const
    @param      long&               nHiddenLevel
    @brief
******************************************************************************/
void CBackgroundFile::WriteHsrFile(LPCTSTR pExportFilePath , list<CDgnElement*>& dgnElementList , const double& uor , const unsigned int& nHiddenLevel)
{
	assert(pExportFilePath && "pExportFilePath is NULL");

	if(pExportFilePath)
	{
		OFSTREAM_T ofile(pExportFilePath);
		///TODO: 한글 문제...

		if(ofile.good())
		{
			ofile.precision( 5 );
			ofile.setf(ios_base:: fixed, ios_base:: floatfield);

			map<int , list<CDgnElement*>* > LevelOnPrimitiveMap;

			long count = 0L;
			for(list<CDgnElement*>::iterator itr = dgnElementList.begin();itr != dgnElementList.end();++itr)
			{
				const int level = (*itr)->level();
				map<int , list<CDgnElement*>* >::iterator where = LevelOnPrimitiveMap.find(level);
				if(where != LevelOnPrimitiveMap.end())
				{
					where->second->push_back(*itr);
				}
				else
				{
					list<CDgnElement*>* plst = new list<CDgnElement*>;
					if(plst)
					{
						plst->push_back(*itr);
						LevelOnPrimitiveMap.insert(make_pair(level , plst));
					}
				}
			}

			for(map<int , list<CDgnElement*>* >::iterator itr = LevelOnPrimitiveMap.begin();itr != LevelOnPrimitiveMap.end();++itr)
			{
				if(nHiddenLevel == itr->first) continue;

				ofile << "[" << (itr->first) << "]" << std::endl;
				ofile << itr->second->size() << std::endl;
				for(list<CDgnElement*>::iterator jtr = itr->second->begin(); jtr != itr->second->end(); ++jtr)
				{
					const unsigned int uiColorIndex = (unsigned int)((*jtr)->color());
					const UInt32 uiType = (*jtr)->elementType();
					switch(uiType)
					{
						case LINE_ELM:
						{
							CDgnLine* pLine = static_cast<CDgnLine*>(*jtr);
							DPoint3d start = pLine->GetStartPoint();
							DPoint3d end = pLine->GetEndPoint();

							ofile << _T("<LINE>|");
							ofile << (start.x/uor) << _T(",") << (start.y/uor) << _T(",") << (start.z/uor) << _T(",");
							ofile << (end.x/uor) << _T(",") << (end.y/uor) << _T(",") << (end.z/uor) << _T("|");
							ofile << itr->first << _T(",") << uiColorIndex << _T(",0,0,0") << std::endl;
						}
							break;
						case LINE_STRING_ELM: case DGNT_SHAPE:
						{
							CDgnLineString* pLineString = static_cast<CDgnLineString*>(*jtr);

							ofile << _T("<PLINE>|");
							for(int i = 0;i < int(pLineString->GetVertexCount());++i)
							{
								DPoint3d pt = pLineString->GetVertexAt(i);
								ofile << (pt.x/uor) << _T(",") << (pt.y/uor);/// << (pt.z/uor) << std::endl;
								if((i + 1) < int(pLineString->GetVertexCount()))
								{
									ofile << _T(",");
								}
								else
								{
									ofile << _T("|");
								}
							}
							ofile << itr->first << _T(",") << uiColorIndex << _T(",0,0,0") << std::endl;
						}
							break;
						case TEXT_ELM:
							CDgnText* pText = static_cast<CDgnText*>(*jtr);
							ofile << _T("<TEXT>|");
							ofile << (pText->origin().x / uor) << _T(",") << (pText->origin().y / uor) << _T(",") << (pText->origin().z / uor) << _T(",");
							ofile << (pText->GetTextHeight() / uor) << _T(",") << (pText->GetTextHeight() / uor) << _T(",");
							ofile << (pText->angle()) << _T(",") << pText->textString() << _T("Left Bottom,0|");
							ofile << itr->first << _T(",") << uiColorIndex << _T(",0,0,0") << std::endl;
							break;
					}
				}
			}
			ofile.close();

			for(map<int , list<CDgnElement*>* >::iterator itr = LevelOnPrimitiveMap.begin();itr != LevelOnPrimitiveMap.end();++itr)
			{
				delete (itr->second);
			}
			LevelOnPrimitiveMap.clear();
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CBackgroundFile
    @function   Generate
    @return     int
    @param      LPCTSTR pOutputFilePath
    @param      LPCTSTR pDrawFilePath
    @brief
******************************************************************************/
int CBackgroundFile::Generate(LPCTSTR pOutputFilePath , LPCTSTR pDrawFilePath , const long& lHiddenLevel)
{
	assert(pOutputFilePath && pDrawFilePath && "pOutputFilePath or pDrawFilePath is NULL");

	if(pOutputFilePath && pDrawFilePath)
	{
		CDgnFile dgnFile;
		CDgnParser parser(NULL);
		{
			parser.AddParsingType(LINE_ELM);
			parser.AddParsingType(LINE_STRING_ELM);
			parser.AddParsingType(POINT_STRING_ELM);
			parser.AddParsingType(DGNT_SHAPE);
			///parser.AddParsingType(TEXT_ELM);
		}
		if(true == dgnFile.Open(pDrawFilePath , CDgnFile::modeRead))
		{
			list<CDgnElement*> oDgnElementList;
			long uor = 80;
			for(;dgnFile.ReadElement();)
			{
				const int nType = dgnFile.GetType();
				if((TCB_ELM == nType) && (false == dgnFile.gotTCB()))
				{
					CDgnTCB* pTCB = (CDgnTCB*)parser.ParseElement(&dgnFile);
					uor = pTCB->uor_per_subunit;
				}
				else if((LINE_ELM == nType) || (LINE_STRING_ELM == nType) || (POINT_STRING_ELM == nType) || 
					(DGNT_SHAPE == nType)/* || (TEXT_ELM == nType)*/)
				{
					CDgnElement* pElm = parser.ParseElement(&dgnFile);
					if(pElm) oDgnElementList.push_back(pElm);
				}
			}

			this->WriteHsrFile(pOutputFilePath , oDgnElementList , double(uor) , lHiddenLevel);

			//! clear allocated dgn elements.
			for(list<CDgnElement*>::iterator itr = oDgnElementList.begin();itr != oDgnElementList.end();++itr)
			{
				CDgnElement::DeleteInstance(*itr);
			}
			oDgnElementList.clear();

			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CBackgroundFile
    @function   Generate
    @return     int
    @param      LPCTSTR         pOutputFilePath
    @param      CaDraw_DraFile* pDraFile
    @brief
******************************************************************************/
int CBackgroundFile::Generate(LPCTSTR pOutputFilePath , CaDraw_DraFile* pDraFile)
{
	return ERROR_SUCCESS;
}