#include "MorphologyFilter.h"

_inline BYTE Min3(BYTE In0, BYTE In1, BYTE In2)
{
	if (In0 < In1)
	{
		if (In2 < In0)
		{
			return In2;
		}
		else
		{
			return In0;
		}
	}
	else
	{
		if (In2 < In1)
		{
			return In2;
		}
		else
		{
			return In1;
		}
	}
}

__inline BYTE Max3(BYTE In0, BYTE In1, BYTE In2)
{
	if (In0 < In1)
	{
		if (In2 < In1)
		{
			return In1;
		}
		else
		{
			return In2;
		}
	}
	else
	{
		if (In2 < In0)
		{
			return In0;
		}
		else
		{
			return In2;
		}
	}
}

__inline BYTE Min4(BYTE C0, BYTE C1, BYTE C2, BYTE C3)
{
	return  MIN2(MIN2(C0, C1), MIN2(C2, C3));
}

__inline BYTE Max4(BYTE C0, BYTE C1, BYTE C2, BYTE C3)
{
	return  MAX2(MAX2(C0, C1), MAX2(C2, C3));
}

__inline BYTE Min5(BYTE C0, BYTE C1, BYTE C2, BYTE C3, BYTE C4)
{
	return  Min3(Min3(C0, C1, C2), C3, C4);
}

__inline BYTE Max5(BYTE C0, BYTE C1, BYTE C2, BYTE C3, BYTE C4)
{
	return  Max3(Max3(C0, C1, C2), C3, C4);
}

void CMorphologyFilter::RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = *(pInLine++);
		pOutLine += nPitch;
	}
}

void CMorphologyFilter::FillLine(BYTE *pOutLine, BYTE nVal, int nWidth, int nPitch)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*pOutLine = nVal;
		pOutLine += nPitch;
	}
}

void CMorphologyFilter::HErosion3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;
	BYTE In[3];

	In[0] = 255;
	In[1] = *(pInLine++);

	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *(pInLine++);
		*pOutLine = (In[0] & In[1] & In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	*pOutLine = (In[0] & In[1]);
	pOutLine += 3;
}

void CMorphologyFilter::HDilation3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;
	BYTE In[3];

	In[0] = 0;
	In[1] = *(pInLine++);

	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *(pInLine++);
		*pOutLine = (In[0] | In[1] | In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	*pOutLine = (In[0] | In[1]);
	pOutLine += 3;
}

void CMorphologyFilter::HMin3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;
	BYTE In[3];

	In[0] = 255;
	In[1] = *(pInLine++);

	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *(pInLine++);
		*pOutLine = Min3(In[0], In[1], In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	In[2] = 255;
	*pOutLine = Min3(In[0], In[1], In[2]);
	pOutLine += 3;
}

void CMorphologyFilter::HMax3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;
	BYTE In[3];

	In[0] = 0;
	In[1] = *(pInLine++);

	for (x = 0; x < nWidth - 1; x++)
	{
		In[2] = *(pInLine++);
		*pOutLine = Max3(In[0], In[1], In[2]);
		pOutLine += 3;

		In[0] = In[1];
		In[1] = In[2];
	}
	In[2] = 0;
	*pOutLine = Max3(In[0], In[1], In[2]);
	pOutLine += 3;
}

void CMorphologyFilter::VErosion3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*(pOutLine++) = (pInLine[0] & pInLine[1] & pInLine[2]);
		pInLine += 3;
	}
}

void CMorphologyFilter::VDilation3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*(pOutLine++) = (pInLine[0] | pInLine[1] | pInLine[2]);
		pInLine += 3;
	}
}

void CMorphologyFilter::VMin3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*(pOutLine++) = Min3(pInLine[0], pInLine[1], pInLine[2]);
		pInLine += 3;
	}
}

void CMorphologyFilter::VMax3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth)
{
	int x;

	for (x = 0; x < nWidth; x++)
	{
		*(pOutLine++) = Max3(pInLine[0], pInLine[1], pInLine[2]);
		pInLine += 3;
	}
}

