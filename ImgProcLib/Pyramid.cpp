#include "Pyramid.h"

bool CPyramid::SubtractEdge(CRGBYUVImage *pInImage0, CRGBYUVImage *pInImage1, CMultipleChannelImage *pOutImage)
{
	int i, x, y, H[3];
	int nWidth = MIN2(pInImage0->GetWidth(), pInImage1->GetWidth());
	int nHeight = MIN2(pInImage0->GetHeight(), pInImage1->GetHeight());

	if (!pOutImage->Create(nWidth, nHeight, 3, 10))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pInLine0 = pInImage0->GetImageLine(y);
		BYTE *pInLine1 = pInImage1->GetImageLine(y);
		short *pOutLine = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				H[i] = *(pInLine0++);
				H[i] -= *(pInLine1++);
				*(pOutLine++) = (short)H[i];
			}
		}
	}

	return true;
}

bool CPyramid::SubtractEdge(CGrayImage *pInImage0, CGrayImage *pInImage1, CMultipleChannelImage *pOutImage)
{
	int x, y, H;
	int nWidth = MIN2(pInImage0->GetWidth(), pInImage1->GetWidth());
	int nHeight = MIN2(pInImage0->GetHeight(), pInImage1->GetHeight());

	if (!pOutImage->Create(nWidth, nHeight, 1, 10))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pInLine0 = pInImage0->GetImageLine(y);
		BYTE *pInLine1 = pInImage1->GetImageLine(y);
		short *pOutLine = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			H = *(pInLine0++);
			H -= *(pInLine1++);
			*(pOutLine++) = H;
		}
	}

	return true;
}

bool CPyramid::SubtractEdge(CYUV420Image *pInImage0, CYUV420Image *pInImage1, CMultipleChannelImage *pOutImage)
{
	int x, y, H;
	int nWidth = MIN2(pInImage0->GetWidth(), pInImage1->GetWidth());
	int nHeight = MIN2(pInImage0->GetHeight(), pInImage1->GetHeight());

	if (!pOutImage->Create(nWidth, nHeight, 1, 10))return false;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pInLine0 = pInImage0->GetYLine(y);
		BYTE *pInLine1 = pInImage1->GetYLine(y);
		short *pOutLine = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			H = *(pInLine0++);
			H -= *(pInLine1++);
			*(pOutLine++) = (short)H;
		}
	}

	return true;
}

bool CPyramid::AddBackEdge(CRGBYUVImage *pInImage, CMultipleChannelImage *pEdgeImage, CRGBYUVImage *pOutImage)
{
	int i, x, y, YUV[3];
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage != pInImage)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y);
		short *pEdge = pEdgeImage->GetImageLine(y);
		BYTE *pOut = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				YUV[i] = *(pIn++);
				YUV[i] += *(pEdge++);
				if (YUV[i] < 0)YUV[i] = 0;	if (YUV[i] > 255)YUV[i] = 255;

				*(pOut++) = (BYTE)YUV[i];
			}
		}
	}

	return true;
}

bool CPyramid::AddBackEdge(CGrayImage *pInImage, CMultipleChannelImage *pEdgeImage, CGrayImage *pOutImage)
{
	int x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage != pInImage)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pIn = pInImage->GetImageLine(y);
		short *pEdge = pEdgeImage->GetImageLine(y);
		BYTE *pOut = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			Y += *(pEdge++);
			if (Y < 0)Y = 0;	if (Y > 255)Y = 255;

			*(pOut++) = (BYTE)Y;
		}
	}

	return true;
}


bool CPyramid::AddBackEdge(CYUV420Image *pInImage, CMultipleChannelImage *pEdgeImage, CYUV420Image *pOutImage)
{
	int x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	int nEdgeDim = pEdgeImage->GetDim();

	if (pOutImage != pInImage)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
		memcpy(pOutImage->GetUVImage(), pInImage->GetUVImage(), (nWidth / 2)*(nHeight / 2) * 2);
	}

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pIn = pInImage->GetYLine(y);
		short *pEdge = pEdgeImage->GetImageLine(y);
		BYTE *pOut = pOutImage->GetYLine(y);
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			Y += pEdge[0];	pEdge += nEdgeDim;
			if (Y < 0)Y = 0;	if (Y > 255)Y = 255;

			*(pOut++) = (BYTE)Y;
		}
	}
	
	return true;
}
