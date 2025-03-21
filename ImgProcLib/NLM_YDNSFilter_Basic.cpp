#include "NLM_YDNSFilter_Basic.h"

void CNLM_YDNSFilter_Basic::RotateSLine(short *pInLine, short *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch)
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

void CNLM_YDNSFilter_Basic::RotateYLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nOutPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *(pInLine++);
		pOutLine += nOutPitch;
	}
}

int CNLM_YDNSFilter_Basic::DNSYWindow()
{
	int i, j, x, y, sumY, sumW, W, d;
	int Y0 = m_nYWin[3][3] >> m_nShift;	
	if (Y0 < 0)Y0 = 0;	if (Y0 > 255)Y0 = 255;
	int nNoise = m_nNoiseYLUT[Y0];

	if (m_bEdgeAdaptiveEnable)
	{
		int nMeanY = 0;
		int nVarY = 0;
		for (i = -1; i <= 1; i++)
		{
			for (j = -1; j <= 1; j++)
			{
				nMeanY += m_nYWin[i + 3][j + 3];
				nVarY += m_nYWin[i + 3][j + 3] * m_nYWin[i + 3][j + 3];
			}
		}
		nVarY -= nMeanY*nMeanY / 9;
		
		if (nVarY >= 8 * nNoise)
		{
			nNoise = (nVarY / 8 + nNoise) / 2;
		}
	}

	if (nNoise == 0)return m_nYWin[3][3];
	if (nNoise > 32767)nNoise = 32767;

	__int64 nInvNoise = (1 << 15) / nNoise;

	sumY = (m_nYWin[3][3]<<8);
	sumW = 256;

	for (y = 1; y < 6; y++)
	{
		for (x = 1; x < 6; x++)
		{
			if (y == 3 && x == 3)continue;

			W = 0;
			for (i = -1; i <= 1; i++)
			{
				for (j = -1; j <= 1; j++)
				{
					d = m_nYWin[i + y][j + x] - m_nYWin[i + 3][j + 3];
					d *= d;
					W += d;
				}
			}
//			W = 8 - W / nNoise;
			W = (int)((W*nInvNoise) >> 15);
			W = 8 - W;
			if (W >= 0)
			{
				W = (1 << W);
				sumY += m_nYWin[y][x]*W;
				sumW += W;
			}
		}
	}

	sumY /= sumW;
	sumY += ((m_nYWin[3][3] - sumY)*m_nAddBackNoiseY) / 16;
	if (sumY < 0)sumY = 0;	if (sumY > m_nMAXS)sumY = m_nMAXS;
	return sumY;
}

void CNLM_YDNSFilter_Basic::DNSYLine(short *pInLines[], short *pOutLine, int nWidth, int nInPitch, int nOutPitch)
{
	int i, j, x, Y;
	short *pIn[7];

	for (i = 0; i < 7; i++)
	{
		pIn[i] = pInLines[i];
	}
	m_nYWin[0] = m_nYWin[1] = m_nYWin[2] = m_nYWin[3] = m_nYWinBuf;
	m_nYWin[4] = m_nYWin[3] + 7;
	m_nYWin[5] = m_nYWin[4] + 7;
	m_nYWin[6] = m_nYWin[5] + 7;

	for (j = 3; j < 6; j++)
	{
		for(i=0; i<7; i++)
		{
			m_nYWin[j][i] = pIn[i][0];
			pIn[i] += nInPitch;
		}
	}

	for (x = 0; x < 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[6][i] = pIn[i][0];
			pIn[i] += nInPitch;
		}

		Y = DNSYWindow();
		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
		m_nYWin[6] += 7;
	}
	for (; x < nWidth - 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[6][i] = pIn[i][0];
			pIn[i] += nInPitch;
		}

		Y = DNSYWindow();
		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		int *pTemp = m_nYWin[0];
		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
		m_nYWin[6] = pTemp;
	}
	for (; x < nWidth; x++)
	{
		Y = DNSYWindow();
		*pOutLine = (short)Y;
		pOutLine += nOutPitch;

		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
	}
}

