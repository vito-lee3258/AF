#ifndef __VECTOR_H__
#define __VECTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum enum_vector_type {ROW_VECTOR, COL_VECTOR} VECTOR_TYPE;

class CMatrix;

//#define float double

class CVector  
{
public:
	CVector();
	CVector(int len, VECTOR_TYPE nType = COL_VECTOR);
	CVector(const CVector &vec);
	virtual ~CVector();

	int GetSize() const;
	VECTOR_TYPE GetType() const;
	void SetSize(int len, VECTOR_TYPE nType = COL_VECTOR);

	CVector& EqSum(const CVector &refvectorA, const CVector &refvectorB);
	CVector& EqDiff(const CVector &refvectorA, const CVector &refvectorB);
	CVector& EqProd(const CMatrix &refmatrixA, const CVector &refvectorB);
	CVector& CrossProd(const CVector &refvectorA, const CVector &refvectorB);
	CVector GetSubvector(int len) const;
	CVector& Augment(int len);
	CMatrix Cross();
	CVector &Unitize();

	float GetLength();
	CMatrix ExtProd(const CVector &A, const CVector &B);

// overload operators
    float& operator[](int i) { return m_pData[i]; }
    const float& operator[](int i) const { return m_pData[i]; }

	CVector operator+(const CVector& refvector) const;
	CVector operator-(const CVector& refvector) const;
	CVector operator*(float dbl) const;
	float  operator*(const CVector& refvector) const;
	CVector operator/(float dbl) const;
	CVector operator-(void) const;

	CVector& operator=(float value);
	CVector& operator=(const CVector &vec);
	CVector& operator+=(const CVector& refvector);
	CVector& operator-=(const CVector& refvector);
	CVector& operator*=(float dbl);
	CVector& operator/=(float dbl);

	bool operator==(const CVector& refvector) const;
	bool operator!=(const CVector& refvector) const;

protected:
	float *m_pData;
	int m_nLen;
	VECTOR_TYPE m_nType;
};

#undef float

#endif 