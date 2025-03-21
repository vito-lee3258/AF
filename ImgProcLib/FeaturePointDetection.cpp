#include "FeaturePointDetection.h"

const int nMask[25]={
	1,	4,	6,	4,	1,
	4, 16, 24, 16,	4,
	6, 24, 36, 24,	6,
	4, 16, 24, 16,	4,
	1,	4,	6,	4,	1
};

void CFeaturePointDetection::RotateBLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *(pInLine++);
		pOutLine += nPitch;
	}
}

bool CFeaturePointDetection::FastDetectCorner()
{
	int i, k, I0, I[16], T, N, P;
	int nTotalN, nTotalP;

	I0 = m_nYWin[3][3];
	T = MAX2((((255 - I0)*m_nRatio + 64) >> 7), m_nThre);

	I[0] = m_nYWin[0][3];
	I[1] = m_nYWin[0][4];
	I[2] = m_nYWin[1][5];
	I[3] = m_nYWin[2][6];
	I[4] = m_nYWin[3][6];
	I[5] = m_nYWin[4][6];
	I[6] = m_nYWin[5][5];
	I[7] = m_nYWin[6][4];
	I[8] = m_nYWin[6][3];
	I[9] = m_nYWin[6][2];
	I[10] = m_nYWin[5][1];
	I[11] = m_nYWin[4][0];
	I[12] = m_nYWin[3][0];
	I[13] = m_nYWin[2][0];
	I[14] = m_nYWin[1][1];
	I[15] = m_nYWin[0][2];

	nTotalN = nTotalP = 0;
	for (i = 0; i < 4; i++)
	{
		N = P = 0;
		for (k = i; k <= i + 12; k += 4)
		{
			I[k] -= I0;
			if (I[k] < -T)
			{
				N++;
			}
			else if (I[k] > T)
			{
				P++;
			}
			else
			{
				I[k] = 0;
			}
		}
		if (N < m_nPtThre1 && P < m_nPtThre1)return false;
		nTotalN += N;
		nTotalP += P;
	}
	if (nTotalN < m_nPtThre2 && nTotalP < m_nPtThre2)return false;

	return true;
}

float CFeaturePointDetection::ComputeHarrisCorner()
{
	int i, x1, y1, gx, gy;
	float sX2, sXY, sY2;
	float det, trace;

	sX2 = 0;
	sXY = 0;
	sY2 = 0;

	for (y1 = 1, i=0; y1 < 6; y1++)
	{
		for (x1 = 1; x1 < 6; x1++, i++)
		{
			gx = m_nYWin[y1][x1 + 1] - m_nYWin[y1][x1 - 1];
			gy = m_nYWin[y1 + 1][x1] - m_nYWin[y1 - 1][x1];
			
			sX2 += gx*gx*nMask[i];
			sXY += gx*gy*nMask[i];
			sY2 += gy*gy*nMask[i];
		}
	}
	sX2 /= 256.0;
	sY2 /= 256.0;
	sXY /= 256.0;

	det = sX2*sY2 - sXY*sXY;
	trace = sX2 + sY2;

	return det - m_fHarris_K*(trace*trace);
}

