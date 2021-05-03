#ifndef __MAT_H__
#define __MAT_H__

#ifndef DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

namespace itk{
	class CMatrix{
	public:
		CMatrix(const int nRows=3,const int nCols=3);
		CMatrix(const CMatrix& matrix);
		virtual ~CMatrix();
	public:
		CMatrix* GaussElimination(CMatrix &matrix);
		CMatrix* RowMatrix(const int nRow);
		CMatrix* ColumnMatrix(const int nCol);
		CMatrix* TransposeMatrix();
		CMatrix& operator=(const CMatrix& matrix);
		CMatrix operator-() const;
		double& operator(const int nRow,const int nCol);
		CMatrix operator+(const CMatrix& matrix) const;
		CMatrix operator-(const CMatrix& matrix) const;
	private:
		int m_nRows,m_nCols;
		double* m_pElm;
	};
	
	// 3x3 matrix
	typedef struct tagMatrix{
		double elm[3][3];
	}MATRIX_T,* PMATRIX_T;

	// 3x4 matrix
	typedef struct tagTMatrix{
		double elm[3][4];
	}TMATRIX_T,* PTMATRIX_T;

	// 1x3 matrix
	typedef struct tagRMatrix{
		double elm[1][3];
	}RMATRIX_T,* PRMATRIX_T;

	// 1x3 matrix
	typedef struct tagCMatrix{
		double elm[3][1];
	}CMATRIX_T,* PCMATRIX_T;
};

extern itk::RMATRIX_T DLL_EXPORT GetRowMatrix(const itk::MATRIX_T& mat,const int nRow);
extern itk::CMATRIX_T DLL_EXPORT GetColumnMatrix(const itk::MATRIX_T& mat,const int nColumn);
extern itk::MATRIX_T  DLL_EXPORT Transposition(const itk::MATRIX_T& mat);
extern itk::MATRIX_T  DLL_EXPORT operator +(const itk::MATRIX_T& mat1,const itk::MATRIX_T& mat2);
extern itk::MATRIX_T  DLL_EXPORT operator -(const itk::MATRIX_T& mat1,const itk::MATRIX_T& mat2);
extern itk::MATRIX_T  DLL_EXPORT operator *(const itk::MATRIX_T& mat1,const itk::MATRIX_T& mat2);

#endif
