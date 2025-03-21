#include "WaveletPyramidYUV420DNSFilter.h"

void CYDNSFilter::BuildNoiseLUT(int nNoise, int nCurve[], int nLUT[])
{
	int Y, iY, dY, nGain;

	for (Y = 0; Y < 256; Y++)
	{
		iY = Y >> 5;
		dY = Y & 31;
		nGain = nCurve[iY];
		nGain += ((nCurve[iY + 1] - nCurve[iY])*dY + 16) / 32;
		nLUT[Y] = nNoise*nGain;
		nLUT[Y] = (nLUT[Y] * nLUT[Y] + 1024) >> 11;
	}
}

void CWavelePyramidYUV420DNSFilter::fWT8x8(short *pBlock)
{
	int i, j;
	short coef[64];

	//8x8 level 0
	short *pData = pBlock;
	short *pCoef = coef;
	for (i = 0; i<8; i++)
	{
		for (j = 0; j<4; j++)
		{
			pCoef[0] = pData[0] + pData[1];
			pCoef[4] = pData[0] - pData[1];
			pData += 2;
			pCoef += 1;
		}
		pCoef += 4;
	}
	for (i = 0; i<8; i++)
	{
		pData = coef + i;
		pCoef = pBlock + i;
		for (j = 0; j<4; j++)
		{
			pCoef[0] = (pData[0] + pData[8]) / 2;
			pCoef[32] = (pData[0] - pData[8]) / 2;
			pData += 16;
			pCoef += 8;
		}
	}
	//4x4 level 1
	pData = pBlock;
	pCoef = coef;
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<2; j++)
		{
			pCoef[0] = pData[0] + pData[1];
			pCoef[2] = pData[0] - pData[1];
			pData += 2;
			pCoef += 1;
		}
		pData += 4;
		pCoef += 6;
	}
	for (i = 0; i<4; i++)
	{
		pData = coef + i;
		pCoef = pBlock + i;
		for (j = 0; j<2; j++)
		{
			pCoef[0] = (pData[0] + pData[8]) / 2;
			pCoef[16] = (pData[0] - pData[8]) / 2;
			pData += 16;
			pCoef += 8;
		}
	}
	//2x2 level 2
	pData = pBlock;
	pCoef = coef;
	for (i = 0; i<2; i++)
	{
		pCoef[0] = pData[0] + pData[1];
		pCoef[1] = pData[0] - pData[1];
		pData += 8;
		pCoef += 8;
	}
	for (i = 0; i<2; i++)
	{
		pData = coef + i;
		pCoef = pBlock + i;
		pCoef[0] = (pData[0] + pData[8]) / 2;
		pCoef[8] = (pData[0] - pData[8]) / 2;
	}
}

void CWavelePyramidYUV420DNSFilter::iWT8x8_Level2(short *pBlock)
{
	int i;
	short data[64];
	short *pData, *pCoef;

	//2x2 level 2
	for (i = 0; i<2; i++)
	{
		pCoef = pBlock + i;
		pData = data + i;
		pData[0] = pCoef[0] + pCoef[8];
		pData[8] = pCoef[0] - pCoef[8];
	}
	pCoef = data;
	pData = pBlock;
	for (i = 0; i<2; i++)
	{
		pData[0] = (pCoef[0] + pCoef[1] + 1) >> 1;
		pData[1] = (pCoef[0] - pCoef[1] + 1) >> 1;
		if (pData[0] < 0)pData[0] = 0;	if (pData[0] > 2047)pData[0] = 2047;
		if (pData[1] < 0)pData[1] = 0;	if (pData[1] > 2047)pData[1] = 2047;
		pData += 8;
		pCoef += 8;
	}
}

