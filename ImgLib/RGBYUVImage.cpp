#include "RGBYUVImage.h"

void CRGBYUVImage::RGB2YCbCr(bool bDithering)
{
	int i, x, y, YUV[3], BGR[3], E[3];

	if (bDithering)
	{
		E[0] = E[1] = E[2] = 0;
	}
	else
	{
		E[0] = E[1] = E[2] = 2048;
	}
	for (y = 0; y<m_nHeight; y++)
	{
		BYTE *pLine = GetImageLine(y);
		for (x = 0; x<m_nWidth; x++, pLine += 3)
		{
			for (i = 0; i<3; i++)
			{
				BGR[i] = pLine[i];
			}

			YUV[0] = BGR[0] * 467 + BGR[1] * 2404 + BGR[2] * 1225;	// 0.114B+0.587G+0.299R
			YUV[1] = (BGR[0] * 4096 - YUV[0]) * 2048 / (4096 - 467) + 4096 * 128;
			YUV[2] = (BGR[2] * 4096 - YUV[0]) * 2048 / (4096 - 1225) + 4096 * 128;

			for (i = 0; i<3; i++)
			{
				if (YUV[i]<0)YUV[i] = 0;

				YUV[i] += E[i];
				if(bDithering)E[i] = YUV[i] & 4095;
				YUV[i] >>= 12;

				if (YUV[i]>255)YUV[i] = 255;
				pLine[i] = (BYTE)YUV[i];
			}
		}
	}
}

void CRGBYUVImage::YCbCr2RGB(bool bDithering)
{
	int i, x, y, YUV[3], BGR[3], E[3];

	E[0] = E[1] = E[2] = 0;
	for (y = 0; y<m_nHeight; y++)
	{
		BYTE *pLine = GetImageLine(y);
		for (x = 0; x<m_nWidth; x++, pLine += 3)
		{
			YUV[0] = pLine[0];
			YUV[1] = pLine[1];	YUV[1] -= 128;
			YUV[2] = pLine[2];	YUV[2] -= 128;

			BGR[0] = YUV[0] * 2048 + YUV[1] * (4096 - 467);
			BGR[2] = YUV[0] * 2048 + YUV[2] * (4096 - 1225);
			BGR[1] = YUV[0] * 2048 - YUV[1] * 705 - YUV[2] * 1463;

			for (i = 0; i<3; i++)
			{
				if (BGR[i]<0)BGR[i] = 0;

				BGR[i] += E[i];
				if(bDithering)E[i] = BGR[i] & 2047;
				BGR[i] >>= 11;

				if (BGR[i]>255)BGR[i] = 255;
				pLine[i] = (BYTE)BGR[i];
			}
		}
	}
}

bool CRGBYUVImage::LoadFromBMPFile(char *pFileName)
{
	int y;
	BITMAPFILEHEADER BMPFileHead;
	BITMAPINFOHEADER BMPInfoHead;
	LONG nReadSize;
	FILE *fp = NULL;
	
	errno_t err=fopen_s(&fp, pFileName, "rb");

	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	nReadSize = fread(&BMPFileHead, 1, sizeof(BITMAPFILEHEADER), fp);
	if (nReadSize != sizeof(BITMAPFILEHEADER) || BMPFileHead.bfType != 0x4D42)
	{
		fclose(fp);
		return false;
	}

	nReadSize = fread(&BMPInfoHead, 1, sizeof(BITMAPINFOHEADER), fp);
	if (nReadSize != sizeof(BITMAPINFOHEADER))
	{
		fclose(fp);
		return false;
	}

	if ((BMPInfoHead.biBitCount != 24) || (BMPInfoHead.biSize != sizeof(BITMAPINFOHEADER)))
	{
		fclose(fp);
		return false;
	}

	int nPitch = ((BMPInfoHead.biWidth * 24 + 31) / 32) * 4;

	if (BMPInfoHead.biSizeImage == 0) BMPInfoHead.biSizeImage = nPitch * BMPInfoHead.biHeight;

	LONG nBISize = sizeof(BITMAPINFOHEADER);
	LONG nSize = BMPFileHead.bfSize - BMPFileHead.bfOffBits;

	if (nSize < nPitch * BMPInfoHead.biHeight) nSize = nPitch * BMPInfoHead.biHeight;

	if (!Create(BMPInfoHead.biWidth, BMPInfoHead.biHeight))
	{
		fclose(fp);
		return false;
	}

	fseek(fp, BMPFileHead.bfOffBits, SEEK_SET);

	BYTE *pBuffer = new BYTE[nPitch];

	for (y = 0; y<m_nHeight; y++)
	{
		BYTE *pLine = GetImageLine(m_nHeight - 1 - y);
		if ((nReadSize = fread(pBuffer, 1, nPitch, fp)) != nPitch)
		{
			fclose(fp);
			delete[] pBuffer;
			return false;
		}
		memcpy(pLine, pBuffer, m_nWidth * 3);
	}

	fclose(fp);
	delete[] pBuffer;
	return true;
}

