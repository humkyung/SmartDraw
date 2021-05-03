// -htypes.h- for intetent for autocad
// if you use IKS this is invalid and crash program.
#ifndef HTYPES_H
#define HTYPES_H

#include "SQLib.h"

// Macros
#define OWNER_BIT 0x40000000

/* Supporting types */

typedef enum {		/* Defign type coding ALPHABETICAL (mostly) */
	df_null = 0,
	df_error_type,
	df_any,
	df_boolean,
	df_defeval,
	df_frame,
	df_integer,
	df_list,
	df_marker,
	df_name,
	df_no_value,
	df_number,
	df_part,
	df_point,
	df_string,
	df_vector,
	df_user,	// ensure that this is last
	// Owner-bit types here
	df_xnull = OWNER_BIT,
	df_xerror_type,
	df_xany,
	df_xboolean,
	df_xdefeval,
	df_xframe,
	df_xinteger,
	df_xlist,
	df_xmarker,
	df_xname,
	df_xno_value,
	df_xnumber,
	df_xpart,
	df_xpoint,
	df_xstring,
	df_xvector,
	df_xuser

} DF_type;

// Support for setting and clearing the owner bit
#define ADD_OWNER(_typ_) (DF_type)((_typ_) | OWNER_BIT)
// The following was changed to Boolean by jgg 15-Sep-99
// I don't understand why it was ever a DF_type
//#define HAS_OWNER(_typ_) (DF_type)((_typ_) & OWNER_BIT)
#define HAS_OWNER(_typ_) (Boolean)((_typ_) & OWNER_BIT)

#define STRIP_OWNER(_typ_) (DF_type)((_typ_) & (~OWNER_BIT))
// Note that the second arg must be an DF_type enum constant
#define TYPEQ(_ta_, _tb_) (STRIP_OWNER(_ta_) == _tb_)
// This is the general type to type comparison
#define TYPEQUAL(_ta_, _tb_) (STRIP_OWNER(_ta_) == STRIP_OWNER(_tb_))


typedef struct _DF_Matrix {
	double element[4][4];
} DF_Matrix;

typedef unsigned Token;
#define t_error_token (Token)0

typedef struct _StrArr {
	char **arr;
	int count;
	int size;
}StrArr;

// Hook = a pointer to a function taking void and returning int
typedef int (*Hook)(void);

/* Defign Types */
typedef unsigned Boolean;

typedef struct _DF_Error {
	int type;	// The error code
	char op;	// The operation
	DF_type operand1;
	DF_type operand2;
	void *evalState;	// Reserved
	const char *title;
	char *message;	// Used to provide more detail
} DF_Error;

typedef struct _Frame {
	DF_Matrix LocaltoWorld;
	DF_Matrix WorldtoLocal;
} Frame;

typedef int Integer;
typedef Token Name;
typedef double Number;

typedef struct _basePart {
	unsigned magic;
	void *design;
	void *bindtab;
	void *inputs;
	Token rule;
	void *nha;
	Token name;
	int index;
	//void *glue;
        void *dbNode;
} basePart;

typedef struct _partHandle {
	basePart *part;
}partHandle,* iPart;
typedef iPart Part;

typedef char *String;

typedef struct {
	double x, y, z;
} Point;

typedef struct _Vector {
	double dx, dy, dz;
} Vector;

struct _DList;	/* Forward declaration */

typedef unsigned char Byte;

typedef struct _EvalCode {
	unsigned int size;
	Byte *bytes;
	int memory_area;
	StrArr *literals; // String literals owned by the code
} EvalCode;

typedef struct _defevalPack {
	DF_type type;
	EvalCode *code;
} defevalPack;

typedef struct _Value {
	DF_type type;
	union {
		void	 *user;
		Boolean  boolean;
		DF_Error *error;
		defevalPack *defeval; //defeval type
		Frame	  *frame;
		Integer	  integer;
		struct	  _DList *list;
		Name	  name;
		Number	  number;
		iPart	  part;
		PPOINT_T  point;
		String	  string;
		Vector*   vector;
	} data;
}Value;

typedef struct _DList {
	Value val;
	struct _DList *next;
}DList;

/*
typedef struct _df_Font {
	char **table;
	int leftOffset;
	int baseOffset;
	int stdWidth;
	int stdHeight;
	int bboxLeft;
	int bboxBottom;
	int bboxRight;
	int bboxTop;
} df_Font;
*/

typedef struct _ArgList {
	int length;		// Length of names and vals arrays
	Token *names;	// Array of names of arguments
	Value *vals;	// Array of argument values
} ArgList;

/* Internal data structure used by graphical picks */
typedef struct _Pick_Data {
	Vector	*normal;
	iPart	workplaneObj;
	Token	*workplaneRef;
	Value	*pickValue;
	Token	*pickObjRef; /* Internal -- for UI to decache part-picks. */
	POINT_T *nearPoint;
	POINT_T *farPoint;
} Pick_Data;


#endif /* HTYPES_H */

/* EOF */
