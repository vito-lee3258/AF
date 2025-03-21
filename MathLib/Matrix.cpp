#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Matrix.h"
#include "Vector.h"

#ifndef _WIN32
#define strcpy_s strcpy
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define float double

CMatrix::CMatrix()
{
	Initialize();
	m_nCols = m_nRows = 0;
}

CMatrix::CMatrix(int nRows, int nCols)
{
	Initialize();
	SetSize(nRows, nCols);
}

CMatrix::CMatrix(const CMatrix &mat)
{
	Initialize();
	SetSize(mat.GetRowSize(), mat.GetColSize());
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nCols; j++)
			(*this)[i][j] = mat[i][j];
	m_nType = mat.GetType();
}

CMatrix::CMatrix(int nRows, int nCols, float *pData)
{
	Initialize();
	SetSize(nRows, nCols);
	Evaluate(pData);
}

CMatrix::CMatrix(int nRows, int nCols, float *pData, float **pRow)
{
	int i;
	Initialize();
	m_bShadow = true;

	assert(nRows >= 0);
	assert(nCols >= 0);

	if (m_pData)
	{
		delete[]m_pData;
		delete[]m_data;
	}
	m_nCols = nCols;
	m_nRows = nRows;
	if (pData != NULL)
	{
		m_pData = pData;
		m_data = pRow;
		for (i = 0; i < nRows; i++)
			m_data[i] = m_pData + i * nCols;
	}
	else if (nRows > 0 && nCols > 0)
	{
		m_pData = new float[nRows * nCols];
		m_data = new float *[nRows];
		for (i = 0; i < nRows; i++)
			m_data[i] = m_pData + i * nCols;
	}
	else
	{
		m_pData = NULL;
		m_nRows = m_nCols = 0;
	}

	
	if ((m_nRows == m_nCols) && (m_nRows > 0))
		m_nType |= MATRIX_SQARE;
	else
		m_nType &= !MATRIX_SQARE;
}

void CMatrix::Initialize()
{
	m_pData = NULL;
	m_nType = 0;
	m_nErrorCode = 0;
	m_bShadow = false;
	strcpy_s(m_sErrorString[0], "No error!");
	strcpy_s(m_sErrorString[1], "Singular Matrix!");
	strcpy_s(m_sErrorString[2], "No convergence in 30 SVD iterations");
}

void CMatrix::MakeZero()
{
	assert( m_nRows > 0 && m_nCols > 0 );
	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nCols; j++)
			(*this)[i][j] = 0;
}

void CMatrix::MakeUnit()
{
	int i, j;
	assert( m_nRows == m_nCols && m_nCols> 0 );
	for (i = 0; i < m_nRows; i++)
	{
		for (j = 0; j < i; j++)
			(*this)[i][j] = 0;
		(*this)[i][i]=1;
		for(j=i+1; j<m_nCols; j++)
			(*this)[i][j] = 0;
	}
}

CMatrix::~CMatrix()
{
	if (m_pData && !m_bShadow)
	{
		delete []m_pData;
		delete []m_data;
	}
}

void CMatrix::SetSize(int rows, int cols)
{
	int i;

	assert(rows >= 0);
	assert(cols >= 0);

	if (m_pData)
	{
		delete []m_pData;
		delete []m_data;
	}
	m_nCols = cols;
	m_nRows = rows;
	if (rows > 0 && cols > 0)
	{
		m_pData = new float [rows * cols];
		m_data = new float * [rows];
		for (i = 0; i < rows; i++)
			m_data[i] = m_pData + i * cols;
	}
	else
	{
		m_pData = NULL;
		m_nRows = m_nCols = 0;
	}
	if ((m_nRows == m_nCols) && (m_nRows > 0))
		m_nType |= MATRIX_SQARE;
	else
		m_nType &= !MATRIX_SQARE;
}

int CMatrix::GetRowSize() const
{
	return m_nRows;
}

int CMatrix::GetColSize() const
{
	return m_nCols;
}

int CMatrix::GetType() const
{
	return m_nType;
}

float * CMatrix::GetData(void)
{
	return m_pData;
}

void CMatrix::Evaluate(float *pData)
{
	memcpy(m_pData, pData, m_nRows*m_nCols*sizeof(float));
}