void CWavelePyramidYUV420DNSFilter::iWT8x8_Level1(short *pBlock)
{
	int i, j;
	short data[64];
	short *pData, *pCoef;

	//4x4 level 1
	for (i = 0; i<4; i++)
	{
		pCoef = pBlock + i;
		pData = data + i;
		for (j = 0; j<2; j++)
		{
			pData[0] = pCoef[0] + pCoef[16];
			pData[8] = pCoef[0] - pCoef[16];
			pData += 16;
			pCoef += 8;
		}
	}
	pCoef = data;
	pData = pBlock;
	for (i = 0; i<4; i++)
	{
		for (j = 0; j<2; j++)
		{
			pData[0] = (pCoef[0] + pCoef[2] + 1) >> 1;
			pData[1] = (pCoef[0] - pCoef[2] + 1) >> 1;
			if (pData[0] < 0)pData[0] = 0;	if (pData[0] > 1023)pData[0] = 1023;
			if (pData[1] < 0)pData[1] = 0;	if (pData[1] > 1023)pData[1] = 1023;
			pData += 2;
			pCoef += 1;
		}
		pData += 4;
		pCoef += 6;
	}
}

void CWavelePyramidYUV420DNSFilter::iWT8x8_Level0(short *pBlock)
{
	int i, j;
	short data[64];
	short *pData, *pCoef;

	//8x8 level0
	for (i = 0; i<8; i++)
	{
		pCoef = pBlock + i;
		pData = data + i;
		for (j = 0; j<4; j++)
		{
			pData[0] = pCoef[0] + pCoef[32];
			pData[8] = pCoef[0] - pCoef[32];
			pData += 16;
			pCoef += 8;
		}
	}
	pCoef = data;
	pData = pBlock;
	for (i = 0; i<8; i++)
	{
		for (j = 0; j<4; j++)
		{
			pData[0] = (pCoef[0] + pCoef[4] + 1) >> 1;
			pData[1] = (pCoef[0] - pCoef[4] + 1) >> 1;
			if (pData[0] < 0)pData[0] = 0;	if (pData[0] > 511)pData[0] = 511;
			if (pData[1] < 0)pData[1] = 0;	if (pData[1] > 511)pData[1] = 511;
			pData += 2;
			pCoef += 1;
		}
		pCoef += 4;
	}
}

void CWavelePyramidYUV420DNSFilter::FWTBlockLine(short *pBlockLine, int nBlockWidth, int nDim)
{
	int i, x;

	short *pBlock = pBlockLine;
	for (x = 0; x<nBlockWidth; x++)
	{
		for (i = 0; i<nDim; i++)
		{
			fWT8x8(pBlock);
			pBlock += 64;
		}
	}
}

void CWavelePyramidYUV420DNSFilter::YUVH2BlockLine(short *pYUVHLine, short *pBlockLine, int nImageWidth, int nInPitch, int nDim)
{
	int i, j, x;

	for (x = i = 0; x<nImageWidth; x++, pYUVHLine += nInPitch)
	{
		for (j = 0; j<nDim; j++)
		{
			pBlockLine[i + 64 * j] = pYUVHLine[j];
		}
		i++;
		if (i == 8)
		{
			pBlockLine += 64 * nDim;
			i = 0;
		}
	}

	if (i>0)
	{
		for (; i<8; i++)
		{
			for (j = 0; j<nDim; j++)
			{
				pBlockLine[i + 64 * j] = pBlockLine[i + 64 * j - 1];
			}
		}
	}
}

void CWavelePyramidYUV420DNSFilter::Block2YUVHLine(short *pBlockLine, short *pYUVHLine, int nImageWidth, int nOutPitch, int nDim)
{
	int i, j, x;

	for (x = i = 0; x<nImageWidth; x++, pYUVHLine += nOutPitch)
	{
		for (j = 0; j<nDim; j++)
		{
			pYUVHLine[j] = pBlockLine[i + 64 * j];
		}

		i++;
		if (i == 8)
		{
			pBlockLine += nDim * 64;
			i = 0;
		}
	}
}