bool CNLM_YDNSFilter_Basic::DNSYImage_Line(CMultipleChannelImage *pYUVImage)
{
	int i, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();
	short *pInLines[7];

	short *pBuffer = new short[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	pInLines[4] = pInLines[3] + 1;
	pInLines[5] = pInLines[4] + 1;
	pInLines[6] = pInLines[5] + 1;

	short *pInLine = pYUVImage->GetImageData();
	for (i = 3; i < 6; i++)
	{
		RotateSLine(pInLine, pInLines[i], 1, nWidth, nDim, 7);
		pInLine += nWidth*nDim;
	}

	for (y = 0; y < 3; y++)
	{
		RotateSLine(pInLine, pInLines[6], 1, nWidth, nDim, 7);
		pInLine += nWidth*nDim;

		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSYLine(pInLines, pOutLine, nWidth, 7, nDim);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateSLine(pInLine, pInLines[6], 1, nWidth, nDim, 7);
		pInLine += nWidth*nDim;

		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSYLine(pInLines, pOutLine, nWidth, 7, nDim);

		short *pTemp = pInLines[0];
		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] = pTemp;
	}
	pInLines[6] = pInLines[5];
	for (; y < nHeight; y++)
	{
		short *pOutLine = pYUVImage->GetImageLine(y);
		DNSYLine(pInLines, pOutLine, nWidth, 7, nDim);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

void CNLM_YDNSFilter_Basic::DNSYLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, j, x, Y;
	BYTE *pIn[7];

	for (i = 0; i < 7; i++)
	{
		pIn[i] = pInLines[i];
	}

	m_nYWin[0] = m_nYWin[1] = m_nYWin[2] = m_nYWin[3] = m_nYWinBuf;
	m_nYWin[4] = m_nYWin[3] + 7;
	m_nYWin[5] = m_nYWin[4] + 7;
	m_nYWin[6] = m_nYWin[5] + 7;

	for (j = 3; j < 6; j++)
	{
		for (i = 0; i<7; i++)
		{
			m_nYWin[j][i] = pIn[i][0];
			pIn[i] += nPitch;
		}
	}

	for (x = 0; x < 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[6][i] = pIn[i][0];
			pIn[i] += nPitch;
		}

		Y = DNSYWindow();
		*(pOutLine++) = (BYTE)Y;

		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
		m_nYWin[6] += 7;
	}
	for (; x < nWidth - 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[6][i] = pIn[i][0];
			pIn[i] += nPitch;
		}

		Y = DNSYWindow();
		*(pOutLine++) = (BYTE)Y;

		int *pTemp = m_nYWin[0];
		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
		m_nYWin[6] = pTemp;
	}
	for (; x < nWidth; x++)
	{
		Y = DNSYWindow();
		*(pOutLine++) = (BYTE)Y;

		for (j = 0; j < 6; j++)
		{
			m_nYWin[j] = m_nYWin[j + 1];
		}
	}
}

