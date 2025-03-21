#include "SmoothFilter5x5.h"

void CSmoothFilter5x5::HSmoothFilterLine(BYTE *pInLine, WORD *pOutLine, int nWidth, int nPitch, int nDim)
{
	int i, j, x;
	WORD In[3][5];

	for (i = 0; i < nDim; i++)
	{
		In[i][0] = In[i][1] = In[i][2] = *(pInLine++);
	}
	for (i = 0; i < nDim; i++)
	{
		In[i][3] = *(pInLine++);
	}

	for (x = 0; x < nWidth - 2; x++)
	{
		for (i = 0; i < nDim; i++)
		{
			In[i][4] = *(pInLine++);

			pOutLine[i] = (In[i][0] + In[i][4]);
			pOutLine[i] += (In[i][1] + In[i][3]) * 4;
			pOutLine[i] += In[i][2] * 6;

			for (j = 0; j < 4; j++)
			{
				In[i][j] = In[i][j + 1];
			}
		}
		pOutLine += nPitch;
	}
	for (; x < nWidth; x++)
	{
		for (i = 0; i < nDim; i++)
		{
			pOutLine[i] = (In[i][0] + In[i][4]);
			pOutLine[i] += (In[i][1] + In[i][3]) * 4;
			pOutLine[i] += In[i][2] * 6;

			for (j = 0; j < 4; j++)
			{
				In[i][j] = In[i][j + 1];
			}
		}
		pOutLine += nPitch;
	}
}

void CSmoothFilter5x5::VSmoothFilterLine(WORD *pInLines[], BYTE *pOutLine, int nE[], int nWidth, int nPitch, int nDim, bool bDitheringEnable)
{
	int i, j, x;
	WORD In[3][5], Out[3];
	WORD *pIn[5];

	for (j = 0; j < 5; j++)
	{
		pIn[j] = pInLines[j];
	}

	for (x = 0; x < nWidth; x++)
	{
		for (j = 0; j < 5; j++)
		{
			for (i = 0; i < nDim; i++)
			{
				In[i][j] = pIn[j][i];
			}
			pIn[j] += nPitch;
		}

		for (i = 0; i < nDim; i++)
		{
			Out[i] = (In[i][0] + In[i][4]);
			Out[i] += (In[i][1] + In[i][3]) * 4;
			Out[i] += In[i][2] * 6;

			if (bDitheringEnable)
			{
				Out[i] += nE[i];
				nE[i] = (Out[i] & 255);
			}
			else
			{
				Out[i] += 128;
			}
			Out[i] >>= 8;

			*(pOutLine++) = (BYTE)Out[i];
		}
	}
}

bool CSmoothFilter5x5::SmoothYUVImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, bool bDitheringEnable)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nE[3];
	WORD *pHLines[5];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	WORD *pBuffer = new WORD[nWidth * 15];
	if (pBuffer == NULL)return false;

	pHLines[0] = pHLines[1] = pHLines[2] = pBuffer;
	pHLines[3] = pHLines[2] + 3;
	pHLines[4] = pHLines[3] + 3;

	BYTE *pInLine = pInImage->GetImageData();
	HSmoothFilterLine(pInLine, pHLines[2], nWidth, 15, 3);
	pInLine += nWidth * 3;
	HSmoothFilterLine(pInLine, pHLines[3], nWidth, 15, 3);
	pInLine += nWidth * 3;

	nE[0] = nE[1] = nE[2] = 0;
	for (y = 0; y < 2; y++)
	{
		HSmoothFilterLine(pInLine, pHLines[4], nWidth, 15, 3);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, nE, nWidth, 15, 3, bDitheringEnable);

		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
		pHLines[4] += 3;
	}
	for (; y < nHeight - 2; y++)
	{
		HSmoothFilterLine(pInLine, pHLines[4], nWidth, 15, 3);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, nE, nWidth, 15, 3, bDitheringEnable);

		WORD *pTemp = pHLines[0];
		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
		pHLines[4] = pTemp;
	}
	pHLines[4] = pHLines[3];
	for (; y < nHeight; y++)
	{
		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, nE, nWidth, 15, 3, bDitheringEnable);

		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CSmoothFilter5x5::SmoothYImage(CGrayImage *pInImage, CGrayImage *pOutImage, bool bDitheringEnable)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nE;
	WORD *pHLines[5];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	WORD *pBuffer = new WORD[nWidth * 5];
	if (pBuffer == NULL)return false;

	pHLines[0] = pHLines[1] = pHLines[2] = pBuffer;
	pHLines[3] = pHLines[2] + 1;
	pHLines[4] = pHLines[3] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	HSmoothFilterLine(pInLine, pHLines[2], nWidth, 5, 1);
	pInLine += nWidth;
	HSmoothFilterLine(pInLine, pHLines[3], nWidth, 5, 1);
	pInLine += nWidth;

	nE = 0;
	for (y = 0; y < 2; y++)
	{
		HSmoothFilterLine(pInLine, pHLines[4], nWidth, 5, 1);
		pInLine += nWidth;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, &nE, nWidth, 5, 1, bDitheringEnable);

		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
		pHLines[4] += 1;
	}
	for (; y < nHeight - 2; y++)
	{
		HSmoothFilterLine(pInLine, pHLines[4], nWidth, 5, 1);
		pInLine += nWidth;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, &nE, nWidth, 5, 1, bDitheringEnable);

		WORD *pTemp = pHLines[0];
		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
		pHLines[4] = pTemp;
	}
	pHLines[4] = pHLines[3];
	for (; y < nHeight; y++)
	{
		BYTE *pOutLine = pOutImage->GetImageLine(y);
		VSmoothFilterLine(pHLines, pOutLine, &nE, nWidth, 5, 1, bDitheringEnable);

		for (i = 0; i < 4; i++)
		{
			pHLines[i] = pHLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