bool CWavelePyramidYUV420DNSFilter::FWTYUVHImage(CMultipleChannelImage *pYUVHImage, CMultipleChannelImage *pBlockImage)
{
	int i, y;
	int nImageWidth = pYUVHImage->GetWidth();
	int nImageHeight = pYUVHImage->GetHeight();
	int nDim = pYUVHImage->GetDim();
	int nBlockWidth = ((nImageWidth & 7) == 0) ? (nImageWidth>>3) : ((nImageWidth>>3) + 1);
	int nBlockHeight = ((nImageHeight & 7) == 0) ? (nImageHeight>>3) : ((nImageHeight>>3) + 1);

	if (!pBlockImage->Create((nBlockWidth<<3), (nBlockHeight<<3), 3, 13))return false;
	pBlockImage->m_nMAXS = pYUVHImage->m_nMAXS;
	pBlockImage->m_nBLK = pYUVHImage->m_nBLK;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = i = 0; y<nImageHeight; y++)
	{
		short *pYUVHLine = pYUVHImage->GetImageLine(y);
		YUVH2BlockLine(pYUVHLine, pBlockLine + i * 8, nImageWidth, nDim);

		i++;
		if (i == 8)
		{
			FWTBlockLine(pBlockLine, nBlockWidth);
			i = 0;
			pBlockLine += 3 * 64 * nBlockWidth;
		}
	}
	if (i>0)
	{
		for (; i<8; i++)
		{
			short *pYUVHLine = pYUVHImage->GetImageLine(nImageHeight - 1);
			YUVH2BlockLine(pYUVHLine, pBlockLine + i * 8, nImageWidth, nDim);
		}
		FWTBlockLine(pBlockLine, nBlockWidth);
	}

	return true;
}

void CWavelePyramidYUV420DNSFilter::IWTBlockLine_Level2(short *pBlockLine, int nBlockWidth, int nDim)
{
	int i, x;

	short *pBlock = pBlockLine;
	for (x = 0; x<nBlockWidth; x++)
	{
		for (i = 0; i<nDim; i++)
		{
			iWT8x8_Level2(pBlock);
			pBlock += 64;
		}
	}
}

void CWavelePyramidYUV420DNSFilter::IWTBlockImage_Level2(CMultipleChannelImage *pBlockImage)
{
	int y;
	int nBlockWidth = pBlockImage->GetWidth()>>3;
	int nBlockHeight = pBlockImage->GetHeight()>>3;
	int nDim = pBlockImage->GetDim();

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nBlockHeight; y++)
	{
		IWTBlockLine_Level2(pBlockLine, nBlockWidth, nDim);
		pBlockLine += nDim * 64 * nBlockWidth;
	}
}

void CWavelePyramidYUV420DNSFilter::IWTBlockLine_Level1(short *pBlockLine, int nBlockWidth, int nDim)
{
	int i, x;

	short *pBlock = pBlockLine;
	for (x = 0; x<nBlockWidth; x++)
	{
		for (i = 0; i<nDim; i++)
		{
			iWT8x8_Level1(pBlock);
			pBlock += 64;
		}
	}
}

void CWavelePyramidYUV420DNSFilter::IWTBlockImage_Level1(CMultipleChannelImage *pBlockImage)
{
	int y;
	int nBlockWidth = pBlockImage->GetWidth() >> 3;
	int nBlockHeight = pBlockImage->GetHeight() >> 3;
	int nDim = pBlockImage->GetDim();

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nBlockHeight; y++)
	{
		IWTBlockLine_Level1(pBlockLine, nBlockWidth, nDim);
		pBlockLine += nDim * 64 * nBlockWidth;
	}
}

void CWavelePyramidYUV420DNSFilter::IWTBlockLine_Level0(short *pBlockLine, int nBlockWidth, int nDim)
{
	int i, x;

	short *pBlock = pBlockLine;
	for (x = 0; x<nBlockWidth; x++)
	{
		for (i = 0; i<nDim; i++)
		{
			iWT8x8_Level0(pBlock);
			pBlock += 64;
		}
	}
}

void CWavelePyramidYUV420DNSFilter::IWTBlockImage_Level0(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVHImage)
{
	int i, j, y;
	int nBlockWidth = pBlockImage->GetWidth() >> 3;
	int nBlockHeight = pBlockImage->GetHeight() >> 3;
	int nDim = pBlockImage->GetDim();
	int nImageWidth = pYUVHImage->GetWidth();
	int nImageHeight = pYUVHImage->GetHeight();
	int nOutPitch = pYUVHImage->GetDim();

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = i = 0; i<nBlockHeight; i++)
	{
		IWTBlockLine_Level0(pBlockLine, nBlockWidth, nDim);
		for (j = 0; j<8 && y<nImageHeight; y++, j++)
		{
			short *pYUVHLine = pYUVHImage->GetImageLine(y);
			Block2YUVHLine(pBlockLine + j * 8, pYUVHLine, nImageWidth, nOutPitch, nDim);
		}
		pBlockLine += nDim * 64 * nBlockWidth;
	}
}