bool CNLM_YDNSFilter_Basic::DNSYImage_Line(CYUV420Image *pYUVImage)
{
	int i, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	BYTE *pInLines[7];

	BYTE *pBuffer = new BYTE[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	pInLines[4] = pInLines[3] + 1;
	pInLines[5] = pInLines[4] + 1;
	pInLines[6] = pInLines[5] + 1;

	BYTE *pInLine = pYUVImage->GetYImage();
	for (i = 3; i < 6; i++)
	{
		RotateYLine(pInLine, pInLines[i], nWidth);
		pInLine += nWidth;
	}

	for (y = 0; y < 3; y++)
	{
		RotateYLine(pInLine, pInLines[6], nWidth);
		pInLine += nWidth;

		BYTE *pOutLine = pYUVImage->GetYLine(y);
		DNSYLine(pInLines, pOutLine, nWidth);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateYLine(pInLine, pInLines[6], nWidth);
		pInLine += nWidth;

		BYTE *pOutLine = pYUVImage->GetYLine(y);
		DNSYLine(pInLines, pOutLine, nWidth);

		BYTE *pTemp = pInLines[0];
		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] = pTemp;
	}
	pInLines[6] = pInLines[5];
	for (; y < nHeight; y++)
	{
		BYTE *pOutLine = pYUVImage->GetYLine(y);
		DNSYLine(pInLines, pOutLine, nWidth);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

void CNLM_YDNSFilter_Basic::DiffLine(BYTE *pSource, BYTE *pTarget, WORD *pDiff, int nOffsetX, int nWidth, int nOutPitch)
{
	int x;
	__int32 Y0, Y1, dY;
	int W = MIN2(nWidth, nWidth - nOffsetX);
	int S = MAX2(nOffsetX, 0);

	pTarget += S;

	Y1 = pTarget[0];
	for (x = 0; x < -nOffsetX; x++)
	{
		Y0 = *(pSource++);

		dY = Y1 - Y0;	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
	for (; x < W; x++)
	{
		Y1 = *(pTarget++);
		Y0 = *(pSource++);

		dY = Y1 - Y0;	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
	for (; x < nWidth; x++)
	{
		Y0 = *(pSource++);

		dY = Y1 - Y0;	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
}

void CNLM_YDNSFilter_Basic::DiffLine(short *pSource, short *pTarget, WORD *pDiff, int nOffsetX, int nWidth, int nInPitch, int nOutPitch)
{
	int x;
	__int32 Y0, Y1, dY;
	int W = MIN2(nWidth, nWidth - nOffsetX);
	int S = MAX2(nOffsetX, 0);

	pTarget += S*nInPitch;

	Y1 = pTarget[0];
	for (x = 0; x < -nOffsetX; x++)
	{
		Y0 = *pSource;	pSource += nInPitch;

		dY = (Y1 >> m_nShift) - (Y0 >> m_nShift);	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
	for (; x < W; x++)
	{
		Y1 = *pTarget; pTarget += nInPitch;
		Y0 = *pSource;	pSource += nInPitch;

		dY = (Y1 >> m_nShift) - (Y0 >> m_nShift);	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
	for (; x < nWidth; x++)
	{
		Y0 = *pSource;	pSource += nInPitch;

		dY = (Y1 >> m_nShift) - (Y0 >> m_nShift);	dY *= dY;
		pDiff[0] = (WORD)dY;
		pDiff[1] = (WORD)Y1;
		pDiff += nOutPitch;
	}
}

void CNLM_YDNSFilter_Basic::HSumLine(WORD *pInLine, unsigned __int32 *pOutLine, int nWidth, int nInPitch)
{
	int x;
	unsigned __int32 In[3];

	In[0] = In[1] = *pInLine;
	pInLine += nInPitch;
	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *pInLine;
		pInLine += nInPitch;

		*pOutLine = (In[0] + In[1] + In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	*pOutLine = (In[0] + In[1] + In[2]);
	pOutLine += 3;
}

void CNLM_YDNSFilter_Basic::VSumLine(unsigned __int32 *pInLines[], WORD *pOutLine, int nWidth, int nOutPitch)
{
	int x;
	unsigned __int32 *pIn[3];
	unsigned __int32 S;

	pIn[0] = pInLines[0];
	pIn[1] = pInLines[1];
	pIn[2] = pInLines[2];
	for (x = 0; x < nWidth; x++)
	{
		S = pIn[0][0];	pIn[0] += 3;
		S += pIn[1][0];	pIn[1] += 3;
		S += pIn[2][0];	pIn[2] += 3;
		S /= 9;

		*pOutLine = (WORD) S;
		pOutLine += nOutPitch;
	}
}

bool CNLM_YDNSFilter_Basic::Avg3x3Image(CBasicImageArray_WORD *pImage, int nChannel)
{
	int y;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	int nDim = pImage->GetDim();
	unsigned __int32 *pLines[3];

	unsigned __int32 *pBuffer = new unsigned __int32[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pLines[1] = pBuffer;
	pLines[2] = pLines[1] + 1;

	WORD *pInLine = pImage->GetImageData()+nChannel;
	WORD *pOutLine = pInLine;

	HSumLine(pInLine, pLines[1], nWidth, nDim);
	pInLine += nWidth*nDim;
	
	HSumLine(pInLine, pLines[2], nWidth, nDim);
	pInLine += nWidth*nDim;
	VSumLine(pLines, pOutLine, nWidth, nDim);
	pOutLine += nWidth*nDim;
	pLines[0] = pLines[1];
	pLines[1] = pLines[2];
	pLines[2] += 1;

	for (y = 1; y < nHeight - 1; y++)
	{
		HSumLine(pInLine, pLines[2], nWidth, nDim);
		pInLine += nWidth*nDim;
		VSumLine(pLines, pOutLine, nWidth, nDim);
		pOutLine += nWidth*nDim;

		unsigned __int32 *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	pLines[2] = pLines[1];
	VSumLine(pLines, pOutLine, nWidth, nDim);

	delete[] pBuffer;
	return true;
}

bool CNLM_YDNSFilter_Basic::DiffYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage, int nOffsetX, int nOffsetY)
{
	int y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nOutDim = pDiffImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pSource = pYUVImage->GetYLine(y);
		BYTE *pTarget = pYUVImage->GetYLine(y + nOffsetY);
		WORD *pDiff = pDiffImage->GetImageLine(y);
		DiffLine(pSource, pTarget, pDiff, nOffsetX, nWidth, nOutDim);
	}

	return Avg3x3Image(pDiffImage, 0);
}

bool CNLM_YDNSFilter_Basic::DiffYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage, int nOffsetX, int nOffsetY)
{
	int y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nInDim = pYUVImage->GetDim();
	int nOutDim = pDiffImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		short *pSource = pYUVImage->GetImageLine(y);
		short *pTarget = pYUVImage->GetImageLine(y + nOffsetY);
		WORD *pDiff = pDiffImage->GetImageLine(y);
		DiffLine(pSource, pTarget, pDiff, nOffsetX, nWidth, nInDim, nOutDim);
	}

	return Avg3x3Image(pDiffImage, 0);
}

bool CNLM_YDNSFilter_Basic::MeanVarYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage)
{
	int x, y, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		WORD *pMeanVar = pDiffImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pY++);
			pMeanVar[0] = (WORD)Y;
			pMeanVar[1] = (WORD)(Y*Y);
			pMeanVar += 3;
		}
	}

	if (!Avg3x3Image(pDiffImage, 0))return false;
	if (!Avg3x3Image(pDiffImage, 1))return false;

	return true;
}

bool CNLM_YDNSFilter_Basic::MeanVarYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage)
{
	int x, y, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		WORD *pMeanVar = pDiffImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *pY;	pY += nDim;
			Y >>= m_nShift;
			pMeanVar[0] = (WORD)Y;
			pMeanVar[1] = (WORD)(Y*Y);
			pMeanVar += 3;
		}
	}

	if (!Avg3x3Image(pDiffImage, 0))return false;
	if (!Avg3x3Image(pDiffImage, 1))return false;

	return true;
}

