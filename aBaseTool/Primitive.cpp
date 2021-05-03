#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SQLib.h"
#include <graphics\ITKGeo.h>

/**	\brief	The NewPrimitive function\n
	create new primitive. setup primitive's contents.

	\remarks\n
	initial dimension is 3D.

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE NewPrimitive(){
	PSQPRIMITIVE primitive = NULL;

	if((primitive = (PSQPRIMITIVE)malloc(sizeof(SQPRIMITIVE))) != NULL){
		memset(primitive,0,sizeof(SQPRIMITIVE));
	}

	return primitive;
}

//	parameter	:
//	description	: delete primitive.
//	remarks		:
//	returns		: 
void DeletePrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	if(pPrimitive){
		int id = pPrimitive->hdr.id;
		
		if(SQ_POINTS == id){
			delete pPrimitive->body.pvtPoints;
			delete pPrimitive;
		}else if(SQ_SOLID == id){
			PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
			for(ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
				ptrNext = ptr->next;
				DeletePrimitive(ptr);
				ptr = ptrNext;
			}
		}else if(ENM_GROUP == id){
			PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
			for(ptr = pPrimitive->body.pPrimitive;ptr;ptr = ptr->next){
				ptrNext = ptr->next;
				DeletePrimitive(ptr);
				ptr = ptrNext;
			}
		}else{
			delete pPrimitive;
		}
	}
}

//	parameter	:
//	description	: initialize pPrimitive with initial condition.
//	remarks		:
//	returns		: 
void InitPrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive){
		memset(pPrimitive,0,sizeof(SQPRIMITIVE));

		//pPrimitive->hdr.prefix.nSubUnit = 1;
		pPrimitive->hdr.attr.f   = 1;
		pPrimitive->hdr.attr.d   = 3;
	}
}

//	parameter	:
//	description	: 
//	remarks		:
//	returns		: 
int GetPrimitiveType(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive) return pPrimitive->hdr.id;
	
	return SQ_NULL;
}

//	parameter	:
//	description	: 
//	remarks		:
//	returns		: 
bool IsSupportedType(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet = false;

	if(pPrimitive) bRet = pPrimitive->hdr.attr.f;
	
	return bRet;
}

//	parameter	:
//	description	: 
//	remarks		:
//	returns		: 
bool ExistUserData(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet = false;

	if(pPrimitive) bRet = pPrimitive->hdr.attr.u;
	
	return bRet;
}

//	parameter	:
//	description	: append append-primitive to primitive list.
//	remarks		:
//	returns		: return primitive list's header.
PSQPRIMITIVE AppendPrimitive(PSQPRIMITIVE primitive,PSQPRIMITIVE append){
	assert(append);

	if(primitive && append){
		PSQPRIMITIVE p = NULL;

		for(p = primitive;p && p->next;p = p->next);
		if(p){
			p->next      = append;
			append->next = NULL;
		}
	}else if(append){
		append->next = NULL;
		return append;
	}

	return primitive;
}


//	parameter	:
//	description	:
//	remarks		:
//	returns		:
PPOINT_TNODE PolylinePush(PPOINT_TNODE polyline,PPOINT_T point)
{
	assert(point);
	PPOINT_TNODE p = NULL;

	if(polyline && point){
		for(p = polyline;p && p->next;p = p->next);
		if(NULL != (p->next = (PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE)))){
			p->next->pt.x = point->x;
			p->next->pt.y = point->y;
			p->next->pt.z = point->z;

			return p;
		}
	}else if(point){
		if(NULL != (p = (PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE)))){
			p->pt.x = point->x;
			p->pt.y = point->y;
			p->pt.z = point->z;
		}

		return p;
	}

	return polyline;
}

/**	\brief	The ClippingPrimitiveByVolume function\n
	clipping pSrc by pVolume, and store to pDest

	\param	pDest	a parameter of type PSQPRIMITIVE
	\param	pSrc	a parameter of type PSQPRIMITIVE
	\param	pVolume	a parameter of type PSQVOLUME

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE ClippingPrimitiveByVolume(PSQPRIMITIVE pDest,PSQPRIMITIVE pSrc,PSQVOLUME pVolume){
	assert(pSrc    && "source is invalid");
	assert(pVolume && "volume is invaid");
	POINT_T ret[8],pt={0};
	LINE_T  line={0};

	if(pSrc && pVolume){
		switch(pSrc->hdr.id){
			case ENM_LINE:{
				PSQPRIMITIVE pLine = NULL;

				line.ptStart.x = pSrc->body.line.ptStart.x;
				line.ptStart.y = pSrc->body.line.ptStart.x;
				line.ptStart.z = pSrc->body.line.ptStart.x;
				line.ptEnd.x = pSrc->body.line.ptEnd.x;
				line.ptEnd.y = pSrc->body.line.ptEnd.x;
				line.ptEnd.z = pSrc->body.line.ptEnd.x;
								
				ITKGeo::IntersectLineToVolume(&line,pVolume,ret);
				pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
				pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
				pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
				if(IsPointInVolume(&pt,pVolume)){			
					pSrc->body.line.ptStart.x = line.ptStart.x;
					pSrc->body.line.ptStart.y = line.ptStart.y;
					pSrc->body.line.ptStart.z = line.ptStart.z;
					
					pSrc->body.line.ptEnd.x = line.ptEnd.x;
					pSrc->body.line.ptEnd.y = line.ptEnd.y;
					pSrc->body.line.ptEnd.z = line.ptEnd.z;
					
					pDest = ListPush(pDest,pSrc);
				}
			}
			break;
			case SQ_POINTS:{
				PSQPRIMITIVE pPolyline = NULL;

				vector<POINT_T>::iterator itr;
				for(itr = pSrc->body.pvtPoints->begin();(itr + 1) != pSrc->body.pvtPoints->end();++itr){
					line.ptStart.x = itr->x;
					line.ptStart.y = itr->y;
					line.ptStart.z = itr->z;
					line.ptEnd.x = (itr + 1)->x;
					line.ptEnd.y = (itr + 1)->y;
					line.ptEnd.z = (itr + 1)->z;
					
					ITKGeo::IntersectLineToVolume(&line,pVolume,ret);
					pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
					pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
					pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
					if(IsPointInVolume(&pt,pVolume)){
						if(pPolyline == NULL){
							pPolyline = new SQPRIMITIVE;
							memset(pPolyline,0,sizeof(SQPRIMITIVE));
							pPolyline->body.pvtPoints = new vector<POINT_T>;
							
							pPolyline->hdr.id      = SQ_POINTS;
							if(pSrc->hdr.pLayer){
								int nLen=strlen(pSrc->hdr.pLayer);
								if(pPolyline->hdr.pLayer=(char*)calloc(1,nLen + 1)) strcpy(pPolyline->hdr.pLayer,pSrc->hdr.pLayer);
							}
							pPolyline->hdr.attr.f  = 1;
							pPolyline->hdr.handle  = pSrc->hdr.handle;
							pPolyline->pLinkageData= pSrc->pLinkageData;
							/*if(pSrc->pLinkageData){
								PDGNDMRS pitr,pDMRS;
								for(pitr = (PDGNDMRS)pSrc->pLinkageData;pitr;pitr = pitr->next){
									pDMRS = (PDGNDMRS)malloc(sizeof(DGNDMRS));
									memset(pDMRS,0,sizeof(DGNDMRS));
									memcpy(&(pDMRS->hdr),&(pitr->hdr),2);
									memcpy(&(pDMRS->UserID),&(pitr->UserID),2);
									if(pitr->hdr.u && pitr->hdr.wtf){
										pDMRS->data.pData = (char*)malloc(pitr->hdr.wtf*2 - 2);
										memcpy(pDMRS->data.pData,pitr->data.pData,pitr->hdr.wtf*2 - 2);
									}else	memcpy(&(pDMRS->data.occur),&(pitr->data.occur),4);
									pPolyline->pLinkageData = ListPush((PDGNDMRS)pPolyline->pLinkageData,pDMRS);
								}
							}
							*/
						}
						
						if(pPolyline && pPolyline->body.pvtPoints){
							pPolyline->body.pvtPoints->push_back(line.ptStart);
							pPolyline->body.pvtPoints->push_back(line.ptEnd);
						}
					}else if(pPolyline){
						vector<POINT_T>::iterator itr;
						for(itr = pPolyline->body.pvtPoints->begin();(itr != pPolyline->body.pvtPoints->end()) && ((itr+1) != pPolyline->body.pvtPoints->end());++itr){
							if((itr->x == (itr+1)->x) && (itr->y == (itr+1)->y) && (itr->z == (itr+1)->z)){
								pPolyline->body.pvtPoints->erase(itr--);
							}
						}
						pDest = ListPush(pDest,pPolyline);
						pPolyline = NULL;
					}
				}

				if(pPolyline){
					//vector<POINT_T>::iterator itr;
					//for(itr = pPolyline->body.pvtPoints->begin();(itr != pPolyline->body.pvtPoints->end()) && ((itr+1) != pPolyline->body.pvtPoints->end());++itr){
					//	if((itr->x == (itr+1)->x) && (itr->y == (itr+1)->y)){
					//		pPolyline->body.pvtPoints->erase(itr--);
					//	}
					//}
					pDest = ListPush(pDest,pPolyline);
				}
				delete pSrc;
			}
			break;
			case ENM_GROUP:{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;){
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
				}
			break;
			case SQ_SOLID:{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;){
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
				      }
			break;
		}
	}

	return pDest;
}