bool CWavelePyramidYUV420DNSFilter::ExtractYUVImage_Level3(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 8;
	int nHeight1 = nHeight / 8;

	if (!pYUVImage->Create(nWidth1, nHeight1, nDim, 13))return false;
	pYUVImage->m_nMAXS = pBlockImage->m_nMAXS * 8;
	pYUVImage->m_nBLK = pBlockImage->m_nBLK * 8;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y++)
	{
		short *pYUVLine = pYUVImage->GetImageLine(y);
		for (x = 0; x<nWidth1; x++)
		{
			for (i = 0; i<nDim; i++)
			{
				*(pYUVLine++) = pBlockLine[64 * i];
			}
			pBlockLine += 64 * nDim;
		}
	}

	return true;
}

void CWavelePyramidYUV420DNSFilter::ReplaceYUVImage_Level3(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 8;
	int nHeight1 = nHeight / 8;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y++)
	{
		short *pYUVLine = pYUVImage->GetImageLine(y);
		for (x = 0; x<nWidth1; x++)
		{
			for (i = 0; i<nDim; i++)
			{
				pBlockLine[64 * i] = *(pYUVLine++);
			}
			pBlockLine += 64 * nDim;
		}
	}
}

bool CWavelePyramidYUV420DNSFilter::ExtractYUVImage_Level2(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, j, k, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 4;
	int nHeight1 = nHeight / 4;

	if (!pYUVImage->Create(nWidth1, nHeight1, nDim, 12))return false;
	pYUVImage->m_nMAXS = pBlockImage->m_nMAXS * 4;
	pYUVImage->m_nBLK = pBlockImage->m_nBLK * 4;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y += 2)
	{
		for (i = 0; i<2; i++)
		{
			short *pYUVLine = pYUVImage->GetImageLine(y + i);
			short *pBlock = pBlockLine;
			for (x = 0; x<nWidth1; x += 2)
			{
				for (j = 0; j<2; j++)
				{
					for (k = 0; k<nDim; k++)
					{
						*(pYUVLine++) = pBlock[64 * k + 8 * i + j];
					}
				}
				pBlock += 64 * nDim;
			}
		}
		pBlockLine += 8 * nDim*nWidth;
	}

	return true;
}

void CWavelePyramidYUV420DNSFilter::ReplaceYUVImage_Level2(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, j, k, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 4;
	int nHeight1 = nHeight / 4;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y += 2)
	{
		for (i = 0; i<2; i++)
		{
			short *pYUVLine = pYUVImage->GetImageLine(y + i);
			short *pBlock = pBlockLine;
			for (x = 0; x<nWidth1; x += 2)
			{
				for (j = 0; j<2; j++)
				{
					for (k = 0; k<nDim; k++)
					{
						pBlock[64 * k + 8 * i + j] = *(pYUVLine++);
					}
				}
				pBlock += 64 * nDim;
			}
		}
		pBlockLine += 8 * nDim*nWidth;
	}
}

bool CWavelePyramidYUV420DNSFilter::ExtractYUVImage_Level1(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, j, k, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 2;
	int nHeight1 = nHeight / 2;

	if (!pYUVImage->Create(nWidth1, nHeight1, nDim, 11))return false;
	pYUVImage->m_nMAXS = pBlockImage->m_nMAXS * 2;
	pYUVImage->m_nBLK = pBlockImage->m_nBLK * 2;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y += 4)
	{
		for (i = 0; i<4; i++)
		{
			short *pYUVLine = pYUVImage->GetImageLine(y + i);
			short *pBlock = pBlockLine;
			for (x = 0; x<nWidth1; x += 4)
			{
				for (j = 0; j<4; j++)
				{
					for (k = 0; k<nDim; k++)
					{
						*(pYUVLine++) = pBlock[64 * k + 8 * i + j];
					}
				}
				pBlock += 64 * nDim;
			}
		}
		pBlockLine += 8 * nDim*nWidth;
	}

	return true;
}