bool CNLM_YDNSFilter_Basic::InitDNSYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	unsigned __int32 Y, nNoise;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	if (!pDiffImage->SetImageSize(nWidth, nHeight, 3))return false;
	if (!pSumYWImage->SetImageSize(nWidth, nHeight, 2))return false;

	if (m_bEdgeAdaptiveEnable)
	{
		if (!MeanVarYImage(pYUVImage, pDiffImage))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		WORD *pNoise = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pY++);
			nNoise = m_nNoiseYLUT[Y];

			if (m_bEdgeAdaptiveEnable)
			{
				int nMean = pNoise[0];
				int nVar = pNoise[1];
				nVar -= nMean*nMean;
				if (nVar < 0)nVar = 0;

				if (nVar >= 8 * nNoise)
				{
					nNoise = (nVar / 8 + nNoise) / 2;
				}
				else if (nVar * 8 <= nNoise)
				{
					nNoise = (nVar * 8 + nNoise) / 2;
				}
			}

			if (nNoise > 0)
			{
				if (nNoise > 32767)nNoise = 32767;
				nNoise = (1 << 15) / nNoise;
			}
			pNoise[2] = (WORD)nNoise;
			pNoise += 3;

			*(pSumYW++) = (Y << 8);
			*(pSumYW++) = 256;
		}
	}

	return true;
}

bool CNLM_YDNSFilter_Basic::InitDNSYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	unsigned __int32 Y, tY, nNoise;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();
	int half = 1 << (2 * m_nShift - 1);
	int nShift = 2 * m_nShift;

	if (!pDiffImage->SetImageSize(nWidth, nHeight, 3))return false;
	if (!pSumYWImage->SetImageSize(nWidth, nHeight, 2))return false;

	if (m_bEdgeAdaptiveEnable)
	{
		if (!MeanVarYImage(pYUVImage, pDiffImage))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		WORD *pNoise = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *pY;	pY += nDim;
			tY = (Y >> m_nShift);
			if (tY < 0)tY = 0;	if (tY > 255)tY = 255;
			nNoise = (m_nNoiseYLUT[tY] + half) >> nShift;
//			nNoise = m_nNoiseYLUT[tY];

			if (m_bEdgeAdaptiveEnable)
			{
				int nMean = pNoise[0];
				int nVar = pNoise[1];
				nVar -= nMean*nMean;
				if (nVar < 0)nVar = 0;

				if (nVar >= 8 * nNoise)
				{
					nNoise = (nVar / 8 + nNoise) / 2;
				}
				else if (nVar * 8 <= nNoise)
				{
					nNoise = (nVar * 8 + nNoise) / 2;
				}
			}

			if (nNoise > 0)
			{
				if (nNoise > 32767)nNoise = 32767;
				nNoise = (1 << 15) / nNoise;
			}
			pNoise[2] = (WORD)nNoise;
			pNoise += 3;

			*(pSumYW++) = (Y << 8);
			*(pSumYW++) = 256;
		}
	}

	return true;
}

void CNLM_YDNSFilter_Basic::AccumulateSumYW(CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	int nWidth = pDiffImage->GetWidth();
	int nHeight = pDiffImage->GetHeight();
	int W;
	unsigned __int32 dY, Y, nInvNoise;

	for (y = 0; y < nHeight; y++)
	{
		WORD *pDiff = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			dY = *(pDiff++);
			Y = *(pDiff++);
			nInvNoise = *(pDiff++);

			if (nInvNoise > 0)
			{
				dY = (dY*nInvNoise) >> 15;
				W = 8 - dY;
				if (W >= 0)
				{
					W = (1 << W);
					pSumYW[0] += Y*W;
					pSumYW[1] += W;
				}
			}
			pSumYW += 2;
		}
	}
}