bool CRGBYUVImage::SaveToBMPFile(char *pFileName)
{
	int y;
	BITMAPFILEHEADER BmpFileHdr;
	BITMAPINFO *pInfo;
	FILE* fp = NULL;
	
	errno_t err = fopen_s(&fp, pFileName, "wb");

	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	LONG nBISize = sizeof(BITMAPINFOHEADER);
	pInfo = (BITMAPINFO *)malloc(nBISize);

	pInfo->bmiHeader.biBitCount = 24;
	pInfo->bmiHeader.biWidth = m_nWidth;
	pInfo->bmiHeader.biHeight = m_nHeight;
	pInfo->bmiHeader.biCompression = BI_RGB;
	pInfo->bmiHeader.biSizeImage = 0;
	pInfo->bmiHeader.biPlanes = 1;
	pInfo->bmiHeader.biClrImportant = 0;
	pInfo->bmiHeader.biClrUsed = 0;
	pInfo->bmiHeader.biXPelsPerMeter = 75;
	pInfo->bmiHeader.biYPelsPerMeter = 75;
	pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	int nPitch = ((m_nWidth * 24 + 31) / 32) * 4;
	LONG nSize = nPitch * m_nHeight;

	BmpFileHdr.bfType = 0x4D42;		// 'BM'
	BmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER) + nBISize + nSize;
	BmpFileHdr.bfReserved1 = 0;
	BmpFileHdr.bfReserved2 = 0;
	BmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + nBISize;

	if (fwrite(&BmpFileHdr, 1, sizeof(BITMAPFILEHEADER), fp) != sizeof(BITMAPFILEHEADER))
	{
		fclose(fp);
		free(pInfo);
		return false;
	}

	if (fwrite(pInfo, 1, nBISize, fp) != (DWORD)nBISize)
	{
		fclose(fp);
		free(pInfo);
		return false;
	}
	free(pInfo);

	BYTE *pBuffer = new BYTE[nPitch];
	for (y = 0; y<m_nHeight; y++)
	{
		BYTE *pLine = GetImageLine(m_nHeight - 1 - y);
		memcpy(pBuffer, pLine, m_nWidth * 3 * sizeof(BYTE));

		if (fwrite(pBuffer, 1, nPitch, fp) != nPitch)
		{
			fclose(fp);
			delete[] pBuffer;
			return false;
		}
	}

	delete[] pBuffer;
	fclose(fp);
	return true;
}