void CWavelePyramidYUV420DNSFilter::ReplaceYUVImage_Level1(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage)
{
	int i, j, k, x, y;
	int nWidth = pBlockImage->GetWidth();
	int nHeight = pBlockImage->GetHeight();
	int nDim = pBlockImage->GetDim();
	int nWidth1 = nWidth / 2;
	int nHeight1 = nHeight / 2;

	short *pBlockLine = pBlockImage->GetImageData();
	for (y = 0; y<nHeight1; y += 4)
	{
		for (i = 0; i<4; i++)
		{
			short *pYUVLine = pYUVImage->GetImageLine(y + i);
			short *pBlock = pBlockLine;
			for (x = 0; x<nWidth1; x += 4)
			{
				for (j = 0; j<4; j++)
				{
					for (k = 0; k<nDim; k++)
					{
						pBlock[64 * k + 8 * i + j] = *(pYUVLine++);
					}
				}
				pBlock += 64 * nDim;
			}
		}
		pBlockLine += 8 * nDim*nWidth;
	}
}

bool CWavelePyramidYUV420DNSFilter::YUV420ToYUVHImage(CYUV420Image *pInImage, CMultipleChannelImage *pOutImage)
{
	int i, j, x, y, Y[4], YUVH[6];
	int nWidth = pInImage->GetWidth() >> 1;
	int nHeight = pInImage->GetHeight() >> 1;

	if (!pOutImage->Create(nWidth, nHeight, 6, 13))return false;
	pOutImage->m_nMAXS = 511;
	pOutImage->m_nBLK = 0;

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pInYLines[2];

		pInYLines[0] = pInImage->GetYLine(y * 2);
		pInYLines[1] = pInImage->GetYLine(y * 2 + 1);
		BYTE *pInUVLine = pInImage->GetUVLine(y * 2);

		short *pOutLine = pOutImage->GetImageLine(y);
		for (x = 0; x < nWidth; x++)
		{
			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					Y[i * 2 + j] = *(pInYLines[i]++);
				}
			}
			
			YUVH[1] = (*(pInUVLine++)) * 2;
			YUVH[2] = (*(pInUVLine++)) * 2;

			YUVH[0] = (Y[0] + Y[1] + Y[2] + Y[3]) / 2;
			YUVH[3] = (Y[0] + Y[1] - Y[2] - Y[3]) / 2;
			YUVH[4] = (Y[0] - Y[1] + Y[2] - Y[3]) / 2;
			YUVH[5] = (Y[0] - Y[1] - Y[2] + Y[3]) / 2;

			for (i = 0; i < 6; i++)
			{
				*(pOutLine++) = (short)YUVH[i];
			}
		}
	}
	
	return true;
}

void CWavelePyramidYUV420DNSFilter::YUVHToYUV420Image(CMultipleChannelImage *pInImage, CYUV420Image *pOutImage)
{
	int i, j, x, y, Y[4], YUVH[6];
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	for (y = 0; y < nHeight; y++)
	{
		BYTE *pOutYLines[2];

		pOutYLines[0] = pOutImage->GetYLine(y * 2);
		pOutYLines[1] = pOutImage->GetYLine(y * 2 + 1);

		BYTE *pOutUVLine = pOutImage->GetUVLine(y * 2);
		short *pInLine = pInImage->GetImageLine(y);

		for (x = 0; x < nWidth; x++)
		{
			for (i = 0; i < 6; i++)
			{
				YUVH[i] = *(pInLine++);
			}

			Y[0] = (YUVH[0] + YUVH[3] + YUVH[4] + YUVH[5] + 1) >> 1;
			Y[1] = (YUVH[0] + YUVH[3] - YUVH[4] - YUVH[5] + 1) >> 1;
			Y[2] = (YUVH[0] - YUVH[3] + YUVH[4] - YUVH[5] + 1) >> 1;
			Y[3] = (YUVH[0] - YUVH[3] - YUVH[4] + YUVH[5] + 1) >> 1;

			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					if (Y[i * 2 + j] < 0)Y[i * 2 + j] = 0;	if (Y[i * 2 + j] > 255)Y[i * 2 + j] = 255;
					
					*(pOutYLines[i]++) = (BYTE)Y[i * 2 + j];
				}
			}
			YUVH[1] = (YUVH[1] + 1) >> 1;
			if (YUVH[1] < 0)YUVH[1] = 0;	if (YUVH[1] > 255)YUVH[1] = 255;
			*(pOutUVLine++) = (BYTE)YUVH[1];

			YUVH[2] = (YUVH[2] + 1) >> 1;
			if (YUVH[2] < 0)YUVH[2] = 0;	if (YUVH[2] > 255)YUVH[2] = 255;
			*(pOutUVLine++) = (BYTE)YUVH[2];
		}
	}
}

