#include "DNSUVFilter.h"

bool CDNSUVFilter::ExtractYUVSubImage(CYUV422Image *pYUV422Image, CRGBYUVImage *pYUVImage)
{
	int i, x, y, YUYV[4], YUV[3];
	int nWidth = pYUV422Image->GetWidth();
	int nHeight = pYUV422Image->GetHeight();

	if (!pYUVImage->Create(nWidth / 2, nHeight))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUYV = pYUV422Image->GetImageLine(y);
		BYTE *pYUV = pYUVImage->GetImageLine(y);
		for (x = 0; x < nWidth; x += 2)
		{
			for (i = 0; i < 4; i++)
			{
				YUYV[i] = *(pYUYV++);
			}
			YUV[0] = (YUYV[0] + YUYV[2]) >> 1;
			YUV[1] = YUYV[1];
			YUV[2] = YUYV[3];

			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV[i];
			}
		}
	}

	return true;
}

bool CDNSUVFilter::ExtractYUVSubImage(CYUV420Image *pYUV420Image, CRGBYUVImage *pYUVImage)
{
	int i, j, x, y, Y[4], YUV[3];
	int nWidth = pYUV420Image->GetWidth();
	int nHeight = pYUV420Image->GetHeight();

	if (!pYUVImage->Create(nWidth / 2, nHeight / 2))return false;

	for (y = 0; y < nHeight; y += 2)
	{
		BYTE *pY[2];
		pY[0] = pYUV420Image->GetYLine(y);
		pY[1] = pYUV420Image->GetYLine(y + 1);
		BYTE *pUV = pYUV420Image->GetUVLine(y);
		BYTE *pYUV = pYUVImage->GetImageLine(y / 2);
		for (x = 0; x < nWidth; x += 2)
		{
			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					Y[i * 2 + j] = *(pY[i]++);
				}
			}

			YUV[0] = (Y[0] + Y[1] + Y[2] + Y[3]) >> 2;
			YUV[1] = *(pUV++);
			YUV[2] = *(pUV++);

			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV[i];
			}
		}
	}

	return true;
}

void CDNSUVFilter::ReplaceUVChannel(CRGBYUVImage *pYUVImage, CYUV422Image *pYUV422Image)
{
	int x, y;
	int nWidth = pYUV422Image->GetWidth();
	int nHeight = pYUV422Image->GetHeight();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUV = pYUVImage->GetImageLine(y);
		BYTE *pYUYV = pYUV422Image->GetImageLine(y);
		for (x = 0; x < nWidth; x += 2)
		{
			pYUYV[1] = pYUV[1];
			pYUYV[3] = pYUV[2];

			pYUYV += 4;
			pYUV += 3;
		}
	}
}

void CDNSUVFilter::ReplaceUVChannel(CRGBYUVImage *pYUVImage, CYUV420Image *pYUV420Image)
{
	int x, y;
	int nWidth = pYUV420Image->GetWidth();
	int nHeight = pYUV420Image->GetHeight();

	for (y = 0; y < nHeight; y += 2)
	{
		BYTE *pYUV = pYUVImage->GetImageLine(y / 2);
		BYTE *pUV = pYUV420Image->GetUVLine(y);
		for (x = 0; x < nWidth; x += 2)
		{
			pUV[0] = pYUV[1];
			pUV[1] = pYUV[2];

			pUV += 2;
			pYUV += 3;
		}
	}
}
