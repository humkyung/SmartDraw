#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__	__declspec(dllexport)
#endif

#ifndef __EXPORT_FUNC_H__
#define __EXPORT_FUNC_H__

#include <SQLib.h>
#include <graphics\geometry.h>
#include "Enum.h"
#include "SmartHSR.h"

static void split_raw_line(PSQPRIMITIVE pPrim,CHSRFace* pFace){
	static double lx[4],ly[4],rx[4],ry[4];

	int count=0;
	lx[count]=pPrim->body.line.ptStart.x;ly[count]=pPrim->body.line.ptStart.y;
	rx[count]=pPrim->body.line.ptEnd.x;ry[count]=pPrim->body.line.ptEnd.y;
	count = 1;

	double x=0,y=0;
	for(int i=0;(i < count) && (i < 4);i++){
		for(HSR_VERTEX* ptr = pFace->pHead;(ptr && ptr->next);ptr = ptr->next){
			if(DO_INTERSECTION == CGeometry::IntersectLine2D(&x,&y,
				lx[i],ly[i],rx[i],ry[i],
				ptr->pt->x,ptr->pt->y,ptr->next->pt->x,ptr->next->pt->y,0)){
				if(!((ISEQUAL(x,lx[i]) && ISEQUAL(y,ly[i])) || (ISEQUAL(x,rx[i]) && ISEQUAL(y,ry[i])))){
					double tx=rx[i],ty=ry[i];
					rx[i] = x;ry[i] = y;
					lx[count] = x;ly[count] = y;
					rx[count] = tx;ry[count]= ty;
					i--;count++;
				}
			}
		}
	}
	if(1 == count) return;
	
	POINT_T pt={0,};
	for(int i = 0;i < count;i++)
	{
		pt.x = (lx[i] + rx[i])*0.5;pt.y = (ly[i] + ry[i])*0.5;
		if(pFace->IsInnerVertex(&pt)){
			pPrim->body.line.ptStart.x = lx[i];
			pPrim->body.line.ptStart.y = ly[i];
			pPrim->body.line.ptEnd.x = rx[i];
			pPrim->body.line.ptEnd.y = ry[i];
			break;
		}
	}
}

void hsr_add_raw_line(HSR_LAYER_E layer,PSQPRIMITIVE ptr,CHSRViewFormat* pHSRViewFormat)
{
	assert(ptr && "ptr is NULL");
	assert(pHSRViewFormat && "pHSRViewFormat is NULL");

	if(ptr){
		CHSRApp* pInst=CHSRApp::GetInstance();
		CHSRFace* pFace=pHSRViewFormat->GetViewFace();
		if(NULL != pFace) split_raw_line(ptr,pFace);
		
		POINT_T pt={0,};
		pt.x = (ptr->body.line.ptStart.x + ptr->body.line.ptEnd.x)*0.5;
		pt.y = (ptr->body.line.ptStart.y + ptr->body.line.ptEnd.y)*0.5;
		if(pFace->IsInnerVertex(&pt))
		{
			if(pInst->m_pExporter) pInst->m_pExporter->Add(layer,SHOW,ptr);
		}
	}
}

#endif