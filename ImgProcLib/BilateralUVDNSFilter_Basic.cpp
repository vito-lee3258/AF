#include <math.h>
#include "BilateralUVDNSFilter_Basic.h"

void CBilateralUVDNSFilter_Basic::BuildMask(int nWinW)
{
	int x, y;
	double fx;
	double fDistMask1D[17];
	double fa = (nWinW / 2.0);
	double ft = (double)fa;

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
			m_nMask2D[y][x] = temp;
		}
	}
	if (m_nMask2D[0][nWinW]>1)
	{
		int t = m_nMask2D[0][nWinW];
		nSum = 0;
		for (y = 0; y <= 2 * nWinW; y++)
		{
			for (x = 0; x <= 2 * nWinW; x++)
			{
				int temp = (int)(fDistMask1D[y] * fDistMask1D[x] * 1024.0 / fsum / t + 0.5);
				nSum += temp;
				m_nMask2D[y][x] = temp;
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
				m_nMask2D[y][x] = (m_nMask2D[y][x] * 1024 / t);
				nSum += m_nMask2D[y][x];
			}
		}
	}

	//printf("nMask2D=\n");
	//for (y = 0; y <= nWinW; y++)
	//{
	//	for (x = 0; x <= nWinW; x++)
	//	{
	//		printf("%4d", m_nMask2D[y][x]);
	//	}
	//	printf("\n");
	//}
	//printf("Sum=%d\n", nSum);
}

void CBilateralUVDNSFilter_Basic::RotateYUVLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, x;

	for (x = 0; x < nWidth; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pOutLine[i] = *(pInLine++);
		}
		pOutLine += nPitch;
	}
}

void CBilateralUVDNSFilter_Basic::DNSUVWindow(int UV[])
{
	int i, j, k, YUV0[3], sumUV[2], sumW, w, s, D[3];
	int nWinS = 2 * m_nWinW + 1;

	YUV0[0] = m_nYUVWin[m_nWinW][m_nWinW][0];
	YUV0[1] = UV[0] = m_nYUVWin[m_nWinW][m_nWinW][1];
	YUV0[2] = UV[1] = m_nYUVWin[m_nWinW][m_nWinW][2];

	sumUV[0] = (UV[0] * m_nMask2D[m_nWinW][m_nWinW]) << 8;
	sumUV[1] = (UV[1] * m_nMask2D[m_nWinW][m_nWinW]) << 8;
	sumW = m_nMask2D[m_nWinW][m_nWinW] << 8;

	for (i = 0; i < nWinS; i++)
	{
		for (j = 0; j < nWinS; j++)
		{
			if (i == m_nWinW&&j == m_nWinW)continue;

			w = m_nMask2D[i][j];
			if (w == 0)continue;

			s = 8;
			for (k = 0; k < 3; k++)
			{
				D[k] = m_nYUVWin[i][j][k] - YUV0[k];
				D[k] *= D[k];
//				D[k] >>= 1;
				s -= D[k] / m_nNoiseYUV[k];
			}

			if (s >= 0)
			{
				w = w << s;
				sumUV[0] += m_nYUVWin[i][j][1] * w;
				sumUV[1] += m_nYUVWin[i][j][2] * w;
				sumW += w;
			}
		}
	}

	UV[0] = (sumUV[0]*16) / sumW;
	UV[0] += m_nDitheringE[0];
	m_nDitheringE[0] = UV[0] & 15;
	UV[0] >>= 4;
	if (UV[0] > 255)UV[0] = 255;

	UV[1] = (sumUV[1]*16) / sumW;
	UV[1] += m_nDitheringE[1];
	m_nDitheringE[1] = UV[1] & 15;
	UV[1] >>= 4;
	if (UV[1] > 255)UV[1] = 255;
}

void CBilateralUVDNSFilter_Basic::DNSUVLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, j, k, x, UV[2];
	int nWinS = 2 * m_nWinW + 1;
	BYTE *pIn[17];

	for (i = 0; i < nWinS; i++)
	{
		pIn[i] = pInLines[i];
		for (j = m_nWinW; j < 2 * m_nWinW; j++)
		{
			for (k = 0; k < 3; k++)
			{
				m_nYUVWin[i][j][k] = pIn[i][k];
			}
			pIn[i] += nPitch;
		}
		for (j = 0; j < m_nWinW; j++)
		{
			for (k = 0; k < 3; k++)
			{
				m_nYUVWin[i][j][k] = m_nYUVWin[i][m_nWinW][k];
			}
		}
	}

	for (x = 0; x < nWidth - m_nWinW; x++)
	{
		for (i = 0; i < nWinS; i++)
		{
			for (k = 0; k < 3; k++)
			{
				m_nYUVWin[i][2 * m_nWinW][k] = pIn[i][k];
			}
			pIn[i] += nPitch;
		}

		DNSUVWindow(UV);
		pOutLine[1] = (BYTE)UV[0];
		pOutLine[2] = (BYTE)UV[1];
		pOutLine += 3;

		for (i = 0; i < nWinS; i++)
		{
			for (j = 0; j < 2 * m_nWinW; j++)
			{
				for (k = 0; k < 3; k++)
				{
					m_nYUVWin[i][j][k] = m_nYUVWin[i][j + 1][k];
				}
			}
		}
	}
	for (; x < nWidth; x++)
	{
		DNSUVWindow(UV);
		pOutLine[1] = (BYTE)UV[0];
		pOutLine[2] = (BYTE)UV[1];
		pOutLine += 3;

		for (i = 0; i < nWinS; i++)
		{
			for (j = 0; j < 2 * m_nWinW; j++)
			{
				for (k = 0; k < 3; k++)
				{
					m_nYUVWin[i][j][k] = m_nYUVWin[i][j + 1][k];
				}
			}
		}
	}
}

bool CBilateralUVDNSFilter_Basic::DNSUVImage(CRGBYUVImage *pImage)
{
	int i, y;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	BYTE *pInLines[17];

	BuildMask(m_nWinW);

	int nWinS = 2 * m_nWinW + 1;
	BYTE *pBuffer = new BYTE[nWidth*nWinS * 3];
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

	BYTE *pInLine = pImage->GetImageData();
	BYTE *pOutLine = pInLine;

	for (i = m_nWinW; i < 2 * m_nWinW; i++)
	{
		RotateYUVLine(pInLine, pInLines[i], nWidth, nWinS * 3);
		pInLine += nWidth * 3;
	}

	m_nDitheringE[0] = m_nDitheringE[1] = 0;
	for (y = 0; y < m_nWinW; y++)
	{
		RotateYUVLine(pInLine, pInLines[2 * m_nWinW], nWidth, nWinS * 3);
		pInLine += nWidth * 3;

		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3);
		pOutLine += nWidth * 3;

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * m_nWinW] += 3;
	}
	for (; y < nHeight - m_nWinW; y++)
	{
		RotateYUVLine(pInLine, pInLines[2 * m_nWinW], nWidth, nWinS * 3);
		pInLine += nWidth * 3;

		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3);
		pOutLine += nWidth * 3;

		BYTE *pTemp = pInLines[0];
		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * m_nWinW] = pTemp;
	}
	pInLines[2 * m_nWinW] = pInLines[2 * m_nWinW - 1];
	for (; y < nHeight; y++)
	{
		DNSUVLine(pInLines, pOutLine, nWidth, nWinS * 3);
		pOutLine += nWidth * 3;

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}