#include "RGBYUVImage.h"
#include "BayerRAWImage.h"

bool CBayerRAWImage::SaveToBMPFile(char *pFileName, bool bColor)
{
	int x, y;
	bool bShift = (m_nMAXS == ((1 << m_nBit) - 1));
	int nShift = (m_nBit >= 8) ? (m_nBit - 8) : (0);
	CRGBYUVImage RGBImage;

	if (!RGBImage.Create(m_nWidth, m_nHeight))return false;

	if (!bColor)
	{
		int g;
		for (y = 0; y < m_nHeight; y++)
		{
			WORD *pCFA = GetImageLine(y);
			BYTE *pBGR = RGBImage.GetImageLine(y);
			for (x = 0; x < m_nWidth; x++)
			{
				g = *(pCFA++);
				g = (bShift)?(g>>nShift):((g * 255 + m_nMAXS / 2) / m_nMAXS);
				pBGR[0] = (BYTE)g;
				pBGR[1] = (BYTE)g;
				pBGR[2] = (BYTE)g;
				pBGR += 3;
			}
		}
	}
	else
	{
		int i, j, nCFA[4], B, R, G[2];
		bool bGFlag = (((m_nCFAPattern >> 1) ^ (m_nCFAPattern & 1)) == 1);
		
		for (y = 0; y < m_nHeight; y+=2)
		{
			WORD *pCFA[2];
			BYTE *pBGR[2];
			for (i = 0; i < 2; i++)
			{
				pCFA[i] = GetImageLine(y + i);
				pBGR[i] = RGBImage.GetImageLine(y + i);
			}
			for (x = 0; x < m_nWidth; x+=2)
			{
				for (i = 0; i < 2; i++)
				{
					for (j = 0; j < 2; j++)
					{
						nCFA[i * 2 + j] = pCFA[i][j];
						nCFA[i * 2 + j] = (bShift) ? (nCFA[i * 2 + j] >> nShift) : ((nCFA[i * 2 + j] * 255 + m_nMAXS / 2) / m_nMAXS);
					}
					pCFA[i] += 2;
				}
				B = nCFA[m_nCFAPattern];
				R = nCFA[(m_nCFAPattern ^ 3)];
				G[0] = (bGFlag) ? (nCFA[0]) : (nCFA[1]);
				G[1] = (bGFlag) ? (nCFA[3]) : (nCFA[2]);

				for (i = 0; i < 2; i++)
				{
					for (j = 0; j < 2; j++)
					{
						pBGR[i][0] = (BYTE) B;
						pBGR[i][1] = (BYTE) G[i];
						pBGR[i][2] = (BYTE) R;
						pBGR[i] += 3;
					}
				}
			}
		}
	}

	return RGBImage.SaveToBMPFile(pFileName);
}

bool CBayerRAWImage::ExtractBayerRAWFromBGRImage(CRGBYUVImage *pBGRImage, int nCFAPattern, int nBit)
{
	int i, j, x, y, g;
	int nCFAArray[2][2];
	int nShift = (nBit >= 8) ? (nBit - 8) : (0);
	int nMask = (1 << nShift) - 1;

	if (!Create(pBGRImage->GetWidth(), pBGRImage->GetHeight(), nBit))return false;
	SetCFAPattern(nCFAPattern);

	for (i = 0, y = (m_nCFAPattern >> 1) & 1; i < 2; i++, y ^= 1)
	{
		for (j = 0, x = (m_nCFAPattern & 1); j < 2; j++, x ^= 1)
		{
			nCFAArray[i][j] = y + x;
		}
	}

	for (y = 0; y < m_nHeight; y++)
	{
		BYTE *pBGR = pBGRImage->GetImageLine(y);
		WORD *pCFA = GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			i = nCFAArray[y & 1][x & 1];
			g = pBGR[i];
			pBGR += 3;

			g = (g << nShift) | nMask;
			*(pCFA++) = (WORD)g;
		}
	}

	return true;
}

bool CBayerRAWImage::LoadFromBMPFile(char *pFileName, int nCFAPattern, int nBit)
{
	CRGBYUVImage BGRImage;

	if (!BGRImage.LoadFromBMPFile(pFileName))return false;

	return ExtractBayerRAWFromBGRImage(&BGRImage, nCFAPattern, nBit);
}

typedef struct tagRAWFileInfo
{
	WORD nWidth;
	WORD nHeight;
	WORD nMAXS;
	WORD nBLK;
	BYTE nCFAPattern;
	BYTE nBit;
	BYTE bMSBAlgn;
	BYTE bByteOrder;
	BYTE nXORMaskL;
	BYTE nXORMaskH;
	char sFileType[4];
}TRAWFileInfo;

