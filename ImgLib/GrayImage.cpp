#include "RGBYUVImage.h"
#include "GrayImage.h"

bool CGrayImage::SaveToBMPFile(char *pFileName)
{
	int i, x, y;
	BYTE g;
	CRGBYUVImage BGRImage;

	if (!BGRImage.Create(m_nWidth, m_nHeight))return false;

	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pInLine = GetImageLine(y);
		BYTE *pOutLine = BGRImage.GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pInLine++);
			for (i = 0; i < 3; i++)
			{
				*(pOutLine++) = g;
			}
		}
	}

	return BGRImage.SaveToBMPFile(pFileName);
}

bool CGrayImage::GetHistogram(int nHist[])
{
	int x, y, g;

	if (m_pData == NULL)return false;
	
	for (g = 0; g < 256; g++)
	{
		nHist[g] = 0;
	}

	BYTE *pY = m_pData;
	for (y = 0; y < m_nHeight; y++)
	{
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pY++);
			nHist[g]++;
		}
	}

	return true;
}

bool CGrayImage::GetHistogram(int nHist[], int nLeft, int nTop, int nRight, int nBottom)
{
	int x, y, g;

	if (m_pData == NULL)return false;

	for (g = 0; g < 256; g++)
	{
		nHist[g] = 0;
	}

	nLeft = MAX2(0, nLeft);
	nTop = MAX2(0, nTop);
	nRight = MIN2(nRight, m_nWidth);
	nBottom = MIN2(nBottom, m_nHeight);

	if (nBottom <= nTop || nRight <= nLeft)return false;

	for (y = nTop; y < nBottom; y++)
	{
		BYTE *pY = GetImageLine(y) + nLeft;
		for (x = nLeft; x < nRight; x++)
		{
			g = *(pY++);
			nHist[g]++;
		}
	}

	return true;
}
