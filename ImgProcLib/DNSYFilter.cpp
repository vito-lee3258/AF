#include "DNSYFilter.h"

bool CDNSYFilter::ExtractYChannel(CRGBYUVImage *pYUVImage, CGrayImage *pYImage)
{
	int x, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	if (!pYImage->Create(nWidth, nHeight))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUV = pYUVImage->GetImageLine(y);
		BYTE *pY = pYImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			*(pY++) = pYUV[0];
			pYUV += 3;
		}
	}

	return true;
}

bool CDNSYFilter::ExtractYChannel(CYUV422Image *pYUVImage, CGrayImage *pYImage)
{
	int x, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	if (!pYImage->Create(nWidth, nHeight))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUYV = pYUVImage->GetImageLine(y);
		BYTE *pY = pYImage->GetImageLine(y);
		for (x = 0; x < nWidth; x += 2)
		{
			*(pY++) = pYUYV[0];
			*(pY++) = pYUYV[2];
			pYUYV += 4;
		}
	}

	return true;
}

bool CDNSYFilter::ExtractYChannel(CYUV420Image *pYUVImage, CGrayImage *pYImage)
{
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	if (!pYImage->Create(nWidth, nHeight))return false;

	memcpy(pYImage->GetImageData(), pYUVImage->GetYImage(), nWidth*nHeight);

	return true;
}

void CDNSYFilter::ReplaceYChannel(CGrayImage *pYImage, CRGBYUVImage *pYUVImage)
{
	int x, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUV = pYUVImage->GetImageLine(y);
		BYTE *pY = pYImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			pYUV[0] = *(pY++);
			pYUV += 3;
		}
	}
}

void CDNSYFilter::ReplaceYChannel(CGrayImage *pYImage, CYUV422Image *pYUVImage)
{
	int x, y;
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pYUYV = pYUVImage->GetImageLine(y);
		BYTE *pY = pYImage->GetImageLine(y);
		for (x = 0; x < nWidth; x += 2)
		{
			pYUYV[0] = *(pY++);
			pYUYV[2] = *(pY++);
			pYUYV += 4;
		}
	}
}

void CDNSYFilter::ReplaceYChannel(CGrayImage *pYImage, CYUV420Image *pYUVImage)
{
	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	memcpy(pYUVImage->GetYImage(), pYImage->GetImageData(), nWidth*nHeight);
}