bool CMorphologyFilter::Erosion3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 255, nWidth, 3);
	HErosion3Line(pInLine, pLines[1], nWidth);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		HErosion3Line(pInLine, pLines[2], nWidth);
		pInLine += nWidth;
		VErosion3Line(pBuffer, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 255, nWidth, 3);
	VErosion3Line(pBuffer, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Min3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 255, nWidth, 3);
	HMin3Line(pInLine, pLines[1], nWidth);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		HMin3Line(pInLine, pLines[2], nWidth);
		pInLine += nWidth;
		VMin3Line(pBuffer, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 255, nWidth, 3);
	VMin3Line(pBuffer, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Dilation3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 0, nWidth, 3);
	HDilation3Line(pInLine, pLines[1], nWidth);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		HDilation3Line(pInLine, pLines[2], nWidth);
		pInLine += nWidth;
		VDilation3Line(pBuffer, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 0, nWidth, 3);
	VDilation3Line(pBuffer, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Max3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 0, nWidth, 3);
	HMax3Line(pInLine, pLines[1], nWidth);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		HMax3Line(pInLine, pLines[2], nWidth);
		pInLine += nWidth;
		VMax3Line(pBuffer, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 0, nWidth, 3);
	VMax3Line(pBuffer, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

void CMorphologyFilter::Erosion3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth)
{
	int i, x;
	BYTE Buffer[9];
	BYTE *pIn[3];
	BYTE *pWin[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		pWin[i] = Buffer + i * 3;
	}
	for (i = 0; i < 3; i++)
	{
		pWin[0][i] = 255;
		pWin[1][i] = pIn[i][0];
		pIn[i] += 3;
	}

	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pWin[2][i] = pIn[i][0];
			pIn[i] += 3;
		}

		*(pOutLine++) = (pWin[0][1] & pWin[1][0] & pWin[1][1] & pWin[1][2] & pWin[2][1]);

		BYTE *pTemp = pWin[0];
		pWin[0] = pWin[1];
		pWin[1] = pWin[2];
		pWin[2] = pTemp;
	}
	{
		*(pOutLine++) = (pWin[0][1] & pWin[1][0] & pWin[1][1] & pWin[1][2]);
	}
}

void CMorphologyFilter::Min3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth)
{
	int i, x;
	BYTE Buffer[9];
	BYTE *pIn[3];
	BYTE *pWin[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		pWin[i] = Buffer + i * 3;
	}
	for (i = 0; i < 3; i++)
	{
		pWin[0][i] = 255;
		pWin[1][i] = pIn[i][0];
		pIn[i] += 3;
	}

	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pWin[2][i] = pIn[i][0];
			pIn[i] += 3;
		}

		*(pOutLine++) = Min5(pWin[0][1], pWin[1][0], pWin[1][1], pWin[1][2], pWin[2][1]);

		BYTE *pTemp = pWin[0];
		pWin[0] = pWin[1];
		pWin[1] = pWin[2];
		pWin[2] = pTemp;
	}
	{
		*(pOutLine++) = Min4(pWin[0][1], pWin[1][0], pWin[1][1], pWin[1][2]);
	}
}

void CMorphologyFilter::Dilation3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth)
{
	int i, x;
	BYTE Buffer[9];
	BYTE *pIn[3];
	BYTE *pWin[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		pWin[i] = Buffer + i * 3;
	}
	for (i = 0; i < 3; i++)
	{
		pWin[0][i] = 0;
		pWin[1][i] = pIn[i][0];
		pIn[i] += 3;
	}

	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pWin[2][i] = pIn[i][0];
			pIn[i] += 3;
		}

		*(pOutLine++) = (pWin[0][1] | pWin[1][0] | pWin[1][1] | pWin[1][2] | pWin[2][1]);

		BYTE *pTemp = pWin[0];
		pWin[0] = pWin[1];
		pWin[1] = pWin[2];
		pWin[2] = pTemp;
	}
	{
		*(pOutLine++) = (pWin[0][1] | pWin[1][0] | pWin[1][1] | pWin[1][2]);
	}
}

void CMorphologyFilter::Max3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth)
{
	int i, x;
	BYTE Buffer[9];
	BYTE *pIn[3];
	BYTE *pWin[3];

	for (i = 0; i < 3; i++)
	{
		pIn[i] = pInLines[i];
		pWin[i] = Buffer + i * 3;
	}
	for (i = 0; i < 3; i++)
	{
		pWin[0][i] = 0;
		pWin[1][i] = pIn[i][0];
		pIn[i] += 3;
	}

	for (x = 0; x < nWidth - 1; x++)
	{
		for (i = 0; i < 3; i++)
		{
			pWin[2][i] = pIn[i][0];
			pIn[i] += 3;
		}

		*(pOutLine++) = Max5(pWin[0][1], pWin[1][0], pWin[1][1], pWin[1][2], pWin[2][1]);

		BYTE *pTemp = pWin[0];
		pWin[0] = pWin[1];
		pWin[1] = pWin[2];
		pWin[2] = pTemp;
	}
	{
		*(pOutLine++) = Max4(pWin[0][1], pWin[1][0], pWin[1][1], pWin[1][2]);
	}
}

