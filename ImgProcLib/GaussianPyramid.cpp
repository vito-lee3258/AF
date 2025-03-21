#include "GaussianPyramid.h"

bool CGaussianPyramid::ExtendImage(CGrayImage *pInImage, CGrayImage *pOutImage, int nS)
{
	int x, y;
	BYTE Y;
	int nMask = (1 << nS) - 1;
	int nInWidth = pInImage->GetWidth();
	int nInHeight = pInImage->GetHeight();
	int nOutWidth = nInWidth + (((1 << nS) - (nInWidth&nMask))&nMask);
	int nOutHeight = nInHeight + (((1 << nS) - (nInHeight&nMask))&nMask);

	if (nS < 0)return false;
	if (nInWidth == nOutWidth&&nInHeight == nOutHeight)
	{
		return pOutImage->Copy(pInImage);
	}

	if (pOutImage->GetWidth() != nOutWidth || pOutImage->GetHeight() != nOutHeight)
	{
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;
	}

	for (y = 0; y < nInHeight; y++)
	{
		BYTE *pInY = pInImage->GetImageLine(y);
		BYTE *pOutY = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			Y = *(pInY++);
			*(pOutY++) = Y;
		}
		for (; x < nOutWidth; x++)
		{
			*(pOutY++) = Y;
		}
	}
	for (; y < nOutHeight; y++)
	{
		BYTE *pInY = pInImage->GetImageLine(nInHeight - 1);
		BYTE *pOutY = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			Y = *(pInY++);
			*(pOutY++) = Y;
		}
		for (; x < nOutWidth; x++)
		{
			*(pOutY++) = Y;
		}
	}

	return true;
}

bool CGaussianPyramid::ExtendImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage, int nS)
{
	int i, x, y;
	int nMask = (1 << nS) - 1;
	int nInWidth = pInImage->GetWidth();
	int nInHeight = pInImage->GetHeight();
	int nOutWidth = nInWidth + (((1 << nS) - (nInWidth&nMask))&nMask);
	int nOutHeight = nInHeight + (((1 << nS) - (nInHeight&nMask))&nMask);
	int nDim = pInImage->GetDim();

	if (nS < 0)return false;
	if (nInWidth == nOutWidth&&nInHeight == nOutHeight)
	{
		return pOutImage->Copy(pInImage);
	}

	if (pOutImage->GetWidth() != nOutWidth || pOutImage->GetHeight() != nOutHeight || pOutImage->GetDim() != nDim)
	{
		if (!pOutImage->Create(nOutWidth, nOutHeight, nDim, pInImage->m_nBit))return false;
	}
	pOutImage->m_nBit = pInImage->m_nBit;
	pOutImage->m_nBLK = pInImage->m_nBLK;
	pOutImage->m_nMAXS = pInImage->m_nMAXS;

	for (y = 0; y < nOutHeight; y++)
	{
		short *pIn = pInImage->GetImageLine(y);
		short *pOut = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			for (i = 0; i < nDim; i++)
			{
				*(pOut++) = *(pIn++);
			}
		}
		pIn -= nDim;
		for (; x < nOutWidth; x++)
		{
			for (i = 0; i < nDim; i++)
			{
				*(pOut++) = pIn[i];
			}
		}
	}

	return true;
}

bool CGaussianPyramid::ExtendImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, int nS)
{
	int i, x, y;
	BYTE BGR[3];
	int nMask = (1 << nS) - 1;
	int nInWidth = pInImage->GetWidth();
	int nInHeight = pInImage->GetHeight();
	int nOutWidth = nInWidth + (((1 << nS) - (nInWidth&nMask))&nMask);
	int nOutHeight = nInHeight + (((1 << nS) - (nInHeight&nMask))&nMask);

	if (nS < 0)return false;
	if (nInWidth == nOutWidth&&nInHeight == nOutHeight)
	{
		return pOutImage->Copy(pInImage);
	}

	if (pOutImage->GetWidth() != nOutWidth || pOutImage->GetHeight() != nOutHeight)
	{
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;
	}

	for (y = 0; y < nOutHeight; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y);
		BYTE *pOut = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				BGR[i] = *(pIn++);
				*(pOut++) = BGR[i];
			}
		}
		for (; x < nOutWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				*(pOut++) = BGR[i];
			}
		}
	}

	return true;
}