bool CNLM_YDNSFilter_Basic::DNSYImage_New(CYUV420Image *pYUVImage)
{
	int x, y;
	__int32 Y0, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	CBasicImageArray_WORD DiffImage;
	CBasicImageArray_UINT32 SumYWImage;

	if (!InitDNSYImage(pYUVImage, &DiffImage, &SumYWImage))return false;

	for (y = -2; y <= 2; y++)
	{
		for (x = -2; x <= 2; x++)
		{
			if (x == 0 && y == 0)continue;

			if (!DiffYImage(pYUVImage, &DiffImage, x, y))return false;
			AccumulateSumYW(&DiffImage, &SumYWImage);
		}
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		unsigned __int32 *pSumYW = SumYWImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y0 = *pY;
			Y = pSumYW[0] / pSumYW[1];	
			Y+= ((Y0 - Y)*m_nAddBackNoiseY)/16;
			if (Y < 0)Y = 0;	if (Y > 255)Y = 255;
			*(pY++) = (BYTE)Y;
			pSumYW += 2;
		}
	}

	return true;
}

bool CNLM_YDNSFilter_Basic::DNSYImage_New(CMultipleChannelImage *pYUVImage)
{
	int i, x, y;
	__int32 Y0, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();
	CBasicImageArray_WORD DiffImage;
	CBasicImageArray_UINT32 SumYWImage;

	for (i = 0; i < 256; i++)
	{
		m_nNoiseYLUT[i] >>= (2 * m_nShift);
	}

	if (!InitDNSYImage(pYUVImage, &DiffImage, &SumYWImage))return false;

	for (y = -2; y <= 2; y++)
	{
		for (x = -2; x <= 2; x++)
		{
			if (x == 0 && y == 0)continue;

			if (!DiffYImage(pYUVImage, &DiffImage, x, y))return false;
			AccumulateSumYW(&DiffImage, &SumYWImage);
		}
	}

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		unsigned __int32 *pSumYW = SumYWImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y0 = *pY;	

			Y = pSumYW[0] / pSumYW[1];
			Y += ((Y0 - Y)*m_nAddBackNoiseY) / 16;
			if (Y < 0)Y = 0;	if (Y > m_nMAXS)Y = m_nMAXS;

			*pY = (short)Y;
			
			pY += nDim;
			pSumYW += 2;
		}
	}

	return true;
}

//CNLM_YDNSFilter_ABS
void CNLM_YDNSFilter_ABS::BuildNoiseLUT(int nNoise, int nCurve[], int nLUT[])
{
	int Y, iY, dY, nGain;

	for (Y = 0; Y < 256; Y++)
	{
		iY = Y >> 5;
		dY = Y & 31;
		nGain = nCurve[iY];
		nGain += ((nCurve[iY + 1] - nCurve[iY])*dY + 16) / 32;
		nLUT[Y] = (nNoise*nGain+32)>>(6+m_nShift);
	}
}