bool CWavelePyramidYUV420DNSFilter::DNSLevel3(CMultipleChannelImage *pBlockImage)
{
	CMultipleChannelImage YUVImage;
	int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[4] + 8) >> 4;
	int nDNSUVYThre = (m_nDNSUVYThre * m_nYThreLevel[3] + 8) >> 4;
	int nDNSUVNoiseUV = (m_nNoiseUV * m_nNoiseUVLevel[3] + 8) >> 4;

	if (!ExtractYUVImage_Level3(pBlockImage, &YUVImage))return false;
	if (m_nAddBackNoiseYLevel[4] < 16)
	{
		if (!DNSYImage(&YUVImage, nDNSYNoise, m_nAddBackNoiseYLevel[4], m_nNoiseYCurve, 4))return false;
	}
	if (!DNSUVImage(&YUVImage, nDNSUVYThre, nDNSUVNoiseUV, m_nDNSUVWinW[3], 4))return false;
	ReplaceYUVImage_Level3(pBlockImage, &YUVImage);

	return true;
}

bool CWavelePyramidYUV420DNSFilter::DNSLevel2(CMultipleChannelImage *pBlockImage)
{
	CMultipleChannelImage YUVImage;
	int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[3] + 8) >> 4;
	int nDNSUVYThre = (m_nDNSUVYThre * m_nYThreLevel[2] + 8) >> 4;
	int nDNSUVNoiseUV = (m_nNoiseUV * m_nNoiseUVLevel[2] + 8) >> 4;

	if (!ExtractYUVImage_Level2(pBlockImage, &YUVImage))return false;
	if (m_nAddBackNoiseYLevel[3] < 16)
	{
		if (!DNSYImage(&YUVImage, nDNSYNoise, m_nAddBackNoiseYLevel[3], m_nNoiseYCurve, 3))return false;
	}
	if (!DNSUVImage(&YUVImage, nDNSUVYThre, nDNSUVNoiseUV, m_nDNSUVWinW[2], 3))return false;
	ReplaceYUVImage_Level2(pBlockImage, &YUVImage);

	return true;
}

bool CWavelePyramidYUV420DNSFilter::DNSLevel1(CMultipleChannelImage *pBlockImage)
{
	CMultipleChannelImage YUVImage;
	int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[2] + 8) >> 4;
	int nDNSUVYThre = (m_nDNSUVYThre * m_nYThreLevel[1] + 8) >> 4;
	int nDNSUVNoiseUV = (m_nNoiseUV * m_nNoiseUVLevel[1] + 8) >> 4;

	if (!ExtractYUVImage_Level1(pBlockImage, &YUVImage))return false;
	if (m_nAddBackNoiseYLevel[2] < 16)
	{
		if (!DNSYImage(&YUVImage, nDNSYNoise, m_nAddBackNoiseYLevel[2], m_nNoiseYCurve, 2))return false;
	}
	if (!DNSUVImage(&YUVImage, nDNSUVYThre, nDNSUVNoiseUV, m_nDNSUVWinW[1], 2))return false;
	ReplaceYUVImage_Level1(pBlockImage, &YUVImage);

	return true;
}

bool CWavelePyramidYUV420DNSFilter::DNSLevel0(CMultipleChannelImage *pYUVHImage)
{
	int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[1] + 8) >> 4;
	int nDNSUVYThre = (m_nDNSUVYThre * m_nYThreLevel[0] + 8) >> 4;
	int nDNSUVNoiseUV = (m_nNoiseUV * m_nNoiseUVLevel[0] + 8) >> 4;

	if (m_nAddBackNoiseYLevel[1] < 16)
	{
		if (!DNSYImage(pYUVHImage, nDNSYNoise, m_nAddBackNoiseYLevel[1], m_nNoiseYCurve, 1))return false;
	}
	if (!DNSUVImage(pYUVHImage, nDNSUVYThre, nDNSUVNoiseUV, m_nDNSUVWinW[0], 1))return false;

	return true;
}
