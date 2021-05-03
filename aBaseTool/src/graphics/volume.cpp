/********************************************************************
	created:	2001/08/03
	created:	3:8:2001   10:40
	filename: 	d:\Project\ITK\volume.cpp
	file path:	d:\Project\ITK
	file base:	volume
	file ext:	h
	author:		
	
	purpose:	
	update :
*********************************************************************/

#include <assert.h>
#include <graphics\volume.h>

//	parameter	:
//	description	: 
//	remarks		:
//	returns		:
bool IsPointInVolume(PPOINT_T pPoint,PVOLUME_T pVolume){
	assert(pPoint  && "pPoint is invalid");
	assert(pVolume && "pVolume is invalid");
	bool ret = false;

	if(     (pPoint->x >= pVolume->x[LOW]) && (pPoint->x <= pVolume->x[HIGH]) &&
		(pPoint->y >= pVolume->y[LOW]) && (pPoint->y <= pVolume->y[HIGH]) &&
		(pPoint->z >= pVolume->z[LOW]) && (pPoint->z <= pVolume->z[HIGH]))
		ret = true;

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool IsOuterVolume(const VOLUME_T& destvolume,const VOLUME_T& volume){
	bool ret = false;

	if((volume.x[LOW] > destvolume.x[HIGH]) || (volume.x[HIGH] < destvolume.x[LOW]) ||
	   (volume.y[LOW] > destvolume.y[HIGH]) || (volume.y[HIGH] < destvolume.y[LOW]) ||
	   (volume.z[LOW] > destvolume.z[HIGH]) || (volume.z[HIGH] < destvolume.z[LOW])){
		ret = true;
	}

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool IsInnerVolume(const VOLUME_T& destvolume,const VOLUME_T& volume){
	bool ret = false;

	if((volume.x[LOW] > destvolume.x[LOW]) && (volume.x[HIGH] < destvolume.x[HIGH]) &&
	   (volume.y[LOW] > destvolume.y[LOW]) && (volume.y[HIGH] < destvolume.y[HIGH]) &&
	   (volume.z[LOW] > destvolume.z[LOW]) && (volume.z[HIGH] < destvolume.z[HIGH])){
		ret = true;
	}

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool IsStraddleVolume(const VOLUME_T& destvolume,const VOLUME_T& volume){
	bool ret = false;
	
	if(!IsOuterVolume(destvolume,volume) && !IsInnerVolume(destvolume,volume)){
		ret = true;
	}

	return ret;
}
