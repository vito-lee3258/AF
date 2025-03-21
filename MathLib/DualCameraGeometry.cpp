#include <math.h>
#include "DualCameraGeometry.h"

bool DLT_Homograph(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &H)
{
	int i, n;
	float CX1, CY1, CX2, CY2;
	float S1, S2;
	CMatrix A(2 * nNum, 9);

	if (nNum < 4)return false;

	CX1 = CY1 = CX2 = CY2 = 0.0;
	for (n = 0; n < nNum; n++)
	{
		CX1 += pMainXYList[n][0];
		CY1 += pMainXYList[n][1];

		CX2 += pSecondXYList[n][0];
		CY2 += pSecondXYList[n][1];
	}
	CX1 /= nNum;
	CY1 /= nNum;
	CX2 /= nNum;
	CY2 /= nNum;

	S1 = S2 = 0.0;
	for (n = 0; n < nNum; n++)
	{
		float fX1 = pMainXYList[n][0] - CX1;
		float fY1 = pMainXYList[n][1] - CY1;
		float fX2 = pSecondXYList[n][0] - CX2;
		float fY2 = pSecondXYList[n][1] - CY2;
		S1 += (float) sqrt(fX1 * fX1 + fY1 * fY1);
		S2 += (float) sqrt(fX2 * fX2 + fY2 * fY2);
	}
	S1 /= nNum;
	S2 /= nNum;
	if (S1 == 0 || S2 == 0)
	{
		return false;
	}

	CMatrix T1(3, 3);
	CMatrix T2(3, 3);

	T1[0][0] = sqrt(2) / S1;	T1[0][1] = 0.0;				T1[0][2] = -CX1*sqrt(2) / S1;
	T1[1][0] = 0.0;				T1[1][1] = sqrt(2) / S1;	T1[1][2] = -CY1*sqrt(2) / S1;
	T1[2][0] = 0.0;				T1[2][1] = 0.0;				T1[2][2] = 1.0;
	T1.Inverse();

	T2[0][0] = sqrt(2) / S2;	T2[0][1] = 0.0;				T2[0][2] = -CX2*sqrt(2) / S2;
	T2[1][0] = 0.0;				T2[1][1] = sqrt(2) / S2;	T2[1][2] = -CY2*sqrt(2) / S2;
	T2[2][0] = 0.0;				T2[2][1] = 0.0;				T2[2][2] = 1.0;

	for (n = 0; n < nNum; n++)
	{
		float fX1 = pMainXYList[n][0] - CX1;
		float fY1 = pMainXYList[n][1] - CY1;
		float fX2 = pSecondXYList[n][0] - CX2;
		float fY2 = pSecondXYList[n][1] - CY2;

		fX1 = (fX1 * sqrt(2)) / S1;
		fY1 = (fY1 * sqrt(2)) / S1;

		fX2 = (fX2 * sqrt(2)) / S2;
		fY2 = (fY2 * sqrt(2)) / S2;


		A[n * 2][0] = A[n * 2][1] = A[n * 2][2] = 0;
		A[n * 2][3] = -fX2;
		A[n * 2][4] = -fY2;
		A[n * 2][5] = -1.0;
		A[n * 2][6] = fY1 * fX2;
		A[n * 2][7] = fY1 * fY2;
		A[n * 2][8] = fY1;

		A[n * 2 + 1][0] = fX2;
		A[n * 2 + 1][1] = fY2;
		A[n * 2 + 1][2] = 1.0;
		A[n * 2 + 1][3] = A[n * 2 + 1][4] = A[n * 2 + 1][5] = 0;
		A[n * 2 + 1][6] = -fX1 * fX2;
		A[n * 2 + 1][7] = -fX1 * fY2;
		A[n * 2 + 1][8] = -fX1;
	}

	CVector W;
	CMatrix V;
	A.SingularValueDecomposition(A, W, V);

	int mini = 0;
	float fMinW = W[0];

	for (i = 1; i < W.GetSize(); i++)
	{
		if (W[i] < fMinW)
		{
			fMinW = W[i];
			mini = i;
		}
	}

	H[0][0] = V[0][mini];
	H[0][1] = V[1][mini];
	H[0][2] = V[2][mini];

	H[1][0] = V[3][mini];
	H[1][1] = V[4][mini];
	H[1][2] = V[5][mini];

	H[2][0] = V[6][mini];
	H[2][1] = V[7][mini];
	H[2][2] = V[8][mini];

	H = T1*H*T2;

	if (H[2][2] != 0)
	{
		H /= H[2][2];
	}

	return true;
}