CMatrix & CMatrix::Transpose(void)
{
	*this = Transposed();
	return *this;
}

CMatrix CMatrix::Transposed(void)
{
    int i, j;
    CMatrix matrixTranspose(m_nCols, m_nRows);

    for (i = 0; i < m_nCols; i++)
        for (j = 0; j < m_nRows; j++)
            matrixTranspose[i][j] = (*this)[j][i];

	return matrixTranspose;
}

CMatrix & CMatrix::Inverse(void)
{
	assert(m_nRows == m_nCols);
	*this = Inversed();
	return *this;
}

CMatrix CMatrix::Inversed(void)
{
	assert(m_nType & MATRIX_SQARE);
	int *indx = new int [m_nRows];
	int i, j;
	float d;
	CMatrix matrixLU(*this);
	CMatrix matrixInverse(m_nRows, m_nCols);
	CVector vectorCol(m_nRows);

	LUDecomposition(matrixLU, indx, d);	// Decompose the matrix just once.
	for (j = 0; j < m_nCols; j++)		// Find inverse by columns.
	{
		for (i = 0; i < m_nRows; i++)
			vectorCol[i] = 0;
		vectorCol[j] = 1;
		LUBackstitution(matrixLU, indx, vectorCol);
		for (i = 0; i < m_nRows; i++)
			matrixInverse[i][j] = vectorCol[i];
	}
	delete []indx;
	return matrixInverse;
}

float CMatrix::Determinant(void)
{
	assert(m_nType & MATRIX_SQARE);
	CMatrix matrixLU(*this);
	int *indx = new int [m_nRows];
	float d;

	LUDecomposition(matrixLU, indx, d);
	for (int i = 0; i < m_nRows; i++)
		d *= matrixLU[i][i];
	return d;
}

CVector CMatrix::SoluteEquation(CVector &b)
{
	CMatrix matrixU(*this);
	CMatrix matrixV(m_nCols, m_nCols);
	CMatrix matrixW(m_nCols, m_nCols);
	CVector vectorW(m_nCols);
	CVector vectorRet;

	if (!SingularValueDecomposition(matrixU, vectorW, matrixV))
	{
		vectorRet.SetSize(m_nCols);
		matrixW = 0;
		for (int i = 0; i < m_nCols; i++)
			matrixW[i][i] = 1 / vectorW[i];
		vectorRet = matrixV * matrixW * matrixU.Transpose() * b;
	}
	return vectorRet;
}

CMatrix CMatrix::SVDInverse()
{
	CMatrix matrixU(*this);
	CMatrix matrixV(m_nCols, m_nCols);
	CMatrix matrixW(m_nCols, m_nCols);
	CVector vectorW(m_nCols);
	CMatrix Ret;

	if (!SingularValueDecomposition(matrixU, vectorW, matrixV))
	{
		matrixW = 0;
		for (int i = 0; i < m_nCols; i++)
			matrixW[i][i] = 1 / vectorW[i];
		Ret = matrixV * matrixW * matrixU.Transpose();
	}
	return Ret;
}

CMatrix& CMatrix::EqSum(const CMatrix &refmatrixA, const CMatrix &refmatrixB)
{
    int nRows = GetRowSize(); 
    int nCols = GetColSize(); 
    assert(nRows == refmatrixA.GetRowSize());
    assert(nCols == refmatrixA.GetColSize());
    assert(nRows == refmatrixB.GetRowSize());
    assert(nCols == refmatrixB.GetColSize());

    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nCols; j++)
            (*this)[i][j] = refmatrixA[i][j] + refmatrixB[i][j];
    return (*this);
}

CMatrix& CMatrix::EqDiff(const CMatrix &refmatrixA, const CMatrix &refmatrixB)
{
    int nRows = GetRowSize(); 
    int nCols = GetColSize(); 
    assert(nRows == refmatrixA.GetRowSize());
    assert(nCols == refmatrixA.GetColSize());
    assert(nRows == refmatrixB.GetRowSize());
    assert(nCols == refmatrixB.GetColSize());

    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nCols; j++)
            (*this)[i][j] = refmatrixA[i][j] - refmatrixB[i][j];
    return (*this);
}

