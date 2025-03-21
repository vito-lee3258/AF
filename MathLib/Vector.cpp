#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Vector.h"
#include "Matrix.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define float double

CVector::CVector()
{
	m_nLen = 0;
	m_pData = NULL;
	m_nType = COL_VECTOR;
}

CVector::CVector(int len, VECTOR_TYPE nType)
{
	m_pData = NULL;
	SetSize(len, nType);
}

CVector::CVector(const CVector &vec)
{
	m_pData = NULL;
	SetSize(vec.GetSize(), vec.GetType());
	for (int i = 0; i < m_nLen; i++)
		(*this)[i] = vec[i];
}

CVector::~CVector()
{
	if (m_pData)
		delete []m_pData;
}

int CVector::GetSize() const
{
	return m_nLen;
}

VECTOR_TYPE CVector::GetType() const
{
	return m_nType;
}

void CVector::SetSize(int len, VECTOR_TYPE nType)
{
	assert(len >= 0);
	m_nType = nType;
	if (m_pData) delete []m_pData;
	m_nLen = len;
	if (len > 0)
		m_pData = new float [len];
	else
		m_pData = NULL;
}

float CVector::GetLength()
{
	float s=0;
    for (int i = 0; i < m_nLen; i++)
		s+=m_pData[i]*m_pData[i];
	return (float) sqrt(s);
}

CVector& CVector::EqSum(const CVector &refvectorA, const CVector &refvectorB)
{
    assert(m_nLen == refvectorA.GetSize()); 
    assert(m_nLen == refvectorB.GetSize()); 

    for (int i = 0; i < m_nLen; i++)
        (*this)[i] = refvectorA[i] + refvectorB[i];
    return *this; 
}

CVector& CVector::EqDiff(const CVector &refvectorA, const CVector &refvectorB)
{
    assert(m_nLen == refvectorA.GetSize()); 
    assert(m_nLen == refvectorB.GetSize()); 

    for (int i = 0; i < m_nLen; i++)
        (*this)[i] = refvectorA[i] - refvectorB[i];
    return *this; 
}

CVector & CVector::EqProd(const CMatrix &refmatrixA, const CVector &refvectorB)
{
    int nRows = refmatrixA.GetRowSize(); 
    int nCols = refmatrixA.GetColSize(); 
    assert(nRows == m_nLen); 
    assert(refmatrixA.GetColSize() == refvectorB.GetSize());

    for (int i = 0; i < nRows; i++)
	{
        float dblSum = 0.0;
        for (int j = 0; j < nCols; j++)
            dblSum += refmatrixA[i][j] * refvectorB[j];
        (*this)[i] = dblSum;
    }
	return (*this);
}

CVector& CVector::CrossProd(const CVector &refvectorA, const CVector &refvectorB)
{
	assert(refvectorA.GetSize() == 3);
	assert(refvectorB.GetSize() == 3);
	SetSize(3);
	(*this)[0] = refvectorA[1] * refvectorB[2] - refvectorB[1] * refvectorA[2];
	(*this)[1] = refvectorA[2] * refvectorB[0] - refvectorB[2] * refvectorA[0];
	(*this)[2] = refvectorA[0] * refvectorB[1] - refvectorB[0] * refvectorA[1];
	return (*this);
}

CMatrix CVector::Cross()
{
	assert(m_nLen == 3);
	CMatrix mat(3, 3);
	mat[0][0] = 0;
	mat[0][1] = -(*this)[2];
	mat[0][2] = (*this)[1];
	mat[1][0] = (*this)[2];
	mat[1][1] = 0;
	mat[1][2] = -(*this)[0];
	mat[2][0] = -(*this)[1];
	mat[2][1] = (*this)[0];
	mat[2][2] = 0;
	return mat;
}

CMatrix CVector::ExtProd(const CVector &A, const CVector &B)
{
	int m=A.GetSize();
	int n=B.GetSize();
	CMatrix M(m,n);
	for(int i=0;i<m;i++)
		for(int j=0;j<n;j++)
			M[i][j]=A[i]*B[j];
	return M;
}

