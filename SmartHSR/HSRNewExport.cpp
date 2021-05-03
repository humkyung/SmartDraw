// KExport.cpp: implementation of the KExport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <SQLib.h>
#include <utility\iolib.h>
#include <graphics\geometry.h>

#include "SmartHSR.h"
#include "Geo.h"
#include "HSRView.h"
#include "HSRNewExport.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	
	@brief
*/
CHSRNewExport::CHSRNewExport() : CHSRExport()
{
	m_sVersion = _T("V2");
}

CHSRNewExport::~CHSRNewExport()
{
}

//! layer name
const static char* szLayer0_1[50]=
{
	"[PIPE_SINGLELINE]","[PIPE_SINGLELINE_HIDDEN]",
	"[PIPE_DOUBLELINE]","[PIPE_DOUBLELINE_HIDDEN]",
	"[PIPE_CENTERLINE]","[]",
	"[PIPE_OTHER]","[H_OTHER]",
	"[PIPE_MARK]","[]",
	"[ELBOW_SINGLELINE]","[ELBOW_SINGLELINE_HIDDEN]",
	"[ELBOW_DOUBLELINE]","[ELBOW_DOUBLELINE_HIDDEN]",
	"[ELBOW_CENTERLINE]","[]",
	"[CROSS_LINE]","[]",
	"[WELD_MARK]","[]",
	"[WELD_MARK]","[]",
	"[CIRCLE_MARK]","[]",
	"[TEE_SINGLELINE]","[TEE_SINGLELINE_HIDDEN]",
	"[TEE_DOUBLELINE]","[TEE_DOUBLELINE_HIDDEN]",
	"[TEE_CENTERLINE]","[]",
	"[OTHER_COMPONENT]","[OTHER_COMPONENT_HIDDEN]",
	"[EQUIPMENT]","[EQUIPMENT_HIDDEN]",
	"[EQUIPMENT_CENTER]","[]",
	"[STRUCTURE]","[STRUCTURE_HIDDEN]",
	"[STRUCTURE_CENTERLINE]","[]",
	"[CABLE]","[H_OTHER]",
	"[VIEW]","[]",
	"[DIMENSION]","[]",
	"[ANNOTATION]","[]",
	"[BORDER]","[]"
};

const static char* szLayer0_2[50]=
{
	"[S_PIPE]","[H_S_PIPE]",
	"[D_PIPE]","[H_D_PIPE]",
	"[C_PIPE]","[0]",
	"[PIPE_OTHER]","[H_OTHER]",
	"[PIPE_MARK]","[0]",
	"[S_ELBOW]","[H_S_ELBOW]",
	"[D_ELBOW]","[H_D_ELBOW]",
	"[C_ELBOW]","[0]",
	"[CROSS]","[0]",
	"[WELD_MARK]","[0]",
	"[WELD_MARK]","[0]",
	"[CIRCLE_MARK]","[0]",
	"[S_TEE]","[H_S_TEE]",
	"[D_TEE]","[H_D_TEE]",
	"[C_TEE]","[0]",
	"[OTHER]","[H_OTHER]",
	"[EQP]","[H_EQP]",
	"[C_EQP]","[0]",
	"[STR]","[H_STR]",
	"[C_STR]","[0]",
	"[CABLE]","[H_OTHER]",
	"[VIEW_LINE]","[0]",
	"[DIMENSION]","[0]",
	"[ANNOTATION]","[0]",
	"[BORDER]","[0]"
};

const static char* szLayer0_3[50]=
{
	"[S_PIPE]","[H_S_PIPE]",
	"[D_PIPE]","[H_D_PIPE]",
	"[C_PIPE]","[]",
	"[PIPE_OTHER]","[H_OTHER]",
	"[PIPE_MARK]","[]",
	"[S_ELBOW]","[H_S_ELBOW]",
	"[D_ELBOW]","[H_D_ELBOW]",
	"[C_ELBOW]","[]",
	"[CROSS]","[]",
	"[WELD_MARK]","[]",
	"[WELD_MARK]","[]",
	"[CIRCLE_MARK]","[]",
	"[S_TEE]","[H_S_TEE]",
	"[D_TEE]","[H_D_TEE]",
	"[C_TEE]","[]",
	"[OTHER]","[H_OTHER]",
	"[EQP]","[H_EQP]",
	"[C_EQP]","[]",
	"[STR]","[H_STR]",
	"[C_STR]","[]",
	"[CABLE]","[H_OTHER]",
	"[VIEW_LINE]","[]",
	"[DIMENSION]","[]",
	"[ANNOTATION]","[]",
	"[BORDER]","[]"
};

/**	
	@brief	write primitive to immediate file
	
	@author	HumKyung.Baek

	@param	pFileName	immediate file name

	@history	[2006.02.13] - add type to output format.
	@history	[2008.08.07] - output write format º¯°æ.

	@return	bool	
*/
bool CHSRNewExport::WritePrimitiveAsAsciiFile(const char *pFileName)
{
	return true;
}