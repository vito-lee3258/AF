#include "ZoomInFilter.h"

void CBicubicZoomInFilter::VZoomInLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, double fdY)
{
	int i, j, x, In[3][4], Out[3], E[3];
	BYTE *pIn[4];

	for (i = 0; i < 4; i++)
	{
		pIn[i] = pInLines[i];
	}

	E[0] = E[1] = E[2] = 0;
	for (x = 0; x < nWidth; x++)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 3; j++)
			{
				In[j][i] = pIn[i][j];
			}
			pIn[i] += 3;
		}

		for (j = 0; j < 3; j++)
		{
			Out[j] = (int)(Interpolate(In[j], fdY)*16.0);

			if (Out[j] < 0)Out[j] = 0;
			Out[j] += E[j];
			E[j] = Out[j] & 15;
			Out[j] >>= 4;
			if (Out[j] > 255)Out[j] = 255;

			*(pOutLine++) = (BYTE)Out[j];
		}
	}
}

void CBicubicZoomInFilter::HZoomInLine(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int i, j, x, nInX, nOutX, In[3][4], Out[3], E[3];
	double fInX, fdX;

	E[0] = E[1] = E[2] = 0;
	for (nOutX = 0; nOutX < nWidth; nOutX++)
	{
		fInX = m_fStartX + nOutX*m_fZoomRect_Width / nWidth;
		nInX = (int)fInX;
		fdX = fInX - nInX;

		for (i = 0, x = nInX - 1; i < 4; i++, x++)
		{
			if (x < 0)
			{
				for (j = 0; j < 3; j++)
				{
					In[j][i] = pInLine[j];
				}
			}
			else if (x >= nWidth)
			{
				for (j = 0; j < 3; j++)
				{
					In[j][i] = pInLine[(nWidth - 1) * 3 + j];
				}
			}
			else
			{
				for (j = 0; j < 3; j++)
				{
					In[j][i] = pInLine[x * 3 + j];
				}
			}
		}

		for (j = 0; j < 3; j++)
		{
			Out[j] = (int)(Interpolate(In[j], fdX)*16.0);

			if (Out[j] < 0)Out[j] = 0;
			Out[j] += E[j];
			E[j] = Out[j] & 15;
			Out[j] >>= 4;
			if (Out[j] > 255)Out[j] = 255;

			*(pOutLine++) = (BYTE)Out[j];
		}
	}
}

bool CBicubicZoomInFilter::ZoomIn(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage)
{
	int i, y, nInY, nOutY;
	double fInY, fdY;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pVLines[4];
	BYTE *pHLine = new BYTE[nWidth * 3];
	if (pHLine == NULL)return false;

	for (nOutY = 0; nOutY < nHeight; nOutY++)
	{
		BYTE *pOutLine = pOutImage->GetImageLine(nOutY);

		fInY = m_fStartY + nOutY*m_fZoomRect_Height / nHeight;
		nInY = (int) fInY;
		fdY = fInY - nInY;

		for (i = 0, y = nInY - 1; i < 4; i++, y++)
		{
			if (y < 0)
			{
				pVLines[i] = pInImage->GetImageLine(y);
			}
			else if (y >= nHeight)
			{
				pVLines[i] = pInImage->GetImageLine(nHeight - 1);
			}
			else
			{
				pVLines[i] = pInImage->GetImageLine(y);
			}
		}

		VZoomInLine(pVLines, pHLine, nWidth, fdY);
		HZoomInLine(pHLine, pOutLine, nWidth);
	}

	delete[] pHLine;
	return true;
}
