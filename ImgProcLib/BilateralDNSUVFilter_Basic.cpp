#include <math.h>
#include "BilateralDNSUVFilter_Basic.h"

int CBilateralDNSUVFilter_Basic::m_nMask1[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
int CBilateralDNSUVFilter_Basic::m_nMask2[25] = { 1, 4, 6, 4, 1, 4, 16, 24, 16, 4, 6, 24, 36, 24, 6, 4, 16, 24, 16, 4, 1, 4, 6, 4, 1 };

void CBilateralDNSUVFilter_Basic::BuildMask(int *pMask, int nWinW)
{
	int x, y;
	double fx;
	double fDistMask1D[17];
	double fa = (nWinW / 2.0);
	double ft = (double)fa;
	int nMask2D[17][17];

	ft = 2.0*ft*ft;
	fDistMask1D[nWinW] = 1.0;
	for (x = 1; x <= nWinW; x++)
	{
		fx = (double)x;
		fDistMask1D[nWinW + x] = pow(2.0, -fx*fx / ft);
		fDistMask1D[nWinW - x] = fDistMask1D[nWinW + x];
	}

	double fsum = 0.0;
	ft = fDistMask1D[0];
	for (x = 0; x <= 2 * nWinW; x++)
	{
		fDistMask1D[x] /= ft;
		fsum += fDistMask1D[x];
	}
	if (fsum>32)
	{
		for (x = 0; x <= 2 * nWinW; x++)
		{
			fDistMask1D[x] /= fsum;
		}
	}
	fsum = 0;
	for (y = 0; y <= 2 * nWinW; y++)
	{
		for (x = 0; x <= 2 * nWinW; x++)
		{
			fsum += fDistMask1D[y] * fDistMask1D[x];
		}
	}

	//	printf("DistMask2D Alpha=%f:\n", fa);
	int nSum = 0;
	for (y = 0; y <= 2 * nWinW; y++)
	{
		for (x = 0; x <= 2 * nWinW; x++)
		{
			int temp = (int)(fDistMask1D[y] * fDistMask1D[x] * 1024.0 / fsum + 0.5);
			nSum += temp;
			nMask2D[y][x] = temp;
		}
	}
	if (nMask2D[0][nWinW]>1)
	{
		int t = nMask2D[0][nWinW];
		nSum = 0;
		for (y = 0; y <= 2 * nWinW; y++)
		{
			for (x = 0; x <= 2 * nWinW; x++)
			{
				int temp = (int)(fDistMask1D[y] * fDistMask1D[x] * 1024.0 / fsum / t + 0.5);
				nSum += temp;
				nMask2D[y][x] = temp;
			}
		}
	}
	if (nSum>1024)
	{
		int t = nSum;
		nSum = 0;
		for (y = 0; y <= 2 * nWinW; y++)
		{
			for (x = 0; x <= 2 * nWinW; x++)
			{
				nMask2D[y][x] = (nMask2D[y][x] * 1024 / t);
				nSum += nMask2D[y][x];
			}
		}
	}

	int *ptr = pMask;
	for (y = 0; y <= 2 * nWinW; y++)
	{
		for (x = 0; x <= 2 * nWinW; x++)
		{
			*(ptr++) = nMask2D[y][x];
		}
	}
}

void CBilateralDNSUVFilter_Basic::RotateYUVLine(short *pInLine, short *pOutLine, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x;

	for (x = 0; x < nWidth; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pOutLine[i] = pInLine[i];
		}
		pInLine += nInPitch;
		pOutLine += nOutPitch;
	}
}

void CBilateralDNSUVFilter_Basic::DNSUVWindow(int UV[])
{
	int i, x, y, sumUV[2], sumW, dYUV[3], m, w, YUV0[3];
	int nWinS = 2 * m_nWinW + 1;

	YUV0[0] = m_pYUVWin[m_nWinW][m_nWinW * 3];
	YUV0[1] = m_pYUVWin[m_nWinW][m_nWinW * 3 + 1];
	YUV0[2] = m_pYUVWin[m_nWinW][m_nWinW * 3 + 2];

	sumUV[0] = sumUV[1] = sumW = 0;

	int *pMask = m_pMask;
	for (x = 0; x < nWinS; x++)
	{
		int *pYUV = m_pYUVWin[x];
		for (y = 0; y < nWinS; y++, pYUV+=3)
		{
			m = *(pMask++);
			if (m == 0)continue;

			for (i = 0; i < 3; i++)
			{
				dYUV[i] = pYUV[i] - YUV0[i];
				dYUV[i] *= dYUV[i];
			}
//			w = 8 - dYUV[0] / m_nNoiseY - (dYUV[1] + dYUV[2]) / m_nNoiseUV;
			__int64 t0 = dYUV[0];			t0 = (t0*m_nInvNoiseY) >> 24;
			__int64 t1 = dYUV[1] + dYUV[2];	t1 = (t1*m_nInvNoiseUV) >> 24;
			w = 8 - ((int)(t0 + t1));

			if (w >= 0)
			{
				m <<= w;
				sumUV[0] += pYUV[1] * m;
				sumUV[1] += pYUV[2] * m;
				sumW += m;
			}
		}
	}

	UV[0] = sumUV[0] / sumW;
	UV[1] = sumUV[1] / sumW;

	if (UV[0] < 0)UV[0] = 0;	if (UV[0] > m_nMAXS)UV[0] = m_nMAXS;
	if (UV[1] < 0)UV[1] = 0;	if (UV[1] > m_nMAXS)UV[1] = m_nMAXS;
}

