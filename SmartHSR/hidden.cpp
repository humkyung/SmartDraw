#include <assert.h>
#include "hidden.h"

bool IsEqualPoint(POINT_T pt1,POINT_T pt2){
	bool   ret = false;
	double dx,dy,dz;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if((fabs(dx) <= 0.001) && (fabs(dy) <= 0.001)) ret = true;

	return ret;
}

//	parameter	:
//	description	: if num1 and num2 is closer, assume two variable is equal.
//	remarks		:
//	returns		:
bool IsEqualNumber(double num1,double num2){
	bool   ret = false;
	double dnum= num2 - num1; 

	if(fabs(dnum) <= 0.001) ret = true;

	return ret;
}

//	parameter	:
//	description	: append pValue to pFace's tail
//	remarks		: set face to HIDDEN or INVALID if from view point exits other faces with co-axis.(face is HSR_CIRCLE_SHAPE)
//	returns		:
CHSRFace* AppendFace(CHSRFace* pFace,CHSRFace* pValue){
	assert(pValue);

	if(pFace && pValue){
		CHSRFace *pFaceNode = pFace;
		PHSR_VERTEX ptr = NULL;
		
		for(pFaceNode = pFace;pFaceNode && pFaceNode->next;pFaceNode = pFaceNode->next){
			if(pFaceNode->IsPerpendicularToViewPoint() && pValue->IsPerpendicularToViewPoint()){
				if((HSR_CIRCLE_SHAPE == pFaceNode->ele) && (HSR_CIRCLE_SHAPE == pValue->ele)){
					if(IsEqualPoint(pFaceNode->ptCenter,pValue->ptCenter)){
						if(pValue->ptCenter.z < pFaceNode->ptCenter.z){
							if((pValue->radius - pFaceNode->radius) < -0.001){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}else if((pValue->radius - pFaceNode->radius >= -0.001) && (pValue->radius - pFaceNode->radius <= 0.001)){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							}
						}else if(pValue->ptCenter.z > pFaceNode->ptCenter.z){
							if((pValue->radius - pFaceNode->radius) > 0.001){
								for(ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}else if((pValue->radius - pFaceNode->radius >= -0.001) && (pValue->radius - pFaceNode->radius <= 0.001)){
							    	for(ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							}
						}else if(IsEqualNumber(pValue->ptCenter.z,pFaceNode->ptCenter.z)){
							if(IsEqualNumber(pValue->radius,pFaceNode->radius)){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}
						}
					}
				}else if((SYMBOL == pFaceNode->ele) && (SYMBOL == pValue->ele)){
					if(IsEqualPoint(pFaceNode->ptCenter,pValue->ptCenter)){
						if(pValue->ptCenter.z < pFaceNode->ptCenter.z){
							if((pValue->radius - pFaceNode->radius) < -0.001){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}else if((pValue->radius - pFaceNode->radius >= -0.001) && (pValue->radius - pFaceNode->radius <= 0.001)){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							}
						}else if(pValue->ptCenter.z > pFaceNode->ptCenter.z){
							if((pValue->radius - pFaceNode->radius) > 0.001){
								for(ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}else if((pValue->radius - pFaceNode->radius >= -0.001) && (pValue->radius - pFaceNode->radius <= 0.001)){
							    	for(ptr = pFaceNode->pHead;ptr;ptr = ptr->next) ptr->render = INVALID;
							}
						}else if(IsEqualNumber(pValue->ptCenter.z,pFaceNode->ptCenter.z)){
							if(IsEqualNumber(pValue->radius,pFaceNode->radius)){
								for(ptr = pValue->pHead;ptr;ptr = ptr->next) ptr->render = HIDE;
							}
						}
					}
				}
			}
		}
		pFaceNode->next = pValue;
	}else if((pFace == NULL) && pValue){
		pFace = pValue;

		return pValue;
	}

	return pFace;
}
