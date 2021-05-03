#include <stdio.h>
#include "queue.h"
#include "KMesh.h"
#include "KTriangle.h"

KMesh::KMesh(int nShape,KArray<SQPOINT>** paryShape,int nBoundary,KArray<SQPOINT>** paryBoundary){
    	m_nShape       = nShape;
	m_nBoundary    = nBoundary;
	m_paryShape    = paryShape;
	m_paryBoundary = paryBoundary;
}

KMesh::~KMesh(){
}

list<KTriangle>* KMesh::Generate(){
    	SQPOINT pt={0};
    	EDGE  edge={0};
	CQueue<EDGE> queEdge;

	list<KTriangle> lstTriangle;
    	if(m_paryBoundary){
	    	double minlen=-1,len=0;

	    	edge.ptStart = (*m_paryBoundary[0])[0];
		edge.ptEnd   = (*m_paryBoundary[0])[1];
		for(int i=0;i < m_nShape;i++){
		    	for(int j=0;j < m_paryShape[i]->Length();j++){
				if(!IsPointOnEdge(edge,(*m_paryShape[i])[j])){
			    		len = GetLength(edge,(*m_paryShape[j])[i]);
					if(minlen < 0){
				    		minlen = len;
						pt = (*m_paryShape[i])[j];
					}else if(len < minlen){
				    		minlen = len;
						pt = (*m_paryShape[i])[j];
					}
				}
			}
		}
	}else{
		bool bFound=false;
	    	double minlen=-1,len=0;

	    	edge.ptStart = (*m_paryShape[0])[0];
		edge.ptEnd   = (*m_paryShape[0])[1];
		for(int i=0;i < m_nShape;i++){
			int nLength = m_paryShape[i]->Length();
		    	for(int j=0;j < nLength;j++){
				if(!IsPointsOnOneBoundary(edge.ptStart,edge.ptEnd,(*m_paryShape[i])[j])){
			    		len = GetLength(edge,(*m_paryShape[i])[j]);
					if(minlen < 0){
				    		minlen = len;
						pt = (*m_paryShape[i])[j];
						bFound = true;
					}else if(len < minlen){
				    		minlen = len;
						pt = (*m_paryShape[i])[j];
						bFound = true;
					}
				}
			}
		}

		if(bFound){
			KTriangle triangle(edge.ptStart,edge.ptEnd,pt);
			lstTriangle.push_back(triangle);

			//queEdge.Push(edge);
			EDGE NewEdge={0};
			NewEdge.ptStart = pt;
			NewEdge.ptEnd   = edge.ptStart;
			queEdge.Push(NewEdge);
			NewEdge.ptEnd   = edge.ptEnd;
			queEdge.Push(NewEdge);
		}
	}

	while(!queEdge.IsEmpty()){
	    	queEdge.Pop(edge);
		if(GetMeshTrianglePoint(pt,edge,lstTriangle)){
		    	KTriangle triangle(edge.ptStart,edge.ptEnd,pt);
			lstTriangle.push_back(triangle);

			EDGE NewEdge={0};
		    	NewEdge.ptStart = pt;
			NewEdge.ptEnd   = edge.ptStart;	
			if(!IsBoundaryEdge(NewEdge)){
			    	queEdge.Push(NewEdge);
			}

			NewEdge.ptStart = pt;
			NewEdge.ptEnd   = edge.ptEnd;
			if(!IsBoundaryEdge(NewEdge)){
			    	queEdge.Push(NewEdge);
			}
		}
	}

	FILE* fp=NULL;
	if(fp = fopen("d:\\triangle.txt","wb")){
		list<KTriangle>::iterator itr;
		for(itr = lstTriangle.begin();itr != lstTriangle.end();++itr){
			fprintf(fp,"FACE : \r\n");
			fprintf(fp,"%lf,%lf,%lf,\r\n",(*itr)[0].x,(*itr)[0].y,(*itr)[0].z);
			fprintf(fp,"%lf,%lf,%lf,\r\n",(*itr)[1].x,(*itr)[1].y,(*itr)[1].z);
			fprintf(fp,"%lf,%lf,%lf,\r\n",(*itr)[2].x,(*itr)[2].y,(*itr)[2].z);
			fprintf(fp,"\r\n");
		}
		fclose(fp);
	}

	return &lstTriangle;
}