void Homograph2Rotation(CMatrix &H, CMatrix &R, bool bAvgS)
{
	int i;
	CVector W(3);
	CMatrix V(3, 3);

	float s0 = 0;
	float s1 = 0;
	for (i = 0; i < 3; i++)
	{
		s0 += H[i][0] * H[i][0];
		s1 += H[i][1] * H[i][1];
	}
	s0 = sqrt(s0);
	s1 = sqrt(s1);
	float s = (s0 + s1) / 2.0;

	float u[3];
	float v[3];
	if (bAvgS)
	{
		for (i = 0; i < 3; i++)
		{
			u[i] = R[i][0] = H[i][0] / s0;
			v[i] = R[i][1] = H[i][1] / s1;
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			u[i] = R[i][0] = H[i][0] / s;
			v[i] = R[i][1] = H[i][1] / s;
		}
	}
	R[0][2] = u[1] * v[2] - u[2] * v[1];
	R[1][2] = u[2] * v[0] - u[0] * v[2];
	R[2][2] = u[0] * v[1] - u[1] * v[0];

	R.SingularValueDecomposition(R, W, V);
	R = R*V.Transposed();
}

bool FundamentalMatrix_8Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F)
{
	int i, mini;
	CMatrix T1(3, 3);
	CMatrix T2(3, 3);
	CMatrix D(3, 3);
	CMatrix A(nNum, 9);
	CVector f(9);

	if (nNum < 8)return false;

	T1.MakeUnit();
	T2.MakeUnit();
	D.MakeZero();

	float fMean1[2], fVar1[2], fMean2[2], fVar2[2];

	fMean1[0] = fMean1[1] = fMean2[0] = fMean2[1] = 0.0;
	fVar1[0] = fVar1[1] = fVar2[0] = fVar2[1] = 0.0;
	for (i = 0; i < nNum; i++)
	{
		fMean1[0] += pMainXYList[i][0];
		fMean1[1] += pMainXYList[i][1];

		fVar1[0] += pMainXYList[i][0] * pMainXYList[i][0];
		fVar1[1] += pMainXYList[i][1] * pMainXYList[i][1];

		fMean2[0] += pSecondXYList[i][0];
		fMean2[1] += pSecondXYList[i][1];

		fVar2[0] += pSecondXYList[i][0] * pSecondXYList[i][0];
		fVar2[1] += pSecondXYList[i][1] * pSecondXYList[i][1];
	}
	fMean1[0] /= nNum;
	fMean1[1] /= nNum;
	fVar1[0] /= nNum;	fVar1[0] -= fMean1[0] * fMean1[0];	fVar1[0] = sqrt(fVar1[0]);
	fVar1[1] /= nNum;	fVar1[1] -= fMean1[1] * fMean1[1];	fVar1[1] = sqrt(fVar1[1]);

	if (fVar1[0] == 0 || fVar1[1] == 0)return false;

	fMean2[0] /= nNum;
	fMean2[1] /= nNum;
	fVar2[0] /= nNum;	fVar2[0] -= fMean2[0] * fMean2[0];	fVar2[0] = sqrt(fVar2[0]);
	fVar2[1] /= nNum;	fVar2[1] -= fMean2[1] * fMean2[1];	fVar2[1] = sqrt(fVar2[1]);

	if (fVar2[0] == 0 || fVar2[1] == 0)return false;

	T1[0][0] = 1.0 / fVar1[0];	T1[0][2] = -fMean1[0] / fVar1[0];
	T1[1][1] = 1.0 / fVar1[1];	T1[1][2] = -fMean1[1] / fVar1[1];

	T2[0][0] = 1.0 / fVar2[0];	T2[0][2] = -fMean2[0] / fVar2[0];
	T2[1][1] = 1.0 / fVar2[1];	T2[1][2] = -fMean2[1] / fVar2[1];

	for (i = 0; i < nNum; i++)
	{
		float fX1 = (pMainXYList[i][0] - fMean1[0]) / fVar1[0];
		float fY1 = (pMainXYList[i][1] - fMean1[1]) / fVar1[1];

		float fX2 = (pSecondXYList[i][0] - fMean2[0]) / fVar2[0];
		float fY2 = (pSecondXYList[i][1] - fMean2[1]) / fVar2[1];
		
		A[i][0] = fX1 * fX2;
		A[i][1] = fX1 * fY2;
		A[i][2] = fX1;

		A[i][3] = fY1 * fX2;
		A[i][4] = fY1 * fY2;
		A[i][5] = fY1;

		A[i][6] = fX2;
		A[i][7] = fY2;
		A[i][8] = 1.0;
	}
	CVector W;
	CMatrix V;
	A.SingularValueDecomposition(A, W, V);

	mini = 0;
	for (i = 1; i < 9; i++)
	{
		if (W[i] < W[mini])
		{
			mini = i;
		}
	}
	for (i = 0; i < 9; i++)
	{
		f[i] = V[i][mini];
	}
	F[0][0] = f[0];	F[0][1] = f[1];	F[0][2] = f[2];
	F[1][0] = f[3];	F[1][1] = f[4];	F[1][2] = f[5];
	F[2][0] = f[6];	F[2][1] = f[7];	F[2][2] = f[8];

	F.SingularValueDecomposition(F, W, V);
	mini = 0;
	for (i = 1; i < 3; i++)
	{
		if (W[i] < W[mini])
		{
			mini = i;
		}
	}
	V.Transpose();

	for (i = 0; i < 3; i++)
	{
		D[i][i] = (i == mini) ? (0.0) : W[i];
	}
	F = F*D*V;

	T1.Transpose();
	F = T1*F*T2;

	float f12 = F[1][2];

	if (f12 == 0)return false;

	F /= -f12;

	return true;
}