bool CGaussianPyramid::ExtendImage(CYUV420Image *pInImage, CYUV420Image *pOutImage, int nS)
{
	int i, x, y;
	BYTE Y, UV[2];
	int nMask = (1 << nS) - 1;
	int nInWidth = pInImage->GetWidth();
	int nInHeight = pInImage->GetHeight();
	int nOutWidth = nInWidth + (((1 << nS) - (nInWidth&nMask))&nMask);
	int nOutHeight = nInHeight + (((1 << nS) - (nInHeight&nMask))&nMask);

	if (nS < 1)return false;
	if (nInWidth == nOutWidth&&nInHeight == nOutHeight)
	{
		return pOutImage->Copy(pInImage);
	}

	if (pOutImage->GetWidth() != nOutWidth || pOutImage->GetHeight() != nOutHeight)
	{
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;
	}

	for (y = 0; y < nOutHeight; y++)
	{
		BYTE *pInY = pInImage->GetYLine(y);
		BYTE *pOutY = pOutImage->GetYLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			Y = *(pInY++);
			*(pOutY++) = Y;
		}
		for (; x < nOutWidth; x++)
		{
			*(pOutY++) = Y;
		}
	}

	for (y = 0; y < nOutHeight; y += 2)
	{
		BYTE *pInUV = pInImage->GetUVLine(y);
		BYTE *pOutUV = pOutImage->GetUVLine(y);
		for (x = 0; x < nInWidth; x += 2)
		{
			for (i = 0; i < 2; i++)
			{
				UV[i] = *(pInUV++);
				*(pOutUV++) = UV[i];
			}
		}
		for (; x < nOutWidth; x += 2)
		{
			for (i = 0; i < 2; i++)
			{
				*(pOutUV++) = UV[i];
			}
		}
	}

	return true;
}

void CGaussianPyramid::RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nDim, int nInPitch, int nOutPitch)
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

void CGaussianPyramid::RotateLine(short *pInLine, short *pOutLine, int nWidth, int nDim, int nInPitch, int nOutPitch)
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

void CGaussianPyramid::DownScaleWindow(int Out[], int nDim)
{
	int i, j; 
	
	DownScaleVFilter(3, nDim);
	DownScaleVFilter(4, nDim);

	for (i = 0; i < nDim; i++)
	{
		Out[i] = DownScaleHFilter(i);

		for (j = 0; j < 3; j++)
		{
			m_nVBuf[i][j] = m_nVBuf[i][j + 2];
		}
	}
}

void CGaussianPyramid::StartDownScaleLine(int nDim)
{
	DownScaleVFilter(0, nDim);
	DownScaleVFilter(1, nDim);
	DownScaleVFilter(2, nDim);
}

void CGaussianPyramid::DownScaleLine(BYTE *pInLines[], BYTE *pOutLine, int nInWidth, int nDim, int nInPitch, int nOutPitch)
{
	int i, j, k, x, Out[8];
	BYTE *pIn[5];

	for (i = 0; i < 5; i++)
	{
		pIn[i] = pInLines[i];
		for (k = 0; k < nDim; k++)
		{
			m_nWin[i][0][k] = m_nWin[i][1][k] = m_nWin[i][2][k] = pIn[i][k];
		}
		pIn[i] += nInPitch;
	}

	StartDownScaleLine(nDim);
	for (x = 0; x < nInWidth - 2; x += 2)
	{
		for (i = 0; i < 5; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][3][k] = pIn[i][k];
			}
			pIn[i] += nInPitch;
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][4][k] = pIn[i][k];
			}
			pIn[i] += nInPitch;
		}

		DownScaleWindow(Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;

		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 2][k];
				}
			}
		}
	}
	for (; x < nInWidth; x += 2)
	{
		for (i = 0; i < 5; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][3][k] = m_nWin[i][4][k] = m_nWin[i][2][k];
			}
		}

		DownScaleWindow(Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;

		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 2][k];
				}
			}
		}
	}
}

