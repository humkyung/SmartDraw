#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "..\include\Intent.h"

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
Value* GetArg(ArgList *args,int nIndex){
	if(args && (nIndex >= 0 && nIndex < args->length)) return ((args->vals) + nIndex);
	
	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
void FreeArgList(ArgList *args){
	if(args){
		free((Value *)args->vals);
		free((ArgList *)args);
	}
}

//	parameter	: 
//	description	: memory allocation for DList
//	remrks		: DList's all members are setted 0
//	returns		: DList pointer
DList* NewDListNode(){
	PDLIST pListNode = NULL;
	
	if((pListNode = (PDLIST)malloc(sizeof(DList))) != NULL){
		memset(pListNode,0,sizeof(DList));
	}
	
	return pListNode;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
void FreeDListNode(DList *pListNode){
	if(pListNode) free((DList *)pListNode);
}

//	parameter	: pList - dest list , pValue - new value
//	description	: append pValue to pList
//	remrks		: 
//	returns		: start list node of pList;
DList* PushValueToDList(DList *pList,Value *pValue){
	if(pList && pValue){
		DList *pListNode = pList;
		for(pListNode = pList;pListNode && pListNode->val.type != df_null && pListNode->next;pListNode = pListNode->next);
		if(pListNode->val.type != df_null && pListNode->next == NULL){
			pListNode->next = NewDListNode();
			if(pListNode->next){
				memcpy(&(pListNode->next->val),pValue,sizeof(Value));

				if(pListNode->next->val.type == df_point){
					pListNode->next->val.data.point = (PPOINT_T)malloc(sizeof(POINT_T));
					if(pListNode->next->val.data.point) memcpy(pListNode->next->val.data.point,pValue->data.point,sizeof(POINT_T));
				}else if(pListNode->next->val.type == df_vector){
					pListNode->next->val.data.vector = (Vector*)malloc(sizeof(Vector));
					if(pListNode->next->val.data.vector) memcpy(pListNode->next->val.data.vector,pValue->data.vector,sizeof(Vector));
				}

				return pList;
			}
		}else if(pListNode->val.type == df_null){
			memcpy(&(pListNode->val),pValue,sizeof(Value));

			return pListNode;
		}
	}else if((pList == NULL) && pValue){
		DList *pTempList = NULL;

		if(NULL != (pTempList = NewDListNode())){
			memcpy(&(pTempList->val),pValue,sizeof(Value));
			if(pTempList->val.type == df_point){
				pTempList->val.data.point = (PPOINT_T)malloc(sizeof(POINT_T));
				if(pTempList->val.data.point) memcpy(pTempList->val.data.point,pValue->data.point,sizeof(POINT_T));
			}else if(pTempList->val.type == df_vector){
				pTempList->val.data.vector = (Vector*)malloc(sizeof(Vector));
				if(pTempList->val.data.vector) memcpy(pTempList->val.data.vector,pValue->data.vector,sizeof(Vector));
			}

			return pTempList;
		}
	}

	return NULL;
}

//	parameter	: pList - dest list , pAppend - source list
//	description	: append pSubList to pList
//	remrks		: pList don't have df_null type.
//	returns		: start list node of pList;
DList* AppendDList(DList *pList,DList *pAppend){
	if(pList){
		DList *pListNode = pList;
		for(pListNode = pList;pListNode->val.type != df_null && pListNode->next;pListNode = pListNode->next);
		
		if((pListNode->val.type != df_null) && (pListNode->next == NULL)){
			pListNode->next = pAppend;
		}

		return pList;
	}else	return pAppend;

	return NULL;
}

//	parameter	: pList - dest list , pSubList - source list
//	description	: append pSubList to pList as SubList
//	remrks		: 
//	returns		: start list node of pList;
DList* AppendSubDList(DList *pList,DList *pSubList){
	assert(pList);
	assert(pSubList);

	if(pList){
		DList *pListNode = pList;
		for(pListNode = pList;pListNode && pListNode->val.type != df_null && pListNode->next;pListNode = pListNode->next);

		if((pListNode->val.type != df_null) && (pListNode->next == NULL) && pSubList){
			if(NULL != (pListNode->next = NewDListNode())){
				pListNode->next->val.type = df_list;
				pListNode->next->val.data.list = pSubList;

				return pList;
			}
		}else if((pListNode->val.type == df_null) && pSubList){
			pListNode->val.type = df_list;
			pListNode->val.data.list = pSubList;

			return pList;
		}

		return pListNode;
	}else	return pSubList;

	return NULL;
}

//	parameter	: 
//	description	: get number of DList's items.
//	remrks		: 
//	returns		: number of DList's items.
int GetDListLength(DList *pList){
    	assert(pList);
	
	if(pList){
		int i = 0;

		DList *pListNode = pList;
		for(i = 0,pListNode = pList;pListNode;pListNode = pListNode->next,i++);
		
		return i;
	}

	return 0;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
void FreeDList(DList *pList){
	assert(pList);

	if(pList){
		DList *pListTemp = pList;
		for(pListTemp = pList;pListTemp;){
			if(pListTemp->val.type != df_list){
				DList *pTemp = pListTemp;
				pListTemp = pListTemp->next;
				FreeDListNode(pTemp);
			}else if(pListTemp->val.type == df_list){
				DList *pTemp = pListTemp;
				pListTemp = pListTemp->next;
				FreeDList(pTemp->val.data.list);
				FreeDListNode(pTemp);
			}
		}
	}
}

//	parameter	: pList - pointer to DList
//	description	: 
//	remrks		: 
//	returns		: 
bool IsDListEmpty(DList *pList){
    	assert(pList);

	if(pList && (pList->val.type != df_null)) return false;
	
	return true;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
Value* GetDListValue(DList *pList,unsigned int nIndex){
    	assert(pList);

	if(pList && (nIndex >= 0)){
		unsigned int i = 0;

		DList *pListNode = pList;
		for(i = 0,pListNode = pList;i < nIndex && pListNode;i++,pListNode = pListNode->next);
		if(pListNode) return &(pListNode->val);
	}

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
Value* DListFirstValue(DList *pList){
    	assert(pList);

	if(pList) return &(pList->val);

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
Value* DListLastValue(DList *pList){
    	assert(pList);

	if(pList){
		DList *pTemp = pList;

		for(pTemp = pList;pTemp && (pTemp->next);pTemp = pTemp->next);

		return &(pTemp->val);
	}

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
DList* ReverseDList(DList *pList){
    	assert(pList);

	if(pList){
		DList *pHeadList = pList;

		DList *pReverseList = pHeadList->next,*pTempList;
		for(pHeadList->next = NULL,pReverseList = pHeadList->next;pReverseList;){
			pTempList = pReverseList;
			pReverseList = pReverseList->next;
			pTempList->next = pHeadList;
			pHeadList = pTempList;
		}

		return pHeadList;
	}

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
DList* DListFirst(DList *pList){
    	assert(pList);

	if(pList) return pList;

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
DList* DListLast(DList *pList){
    	assert(pList);

	if(pList){
		DList *pTemp = pList;

		for(pTemp = pList;pTemp && (pTemp->next);pTemp = pTemp->next);

		return pTemp;
	}

	return NULL;
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
DList* DListRest(DList *pList){
    	assert(pList);

	if(pList) return pList->next;

	return NULL;
}

//	parameter	: pList - pointer to DList,...
//	description	: copy between nStartIndex and nEndIndex in pList to SubList;
//	remrks		: 
//	returns		: DList*
DList* GetSubDList(DList *pList,unsigned int nStartIndex,unsigned int nEndIndex){
    	assert(pList);

	if(pList && (nStartIndex <= nEndIndex)){
		unsigned int i = 0,length = GetDListLength(pList);
		
		DList *pTemp = pList,*pSubList = NULL;
		for(i = 0,pTemp;pTemp && (i < nStartIndex && i < length);pTemp = pTemp->next,i++);
		for(;pTemp && (i <= nEndIndex && i < length);pTemp = pTemp->next,i++){
			if(pTemp->val.type != df_list) pSubList = PushValueToDList(pSubList,&(pTemp->val));
			else{
				Value SubValue;
				DList *pSubSubList = NULL;

				int sublen  = GetDListLength(pTemp->val.data.list);
				pSubSubList = GetSubDList(pTemp->val.data.list,0,sublen);
				SubValue.type = df_list;
				SubValue.data.list = pSubSubList;

				pSubList = PushValueToDList(pSubList,&SubValue);
			}
		}
		
		return pSubList;
	}

	return NULL;
}

//	parameter	: 
//	description	: 
//	remarks		: don't reference to original list
//	returns		: DList*
DList* FlatDList(DList *pList){
    	assert(pList);

	if(pList){
		int sublists = 0;
		DList *pTemp = pList,*pFlattern = pList,*p;

		while(1){
			for(sublists = 0,pTemp = pFlattern;pTemp;pTemp = pTemp->next)
				if(pTemp->val.type == df_list) sublists++;

			if(sublists){
				for(pTemp = pFlattern,pFlattern = NULL;pTemp;){
					if(pTemp->val.type == df_list){
						p = pTemp;
						pTemp = pTemp->next;

						pFlattern = AppendDList(pFlattern,p->val.data.list);
						FreeDListNode(p);
					}else{
						pFlattern = PushValueToDList(pFlattern,&(pTemp->val));
						p = pTemp;
						pTemp = pTemp->next;
						FreeDListNode(p);
					}
				}
			}else	break;
		}

		return pFlattern;
	}

	return NULL;
}

const static char* _9SQFORMAT[]={
	"",//df_null = 0
	"",//df_error_type
	"",//df_any
	"",//df_boolean
	"",//df_defeval
	"",//df_frame,
	"%d ",//df_integer,
	"",//df_list,
	"",//df_marker,
	"%d ",//df_name,
	"",//df_no_value,
	"%lf ",//df_number,
	"",//df_part,
	"%lf %lf %lf ",//df_point,
	"\"%s\" ",//df_string,
	"<%lf %lf %lf> ",//df_vector,
	"",//df_user,	/* ensure that this is last */
	/* These are extremely internal, and should never be seen in normal values. */
	"",//df_packed, 
	"",//df_heterogeneous_list,
	"",//df_homogeneous_list,
	"",//df_null_list,
	/* Owner-bit types here */
	"",//df_xnull = OWNER_BIT,
	"",//df_xerror_type,
	"",//df_xany,
	"",//df_xboolean,
	"",//df_xdefeval,
	"",//df_xframe,
	"",//df_xhostpointer,
	"",//df_xinteger,
	"",//df_xlist,
	"",//df_xmarker,
	"",//df_xname,
	"",//df_xno_value,
	"",//df_xnumber,
	"",//df_xpart,
	"",//df_xpoint,
	"",//df_xstring,
	"",//df_xvector,
	"",//df_xuser
};

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
void DumpDList(DList *pList){
    	assert(pList);

	if(pList){
		DList *pTemp = pList;

		for(pTemp = pList;pTemp;pTemp = pTemp->next){
			if(pTemp->val.type == df_number){
				printf(_9SQFORMAT[df_number],pTemp->val.data.number);
			}else if(pTemp->val.type == df_string){
				printf(_9SQFORMAT[df_string],pTemp->val.data.string);
			}else if(pTemp->val.type == df_point){
				printf(_9SQFORMAT[df_point],pTemp->val.data.point->x,pTemp->val.data.point->y,pTemp->val.data.point->z);
			}else if(pTemp->val.type == df_vector){
				printf(_9SQFORMAT[df_vector],pTemp->val.data.vector->dx,pTemp->val.data.vector->dy,pTemp->val.data.vector->dz);
			}else if(pTemp->val.type == df_name){
				printf(_9SQFORMAT[df_name],pTemp->val.data.name);
			}else if(pTemp->val.type == df_list){
				const static char* format[]={"{ ","} "};
				printf(format[0]);
				DumpDList(pTemp->val.data.list);
				printf(format[1]);
			}
		}
	}
}

//	parameter	: 
//	description	: 
//	remrks		: 
//	returns		: 
void DumpDListToFile(FILE *fp,DList *pList){
    	assert(fp);
	assert(pList);

	if(pList){
		DList *pTemp = pList;

		for(pTemp = pList;pTemp;pTemp = pTemp->next){
			if(pTemp->val.type == df_number){
				fprintf(fp,_9SQFORMAT[pTemp->val.type],pTemp->val.data.number);
			}else if(pTemp->val.type == df_string){
				fprintf(fp,_9SQFORMAT[pTemp->val.type],pTemp->val.data.string);
			}else if(pTemp->val.type == df_point){
				fprintf(fp,_9SQFORMAT[pTemp->val.type],pTemp->val.data.point->x,pTemp->val.data.point->y,pTemp->val.data.point->z);
			}else if(pTemp->val.type == df_vector){
				fprintf(fp,_9SQFORMAT[pTemp->val.type],pTemp->val.data.vector->dx,pTemp->val.data.vector->dy,pTemp->val.data.vector->dz);
			}else if(pTemp->val.type == df_name){
				fprintf(fp,_9SQFORMAT[pTemp->val.type],pTemp->val.data.name);
			}else if(pTemp->val.type == df_list){
				const static char* format[]={"{ ","}\r\n "};
				fprintf(fp,format[0]);
				DumpDListToFile(fp,pTemp->val.data.list);
				fprintf(fp,format[1]);
			}
		}
	}
}