bool FundamentalMatrix_2Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F)
{
	int i, mini;
	CMatrix T1(3, 3);
	CMatrix T2(3, 3);
	CMatrix A(nNum, 3);
	CVector f(3);

	if (nNum < 2)return false;

	T1.MakeUnit();
	T2.MakeUnit();

	float fMean1[2], fVar1[2], fMean2[2], fVar2[2];

	fMean1[0] = fMean1[1] = fMean2[0] = fMean2[1] = 0.0;
	fVar1[0] = fVar1[1] = fVar2[0] = fVar2[1] = 0.0;
	for (i = 0; i < nNum; i++)
	{
		fMean1[0] += pMainXYList[i][0];
		fMean1[1] += pMainXYList[i][1];

		fVar1[0] += pMainXYList[i][0] * pMainXYList[i][0];
		fVar1[1] += pMainXYList[i][1] * pMainXYList[i][1];

		fMean2[0] += pSecondXYList[i][0];
		fMean2[1] += pSecondXYList[i][1];

		fVar2[0] += pSecondXYList[i][0] * pSecondXYList[i][0];
		fVar2[1] += pSecondXYList[i][1] * pSecondXYList[i][1];
	}
	fMean1[0] /= nNum;
	fMean1[1] /= nNum;
	fVar1[0] /= nNum;	fVar1[0] -= fMean1[0] * fMean1[0];	fVar1[0] = (float)sqrt(fVar1[0]);
	fVar1[1] /= nNum;	fVar1[1] -= fMean1[1] * fMean1[1];	fVar1[1] = (float)sqrt(fVar1[1]);

	fMean2[0] /= nNum;
	fMean2[1] /= nNum;
	fVar2[0] /= nNum;	fVar2[0] -= fMean2[0] * fMean2[0];	fVar2[0] = (float)sqrt(fVar2[0]);
	fVar2[1] /= nNum;	fVar2[1] -= fMean2[1] * fMean2[1];	fVar2[1] = (float)sqrt(fVar2[1]);

	if (fVar1[0] == 0 || fVar1[1] == 0 || fVar2[0] == 0 || fVar2[1] == 0)return false;

	T1[0][0] = 1.0 / fVar1[0];	T1[0][2] = -fMean1[0] / fVar1[0];
	T1[1][1] = 1.0 / fVar1[1];	T1[1][2] = -fMean1[1] / fVar1[1];

	T2[0][0] = 1.0 / fVar2[0];	T2[0][2] = -fMean2[0] / fVar2[0];
	T2[1][1] = 1.0 / fVar2[1];	T2[1][2] = -fMean2[1] / fVar2[1];

	for (i = 0; i < nNum; i++)
	{
		float fY1 = (pMainXYList[i][1] - fMean1[1]) / fVar1[1];
		float fY2 = (pSecondXYList[i][1] - fMean2[1]) / fVar2[1];

		A[i][0] = fY1;
		A[i][1] = fY2;
		A[i][2] = 1.0;
	}
	CVector W;
	CMatrix V;
	A.SingularValueDecomposition(A, W, V);

	mini = 0;
	for (i = 1; i < 3; i++)
	{
		if (W[i] < W[mini])
		{
			mini = i;
		}
	}
	for (i = 0; i < 3; i++)
	{
		f[i] = V[i][mini];
	}
	F[0][0] = 0;	F[0][1] = 0;	F[0][2] = 0;
	F[1][0] = 0;	F[1][1] = 0;	F[1][2] = f[0];
	F[2][0] = 0;	F[2][1] = f[1];	F[2][2] = f[2];

	T1.Transpose();
	F = T1*F*T2;

	float f12 = F[1][2];
	if (f12 == 0)return false;
	F /= -f12;

	return true;
}