CVector &CVector::Unitize()
{
	float norm = 0;
	for (int i = 0; i < m_nLen; i++)
		norm += m_pData[i] * m_pData[i];
	if (norm > 0)
	{
		norm = sqrt(norm);
		for (int i = 0; i < m_nLen; i++)
			m_pData[i] /= norm;
	}
	return *this;
}

CVector CVector::GetSubvector(int len) const
{
	assert(len >= 0);
	CVector vectorRet;
	if (len >= m_nLen)
		vectorRet = *this;
	else if (len > 0)
	{
		vectorRet.SetSize(len);
		for (int i = 0; i < len; i++)
			vectorRet[i] = (*this)[i];
	}
	return vectorRet;
}

CVector &CVector::Augment(int len)
{
	assert(len >= 0);
	if (len > 0)
	{
		float * temp = new float [len + m_nLen];
		for (int i = 0; i < m_nLen; i++)
			temp[i] = m_pData[i];
		if (m_pData) delete []m_pData;
		m_pData = temp;
		m_nLen += len;
	}
	return *this;
}

CVector CVector::operator+(const CVector& refvector) const
{
    assert(m_nLen == refvector.GetSize()); 
    CVector vectorRet(*this);
    return (vectorRet += refvector); 
}

CVector CVector::operator-(const CVector& refvector) const
{
    assert(m_nLen == refvector.GetSize()); 
    CVector vectorRet(*this);
    return (vectorRet -= refvector); 
}

CVector CVector::operator*(float dbl) const
{
    CVector vectorRet(*this);
    return (vectorRet *= dbl); 
}

float  CVector::operator*(const CVector& refvector) const
{
    float sum = 0.0;
    assert(m_nLen == refvector.GetSize());

    for (int i = 0; i < m_nLen; i++)
        sum += (*this)[i] * refvector[i];
    return sum;
}

CVector CVector::operator/(float dbl) const
{
	assert(dbl != 0);
	CVector vectorRet(*this);
	return (vectorRet /= dbl); 
}

CVector CVector::operator-(void) const
{
    CVector vectorRet(m_nLen);
    for (int i=0; i < m_nLen; i++)
        vectorRet[i] = - (*this)[i];
    return vectorRet; 
}

CVector& CVector::operator=(float value)
{
	if ((value == 0.0) && (m_nLen != 0))
	{
		assert(m_nLen > 0);
		// IEEE float
		memset(m_pData, 0, m_nLen * sizeof(float));
	}
	else
	{
		for (int i = 0; i < m_nLen; i++)
			m_pData[i] = value;
	}
	return *this;
}

CVector & CVector::operator =(const CVector &vec)
{
	SetSize(vec.GetSize());
	for (int i = 0; i < m_nLen; i++)
		(*this)[i] = vec[i];
	return *this;
}

CVector& CVector::operator+=(const CVector& refvector)
{
    return EqSum(*this, refvector); 
}

CVector& CVector::operator-=(const CVector& refvector)
{
    return EqDiff(*this, refvector); 
}

CVector& CVector::operator*=(float dbl)
{
    for (int i = 0; i < m_nLen; i++)
        (*this)[i] *= dbl;
    return *this; 
}

CVector& CVector::operator/=(float dbl)
{
	assert(dbl != 0);
    for (int i = 0; i < m_nLen; i++)
        (*this)[i] /= dbl;
    return *this; 
}

bool CVector::operator==(const CVector& refvector) const
{
    if (m_nLen == refvector.GetSize())
	{
		if (m_nLen == 0)
			return true;
		if (memcmp(m_pData, &(refvector[0]), m_nLen * sizeof(float)) == 0)
			return true;
	}
    return false; 
}

bool CVector::operator!=(const CVector& refvector) const
{
    return !(*this == refvector);
}

#undef float