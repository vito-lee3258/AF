#include "MultipleChannelImage.h"

bool CMultipleChannelImage::RenderSingleChannel(CGrayImage *pImage, int nChannel, int nScale, int nOffset)
{
	int x, y, g;

	if (!pImage->Create(m_nWidth, m_nHeight))return false;

	for (y = 0; y < m_nHeight; y++)
	{
		short *pIn = GetImageLine(y);
		BYTE *pOut = pImage->GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			g = pIn[nChannel];
			pIn += m_nDim;

			g = (g * 255) / nScale + nOffset;
			if (g < 0)g = 0;	if (g > 255)g = 255;
			*(pOut++) = (BYTE)g;
		}
	}

	return true;
}

bool CMultipleChannelImage::RenderBGRH(CRGBYUVImage *pImage, int nB, int nG, int nR, int nH, int nScale, int nOffset)
{
	int i, x, y, BGRH[4];

	if (nB < 0)nB = 0;	if (nB >= m_nDim)nB = m_nDim - 1;
	if (nG < 0)nG = 0;	if (nG >= m_nDim)nG = m_nDim - 1;
	if (nR < 0)nR = 0;	if (nR >= m_nDim)nR = m_nDim - 1;

	bool bHValid = (nH >= 0 && nH < m_nDim);
	BGRH[3] = 0;

	if (!pImage->Create(m_nWidth, m_nHeight))return false;

	for (y = 0; y < m_nHeight; y++)
	{
		short *pIn = GetImageLine(y);
		BYTE *pOut = pImage->GetImageLine(y);
		for (x = 0; x < m_nWidth; x++, pIn+=m_nDim)
		{
			BGRH[0] = pIn[nB];
			BGRH[1] = pIn[nG];
			BGRH[2] = pIn[nR];
			if (bHValid)BGRH[3] = pIn[nH];

			for (i = 0; i < 3; i++)
			{
				BGRH[i] += BGRH[3];
				BGRH[i] = (BGRH[i] * 255) / nScale + nOffset;
				if (BGRH[i] < 0)BGRH[i] = 0;	if (BGRH[i] > 255)BGRH[i] = 255;
				*(pOut++) = (BYTE)BGRH[i];
			}
		}
	}

	return true;
}

bool CMultipleChannelImage::BGRH2BGRImage(CRGBYUVImage *pBGRImage)
{
	int i, x, y, BGRH[4], E[3];
	int nShift = (m_nBit >= 8) ? (m_nBit - 8) : (0);
	int nMask = (1 << nShift) - 1;
	int D = MIN2(m_nDim, 4);

	if (m_nMAXS != ((1 << m_nBit) - 1))return false;
	if (m_nDim < 3)return false;

	if (!pBGRImage->Create(m_nWidth, m_nHeight))return false;

	BGRH[0] = BGRH[1] = BGRH[2] = BGRH[3] = 0;
	for (y = 0; y < m_nHeight; y++)
	{
		short *pIn = GetImageLine(y);
		BYTE *pOut = pBGRImage->GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			for (i = 0; i < D; i++)
			{
				BGRH[i] = pIn[i];
			}
			pIn += m_nDim;

			for (i = 0; i < 3; i++)
			{
				BGRH[i] += BGRH[3];
				if (BGRH[i] < 0)BGRH[i] = 0;	if (BGRH[i] > m_nMAXS)BGRH[i] = m_nMAXS;

				BGRH[i] += E[i];
				E[i] = BGRH[i] & nMask;
				BGRH[i] >>= nShift;

				if (BGRH[i] > 255)BGRH[i] = 255;

				*(pOut++) = (BYTE)BGRH[i];
			}
		}
	}

	return true;
}

void CMultipleChannelImage::RotateSLine(short *pInLine, short *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch)
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

void CMultipleChannelImage::HFilterLine(short *pInLine, short *pOutLine, int nMask[], int nWinW, int nScale, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x, nWin[9], Y;

	for (i = nWinW; i < 2*nWinW; i++)
	{
		nWin[i] = pInLine[0];
		pInLine += nInPitch;
	}
	for (i = 0; i < nWinW; i++)
	{
		nWin[i] = nWin[nWinW];
	}

	for (x = 0; x < nWidth - nWinW; x++)
	{
		nWin[2 * nWinW] = pInLine[0];
		pInLine += nInPitch;

		Y = 0;
		for (i = 0; i < 2 * nWinW + 1; i++)
		{
			Y += nWin[i] * nMask[i];
		}
		Y /= nScale;

		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nWin[i] = nWin[i + 1];
		}
	}
	for (; x < nWidth; x++)
	{
		Y = 0;
		for (i = 0; i < 2 * nWinW + 1; i++)
		{
			Y += nWin[i] * nMask[i];
		}
		Y /= nScale;

		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nWin[i] = nWin[i + 1];
		}
	}
}

