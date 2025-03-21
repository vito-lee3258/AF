#include "ByteImageArray.h"

void CByteImageArray::RotateLine(BYTE *pInLine, BYTE *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x;

	for (x = 0; x < nWidth; x++)
	{
		for (i = 0; i < nDim; i++)
		{
			pOutLine[i] = pInLine[i];
		}
		pInLine += nInPitch;
		pOutLine += nOutPitch;
	}
}

void CByteImageArray::HFilterLine(BYTE *pInLine, short *pOutLine, int nWidth, int nMask[], int nWinW, int nScale, int nInPitch, int nOutPitch)
{
	int i, x, nYWin[9], Y;
	int nWinS = 2 * nWinW + 1;

	for (i = 0; i <= nWinW; i++)
	{
		nYWin[i] = pInLine[0];
	}
	for (; i < 2 * nWinW; i++)
	{
		nYWin[i] = pInLine[0];
		pInLine += nInPitch;
	}

	for (x = 0; x < nWidth - nWinW; x++)
	{
		nYWin[2 * nWinW] = pInLine[0];
		pInLine += nInPitch;

		Y = 0;
		for (i = 0; i < nWinS; i++)
		{
			Y += nMask[i] * nYWin[i];
		}
		Y /= nScale;
		if (Y < -32768)Y = -32768;	if (Y > 32767)Y = 32767;
		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nYWin[i] = nYWin[i + 1];
		}
	}
	for (; x < nWidth; x++)
	{
		Y = 0;
		for (i = 0; i < nWinS; i++)
		{
			Y += nMask[i] * nYWin[i];
		}
		Y /= nScale;
		if (Y < -32768)Y = -32768;	if (Y > 32767)Y = 32767;
		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nYWin[i] = nYWin[i + 1];
		}
	}
}

void CByteImageArray::VFilterLine(short *pInLines[], BYTE *pOutLine, int nWidth, int nMask[], int nWinW, int nScale, int nOffset, int nInPitch, int nOutPitch)
{
	int i, x, nYWin[9], Y;
	int nWinS = 2 * nWinW + 1;
	short *pIn[9];

	for (i = 0; i < nWinS; i++)
	{
		pIn[i] = pInLines[i];
	}

	for (x = 0; x < nWidth; x++)
	{
		Y = 0;
		for (i = 0; i < nWinS; i++)
		{
			nYWin[i] = pIn[i][0];
			Y += nMask[i] * nYWin[i];
			pIn[i] += nInPitch;
		}
		Y /= nScale;
		Y += nOffset;
		if (Y < 0)Y = 0;	if (Y > 255)Y = 255;
		*pOutLine = (BYTE)Y;
		pOutLine += nOutPitch;
	}
}