bool CBayerRAWImage::CheckRAWFileInfo(char *pFileName, int &nFileSize, bool &bRAW8File, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bMSBAlign, bool &bByteOrder)
{
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	TRAWFileInfo tInfo;
	long offset = sizeof(tInfo);
	fseek(fp, -offset, SEEK_END);
	long filesize = ftell(fp);
	fread(&tInfo, sizeof(tInfo), 1, fp);
	long imagesize = ((long)tInfo.nWidth)*tInfo.nHeight;
	nFileSize = filesize + sizeof(tInfo);
	fclose(fp);

	if (strcmp(tInfo.sFileType, "R16") == 0 && filesize == imagesize * 2)
	{
		bRAW8File = false;
		nWidth = tInfo.nWidth;
		nHeight = tInfo.nHeight;
		nBit = tInfo.nBit;
		nBLK = tInfo.nBLK;
		nCFAPattern = tInfo.nCFAPattern;
		bMSBAlign = (tInfo.bMSBAlgn == 1);
		bByteOrder = (tInfo.bByteOrder == 1);

		return true;
	}
	else if (strcmp(tInfo.sFileType, "R08") == 0 && filesize == imagesize)
	{
		bRAW8File = true;
		nWidth = tInfo.nWidth;
		nHeight = tInfo.nHeight;
		nCFAPattern = tInfo.nCFAPattern;
		nBit = 10;
		nBLK = (tInfo.nBLK << 2) | 3;

		return true;
	}

	printf("%s does not contain raw file info!!!\n", pFileName);
	return false;
}

bool CBayerRAWImage::SaveToRAW16File(char *pFileName, bool bMSBAlign, bool bByteOrder, bool bWithInfo)
{
	int x, y;
	int nShift = (16 - m_nBit);
	WORD g, nMask;
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "wb");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	WORD *pBuffer = new WORD[m_nWidth];
	if (pBuffer == NULL)
	{
		fclose(fp);
		return false;
	}

	nMask = 0;
	for (y = 0; y < m_nHeight; y++)
	{
		WORD *pIn = GetImageLine(y);
		WORD *pOut = pBuffer;
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pIn++);
			nMask ^= g;
			if (bMSBAlign) g <<= nShift;
			if (!bByteOrder)
			{
				g = ((g >> 8) & 255) | ((g & 255) << 8);
			}
			*(pOut++) = g;
		}
		fwrite(pBuffer, sizeof(WORD), m_nWidth, fp);
	}

	if (bWithInfo)
	{
		TRAWFileInfo tInfo;
		tInfo.nWidth = (WORD)m_nWidth;
		tInfo.nHeight = (WORD)m_nHeight;
		tInfo.nMAXS = (WORD)m_nMAXS;
		tInfo.nBLK = (WORD)m_nBLK;
		tInfo.nBit = (BYTE)m_nBit;
		tInfo.nCFAPattern = (BYTE)m_nCFAPattern;
		tInfo.nXORMaskL = (nMask & 255);
		tInfo.nXORMaskH = ((nMask >> 8) & 255);
		tInfo.bByteOrder = (bByteOrder) ? (1) : (0);
		tInfo.bMSBAlgn = (bMSBAlign) ? (1) : (0);
		tInfo.sFileType[0] = 'R';
		tInfo.sFileType[1] = '1';
		tInfo.sFileType[2] = '6';
		tInfo.sFileType[3] = 0;
		fwrite(&tInfo, sizeof(tInfo), 1, fp);
	}

	delete[] pBuffer;
	fclose(fp);
	return true;
}

