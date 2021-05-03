#include <math\mat.h>
using namespace itk;

CMatrix::CMatrix(const int nRows,const int nCols){
	m_nRows = m_nCols = 0;
	if(NULL != (m_pElm = (double*)calloc(1,sizeof(double)*(nRows*nCols)))){
		m_nRows = nRows;
		m_nCols = nCols;
	}
}

CMatrix::CMatrix(const CMatrix& matrix){
	if(NULL != (m_pElm = (double*)calloc(1,sizeof(double)*(matrix.m_nRows*matrix.m_nCols)))){
		m_nRows = matrix.m_nRows;
		m_nCols = matrix.m_nCols;
		memcpy(m_pElm,matrix.m_pElm,sizeof(double)*(m_nRows*m_nCols));
	}
}

CMatrix::~CMatrix(){
	if(NULL != m_pElm) free((void*)m_pElm);
}

CMatrix& CMatrix::operator =(const CMatrix& matrix){
	if(this != &matrix){
		if(NULL != (m_pElm = (double*)realloc(m_pElm,sizeof(double)*(matrix.m_nRows*matrix.m_nCols)))){
			m_nRows = matrix.m_nRows;
			m_nCols = matrix.m_nCols;
			memcpy(m_pElm,matrix.m_pElm,sizeof(double)*(m_nRows*m_nCols));
		}
	}
    	
  	return *this;
}

double& CMatrix::operator ()(const int nRow,const int nCol){
	assert((nRow < m_nRows) && "nRow is greater than m_nRows");
	assert((nCol < m_nCols) && "nCol is greater than m_nCols");
	
	return m_pElm[nRow*m_nCols + nCol];
}

CMatrix CMatrix::operator-() const {
  	CMatrix ret(m_Rows, m_Cols);

  	for(int m = 0;m < m_Rows;m++ )
  		for(int n = 0;n < m_Cols;n++ )
    			ret(m,n) = -(*this)(m,n);

  	return ret;
}

CMatrix CMatrix::operator+(const CMatrix& matrix) const {
  	assert((m_Rows == matrix.m_Rows) && "m_Row and matrix.m_Rows is not equal");
  	assert((m_Cols == matrix.m_Cols) && "m_Row and matrix.m_Rows is not equal");
  	CMatrix ret(m_Rows, m_Cols);

	if((m_nRows == matrix.m_nRows) && (m_nCols == matrix.m_nCols)){
	  	for(int m = 0;m < m_Rows;m++)
			for(int n = 0;n < m_Cols;n++)
				ret(m, n) = (*this)(m, n) + matrix(m, n);
	}

  	return ret;
}

CMatrix CMatrix::operator-(const CMatrix& matrix) const{
	assert((m_Rows == matrix.m_Rows) && "m_Row and matrix.m_Rows is not equal");
  	assert((m_Cols == matrix.m_Cols) && "m_Row and matrix.m_Rows is not equal");
  	
	if((m_nRows == matrix.m_nRows) && (m_nCols == matrix.m_nCols)){
		return (*this) + (-matrix);
	}
	return (*this);
}

CMatrix* CMatrix::RowMatrix(const int nRow){
	CMatrix* pRet=new CMatrix(1,m_nCols);
	
	if(pRet){
		long nIndex=nRow*m_nCols;
		for(int i = 0;i < m_nCols;i++){
			pRet->m_pElm[i]=m_pElm[nIndex + i];
		}
	}

	return pRet;
}

CMatrix* CMatrix::ColumnMatrix(const int nCol){
	CMatrix* pRet=new CMatrix(m_nRows,1);

	if(pRet){
		for(int i = 0;i < m_nRows;i++){
			pRet->m_pElm[i] = m_pElm[i*m_nCols];
		}
	}

	return pRet;
}

CMatrix* CMatrix::TransposeMatrix(){
	CMatrix* pRet=new CMatrix(m_nCols,m_nRows);

	if(pRet){
	    	int m=0,n=0;
		for(size_t i = 0;i < m_Rows * m_Cols;i++){
		    	m = i / m_Cols;
			n = i % m_Cols;
			(*pRet)(n,m) = (*this)(m,n);
		}
	}

	return pRet;
}

