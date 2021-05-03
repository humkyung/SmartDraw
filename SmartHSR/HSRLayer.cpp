// HSRLayer.cpp: implementation of the CHSRLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <exception>
#include "HSRLayer.h"

#include "HSRPipe.h"
#include "ElbowElement.h"
#include "TeeElement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRLayer::CHSRLayer()
{

}

CHSRLayer::~CHSRLayer()
{

}

/**
	@brief	return layer code
	@author	humkyung
	@date	?
	@param	rTypeString
	@param	model type
	@return HSR_LAYER_E 
 */
HSR_LAYER_E CHSRLayer::GetLayerCode(const STRING_T& rTypeString , const CHSRElement::MODEL_T& modelT , const STRING_T& sLayerName)
{
	static const STRING_T __LayerName[]={   _T("PIPE")    , _T("EQUP") , _T("STRU") , _T("CABLE") , _T("INSTRUMENT") , _T("OTHER") , _T("")};
	static const HSR_LAYER_E __LayerCode[]={LAYER_PIPE_DOU, LAYER_EQUI, LAYER_STRU , LAYER_CABLE , LAYER_INSTRUMENT , LAYER_OTHER , LAYER_OTHER};

	HSR_LAYER_E res = LAYER_NONE;
	if(!sLayerName.empty())
	{
		const int nSiz=sizeof(__LayerName)/sizeof(__LayerName[0]);
		const STRING_T* p = find(__LayerName , __LayerName + nSiz , sLayerName);
		const int at = (p - __LayerName);
		res = (at < nSiz) ? __LayerCode[at] : LAYER_NONE;
	}
	else
	{
		switch(modelT.modelType)
		{
			case CHSRElement::PIPE_MODEL_T:
				res = LAYER_PIPE_DOU;
				if(((rTypeString != CHSRPipe::TypeString()) && (rTypeString != CElbowElement::TypeString()) && (rTypeString != CTeeElement::TypeString())))
				{
					LAYER_PIPE_OTHER;
				}
				break;
			case CHSRElement::EQUIPMENT_MODEL_T:
				res = LAYER_EQUI;
				break;
			case CHSRElement::STRUCTURE_MODEL_T:
				res = LAYER_STRU;
				break;
			case CHSRElement::CABLE_T_MODEL_T:
				res = LAYER_CABLE;
				break;
			case CHSRElement::MISC_MODEL_T:
				res = LAYER_OTHER;
				break;
		}
	}
	if(LAYER_NONE == res)
	{
		int d = 1;
	}

	return res;
}
