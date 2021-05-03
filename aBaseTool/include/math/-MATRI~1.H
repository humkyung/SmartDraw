//	name		: matrix.h
//	author		: baek hum kyung(baekhk@hanmail.net)
//	date		:
//	Description	: I find the original source in C++ journal.(author - Stephen)
//			  I modify the source
//
//	Update		: 1. add Eigenvalue()	-99.9.29
//			  2. add Permanent()	-99.10.06	


#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <math.h>
#include <array.h>

#define	CRMatrix CMatrix<double>(3,3)

template<class T>
class CMatrix{
//---------------------------------------------------------------------------------------------
//	constructure and destructure
public:
   	CMatrix(int nRows = 0, int nCols = 0);
	CMatrix(int nRows,int nCols,const T *data);
   	CMatrix(const CMatrix<T>& matrix);
  	virtual ~CMatrix();
public:
  	CMatrix<T>& operator=(const CMatrix<T>& matrix);

  	bool operator==(const CMatrix<T>& matrix) const;
  	int operator!=(const CMatrix<T>& matrix) const;

  	int	Rows() const { return m_nRows; }
  	int	Columns() const { return m_nCols; }

  	CMatrix<T> operator-() const;
  	CMatrix<T> operator~() const;

  	CMatrix<T> operator+(const CMatrix<T>& m) const;
  	CMatrix<T> operator-(const CMatrix<T>& m) const { return *this + -m; }
  	CMatrix<T> operator*(const T& t) const;
  	CMatrix<T> operator/(const T& t) const { return *this * (1/t); }

  	CMatrix<T>& operator+=(const CMatrix<T>& m) { return *this = *this + m; }
  	CMatrix<T>& operator-=(const CMatrix<T>& m) { return *this = *this - m; }
  	CMatrix<T>& operator*=(const T& t)         { return *this = *this * t; }
  	CMatrix<T>& operator/=(const T& t)         { return *this = *this / t; }

  	friend CMatrix<T> operator*(const T& t, const CMatrix<T>& m) { return m * t; }

  	CMatrix<T> operator*(const CMatrix<T>& m) const;

  	CMatrix<T>	RowMatrix(int nRow) const;
  	CMatrix<T> 	ColumnMatrix(int nCol) const;

  	void SetRow   (int nRow, const CMatrix<T>& matrix);
  	void SetColumn(int nCol, const CMatrix<T>& matrix);
	void SetData(int nRow,int nCol,T Data);

  	const T& operator()(int nRow, int nCol) const;
        T& operator()(int nRow, int nCol);

	CMatrix<T> Transpose();
	T	Determinant();
	bool	Eigenvalue(T *value);
	T	Permanent();

	bool	IsSquare();
	bool	IsSingular() const ;
	
	const CMatrix<T> GaussElimination(CMatrix<T> &Matrix);	// Gauss Elimination.
private:
	T	DetRecursive();
	CMatrix<T> Minor(size_t rdel,size_t cdel);
	T	sumSub(int m,int n,int k);
private:
  	int m_nRows;
  	int m_nCols;

	KArray<T> m_Data;
};