/*PSQPRIMITIVE ClippingPrimitiveByVolume(PSQPRIMITIVE pDest,PSQPRIMITIVE pSrc,PSQVOLUME pVolume){
	assert(pSrc    && "source is invalid");
	assert(pVolume && "volume is invaid");
	POINT_T ret[8],pt={0};
	LINE_T  line={0};

	if(pSrc && pVolume){
		switch(pSrc->hdr.prefix.id){
			case ENM_LINE:{
				PSQPRIMITIVE pLine = NULL;

				line.ptStart.x = pSrc->body.line.ptStart.x;
				line.ptStart.y = pSrc->body.line.ptStart.x;
				line.ptStart.z = pSrc->body.line.ptStart.x;
				line.ptEnd.x = pSrc->body.line.ptEnd.x;
				line.ptEnd.y = pSrc->body.line.ptEnd.x;
				line.ptEnd.z = pSrc->body.line.ptEnd.x;
								
				IntersectLineToVolume(&line,pVolume,ret);
				pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
				pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
				pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
				if(IsPointInVolume(&pt,pVolume)){			
					pSrc->body.line.ptStart.x = line.ptStart.x;
					pSrc->body.line.ptStart.y = line.ptStart.y;
					pSrc->body.line.ptStart.z = line.ptStart.z;
					
					pSrc->body.line.ptEnd.x = line.ptEnd.x;
					pSrc->body.line.ptEnd.y = line.ptEnd.y;
					pSrc->body.line.ptEnd.z = line.ptEnd.z;
					
					pDest = ListPush(pDest,pSrc);
				}
			}
			break;
			case SQ_POINTS:{
				PSQPRIMITIVE pPolyline = NULL;

				vector<POINT_T>::iterator itr;
				for(itr = pSrc->body.pvtPoints->begin();(itr + 1) != pSrc->body.pvtPoints->end();++itr){
					line.ptStart.x = itr->x;
					line.ptStart.y = itr->y;
					line.ptStart.z = itr->z;
					line.ptEnd.x = (itr + 1)->x;
					line.ptEnd.y = (itr + 1)->y;
					line.ptEnd.z = (itr + 1)->z;
					
					IntersectLineToVolume(&line,pVolume,ret);
					pt.x = (line.ptStart.x + line.ptEnd.x)/2.;
					pt.y = (line.ptStart.y + line.ptEnd.y)/2.;
					pt.z = (line.ptStart.z + line.ptEnd.z)/2.;
					if(IsPointInVolume(&pt,pVolume)){
						if(pPolyline == NULL){
							pPolyline = new SQPRIMITIVE;
							memset(pPolyline,0,sizeof(SQPRIMITIVE));
							pPolyline->body.pvtPoints = new vector<POINT_T>;
							
							pPolyline->hdr.prefix.id     = SQ_POINTS;
							pPolyline->hdr.prefix.layer  = pSrc->hdr.prefix.layer;
							pPolyline->hdr.prefix.type.f = 1;
							pPolyline->hdr.prefix.seq    = pSrc->hdr.prefix.seq;
							pPolyline->pLinkageData      = pSrc->pLinkageData;
							//if(pSrc->pLinkageData){
							//	PDGNDMRS pitr,pDMRS;
							//	for(pitr = (PDGNDMRS)pSrc->pLinkageData;pitr;pitr = pitr->next){
							//		pDMRS = (PDGNDMRS)malloc(sizeof(DGNDMRS));
							//		memset(pDMRS,0,sizeof(DGNDMRS));
							//		memcpy(&(pDMRS->hdr),&(pitr->hdr),2);
							//		memcpy(&(pDMRS->UserID),&(pitr->UserID),2);
							//		if(pitr->hdr.u && pitr->hdr.wtf){
							//			pDMRS->data.pData = (char*)malloc(pitr->hdr.wtf*2 - 2);
							//			memcpy(pDMRS->data.pData,pitr->data.pData,pitr->hdr.wtf*2 - 2);
							//		}else	memcpy(&(pDMRS->data.occur),&(pitr->data.occur),4);
							//		pPolyline->pLinkageData = ListPush((PDGNDMRS)pPolyline->pLinkageData,pDMRS);
							//	}
							//}
							//
						}
						
						if(pPolyline && pPolyline->body.pvtPoints){
							pPolyline->body.pvtPoints->push_back(line.ptStart);
							pPolyline->body.pvtPoints->push_back(line.ptEnd);
						}
					}else if(pPolyline){
						//vector<POINT_T>::iterator itr;
						//for(itr = pPolyline->body.pvtPoints->begin();(itr != pPolyline->body.pvtPoints->end()) && ((itr+1) != pPolyline->body.pvtPoints->end());++itr){
						//	if((itr->x == (itr+1)->x) && (itr->y == (itr+1)->y)){
						//		pPolyline->body.pvtPoints->erase(itr--);
						//	}
						//}
						pDest = ListPush(pDest,pPolyline);
						pPolyline = NULL;
					}
				}

				if(pPolyline){
					//vector<POINT_T>::iterator itr;
					//for(itr = pPolyline->body.pvtPoints->begin();(itr != pPolyline->body.pvtPoints->end()) && ((itr+1) != pPolyline->body.pvtPoints->end());++itr){
					//	if((itr->x == (itr+1)->x) && (itr->y == (itr+1)->y)){
					//		pPolyline->body.pvtPoints->erase(itr--);
					//	}
					//}
					pDest = ListPush(pDest,pPolyline);
				}
				delete pSrc;
			}
			break;
			case ENM_GROUP:{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;){
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
				}
			break;
			case SQ_SOLID:{
				PSQPRIMITIVE pElement = pSrc->body.pPrimitive;
				pSrc->body.pPrimitive = NULL;
				
				PSQPRIMITIVE ptr=NULL,ptrNext=NULL;
				for(ptr = pElement;ptr;){
					ptrNext = ptr->next;
					
					ptr->next = NULL;
					pSrc->body.pPrimitive = ClippingPrimitiveByVolume(pSrc->body.pPrimitive,ptr,pVolume);
					ptr = ptrNext;
				}
				
				if(pSrc && pSrc->body.pPrimitive)
					pDest = ListPush(pDest,pSrc);
				else	int i = 1;
				      }
			break;
		}
	}

	return pDest;
}
*/