bool KMesh::GetMeshTrianglePoint(SQPOINT& pt,EDGE& edge,list<KTriangle>& lstTriangle){
    	bool bRet=false;
	double minlen=-1,len=0;

	SQPOINT ptEdge={0};
    	for(int i=0;i < m_nShape;i++){
    		for(int j=0;j < m_paryShape[i]->Length();j++){
			ptEdge=(*m_paryShape[i])[j];
			if(!IsPointOnEdge(edge,ptEdge) && !IsPointsOnOneBoundary(edge.ptStart,edge.ptEnd,ptEdge)){
				len=GetLength(edge,ptEdge);
				if(minlen < 0){
			    		KTriangle triangle(edge.ptStart,edge.ptEnd,(*m_paryShape[i])[j]);
					if(!IsExistEqualOrStraddleTriangle(lstTriangle,triangle)){
						minlen = len;
						pt = (*m_paryShape[i])[j];
						bRet = true;
					}
				}else if(len < minlen){
					KTriangle triangle(edge.ptStart,edge.ptEnd,(*m_paryShape[i])[j]);
			    		if(!IsExistEqualOrStraddleTriangle(lstTriangle,triangle)){
			    			minlen = len;
						pt = (*m_paryShape[i])[j];
						bRet = true;
					}
				}
			}
		}
	}
	
	for(i=0;i < m_nBoundary;i++){
    		for(int j=0;j < m_paryBoundary[i]->Length();j++){
			if(!IsPointOnEdge(edge,(*m_paryBoundary[i])[j]) && !IsPointsOnOneBoundary(edge.ptStart,edge.ptEnd,(*m_paryBoundary[i])[j])){
		    		len=GetLength(edge,(*m_paryBoundary[i])[j]);
				if(minlen < 0){
			    		KTriangle triangle(edge.ptStart,edge.ptEnd,(*m_paryBoundary[i])[j]);
					if(!IsExistEqualOrStraddleTriangle(lstTriangle,triangle)){
						minlen = len;
						pt = (*m_paryBoundary[i])[j];
					}
				}else if(len < minlen){
					KTriangle triangle(edge.ptStart,edge.ptEnd,(*m_paryBoundary[i])[j]);
			    		if(!IsExistEqualOrStraddleTriangle(lstTriangle,triangle)){
			    			minlen = len;
						pt = (*m_paryBoundary[i])[i];
					}
				}
			}
		}
	}

	return bRet;
}

double KMesh::GetLength(EDGE& edge,SQPOINT& pt){
    	double len=0.;

	double dx=edge.ptStart.x - pt.x;
	double dy=edge.ptStart.y - pt.y;
	double dz=edge.ptStart.z - pt.z;

	len = dx*dx + dy*dy + dz*dz;

	dx = edge.ptEnd.x - pt.x;
	dy = edge.ptEnd.y - pt.y;
	dz = edge.ptEnd.z - pt.z;

	len+= dx*dx + dy*dy + dz*dz;

	return len;
}

bool KMesh::IsExistEqualOrStraddleTriangle(list<KTriangle>& lstTriangle,KTriangle& triangle){
    	list<KTriangle>::iterator itr;
    	for(itr = lstTriangle.begin();itr != lstTriangle.end();++itr){
	    	if(((*itr) == triangle) || itr->IsStraddle(triangle)){
		    	return true;
		}
	}

	return false;
}

