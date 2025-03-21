#ifndef __MATRIX_H__
#define __MATRIX_H__

#define MATRIX_SQARE	1
#define TINY			1.0e-20
typedef int _RESULT;

#include <stdio.h>
#include <assert.h>

class CVector;

//#define float double

class CMatrix  
{
public:
	CMatrix();
	CMatrix(int nRows, int nCols);
	CMatrix(const CMatrix &mat);
	CMatrix(int nRows, int nCols, float *pData);
	CMatrix(int nRows, int nCols, float *pData, float **pRow);
	virtual ~CMatrix();
protected:
	void Initialize();

	CMatrix& EqSum(const CMatrix &refmatrixA, const CMatrix &refmatrixB);
	CMatrix& EqDiff(const CMatrix &refmatrixA, const CMatrix &refmatrixB);
	CMatrix& EqProd(const CMatrix &refmatrixA, const CMatrix &refmatrixB);

public:
	int GetColSize() const;
	int GetRowSize() const;
	int GetType() const;
	float *GetData(void);
	void Evaluate(float *pData);

	void PRINTF(const char *pFormat)
	{
		int i, j;
		for (i = 0; i < m_nRows; i++)
		{
			for (j = 0; j < m_nCols; j++)
			{
				printf(pFormat, m_pData[i*m_nCols + j]);
			}
			printf("\n");
		}
	}

	void SetSize(int rows, int cols);
	void CleareErrorFlag() { m_nErrorCode = 0; }

	CMatrix & Transpose(void);
	CMatrix Transposed(void);
	CMatrix & Inverse(void);
	CMatrix Inversed(void);
	float Determinant(void);
	void MakeZero();
	void MakeUnit();

	CVector SoluteEquation(CVector &b);

// overload operators
	float *operator[](int i)
	{
		assert(i < m_nRows && i >= 0);
		return m_pData+i*m_nCols;
	}
	const float *operator[](int i) const { return m_pData+i*m_nCols; }

	CMatrix operator+(const CMatrix& refmatrix) const;
	CMatrix operator-(const CMatrix& refmatrix) const;
	CMatrix operator*(const CMatrix &refmatrix);
	CMatrix operator*(float dbl) const;
	CVector operator*(const CVector& refvector) const;
	CMatrix operator/(float dbl) const;
	CMatrix operator-(void) const;

	CMatrix& operator=(float value);
	CMatrix& operator=(const CMatrix &mat);
	CMatrix& operator+=(const CMatrix& refmatrix);
	CMatrix& operator-=(const CMatrix& refmatrix);
	CMatrix& operator*=(float dbl);
	CMatrix& operator*=(const CMatrix& refmatrix);
	CMatrix& operator/=(float dbl);

	bool operator==(const CMatrix& refmatrix) const;
	bool operator!=(const CMatrix& refmatrix) const;

public:
	// Algorithms
	_RESULT GaussJordan(CMatrix &A, CMatrix &B);
	_RESULT LUDecomposition(CMatrix &A, int *indx, float &d);
	_RESULT LUBackstitution(CMatrix &A, int *indx, CVector &B);
	_RESULT SingularValueDecomposition(CMatrix &A, CVector &W, CMatrix &V);
	static bool Jacobi(CMatrix &A, CVector &W, CMatrix &V);
	float pythag(float a, float b);
	float sqr(float a);
	float sign(float a, float b);
	float Max(float a, float b);

	CMatrix SVDInverse();

protected:
	float ** m_data;
	float * m_pData;
	int		m_nCols, m_nRows;
	int		m_nType;

	int		m_nErrorCode;
	char	m_sErrorString[3][100];
	char	m_sErrorRoutine[30];

	bool    m_bShadow;
};

#undef float
#endif 