void CFeaturePointDetection::ComputeCornernessLine(BYTE *pInLines[], float *pOutLine, int nWidth, int nPitch)
{
	int i, j, x; 
	float R;
	BYTE *pIn[7];

	for (i = 0; i < 7; i++)
	{
		pIn[i] = pInLines[i];
		for (j = 3; j < 6; j++)
		{
			m_nYWin[i][j] = pIn[i][0];
			pIn[i] += nPitch;
		}
		for (j = 0; j < 3; j++)
		{
			m_nYWin[i][j] = m_nYWin[i][3];
		}
	}

	for (x = 0; x < nWidth - 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[i][6] = pIn[i][0];
			pIn[i] += nPitch;
		}

		if (FastDetectCorner())
		{
			R = ComputeHarrisCorner();
			if (R < m_nHarrisThre)R = 0;

			*(pOutLine++) = R;
		}
		else
		{
			*(pOutLine++) = 0;
		}

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
	for (; x < nWidth; x++)
	{
		if (FastDetectCorner())
		{
			R = ComputeHarrisCorner();
			if (R < m_nHarrisThre)R = 0;

			*(pOutLine++) = R;
		}
		else
		{
			*(pOutLine++) = 0;
		}

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
}

void CFeaturePointDetection::ComputeCornernessLine(BYTE *pInLines[], float *pOutLine, int nWidth, int nLeft, int nRight, int nPitch)
{
	int i, j, x;
	float R;
	BYTE *pIn[7];

	for (i = 0; i < 7; i++)
	{
		pIn[i] = pInLines[i];
		for (j = 3; j < 6; j++)
		{
			m_nYWin[i][j] = pIn[i][0];
			pIn[i] += nPitch;
		}
		for (j = 0; j < 3; j++)
		{
			m_nYWin[i][j] = m_nYWin[i][3];
		}
	}

	for (x = 0; x < nWidth - 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_nYWin[i][6] = pIn[i][0];
			pIn[i] += nPitch;
		}

		if (x >= nLeft&&x < nRight)
		{
			if (FastDetectCorner())
			{
				R = ComputeHarrisCorner();
				if (R < m_nHarrisThre)R = 0;

				*(pOutLine++) = R;
			}
			else
			{
				*(pOutLine++) = 0;
			}
		}

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
	for (; x < nWidth; x++)
	{
		if (x >= nLeft&&x < nRight)
		{
			if (FastDetectCorner())
			{
				R = ComputeHarrisCorner();
				if (R < m_nHarrisThre)R = 0;

				*(pOutLine++) = R;
			}
			else
			{
				*(pOutLine++) = 0;
			}
		}

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
}

bool CFeaturePointDetection::ComputeCornernessImage(CGrayImage *pInImage, CBasicImageArray_FLOAT *pOutImage)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[7];

	if (!pOutImage->SetImageSize(nWidth, nHeight, 1))return false;

	BYTE *pBuffer = new BYTE[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	for (i = 4; i < 7; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}

	BYTE *pInLine = pInImage->GetImageData();
	for (i = 3; i < 6; i++)
	{
		RotateBLine(pInLine, pInLines[i], nWidth, 7);
		pInLine += nWidth;
	}

	for (y = 0; y < 3; y++)
	{
		RotateBLine(pInLine, pInLines[6], nWidth, 7);
		pInLine += nWidth;

		float *pOutLine = pOutImage->GetImageLine(y);
		ComputeCornernessLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateBLine(pInLine, pInLines[6], nWidth, 7);
		pInLine += nWidth;

		float *pOutLine = pOutImage->GetImageLine(y);
		ComputeCornernessLine(pInLines, pOutLine, nWidth, 7);

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
		float *pOutLine = pOutImage->GetImageLine(y);
		ComputeCornernessLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CFeaturePointDetection::ComputeCornernessImage(CGrayImage *pInImage, CBasicImageArray_FLOAT *pOutImage, int nLeft, int nTop, int nRight, int nBottom)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[7];

	if (nRight > nWidth)nRight = nWidth;
	if (nBottom > nHeight)nBottom = nHeight;
	if (nBottom <= nTop || nRight <= nLeft)return false;

	if (!pOutImage->SetImageSize(nRight-nLeft, nBottom-nTop, 1))return false;

	BYTE *pBuffer = new BYTE[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	for (i = 4; i < 7; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}

	BYTE *pInLine = pInImage->GetImageData();
	for (i = 3; i < 6; i++)
	{
		RotateBLine(pInLine, pInLines[i], nWidth, 7);
		pInLine += nWidth;
	}

	for (y = 0; y < 3; y++)
	{
		RotateBLine(pInLine, pInLines[6], nWidth, 7);
		pInLine += nWidth;

		if (y >= nTop&&y < nBottom)
		{
			float *pOutLine = pOutImage->GetImageLine(y - nTop);
			ComputeCornernessLine(pInLines, pOutLine, nWidth, nLeft, nRight, 7);
		}

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateBLine(pInLine, pInLines[6], nWidth, 7);
		pInLine += nWidth;

		if (y >= nTop&&y < nBottom)
		{
			float *pOutLine = pOutImage->GetImageLine(y - nTop);
			ComputeCornernessLine(pInLines, pOutLine, nWidth, nLeft, nRight, 7);
		}

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
		if (y >= nTop&&y < nBottom)
		{
			float *pOutLine = pOutImage->GetImageLine(y - nTop);
			ComputeCornernessLine(pInLines, pOutLine, nWidth, nLeft, nRight, 7);
		}

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

void CFeaturePointDetection::RotateSLine(float *pInLine, float *pOutLine, int nWidth, int nPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *(pInLine++);
		pOutLine += nPitch;
	}
}

bool CFeaturePointDetection::NonMaximumWindow(float &fX, float &fY, float &max)
{
	int x, y, C;

	max = 0.0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 7; x++)
		{
			if (max < m_fYWin[y][x])max = m_fYWin[y][x];
		}
	}
	if (m_fYWin[3][3] < max || max == 0)return false;

	fX = fY = 0;
	C = 0;
	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 7; x++)
		{
			if (m_fYWin[y][x] == max)
			{
				fX += x;
				fY += y;
				C++;
			}
		}
	}
	if (C > 1)return false;

	fX /= C;
	fY /= C;
	fX -= 3;
	fY -= 3;

	return true;
}

void CFeaturePointDetection::NonMaximumLine(float *pInLines[], int nY, int nWidth, int nPitch, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum)
{
	int i, j, x;
	float *pIn[7];
	float fx, fy, R;

	for (i = 0; i < 7; i++)
	{
		pIn[i] = pInLines[i];
		for (j = 0; j < 6; j++)
		{
			m_fYWin[i][j] = pIn[i][0];
			pIn[i] += nPitch;
		}
	}

	for (x = 3; x < nWidth - 3; x++)
	{
		for (i = 0; i < 7; i++)
		{
			m_fYWin[i][6] = pIn[i][0];
			pIn[i] += nPitch;
		}

		if (m_fYWin[3][3] > 0)
		{
			if (NonMaximumWindow(fx, fy, R))
			{
				if (nPtNum < nMaxNum)
				{
					pPtList[nPtNum].fX = (float) x;
					pPtList[nPtNum].fY = (float) nY;
					pPtList[nPtNum].fScore = R;
					pPtList[nPtNum].nFlag = -1;

					pPtList[nPtNum].fX += fx;
					pPtList[nPtNum].fY += fy;
					nPtNum++;
				}
				else
				{
					int mini = 0;
					for (i = 1; i < nMaxNum; i++)
					{
						if (pPtList[i].fScore < pPtList[mini].fScore)
						{
							mini = i;
						}
					}
					if (pPtList[mini].fScore < R)
					{
						pPtList[mini].fX = (float) x;
						pPtList[mini].fY = (float) nY;
						pPtList[mini].fScore = R;
						pPtList[mini].nFlag = -1;

						pPtList[mini].fX += fx;
						pPtList[mini].fY += fy;
					}
				}
			}
		}

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_fYWin[i][j] = m_fYWin[i][j + 1];
			}
		}
	}
}