bool CGaussianPyramid::DownScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[5];

	if (((nWidth & 1) != 0) || ((nHeight & 1) != 0))return false;
	if (pOutImage == pInImage)return false;
	if (!pOutImage->Create(nWidth >> 1, nHeight >> 1))return false;

	BYTE *pBuffer = new BYTE[nWidth * 3 * 5];
	if (pBuffer == NULL)return false;
	pInLines[0] = pInLines[1] = pInLines[2] = pBuffer;
	pInLines[3] = pInLines[2] + 3;
	pInLines[4] = pInLines[3] + 3;

	BYTE *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[2], nWidth, 3, 3, 3 * 5);
	pInLine += nWidth * 3;

	m_bDitheringEnable = true;
	m_nDownScaleDitheringE[0] = m_nDownScaleDitheringE[1] = m_nDownScaleDitheringE[2] = 0;
	for (y = 0; y < 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, 3, 3, 3 * 5);
		pInLine += nWidth * 3;
		RotateLine(pInLine, pInLines[4], nWidth, 3, 3, 3 * 5);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 3, 3 * 5, 3);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pInLines[4] + 3;
		pInLines[4] += 6;
	}
	for (; y < nHeight - 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, 3, 3, 3 * 5);
		pInLine += nWidth * 3;
		RotateLine(pInLine, pInLines[4], nWidth, 3, 3, 3 * 5);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 3, 3 * 5, 3);

		BYTE *pTemp0 = pInLines[0];
		BYTE *pTemp1 = pInLines[1];
		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pTemp0;
		pInLines[4] = pTemp1;
	}
	for (; y < nHeight; y += 2)
	{
		pInLines[4] = pInLines[3] = pInLines[2];

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 3, 3 * 5, 3);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CGaussianPyramid::DownScaleImage(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[5];

	if (((nWidth & 1) != 0) || ((nHeight & 1) != 0))return false;
	if (pOutImage == pInImage)return false;
	if (!pOutImage->Create(nWidth >> 1, nHeight >> 1))return false;

	BYTE *pBuffer = new BYTE[nWidth * 5];
	if (pBuffer == NULL)return false;
	pInLines[0] = pInLines[1] = pInLines[2] = pBuffer;
	pInLines[3] = pInLines[2] + 1;
	pInLines[4] = pInLines[3] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[2], nWidth, 1, 1, 5);
	pInLine += nWidth;

	m_bDitheringEnable = false;
	m_nDownScaleDitheringE[0] = m_nDownScaleDitheringE[1] = m_nDownScaleDitheringE[2] = 0;
	for (y = 0; y < 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, 1, 1, 5);
		pInLine += nWidth;
		RotateLine(pInLine, pInLines[4], nWidth, 1, 1, 5);
		pInLine += nWidth;

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 1);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pInLines[4] + 1;
		pInLines[4] += 2;
	}
	for (; y < nHeight - 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, 1, 1, 5);
		pInLine += nWidth;
		RotateLine(pInLine, pInLines[4], nWidth, 1, 1, 5);
		pInLine += nWidth;

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 1);

		BYTE *pTemp0 = pInLines[0];
		BYTE *pTemp1 = pInLines[1];
		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pTemp0;
		pInLines[4] = pTemp1;
	}
	for (; y < nHeight; y += 2)
	{
		pInLines[4] = pInLines[3] = pInLines[2];

		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 1);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CGaussianPyramid::DownScaleImage(CYUV420Image *pInImage, CRGBYUVImage *pOutImage)
{
	int i, x, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[5];

	if (((nWidth & 1) != 0) || ((nHeight & 1) != 0))return false;
	if (!pOutImage->Create(nWidth >> 1, nHeight >> 1))return false;

	BYTE *pBuffer = new BYTE[nWidth * 5];
	if (pBuffer == NULL)return false;
	pInLines[0] = pInLines[1] = pInLines[2] = pBuffer;
	pInLines[3] = pInLines[2] + 1;
	pInLines[4] = pInLines[3] + 1;

	BYTE *pInYLine = pInImage->GetYImage();
	RotateLine(pInYLine, pInLines[2], nWidth, 1, 1, 5);
	pInYLine += nWidth;

	m_bDitheringEnable = true;
	m_nDownScaleDitheringE[0] = m_nDownScaleDitheringE[1] = m_nDownScaleDitheringE[2] = 0;
	for (y = 0; y < 2; y += 2)
	{
		RotateLine(pInYLine, pInLines[3], nWidth, 1, 1, 5);
		pInYLine += nWidth;
		RotateLine(pInYLine, pInLines[4], nWidth, 1, 1, 5);
		pInYLine += nWidth;

		BYTE *pInUVLine = pInImage->GetUVLine(y);
		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 3);
		for (x = 0; x < (nWidth>>1); x++)
		{
			pOutLine[1] = *(pInUVLine++);
			pOutLine[2] = *(pInUVLine++);
			pOutLine += 3;
		}

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pInLines[4] + 1;
		pInLines[4] += 2;
	}
	for (; y < nHeight - 2; y += 2)
	{
		RotateLine(pInYLine, pInLines[3], nWidth, 1, 1, 5);
		pInYLine += nWidth;
		RotateLine(pInYLine, pInLines[4], nWidth, 1, 1, 5);
		pInYLine += nWidth;

		BYTE *pInUVLine = pInImage->GetUVLine(y);
		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 3);
		for (x = 0; x < (nWidth>>1); x++)
		{
			pOutLine[1] = *(pInUVLine++);
			pOutLine[2] = *(pInUVLine++);
			pOutLine += 3;
		}

		BYTE *pTemp0 = pInLines[0];
		BYTE *pTemp1 = pInLines[1];
		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pTemp0;
		pInLines[4] = pTemp1;
	}
	for (; y < nHeight; y += 2)
	{
		pInLines[4] = pInLines[3] = pInLines[2];

		BYTE *pInUVLine = pInImage->GetUVLine(y);
		BYTE *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, 1, 5, 3);
		for (x = 0; x < (nWidth>>1); x++)
		{
			pOutLine[1] = *(pInUVLine++);
			pOutLine[2] = *(pInUVLine++);
			pOutLine += 3;
		}

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
	}

	delete[] pBuffer;
	return true;
}