bool CRGBYUVImage::LoadYUV444File(char *pFileName, int nWidth, int nHeight)
{
	FILE *fp = NULL;
	if (!Create(nWidth, nHeight))return false;

	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp == NULL)
	{
		printf("Can not load YUV444 image from %s!!! err=%d\n", pFileName, err);
		return false;
	}

	size_t size = (nWidth*nHeight * 3);
	size_t res = fread_s(m_pData, size, 1, size, fp);
	fclose(fp);

	if (res != size)
	{
		printf("%s file size is smaller than %dX%dX3\n", pFileName, nWidth, nHeight);
		return false;
	}

	return true;
}

bool CRGBYUVImage::SaveYUV444File(char *pFileName)
{
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "wb");
	if (fp == NULL)
	{
		printf("Can not Save YUV444 image to %s!!! err=%d\n", pFileName, err);
		return false;
	}

	size_t size = (m_nWidth*m_nHeight * 3);
	size_t res = fwrite(m_pData, 1, size, fp);
	fclose(fp);

	if (res != size)
	{
		printf("%s file save fail due to not enough space!!!\n", pFileName);
		return false;
	}

	return true;
}

bool CRGBYUVImage::GetHistogram(int pHistB[], int pHistG[], int pHistR[])
{
	int x, y, g;

	if (m_pData == NULL)return false;

	for (g = 0; g < 256; g++)
	{
		pHistB[g] = pHistG[g] = pHistR[g] = 0;
	}

	BYTE *pBGR = m_pData;
	for (y = 0; y < m_nHeight; y++)
	{
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pBGR++);
			pHistB[g]++;

			g = *(pBGR++);
			pHistG[g]++;

			g = *(pBGR++);
			pHistR[g]++;
		}
	}

	return true;
}

bool CRGBYUVImage::GetHistogram(int nChannel, int nHist[])
{
	int x, y, g;

	if (m_pData == NULL)return false;

	for (g = 0; g < 256; g++)
	{
		nHist[g] = 0;
	}

	BYTE *pBGR = m_pData + nChannel;
	for (y = 0; y < m_nHeight; y++)
	{
		for (x = 0; x < m_nWidth; x++)
		{
			g = *pBGR;
			nHist[g]++;

			pBGR += 3;
		}
	}

	return true;
}

//CYUV422Image
bool CYUV422Image::YUV444ToYUV422(CRGBYUVImage *pYUV444Image)
{
	int i, x, y, YUV1[3], YUV2[3], YUYV[4];
	int nWidth = pYUV444Image->GetWidth();
	int nHeight = pYUV444Image->GetHeight();

	if ((nWidth & 1) != 0)return false;
	if (!Create(nWidth, nHeight))return false;

	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pYUV = pYUV444Image->GetImageLine(y);
		BYTE *pYUYV = GetImageLine(y);
		for (x = 0; x < m_nWidth; x += 2)
		{
			for (i = 0; i < 3; i++)
			{
				YUV1[i] = *(pYUV++);
			}
			for (i = 0; i < 3; i++)
			{
				YUV2[i] = *(pYUV++);
			}
			YUYV[0] = YUV1[0];
			YUYV[1] = (YUV1[1] + YUV2[1]) >> 1;
			YUYV[2] = YUV2[0];
			YUYV[3] = (YUV1[2] + YUV2[2]) >> 1;

			for (i = 0; i < 4; i++)
			{
				*(pYUYV++) = (BYTE)YUYV[i];
			}
		}
	}

	return true;
}

