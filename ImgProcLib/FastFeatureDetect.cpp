#include "FastFeatureDetect.h"

void CFastFeatureDetection::RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nInPitch, int nOutPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *pInLine;
		pInLine += nInPitch;
		pOutLine += nOutPitch;
	}
}

int CFastFeatureDetection::InitDetectWindow()
{
	int i, j, k, I0, I[16], T, N, P;
	int nTotalN, nTotalP, nSumN, nSumP;
	int nScore;

	I0 = m_nYWin[3][3];
	T = MAX2(((I0*m_nRatio) >> 7), m_nThre);

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
	nSumN = nSumP = 0;
	for (i = 0; i < 4; i++)
	{
		N = P = 0;
		for (k = i; k <= i + 12; k += 4)
		{
			I[k] -= I0;
			if (I[k] < -T)
			{
				N++;
				nSumN -= I[k];
			}
			else if (I[k] > T)
			{
				P++;
				nSumP += I[k];
			}
			else
			{
				I[k] = 0;
			}
		}
		if (N < m_nPtThre1 && P < m_nPtThre1)return 0;
		nTotalN += N;
		nTotalP += P;
	}
	if (nTotalN < m_nPtThre2 && nTotalP < m_nPtThre2)return 0;

	if (nTotalN >= m_nPtThre2)
	{
		nSumN /= nTotalN;
		int dT = nSumN / 2;
		for (i = 0; i < 16; i++)
		{
			if (I[i] < 0 && I[i]>=-dT)
			{
				I[i] = 0;
			}
		}
		
		if (I[0] < 0)
		{
			N = 1;
			nScore = -I[0];
			I[0] = 0;
			for (j= 15; j > 0; j--)
			{
				if (I[j] < 0)
				{
					N++;
					nScore -= I[j];
					I[j] = 0;
				}
				else
				{
					break;
				}
			}
			for (k = 1; k < 16; k++)
			{
				if (I[k] < 0)
				{
					N++;
					nScore -= I[k];
					I[k] = 0;
				}
				else
				{
					break;
				}
			}
			if (N >= m_nPtThre2)
			{
				nScore /= N;
				if (nScore > 255)nScore = 255;
				return nScore;
			}
		}
		for (i = 1; i < 16; i++)
		{
			if (I[i] < 0)
			{
				N = 1;
				nScore = -I[i];
				I[i] = 0;
				for (k = i + 1; k < 16; k++)
				{
					if (I[k] < 0)
					{
						N++;
						nScore -= I[k];
						I[k] = 0;
					}
					else
					{
						break;
					}
				}
				if (N >= m_nPtThre2)
				{
					nScore /= N;
					if (nScore > 255)nScore = 255;
					return nScore;
				}
			}
		}
	}
	else
	{
		nSumP /= nTotalP;
		int dT = nSumP / 2;
		for (i = 0; i < 16; i++)
		{
			if (I[i] > 0 && I[i] <= dT)
			{
				I[i] = 0;
			}
		}

		if (I[0] > 0)
		{
			P = 1;
			nScore = I[0];
			I[0] = 0;
			for (j = 15; j > 0; j--)
			{
				if (I[j] > 0)
				{
					P++;
					nScore += I[j];
					I[j] = 0;
				}
				else
				{
					break;
				}
			}
			for (k = 1; k < 16; k++)
			{
				if (I[k] > 0)
				{
					P++;
					nScore += I[k];
					I[k] = 0;
				}
				else
				{
					break;
				}
			}
			if (P >= m_nPtThre2)
			{
				nScore /= P;
				if (nScore > 255)nScore = 255;
				return nScore;
			}
		}
		for (i = 1; i < 16; i++)
		{
			if (I[i] > 0)
			{
				P = 1;
				nScore = I[i];
				I[i] = 0;
				for (k = i + 1; k < 16; k++)
				{
					if (I[k] > 0)
					{
						P++;
						nScore += I[k];
						I[k] = 0;
					}
					else
					{
						break;
					}
				}
				if (P >= m_nPtThre2)
				{
					nScore /= P;
					if (nScore > 255)nScore = 255;
					return nScore;
				}
			}
		}
	}

	return 0;
}

void CFastFeatureDetection::InitDetectLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, j, x, Out;
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

		Out = InitDetectWindow();
		*(pOutLine++) = (BYTE)Out;

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
		Out = InitDetectWindow();
		*(pOutLine++) = (BYTE)Out;

		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 6; j++)
			{
				m_nYWin[i][j] = m_nYWin[i][j + 1];
			}
		}
	}
}