void CGaussianPyramid::DownScaleLine(short *pInLines[], short *pOutLine, int nInWidth, int nDim, int nPitch)
{
	int i, j, k, x, Out[8];
	short *pIn[5];

	for (i = 0; i < 5; i++)
	{
		pIn[i] = pInLines[i];
		for (k = 0; k < nDim; k++)
		{
			m_nWin[i][0][k] = m_nWin[i][1][k] = m_nWin[i][2][k] = pIn[i][k];
		}
		pIn[i] += nPitch;
	}

	StartDownScaleLine(nDim);
	for (x = 0; x < nInWidth - 2; x += 2)
	{
		for (i = 0; i < 5; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][3][k] = pIn[i][k];
			}
			pIn[i] += nPitch;
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][4][k] = pIn[i][k];
			}
			pIn[i] += nPitch;
		}

		DownScaleWindow(Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}

		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 2][k];
				}
			}
		}
	}
	for (; x < nInWidth; x += 2)
	{
		for (i = 0; i < 5; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][3][k] = m_nWin[i][4][k] = m_nWin[i][2][k];
			}
		}

		DownScaleWindow(Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}

		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 2][k];
				}
			}
		}
	}
}

bool CGaussianPyramid::DownScaleImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nDim = pInImage->GetDim();
	short *pInLines[5];

	if (((nWidth & 1) != 0) || ((nHeight & 1) != 0)||nDim>8)return false;
	if (pOutImage == pInImage)return false;
	if (!pOutImage->Create(nWidth >> 1, nHeight >> 1, nDim, pInImage->m_nBit))return false;
	pOutImage->m_nBLK = pInImage->m_nBLK;
	pOutImage->m_nMAXS = pInImage->m_nMAXS;

	short *pBuffer = new short[nWidth * nDim * 5];
	if (pBuffer == NULL)return false;
	pInLines[0] = pInLines[1] = pInLines[2] = pBuffer;
	pInLines[3] = pInLines[2] + nDim;
	pInLines[4] = pInLines[3] + nDim;

	short *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[2], nWidth, nDim, nDim, nDim * 5);
	pInLine += nWidth * nDim;

	m_bDitheringEnable = false;
	m_nDownScaleDitheringE[0] = m_nDownScaleDitheringE[1] = m_nDownScaleDitheringE[2] = 0;
	for (y = 0; y < 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, nDim, nDim, nDim * 5);
		pInLine += nWidth * nDim;
		RotateLine(pInLine, pInLines[4], nWidth, nDim, nDim, nDim * 5);
		pInLine += nWidth * nDim;

		short *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, nDim, nDim * 5);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pInLines[4] + nDim;
		pInLines[4] += nDim*2;
	}
	for (; y < nHeight - 2; y += 2)
	{
		RotateLine(pInLine, pInLines[3], nWidth, nDim, nDim, nDim * 5);
		pInLine += nWidth * nDim;
		RotateLine(pInLine, pInLines[4], nWidth, nDim, nDim, nDim * 5);
		pInLine += nWidth * nDim;

		short *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, nDim, nDim * 5);

		short *pTemp0 = pInLines[0];
		short *pTemp1 = pInLines[1];
		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
		pInLines[3] = pTemp0;
		pInLines[4] = pTemp1;
	}
	for (; y < nHeight; y += 2)
	{
		pInLines[4] = pInLines[3] = pInLines[2];

		short *pOutLine = pOutImage->GetImageLine(y >> 1);
		DownScaleLine(pInLines, pOutLine, nWidth, nDim, nDim * 5);

		for (i = 0; i < 3; i++)
		{
			pInLines[i] = pInLines[i + 2];
		}
	}

	delete[] pBuffer;
	return true;
}