bool CYUV422Image::YUV422ToYUV444(CRGBYUVImage *pYUV444Image)
{
	int i, x, y, YUV1[3], YUV2[3], YUYV[3][4];

	if (!pYUV444Image->Create(m_nWidth, m_nHeight))return false;

	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pYUYV = GetImageLine(y);
		BYTE *pYUV = pYUV444Image->GetImageLine(y);
		
		for (i = 0; i < 4; i++)
		{
			YUYV[0][i] = YUYV[1][i] = *(pYUYV++);
		}
		for (x = 0; x < m_nWidth - 2; x += 2)
		{
			for (i = 0; i < 4; i++)
			{
				YUYV[2][i] = *(pYUYV++);
			}

			YUV1[0] = YUYV[1][0];
			YUV1[1] = (YUYV[0][1] + YUYV[1][1] * 3) / 4;
			YUV1[2] = (YUYV[0][2] + YUYV[1][2] * 3) / 4;
			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV1[i];
			}

			YUV2[0] = YUYV[1][2];
			YUV2[1] = (YUYV[2][1] + YUYV[1][1] * 3) / 4;
			YUV2[2] = (YUYV[2][2] + YUYV[1][2] * 3) / 4;
			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV2[i];
			}

			for (i = 0; i < 4; i++)
			{
				YUYV[0][i] = YUYV[1][i];
				YUYV[1][i] = YUYV[2][i];
			}
		}
		{
			YUV1[0] = YUYV[1][0];
			YUV1[1] = (YUYV[0][1] + YUYV[1][1] * 3) / 4;
			YUV1[2] = (YUYV[0][2] + YUYV[1][2] * 3) / 4;
			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV1[i];
			}

			YUV2[0] = YUYV[1][2];
			YUV2[1] = (YUYV[2][1] + YUYV[1][1] * 3) / 4;
			YUV2[2] = (YUYV[2][2] + YUYV[1][2] * 3) / 4;
			for (i = 0; i < 3; i++)
			{
				*(pYUV++) = (BYTE)YUV2[i];
			}
		}
	}

	return true;
}

bool CYUV422Image::SaveToYUV422File(char *pFileName, int nYUYVFormat)
{
	int x, y;
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "wb");
	if (fp==NULL)
	{
		printf("Save to YUV422 file %s fail!!!\n", pFileName);
		return false;
	}

	if (nYUYVFormat == 0)
	{
		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pLine = GetImageLine(y);
			fwrite(pLine, 2, m_nWidth, fp);
		}
	}
	else
	{
		BYTE *pBuffer = new BYTE[m_nWidth * 2 + 8];
		if (pBuffer == NULL)
		{
			fclose(fp);
			return false;
		}

		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pYUYV = GetImageLine(y);
			BYTE *pOut = pBuffer;
			for (x = 0; x < m_nWidth; x += 2)
			{
				if (nYUYVFormat == 1)
				{
					pOut[0] = pYUYV[0];
					pOut[1] = pYUYV[3];
					pOut[2] = pYUYV[2];
					pOut[3] = pYUYV[1];
				}
				else if (nYUYVFormat == 2)
				{
					pOut[0] = pYUYV[1];
					pOut[1] = pYUYV[0];
					pOut[2] = pYUYV[3];
					pOut[3] = pYUYV[2];
				}
				else
				{
					pOut[0] = pYUYV[3];
					pOut[1] = pYUYV[0];
					pOut[2] = pYUYV[1];
					pOut[3] = pYUYV[2];
				}

				pOut += 4;
				pYUYV += 4;
			}
		}

		fwrite(pBuffer, 2, m_nWidth, fp);

		delete[] pBuffer;
	}

	fclose(fp);
	return true;
}