bool CFeaturePointDetection::NonMaximumImage(CBasicImageArray_FLOAT *pImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum)
{
	int i, y;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	float *pInLines[7];

	float *pBuffer = new float[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pBuffer;
	for (i = 1; i < 7; i++)
	{
		pInLines[i] = pInLines[i - 1] + 1;
	}

	float *pInLine = pImage->GetImageData();
	for (i = 0; i < 6; i++)
	{
		RotateSLine(pInLine, pInLines[i], nWidth, 7);
		pInLine += nWidth;
	}

	nPtNum = 0;
	for (y = 3; y < nHeight - 3; y++)
	{
		RotateSLine(pInLine, pInLines[6], nWidth, 7);
		pInLine += nWidth;

		NonMaximumLine(pInLines, y, nWidth, 7, pPtList, nPtNum, nMaxNum);

		float *pTemp = pInLines[0];
		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] = pTemp;
	}

	delete[] pBuffer;
	return true;
}

bool CFeaturePointDetection::DetectFeaturePoint(CGrayImage *pInYImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum)
{
	CBasicImageArray_FLOAT CornerImage;

	if (!ComputeCornernessImage(pInYImage, &CornerImage))return false;

	if (!NonMaximumImage(&CornerImage, pPtList, nPtNum, nMaxNum))return false;

	return true;
}

bool CFeaturePointDetection::DetectFeaturePoint(CGrayImage *pInYImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum, int nLeft, int nTop, int nRight, int nBottom)
{
	int i;
	CBasicImageArray_FLOAT CornerImage;

	if (!ComputeCornernessImage(pInYImage, &CornerImage, nLeft, nTop, nRight, nBottom))return false;

	if (!NonMaximumImage(&CornerImage, pPtList, nPtNum, nMaxNum))return false;

	for (i = 0; i < nPtNum; i++)
	{
		pPtList[i].fX += nLeft;
		pPtList[i].fY += nTop;
	}

	return true;
}

bool CFeaturePointDetection::MarkFeaturePoint(char *pFileName, CGrayImage *pInImage, TFeaturePoint pPtList[], int nNum, int nS, int nC)
{
	int i, j, x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	CRGBYUVImage RGBImage;

	if (!RGBImage.Create(nWidth, nHeight))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y);
		BYTE *pOut = RGBImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			for (i = 0; i < 3; i++)
			{
				*(pOut++) = (BYTE)Y;
			}
		}
	}

	for (i = 0; i < nNum; i++)
	{
		x = (int)(pPtList[i].fX + 0.5);
		y = (int)(pPtList[i].fY + 0.5);
		for (j = -nS; j <= nS; j++)
		{
			BYTE *pPixel1 = RGBImage.GetPixelAt(x + j, y);
			pPixel1[0] = (BYTE)((i*nC) & 255);
			pPixel1[1] = (BYTE)((i*nC * 2) & 255);
			pPixel1[2] = (BYTE)((255 - i*nC * 3) & 255);

			BYTE *pPixel2 = RGBImage.GetPixelAt(x, y + j);
			pPixel2[0] = (BYTE)((i*nC) & 255);
			pPixel2[1] = (BYTE)((i*nC * 2) & 255);
			pPixel2[2] = (BYTE)((255 - i*nC * 3) & 255);
		}
	}

	return RGBImage.SaveToBMPFile(pFileName);
}