bool FundamentalMatrix_3Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F)
{
	int i, mini;
	CMatrix T1(3, 3);
	CMatrix T2(3, 3);
	CMatrix A(nNum, 4);
	CVector f(4);

	if (nNum < 3)return false;

	T1.MakeUnit();
	T2.MakeUnit();

	float fMean1[2], fVar1[2], fMean2[2], fVar2[2];

	fMean1[0] = fMean1[1] = fMean2[0] = fMean2[1] = 0.0;
	fVar1[0] = fVar1[1] = fVar2[0] = fVar2[1] = 0.0;
	for (i = 0; i < nNum; i++)
	{
		fMean1[0] += pMainXYList[i][0];
		fMean1[1] += pMainXYList[i][1];

		fVar1[0] += pMainXYList[i][0] * pMainXYList[i][0];
		fVar1[1] += pMainXYList[i][1] * pMainXYList[i][1];

		fMean2[0] += pSecondXYList[i][0];
		fMean2[1] += pSecondXYList[i][1];

		fVar2[0] += pSecondXYList[i][0] * pSecondXYList[i][0];
		fVar2[1] += pSecondXYList[i][1] * pSecondXYList[i][1];
	}
	fMean1[0] /= nNum;
	fMean1[1] /= nNum;
	fVar1[0] /= nNum;	fVar1[0] -= fMean1[0] * fMean1[0];	fVar1[0] = (float)sqrt(fVar1[0]);
	fVar1[1] /= nNum;	fVar1[1] -= fMean1[1] * fMean1[1];	fVar1[1] = (float)sqrt(fVar1[1]);

	fMean2[0] /= nNum;
	fMean2[1] /= nNum;
	fVar2[0] /= nNum;	fVar2[0] -= fMean2[0] * fMean2[0];	fVar2[0] = (float)sqrt(fVar2[0]);
	fVar2[1] /= nNum;	fVar2[1] -= fMean2[1] * fMean2[1];	fVar2[1] = (float)sqrt(fVar2[1]);

	if (fVar1[0] == 0 || fVar1[1] == 0 || fVar2[0] == 0 || fVar2[1] == 0)return false;

	T1[0][0] = 1.0 / fVar1[0];	T1[0][2] = -fMean1[0] / fVar1[0];
	T1[1][1] = 1.0 / fVar1[1];	T1[1][2] = -fMean1[1] / fVar1[1];

	T2[0][0] = 1.0 / fVar2[0];	T2[0][2] = -fMean2[0] / fVar2[0];
	T2[1][1] = 1.0 / fVar2[1];	T2[1][2] = -fMean2[1] / fVar2[1];

	for (i = 0; i < nNum; i++)
	{
		float fY1 = (pMainXYList[i][1] - fMean1[1]) / fVar1[1];
		float fX2 = (pSecondXYList[i][0] - fMean2[0]) / fVar2[0];
		float fY2 = (pSecondXYList[i][1] - fMean2[1]) / fVar2[1];
		
		A[i][0] = fY1;
		A[i][1] = fX2;
		A[i][2] = fY2;
		A[i][3] = 1.0;
	}
	CVector W;
	CMatrix V;
	A.SingularValueDecomposition(A, W, V);

	mini = 0;
	for (i = 1; i < 4; i++)
	{
		if (W[i] < W[mini])
		{
			mini = i;
		}
	}
	for (i = 0; i < 4; i++)
	{
		f[i] = V[i][mini];
	}
	F[0][0] = 0;	F[0][1] = 0;	F[0][2] = 0;
	F[1][0] = 0;	F[1][1] = 0;	F[1][2] = f[0];
	F[2][0] = f[1];	F[2][1] = f[2];	F[2][2] = f[3];

	T1.Transpose();
	F = T1*F*T2;

	float f12 = F[1][2];
	if (f12 == 0)return false;
	F /= -f12;

	return true;
}