CMatrix& CMatrix::EqProd(const CMatrix &refmatrixA, const CMatrix &refmatrixB)
{
    int nTerms = refmatrixA.GetColSize(); 
	int i, j;
    assert(m_nRows == refmatrixA.GetRowSize());
    assert(nTerms == refmatrixB.GetRowSize());
    assert(refmatrixB.GetColSize() == m_nCols);

    for (i = 0; i < m_nRows; i++)
	{
        for (j = 0; j < m_nCols; j++)
		{
            float sum = 0.0;
            for (int k = 0; k < nTerms; k++)
                sum += refmatrixA[i][k] * refmatrixB[k][j];
            (*this)[i][j] = sum;
        }
    }
    return (*this);
}

CMatrix CMatrix::operator+(const CMatrix& refmatrix) const
{
    CMatrix matrixRet(*this);
    return matrixRet.EqSum(*this, refmatrix);
}

CMatrix CMatrix::operator-(const CMatrix& refmatrix) const
{
    CMatrix matrixRet(*this);
    return matrixRet.EqDiff(*this, refmatrix);
}

CMatrix CMatrix::operator*(float dbl) const
{
    CMatrix matrixRet(*this);
    return (matrixRet *= dbl);
}

CVector CMatrix::operator*(const CVector& refvector) const
{
    CVector vectorRet(m_nRows);
	return vectorRet.EqProd(*this, refvector);
}

CMatrix CMatrix::operator *(const CMatrix &refmatrix)
{
    CMatrix matrixRet(m_nRows, refmatrix.GetColSize());
	return matrixRet.EqProd(*this, refmatrix);
}

CMatrix CMatrix::operator/(float dbl) const
{
    CMatrix matrixRet(*this);
    return (matrixRet /= dbl);
}

CMatrix CMatrix::operator-(void) const
{
    CMatrix matrixRet(GetRowSize(), GetColSize());

    for (int i = 0; i < GetRowSize(); i++)
        for (int j = 0; j < GetColSize(); j++)
            matrixRet[i][j] = - (*this)[i][j];
    return matrixRet;
}

CMatrix& CMatrix::operator=(float value)
{
	if ((value == 0.0) && (m_nRows != 0))
	{
		assert(m_nRows > 0);
		assert(m_nCols > 0);

		// IEEE float
        memset(m_pData, 0, m_nRows*m_nCols*sizeof(float));
	}
	else
	{
		int n = m_nRows * m_nCols;
		for (int i = 0; i < n; i++)
			m_pData[i] = value;
	}
	return *this;
}

CMatrix& CMatrix::operator =(const CMatrix &mat)
{
	SetSize(mat.GetRowSize(), mat.GetColSize());

	for (int i = 0; i < m_nRows; i++)
		for (int j = 0; j < m_nCols; j++)
			(*this)[i][j] = mat[i][j];
	return *this;
}

CMatrix& CMatrix::operator+=(const CMatrix& refmatrix)
{
	assert((GetRowSize() == refmatrix.GetRowSize())
			&& (GetColSize() == refmatrix.GetColSize()));

    for (int i = 0; i < GetRowSize(); i++)
        for (int j = 0; j < GetColSize(); j++)
            (*this)[i][j] += refmatrix[i][j];
    return *this;
}

CMatrix& CMatrix::operator-=(const CMatrix& refmatrix)
{
	assert((m_nRows == refmatrix.GetRowSize()) && (m_nCols == refmatrix.GetColSize()));

    for (int i = 0; i < m_nRows; i++)
        for (int j = 0; j < m_nCols; j++)
            (*this)[i][j] -= refmatrix[i][j];
    return *this;
}

CMatrix& CMatrix::operator*=(float dbl)
{
    for (int i = 0; i < m_nRows; i++)
        for (int j = 0; j < m_nCols; j++)
            (*this)[i][j] *= dbl;
    return *this;
}

CMatrix& CMatrix::operator*=(const CMatrix& refmatrix)
{
	CMatrix matrixT(*this);

    assert(matrixT.GetColSize() == refmatrix.GetRowSize());

	if (refmatrix.GetColSize() != refmatrix.GetRowSize())
		SetSize(GetRowSize(), refmatrix.GetColSize());

    for (int i = 0; i < matrixT.GetRowSize(); i++)
	{
        for (int j = 0; j < refmatrix.GetColSize(); j++)
		{
            float sum = 0.0;
            for (int k = 0; k < GetColSize(); k++)
                sum += matrixT[i][k] * refmatrix[k][j];
            (*this)[i][j] = sum;
        }
    }
	return *this;
}