void CMultipleChannelImage::VFilterLine(short *pInLines[], short *pOutLine, int nMask[], int nWinW, int nScale, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x, nWin[9], Y;
	short *pIn[9];

	for (i = 0; i < 2 * nWinW + 1; i++)
	{
		pIn[i] = pInLines[i];
	}

	for (x = 0; x < nWidth; x++)
	{
		Y = 0;
		for (i = 0; i < 2 * nWinW + 1; i++)
		{
			nWin[i] = pIn[i][0];
			pIn[i] += nInPitch;

			Y += nWin[i] * nMask[i];
		}
		Y /= nScale;

		pOutLine[0] = (short)Y;
		pOutLine += nOutPitch;
	}
}

bool CMultipleChannelImage::HFilterChannel(int nChannel, int nMask[], int nWinW, int nScale)
{
	int y;

	if (nWinW < 0 || nWinW>4)return false;

	for (y = 0; y < m_nHeight; y++)
	{
		short *pLine = GetImageLine(y) + nChannel;
		HFilterLine(pLine, pLine, nMask, nWinW, nScale, m_nWidth, m_nDim, m_nDim);
	}

	return true;
}

bool CMultipleChannelImage::VFilterChannel(int nChannel, int nMask[], int nWinW, int nScale)
{
	int i, y;
	int nPitch = m_nWidth*m_nDim;
	short *pInLines[9];
	if (nWinW < 0 || nWinW>4)return false;

	short *pBuffer = new short[m_nWidth * (2*nWinW+1)];
	if (pBuffer == NULL)return false;

	pInLines[nWinW] = pBuffer;
	for (i = nWinW + 1; i < 2 * nWinW + 1; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}
	for (i = 0; i < nWinW; i++)
	{
		pInLines[i] = pInLines[nWinW];
	}

	short *pInLine = GetImageData()+nChannel;
	for (i = nWinW; i < 2 * nWinW; i++)
	{
		RotateSLine(pInLine, pInLines[i], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;
	}

	for (y = 0; y < nWinW; y++)
	{
		RotateSLine(pInLine, pInLines[2 * nWinW], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nMask, nWinW, nScale, m_nWidth, 2 * nWinW + 1, m_nDim);

		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinW] += 1;
	}
	for (; y < m_nHeight-nWinW; y++)
	{
		RotateSLine(pInLine, pInLines[2 * nWinW], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nMask, nWinW, nScale, m_nWidth, 2 * nWinW + 1, m_nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinW] = pTemp;
	}
	pInLines[2 * nWinW] = pInLines[2 * nWinW - 1];
	for (; y < m_nHeight; y++)
	{
		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nMask, nWinW, nScale, m_nWidth, 2 * nWinW + 1, m_nDim);

		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CMultipleChannelImage::HVFilterChannel(int nChannel, int nHMask[], int nHWinW, int nHScale, int nVMask[], int nVWinW, int nVScale)
{
	int i, y;
	int nPitch = m_nWidth*m_nDim;
	short *pInLines[9];
	if (nHWinW < 0 || nHWinW>4 || nVWinW < 0 || nVWinW>4)return false;

	short *pBuffer = new short[m_nWidth * (2 * nVWinW + 1)];
	if (pBuffer == NULL)return false;

	pInLines[nVWinW] = pBuffer;
	for (i = nVWinW + 1; i < 2 * nVWinW + 1; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}
	for (i = 0; i < nVWinW; i++)
	{
		pInLines[i] = pInLines[nVWinW];
	}

	short *pInLine = GetImageData() + nChannel;
	for (i = nVWinW; i < 2 * nVWinW; i++)
	{
		HFilterLine(pInLine, pInLines[i], nHMask, nHWinW, nHScale, m_nWidth, m_nDim, 2 * nVWinW + 1);
		pInLine += nPitch;
	}

	for (y = 0; y < nVWinW; y++)
	{
		HFilterLine(pInLine, pInLines[2 * nVWinW], nHMask, nHWinW, nHScale, m_nWidth, m_nDim, 2 * nVWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nVMask, nVWinW, nVScale, m_nWidth, 2 * nVWinW + 1, m_nDim);

		for (i = 0; i < 2 * nVWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nVWinW] += 1;
	}
	for (; y < m_nHeight - nVWinW; y++)
	{
		HFilterLine(pInLine, pInLines[2 * nVWinW], nHMask, nHWinW, nHScale, m_nWidth, m_nDim, 2 * nVWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nVMask, nVWinW, nVScale, m_nWidth, 2 * nVWinW + 1, m_nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 2 * nVWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nVWinW] = pTemp;
	}
	pInLines[2 * nVWinW] = pInLines[2 * nVWinW - 1];
	for (; y < m_nHeight; y++)
	{
		short *pOutLine = GetImageLine(y) + nChannel;
		VFilterLine(pInLines, pOutLine, nVMask, nVWinW, nVScale, m_nWidth, 2 * nVWinW + 1, m_nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 2 * nVWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

void CMultipleChannelImage::HMidFilterLine(short *pInLine, short *pOutLine, int nWinW, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x, nWin[9], Y;

	for (i = nWinW; i < 2 * nWinW; i++)
	{
		nWin[i] = pInLine[0];
		pInLine += nInPitch;
	}
	for (i = 0; i < nWinW; i++)
	{
		nWin[i] = nWin[nWinW];
	}

	for (x = 0; x < nWidth - nWinW; x++)
	{
		nWin[2 * nWinW] = pInLine[0];
		pInLine += nInPitch;

		Y = Mid(nWin, 2*nWinW+1);

		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nWin[i] = nWin[i + 1];
		}
	}
	for (; x < nWidth; x++)
	{
		Y = Mid(nWin, 2 * nWinW + 1);

		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (i = 0; i < 2 * nWinW; i++)
		{
			nWin[i] = nWin[i + 1];
		}
	}
}

void CMultipleChannelImage::VMidFilterLine(short *pInLines[], short *pOutLine, int nWinW, int nWidth, int nInPitch, int nOutPitch)
{
	int i, x, nWin[9], Y;
	short *pIn[9];

	for (i = 0; i < 2 * nWinW + 1; i++)
	{
		pIn[i] = pInLines[i];
	}

	for (x = 0; x < nWidth; x++)
	{
		for (i = 0; i < 2 * nWinW + 1; i++)
		{
			nWin[i] = pIn[i][0];
			pIn[i] += nInPitch;
		}
		Y = Mid(nWin, 2 * nWinW + 1);

		pOutLine[0] = (short)Y;
		pOutLine += nOutPitch;
	}
}

bool CMultipleChannelImage::HMidFilterChannel(int nChannel, int nWinW)
{
	int y;

	if (nWinW < 0 || nWinW>4)return false;

	for (y = 0; y < m_nHeight; y++)
	{
		short *pLine = GetImageLine(y) + nChannel;
		HMidFilterLine(pLine, pLine, nWinW, m_nWidth, m_nDim, m_nDim);
	}

	return true;
}

bool CMultipleChannelImage::VMidFilterChannel(int nChannel, int nWinW)
{
	int i, y;
	int nPitch = m_nWidth*m_nDim;
	short *pInLines[9];
	if (nWinW < 0 || nWinW>4)return false;

	short *pBuffer = new short[m_nWidth * (2 * nWinW + 1)];
	if (pBuffer == NULL)return false;

	pInLines[nWinW] = pBuffer;
	for (i = nWinW + 1; i < 2 * nWinW + 1; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}
	for (i = 0; i < nWinW; i++)
	{
		pInLines[i] = pInLines[nWinW];
	}

	short *pInLine = GetImageData() + nChannel;
	for (i = nWinW; i < 2 * nWinW; i++)
	{
		RotateSLine(pInLine, pInLines[i], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;
	}

	for (y = 0; y < nWinW; y++)
	{
		RotateSLine(pInLine, pInLines[2 * nWinW], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VMidFilterLine(pInLines, pOutLine, nWinW, m_nWidth, 2 * nWinW + 1, m_nDim);

		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinW] += 1;
	}
	for (; y < m_nHeight - nWinW; y++)
	{
		RotateSLine(pInLine, pInLines[2 * nWinW], 1, m_nWidth, m_nDim, 2 * nWinW + 1);
		pInLine += nPitch;

		short *pOutLine = GetImageLine(y) + nChannel;
		VMidFilterLine(pInLines, pOutLine, nWinW, m_nWidth, 2 * nWinW + 1, m_nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[2 * nWinW] = pTemp;
	}
	pInLines[2 * nWinW] = pInLines[2 * nWinW - 1];
	for (; y < m_nHeight; y++)
	{
		short *pOutLine = GetImageLine(y) + nChannel;
		VMidFilterLine(pInLines, pOutLine, nWinW, m_nWidth, 2 * nWinW + 1, m_nDim);

		for (i = 0; i < 2 * nWinW; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}