bool CYUV422Image::LoadFromYUV422File(char *pFileName, int nWidth, int nHeight, int nYUYVFormat)
{
	int i, x, y;
	BYTE Out[4];

	if (!Create(nWidth, nHeight))return false;

	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp == NULL)
	{
		printf("Load YUV422 file %s fail!!!\n", pFileName);
		return false;
	}

	size_t size = (m_nWidth*m_nHeight * 2);
	size_t res=fread_s(m_pData, size, 1, size, fp);
	fclose(fp);

	if (res < size)
	{
		printf("%s file size is smaller than %dX%dX2!!!\n", pFileName, m_nWidth, m_nHeight);
		return false;
	}

	if (nYUYVFormat == 1)
	{
		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pYUYV = GetImageLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				Out[0] = pYUYV[0];
				Out[3] = pYUYV[1];
				Out[2] = pYUYV[2];
				Out[1] = pYUYV[3];

				for (i = 0; i < 4; i++)
				{
					*(pYUYV++) = Out[i];
				}
			}
		}
	}
	else if (nYUYVFormat == 2)
	{
		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pYUYV = GetImageLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				Out[0] = pYUYV[1];
				Out[3] = pYUYV[0];
				Out[2] = pYUYV[3];
				Out[1] = pYUYV[2];

				for (i = 0; i < 4; i++)
				{
					*(pYUYV++) = Out[i];
				}
			}
		}
	}
	else if (nYUYVFormat == 3)
	{
		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pYUYV = GetImageLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				Out[0] = pYUYV[1];
				Out[3] = pYUYV[2];
				Out[2] = pYUYV[3];
				Out[1] = pYUYV[0];

				for (i = 0; i < 4; i++)
				{
					*(pYUYV++) = Out[i];
				}
			}
		}
	}

	return true;
}

//CYUV420Image
bool CYUV420Image::YUV444ToYUV420(CRGBYUVImage *pYUV444Image)
{
	int i, j, x, y, UV[2];
	int nWidth = pYUV444Image->GetWidth();
	int nHeight = pYUV444Image->GetHeight();

	if (!Create(nWidth, nHeight))return false;

	//Fill Y Plane
	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pYUV = pYUV444Image->GetImageLine(y);
		BYTE *pY = GetYLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			*(pY++) = pYUV[0];
			pYUV += 3;
		}
	}

	for (y = 0; y < m_nHeight; y += 2)
	{
		BYTE *pYUV[2];
		BYTE *pUV = GetUVLine(y);
		pYUV[0] = pYUV444Image->GetImageLine(y);
		pYUV[1] = pYUV444Image->GetImageLine(y + 1);
		for (x = 0; x < m_nWidth; x += 2)
		{
			UV[0] = UV[1] = 0;
			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					UV[0] += pYUV[i][1];
					UV[1] += pYUV[i][2];
					pYUV[i] += 3;
				}
			}

			for (i = 0; i < 2; i++)
			{
				UV[i] = (UV[i] + 2) >> 2;
				if (UV[i] > 255)UV[i] = 255;
				*(pUV++) = (BYTE)UV[i];
			}
		}
	}

	return true;
}

void CYUV420Image::HIntUVLine(BYTE *pInLine, WORD *pOutLine, int nWidth)
{
	int x, U0, V0, U1, V1, UV[2];

	U0 = *(pInLine++);
	V0 = *(pInLine++);

	pOutLine[0] = (WORD)(U0 << 2);
	pOutLine[1] = (WORD)(V0 << 2);
	pOutLine += 2;

	for (x = 1; x<nWidth - 1; x += 2)
	{
		U1 = *(pInLine++);
		V1 = *(pInLine++);

		UV[0] = (U0 * 3 + U1);
		UV[1] = (V0 * 3 + V1);
		pOutLine[0] = (WORD)UV[0];
		pOutLine[1] = (WORD)UV[1];
		pOutLine += 2;

		UV[0] = (U0 + U1 * 3);
		UV[1] = (V0 + V1 * 3);
		pOutLine[0] = (WORD)UV[0];
		pOutLine[1] = (WORD)UV[1];
		pOutLine += 2;

		U0 = U1;
		V0 = V1;
	}

	pOutLine[0] = (WORD)(U0 << 2);
	pOutLine[1] = (WORD)(V0 << 2);
	pOutLine += 2;
}