void CGaussianPyramid::UpScaleWindow(int bXFlag, int bYFlag, int Out[], int nDim)
{
	int i;

	if (bXFlag == 0)
	{
		UpScaleVFilter(2, bYFlag, nDim);
		for (i = 0; i < nDim; i++)
		{
			Out[i]=UpScaleHFilter(0, i);
		}
	}
	else
	{
		for (i = 0; i < nDim; i++)
		{
			Out[i] = UpScaleHFilter(1, i);

			m_nVBuf[i][0] = m_nVBuf[i][1];
			m_nVBuf[i][1] = m_nVBuf[i][2];
		}
	}
}

void CGaussianPyramid::StartUpScaleLine(int bYFlag, int nDim)
{
	UpScaleVFilter(0, bYFlag, nDim);
	UpScaleVFilter(1, bYFlag, nDim);
}

void CGaussianPyramid::UpScaleLine(BYTE *pInLines[], BYTE *pOutLine, int bYFlag, int nWidth, int nDim, int nInPitch, int nOutPitch)
{
	int i, j, k, x, Out[3];
	BYTE *pIn[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		for (k = 0; k < nDim; k++)
		{
			m_nWin[i][0][k] = m_nWin[i][1][k] = pIn[i][k];
		}
		pIn[i] += nInPitch;
	}

	StartUpScaleLine(bYFlag, nDim);
	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][2][k] = pIn[i][k];
			}
			pIn[i] += nInPitch;
		}

		UpScaleWindow(0, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;

		UpScaleWindow(1, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 2; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 1][k];
				}
			}
		}
	}
	{
		UpScaleWindow(0, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;

		UpScaleWindow(1, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			if (Out[k] < 0)Out[k] = 0;	if (Out[k] > 255)Out[k] = 255;
			pOutLine[k] = (BYTE)Out[k];
		}
		pOutLine += nOutPitch;
	}
}