CMatrix& CMatrix::operator/=(float dbl)
{
	assert(dbl != 0);

    for (int i = 0; i < GetRowSize(); i++)
        for (int j = 0; j < GetColSize(); j++)
            (*this)[i][j] /= dbl;
    return *this;
}

bool CMatrix::operator==(const CMatrix& refmatrix) const
{
    if ((GetRowSize() == refmatrix.GetRowSize())
			&& (GetColSize() == refmatrix.GetColSize()))
	{
		if ((GetRowSize() == 0) || (GetColSize() == 0))
			return true;
		int cbRow = GetColSize()  * sizeof(float);
		for (int iRow = 0; iRow < GetRowSize(); ++iRow)
		{
			if (memcmp((*this)[iRow], refmatrix[iRow], cbRow) != 0)
				return false;
		}
		return true;
	}
    return false; 
}

bool CMatrix::operator!=(const CMatrix& refmatrix) const
{
    return !(*this == refmatrix);
}

// Linear equation solution by Gauss-Jordan elimination.
// Input:
//		A: input matrix.
//		B: input containing the m right-hand side vectors.
// Output:
//		A: replaced by its matrix inverse.
//		B: replaced by the corresponding set of solution vectors.
_RESULT CMatrix::GaussJordan(CMatrix &A, CMatrix &B)
{
	assert(A.GetType() & MATRIX_SQARE);

	int *indxc, *indxr, *ipiv;	// These three integer arrays are used for bookkeeping on the pivoting.
	int i, j, k, l, ll, icol, irow;
	float big, dum, pivinv, temp;

	int n = A.GetRowSize();
	int m = B.GetColSize();
	indxc = new int [n];
	indxr = new int [n];
	ipiv = new int [n];
	memset(ipiv, 0, n * sizeof (int));

	for (i = 0; i < n; i++)
	{	// This is the main loop over the columns to be reduced.
		big = 0.0;
		for (j = 0; j < n; j++)	// This is the outer loop of the search for a pivot element.
			if (ipiv[j] != 1)
				for (k = 0; k < n; k++)
				{
					if (ipiv[k] == 0)
					{
						if (fabs(A[j][k]) >= big)
						{
							big = fabs(A[j][k]);
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
					{
						m_nErrorCode = 1;
						strcpy_s(m_sErrorRoutine, "GaussJordan");
						delete []indxc;
						delete []indxr;
						delete []ipiv;
						return m_nErrorCode;
					}
				}
		ipiv[icol] ++;

		// We now have the pivot element, so we interchange rows, if needed, to put the pivot
		// element on the diagonal. The columns are not physically interchanged, only relabeled:
		// indxc[i], the column of the ith pivot element, is the ith column that is reduced, while
		// indxr[i] is the row in which that pivot element was originally located. If indxr[i] !=
		// indxc[i] there is an implied column interchange. With this form of bookkeeping, the
		// solution b's will end up in the correct order, and the inverse matrix will be scrambled
		// by columns.
		if (irow != icol)
		{
			for (l = 0; l < n; l++)
			{
				temp = A[irow][l];
				A[irow][l] = A[icol][l];
				A[icol][l] = temp;
			}
			for (l = 0; l < m; l++)
			{
				temp = B[irow][l];
				B[irow][l] = B[icol][l];
				B[icol][l] = temp;
			}
		}
		indxr[i] = irow;	// We are now ready to divide the pivot row by the
		indxc[i] = icol;	// pivot element, located at irow and icol.
		if (A[icol][icol] == 0.0)
		{
			m_nErrorCode = 1;
			strcpy_s(m_sErrorRoutine, "GaussJordan");
			delete []indxc;
			delete []indxr;
			delete []ipiv;
			return m_nErrorCode;
		}
		pivinv = 1. / A[icol][icol];
		A[icol][icol] = 1.;
		for (l = 0; l < n; l++)
			A[icol][l] *= pivinv;
		for (l = 0; l < m; l++)
			B[icol][l] *= pivinv;
		for (ll = 0; ll < n; ll++)	// Next, we reduce the rows...
			if (ll != icol)			// ...except for the pivot one, of course.
			{
				dum = A[ll][icol];
				A[ll][icol] = 0.;
				for (l = 0; l < n; l++)
					A[ll][l] -= A[icol][l] * dum;
				for (l = 0; l < m; l++)
					B[ll][l] -= B[icol][l] * dum;
			}
	}

	// This is the end of the main loop over columns of the reduction. It only remains to unscramble
	// the solution in view of the column interchanges. We do this by interchanging pairs of
	// columns in the reverse order that the permutation was built up.
	for (l = n - 1; l >= 0; l--)
	{
		if (indxr[l] != indxc[l])
			for (k = 0; k < n; k++)
			{
				temp = A[k][indxr[l]];
				A[k][indxr[l]] = A[k][indxc[l]];
				A[k][indxc[l]] = temp;
			}
	}	// And we are done.
	delete []ipiv;
	delete []indxr;
	delete []indxc;
	return m_nErrorCode;
}

// Given a matrix A, this routine replaces it by the LU decomposition of a rowwise
// permutation of itself. a and n are input. a is output;
// indx[1..n] is an output vector that records the row permutation effected by the partial
// pivoting; d is output as +/-1 depending on whether the number of row interchanges was even
// or odd, respectively. This routine is used in combination with LUBacksubstitution to solve linear equations
// or invert a matrix.
_RESULT CMatrix::LUDecomposition(CMatrix &A, int *indx, float &d)
{
	assert(A.GetType() & MATRIX_SQARE);

	int i, j, k, imax;
	float big, dum, sum, temp;
	int n = A.GetColSize();
	CVector vv(n);	// vv stores the implicit scaling of each row.

	d = 1.;			// No row interchanges yet.
	for (i = 0; i < n; i++)
	{	// Loop over rows to get the implicit scaling information.
		big = 0.;
		for (j = 0; j < n; j++)
			if ((temp = fabs(A[i][j])) > big) big = temp;
		if (big == 0.0)
		{
			m_nErrorCode = 1;
			strcpy_s(m_sErrorRoutine, "LUDecomposition");
			return m_nErrorCode;
		}
		// No nonzero largest element.
		vv[i] = 1. / big;	// Save the scaling.
	}
	for (j = 0; j < n; j++)
	{	// This is the loop over columns of Crout's method.
		for (i = 0; i < j; i++)
		{
			sum = A[i][j];
			for (k = 0; k < i; k++)
				sum -= A[i][k] * A[k][j];
			A[i][j]=sum;
		}
		big = 0.;	// Initialize for the search for largest pivot element.
		for (i = j; i < n; i++)
		{
			sum = A[i][j];
			for (k = 0; k < j; k++)
				sum -= A[i][k] * A[k][j];
			A[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big)
			{
				// Is the figure of merit for the pivot better than the best so far?
				big = dum;
				imax = i;
			}
		}
		if (j != imax)
		{	// Do we need to interchange rows?
			for (k = 0; k < n; k++)
			{	// Yes, do so...
				dum = A[imax][k];
				A[imax][k] = A[j][k];
				A[j][k] = dum;
			}
			d = -d;	// ...and change the parity of d.
			vv[imax] = vv[j];	// Also interchange the scale factor.
		}
		indx[j] = imax;
		if (A[j][j] == 0.0) A[j][j] = TINY;
		// If the pivot element is zero the matrix is singular (at least to the precision of the
		// algorithm). For some applications on singular matrices, it is desirable to substitute
		// TINY for zero.
		if (j != n)
		{	// Now, finally, divide by the pivot element.
			dum = 1. / A[j][j];
			for (i = j + 1; i < n; i++) A[i][j] *= dum;
		}
	}	// Go back for the next column in the reduction.
	return m_nErrorCode;
}

// Solves the set of n linear equations A.X = B. Here A is input, not as the matrix
// A but rather as its LU decomposition, determined by the routine ludcmp. indx[1..n] is input
// as the permutation vector returned by ludcmp. B is input as the right-hand side vector
// B, and returns with the solution vector X. a, n, and indx are not modified by this routine
// and can be left in place for successive calls with different right-hand sides b. This routine takes
// into account the possibility that b will begin with many zero elements, so it is e.cient for use
// in matrix inversion.
_RESULT CMatrix::LUBackstitution(CMatrix &A, int *indx, CVector &B)
{
	assert(A.GetType() & MATRIX_SQARE);
	int i, ii = -1, ip, j;
	float sum;
	int n = A.GetColSize();

	for (i = 0; i < n; i++)
	{							// When ii is set to a non-negative value, it will become the
		ip = indx[i];			// index of the first nonvanishing element of b. Wenow
		sum = B[ip];			// do the forward substitution, equation (2.3.6). The
		B[ip] = B[i];			// only new wrinkle is to unscramble the permutation
		if (ii >= 0)			// as we go.
			for (j = ii; j <= i-1; j++)
				sum -= A[i][j] * B[j];
		else
			if (sum) ii = i;	// A nonzero element was encountered, so from now on we
		B[i] = sum;				// will have to do the sums in the loop above.
	}
	for (i = n - 1; i >= 0; i--)
	{							// Now we do the backsubstitution, equation (2.3.7).
		sum = B[i];
		for (j = i+1; j < n; j++)
			sum -= A[i][j] * B[j];
		B[i] = sum / A[i][i];	// Store a component of the solution vector X.
	}							// All done!
	return m_nErrorCode;
}

// Given a matrix A[1..m][1..n], this routine computes its singular value decomposition,
// A=U.W.V(T). The matrix U replaces A on output. The diagonal matrix of singular values W is output
// as a vector W[1..n]. The matrix V (not the transpose V(T) ) is output as V[1..n][1..n].
_RESULT CMatrix::SingularValueDecomposition(CMatrix &A, CVector &W, CMatrix &V)
{
	int i, j, k, l, flag, its, jj, nm;
	float anorm, c, f, g, h, s, scale, x, y, z;
	int m = A.GetRowSize();
	int n = A.GetColSize();
	CVector rv1(n);

	W.SetSize(n);
	V.SetSize(n, n);

	g = scale = anorm = 0.0;		// Householder reduction to bidiagonal form.
	for (i = 0; i < n; i++)
	{
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m)
		{
			for (k = i; k < m; k++)
			{
				scale += fabs(A[k][i]);
			}
			if (scale)
			{
				for (k = i; k < m; k++)
				{
					A[k][i] /= scale;
					s += A[k][i] * A[k][i];
				}
				f = A[i][i];
				g = -sign(sqrt(s), f);
				h = f * g - s;
				A[i][i] = f - g;
				for (j = l; j < n; j++)
				{
					for (s = 0.0, k = i; k < m; k++)
						s += A[k][i] * A[k][j];
					f = s / h;
					for (k = i; k < m; k++)
						A[k][j] += f * A[k][i];
				}
				for (k = i; k < m; k++)
					A[k][i] *= scale;
			}
		}
		W[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m && i != n - 1)
		{
			for (k = l; k < n; k++)
			{
				scale += fabs(A[i][k]);
			}
			if (scale)
			{
				for (k = l; k < n; k++)
				{
					A[i][k] /= scale;
					s += A[i][k] * A[i][k];
				}
				f = A[i][l];
				g = -sign(sqrt(s), f);
				h = f * g - s;
				A[i][l] = f - g;
				for (k = l; k < n; k++)
					rv1[k] = A[i][k] / h;
				for (j = l; j < m; j++)
				{
					for (s = 0.0, k = l; k < n; k++)
						s += A[j][k] * A[i][k];
					for (k = l; k < n; k++)
						A[j][k] += s * rv1[k];
				}
				for (k = l; k < n; k++)
					A[i][k] *= scale;
			}
		}
		anorm = Max(anorm, (fabs(W[i]) + fabs(rv1[i])));
	}
	for (i = n - 1; i >= 0; i--)
	{		// Accumulation of right-hand transformations.
		if (i < n - 1)
		{
			if (g)
			{
				for (j = l; j < n; j++)		// float division to avoid possible underflow.
					V[j][i] = (A[i][j] / A[i][l]) / g;
				for (j = l; j < n; j++)
				{
					for (s = 0.0, k = l; k < n; k++)
						s += A[i][k] * V[k][j];
					for (k = l; k < n; k++) V[k][j] += s * V[k][i];
				}
			}
			for (j = l; j < n; j++)
				V[i][j] = V[j][i] = 0.0;
		}
		V[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = (m < n ? m : n) - 1; i >= 0; i--)
	{		// Accumulation of left-hand transformations.
		l = i + 1;
		g = W[i];
		for (j = l; j < n; j++)
			A[i][j] = 0.0;
		if (g)
		{
			g = 1.0 / g;
			for (j = l; j < n; j++)
			{
				for (s = 0.0, k = l; k < m; k++)
					s += A[k][i] * A[k][j];
				f = (s / A[i][i]) * g;
				for (k = i; k < m; k++)
					A[k][j] += f * A[k][i];
			}
			for (j = i; j < m; j++)
				A[j][i] *= g;
		}
		else for (j = i; j < m; j++)
			A[j][i] = 0.0;
		A[i][i]++;
	}
	for (k = n - 1; k >= 0; k--)
	{		// Diagonalization of the bidiagonal form: Loop over
		for (its = 1; its <= 30; its++)
		{		// singular values, and over allowed iterations.
			flag = 1;
			for (l = k; l >= 0; l--)
			{		// Test for splitting.
				nm = l - 1;					// Note that rv1[1] is always zero.
				if ((float)(fabs(rv1[l]) + anorm) == anorm)
				{
					flag = 0;
					break;
				}
				if ((float)(fabs(W[nm]) + anorm) == anorm)
					break;
			}
			if (flag)
			{
				c = 0.0;					// Cancellation of rv1[l], if l > 1.
				s = 1.0;
				for (i = l; i <= k; i++)
				{
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if ((float)(fabs(f) + anorm) == anorm)
						break;
					g = W[i];
					h = pythag(f, g);
					W[i] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 0; j < m; j++)
					{
						y = A[j][nm];
						z = A[j][i];
						A[j][nm] = y * c + z * s;
						A[j][i] = z * c - y * s;
					}
				}
			}
			z = W[k];
			if (l == k)
			{			// Convergence.
				if (z < 0.0)
				{		// Singular value is made nonnegative.
					W[k] = -z;
					for (j = 0; j < n; j++)
						V[j][k] = -V[j][k];
				}
				break;
			}
			if (its == 30)
			{
				m_nErrorCode = 2;
				strcpy_s(m_sErrorRoutine, "SingularVlueDecomposition");
				return m_nErrorCode;
			}
			x = W[l];					// Shift from bottom 2-by-2 minor.
			nm = k - 1;
			y = W[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + sign(g, f))) - h)) / x;
			c = s = 1.0;				// Next QR transformation:
			for (j = l; j <= nm; j++)
			{
				i = j + 1;
				g = rv1[i];
				y = W[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 0; jj < n; jj++)
				{
					x = V[jj][j];
					z = V[jj][i];
					V[jj][j] = x * c + z * s;
					V[jj][i] = z * c - x * s;
				}
				z = pythag(f, h);
				W[j] = z;				// Rotation can be arbitrary if z = 0.
				if (z)
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for (jj = 0; jj < m; jj++)
				{
					y = A[jj][j];
					z = A[jj][i];
					A[jj][j] = y * c + z * s;
					A[jj][i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			W[k] = x;
		}
	}
	return m_nErrorCode;
}

// Computes sqrt(a*a + b*b) without destructive underflow or overflow.
float CMatrix::pythag(float a, float b)
{
	float absa, absb;
	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb) return absa * sqrt(1.0 + sqr(absb / absa));
	else return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + sqr(absa / absb)));
}