void CBilateralDNSUVFilter_Basic::DNSUVLine(short *pInLines[], short *pOutLine, int nWidth, int nInPitch, int nOutPitch)
{
	int i, j, k, x, UV[2];
	int nWinS = 2 * m_nWinW + 1;
	short *pIn[17];

	for (i = 0; i < nWinS; i++)
	{
		pIn[i] = pInLines[i];
	}

	for (j = 0; j <= m_nWinW; j++)
	{
		m_pYUVWin[j] = m_pYUVWinBuffer;
	}
	for (; j < nWinS; j++)
	{
		m_pYUVWin[j] = m_pYUVWin[j - 1] + nWinS * 3;
	}

	for (j = m_nWinW; j < 2 * m_nWinW; j++)
	{
		int *pWin = m_pYUVWin[j];
		for (i = 0; i < nWinS; i++)
		{
			for (k = 0; k < 3; k++)
			{
				*(pWin++) = pIn[i][k];
			}
			pIn[i] += nInPitch;
		}
	}

	for (x = 0; x < m_nWinW; x++)
	{
		int *pWin = m_pYUVWin[2*m_nWinW];
		for (i = 0; i < nWinS; i++)
		{
			for (k = 0; k < 3; k++)
			{
				*(pWin++) = pIn[i][k];
			}
			pIn[i] += nInPitch;
		}

		DNSUVWindow(UV);
		pOutLine[1] = (short)UV[0];
		pOutLine[2] = (short)UV[1];
		pOutLine += nOutPitch;

		for (j = 0; j < 2 * m_nWinW; j++)
		{
			m_pYUVWin[j] = m_pYUVWin[j + 1];
		}
		m_pYUVWin[2 * m_nWinW] += nWinS * 3;
	}
	for (; x < nWidth-m_nWinW; x++)
	{
		int *pWin = m_pYUVWin[2 * m_nWinW];
		for (i = 0; i < nWinS; i++)
		{
			for (k = 0; k < 3; k++)
			{
				*(pWin++) = pIn[i][k];
			}
			pIn[i] += nInPitch;
		}

		DNSUVWindow(UV);
		pOutLine[1] = (short)UV[0];
		pOutLine[2] = (short)UV[1];
		pOutLine += nOutPitch;

		int *pTemp = m_pYUVWin[0];
		for (j = 0; j < 2 * m_nWinW; j++)
		{
			m_pYUVWin[j] = m_pYUVWin[j + 1];
		}
		m_pYUVWin[2 * m_nWinW] = pTemp;
	}
	for (; x < nWidth; x++)
	{
		DNSUVWindow(UV);
		pOutLine[1] = (short)UV[0];
		pOutLine[2] = (short)UV[1];
		pOutLine += nOutPitch;

		for (j = 0; j < 2 * m_nWinW; j++)
		{
			m_pYUVWin[j] = m_pYUVWin[j + 1];
		}
	}
}

bool CBilateralDNSUVFilter_Basic::DNSUVImage(CMultipleChannelImage *pYUVImage)
{
	int i, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();
	int nWinS = 2 * m_nWinW + 1;
	short *pInLines[17];

	if (nDim < 3)return false;
	if (m_nWinW == 1)
	{
		m_pMask = m_nMask1;
	}
	else if (m_nWinW == 2)
	{
		m_pMask = m_nMask2;
	}
	else if (m_nWinW == 4)
	{
		m_pMask = m_nMask4;
	}
	else if (m_nWinW == 8)
	{
		m_pMask = m_nMask8;
	}
	else
	{
		return false;
	}

	short *pBuffer = new short[nWidth * 3 * nWinS];
	if (pBuffer == NULL)return false;

	pInLines[m_nWinW] = pBuffer;
	for (i = m_nWinW + 1; i < nWinS; i++)
	{
		pInLines[i] = pInLines[i - 1] + 3;
	}
	for (i = 0; i < m_nWinW; i++)
	{
		pInLines[i] = pInLines[m_nWinW];
	}

	short *pInLine = pYUVImage->GetImageData();
	for (i = m_nWinW; i < 2 * m_nWinW; i++)
	{
		RotateYUVLine(pInLine, pInLines[i], nWidth, nDim, nWinS * 3);
		pInLine += nWidth*nDim;
	}

	for (y = 0; y < m_nWinW; y++)
	{
		RotateYUVLine(pInLine, pInLines[2*m_nWinW], nWidth, nDim, nWinS * 3);
		pInLine += nWidth*nDim;

		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3, nDim);

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * m_nWinW] += 3;
	}
	for (; y < nHeight - m_nWinW; y++)
	{
		RotateYUVLine(pInLine, pInLines[2 * m_nWinW], nWidth, nDim, nWinS * 3);
		pInLine += nWidth*nDim;

		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3, nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * m_nWinW] = pTemp;
	}
	pInLines[2 * m_nWinW] = pInLines[2 * m_nWinW - 1];
	for (; y < nHeight; y++)
	{
		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3, nDim);

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

