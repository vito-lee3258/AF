#include "DownScaleFilter.h"

//#define __DITHERING_

void CBicubicDownScaleFilter::VDownScaleLine(BYTE *pInLines[], BYTE *pOutLine, int nDim, int nWidth, double fdY)
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
			for (j = 0; j < nDim; j++)
			{
				In[j][i] = pIn[i][j];
			}
			pIn[i] += nDim;
		}

		for (j = 0; j < nDim; j++)
		{
			Out[j] = (int)(Interpolate(In[j], fdY)*16.0);

			if (Out[j] < 0)Out[j] = 0;
#ifdef __DITHERING_
			Out[j] += E[j];
			E[j] = Out[j] & 15;
#else
			Out[j] += 8;
#endif
			Out[j] >>= 4;
			if (Out[j] > 255)Out[j] = 255;

			*(pOutLine++) = (BYTE)Out[j];
		}
	}
}

void CBicubicDownScaleFilter::HDownScaleLine(BYTE *pInLine, BYTE *pOutLine, int nDim, int nInWidth, int nOutWidth)
{
	int i, j, x, nInX, nOutX, In[3][4], Out[3], E[3];
	double fX;

	E[0] = E[1] = E[2] = 0;
	for (nOutX = 0; nOutX < nOutWidth; nOutX++)
	{
		fX = m_fXScale / 2.0 - 0.5 + nOutX*m_fXScale;
		nInX = (int)fX;
		fX -= nInX;

		for (i =0, x=nInX-1; i <4; i++, x++)
		{
			if (x < 0)
			{
				for (j = 0; j < nDim; j++)
				{
					In[j][i] = pInLine[j];
				}
			}
			else if (x >= nInWidth)
			{
				for (j = 0; j < nDim; j++)
				{
					In[j][i] = pInLine[(nInWidth - 1) * nDim + j];
				}
			}
			else
			{
				for (j = 0; j < nDim; j++)
				{
					In[j][i] = pInLine[x * nDim + j];
				}
			}
		}

		for (j = 0; j < nDim; j++)
		{
			Out[j] = (int)(Interpolate(In[j], fX)*16.0);

			if (Out[j] < 0)Out[j] = 0;
#ifdef __DITHERING_
			Out[j] += E[j];
			E[j] = Out[j] & 15;
#else
			Out[j] += 8;
#endif
			Out[j] >>= 4;
			if (Out[j] > 255)Out[j] = 255;

			*(pOutLine++) = (BYTE)Out[j];
		}
	}
}

bool CBicubicDownScaleFilter::DownScaleImage(BYTE *pInImage, BYTE *pOutImage, int nDim, int nInWidth, int nInHeight, int nOutWidth, int nOutHeight)
{
	int i, y, nInY, nOutY;
	double fY;
	BYTE *pVLines[4];
	BYTE *pHLine = new BYTE[nInWidth * nDim];
	if (pHLine == NULL)return false;

	BYTE *pOutLine = pOutImage;
	for (nOutY = 0; nOutY < nOutHeight; nOutY++)
	{
		fY = m_fYScale / 2.0 - 0.5 + nOutY*m_fYScale;
		nInY = (int)fY;
		fY -= nInY;

		for (i = 0, y = nInY - 1; i < 4; i++, y++)
		{
			if (y < 0)
			{
				pVLines[i] = pInImage;
			}
			else if (y < nInHeight)
			{
				pVLines[i] = pInImage + y*nInWidth*nDim;
			}
			else
			{
				pVLines[i] = pInImage + (nInHeight - 1)*nInWidth*nDim;
			}
		}
		VDownScaleLine(pVLines, pHLine, nDim, nInWidth, fY);
		HDownScaleLine(pHLine, pOutLine, nDim, nInWidth, nOutWidth);

		pOutLine += nOutWidth*nDim;
	}

	delete[] pHLine;
	return true;
}