void CNLM_YDNSFilter_ABS::HSumLine(BYTE *pInLine, WORD *pOutLine, int nWidth, int nInPitch)
{
	int x;
	WORD In[3];

	In[0] = In[1] = *pInLine;
	pInLine += nInPitch;
	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *pInLine;
		pInLine += nInPitch;

		*pOutLine = (In[0] + In[1] + In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	*pOutLine = (In[0] + In[1] + In[2]);
	pOutLine += 3;
}

void CNLM_YDNSFilter_ABS::VSumLine(WORD *pInLines[], BYTE *pOutLine, int nWidth, int nOutPitch)
{
	int x;
	WORD *pIn[3];
	WORD S;

	pIn[0] = pInLines[0];
	pIn[1] = pInLines[1];
	pIn[2] = pInLines[2];
	for (x = 0; x < nWidth; x++)
	{
		S = pIn[0][0];	pIn[0] += 3;
		S += pIn[1][0];	pIn[1] += 3;
		S += pIn[2][0];	pIn[2] += 3;
		S /= 9;

		*pOutLine = (BYTE)S;
		pOutLine += nOutPitch;
	}
}

bool CNLM_YDNSFilter_ABS::Avg3x3Image(CBasicImageArray_BYTE *pImage, int nChannel)
{
	int y;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	int nDim = pImage->GetDim();
	WORD *pLines[3];

	WORD *pBuffer = new WORD[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pLines[1] = pBuffer;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pImage->GetImageData() + nChannel;
	BYTE *pOutLine = pInLine;

	HSumLine(pInLine, pLines[1], nWidth, nDim);
	pInLine += nWidth*nDim;

	HSumLine(pInLine, pLines[2], nWidth, nDim);
	pInLine += nWidth*nDim;
	VSumLine(pLines, pOutLine, nWidth, nDim);
	pOutLine += nWidth*nDim;
	pLines[0] = pLines[1];
	pLines[1] = pLines[2];
	pLines[2] += 1;

	for (y = 1; y < nHeight - 1; y++)
	{
		HSumLine(pInLine, pLines[2], nWidth, nDim);
		pInLine += nWidth*nDim;
		VSumLine(pLines, pOutLine, nWidth, nDim);
		pOutLine += nWidth*nDim;

		WORD *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	pLines[2] = pLines[1];
	VSumLine(pLines, pOutLine, nWidth, nDim);

	delete[] pBuffer;
	return true;
}

bool CNLM_YDNSFilter_ABS::MeanVarYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pMeanVarImage)
{
	int x, y;
	BYTE Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pMeanVarImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		BYTE *pMeanVar = pMeanVarImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pY++);
			pMeanVar[0] = Y;
			pMeanVar[1] = Y;
			pMeanVar += nDim;
		}
	}

	if (!Avg3x3Image(pMeanVarImage, 0))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pMeanVar= pMeanVarImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			pMeanVar[0] = (pMeanVar[1] >= pMeanVar[0]) ? (pMeanVar[1] - pMeanVar[0]) : (pMeanVar[0] - pMeanVar[1]);
			pMeanVar += nDim;
		}
	}

	if (!Avg3x3Image(pMeanVarImage, 0))return false;

	return true;
}

bool CNLM_YDNSFilter_ABS::MeanVarYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pMeanVarImage)
{
	int x, y;
	BYTE Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nInDim = pYUVImage->GetDim();
	int nOutDim = pMeanVarImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		BYTE *pMeanVar = pMeanVarImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y =(BYTE) ((*pY)>>m_nShift);
			pY += nInDim;
			
			pMeanVar[0] = Y;
			pMeanVar[1] = Y;
			pMeanVar += nOutDim;
		}
	}

	if (!Avg3x3Image(pMeanVarImage, 0))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pMeanVar = pMeanVarImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			pMeanVar[0] = (pMeanVar[1] >= pMeanVar[0]) ? (pMeanVar[1] - pMeanVar[0]) : (pMeanVar[0] - pMeanVar[1]);
			pMeanVar += nOutDim;
		}
	}

	if (!Avg3x3Image(pMeanVarImage, 0))return false;

	return true;
}

void CNLM_YDNSFilter_ABS::DiffLine(BYTE *pSource, BYTE *pTarget, BYTE *pDiff, int nOffsetX, int nWidth, int nOutPitch)
{
	int x;
	BYTE Y0, Y1, dY;
	int W = MIN2(nWidth, nWidth - nOffsetX);
	int S = MAX2(nOffsetX, 0);

	pTarget += S;

	Y1 = pTarget[0];
	for (x = 0; x < -nOffsetX; x++)
	{
		Y0 = *(pSource++);

		dY = (Y1>=Y0)?(Y1-Y0):(Y0-Y1);
		pDiff[0] = (BYTE)dY;
		pDiff[1] = (BYTE)Y1;
		pDiff += nOutPitch;
	}
	for (; x < W; x++)
	{
		Y1 = *(pTarget++);
		Y0 = *(pSource++);

		dY = (Y1 >= Y0) ? (Y1 - Y0) : (Y0 - Y1);
		pDiff[0] = dY;
		pDiff[1] = Y1;
		pDiff += nOutPitch;
	}
	for (; x < nWidth; x++)
	{
		Y0 = *(pSource++);

		dY = (Y1 >= Y0) ? (Y1 - Y0) : (Y0 - Y1);
		pDiff[0] = dY;
		pDiff[1] = Y1;
		pDiff += nOutPitch;
	}
}