bool KMesh::IsBoundaryEdge(EDGE& edge){
    	bool bStart=false,bEnd=false;

	for(int i=0;i < m_nShape;i++){
		bStart = false;
		bEnd   = false;
    		for(int j=0;j < m_paryShape[i]->Length();j++){
		    	if(((*m_paryShape[i])[j].x == edge.ptStart.x) && ((*m_paryShape[i])[j].y == edge.ptStart.y) && ((*m_paryShape[i])[j].z == edge.ptStart.z)) bStart=true;
			if(((*m_paryShape[i])[j].x == edge.ptEnd.x) && ((*m_paryShape[i])[j].y == edge.ptEnd.y) && ((*m_paryShape[i])[j].z == edge.ptEnd.z)) bEnd=true;
			if(bStart && bEnd) return true;
		}
	}
	
	bStart=false;
	bEnd  =false;
	for(i=0;i < m_nBoundary;i++){
    		for(int j=0;j < m_paryBoundary[i]->Length();j++){
			if(((*m_paryBoundary[i])[j].x == edge.ptStart.x) && ((*m_paryBoundary[i])[j].y == edge.ptStart.y) && ((*m_paryBoundary[i])[j].z == edge.ptStart.z)) bStart=true;
			if(((*m_paryBoundary[i])[j].x == edge.ptEnd.x) && ((*m_paryBoundary[i])[j].y == edge.ptEnd.y) && ((*m_paryBoundary[i])[j].z == edge.ptEnd.z)) bEnd=true;
			if(bStart && bEnd) return true;

		}
	}

	return false;
}

bool KMesh::IsPointsOnOneBoundary(SQPOINT& pt1,SQPOINT& pt2,SQPOINT& pt3){
    	bool bFirst=false,bSecond=false,bThird=false;

	for(int i=0;i < m_nShape;i++){
		bFirst = false;
		bSecond= false;
		bThird = false;
    		for(int j=0;j < m_paryShape[i]->Length();j++){
		    	if(((*m_paryShape[i])[j].x == pt1.x) && ((*m_paryShape[i])[j].y == pt1.y) && ((*m_paryShape[i])[j].z == pt1.z)) bFirst=true;
			if(((*m_paryShape[i])[j].x == pt2.x) && ((*m_paryShape[i])[j].y == pt2.y) && ((*m_paryShape[i])[j].z == pt2.z)) bSecond=true;
			if(((*m_paryShape[i])[j].x == pt3.x) && ((*m_paryShape[i])[j].y == pt3.y) && ((*m_paryShape[i])[j].z == pt3.z)) bThird=true;
			if(bFirst && bSecond && bThird) return true;
		}
	}
	
	bFirst =false;
	bSecond=false;
	bThird =false;
	for(i=0;i < m_nBoundary;i++){
		bFirst = false;
		bSecond= false;
		bThird = false;
    		for(int j=0;j < m_paryBoundary[i]->Length();j++){
			if(((*m_paryShape[i])[j].x == pt1.x) && ((*m_paryShape[i])[j].y == pt1.y) && ((*m_paryShape[i])[j].z == pt1.z)) bFirst=true;
			if(((*m_paryShape[i])[j].x == pt2.x) && ((*m_paryShape[i])[j].y == pt2.y) && ((*m_paryShape[i])[j].z == pt2.z)) bSecond=true;
			if(((*m_paryShape[i])[j].x == pt3.x) && ((*m_paryShape[i])[j].y == pt3.y) && ((*m_paryShape[i])[j].z == pt3.z)) bThird=true;
			if(bFirst && bSecond && bThird) return true;
		}
	}

	return false;
}

bool KMesh::IsPointOnEdge(EDGE& edge,SQPOINT& pt){
	bool bRet=false;

	if(	((edge.ptStart.x == pt.x) && 
		(edge.ptStart.y == pt.y) && 
		(edge.ptStart.z == pt.z)) ||
		((edge.ptEnd.x == pt.x) && 
		(edge.ptEnd.y == pt.y) && 
		(edge.ptEnd.z == pt.z))){
		bRet=true;
	}

	return bRet;
}