#include "StdAfx.h"
#include <assert.h>
#include <util/ado/ADODB.h>
#include <util/SplitPath.h>
#include <Tokenizer.h>
#include <DgnDocument.h>
#include <DgnRefData.h>
#include "GenerateDraFromDrawing.h"
#include "../SmartDrawModule/aDraw_DraFile.h"
#include "../SmartDrawModule/aDraw_View.h"

#include <map>
using namespace std;

CGenerateDraFromDrawing::CGenerateDraFromDrawing(const double& dProjectUOR) : m_dProjectUOR(dProjectUOR)
{
}

CGenerateDraFromDrawing::~CGenerateDraFromDrawing(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawing
    @function   GetRotFrom
    @return     CaDraw_View::ROTATE_T
    @param      const   double&
    @param      dAngle
    @brief
******************************************************************************/
CaDraw_View::ROTATE_T CGenerateDraFromDrawing::GetRotFrom( const double& dAngle )
{
	CaDraw_View::ROTATE_T rot = CaDraw_View::R0_T;
	if(0 == dAngle)
	{
		rot = CaDraw_View::R0_T;
	}
	else if(90 == dAngle)
	{
		rot = CaDraw_View::R90_T;
	}
	else if(180 == dAngle)
	{
		rot = CaDraw_View::R180_T;
	}
	else if(270 == dAngle)
	{
		rot = CaDraw_View::R270_T;
	}

	return rot;
}

double RAD_DEG(const double& In_R)
{
	double res = 0.0;
    if(0 == sqrt(-In_R*In_R + 1))
	{
        res = 0;
	}
	else
	{
		res = atan(double(-In_R / sqrt(-In_R * In_R + 1))) + 2 * atan(1.0);
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawing
    @function   GetViewDirAndRot
    @return     int
    @param      CaDraw_View::DIRECTIONT_T&  dir
    @param      CaDraw_View::ROTATE_T&      rot
    @param      	const                      STRING_T&
    @param      sViewDirection              double*
    @param      pTransMatrix
    @brief
******************************************************************************/
int CGenerateDraFromDrawing::GetViewDirAndRot(CaDraw_View::DIRECTIONT_T& dir , CaDraw_View::ROTATE_T& rot , 
	const STRING_T& sViewDirection , double* pTransMatrix)
{
	assert(pTransMatrix && "pTransMatrix is NULL");

	if(pTransMatrix)
	{
		double dAngle = 0.0;
		if(_T("4") == sViewDirection)
		{
			dir = CaDraw_View::ISO1_VIEW_T;
			dAngle = 0.0;
		}
		else if((0 != pTransMatrix[0]) && (0 != pTransMatrix[5]) && (-1 == pTransMatrix[7]))	/// Front
		{
			dir = CaDraw_View::FRONT_VIEW_T;

			if((1 == pTransMatrix[0]) && (1 == pTransMatrix[5]))
			{
				dAngle = 0.0;
			}
			else if((1 == pTransMatrix[0]) || (-1 == pTransMatrix[0]))
			{
				dAngle = RAD_DEG(pTransMatrix[5]) * -1;
			}
			else
			{
				dAngle = RAD_DEG(pTransMatrix[0]) * -1;
			}

			if(0.0 == dAngle)
			{
				rot = CaDraw_View::R0_T;
			}
			else if(1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R90_T;
			}
			else if(-1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R270_T;
			}
			else if(3.2 == fabs(SAFE_ROUND(dAngle , 1)))
			{
				rot = CaDraw_View::R180_T;
			}
			else
			{
				rot = CaDraw_View::R0_T;
			}
		}
		else if((0 != pTransMatrix[0]) && (pTransMatrix[5] != 0) && (pTransMatrix[7] == 1)) ///BACK
		{
			dir = CaDraw_View::BACK_VIEW_T;

			if((pTransMatrix[0] == 1) && (pTransMatrix[5] == 1))
			{
				dAngle = 0.0;
			}
			else if((pTransMatrix[0] == 1) || (pTransMatrix[0] == -1))
			{
				dAngle = RAD_DEG(pTransMatrix[5]) * -1;
			}
			else
			{
				dAngle = RAD_DEG(pTransMatrix[0]) * -1;
			}

			if(0.0 == dAngle)
			{
				rot = CaDraw_View::R0_T;
			}
			else if(1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R90_T;
			}
			else if(-1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R270_T;
			}
			else if(3.2 == fabs(SAFE_ROUND(dAngle , 1)))
			{
				rot = CaDraw_View::R180_T;
			}
			else
			{
				rot = CaDraw_View::R0_T;
			}
		}
		else if((pTransMatrix[1] = 1) && (pTransMatrix[5] == 1) && (pTransMatrix[6] == 1))	/// Right
		{
			dir = CaDraw_View::RIGHT_VIEW_T;

			if((pTransMatrix[1] == 1) && (pTransMatrix[5] == 1))
			{
				dAngle = 0.0;
			}
			else if((pTransMatrix[1] == 1) || (pTransMatrix[1] == -1))
			{
				dAngle = RAD_DEG(pTransMatrix[5]) * -1;
			}
			else
			{
				dAngle = RAD_DEG(pTransMatrix[1]) * -1;
			}

			if(0.0 == dAngle)
			{
				rot = CaDraw_View::R0_T;
			}
			else if(1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R90_T;
			}
			else if(-1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R270_T;
			}
			else if(3.2 == fabs(SAFE_ROUND(dAngle , 1)))
			{
				rot = CaDraw_View::R180_T;
			}
			else
			{
				rot = CaDraw_View::R0_T;
			}
		}
		else if((pTransMatrix[1] = -1) && (pTransMatrix[5] != 0) && (pTransMatrix[6] == -1))	/// Left
		{
			dir = CaDraw_View::LEFT_VIEW_T;

			if((pTransMatrix[1] == 1) && (pTransMatrix[5] == 1))
			{
				dAngle = 0.0;
			}
			else if((pTransMatrix[1] == 1) || (pTransMatrix[1] == -1))
			{
				dAngle = RAD_DEG(pTransMatrix[5]) * -1;
			}
			else
			{
				dAngle = RAD_DEG(pTransMatrix[1]) * -1;
			}

			if(0.0 == dAngle)
			{
				rot = CaDraw_View::R0_T;
			}
			else if(1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R90_T;
			}
			else if(-1.6 == SAFE_ROUND(dAngle , 1))
			{
				rot = CaDraw_View::R270_T;
			}
			else if(3.2 == fabs(SAFE_ROUND(dAngle , 1)))
			{
				rot = CaDraw_View::R180_T;
			}
			else
			{
				rot = CaDraw_View::R0_T;
			}
		}
		else	/// TOP
		{
			dir = CaDraw_View::TOP_VIEW_T;

			const double x = pTransMatrix[0]*1.0 + pTransMatrix[1]*0.0;
			const double y = pTransMatrix[3]*1.0 + pTransMatrix[4]*0.0;

			if(1.0 == y)
			{
				rot = CaDraw_View::R90_T;
			}
			else if(-1.0 == x)
			{
				rot = CaDraw_View::R180_T;
			}
			else if(-1.0 == y)
			{
				rot = CaDraw_View::R270_T;
			}
			else
			{
				rot = CaDraw_View::R0_T;
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CGenerateDraFromDrawing
    @function   GetViewOrigin
    @return     CIsPoint3d
    @param      const                   CaDraw_View::DIRECTIONT_T&
    @param      dir                     const
    @param      CaDraw_View::ROTATE_T&  ro
    @param      												            const
    @param      CIsVolume&              vol
    @param      const                   CIsPoint3d
    @param      ptDestOrigin
    @brief
******************************************************************************/
CIsPoint3d CGenerateDraFromDrawing::GetViewOrigin(const CaDraw_View::DIRECTIONT_T& dir , const CaDraw_View::ROTATE_T& rot , 
												  const double& scale , const CIsVolume& vol , const CIsPoint3d ptDestOrigin)
{
	double x = 0.0 , y = 0.0;
	if((dir == CaDraw_View::TOP_VIEW_T) || (dir == CaDraw_View::BOTTOM_VIEW_T))
	{
		if((rot == CaDraw_View::R0_T) || (rot == CaDraw_View::R180_T))
		{
			x = ptDestOrigin.x() - (vol.maxx() - vol.minx()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxy() - vol.miny()) / scale * 0.5;
		}
		else
		{
			x = ptDestOrigin.x() - (vol.maxy() - vol.miny()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxx() - vol.minx()) / scale * 0.5;
		}
	}
	else if((dir == CaDraw_View::FRONT_VIEW_T) || (dir == CaDraw_View::BACK_VIEW_T))
	{
		if((rot == CaDraw_View::R0_T) || (rot == CaDraw_View::R180_T))
		{
			x = ptDestOrigin.x() - (vol.maxx() - vol.minx()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxz() - vol.minz()) / scale * 0.5;
		}
		else
		{
			x = ptDestOrigin.x() - (vol.maxz() - vol.minz()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxx() - vol.minx()) / scale * 0.5;
		}
	}
	else
	{
		if((rot == CaDraw_View::R0_T) || (rot == CaDraw_View::R180_T))
		{
			x = ptDestOrigin.x() - (vol.maxy() - vol.miny()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxz() - vol.minz()) / scale * 0.5;
		}
		else
		{
			x = ptDestOrigin.x() - (vol.maxz() - vol.minz()) / scale * 0.5;
			y = ptDestOrigin.y() - (vol.maxy() - vol.miny()) / scale * 0.5;
		}
	}

	return CIsPoint3d(x , y , 0.0);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   Generate
    @return     int
    @param      const           STRING_T&
    @param      sDraFilePath    const
    @param      STRING_T&       sDrawingFilePath
    @brief
******************************************************************************/
int CGenerateDraFromDrawing::Generate(const STRING_T& sOutDraFilePath , const STRING_T& sDrawingFilePath , const STRING_T& sDatabaseFilePath)
{
	CADODB adoDB;
	const CString rConnectionString = CString("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + sDatabaseFilePath.c_str();
	if(TRUE == adoDB.DBConnect(rConnectionString.operator LPCTSTR()))
	{
		map<int , CDgnRefData*> oDgnRefDataMap;
		auto_ptr<CDgnDocument> oDgnDoc(CDgnDocument::CreateInstance());
		oDgnDoc->Read(sDrawingFilePath.c_str() , CDgnDocument::SUBUNIT_BASE);
		const int iEntCount = oDgnDoc->GetEntityCount();
		for(int i = 0;i < iEntCount;++i)
		{
			CDgnElement* pDgnElm = oDgnDoc->GetEntityAt(i);
			if(pDgnElm->IsKindOf(CDgnRefData::TypeString()))
			{
				CDgnRefData* pDgnRefData = static_cast<CDgnRefData*>(pDgnElm);
				if('\0' != *(pDgnRefData->GetLogicalName()))
				{
					const int iDwgViewIndexNo = pDgnRefData->GetDwgViewIndexNo();
					map<int , CDgnRefData*>::iterator where = oDgnRefDataMap.find(iDwgViewIndexNo);
					if(where == oDgnRefDataMap.end())
					{
						oDgnRefDataMap.insert(make_pair(iDwgViewIndexNo , pDgnRefData));
						/*const CIsPoint3d ptRefOrigin = pDgnRefData->GetRefOrigin();
						const CIsPoint3d ptDestOrigin= pDgnRefData->GetDestOrigin();
						double* pTransMatrix = pDgnRefData->GetTransMatrix();*/
					}
				}
			}
		}

		auto_ptr<CaDraw_DraFile> pDraFile(new CaDraw_DraFile(sOutDraFilePath));
		pDraFile->SetSettingFileName(_T("Default"));

		CSplitPath path(sDrawingFilePath.c_str());
		CString sFileName = path.GetFileName() + path.GetExtension();
		STRING_T sSql = _T("select dwg_view_index_no,dwg_view_scale,dwg_view_x_low,dwg_view_y_low,dwg_view_z_low,dwg_view_x_high,dwg_view_y_high,dwg_view_z_high,dwg_view_no,viewing_direction from pdtable_122 where dwg_index_no=(select dwg_index_no from pdtable_121 where drawing_file_spec='") + sFileName + _T("')");
		if(TRUE == adoDB.OpenQuery(sSql))
		{
			long lRecordCount = 0L;
			adoDB.GetRecordCount(&lRecordCount);
			for(int i = 0;i < lRecordCount;++i)
			{
				STRING_T sValue;
				
				adoDB.GetFieldValue(i , _T("dwg_view_index_no") , &sValue);
				const STRING_T sViewIndexNo = sValue;
				adoDB.GetFieldValue(i , _T("dwg_view_scale") , &sValue);
				double scale = 1.0;
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , sValue , CIsFromString(_T(":")));
				if(2 == oResult.size())
				{
					scale = ATOF_T(oResult[1].c_str());
				}
				adoDB.GetFieldValue(i , _T("dwg_view_x_low") , &sValue);
				const double minx = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_y_low") , &sValue);
				const double miny = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_z_low") , &sValue);
				const double minz = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_x_high") , &sValue);
				const double maxx = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_y_high") , &sValue);
				const double maxy = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_z_high") , &sValue);
				const double maxz = ATOF_T(sValue.c_str()) / m_dProjectUOR;
				adoDB.GetFieldValue(i , _T("dwg_view_no") , &sValue);
				adoDB.GetFieldValue(i , _T("viewing_direction") , &sValue);
				const STRING_T sViewDir = sValue;
				
				CaDraw_View* pDraView = pDraFile->AddView(sViewIndexNo);
				if(pDraView)
				{
					pDraView->SetScale(scale);
					pDraView->SetVolume(CIsVolume(minx , miny , minz , maxx , maxy , maxz));

					map<int , CDgnRefData*>::iterator where = oDgnRefDataMap.find(ATOI_T(sViewIndexNo.c_str()));
					if(where != oDgnRefDataMap.end())
					{
						double* pTransMatrix = where->second->GetTransMatrix();

						CaDraw_View::DIRECTIONT_T dir;
						CaDraw_View::ROTATE_T rot;
						this->GetViewDirAndRot(dir , rot , sViewDir , pTransMatrix);
						pDraView->SetViewDir(dir);
						pDraView->SetViewAngle(rot);

						const CIsVolume vol = pDraView->GetVolume();
						const CIsPoint3d ptDestOrigin= where->second->GetDestOrigin();
						CIsPoint3d origin = this->GetViewOrigin(dir , rot , scale , vol , ptDestOrigin);
						pDraView->SetOrigin(origin);
					}
				}
			}

			list<CaDraw_View*> oViewList;
			pDraFile->GetViewList(&oViewList);
			for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
			{
				list<CaDraw_View::MODEL_T> oRefModelList;

				sSql = _T("select discipline_indx_no,model_file_spec from pdtable_113 where model_index_no in (select model_index_no from pdtable_123 where dwg_view_index_no = '") + CString((*itr)->name()) + _T("')");
				if(TRUE == adoDB.OpenQuery(sSql))
				{
					lRecordCount = 0L;
					adoDB.GetRecordCount(&lRecordCount);
					for(int i = 0;i < lRecordCount;++i)
					{
						CaDraw_View::MODEL_T model;

						STRING_T sValue;
						adoDB.GetFieldValue(i , _T("discipline_indx_no") , &sValue);
						if(_T("1") == sValue)		/// pipe
						{
							model.modelType = CaDraw_Entity::PIPE_MODEL_T;
						}
						else if(_T("2") == sValue)	/// equipment
						{
							model.modelType = CaDraw_Entity::EQUIPMENT_MODEL_T;
						}
						else if(_T("3") == sValue)	/// structure
						{
							model.modelType = CaDraw_Entity::STRUCTURE_MODEL_T;
						}
						else if(_T("5") == sValue)	/// CableT
						{
							model.modelType = CaDraw_Entity::CABLE_T_MODEL_T;
						}
						else
						{
							model.modelType = CaDraw_Entity::MISC_MODEL_T;
						}

						adoDB.GetFieldValue(i , _T("model_file_spec") , &sValue);
						model.rModelName = sValue;

						oRefModelList.push_back(model);
					}
				}

				(*itr)->SetModelList(&oRefModelList);
			}
		}

		pDraFile->Save();

	}
	else
	{
		AfxMessageBox(_T("Can't connect to database") , MB_OK);
	}

	return ERROR_SUCCESS;
}