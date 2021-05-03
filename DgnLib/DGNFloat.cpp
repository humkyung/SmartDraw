#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <SQLib.h>
#include "DGNFloat.h"

typedef struct dbl {
	unsigned int hi;
	unsigned int lo;
}double64_t;

/************************************************************************/
/*                           DGN2IEEEDouble()                           */
/************************************************************************/
void DGN2IEEEDouble(void * dbl){
	assert(dbl && "dbl is invalid");

	if(dbl){
		double64_t	dt;
		unsigned int	sign;
		unsigned int  	exponent;
		unsigned int	rndbits;
		unsigned char	*src;
		unsigned char	*dest;
		
		src =  (unsigned char *) dbl;
		dest = (unsigned char *) &dt;
#ifdef __LITTLE_ENDIAN__
		dest[2] = src[0];
		dest[3] = src[1];
		dest[0] = src[2];
		dest[1] = src[3];
		dest[6] = src[4];
		dest[7] = src[5];
		dest[4] = src[6];
		dest[5] = src[7];
#else
		dest[1] = src[0];
		dest[0] = src[1];
		dest[3] = src[2];
		dest[2] = src[3];
		dest[5] = src[4];
		dest[4] = src[5];
		dest[7] = src[6];
		dest[6] = src[7];
#endif
		sign 	 = dt.hi & 0x80000000;	
		
		exponent = dt.hi >> 23;
		exponent = exponent & 0x000000ff;
		
		if (exponent) exponent = exponent -129 + 1023;
		
		rndbits = dt.lo & 0x00000007;
		
		dt.lo = dt.lo >> 3;
		dt.lo = (dt.lo & 0x1fffffff) | (dt.hi << 29);
		
		if (rndbits) dt.lo = dt.lo | 0x00000001;
		
		dt.hi = dt.hi >> 3;
		dt.hi = dt.hi & 0x000fffff;
		dt.hi = dt.hi | (exponent << 20) | sign;
		
#ifdef __LITTLE_ENDIAN__
		src = (unsigned char *) &dt;
		dest = (unsigned char *) dbl;
		
		dest[0] = src[4];
		dest[1] = src[5];
		dest[2] = src[6];
		dest[3] = src[7];
		dest[4] = src[0];
		dest[5] = src[1];
		dest[6] = src[2];
		dest[7] = src[3];
#else
		memcpy( dbl, &dt, 8 );
#endif	
	}
}

/**	\brief
*/
double IEEE2VAX ( double *d ){
	assert(d && "d is NULL");

#define BYTE     unsigned char
#define WORD     unsigned short
#define SWAP(A,B)    { BYTE  _t_; _t_ = A; A = B; B =_t_; }
	
	int  i=0;
	WORD sign,  // 0
	     expo;  // 1  -  11
	
	union {
		BYTE   b[8];
		double dd;
	} ieee;
	
	union {
		BYTE    b[8];
		WORD    w[4];
		double  dd;
	} vax;
	
	if( *d == 0.0) return(0.0);
	
	ieee.dd = *d;
	for( i = 0; i < 4; i++ ) SWAP( ieee.b[7-i],ieee.b[i] );
	
	
	sign = (ieee.b[0] & 0x80) ? 1 : 0;
	expo = ((ieee.b[0] & 0x7f) << 4) | ((ieee.b[1] & 0xf0) >> 4);
	
	expo += 128 - 1022;
	vax.w[0] = (expo << 7)  | ((ieee.b[1] & 0x0f) << 3) |
		(sign << 15) | ((ieee.b[2] & 0xe0) >> 5);
	
	
	SWAP( vax.b[0],vax.b[1] );
	
	for( i = 2; i < 7; i++ )
		vax.b[i] = ((ieee.b[i] & 0x1f) << 3) | ((ieee.b[i+1] & 0xe0) >> 5);
	vax.b[7] = ((ieee.b[7] & 0x1f) << 3);
	
	for( i = 0; i < 8; i += 2 )
		SWAP( vax.b[i],vax.b[i+1] );
	
	return( vax.dd );
	
#undef  BYTE
#undef  WORD
#undef  SWAP
}

/**	\brief	The VAX2IEEE function

	\param	d	a parameter of type double *

	\return	double	
*/
double VAX2IEEE( double *d ){
#define BYTE  unsigned char
#define WORD  unsigned int
#define SWAP(A,B) { BYTE  _t_; _t_ = A; A = B; B =_t_; }
	
	int  i=0;
	
	WORD sign,
	     expo;
	BYTE  nib;
	
	union {
		BYTE   v[8];
		double dd;
	}vax;
	
	union {
		BYTE    b[8];
		WORD    w[4];
		double  dd;
	} ieee;
	
	if( *d == 0.0 ) return(0.0);
	
	vax.dd = *d;
	for( i = 0; i < 8; i += 2 ) SWAP( vax.v[i],vax.v[i+1] );
	
	sign =  (vax.v[0] & 0x80) ? 1 : 0;
	expo = ((vax.v[0] & 0x7f) << 1) | ((vax.v[1] & 0x80) >> 7);
	nib  = ((vax.v[1] & 0x78) >> 3);
	
	expo += 1022 - 128;
	ieee.w[0] = (expo << 4) | nib | (sign << 15);
	
	SWAP( ieee.b[0],ieee.b[1] );
	for( i = 0; i < 6; i++ )
		ieee.b[i+2] = (vax.v[i+1] << 5) | (vax.v[i+2] >> 3);
	
	for( i = 0; i < 4; i++ )
		SWAP(ieee.b[7-i],ieee.b[i]);
	
	return( ieee.dd );
	
#undef  BYTE
#undef  WORD
#undef  SWAP
}