bool CBayerRAWImage::LoadFromRAW16File(char *pFileName, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bMSBAlign, bool &bByteOrder, bool &bWithInfo)
{
	int x, y;
	WORD g;
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	TRAWFileInfo tInfo;
	long offset = sizeof(tInfo);
	fseek(fp, -offset, SEEK_END);
	long filesize = ftell(fp);
	fread(&tInfo, sizeof(tInfo), 1, fp);
	long imagesize = ((long)tInfo.nWidth)*tInfo.nHeight;

	if (strcmp(tInfo.sFileType, "R16") == 0 && filesize == imagesize * 2)
	{
		printf("RAW file %s contain file info\n", pFileName);
		bWithInfo = true;
		nWidth = tInfo.nWidth;
		nHeight = tInfo.nHeight;
		nBit = tInfo.nBit;
		nCFAPattern = tInfo.nCFAPattern;
		nBLK = tInfo.nBLK;
		bMSBAlign = (tInfo.bMSBAlgn == 1);
		bByteOrder = (tInfo.bByteOrder == 1);
	}
	else
	{
		bWithInfo = false;
	}

	if (!Create(nWidth, nHeight, nBit))
	{
		fclose(fp);
		return false;
	}
	SetCFAPattern(nCFAPattern);

	if (bWithInfo)
	{
		m_nMAXS = tInfo.nMAXS;
	}
	m_nBLK = nBLK;

	fseek(fp, 0, SEEK_SET);
	fread(m_pData, sizeof(WORD), m_nWidth*m_nHeight, fp);
	fclose(fp);

	int nShift = (16 - m_nBit);
	WORD nMask = 0;
	for (y = 0; y < m_nHeight; y++)
	{
		WORD *pLine = GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			g = *pLine;

			if (!bByteOrder)
			{
				g = ((g >> 8) & 255) | ((g & 255) << 8);
			}
			if (bMSBAlign)g >>= nShift;
			nMask ^= g;

			*(pLine++) = g;
		}
	}

	if (bWithInfo && (tInfo.nXORMaskL != (nMask & 255) || tInfo.nXORMaskH != ((nMask >> 8) & 255)))
	{
		printf("XOR Mask is wrong %02X%02X vs. %04X\n", tInfo.nXORMaskH, tInfo.nXORMaskL, nMask);
		return false;
	}
	else
		return true;
}

bool CBayerRAWImage::SaveToRAW8File(char *pFileName, bool bWithInfo)
{
	int x, y, g;
	int nShift = (m_nBit >= 8) ? (m_nBit - 8) : (0);
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "wb");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	BYTE *pBuffer = new BYTE[m_nWidth];
	if (pBuffer == NULL)
	{
		fclose(fp);
		return false;
	}

	BYTE nMask = 0;
	for (y = 0; y < m_nHeight; y++)
	{
		WORD *pIn = GetImageLine(y);
		BYTE *pOut = pBuffer;
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pIn++);
			g >>= nShift;
			if (g > 255)g = 255;
			nMask ^= (BYTE)g;
			*(pOut++) = (BYTE)g;
		}
		fwrite(pBuffer, 1, m_nWidth, fp);
	}

	if (bWithInfo)
	{
		TRAWFileInfo tInfo;

		tInfo.nWidth = (WORD)m_nWidth;
		tInfo.nHeight = (WORD)m_nHeight;
		tInfo.nCFAPattern = (BYTE)m_nCFAPattern;
		tInfo.nBLK = (BYTE)(m_nBLK >> nShift);
		tInfo.nMAXS = (BYTE)(m_nMAXS >> nShift);
		tInfo.nXORMaskL = nMask;
		tInfo.nXORMaskH = 0;
		tInfo.sFileType[0] = 'R';
		tInfo.sFileType[1] = '0';
		tInfo.sFileType[2] = '8';
		tInfo.sFileType[3] = 0;

		fwrite(&tInfo, sizeof(tInfo), 1, fp);
	}

	fclose(fp);
	delete[] pBuffer;
	return true;
}

bool CBayerRAWImage::LoadFromRAW8File(char *pFileName, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bWithInfo)
{
	int x, y, g;
	int nShift = (nBit >= 8) ? (nBit - 8) : (0);
	int M = (1 << nShift) - 1;
	int nMAXS = (1 << nBit) - 1;
	BYTE nXorMask = 0;
	FILE *fp = NULL;

	errno_t err = fopen_s(&fp, pFileName, "rb");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	TRAWFileInfo tInfo;
	long offset = sizeof(tInfo);
	fseek(fp, -offset, SEEK_END);
	long filesize = ftell(fp);
	fread(&tInfo, sizeof(tInfo), 1, fp);
	long imagesize = ((long)tInfo.nWidth)*tInfo.nHeight;

	if (strcmp(tInfo.sFileType, "R08") == 0 && filesize == imagesize)
	{
		printf("RAW file %s contain file info\n", pFileName);
		bWithInfo = true;
		nWidth = tInfo.nWidth;
		nHeight = tInfo.nHeight;
		nCFAPattern = tInfo.nCFAPattern;
		nMAXS = (((int)tInfo.nMAXS) << nShift) | M;
		nBLK = (((int)tInfo.nBLK) << nShift) | M;
	}
	else
	{
		bWithInfo = false;
	}

	if (!Create(nWidth, nHeight, nBit))
	{
		fclose(fp);
		return false;
	}
	SetCFAPattern(nCFAPattern);
	m_nMAXS = nMAXS;
	m_nBLK = nBLK;

	BYTE *pBuffer = new BYTE[nWidth*nHeight];
	if (pBuffer == NULL)
	{
		fclose(fp);
		return false;
	}

	fseek(fp, 0, SEEK_SET);
	fread(pBuffer, 1, nWidth*nHeight, fp);
	fclose(fp);

	BYTE *pIn = pBuffer;
	for (y = 0; y < m_nHeight; y++)
	{
		WORD *pOut = GetImageLine(y);
		for (x = 0; x < m_nWidth; x++)
		{
			g = *(pIn++);
			nXorMask ^= (BYTE)g;
			g = (g << nShift) | M;
			*(pOut++) = (WORD)g;
		}
	}

	delete[] pBuffer;

	if (bWithInfo && (tInfo.nXORMaskL != nXorMask))
	{
		printf("XOR Mask is wrong: %04X vs. %04X\n", tInfo.nXORMaskL, nXorMask);
		return false;
	}
	else
		return true;
}