bool CMorphologyFilter::Erosion3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 255, nWidth, 3);
	RotateLine(pInLine, pLines[1], nWidth, 3);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pLines[2], nWidth, 3);
		pInLine += nWidth;

		Erosion3x3Line(pLines, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 255, nWidth, 3);
	Erosion3x3Line(pLines, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Min3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 255, nWidth, 3);
	RotateLine(pInLine, pLines[1], nWidth, 3);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pLines[2], nWidth, 3);
		pInLine += nWidth;

		Min3x3Line(pLines, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 255, nWidth, 3);
	Min3x3Line(pLines, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Dilation3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 0, nWidth, 3);
	RotateLine(pInLine, pLines[1], nWidth, 3);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pLines[2], nWidth, 3);
		pInLine += nWidth;

		Dilation3x3Line(pLines, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 0, nWidth, 3);
	Dilation3x3Line(pLines, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::Max3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();
	BYTE *pLines[3];

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pBuffer = new BYTE[nWidth * 3];
	if (pBuffer == NULL)return false;

	pLines[0] = pBuffer;
	pLines[1] = pLines[0] + 1;
	pLines[2] = pLines[1] + 1;

	BYTE *pInLine = pInImage->GetImageData();
	BYTE *pOutLine = pOutImage->GetImageData();

	FillLine(pLines[0], 0, nWidth, 3);
	RotateLine(pInLine, pLines[1], nWidth, 3);
	pInLine += nWidth;

	for (y = 0; y < nHeight - 1; y++)
	{
		RotateLine(pInLine, pLines[2], nWidth, 3);
		pInLine += nWidth;

		Max3x3Line(pLines, pOutLine, nWidth);
		pOutLine += nWidth;

		BYTE *pTemp = pLines[0];
		pLines[0] = pLines[1];
		pLines[1] = pLines[2];
		pLines[2] = pTemp;
	}
	FillLine(pLines[2], 0, nWidth, 3);
	Max3x3Line(pLines, pOutLine, nWidth);

	delete[] pBuffer;
	return true;
}

bool CMorphologyFilter::BinarizeImageGT(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre)
{
	int x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn = pInImage->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			Y = (Y >= nThre) ? (255) : (0);
			*(pOut++) = (BYTE)Y;
		}
	}

	return true;
}

bool CMorphologyFilter::BinarizeImageLT(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre)
{
	int x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn = pInImage->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			Y = (Y <= nThre) ? (255) : (0);
			*(pOut++) = (BYTE)Y;
		}
	}

	return true;
}

bool CMorphologyFilter::BinarizeImageRange(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre1, int nThre2)
{
	int x, y, Y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn = pInImage->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			Y = *(pIn++);
			Y = (Y >= nThre1&&Y <= nThre2) ? (255) : (0);
			*(pOut++) = (BYTE)Y;
		}
	}

	return true;
}

bool CMorphologyFilter::InverseImage(CGrayImage *pInImage, CGrayImage *pOutImage)
{
	int x, y;
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn = pInImage->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			*(pOut++) = ((*(pIn++))^255);
		}
	}

	return true;
}

bool CMorphologyFilter::AND2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage)
{
	int x, y;
	int nWidth = pInImage1->GetWidth();
	int nHeight = pInImage1->GetHeight();

	if (pInImage2->GetWidth() != nWidth || pInImage2->GetHeight() != nHeight)return false;

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn1 = pInImage1->GetImageData();
	BYTE *pIn2 = pInImage2->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			*(pOut++) = ((*(pIn1++)) &(*(pIn2++)));
		}
	}

	return true;
}

bool CMorphologyFilter::OR2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage)
{
	int x, y;
	int nWidth = pInImage1->GetWidth();
	int nHeight = pInImage1->GetHeight();

	if (pInImage2->GetWidth() != nWidth || pInImage2->GetHeight() != nHeight)return false;

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn1 = pInImage1->GetImageData();
	BYTE *pIn2 = pInImage2->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			*(pOut++) = ((*(pIn1++)) |(*(pIn2++)));
		}
	}

	return true;
}

bool CMorphologyFilter::XOR2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage)
{
	int x, y;
	int nWidth = pInImage1->GetWidth();
	int nHeight = pInImage1->GetHeight();

	if (pInImage2->GetWidth() != nWidth || pInImage2->GetHeight() != nHeight)return false;

	if (pOutImage->GetWidth() != nWidth || pOutImage->GetHeight() != nHeight)
	{
		if (!pOutImage->Create(nWidth, nHeight))return false;
	}

	BYTE *pIn1 = pInImage1->GetImageData();
	BYTE *pIn2 = pInImage2->GetImageData();
	BYTE *pOut = pOutImage->GetImageData();

	for (y = 0; y < nHeight; y++)
	{
		for (x = 0; x < nWidth; x++)
		{
			*(pOut++) = ((*(pIn1++)) ^ (*(pIn2++)));
		}
	}

	return true;
}
