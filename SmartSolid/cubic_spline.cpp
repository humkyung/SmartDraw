#include <math\matrix.h>
#include "cubic_spline.h"

/**	\brief
*/
CCubicSpline::CCubicSpline(){
	m_nCount = 0;
	
	m_pnCoord= NULL;
	m_pnDelta= NULL;
}

/**	\brief
*/
CCubicSpline::CCubicSpline(int nCount,POINT_T ptControl[]){
	assert((nCount > 0) && "m_nCount is less than 0.");

	if(nCount > 0){
		m_nCount = nCount;
		memcpy(&(CCurve3d::pt(0)),ptControl,sizeof(POINT_T)*nCount);

		m_pnCoord = new double[m_nCount];
		m_pnDelta = new double[m_nCount];

		CreateSegments();
	}
}

/**	\brief
*/
CCubicSpline::CCubicSpline(const SQCURVE& CubicSpline){
	assert((CubicSpline.nPoints > 0) && "CubicSpline.nCount is less than 0.");

	if(CubicSpline.nPoints > 0){
		m_nCount = CubicSpline.nPoints;
		memcpy(&(CCurve3d::pt(0)),CubicSpline.ppt,sizeof(POINT_T)*m_nCount);
		memcpy(m_ptControl,CubicSpline.ppt,sizeof(POINT_T)*m_nCount);

		m_pnCoord = new double[m_nCount];
		m_pnDelta = new double[m_nCount];

		CreateSegments();
	}
}

/**	\brief
*/
CCubicSpline::~CCubicSpline(){
	OnDestroy();
}

/**	\brief	The CCubicSpline::CreateSegments function


	\return	void	
*/
void CCubicSpline::CreateSegments(){
	CMatrix<double> *A=NULL,*B=NULL,*C=NULL;
	A = new CMatrix<double>(m_nCount - 2,m_nCount - 2);
	B = new CMatrix<double>(m_nCount - 2,1);
	C = new CMatrix<double>(m_nCount - 2,1);
	if((m_nCount > 3) && A && B && C){
		// for X.
		for(int i=0;i < m_nCount-2;i++){
			for(int j=0;j < m_nCount-2;j++){
				(*A)(i,j)=0;
			}
		}

		(*A)(0,0) = 4.;
		(*A)(0,1) = 1.;
		for(i = 1;i < m_nCount - 3;i++){
			(*A)(i,i - 1)	= 1.;
			(*A)(i,i)	= 4.;
			(*A)(i,i + 1)	= 1.;
		}
		(*A)(i,i - 1) = 1.;
		(*A)(i,i)     = 4.;
		for(i = 0;i < m_nCount - 2;i++){
			(*B)(i,0) = 3*((CCurve3d::pt(i+2).x - CCurve3d::pt(i+1).x) + (CCurve3d::pt(i+1).x - CCurve3d::pt(i).x));
		}
		(*C) = (*A).GaussElimination((*B));

		m_ptDerivative[0].x = 0.;
		for(i = 0;i < m_nCount - 2;i++)	m_ptDerivative[i+1].x = (*C)(i,0);
		m_ptDerivative[i+1].x = 0.;
		
		// for Y.
		(*A)(0,0) = 4.;
		(*A)(0,1) = 1.;
		for(i = 1;i < m_nCount - 3;i++){
			(*A)(i,i - 1)	= 1.;
			(*A)(i,i)	= 4.;
			(*A)(i,i + 1)	= 1.;
		}
		(*A)(i,i - 1) = 1.;
		(*A)(i,i)     = 4.;
		for(i = 0;i < m_nCount - 2;i++){
			(*B)(i,0) = 3*((CCurve3d::pt(i+2).y - CCurve3d::pt(i+1).y) + (CCurve3d::pt(i+1).y - CCurve3d::pt(i).y));
		}
		(*C) = (*A).GaussElimination((*B));

		m_ptDerivative[0].y = 0.;
		for(i = 0;i < m_nCount - 2;i++)	m_ptDerivative[i+1].y = (*C)(i,0);
		m_ptDerivative[i+1].y = 0.;

		// for Z.
		(*A)(0,0) = 4.;
		(*A)(0,1) = 1.;
		for(i = 1;i < m_nCount - 3;i++){
			(*A)(i,i - 1)	= 1.;
			(*A)(i,i)	= 4.;
			(*A)(i,i + 1)	= 1.;
		}
		(*A)(i,i - 1) = 1.;
		(*A)(i,i)     = 4.;
		for(i = 0;i < m_nCount - 2;i++){
			(*B)(i,0) = 3*((CCurve3d::pt(i+2).z - CCurve3d::pt(i+1).z) + (CCurve3d::pt(i+1).z - CCurve3d::pt(i).z));
		}
		(*C) = (*A).GaussElimination((*B));

		m_ptDerivative[0].z = 0.;
		for(i = 0;i < m_nCount - 2;i++)	m_ptDerivative[i+1].z = (*C)(i,0);
		m_ptDerivative[i+1].z = 0.;

		delete A;
		delete B;
		delete C;
	}
}

/**	\brief	The CCubicSpline::OnDestroy function


	\return	void	
*/
void CCubicSpline::OnDestroy(){
	if(m_pnCoord) delete m_pnCoord;
	if(m_pnDelta) delete m_pnDelta;
}

/**	\brief	The CCubicSpline::operator() function

	\param	k	a parameter of type int
	\param	u	a parameter of type double

	\return	POINT_T	
*/
POINT_T CCubicSpline::operator()(int k,double u){
	assert((k < m_nCount) && "k is greater than m_nCount");
	assert((u >= 0) && (u <= 1) && "u is out of range");
	POINT_T pt={0};

	if((k < m_nCount) && (u >= 0) && (u <= 1)){
		pt.x =	m_ptControl[k].x*(2*u*u*u - 3*u*u + 1) + m_ptControl[k+1].x*(-2*u*u*u + 3*u*u) +
			m_ptDerivative[k].x*(u*u*u - 2*u*u + u) + m_ptDerivative[k+1].x*(u*u*u - u*u);
		pt.y =	m_ptControl[k].y*(2*u*u*u - 3*u*u + 1) + m_ptControl[k+1].y*(-2*u*u*u + 3*u*u) +
			m_ptDerivative[k].y*(u*u*u - 2*u*u + u) + m_ptDerivative[k+1].y*(u*u*u - u*u);
		pt.z =	m_ptControl[k].z*(2*u*u*u - 3*u*u + 1) + m_ptControl[k+1].z*(-2*u*u*u + 3*u*u) +
			m_ptDerivative[k].z*(u*u*u - 2*u*u + u) + m_ptDerivative[k+1].z*(u*u*u - u*u);
	}

	return pt;
}