float CMatrix::sqr(float a)
{
	return (a == 0.0 ? 0.0 : a * a);
}

float CMatrix::sign(float a, float b)
{
	return (b >= 0.0 ? fabs(a) : -fabs(a));
}

float CMatrix::Max(float a, float b)
{
	return (a > b ? a : b);
}

#define OP_ROTATE(a,i,j,k,l) g=a[i][j]; h=a[k][l]; a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);
#define OP_MAX_ROTATIONS 20

// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in W; and output eigenvectors in V. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.

bool CMatrix::Jacobi(CMatrix &A, CVector &W, CMatrix &V)
{
	//assert(A.GetType() & MATRIX_SQARE);
	//assert(V.GetType() & MATRIX_SQARE);
	//assert(A.GetColSize() == V.GetColSize());
	//assert(A.GetColSize() == W.GetSize());
	//assert(A.GetColSize() < 5);

	int n = A.m_nCols;

	int i, j, k, ip, iq, numPos;
	float tresh, theta, tau, t, s, h, g, c, tmp;
	float *bspace=new float[n];
	float *zspace=new float[n];
	float *b = bspace;
	float *z = zspace;

	// initialize
	for (ip=0; ip<n; ip++) 
    {
		for (iq=0; iq<n; iq++)
			V[ip][iq] = 0.0;
		V[ip][ip] = 1.0;
    }
	for (ip=0; ip<n; ip++) 
	{
		b[ip] = W[ip] = A[ip][ip];
		z[ip] = 0.0;
	}

	// begin rotation sequence
	for (i=0; i< OP_MAX_ROTATIONS; i++) 
	{
		float sm = 0.0;
		for (ip=0; ip<n-1; ip++) 
		  for (iq=ip+1; iq<n; iq++)
			sm += fabs(A[ip][iq]);

		if (sm == 0.0) break;

		if ( i < 3 ) // first 3 sweeps
			tresh = 0.2*sm/(n*n);
		else
			tresh = 0.0;
	
		for (ip=0; ip<n-1; ip++) 
 			for (int iq=ip+1; iq<n; iq++) 
			{
				g = 100.0*fabs(A[ip][iq]);

				// after 4 sweeps
				if (i > 3 && (fabs(W[ip])+g) == fabs(W[ip])	&& (fabs(W[iq])+g) == fabs(W[iq]))
				{
					A[ip][iq] = 0.0;
				}
				else if (fabs(A[ip][iq]) > tresh) 
				{
					h = W[iq] - W[ip];
					if ( (fabs(h)+g) == fabs(h))
					{
						t = (A[ip][iq]) / h;
					}
					else 
					{
						theta = 0.5*h / (A[ip][iq]);
						t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c = 1.0 / sqrt(1+t*t);
					s = t*c;
					tau = s/(1.0+c);
					h = t*A[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					W[ip] -= h;
					W[iq] += h;
					A[ip][iq]=0.0;

					// ip already shifted left by 1 unit
					for (j = 0;j <= ip-1;j++) 
					{
						OP_ROTATE(A,j,ip,j,iq)
					}
	
					// ip and iq already shifted left by 1 unit
					for (j = ip+1;j <= iq-1;j++) 
					{
						OP_ROTATE(A,ip,j,j,iq)
					}

					// iq already shifted left by 1 unit
					for (j=iq+1; j<n; j++) 
					{
						OP_ROTATE(A,ip,j,iq,j)
					}
	
					for (j=0; j<n; j++) 
					{
						OP_ROTATE(V,j,ip,j,iq)
					}
				}
			}

		for (ip=0; ip<n; ip++) 
		{
			b[ip] += z[ip];
			W[ip] = b[ip];
			z[ip] = 0.0;
		}
	}

	if ( i >= OP_MAX_ROTATIONS ) 	//// this is NEVER called
	{
		delete[] bspace;
		delete[] zspace;
		return false;
	}

	// sort eigenfunctions                 these changes do not affect accuracy 
	for (j=0; j<n-1; j++)                  // boundary incorrect
	{
		k = j;
		tmp = W[k];
		for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
		{
			if (W[i] >= tmp)                   // why exchage if same?
			{
			k = i;
			tmp = W[k];
			}
		}

		if (k != j) 
		{
			W[k] = W[j];
			W[j] = tmp;
			for (i=0; i<n; i++) 
			{
				tmp = V[i][j];
				V[i][j] = V[i][k];
				V[i][k] = tmp;
			}
		}
	}

	for (j=0; j<n; j++)
	{
		for (numPos=0, i=0; i<n; i++)
			if ( V[i][j] >= 0.0 ) numPos++;

		if ( numPos < ceil(float(n)/float(2.0)) )
			for(i=0; i<n; i++)	V[i][j] *= -1.0;
	}

	delete[] bspace;
	delete[] zspace;
	return true;
}

#undef float

#undef OP_ROTATE
#undef OP_MAX_ROTATIONS
