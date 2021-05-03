#ifndef __TEXT_H__
#define __TEXT_H__

enum{
	JUST_LEFT_BOT	   = 1,
	JUST_RIGHT_BOT	   = 2,
	JUST_CENTER_CENTER = 3
};

typedef struct tagText{
	POINT_T	 ptOrigin;	// origin point
	double	 nWidth;	// width of text
	double   nHeight;	// height of text
	VECTOR_T vecNorm;	// normal vector
	double	 nRotate;	// rotate angle in degree
	double	 nOblique;	// oblique angle in degree
	short	 nJust;		// justification
	char*	 pFamily;	// font id
	char*	 pText;		// actual text(NULL terminated)
}TEXT_T,* PTEXT_T;

#endif