bool CYUV420Image::YUV420ToYUV444(CRGBYUVImage *pYUV444Image, bool bSimpleUV)
{
	int x, y, UV[2];
	WORD *pUVLines[2];

	if (!pYUV444Image->Create(m_nWidth, m_nHeight))return false;

	//Fill Y 
	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pY = GetYLine(y);
		BYTE *pYUV = pYUV444Image->GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			pYUV[0] = *(pY++);
			pYUV += 3;
		}
	}

	//Fill UV
	if (bSimpleUV)
	{
		for (y = 0; y < m_nHeight; y++)
		{
			BYTE *pUV = GetUVLine(y);
			BYTE *pYUV = pYUV444Image->GetImageLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				pYUV[1] = pYUV[4] = pUV[0];
				pYUV[2] = pYUV[5] = pUV[1];
				pYUV += 6;
				pUV += 2;
			}
		}
	}
	else
	{
		WORD *pUVLineBuffer = new WORD[m_nWidth * 4];
		if (pUVLineBuffer == NULL)return false;

		pUVLines[0] = pUVLineBuffer;
		pUVLines[1] = pUVLineBuffer + m_nWidth * 2;
		BYTE *pUVLine = GetUVLine(0);
		HIntUVLine(pUVLine, pUVLines[0], m_nWidth);

		{
			BYTE *pYUVLine = pYUV444Image->GetImageLine(0);
			WORD *pUV = pUVLines[0];
			for (x = 0; x<m_nWidth; x++)
			{
				pYUVLine[1] = (BYTE)(*(pUV++) >> 2);
				pYUVLine[2] = (BYTE)(*(pUV++) >> 2);
				pYUVLine += 3;
			}
		}
		for (y = 1; y<m_nHeight - 1; y += 2)
		{
			pUVLine = GetUVLine(y + 1);
			HIntUVLine(pUVLine, pUVLines[1], m_nWidth);

			BYTE *pYUVLine = pYUV444Image->GetImageLine(y);
			WORD *pUV0 = pUVLines[0];
			WORD *pUV1 = pUVLines[1];

			for (x = 0; x<m_nWidth; x++)
			{
				UV[0] = *(pUV0++);
				UV[1] = *(pUV1++);
				UV[0] = (UV[0] * 3 + UV[1]) >> 4;
				pYUVLine[1] = (BYTE)UV[0];

				UV[0] = *(pUV0++);
				UV[1] = *(pUV1++);
				UV[0] = (UV[0] * 3 + UV[1]) >> 4;
				pYUVLine[2] = (BYTE)UV[0];

				pYUVLine += 3;
			}

			pYUVLine = pYUV444Image->GetImageLine(y + 1);
			pUV0 = pUVLines[0];
			pUV1 = pUVLines[1];

			for (x = 0; x<m_nWidth; x++)
			{
				UV[0] = *(pUV0++);
				UV[1] = *(pUV1++);
				UV[0] = (UV[0] + UV[1] * 3) >> 4;
				pYUVLine[1] = (BYTE)UV[0];

				UV[0] = *(pUV0++);
				UV[1] = *(pUV1++);
				UV[0] = (UV[0] + UV[1] * 3) >> 4;
				pYUVLine[2] = (BYTE)UV[0];

				pYUVLine += 3;
			}

			WORD *pTemp = pUVLines[0];
			pUVLines[0] = pUVLines[1];
			pUVLines[1] = pTemp;
		}
		{
			BYTE *pYUVLine = pYUV444Image->GetImageLine(m_nHeight - 1);
			WORD *pUV = pUVLines[0];
			for (x = 0; x<m_nWidth; x++)
			{
				pYUVLine[1] = (BYTE)(*(pUV++) >> 2);
				pYUVLine[2] = (BYTE)(*(pUV++) >> 2);
				pYUVLine += 3;
			}
		}

		delete[] pUVLineBuffer;
	}
	return true;
}

