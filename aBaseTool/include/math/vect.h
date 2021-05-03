#ifndef __VECT_H__
#define __VECT_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

typedef struct tagVector{
	double dx,dy,dz;	// dx,dy,dz
}VECTOR_T,SQVECTOR,* PVECTOR_T,* PSQVECTOR;

class DLL_EXPORT CVect : public VECTOR_T{
public:
	CVect();
	CVect(double x,double y,double z);
	CVect(const VECTOR_T& vec);
	CVect(const CVect& vec);
public:
	bool   Valid();
	double Length();
	void   Flatten();
	CVect  Perpendicular();
	bool   Normalize();

	void operator =(const SQVECTOR& vec);
	void operator =(const CVect& vec);
	void operator +=(const CVect& vec);
	void operator -=(const CVect& vec);
	void operator *=(const CVect& vec);
	double   Dot(const CVect& vec);

	CVect  operator +(CVect& rhs);
	CVect  operator -(CVect& rhs);
	CVect  operator *(CVect& rhs);
	double Dot(CVect& rhs);
};

DLL_EXPORT VECTOR_T operator -(const VECTOR_T& vec);
DLL_EXPORT VECTOR_T operator +(const VECTOR_T& vec1,const VECTOR_T& vec2);
DLL_EXPORT VECTOR_T operator -(const VECTOR_T& vec1,const VECTOR_T& vec2);
DLL_EXPORT VECTOR_T operator *(const double nVal,const VECTOR_T& vec);
DLL_EXPORT VECTOR_T operator *(const VECTOR_T& vec1,const VECTOR_T& vec2);

DLL_EXPORT double   Length(const VECTOR_T& vec);
DLL_EXPORT bool	Normalize(VECTOR_T& vec);
DLL_EXPORT VECTOR_T Perpendicular(VECTOR_T& vec);
DLL_EXPORT bool	IsPerpendicular(const VECTOR_T& vec1,const VECTOR_T& vec2);
DLL_EXPORT double   DotProduct(const VECTOR_T vec1,const VECTOR_T vec2);
DLL_EXPORT VECTOR_T CrossProduct(const VECTOR_T vec1,const VECTOR_T vec2);

#endif
