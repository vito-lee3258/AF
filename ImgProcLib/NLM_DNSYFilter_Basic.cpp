#include "NLM_DNSYFilter_Basic.h"

void CNLM_DNSYFilter_Basic::RotateYLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *(pInLine++);
		pOutLine += nPitch;
	}
}

int CNLM_DNSYFilter_Basic::DNSYWindow()
{
	int i, j, x, y, w, sumY, sumW, Y0;
	int nWinS = 2 * m_nWinW + 1;

	Y0 = m_nYWin[m_nWinW][m_nWinW];
	int nNoiseY = m_nNoiseLUT[Y0];

	if (m_bEdgeAdaptiveEnable)
	{
		int nMeanY = 0;
		int nVarY = 0;
		for (i = m_nWinW - 1; i <= m_nWinW + 1; i++)
		{
			for (j = m_nWinW - 1; j <= m_nWinW + 1; j++)
			{
				nMeanY += m_nYWin[i][j];
				nVarY += m_nYWin[i][j] * m_nYWin[i][j];
			}
		}
		nMeanY /= 9;
		nVarY /= 9;	nVarY -= nMeanY*nMeanY;

		if (nVarY >= 9 * nNoiseY)
		{
			nNoiseY = (nVarY / 9 + nNoiseY) / 2;
		}
		else if (nVarY * 9 <= nNoiseY)
		{
			nNoiseY = (nVarY * 9 + nNoiseY) / 2;
		}
	}

	if (nNoiseY < 1)return Y0;
	if (nNoiseY > 65535)nNoiseY = 65535;

	sumY = (Y0 << 8);
	sumW = 256;
	for (y = 1; y < nWinS - 1; y++)
	{
		for (x = 1; x < nWinS - 1; x++)
		{
			if (y == m_nWinW&&x == m_nWinW)continue;

			w = 0;
			for (i = -1; i <= 1; i++)
			{
				for (j = -1; j <= 1; j++)
				{
					int d = m_nYWin[y + i][x + j] - m_nYWin[m_nWinW + i][m_nWinW + j];
					w += (d*d) / 2;
				}
			}
			w /= 9;
			w = 8 - w / nNoiseY;
			if (w >= 0)
			{
				sumY += (m_nYWin[y][x] << w);
				sumW += (1 << w);
			}
		}
	}

	sumY = (sumY*16)/sumW;
	sumY += m_nDitheringE;
	m_nDitheringE = sumY & 15;
	sumY >>= 4;
	if (sumY < 0)sumY = 0;	if (sumY > 255)sumY = 255;

	sumY += ((Y0 - sumY)*m_nAddBackNoise) / 16;
	if (sumY < 0)sumY = 0;	if (sumY > 255)sumY = 255;

	return sumY;
}

void CNLM_DNSYFilter_Basic::DNSYLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, j, x, Y;
	int nWinS = 2 * m_nWinW + 1;
	BYTE *pIn[9];

	for (i = 0; i < nWinS; i++)
	{
		pIn[i] = pInLines[i];
		for (j = m_nWinW; j < 2 * m_nWinW; j++)
		{
			m_nYWin[i][j] = pIn[i][0];
			pIn[i] += nPitch;
		}
		for (j = 0; j < m_nWinW; j++)
		{
			m_nYWin[i][j] = m_nYWin[i][m_nWinW];
		}
	}

	for (x = 0; x < nWidth - m_nWinW; x++)
	{
		for (i = 0; i < nWinS; i++)
		{
			m_nYWin[i][2 * m_nWinW] = pIn[i][0];
			pIn[i] += nPitch;
		}

		Y = DNSYWindow();
		*(pOutLine++) = (BYTE)Y;

		for (i = 0; i < nWinS; i++)
		{
			for (j = 0; j < 2 * m_nWinW; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
	for (; x < nWidth; x++)
	{
		Y = DNSYWindow();
		*(pOutLine++) = (BYTE)Y;

		for (i = 0; i < nWinS; i++)
		{
			for (j = 0; j < 2 * m_nWinW; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
}

bool CNLM_DNSYFilter_Basic::DNSYImage(CGrayImage *pYImage)
{
	int i, y;
	int nWidth = pYImage->GetWidth();
	int nHeight = pYImage->GetHeight();
	int nWinS = 2 * m_nWinW + 1;
	BYTE *pInLines[9];

	if (m_nWinW == 1)return true;

	BYTE *pBuffer = new BYTE[nWidth*nWinS];
	if (pBuffer == NULL)return false;

	pInLines[m_nWinW] = pBuffer;
	for (i = m_nWinW + 1; i < nWinS; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}
	for (i = 0; i < m_nWinW; i++)
	{
		pInLines[i] = pInLines[m_nWinW];
	}

	BYTE *pInLine = pYImage->GetImageData();
	BYTE *pOutLine = pInLine;

	for (i = m_nWinW; i < 2 * m_nWinW; i++)
	{
		RotateYLine(pInLine, pInLines[i], nWidth, nWinS);
		pInLine += nWidth;
	}

	m_nDitheringE = 0;
	for (y = 0; y < m_nWinW; y++)
	{
		RotateYLine(pInLine, pInLines[2 * m_nWinW], nWidth, nWinS);
		pInLine += nWidth;

		DNSYLine(pInLines, pOutLine, nWidth, nWinS);
		pOutLine += nWidth;

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * m_nWinW] += 1;
	}
	for (; y <nHeight - m_nWinW; y++)
	{
		RotateYLine(pInLine, pInLines[2 * m_nWinW], nWidth, nWinS);
		pInLine += nWidth;

		DNSYLine(pInLines, pOutLine, nWidth, nWinS);
		pOutLine += nWidth;

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
		DNSYLine(pInLines, pOutLine, nWidth, nWinS);
		pOutLine += nWidth;

		for (i = 0; i < 2 * m_nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}