bool CFeaturePointDetection::MarkFeaturePoint(char *pFileName, CGrayImage *pInImage, TFeaturePoint pPtList[], int nNum, int nS, int nLeft, int nTop, int nRight, int nBottom, int nC)
{
	int i, j, x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	CRGBYUVImage RGBImage;

	nLeft = MAX2(0, nLeft);
	nTop = MAX2(0, nTop);
	nRight = MIN2(nWidth, nRight);
	nBottom = MIN2(nHeight, nBottom);

	if (!RGBImage.Create(nRight - nLeft, nBottom - nTop))return false;

	for (y = nTop; y < nBottom; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y) + nLeft;
		BYTE *pOut = RGBImage.GetImageLine(y - nTop);
		for (x = nLeft; x < nRight; x++)
		{
			Y = *(pIn++);
			for (i = 0; i < 3; i++)
			{
				*(pOut++) = (BYTE)Y;
			}
		}
	}

	for (i = 0; i < nNum; i++)
	{
		x = (int)(pPtList[i].fX - nLeft + 0.5);
		y = (int)(pPtList[i].fY - nTop + 0.5);
		for (j = -nS; j <= nS; j++)
		{
			BYTE *pPixel1 = RGBImage.GetPixelAt(x + j, y);
			pPixel1[0] = (BYTE)((i*nC) & 255);
			pPixel1[1] = (BYTE)((i*nC * 2) & 255);
			pPixel1[2] = (BYTE)((255 - i*nC * 3) & 255);

			BYTE *pPixel2 = RGBImage.GetPixelAt(x, y + j);
			pPixel2[0] = (BYTE)((i*nC) & 255);
			pPixel2[1] = (BYTE)((i*nC * 2) & 255);
			pPixel2[2] = (BYTE)((255 - i*nC * 3) & 255);
		}
	}

	return RGBImage.SaveToBMPFile(pFileName);
}

bool CFeaturePointDetection::UpdateFeaturePoint(CGrayImage *pYImage, TFeaturePoint *pPtList, int nPtNum)
{
	int i, j, k, m, n, x, y, x1, y1, x2, y2, C;
	float R[5][5], max;

	for (i = 0; i < nPtNum; i++)
	{
		pPtList[i].fX *= 2.0;
		pPtList[i].fY *= 2.0;

		x = (int)(pPtList[i].fX + 0.5);
		y = (int)(pPtList[i].fY + 0.5);

		max = 0;
		for (y1 = y - 2, j = 0; y1 <= y + 2; y1++, j++)
		{
			for (x1 = x - 2, k = 0; x1 <= x + 2; x1++, k++)
			{
				for (y2 = y1 - 3, m = 0; y2 <= y1 + 3; y2++, m++)
				{
					for (x2 = x1 - 3, n = 0; x2 <= x1 + 3; x2++, n++)
					{
						m_nYWin[m][n] = pYImage->GetPixelAt(x2, y2)[0];
					}
				}

				R[j][k] = ComputeHarrisCorner();
				if (max < R[j][k])max = R[j][k];
			}
		}
		
		if (max > 0)
		{
			C = 0;
			pPtList[i].fX = pPtList[i].fY = 0.0;
			for (y1 = y - 2, j = 0; y1 <= y + 2; y1++, j++)
			{
				for (x1 = x - 2, k = 0; x1 <= x + 2; x1++, k++)
				{
					if (R[j][k] == max)
					{
						pPtList[i].fX += x1;
						pPtList[i].fY += y1;
						C++;
					}
				}
			}
			pPtList[i].fX /= C;
			pPtList[i].fY /= C;
		}
		pPtList[i].fScore = max;
	}
	
	return true;
}

bool CFeaturePointDetection::PyramidDetectPoint(CGrayImage *pPyramid, int nDetectLevel, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum)
{
	int i;

	if (!DetectFeaturePoint(pPyramid + nDetectLevel, pPtList, nPtNum, nMaxNum))return false;

	for (i = nDetectLevel - 1; i >= 0; i--)
	{
		if (!UpdateFeaturePoint(pPyramid + i, pPtList, nPtNum))return false;
	}

	return true;
}