void CNLM_YDNSFilter_ABS::DiffLine(short *pSource, short *pTarget, BYTE *pDiff, int nOffsetX, int nWidth, int nInPitch, int nOutPitch)
{
	int x;
	BYTE Y0, Y1, dY, LY;
	int W = MIN2(nWidth, nWidth - nOffsetX);
	int S = MAX2(nOffsetX, 0);

	pTarget += S*nInPitch;

	Y1 = (BYTE)(pTarget[0] >> m_nShift);
	LY = (BYTE)(pTarget[0] & m_nMask);
	for (x = 0; x < -nOffsetX; x++)
	{
		Y0 = (BYTE)(pSource[0] >> m_nShift);
		pSource += nInPitch;

		dY = (Y1 >= Y0) ? (Y1 - Y0) : (Y0 - Y1);
		pDiff[0] = dY;
		pDiff[1] = Y1;
		pDiff[2] = LY;
		pDiff += nOutPitch;
	}
	for (; x < W; x++)
	{
		Y1 = (BYTE)(pTarget[0] >> m_nShift);
		LY = (BYTE)(pTarget[0] & m_nMask);
		pTarget += nInPitch;
		Y0 = (BYTE)(pSource[0] >> m_nShift);
		pSource += nInPitch;

		dY = (Y1 >= Y0) ? (Y1 - Y0) : (Y0 - Y1);
		pDiff[0] = dY;
		pDiff[1] = Y1;
		pDiff[2] = LY;
		pDiff += nOutPitch;
	}
	for (; x < nWidth; x++)
	{
		Y0 = (BYTE)(pSource[0] >> m_nShift);
		pSource += nInPitch;

		dY = (Y1 >= Y0) ? (Y1 - Y0) : (Y0 - Y1);
		pDiff[0] = dY;
		pDiff[1] = Y1;
		pDiff[2] = LY;
		pDiff += nOutPitch;
	}
}

bool CNLM_YDNSFilter_ABS::DiffYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pDiffImage, int nOffsetX, int nOffsetY)
{
	int y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nOutDim = pDiffImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pSource = pYUVImage->GetYLine(y);
		BYTE *pTarget = pYUVImage->GetYLine(y + nOffsetY);
		BYTE *pDiff = pDiffImage->GetImageLine(y);
		DiffLine(pSource, pTarget, pDiff, nOffsetX, nWidth, nOutDim);
	}

	return Avg3x3Image(pDiffImage, 0);
}

bool CNLM_YDNSFilter_ABS::DiffYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pDiffImage, int nOffsetX, int nOffsetY)
{
	int y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nInDim = pYUVImage->GetDim();
	int nOutDim = pDiffImage->GetDim();

	for (y = 0; y < nHeight; y++)
	{
		short *pSource = pYUVImage->GetImageLine(y);
		short *pTarget = pYUVImage->GetImageLine(y + nOffsetY);
		BYTE *pDiff = pDiffImage->GetImageLine(y);
		DiffLine(pSource, pTarget, pDiff, nOffsetX, nWidth, nInDim, nOutDim);
	}

	return Avg3x3Image(pDiffImage, 0);
}

bool CNLM_YDNSFilter_ABS::InitDNSYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	unsigned __int32 Y, nNoise;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	if (!pDiffImage->SetImageSize(nWidth, nHeight, 3))return false;
	if (!pSumYWImage->SetImageSize(nWidth, nHeight, 2))return false;

	if (m_bEdgeAdaptiveEnable)
	{
		if (!MeanVarYImage(pYUVImage, pDiffImage))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		BYTE *pNoise = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pY++);
			nNoise = m_nNoiseYLUT[Y];

			if (m_bEdgeAdaptiveEnable)
			{
				unsigned __int32 nVar = pNoise[0];

				if (nVar >= 4 * nNoise)
				{
					nNoise = (nVar / 4 + nNoise) / 2;
				}
				else if (nVar * 4 <= nNoise)
				{
					nNoise = (nVar * 4 + nNoise) / 2;
				}
			}

			if (nNoise > 0)
			{
				if (nNoise > 255)nNoise = 255;
				nNoise = (1 << 8) / nNoise;
				if (nNoise > 255)nNoise = 255;
			}
			
			pNoise[2] = (BYTE)nNoise;
			pNoise += 3;

			*(pSumYW++) = (Y << 8);
			*(pSumYW++) = 256;
		}
	}

	return true;
}

bool CNLM_YDNSFilter_ABS::InitDNSYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	unsigned __int32 Y, tY, nNoise;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();

	if (!pDiffImage->SetImageSize(nWidth, nHeight, 4))return false;
	if (!pSumYWImage->SetImageSize(nWidth, nHeight, 2))return false;

	if (m_bEdgeAdaptiveEnable)
	{
		if (!MeanVarYImage(pYUVImage, pDiffImage))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		BYTE *pNoise = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *pY;	pY += nDim;
			tY = (Y >> m_nShift);
			if (tY < 0)tY = 0;	if (tY > 255)tY = 255;
			nNoise = m_nNoiseYLUT[tY];

			if (m_bEdgeAdaptiveEnable)
			{
				unsigned __int32 nVar = pNoise[0];

				if (nVar >= 4 * nNoise)
				{
					nNoise = (nVar / 4 + nNoise) / 2;
				}
				else if (nVar * 4 <= nNoise)
				{
					nNoise = (nVar * 4 + nNoise) / 2;
				}
			}

			if (nNoise > 0)
			{
				if (nNoise > 255)nNoise = 255;
				nNoise = (1 << 8) / nNoise;
				if (nNoise > 255)nNoise = 255;
			}

			pNoise[3] = (BYTE)nNoise;
			pNoise += 4;

			*(pSumYW++) = (Y << 8);
			*(pSumYW++) = 256;
		}
	}

	return true;
}