bool CBayerRAWImage::Bayer2BGGRImage(CMultipleChannelImage *pBGGRImage)
{
	int i, j, x, y, BGGR[4];
	int nCFAArray[2][2];

	if (!pBGGRImage->Create((m_nWidth >> 1), (m_nHeight >> 1), 4, m_nBit))return false;
	pBGGRImage->m_nMAXS = m_nMAXS;
	pBGGRImage->m_nBLK = m_nBLK;

	for (i = 0, y = (m_nCFAPattern & 2); i < 2; i++, y ^= 2)
	{
		for (j = 0, x = (m_nCFAPattern & 1) | y; j < 2; j++, x ^= 1)
		{
			nCFAArray[i][j] = x;
		}
	}

	for (y = 0; y < m_nHeight; y += 2)
	{
		WORD *pIn[2];
		pIn[0] = GetImageLine(y);
		pIn[1] = GetImageLine(y + 1);
		short *pOut = pBGGRImage->GetImageLine(y >> 1);
		for (x = 0; x < m_nWidth; x += 2)
		{
			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					BGGR[nCFAArray[i][j]] = pIn[i][j];
				}
				pIn[i] += 2;
			}

			for (i = 0; i < 4; i++)
			{
				*(pOut++) = (short)BGGR[i];
			}
		}
	}

	return true;
}

bool CBayerRAWImage::BGGR2BayerImage(CMultipleChannelImage *pBGGRImage, int nCFAPattern)
{
	int i, j, x, y, BGGR[4];
	int nCFAArray[2][2];
	int nWidth = pBGGRImage->GetWidth() * 2;
	int nHeight = pBGGRImage->GetHeight() * 2;

	if (!Create(nWidth, nHeight, pBGGRImage->m_nBit))return false;
	m_nMAXS = pBGGRImage->m_nMAXS;
	m_nBLK = pBGGRImage->m_nBLK;
	SetCFAPattern(nCFAPattern);

	for (i = 0, y = (m_nCFAPattern & 2); i < 2; i++, y ^= 2)
	{
		for (j = 0, x = (m_nCFAPattern & 1) | y; j < 2; j++, x ^= 1)
		{
			nCFAArray[i][j] = x;
		}
	}

	for (y = 0; y < m_nHeight; y += 2)
	{
		WORD *pOut[2];
		pOut[0] = GetImageLine(y);
		pOut[1] = GetImageLine(y + 1);
		short *pIn = pBGGRImage->GetImageLine(y >> 1);
		for (x = 0; x < m_nWidth; x += 2)
		{
			for (i = 0; i < 4; i++)
			{
				BGGR[i] = *(pIn++);
				if (BGGR[i] < 0)BGGR[i] = 0;
				if (BGGR[i] > m_nMAXS)BGGR[i] = m_nMAXS;
			}

			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					pOut[i][j]=(WORD)BGGR[nCFAArray[i][j]];
				}
				pOut[i] += 2;
			}
		}
	}

	return true;
}

bool CBayerRAWImage::Bayer2BGRHImage(CMultipleChannelImage *pBGRHImage)
{
	int i, j, x, y, BGGR[4], BGRH[4];
	int nCFAArray[2][2];

	if (!pBGRHImage->Create((m_nWidth >> 1), (m_nHeight >> 1), 4, m_nBit))return false;
	pBGRHImage->m_nMAXS = m_nMAXS;
	pBGRHImage->m_nBLK = m_nBLK;

	for (i = 0, y = (m_nCFAPattern & 2); i < 2; i++, y ^= 2)
	{
		for (j = 0, x = (m_nCFAPattern & 1) | y; j < 2; j++, x ^= 1)
		{
			nCFAArray[i][j] = x;
		}
	}

	for (y = 0; y < m_nHeight; y += 2)
	{
		WORD *pIn[2];
		pIn[0] = GetImageLine(y);
		pIn[1] = GetImageLine(y + 1);
		short *pOut = pBGRHImage->GetImageLine(y >> 1);
		for (x = 0; x < m_nWidth; x += 2)
		{
			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					BGGR[nCFAArray[i][j]] = pIn[i][j];
				}
				pIn[i] += 2;
			}

			BGRH[0] = BGGR[0];
			BGRH[1] = (BGGR[1] + BGGR[2]) / 2;
			BGRH[2] = BGGR[3];
			BGRH[3] = (BGGR[1] - BGGR[2]) / 2;
			
			for (i = 0; i < 4; i++)
			{
				*(pOut++) = (short)BGRH[i];
			}
		}
	}

	return true;
}