bool CGaussianPyramid::UpScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[3];

	if (pOutImage->GetWidth() != (nWidth << 1) || pOutImage->GetHeight() != (nHeight << 1))
	{
		if (!pOutImage->Create((nWidth << 1), (nHeight << 1)))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3 * 3];
	if (pBuffer == NULL)return false;

	pInLines[0] = pBuffer;
	pInLines[1] = pInLines[0] + 3;
	pInLines[2] = pInLines[1] + 3;

	BYTE *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[0], nWidth, 3, 3, 3 * 3);
	RotateLine(pInLine, pInLines[1], nWidth, 3, 3, 3 * 3);
	pInLine += nWidth * 3;

	m_bDitheringEnable = true;
	m_nUpScaleDitheringE[0] = m_nUpScaleDitheringE[1] = m_nUpScaleDitheringE[2] = 0;
	for (y = 0; y < nHeight-1; y++)
	{
		RotateLine(pInLine, pInLines[2], nWidth, 3, 3, 3 * 3);
		pInLine += nWidth * 3;

		BYTE *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 3, 3 * 3, 3);

		BYTE *pOutLine1 = pOutImage->GetImageLine(y * 2+1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 3, 3 * 3, 3);

		BYTE *pTemp = pInLines[0];
		pInLines[0] = pInLines[1];
		pInLines[1] = pInLines[2];
		pInLines[2] = pTemp;
	}
	{
		pInLines[2] = pInLines[1];

		BYTE *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 3, 3 * 3, 3);

		BYTE *pOutLine1 = pOutImage->GetImageLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 3, 3 * 3, 3);
	}

	delete[] pBuffer;
	return true;
}

bool CGaussianPyramid::UpScaleImage(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[3];

	if (!pOutImage->Create((nWidth << 1), (nHeight << 1)))return false;

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pInLines[0] = pBuffer;
	pInLines[1] = pInLines[0] + 1;
	pInLines[2] = pInLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[0], nWidth, 1, 1, 3);
	RotateLine(pInLine, pInLines[1], nWidth, 1, 1, 3);
	pInLine += nWidth;

	m_bDitheringEnable = true;
	m_nUpScaleDitheringE[0] = m_nUpScaleDitheringE[1] = m_nUpScaleDitheringE[2] = 0;
	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pInLines[2], nWidth, 1, 1, 3);
		pInLine += nWidth;

		BYTE *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 1, 3, 1);

		BYTE *pOutLine1 = pOutImage->GetImageLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 1, 3, 1);

		BYTE *pTemp = pInLines[0];
		pInLines[0] = pInLines[1];
		pInLines[1] = pInLines[2];
		pInLines[2] = pTemp;
	}
	{
		pInLines[2] = pInLines[1];

		BYTE *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 1, 3, 1);

		BYTE *pOutLine1 = pOutImage->GetImageLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 1, 3, 1);
	}

	delete[] pBuffer;
	return true;
}

bool CGaussianPyramid::UpScaleImage(CRGBYUVImage *pInImage, CYUV420Image *pOutImage)
{
	int x, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[3];

	if (!pOutImage->Create((nWidth << 1), (nHeight << 1)))return false;

	for (y = 0; y < nHeight; y ++)
	{
		BYTE *pInYUV = pInImage->GetImageLine(y);
		BYTE *pOutUV = pOutImage->GetUVLine(y<<1);
		for (x = 0; x < nWidth; x++)
		{
			*(pOutUV++) = pInYUV[1];
			*(pOutUV++) = pInYUV[2];
			pInYUV += 3;
		}
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pInLines[0] = pBuffer;
	pInLines[1] = pInLines[0] + 1;
	pInLines[2] = pInLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[0], nWidth, 1, 3, 3);
	RotateLine(pInLine, pInLines[1], nWidth, 1, 3, 3);
	pInLine += nWidth*3;

	m_bDitheringEnable = true;
	m_nUpScaleDitheringE[0] = m_nUpScaleDitheringE[1] = m_nUpScaleDitheringE[2] = 0;
	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pInLines[2], nWidth, 1, 3, 3);
		pInLine += nWidth*3;

		BYTE *pOutLine0 = pOutImage->GetYLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 1, 3, 1);

		BYTE *pOutLine1 = pOutImage->GetYLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 1, 3, 1);

		BYTE *pTemp = pInLines[0];
		pInLines[0] = pInLines[1];
		pInLines[1] = pInLines[2];
		pInLines[2] = pTemp;
	}
	{
		pInLines[2] = pInLines[1];

		BYTE *pOutLine0 = pOutImage->GetYLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, 1, 3, 1);

		BYTE *pOutLine1 = pOutImage->GetYLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, 1, 3, 1);
	}

	delete[] pBuffer;
	return true;
}