void CNLM_YDNSFilter_ABS::AccumulateSumYW3(CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	int nWidth = pDiffImage->GetWidth();
	int nHeight = pDiffImage->GetHeight();
	int W;
	unsigned __int32 dY, Y, nInvNoise;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pDiff = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			dY = *(pDiff++);
			Y = *(pDiff++);
			nInvNoise = *(pDiff++);

			if (nInvNoise > 0)
			{
				dY = (dY*nInvNoise + 128) >> 8;
				W = 8 - dY;
				if (W >= 0)
				{
					W = (1 << W);
					pSumYW[0] += Y*W;
					pSumYW[1] += W;
				}
			}
			pSumYW += 2;
		}
	}
}

void CNLM_YDNSFilter_ABS::AccumulateSumYW4(CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage)
{
	int x, y;
	int nWidth = pDiffImage->GetWidth();
	int nHeight = pDiffImage->GetHeight();
	int W;
	unsigned __int32 dY, Y, LY, nInvNoise;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pDiff = pDiffImage->GetImageLine(y);
		unsigned __int32 *pSumYW = pSumYWImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			dY = *(pDiff++);
			Y = *(pDiff++);
			LY = *(pDiff++);
			nInvNoise = *(pDiff++);

			if (nInvNoise > 0)
			{
				dY = (dY*nInvNoise+128) >> 8;
				W = 8 - dY;
				if (W >= 0)
				{
					W = (1 << W);
					Y = (Y << m_nShift) | LY;
					pSumYW[0] += Y*W;
					pSumYW[1] += W;
				}
			}
			pSumYW += 2;
		}
	}
}

bool CNLM_YDNSFilter_ABS::DNSYImage(CYUV420Image *pYUVImage)
{
	int x, y;
	__int32 Y0, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	CBasicImageArray_BYTE DiffImage;
	CBasicImageArray_UINT32 SumYWImage;

	if (!InitDNSYImage(pYUVImage, &DiffImage, &SumYWImage))return false;

	for (y = -2; y <= 2; y++)
	{
		for (x = -2; x <= 2; x++)
		{
			if (x == 0 && y == 0)continue;

			if (!DiffYImage(pYUVImage, &DiffImage, x, y))return false;
			AccumulateSumYW3(&DiffImage, &SumYWImage);
		}
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pY = pYUVImage->GetYLine(y);
		unsigned __int32 *pSumYW = SumYWImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y0 = *pY;
			Y = pSumYW[0] / pSumYW[1];
			Y += ((Y0 - Y)*m_nAddBackNoiseY) / 16;
			if (Y < 0)Y = 0;	if (Y > 255)Y = 255;
			*(pY++) = (BYTE)Y;
			pSumYW += 2;
		}
	}

	return true;
}

bool CNLM_YDNSFilter_ABS::DNSYImage(CMultipleChannelImage *pYUVImage)
{
	int i, x, y;
	__int32 Y0, Y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();
	int nDim = pYUVImage->GetDim();
	CBasicImageArray_BYTE DiffImage;
	CBasicImageArray_UINT32 SumYWImage;

	for (i = 0; i < 256; i++)
	{
		m_nNoiseYLUT[i] >>= (2 * m_nShift);
	}

	if (!InitDNSYImage(pYUVImage, &DiffImage, &SumYWImage))return false;

	for (y = -2; y <= 2; y++)
	{
		for (x = -2; x <= 2; x++)
		{
			if (x == 0 && y == 0)continue;

			if (!DiffYImage(pYUVImage, &DiffImage, x, y))return false;
			AccumulateSumYW4(&DiffImage, &SumYWImage);
		}
	}

	for (y = 0; y < nHeight; y++)
	{
		short *pY = pYUVImage->GetImageLine(y);
		unsigned __int32 *pSumYW = SumYWImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y0 = *pY;

			Y = pSumYW[0] / pSumYW[1];
			Y += ((Y0 - Y)*m_nAddBackNoiseY) / 16;
			if (Y < 0)Y = 0;	if (Y > m_nMAXS)Y = m_nMAXS;

			*pY = (short)Y;

			pY += nDim;
			pSumYW += 2;
		}
	}

	return true;
}