//---------------------------------------------------------------------------------------------
//	constructure and destructure
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
inline CMatrix<T>::CMatrix(int nRows, int nCols) : 
	m_nRows(nRows),m_nCols(nCols),m_Data(nRows * nCols){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
inline CMatrix<T>::CMatrix(int nRows,int nCols,const T *data) : 
	m_Rows(nRows),m_Cols(nCols),m_Data(nRows * nCols,data){}
   	 
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T>::CMatrix(const CMatrix<T>& matrix) :
  	m_nRows(matrix.m_nRows),m_nCols(matrix.m_nCols),m_Data(matrix.m_Data){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> inline CMatrix<T>::~CMatrix(){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T>& CMatrix<T>::operator=(const CMatrix<T> &matrix){
  	if(this != &matrix){
    		m_nRows = matrix.m_nRows;
    		m_nCols = matrix.m_nCols;
    		m_Data  = matrix.m_Data;
  	}
  	return *this;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline bool CMatrix<T>::operator==(const CMatrix<T> &matrix) const {
  	bool equal = ( m_Rows == matrix.m_Rows ) && ( m_Cols == matrix.m_Cols );

  	for (int m = 0;equal && ( m < m_Rows );m++)
  		for(int n = 0;equal && ( n < m_Cols );n++)
    			equal = ( (*this)(m,n) == matrix(m,n));

  	return equal;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> inline int CMatrix<T>::operator!=(const CMatrix<T> &matrix) const{
  	return !operator==(matrix);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::operator-() const {
  	CMatrix<T> matrix(m_Rows, m_Cols);

  	for ( int m = 0; m < m_Rows; m++ )
  		for ( int n = 0; n < m_Cols; n++ )
    			matrix(m, n) = -(*this)(m, n);

  	return matrix;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> inline CMatrix<T> CMatrix<T>::operator~() const {
  	CMatrix<T> matrix(m_Cols, m_Rows);

  	for ( int m = 0; m < m_Rows; m++ )
  		for ( int n = 0; n < m_Cols; n++ )
    			matrix(n, m) = (*this)(m, n);

  	return matrix;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::operator+(const CMatrix<T>& matrix) const {
  	assert((m_Rows == matrix.m_Rows) && "m_Row and matrix.m_Rows is not equal");
  	assert((m_Cols == matrix.m_Cols) && "m_Row and matrix.m_Rows is not equal");

  	CMatrix<T> tmp(m_Rows, m_Cols);

  	for ( int m = 0; m < m_Rows; m++ )
  		for ( int n = 0; n < m_Cols; n++ )
    			tmp(m, n) = (*this)(m, n) + matrix(m, n);

  	return tmp;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline const T& CMatrix<T>::operator()(int nRow,int nCol) const {
  	assert(nRow < m_Rows);
  	assert(nCol < m_Cols);

  	return m_Data[nRow*m_Cols + nCol];
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline T& CMatrix<T>::operator()(int nRow,int nCol){
	assert((nRow < m_nRows) && "nRow is greater than m_nRows");
	assert((nCol < m_nCols) && "nCol is greater than m_nCols");
	
	return m_Data[nRow*m_nCols + nCol];
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::operator*(const T& t) const {
  	CMatrix<T> matrix(m_Rows,m_Cols);
/*
	for(int i = 0;i < m_Rows * m_Cols;i++)
	    	matrix(i / m_Cols,i % m_Cols) = (*this)(i / m_Cols,i % m_Cols) * t;
*/	
  	for (int m = 0;m < m_Rows;m++ )
  		for (int n = 0;n < m_Cols;n++ )
    			matrix(m, n) = (*this)(m, n) * t;

  	return matrix;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::operator*(const CMatrix<T>& matrix) const {
  	assert(m_Cols == matrix.m_Rows);

  	CMatrix tmp(m_Rows, matrix.m_Cols);

  	for ( int m = 0; m <        m_Rows; m++ )
  		for ( int n = 0; n < matrix.m_Cols; n++ ){
    			tmp(m, n) = 0;
    			for ( int i = 0; i < m_Cols; i++ )
      				tmp(m, n) += (*this)(m, i) * matrix(i, n);
  		}

  	return tmp;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::RowMatrix(int nRow) const {
  	assert(nRow < m_Rows);

  	CMatrix<T> matrix(1, m_Cols);
  	for ( int n = 0; n < m_Cols; n++ )
    		matrix(0, n) = (*this)(nRow, n);
  	
	return matrix;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline CMatrix<T> CMatrix<T>::ColumnMatrix(int nCol) const {
  	assert(nCol < m_Cols);

  	CMatrix<T> matrix(m_Rows, 1);
  	for ( int m = 0; m < m_Rows; m++ )
    		matrix(m, 0) = (*this)(m, nCol);
  
	return matrix;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline void CMatrix<T>::SetRow(int nRows,const CMatrix<T> &matrix){
  	assert(nRow < m_Rows);

  	assert(matrix.Rows() == 1);
  	assert(matrix.Columns() == m_Cols);

  	for ( int n = 0; n < m_Cols; n++ )
    		(*this)(nRow, n) = matrix(0, n);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T> 
inline void CMatrix<T>::SetColumn(int nCol,const CMatrix<T> &matrix){
  	assert(nCol < m_Cols);

  	assert(matrix.Rows() == m_Rows);
  	assert(matrix.Columns() == 1);

  	for ( int m = 0; m < m_Rows; m++ )
    		(*this)(m, nCol) = matrix(m, 0);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
inline void CMatrix<T>::SetData(int nRow,int nCol,T Data){
	assert((nRow >= 0 && nRow < m_Rows) && (nCol >= 0 && nCol < m_Cols));

	m_Data[nRow * m_Cols + nCol] = Data;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
CMatrix<T> CMatrix<T>::Transpose(){
    	int m,n;
    	CMatrix<T> ret(m_Cols,m_Rows);
	
	for(size_t i = 0;i < m_Rows * m_Cols;i++){
	    	m = i / m_Cols;
		n = i % m_Cols;
	    	ret(n,m) = (*this)(m,n);
	}

	return ret;
}
	    	
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
inline bool CMatrix<T>::IsSquare(){
    	if(m_Rows == m_Cols)	return true;
	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
bool CMatrix<T>::IsSingular() const { 
    	int data = 0;
	if(m_Rows != m_Cols)	return false;

	CMatrix<T> csum(1,m_Cols),rsum(m_Rows,1);

	for(size_t ri = 0;ri < m_Rows;ri++){
	    	for(size_t ci = 0;ci < m_Cols;ci++){
		    	csum(0,ci) += m_Data[data];
			rsum(ri,0) += m_Data[data];
			data++;
		}
	}

	for(size_t i = 0;i < m_Rows;i++)
	    	if((csum(0,i) == 0) || (rsum(i,0) == 0))	return true;
			
	return false;		
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
template<class T>
T CMatrix<T>::Determinant(){
    	if(m_Rows != m_Cols)	return (T)0;

	if(m_Cols == 1)	return m_Data[0];
	if(IsSingular())	return (T)0;

	return DetRecursive();
}

//---------------------------------------------------------------------------------------------
//	eigenvalue를 찾는다. 리턴값은 성공적이면 true,그렇지 않으면 false,
//	value에 eignevalue가 반환된다.[i*2]-> real,[i*2+1]->imaginary
template<class T>
bool CMatrix<T>::Eigenvalue(T *value){
	int	i,it,j,k,l,m,na,nn,_i,_r;
	double	b23,dummy,f[11],h,hh,ma,p,q,r,M_eps,e,e_,s,ssr,rd,rd_cnv,uau,w,x,y,z;
	static	n=6;
	int	ir;
	T	rl[MAXMATRIX],im[MAXMATRIX];
	T	u[MAXMATRIX],t[MAXMATRIX][MAXMATRIX],a[MAXMATRIX][MAXMATRIX];
	cMath	math;

	for(i=1;i<=m_Rows;i++){
		for(j=1;j<=m_Cols;j++)	a[i][j]=(*this)(i-1,j-1);
	}

	for(ir=1;ir<=m_Rows-2;ir++){
		s=0;
		for(i=1;i<=m_Rows;i++){
			u[i]=0;
			if(i > ir+1)	u[i]=a[i][ir];
			if(i>ir)	s=s+a[i][ir]*a[i][ir];
		}
		w=1;
		if(a[ir+1][ir]<0)	w=-1;
		ssr=sqrt(s);
		h=s+fabs(a[ir+1][ir])*ssr;
		u[ir+1]=a[ir+1][ir]+ssr*w;
		uau=0;
		for(i=1;i<=m_Rows;i++){
			for(j=1;j<=m_Rows;j++){
				uau=uau+u[i]*a[i][j]*u[j];
				if(i<=ir && j<=ir)		t[i][j]=a[i][j];
				else if(j==ir && i >=ir+2)	t[i][j]=0;
				else{
					b23=0;
					for(k=1;k<=m_Rows;k++)
						b23=b23-(u[i]*a[k][j]+a[i][k]*u[j])*u[k];
					t[i][j]=a[i][j]+b23/h;
				}
			}
		}
		
		uau=uau/h/h;
		for(i=1;i<=m_Rows;i++){
			for(j=1;j<=m_Rows;j++){
				a[i][j]=t[i][j]+uau*u[i]*u[j];
				if(fabs(a[i][j]) < .000001)	a[i][j]=0;
			}
		}
	}

//	ma	=math.MachineEpsilon() * math.MachineEpsilon();
	ma	= 0.1 * 0.1;

	nn=n;rd_cnv=0.99;
	while(nn!=0){
		na=nn-1;
		for(it=1;it<=90;it++){
			l=1;
			for(m=nn;m>=2;m--){
				rd=fabs(a[m][m-1]) / (fabs(a[m-1][m-1]) + fabs(a[m][m]));
				if(rd < ma){
					l=m;
					rd_cnv=rd;
				}
			}
			x=a[nn][nn];
			if(l==nn){
				rl[nn]=x;im[nn]=0;
				nn=nn-1;
				goto L_1202;
			}
			y=a[na][na];
			r=a[nn][na]*a[na][nn];
			if(l==nn-1){
				p=(y-x)/2;q=p*p + r;y=sqrt(fabs(q));
				if(q<0){
					rl[nn-1]=x+p;rl[nn]=x+p;
					im[nn-1]=y;im[nn]=-y;
					nn=nn-2;
				}else{
					if(p<0)	y=-y;
					y=p+y;
					rl[nn-1]=x+y;rl[nn]=x-r/y;
					im[nn-1]=0;im[nn]=0;
					nn=nn-2;
				}
				goto L_1202;
			}
			if(it==90){
				goto L_765;
			}
			
			if(it==10 || it==20){
				y=fabs(a[nn][nn-1])+fabs(a[nn-1][nn-2]);
				s=1.5*y;
				y=y*y;
			}else{
				s=x+y;
				y=x*y - r;
			}

			for(m=nn-2;m>=1;m--){
				x=a[m][m];
				r=a[m+1][m];
				z=a[m+1][m+1];
				p=x*(x-s) + y + r*a[m][m+1];
				q=r*(x+z-s);
				r=r*a[m+2][m+1];
				w=fabs(p)+fabs(q)+fabs(r);
				p=p/w;q=q/w;r=r/w;
				if(m==1)	break;
				hh=fabs(a[m][m-1])*(fabs(q) + fabs(r));
				if(hh < ma*fabs(p)*(fabs(a[m-1][m-1]) + fabs(x)+fabs(z))) break;
			}
			
			for(i=m+2;i<=nn;i++)	a[i][i-2]=0;
			for(i=m+3;i<=nn;i++)	a[i][i-3]=0;
			for(k=m;k<=nn-1;k++){
				if(k!=m){
					p=a[k][k-1];q=a[k+1][k-1];r=a[k+2][k-1];
					if(nn-1 == k)	r=0;
					x=fabs(p) + fabs(q) + fabs(r);
					if(x==0)	break;
					p=p/x;q=q/x;r=r/x;
				}
				s=sqrt(p*p + q*q + r*r);
				if(p<0)	s=-s;
				if(k!=m)	a[k][k-1]=-s*x;
				if(l!=m)	a[k][k-1]=-a[k][k-1];
				p=p+s;x=p/s;y=q/s;
				z=r/s;q=q/p;r=r/p;
				
				for(j=k;j<=nn;j++){
					p=a[k][j]+q*a[k+1][j];
					if(nn-1 != k){
						p=p+r*a[k+2][j];
						a[k+2][j]=a[k+2][j]-p*z;
					}
					a[k+1][j]=a[k+1][j]-p*y;
					a[k][j]=a[k][j]-p*x;
				}
				j=nn;	if(k+3 < nn)	j=k+3;
				for(i=1;i<=j;i++){
					p=x*a[i][k]+y*a[i][k+1];
					if(nn-1!=k){
						p=p+z*a[i][k+2];
						a[i][k+2]=a[i][k+2]-p*r;
					}
					a[i][k+1]=a[i][k+1]-p*q;
					a[i][k]=a[i][k]-p;
				}
			}
		}
L_1202:;
	}
L_765:;

	for(i=0;i<m_Rows;i++){
		value[i*2]	=rl[i+1];
		value[i*2+1]	=im[i+1];
	}
	
      return true;
}

//---------------------------------------------------------------------------------------------------------------------------
//	매트릭스의 퍼머넌트를 구한다.
template<class T>
T CMatrix<T>::Permanent(){
	int i;
	T rtValue=0.0;
	cMath math;
	
	for(i=0;i<m_Rows;i++){
//		if(i%2)	rtValue-=math.binominal(m_Cols - m_Rows + i,i)*sumSub(m_Rows,m_Cols,m_Rows-i);
//		else	rtValue+=math.binominal(m_Cols - m_Rows + i,i)*sumSub(m_Rows,m_Cols,m_Rows-i);
	}
	return rtValue;
}

//	m행의 매트릭스에서 k개의 열을 뽑아서 이루는 행렬의 열의 합을 구해서 모두 곱한다.
template<class T>
T CMatrix<T>::sumSub(int m,int n,int k){
	int i,j,idx=0,idxArr[32];
	long value,initvalue=pow(2,k)-1,maxvalue=pow(2,k);
	long mask=0x0001;
	T temp,tmpValue=1.0,rtValue=0.0;
	
	
	value=initvalue;
	while(value >= initvalue){
		idx=0,mask=0x0001;
		for(i=0;i<n;i++){
			if(value & mask){
				idxArr[idx]=i;
				idx++;
			}
			mask<<=1;	
		}

		temp=0,tmpValue=1.0;
		for(i=0;i<m;i++){
			for(j=0;j<k;j++)	temp+=(*this)(i,idxArr[j]);
			tmpValue*=temp;
			temp=0.0;
		}
		rtValue+=tmpValue;
		
		value <<= 1;
		if(value > maxvalue){
			value%=maxvalue;
			value+=1;
		}
		if(value <= initvalue)	break;
	}
	return rtValue;
}

//--------------------------------------------------------------------------------------------
template<class T>
T CMatrix<T>::DetRecursive(){
    	size_t j = 0;
    	if(m_Cols == 2)
	    	return (m_Data[0] * m_Data[3] - (m_Data[1] * m_Data[2]));

	T ret = (T)0;

	for(size_t i = 0;i < m_Cols;i++){
	    	if(i & 1)	// if on an even column
		    	ret -= m_Data[j] * (Minor(0,i)).DetRecursive();
		else
		    	ret += m_Data[j] * (Minor(0,i)).DetRecursive();
		j++;
	}
	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:		
template<class T>
CMatrix<T> CMatrix<T>::Minor(size_t rdel,size_t cdel){
    	int i = 0,j = 0;
    	CMatrix<T> ret(m_Rows - 1,m_Cols - 1);

	for(size_t rsrc = 0;rsrc < m_Rows;rsrc++){
	    	if(rsrc != rdel){
		    	for(size_t csrc = 0;csrc < m_Cols;csrc++){
			    	if(csrc != cdel){
				    	ret.m_Data[i] = m_Data[j];
					i++;
				}
				j++;
			}
		}else	j += m_Cols;
	}

	return ret;
}

//	
//	parameter	: 
//	description	: 1. run pivotting
//			  2. run forword and backword substitution
//	remarks		: column and row must be equal!
//	returns		:
//
template<class T>
const CMatrix<T> CMatrix<T>::GaussElimination(CMatrix<T> &Matrix){
	int i,ii,j,k,l,n = Columns(),m = Matrix.Columns();
	int pivot,max_row;
	double temp;
	T c,sum;

	for(k = 0;k < n - 1;k++){
		
		// pivotting
		max_row = k;
		for(pivot = k;pivot < n;pivot++){
			if(fabs((*this)(pivot,k)) > fabs((*this)(max_row,k)))	max_row = pivot;
		}
		for(pivot = 0;pivot < n;pivot++){
			temp = (*this)(k,pivot);
			(*this)(k,pivot) = (*this)(max_row,pivot);
			(*this)(max_row,pivot) = temp;
		}
		for(pivot = 0;pivot < m;pivot++){
			temp = Matrix(k,pivot);
			Matrix(k,pivot) = Matrix(max_row,pivot);
			Matrix(max_row,pivot) = temp;
		}
		//

		//	Forword eliminstaion
		for(i = k + 1;i < n;i++){
			c = (*this)(i,k)/(*this)(k,k);
			for(j = k + 1;j < n;j++){
				(*this)(i,j) -= c*(*this)(k,j);
			}
			for(l = 0;l < m;l++)
				Matrix(i,l) -= c*Matrix(k,l);
		}
	}

	for(i = 1;i < n;i++)
		for(j = 0;j < i;j++)
			(*this)(i,j) = 0;
	
	CMatrix<T> ret(Matrix);
//	Backsubstitution
	ret(n - 1,0)/=(*this)(n - 1,n - 1);

	for(ii = n - 2;ii >= 0;ii--){
		i = n - ii;
		for(j = ii + 1,sum = 0;j < n;j++)
			sum += (*this)(ii,j)*ret(j,0);
		ret(ii,0) = (ret(ii,0) - sum)/(*this)(ii,ii);
		
	}

	return ret;
}

//---------------------------------------------------------------------------------------------------------------------------
/*
template<class T> 
inline ostream& operator<<(ostream &stream,const CMatrix<T> &matrix){
  	for ( int m = 0; m < matrix.Rows(); m++ ){
    		for ( int n = 0; n < matrix.Columns(); n++ ){
      			stream << matrix(m,n) << " ";
    		}
    		stream << endl;
  	}
  
	return stream;
}
*/
#endif // MATRIX_H