bool CByteImageArray::HVFilterChannel(int nChannel, int nHMask[], int nHWinW, int nHScale, int nVMask[], int nVWinW, int nVScale, int nOffset)
{
	int i, y;
	short *pVLines[9];

	if (nChannel < 0)nChannel = 0;	if (nChannel >= m_nDim)nChannel = m_nDim - 1;
	if (nHWinW < 0)nHWinW = 0;	if (nHWinW > 4)nHWinW = 4;
	if (nVWinW < 0)nVWinW = 0;	if (nVWinW > 4)nVWinW = 4;

	int nVWinS = 2 * nVWinW + 1;
	short *pBuffer = new short[m_nWidth*nVWinS];
	if (pBuffer == NULL)return false;

	pVLines[nVWinW] = pBuffer;
	for (i = nVWinW + 1; i < nVWinS; i++)
	{
		pVLines[i] = pVLines[i - 1] + 1;
	}
	for (i = 0; i < nVWinW; i++)
	{
		pVLines[i] = pVLines[nVWinW];
	}

	BYTE *pInLine = GetImageData() + nChannel;
	BYTE *pOutLine = pInLine;
	for (i = nVWinW; i < 2 * nVWinW; i++)
	{
		HFilterLine(pInLine, pVLines[i], m_nWidth, nHMask, nHWinW, nHScale, m_nDim, nVWinS);
		pInLine += m_nWidth*m_nDim;
	}

	for (y = 0; y < nVWinW; y++)
	{
		HFilterLine(pInLine, pVLines[2 * nVWinW], m_nWidth, nHMask, nHWinW, nHScale, m_nDim, nVWinS);
		pInLine += m_nWidth*m_nDim;

		VFilterLine(pVLines, pOutLine, m_nWidth, nVMask, nVWinW, nVScale, nOffset, nVWinS, m_nDim);
		pOutLine += m_nWidth*m_nDim;

		for (i = 0; i < 2 * nVWinW; i++)
		{
			pVLines[i] = pVLines[i + 1];
		}
		pVLines[2 * nVWinW] += 1;
	}
	for (; y < m_nHeight - nVWinW; y++)
	{
		HFilterLine(pInLine, pVLines[2 * nVWinW], m_nWidth, nHMask, nHWinW, nHScale, m_nDim, nVWinS);
		pInLine += m_nWidth*m_nDim;

		VFilterLine(pVLines, pOutLine, m_nWidth, nVMask, nVWinW, nVScale, nOffset, nVWinS, m_nDim);
		pOutLine += m_nWidth*m_nDim;

		short *pTemp = pVLines[0];
		for (i = 0; i < 2 * nVWinW; i++)
		{
			pVLines[i] = pVLines[i + 1];
		}
		pVLines[2 * nVWinW] = pTemp;
	}
	pVLines[2 * nVWinW] = pVLines[2 * nVWinW - 1];
	for (; y < m_nHeight; y++)
	{
		VFilterLine(pVLines, pOutLine, m_nWidth, nVMask, nVWinW, nVScale, nOffset, nVWinS, m_nDim);
		pOutLine += m_nWidth*m_nDim;

		for (i = 0; i < 2 * nVWinW; i++)
		{
			pVLines[i] = pVLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

void CByteImageArray::Filter2DLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nMask2D[], int nWinW, int nWinH, int nScale, int nOffset, int nInPitch)
{
	int i, j, x, nWin[9][9], Y;
	BYTE *pIn[9];

	for (i = 0; i < 2 * nWinH + 1; i++)
	{
		pIn[i] = pInLines[i];
		for (j = 0; j <= nWinW; j++)
		{
			nWin[i][j] = pIn[i][0];
		}
		for (; j < 2 * nWinW; j++)
		{
			nWin[i][j] = pIn[i][0];
			pIn[i] += nInPitch;
		}
	}

	for (x = 0; x < m_nWidth - nWinW; x++)
	{
		Y = 0;
		int *pM = nMask2D;
		for (i = 0; i < 2 * nWinH + 1; i++)
		{
			nWin[i][2 * nWinW] = pIn[i][0];
			pIn[i] += nInPitch;

			for (j = 0; j < 2 * nWinW + 1; j++)
			{
				Y += nWin[i][j] * pM[0];
				pM++;
			}
		}
		Y /= nScale;
		Y += nOffset;
		if (Y < 0)Y = 0;	if (Y > 255)Y = 255;
		*pOutLine = (BYTE)Y;
		pOutLine += m_nDim;

		for (i = 0; i < 2 * nWinH + 1; i++)
		{
			for (j = 0; j < 2 * nWinW; j++)
			{
				nWin[i][j] = nWin[i][j + 1];
			}
		}
	}
	for (; x < m_nWidth; x++)
	{
		Y = 0;
		int *pM = nMask2D;
		for (i = 0; i < 2 * nWinH + 1; i++)
		{
			for (j = 0; j < 2 * nWinW + 1; j++)
			{
				Y += nWin[i][j] * pM[0];
				pM++;
			}
		}
		Y /= nScale;
		Y += nOffset;
		if (Y < 0)Y = 0;	if (Y > 255)Y = 255;
		*pOutLine = (BYTE)Y;
		pOutLine += m_nDim;

		for (i = 0; i < 2 * nWinH + 1; i++)
		{
			for (j = 0; j < 2 * nWinW; j++)
			{
				nWin[i][j] = nWin[i][j + 1];
			}
		}
	}
}

bool CByteImageArray::Filter2DChannel(int nChannel, int nMask2D[], int nWinW, int nWinH, int nScale, int nOffset)
{
	int i, y;
	int nPitch = m_nWidth*m_nDim;
	BYTE *pInLines[9];

	if (nChannel < 0)nChannel = 0;	if (nChannel >= m_nDim)nChannel = m_nDim - 1;
	if (nWinW < 0)nWinW = 0;	if (nWinW > 4)nWinW = 4;
	if (nWinH < 0)nWinH = 0;	if (nWinH > 4)nWinH = 4;

	BYTE *pBuffer = new BYTE[m_nWidth*(2 * nWinH + 1)];
	if (pBuffer == NULL)return false;

	for (i = 0; i <= nWinH; i++)
	{
		pInLines[i] = pBuffer;
	}
	for (; i < 2 * nWinH + 1; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}

	BYTE *pInLine = GetImageData() + nChannel;
	BYTE *pOutLine = pInLine;
	for (i = nWinH; i < 2 * nWinH; i++)
	{
		RotateLine(pInLine, pInLines[i], 1, m_nWidth, m_nDim, 2 * nWinH + 1);
		pInLine += nPitch;
	}

	for (y = 0; y < nWinH; y++)
	{
		RotateLine(pInLine, pInLines[2 * nWinH], 1, m_nWidth, m_nDim, 2 * nWinH + 1);
		pInLine += nPitch;

		Filter2DLine(pInLines, pOutLine, m_nWidth, nMask2D, nWinW, nWinH, nScale, nOffset, 2 * nWinH + 1);
		pOutLine += nPitch;

		for (i = 0; i < 2 * nWinH; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinH] += 1;
	}
	for (; y <m_nHeight - nWinH; y++)
	{
		RotateLine(pInLine, pInLines[2 * nWinH], 1, m_nWidth, m_nDim, 2 * nWinH + 1);
		pInLine += nPitch;

		Filter2DLine(pInLines, pOutLine, m_nWidth, nMask2D, nWinW, nWinH, nScale, nOffset, 2 * nWinH + 1);
		pOutLine += nPitch;

		BYTE *pTemp = pInLines[0];
		for (i = 0; i < 2 * nWinH; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinH] = pTemp;
	}
	pInLines[2 * nWinH] = pInLines[2 * nWinH - 1];
	for (; y <m_nHeight - nWinH; y++)
	{
		Filter2DLine(pInLines, pOutLine, m_nWidth, nMask2D, nWinW, nWinH, nScale, nOffset, 2 * nWinH + 1);
		pOutLine += nPitch;

		for (i = 0; i < 2 * nWinH; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}