#ifndef	__SQDLIST_H__
#define	__SQDLIST_H__

#include "htypes.h"

typedef DList* PDLIST;

// macros for interfacing intent.
#define dfk_freeArgList   FreeArgList
#define dfk_listLength    GetDListLength
#define dfk_listPush      PushValueToDList
#define dfk_listAdd       PushValueToDList
#define dfk_listFree      FreeDList
#define dfk_listEmpty     IsDListEmpty
#define dfk_listValue     GetDListValue
#define dfk_listAppend    AppendDList
#define dfk_sublistAppend AppendSubDList
#define dfk_listReverse   ReverseDList
#define dfk_listFlattern  FlatDList

#define	_ARGS iPart p, ArgList *args, Value *rv
#define _9SQDEFUN(__func_name__) extern "C" __declspec(dllexport) void __func_name__ (_ARGS)
#define _9SQENDDEFUN(__func_name__)

// These provide pointers to Value objects
#define VAL1 (GetArg(args,0))
#define VAL2 (GetArg(args,1))
#define VAL3 (GetArg(args,2))
#define VAL4 (GetArg(args,3))
#define VAL5 (GetArg(args,4))
#define VAL6 (GetArg(args,5))
#define VAL7 (GetArg(args,6))
#define VAL8 (GetArg(args,7))
#define VAL9 (GetArg(args,8))
	
// These put you right in the data union
#define RA1 (GetArg(args,0)->data)
#define RA2 (GetArg(args,1)->data)
#define RA3 (GetArg(args,2)->data)
#define RA4 (GetArg(args,3)->data)
#define RA5 (GetArg(args,4)->data)
#define RA6 (GetArg(args,5)->data)
#define RA7 (GetArg(args,6)->data)
#define RA8 (GetArg(args,7)->data)
#define RA9 (GetArg(args,8)->data)


extern "C" __declspec(dllexport) Value* GetArg(ArgList*,int nIndex);

extern DList* PushValueToDList(DList*,Value*);
extern int    GetDListLength(DList*);
extern Value* GetDListValue(DList*,int);
extern DList* GetSubDList(DList*,unsigned int,unsigned int);

extern void DumpDList(DList* pList);
extern void DumpDListToFile(FILE* fp,DList* pList);

#endif