bool CYUV420Image::LoadYUV420File(char *pFileName, int nWidth, int nHeight, int bUVOrder, int bInterleaveUV)
{
	if (!Create(nWidth, nHeight))return false;

	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp==NULL)
	{
		printf("Load YUV422 file %s fail!!! err=%d\n", pFileName, err);
		return false;
	}

	size_t size = (m_nWidth*m_nHeight + (m_nWidth / 2)*(m_nHeight / 2) * 2);
	size_t res = fread_s(m_pData, size, 1, size, fp);
	fclose(fp);

	if (bInterleaveUV == 0)
	{
		BYTE *pUVBuf = new BYTE[(m_nWidth / 2)*(m_nHeight / 2) * 2];
		if (pUVBuf == NULL)return false;

		memcpy(pUVBuf, m_pData + m_nWidth*m_nHeight, (m_nWidth / 2)*(m_nHeight / 2) * 2);

		int x, y;

		BYTE *pU = pUVBuf;
		BYTE *pV = pUVBuf + (m_nWidth / 2)*(m_nHeight / 2);
		BYTE *pUV = m_pData + m_nWidth*m_nHeight;
		for (y = 0; y < m_nHeight / 2; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				*(pUV++) = *(pU++);
				*(pUV++) = *(pV++);
			}
		}

		delete[] pUVBuf;
	}

	if (res < size)
	{
		printf("%s file size is smaller than %dX%d+%dX%dX2!!!\n", pFileName, m_nWidth, m_nHeight, m_nWidth/2, m_nHeight/2);
		return false;
	}

	if (bUVOrder == 1)
	{
		int x, y;
		BYTE Temp;

		for (y = 0; y < m_nHeight; y += 2)
		{
			BYTE *pUV = GetUVLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				Temp = pUV[0];
				pUV[0] = pUV[1];
				pUV[1] = Temp;
				pUV += 2;
			}
		}
	}

	return true;
}

bool CYUV420Image::SaveYUV420File(char *pFileName, int bUVOrder, int bInterleaveUV)
{
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, pFileName, "wb");
	if (fp == NULL)
	{
		printf("Save YUV420 file %s fail!!! err=%d\n", pFileName, err);
		return false;
	}

	size_t size = (m_nWidth*m_nHeight + (m_nWidth / 2)*(m_nHeight / 2) * 2);
	size_t res = fwrite(m_pYPlane, 1, m_nWidth*m_nHeight, fp);

	if (bUVOrder == 1)
	{
		int x, y;
		BYTE Temp;
		for (y = 0; y < m_nHeight; y += 2)
		{
			BYTE *pUV = GetUVLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				Temp = pUV[0];
				pUV[0] = pUV[1];
				pUV[1] = Temp;
				pUV += 2;
			}
		}
	}

	if (bInterleaveUV == 0)
	{
		BYTE *pUVBuf = new BYTE[(m_nWidth / 2)*(m_nHeight / 2) * 2];
		if (pUVBuf == NULL)return false;

		int x, y;

		BYTE *pU = pUVBuf;
		BYTE *pV = pUVBuf + (m_nWidth / 2)*(m_nHeight / 2);
		BYTE *pUV = m_pUVPlane;
		for (y = 0; y < m_nHeight / 2; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				*(pU++) = *(pUV++);
				*(pV++) = *(pUV++);
			}
		}

		res += fwrite(pUVBuf, 1, (m_nWidth / 2)*(m_nHeight / 2) * 2, fp);

		delete[] pUVBuf;
	}
	else
	{
		res += fwrite(m_pUVPlane, 1, (m_nWidth / 2)*(m_nHeight / 2) * 2, fp);
	}

	fclose(fp);

	if(bUVOrder==1)
	{
		int x, y;

		for (y = 0; y < m_nHeight; y += 2)
		{
			BYTE *pUV = GetUVLine(y);
			for (x = 0; x < m_nWidth; x += 2)
			{
				BYTE Temp = pUV[0];
				pUV[0] = pUV[1];
				pUV[1] = Temp;
				pUV += 2;
			}
		}
	}

	if (res != size)
	{
		printf("%s file save fail due to not enough space!!!\n", pFileName);
		return false;
	}

	return true;
}