bool FundamentalMatrix_5Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F)
{
	int i, mini;
	CMatrix T1(3, 3);
	CMatrix T2(3, 3);
	CMatrix A(nNum, 6);
	CVector f(6);

	if (nNum < 5)return false;

	T1.MakeUnit();
	T2.MakeUnit();

	float fMean1[2], fVar1[2], fMean2[2], fVar2[2];

	fMean1[0] = fMean1[1] = fMean2[0] = fMean2[1] = 0.0;
	fVar1[0] = fVar1[1] = fVar2[0] = fVar2[1] = 0.0;
	for (i = 0; i < nNum; i++)
	{
		fMean1[0] += pMainXYList[i][0];
		fMean1[1] += pMainXYList[i][1];

		fVar1[0] += pMainXYList[i][0] * pMainXYList[i][0];
		fVar1[1] += pMainXYList[i][1] * pMainXYList[i][1];

		fMean2[0] += pSecondXYList[i][0];
		fMean2[1] += pSecondXYList[i][1];

		fVar2[0] += pSecondXYList[i][0] * pSecondXYList[i][0];
		fVar2[1] += pSecondXYList[i][1] * pSecondXYList[i][1];
	}
	fMean1[0] /= nNum;
	fMean1[1] /= nNum;
	fVar1[0] /= nNum;	fVar1[0] -= fMean1[0] * fMean1[0];	fVar1[0] = (float)sqrt(fVar1[0]);
	fVar1[1] /= nNum;	fVar1[1] -= fMean1[1] * fMean1[1];	fVar1[1] = (float)sqrt(fVar1[1]);

	fMean2[0] /= nNum;
	fMean2[1] /= nNum;
	fVar2[0] /= nNum;	fVar2[0] -= fMean2[0] * fMean2[0];	fVar2[0] = (float)sqrt(fVar2[0]);
	fVar2[1] /= nNum;	fVar2[1] -= fMean2[1] * fMean2[1];	fVar2[1] = (float)sqrt(fVar2[1]);

	if (fVar1[0] == 0 || fVar1[1] == 0 || fVar2[0] == 0 || fVar2[1] == 0)return false;

	T1[0][0] = 1.0 / fVar1[0];	T1[0][2] = -fMean1[0] / fVar1[0];
	T1[1][1] = 1.0 / fVar1[1];	T1[1][2] = -fMean1[1] / fVar1[1];

	T2[0][0] = 1.0 / fVar2[0];	T2[0][2] = -fMean2[0] / fVar2[0];
	T2[1][1] = 1.0 / fVar2[1];	T2[1][2] = -fMean2[1] / fVar2[1];

	for (i = 0; i < nNum; i++)
	{
		float fY1 = (pMainXYList[i][1] - fMean1[1]) / fVar1[1];
		float fX2 = (pSecondXYList[i][0] - fMean2[0]) / fVar2[0];
		float fY2 = (pSecondXYList[i][1] - fMean2[1]) / fVar2[1];

		A[i][0] = fX2*fY1;
		A[i][1] = fY2*fY1;
		A[i][2] = fY1;

		A[i][3] = fX2;
		A[i][4] = fY2;
		A[i][5] = 1.0;
	}

	CVector W;
	CMatrix V;
	A.SingularValueDecomposition(A, W, V);

	mini = 0;
	for (i = 1; i < 6; i++)
	{
		if (W[i] < W[mini])
		{
			mini = i;
		}
	}
	for (i = 0; i < 6; i++)
	{
		f[i] = V[i][mini];
	}
	F[0][0] = 0;	F[0][1] = 0;	F[0][2] = 0;
	F[1][0] = f[0];	F[1][1] = f[1];	F[1][2] = f[2];
	F[2][0] = f[3];	F[2][1] = f[4];	F[2][2] = f[5];

	T1.Transpose();
	F = T1*F*T2;

	float f12 = F[1][2];
	if (f12 == 0)return false;
	F /= -f12;

	return true;
}

