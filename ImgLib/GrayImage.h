#ifndef __GRAY_IMAGE_H_
#define __GRAY_IMAGE_H_

#include "ByteImageArray.h"

class CGrayImage : public CByteImageArray
{
public:
	bool Create(int nWidth, int nHeight)
	{
		return SetImageSize(nWidth, nHeight, 1);
	}
	bool Copy(CGrayImage *pImage)
	{
		if (!SetImageSize(pImage->GetWidth(), pImage->GetHeight(), 1))return false;
		memcpy(m_pData, pImage->GetImageData(), m_nWidth*m_nHeight);

		return true;
	}
	bool CopyRect(CGrayImage *pImage, int nLeft, int nTop, int nRight, int nBottom)
	{
		int y;

		if (!SetImageSize(nRight - nLeft, nBottom - nTop, 1))return false;

		for (y = nTop; y < nBottom; y++)
		{
			BYTE *pIn = pImage->GetImageLine(y) + nLeft;
			BYTE *pOut = GetImageLine(y - nTop);
			memcpy(pOut, pIn, m_nWidth);
		}

		return true;
	}

	bool SaveToBMPFile(char *pFileName);

	bool HVFilterImage(int nHMask[], int nHWinW, int nHScale, int nVMask[], int nVWinW, int nVScale, int nOffset = 0)
	{
		return HVFilterChannel(0, nHMask, nHWinW, nHScale, nVMask, nVWinW, nVScale, nOffset);
	}

	bool GetHistogram(int nHist[]);
	bool GetHistogram(int nHist[], int nLeft, int nTop, int nRight, int nBottom);
};

#endif

