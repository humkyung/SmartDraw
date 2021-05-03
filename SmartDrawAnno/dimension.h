#pragma once

#include "SmartDrawAnnoItem.h"
#include "Enum.h"

/// dimension and annotation item.
typedef struct
{
	double nGap;
	double nDistA,nDistB,nDistC,nDistD;
	double nBaseLineOffset;
	int    level[4];
	bool   bFixEquipment;
}DIMOPT,* PEXTDIMOPT;