void CGaussianPyramid::UpScaleLine(short *pInLines[], short *pOutLine, int bYFlag, int nWidth, int nDim, int nPitch)
{
	int i, j, k, x, Out[8];
	short *pIn[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		for (k = 0; k < nDim; k++)
		{
			m_nWin[i][0][k] = m_nWin[i][1][k] = pIn[i][k];
		}
		pIn[i] += nPitch;
	}

	StartUpScaleLine(bYFlag, nDim);
	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			for (k = 0; k < nDim; k++)
			{
				m_nWin[i][2][k] = pIn[i][k];
			}
			pIn[i] += nPitch;
		}

		UpScaleWindow(0, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}
		UpScaleWindow(1, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 2; j++)
			{
				for (k = 0; k < nDim; k++)
				{
					m_nWin[i][j][k] = m_nWin[i][j + 1][k];
				}
			}
		}
	}
	{
		UpScaleWindow(0, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}
		UpScaleWindow(1, bYFlag, Out, nDim);
		for (k = 0; k < nDim; k++)
		{
			*(pOutLine++) = (short)Out[k];
		}
	}
}

bool CGaussianPyramid::UpScaleImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nDim = pInImage->GetDim();
	short *pInLines[3];

	if (nDim > 8)return false;
	if (!pOutImage->Create((nWidth << 1), (nHeight << 1), nDim, pInImage->m_nBit))return false;
	pOutImage->m_nBLK = pInImage->m_nBLK;
	pOutImage->m_nMAXS = pInImage->m_nMAXS;

	short *pBuffer = new short[nWidth * nDim * 3];
	if (pBuffer == NULL)return false;

	pInLines[0] = pBuffer;
	pInLines[1] = pInLines[0] + nDim;
	pInLines[2] = pInLines[1] + nDim;

	short *pInLine = pInImage->GetImageData();
	RotateLine(pInLine, pInLines[0], nWidth, nDim, nDim, nDim * 3);
	RotateLine(pInLine, pInLines[1], nWidth, nDim, nDim, nDim * 3);
	pInLine += nWidth * nDim;

	m_bDitheringEnable = false;
	m_nUpScaleDitheringE[0] = m_nUpScaleDitheringE[1] = m_nUpScaleDitheringE[2] = 0;
	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pInLines[2], nWidth, nDim, nDim, nDim * 3);
		pInLine += nWidth * nDim;

		short *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, nDim, nDim * 3);

		short *pOutLine1 = pOutImage->GetImageLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, nDim, nDim * 3);

		short *pTemp = pInLines[0];
		pInLines[0] = pInLines[1];
		pInLines[1] = pInLines[2];
		pInLines[2] = pTemp;
	}
	{
		pInLines[2] = pInLines[1];

		short *pOutLine0 = pOutImage->GetImageLine(y * 2);
		UpScaleLine(pInLines, pOutLine0, 0, nWidth, nDim, nDim * 3);

		short *pOutLine1 = pOutImage->GetImageLine(y * 2 + 1);
		UpScaleLine(pInLines, pOutLine1, 1, nWidth, nDim, nDim * 3);
	}

	delete[] pBuffer;
	return true;
}