bool CBayerRAWImage::BGRH2BayerImage(CMultipleChannelImage *pBGRHImage, int nCFAPattern)
{
	int i, j, x, y, BGGR[4], BGRH[4];
	int nCFAArray[2][2];
	int nWidth = pBGRHImage->GetWidth() * 2;
	int nHeight = pBGRHImage->GetHeight() * 2;

	if (!Create(nWidth, nHeight, pBGRHImage->m_nBit))return false;
	m_nMAXS = pBGRHImage->m_nMAXS;
	m_nBLK = pBGRHImage->m_nBLK;
	SetCFAPattern(nCFAPattern);

	for (i = 0, y = (m_nCFAPattern & 2); i < 2; i++, y ^= 2)
	{
		for (j = 0, x = (m_nCFAPattern & 1) | y; j < 2; j++, x ^= 1)
		{
			nCFAArray[i][j] = x;
		}
	}

	for (y = 0; y < m_nHeight; y += 2)
	{
		WORD *pOut[2];
		pOut[0] = GetImageLine(y);
		pOut[1] = GetImageLine(y + 1);
		short *pIn = pBGRHImage->GetImageLine(y >> 1);
		for (x = 0; x < m_nWidth; x += 2)
		{
			for (i = 0; i < 4; i++)
			{
				BGRH[i] = *(pIn++);
			}

			BGGR[0] = BGRH[0];
			BGGR[1] = BGRH[1] + BGRH[3];
			BGGR[2] = BGRH[1] - BGRH[3];
			BGGR[3] = BGRH[2];

			for (i = 0; i < 4; i++)
			{
				if (BGGR[i] < 0)BGGR[i] = 0;
				if (BGGR[i] > m_nMAXS)BGGR[i] = m_nMAXS;
			}

			for (i = 0; i < 2; i++)
			{
				for (j = 0; j < 2; j++)
				{
					pOut[i][j] = (WORD)BGGR[nCFAArray[i][j]];
				}
				pOut[i] += 2;
			}
		}
	}

	return true;
}

bool CBayerRAWImage::Normalize(int nNewBit)
{
	int x, y, g;

	if (m_nMAXS != ((1 << m_nBit) - 1))return false;

	if (nNewBit < 8)nNewBit = 8;
	if (nNewBit > 16)nNewBit = 16;

	if (m_nBit < nNewBit)
	{
		int nShift = nNewBit - m_nBit;
		int nMask = (1 << nShift) - 1;

		for (y = 0; y < m_nHeight; y++)
		{
			WORD *pLine = GetImageLine(y);
			for (x = 0; x < m_nWidth; x++)
			{
				g = *pLine;
				g = (g << nShift) | nMask;
				*(pLine++) = (WORD)g;
			}
		}
		m_nMAXS = (m_nMAXS << nShift) | nMask;
		m_nBLK = (m_nBLK << nShift) | nMask;
		m_nBit = nNewBit;
	}
	else if (m_nBit > nNewBit)
	{
		int nShift = (m_nBit - nNewBit);
		int nMask = (1 << nShift) - 1;
		int E[2][2];

		m_nMAXS = (m_nMAXS + (nMask >> 1) + 1) >> nShift;
		m_nBLK = (m_nBLK + (nMask >> 1) + 1) >> nShift;
		m_nBit = nNewBit;

		E[0][0] = E[0][1] = E[1][0] = E[1][1] = 0;
		for (y = 0; y < m_nHeight; y++)
		{
			WORD *pLine = GetImageLine(y);
			for (x = 0; x < m_nWidth; x++)
			{
				g = *pLine;
				
				g += E[(y&1)][(x&1)];
				E[(y & 1)][(x & 1)] = (g&nMask);
				g >>= nShift;
				if (g > m_nMAXS)g = m_nMAXS;

				*(pLine++) = (WORD)g;
			}
		}
	}

	return true;
}