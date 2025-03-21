#ifndef __RGBYUV_IMAGE_H_
#define __RGBYUV_IMAGE_H_

#include "BasicImageArray.h"

class CRGBYUVImage : public CBasicImageArray_BYTE
{
public:
	bool Create(int nWidth, int nHeight)
	{
		return SetImageSize(nWidth, nHeight, 3);
	}
	bool Copy(CRGBYUVImage *pImage)
	{
		if (!SetImageSize(pImage->GetWidth(), pImage->GetHeight(), 3))return false;
		memcpy(GetImageData(), pImage->GetImageData(), m_nWidth * m_nHeight * 3);
		return true;
	}
	bool CopyRect(CRGBYUVImage *pImage, int nLeft, int nTop, int nRight, int nBottom)
	{
		int y;
		
		if (!SetImageSize(nRight - nLeft, nBottom - nTop, 3))return false;

		for (y = nTop; y < nBottom; y++)
		{
			BYTE *pIn = pImage->GetImageLine(y) + nLeft * 3;
			BYTE *pOut = GetImageLine(y - nTop);
			memcpy(pOut, pIn, m_nWidth * 3);
		}

		return true;
	}

	void RGB2YCbCr(bool bDithering=false);
	void YCbCr2RGB(bool bDithering=false);
	bool LoadFromBMPFile(char *pFileName);
	bool SaveToBMPFile(char *pFileName);

	bool LoadYUV444File(char *pFileName, int nWidth, int nHeight);
	bool SaveYUV444File(char *pFileName);

	bool GetHistogram(int pHistB[], int pHistG[], int pHistR[]);
	bool GetHistogram(int nChannel, int pHist[]);
};

class CYUV422Image : public CBasicImageArray_BYTE
{
public:
	bool Create(int nWidth, int nHeight)
	{
		nWidth = (nWidth >> 1) << 1;
		return SetImageSize(nWidth, nHeight, 2);
	}

	bool YUV444ToYUV422(CRGBYUVImage *pYUV444Image);
	bool YUV422ToYUV444(CRGBYUVImage *pYUV444Image);

	/*
		nYUYVFormat = 0, YUYV
		nYUYVFormat = 1, YVYU
		nYUYVFormat = 2, UYVY
		nYUYVFormat = 3, VYUY
	*/
	bool SaveToYUV422File(char *pFileName, int nYUYVFormat=0);
	bool LoadFromYUV422File(char *pFileName, int nWidth, int nHeight, int nYUYVFormat=0);
};

class CYUV420Image
{
protected:
	BYTE *m_pData;
	BYTE *m_pYPlane;
	BYTE *m_pUVPlane;
	int m_nWidth;
	int m_nHeight;

	void HIntUVLine(BYTE *pInLine, WORD *pOutLine, int nWidth);
public:
	CYUV420Image()
	{
		m_pData = m_pYPlane = m_pUVPlane = NULL;
		m_nWidth = m_nHeight = 0;
	}
	~CYUV420Image()
	{
		if (m_pData != NULL)delete[] m_pData;
	}

	bool Copy(CYUV420Image *pImage)
	{
		if (!Create(pImage->GetWidth(), pImage->GetHeight()))return false;

		memcpy(m_pYPlane, pImage->GetYImage(), m_nWidth*m_nHeight);
		memcpy(m_pUVPlane, pImage->GetUVImage(), (m_nWidth / 2)*(m_nHeight / 2) * 2);

		return true;
	}
	bool CopyRect(CYUV420Image *pImage, int nLeft, int nTop, int nRight, int nBottom)
	{
		int y;

		nLeft = (nLeft >> 1) << 1;
		nTop = (nTop >> 1) << 1;
		nRight = ((nRight >> 1) + (nRight & 1)) << 1;
		nBottom = ((nBottom >> 1) + (nBottom & 1)) << 1;
		if (!Create(nRight - nLeft, nBottom - nTop))return false;

		for (y = nTop; y < nBottom; y++)
		{
			BYTE *pInY = pImage->GetYLine(y) + nLeft;
			BYTE *pOutY = GetYLine(y - nTop);
			memcpy(pOutY, pInY, m_nWidth);
		}
		for (y = nTop; y < nBottom; y += 2)
		{
			BYTE *pInUV = pImage->GetUVLine(y) + nLeft;
			BYTE *pOutUV = GetUVLine(y - nTop);
			memcpy(pOutUV, pInUV, m_nWidth);
		}

		return true;
	}
	bool ReplaceRect(CYUV420Image *pImage, int &nLeft, int &nTop, int &nRight, int &nBottom)
	{
		int y;

		nLeft = (nLeft >> 1) << 1;
		nTop = (nTop >> 1) << 1;
		nRight = ((nRight >> 1) + (nRight & 1)) << 1;
		nBottom = ((nBottom >> 1) + (nBottom & 1)) << 1;

		if (nLeft >= m_nWidth || nRight > m_nWidth || nTop >= m_nHeight || nBottom >= m_nHeight)return false;

		for (y = nTop; y < nBottom; y++)
		{
			BYTE *pInY = pImage->GetYLine(y - nTop);
			BYTE *pOutY = GetYLine(y) + nLeft;
			memcpy(pOutY, pInY, nRight-nLeft);
		}
		for (y = nTop; y < nBottom; y += 2)
		{
			BYTE *pInUV = pImage->GetUVLine(y - nTop);
			BYTE *pOutUV = GetUVLine(y) + nLeft;
			memcpy(pOutUV, pInUV, nRight-nLeft);
		}

		return true;
	}