void DecomposeRotation(CMatrix &R, float &fAngleX, float &fAngleY, float &fAngleZ)
{
	fAngleX = atan2(R[2][1], R[2][2]);
	fAngleY = atan2(-R[2][0], sqrt(R[2][1] * R[2][1] + R[2][2] * R[2][2]));
	fAngleZ = atan2(R[1][0], R[0][0]);
}

void Angle2Rx(float fAngleX, CMatrix &Rx)
{
	float c = cos(fAngleX);
	float s = sin(fAngleX);

	Rx[0][0] = 1.0;		Rx[0][1] = 0.0;		Rx[0][2] = 0.0;
	Rx[1][0] = 0.0;		Rx[1][1] = c;		Rx[1][2] = -s;
	Rx[2][0] = 0.0;		Rx[2][1] = s;		Rx[2][2] = c;
}

void Angle2Ry(float fAngleY, CMatrix &Ry)
{
	float c = cos(fAngleY);
	float s = sin(fAngleY);

	Ry[0][0] = c;		Ry[0][1] = 0.0;		Ry[0][2] = s;
	Ry[1][0] = 0.0;		Ry[1][1] = 1.0;		Ry[1][2] = 0;
	Ry[2][0] = -s;		Ry[2][1] = 0.0;		Ry[2][2] = c;
}

void Angle2Rz(float fAngleZ, CMatrix &Rz)
{
	float c = cos(fAngleZ);
	float s = sin(fAngleZ);

	Rz[0][0] = c;		Rz[0][1] = -s;		Rz[0][2] = 0.0;
	Rz[1][0] = s;		Rz[1][1] = c;		Rz[1][2] = 0.0;
	Rz[2][0] = 0.0;		Rz[2][1] = 0.0;		Rz[2][2] = 1.0;
}

void Fundamental2Homograph2D(CMatrix &F, CMatrix &H)
{
	H[1][0] = F[2][0];	H[1][1] = F[2][1];	H[1][2] = F[2][2];
	H[2][0] = -F[1][0];	H[2][1] = -F[1][1];	H[2][2] = -F[1][2];

	H[0][0] = H[1][1];	H[0][1] = -H[1][0];	H[0][2] = 0;
}

bool Fundamental2RotationXY(CMatrix &F, CMatrix &R)
{
	CMatrix H(3, 3);
	float f1 = (float) sqrt(F[2][1] * F[2][1] + F[1][1] * F[1][1]);
	if (f1 == 0.0)return false;

	H[1][0] = F[2][0] / f1;		H[1][1] = F[2][1] / f1;		H[1][2] = F[2][2] / f1;
	H[2][0] = -F[1][0] / f1;	H[2][1] = -F[1][1] / f1;	H[2][2] = -F[1][2] / f1;

	float f2 = 1.0 - (H[1][0] * H[1][0] + H[2][0] * H[2][0]);
	if (f2 < 0.0)return false;
	H[0][0] = (float) sqrt(f2);		H[0][1] = 0;	H[0][2] = 0;

	Homograph2Rotation(H, R);

	return true;
}