CMatrix* CMatrix::GaussElimination(CMatrix &matrix){
	int ii,j,n = Columns(),m = Matrix.Columns();
	int pivot,max_row;
	double temp;
	double c,sum;

	for(int k = 0;k < m_nCols - 1;k++){	
		/// pivotting
		max_row = k;
		for(int pivot = k;pivot < n;pivot++){
			if(fabs((*this)(pivot,k)) > fabs((*this)(max_row,k)))	max_row = pivot;
		}
		for(pivot = 0;pivot < m_nCols;pivot++){
			temp = (*this)(k,pivot);
			(*this)(k,pivot) = (*this)(max_row,pivot);
			(*this)(max_row,pivot) = temp;
		}
		for(pivot = 0;pivot < matrix.m_nCols;pivot++){
			temp = matrix(k,pivot);
			matrix(k,pivot) = matrix(max_row,pivot);
			matrix(max_row,pivot) = temp;
		}

		/// Forword eliminstaion
		for(int i = k + 1;i < m_nCols;i++){
			c = (*this)(i,k)/(*this)(k,k);
			for(int j = k + 1;j < n;j++){
				(*this)(i,j) -= c*(*this)(k,j);
			}
			for(int l = 0;l < m;l++)
				matrix(i,l) -= c*matrix(k,l);
		}
	}

	for(i = 1;i < n;i++)
		for(j = 0;j < i;j++)
			(*this)(i,j) = 0;
	
	CMatrix* pRet=new CMatrix(matrix);
	if(pRet){
		/// Backsubstitution
		(*pRet)(m_nCols - 1,0)/=(*this)(m_nCols - 1,m_nCols - 1);

		for(int ii = m_nCols - 2;ii >= 0;ii--){
			i = m_nCols - ii;
			for(j = ii + 1,sum = 0;j < m_nCols;j++)
				sum += (*this)(ii,j)*(*pRet)(j,0);
			(*pRet)(ii,0) = ((*pRet)(ii,0) - sum)/(*this)(ii,ii);	
		}
	}

	return pRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
MATRIX_T Transposition(const itk::MATRIX_T& mat){
	MATRIX_T varRet;

	varRet.elm[0][0] = mat.elm[0][0];
	varRet.elm[0][1] = mat.elm[1][0];
	varRet.elm[0][2] = mat.elm[2][0];

	varRet.elm[1][0] = mat.elm[0][1];
	varRet.elm[1][1] = mat.elm[1][1];
	varRet.elm[1][2] = mat.elm[2][1];

	varRet.elm[2][0] = mat.elm[0][2];
	varRet.elm[2][1] = mat.elm[1][2];
	varRet.elm[2][2] = mat.elm[2][2];

	return varRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
MATRIX_T operator +(const MATRIX_T& mat1,const MATRIX_T& mat2){
	MATRIX_T ret;

	ret.elm[0][0] = mat1.elm[0][0] + mat2.elm[0][0];
	ret.elm[0][1] = mat1.elm[0][1] + mat2.elm[0][1];
	ret.elm[0][2] = mat1.elm[0][2] + mat2.elm[0][2];
	ret.elm[1][0] = mat1.elm[1][0] + mat2.elm[1][0];
	ret.elm[1][1] = mat1.elm[1][1] + mat2.elm[1][1];
	ret.elm[1][2] = mat1.elm[1][2] + mat2.elm[1][2];
	ret.elm[2][0] = mat1.elm[2][0] + mat2.elm[2][0];
	ret.elm[2][1] = mat1.elm[2][1] + mat2.elm[2][1];
	ret.elm[2][2] = mat1.elm[2][2] + mat2.elm[2][2];

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
MATRIX_T operator -(const MATRIX_T& mat1,const MATRIX_T& mat2){
	MATRIX_T ret;

	ret.elm[0][0] = mat1.elm[0][0] - mat2.elm[0][0];
	ret.elm[0][1] = mat1.elm[0][1] - mat2.elm[0][1];
	ret.elm[0][2] = mat1.elm[0][2] - mat2.elm[0][2];
	ret.elm[1][0] = mat1.elm[1][0] - mat2.elm[1][0];
	ret.elm[1][1] = mat1.elm[1][1] - mat2.elm[1][1];
	ret.elm[1][2] = mat1.elm[1][2] - mat2.elm[1][2];
	ret.elm[2][0] = mat1.elm[2][0] - mat2.elm[2][0];
	ret.elm[2][1] = mat1.elm[2][1] - mat2.elm[2][1];
	ret.elm[2][2] = mat1.elm[2][2] - mat2.elm[2][2];

	return ret;
}

//	parameter	:
//	description	: mat1 times mat2
//	remarks		:
//	returns		:
MATRIX_T operator *(const MATRIX_T& mat1,const MATRIX_T& mat2){
	MATRIX_T ret;

	ret.elm[0][0] = mat1.elm[0][0]*mat2.elm[0][0] + 
			    mat1.elm[0][1]*mat2.elm[1][0] +
			    mat1.elm[0][2]*mat2.elm[2][0];

	ret.elm[0][1] = mat1.elm[0][0]*mat2.elm[0][1] + 
			    mat1.elm[0][1]*mat2.elm[1][1] +
			    mat1.elm[0][2]*mat2.elm[2][1];

	ret.elm[0][2] = mat1.elm[0][0]*mat2.elm[0][2] + 
			    mat1.elm[0][1]*mat2.elm[1][2] +
			    mat1.elm[0][2]*mat2.elm[2][2];
		
	ret.elm[1][0] = mat1.elm[1][0]*mat2.elm[0][1] + 
			    mat1.elm[1][1]*mat2.elm[1][1] +
			    mat1.elm[1][2]*mat2.elm[2][1];

	ret.elm[1][0] = mat1.elm[1][0]*mat2.elm[0][0] + 
			    mat1.elm[1][1]*mat2.elm[1][0] +
			    mat1.elm[1][2]*mat2.elm[2][0];

	ret.elm[1][1] = mat1.elm[1][0]*mat2.elm[0][1] + 
			    mat1.elm[1][1]*mat2.elm[1][1] +
			    mat1.elm[1][2]*mat2.elm[2][1];

	ret.elm[1][2] = mat1.elm[1][0]*mat2.elm[0][2] + 
			    mat1.elm[1][1]*mat2.elm[1][2] +
			    mat1.elm[1][2]*mat2.elm[2][2];

	ret.elm[2][0] = mat1.elm[2][0]*mat2.elm[0][0] + 
			    mat1.elm[2][1]*mat2.elm[1][0] +
			    mat1.elm[2][2]*mat2.elm[2][0];

	ret.elm[2][1] = mat1.elm[2][0]*mat2.elm[0][1] + 
			    mat1.elm[2][1]*mat2.elm[1][1] +
			    mat1.elm[2][2]*mat2.elm[2][1];

	ret.elm[2][2] = mat1.elm[2][0]*mat2.elm[0][2] + 
			    mat1.elm[2][1]*mat2.elm[1][2] +
			    mat1.elm[2][2]*mat2.elm[2][2];

	return ret;
}