	bool Create(int nWidth, int nHeight)
	{
		if (m_pData != NULL)delete[] m_pData;
		m_pData = m_pYPlane = m_pUVPlane = NULL;
		m_nWidth = m_nHeight = 0;

		nWidth = (nWidth >> 1) << 1;
		nHeight = (nHeight >> 1) << 1;
		m_pData = new BYTE[nWidth*nHeight + (nWidth / 2)*(nHeight / 2) * 2];
		if (m_pData == NULL)return false;

		m_pYPlane = m_pData;
		m_pUVPlane = m_pData + nWidth*nHeight;
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		return true;
	}

	__inline int GetWidth() { return m_nWidth; }
	__inline int GetHeight() { return m_nHeight; }
	__inline BYTE *GetYImage() { return m_pYPlane; }
	__inline BYTE *GetUVImage() { return m_pUVPlane; }

	__inline BYTE *GetYLine(int nY)
	{
		if (nY<0)nY = 0;	if (nY >= m_nHeight)nY = m_nHeight - 1;
		return m_pYPlane + nY*m_nWidth;
	}
	__inline BYTE *GetUVLine(int nY)
	{
		if (nY<0)nY = 0;	if (nY >= m_nHeight)nY = m_nHeight - 1;
		return m_pUVPlane + (nY >> 1)*m_nWidth;
	}

	bool Crop(int nLeft_Margin, int nTop_Margin, int nRight_Margin, int nBottom_Margin)
	{
		if (m_pData == NULL)return false;

		if (nLeft_Margin<0)nLeft_Margin = 0;		if (nLeft_Margin >= m_nWidth)nLeft_Margin = m_nWidth - 1;
		if (nRight_Margin<0)nRight_Margin = 0;		if (nRight_Margin >= m_nWidth)nRight_Margin = m_nWidth - 1;
		if (nTop_Margin<0)nTop_Margin = 0;			if (nTop_Margin >= m_nHeight)nTop_Margin = m_nHeight - 1;
		if (nBottom_Margin<0)nBottom_Margin = 0;	if (nBottom_Margin >= m_nHeight)nBottom_Margin = m_nHeight - 1;

		int x, y;
		int nLeft = (nLeft_Margin>>1)<<1;
		int nTop = (nTop_Margin>>1)<<1;	
		int nW = m_nWidth - nLeft_Margin - nRight_Margin;
		int nH = m_nHeight - nTop_Margin - nBottom_Margin;

		nW = (nW >> 1) << 1;
		nH = (nH >> 1) << 1;
		if ((nW<2) || (nH<2))return false;

		if (nW == m_nWidth&&nH == m_nHeight)return true;

		BYTE *pOut = m_pYPlane;
		for (y = 0; y < nH; y++)
		{
			BYTE *pIn = GetYLine(y + nTop) + nLeft;
			for (x = 0; x < nW; x++)
			{
				*(pOut++) = *(pIn++);
			}
		}
		pOut = m_pYPlane + nW*nH;
		for (y = 0; y < nH; y += 2)
		{
			BYTE *pIn = GetUVLine(y + nTop) + nLeft;
			for (x = 0; x < nW; x += 2)
			{
				*(pOut++) = *(pIn++);
				*(pOut++) = *(pIn++);
			}
		}
		m_pUVPlane = m_pYPlane + nW*nH;
		m_nWidth = nW;
		m_nHeight = nH;

		return true;
	}

	bool YUV444ToYUV420(CRGBYUVImage *pYUV444Image);
	bool YUV420ToYUV444(CRGBYUVImage *pYUV444Image, bool bSimpleUV=false);

	bool LoadYUV420File(char *pFileName, int nWidth, int nHeight, int bUVOrder = 0, int bInterleaveUV=1);
	bool SaveYUV420File(char *pFileName, int bUVOrder=0, int bInterleaveUV=1);
};

#endif