bool CFastFeatureDetection::InitDetect(CRGBYUVImage *pInImage, CGrayImage *pOutImage, int nChannel)
{
	int i, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pInLines[7];

	if (!pOutImage->Create(nWidth, nHeight))return false;

	BYTE *pBuffer = new BYTE[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	pInLines[4] = pInLines[3] + 1;
	pInLines[5] = pInLines[4] + 1;
	pInLines[6] = pInLines[5] + 1;

	BYTE *pInLine = pInImage->GetImageData() + nChannel;
	RotateLine(pInLine, pInLines[3], nWidth, 3, 7);
	pInLine += nWidth * 3;
	RotateLine(pInLine, pInLines[4], nWidth, 3, 7);
	pInLine += nWidth * 3;
	RotateLine(pInLine, pInLines[5], nWidth, 3, 7);
	pInLine += nWidth * 3;

	for (y = 0; y < 3; y++)
	{
		RotateLine(pInLine, pInLines[6], nWidth, 3, 7);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		InitDetectLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateLine(pInLine, pInLines[6], nWidth, 3, 7);
		pInLine += nWidth * 3;

		BYTE *pOutLine = pOutImage->GetImageLine(y);
		InitDetectLine(pInLines, pOutLine, nWidth, 7);

		BYTE *pTemp = pInLines[0];
		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] = pTemp;
	}
	for (; y < nHeight; y++)
	{
		BYTE *pOutLine = pOutImage->GetImageLine(y);
		InitDetectLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

int CFastFeatureDetection::NonMaxSupressionWindow()
{
	int i, j, Y0;

	Y0 = m_nYWin[3][3];
	if (Y0 > 0)
	{
		for (i = 0; i < 7; i++)
		{
			for (j = 0; j < 7; j++)
			{
				if (i == 3 && j == 3)continue;

				if (m_nYWin[i][j] >= Y0)
				{
					return 0;
				}
			}
		}

		return Y0;
	}

	return 0;
}

void CFastFeatureDetection::NonMaxSupressionLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch)
{
	int i, j, x, Out;
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

		Out = NonMaxSupressionWindow();
		*(pOutLine++) = (BYTE)Out;
		if (Out > 0)
		{
			m_nHist[Out]++;
			m_nTotalNum++;
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
		Out = NonMaxSupressionWindow();
		*(pOutLine++) = (BYTE)Out;
		if (Out > 0)
		{
			m_nHist[Out]++;
			m_nTotalNum++;
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

bool CFastFeatureDetection::NonMaxSupression(CGrayImage *pImage)
{
	int i, y;
	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();
	BYTE *pInLines[7];

	BYTE *pBuffer = new BYTE[nWidth * 7];
	if (pBuffer == NULL)return false;

	pInLines[0] = pInLines[1] = pInLines[2] = pInLines[3] = pBuffer;
	pInLines[4] = pInLines[3] + 1;
	pInLines[5] = pInLines[4] + 1;
	pInLines[6] = pInLines[5] + 1;

	BYTE *pInLine = pImage->GetImageData();
	RotateLine(pInLine, pInLines[3], nWidth, 1, 7);
	pInLine += nWidth;
	RotateLine(pInLine, pInLines[4], nWidth, 1, 7);
	pInLine += nWidth;
	RotateLine(pInLine, pInLines[5], nWidth, 1, 7);
	pInLine += nWidth;

	m_nTotalNum = 0;
	for (i = 0; i < 256; i++)
	{
		m_nHist[i] = 0;
	}

	for (y = 0; y < 3; y++)
	{
		RotateLine(pInLine, pInLines[6], nWidth, 1, 7);
		pInLine += nWidth;

		BYTE *pOutLine = pImage->GetImageLine(y);
		NonMaxSupressionLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] += 1;
	}
	for (; y < nHeight - 3; y++)
	{
		RotateLine(pInLine, pInLines[6], nWidth, 1, 7);
		pInLine += nWidth;

		BYTE *pOutLine = pImage->GetImageLine(y);
		NonMaxSupressionLine(pInLines, pOutLine, nWidth, 7);

		BYTE *pTemp = pInLines[0];
		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
		pInLines[6] = pTemp;
	}
	for (; y < nHeight; y++)
	{
		BYTE *pOutLine = pImage->GetImageLine(y);
		NonMaxSupressionLine(pInLines, pOutLine, nWidth, 7);

		for (i = 0; i < 6; i++)
		{
			pInLines[i] = pInLines[i + 1];
		}
	}

	delete[] pBuffer;
	return true;
}

bool CFastFeatureDetection::ProcessImage(CRGBYUVImage *pInImage, int nChannel, int nMaxNum, TFeaturePoint pPtList[], int &nNum)
{
	int x, y, g;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nThre = 0;
	CGrayImage DetectImage;

	if (!InitDetect(pInImage, &DetectImage, nChannel))return false;
	if (!NonMaxSupression(&DetectImage))return false;

	if (m_nTotalNum > nMaxNum)
	{
		if (m_nHist[255] >= nMaxNum)
		{
			nThre = 254;
		}
		else
		{
			for (nThre = 254; nThre > 0; nThre--)
			{
				m_nHist[nThre] += m_nHist[nThre + 1];
				if (m_nHist[nThre] >= nMaxNum)
				{
					break;
				}
			}
			nThre--;
		}
	}
	nNum = 0;
	for (y = 0; y < nHeight; y++)
	{
		BYTE *pS = DetectImage.GetImageLine(y);
		for (x = 0; x < nWidth; x++, pS++)
		{
			g = *pS;
			if (g > nThre)
			{
				if (nNum < nMaxNum)
				{
					pPtList[nNum].nX = x;
					pPtList[nNum].nY = y;
					pPtList[nNum].nScore = g;
					nNum++;

					*pS = 255;
				}
				else
				{
					*pS = 0;
				}
			}
		}
	}
	printf("Detected Feature Point=%d nThre=%d\n", nNum, nThre);

	return true;
}

bool CFastFeatureDetection::MarkFeaturePoint(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, int nChannel, TFeaturePoint pPtList[], int nNum)
{
	int i, x, y;
	BYTE Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (!pOutImage->Create(nWidth, nHeight))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y);
		BYTE *pOut = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = pIn[nChannel];	pIn += 3;
			*(pOut++) = Y;
			*(pOut++) = Y;
			*(pOut++) = Y;
		}
	}

	for (i = 0; i < nNum; i++)
	{
		x = pPtList[i].nX;
		y = pPtList[i].nY;
		BYTE *pOut = pOutImage->GetPixelAt(x, y);
		pOut[0] = pOut[1] = 0;
		pOut[2] = 255;
	}

	return true;
}
