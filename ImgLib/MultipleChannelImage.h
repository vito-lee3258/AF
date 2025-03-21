#ifndef __MULTIPLE_CHANNEL_IMAGE_H_
#define __MULTIPLE_CHANNEL_IMAGE_H_

#include "BasicImageArray.h"
#include "GrayImage.h"
#include "RGBYUVImage.h"

class CMultipleChannelImage : public CBasicImageArray_SHORT
{
protected:
	void RotateSLine(short *pInLine, short *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch);

	void HFilterLine(short *pInLine, short *pOutLine, int nMask[], int nWinW, int nScale, int nWidth, int nInPitch, int nOutPitch);
	void VFilterLine(short *pInLines[], short *pOutLine, int nMask[], int nWinW, int nScale, int nWidth, int nInPitch, int nOutPitch);

	__inline int Mid(int In[], int Num)
	{
		int i, j, C[81];

		for (i = 0; i < Num; i++)
		{
			C[i] = In[i];
		}

		for (i = 0; i <= Num / 2; i++)
		{
			for (j = i + 1; j < Num; j++)
			{
				if (C[i] > C[j])
				{
					int t = C[i];
					C[i] = C[j];
					C[j] = t;
				}
			}
		}

		return C[Num / 2];
	}

	void HMidFilterLine(short *pInLine, short *pOutLine, int nWinW, int nWidth, int nInPitch, int nOutPitch);
	void VMidFilterLine(short *pInLines[], short *pOutLine, int nWinW, int nWidth, int nInPitch, int nOutPitch);

public:
	int m_nMAXS; //max signal range
	int m_nBLK;	//black level
	int m_nBit;	//bit range;

	bool Create(int nWidth, int nHeight, int nDim, int nBit)
	{
		m_nBit = (nBit > 15) ? (15) : (nBit);
		m_nMAXS = (1 << m_nBit) - 1;
		m_nBLK = 0;

		return SetImageSize(nWidth, nHeight, nDim);
	}
	void CopyParameter(CMultipleChannelImage *pImage)
	{
		m_nMAXS = pImage->m_nMAXS;
		m_nBLK = pImage->m_nBLK;
		m_nBit = pImage->m_nBit;
	}
	bool Copy(CMultipleChannelImage *pImage)
	{
		if (!SetImageSize(pImage->GetWidth(), pImage->GetHeight(), pImage->GetDim()))return false;
		CopyParameter(pImage);

		memcpy(m_pData, pImage->GetImageData(), sizeof(short)*m_nWidth*m_nHeight*m_nDim);

		return true;
	}
	bool CopyRect(CMultipleChannelImage *pImage, int nLeft, int nTop, int nRight, int nBottom)
	{
		int y;
		int nDim = pImage->GetDim();

		if (!SetImageSize(nRight - nLeft, nBottom - nTop, nDim))return false;
		CopyParameter(pImage);

		for (y = nTop; y < nBottom; y++)
		{
			short *pIn = pImage->GetImageLine(y) + nLeft * nDim;
			short *pOut = GetImageLine(y - nTop);
			memcpy(pOut, pIn, m_nWidth * nDim * sizeof(short));
		}

		return true;
	}

	bool RenderSingleChannel(CGrayImage *pImage, int nChannel, int nScale, int nOffset);
	
	bool DumpSingleChannel2BMPFile(char *pFileName, int nChannel, int nScale, int nOffset)
	{
		CGrayImage Image;
		if (!RenderSingleChannel(&Image, nChannel, nScale, nOffset))return false;

		return Image.SaveToBMPFile(pFileName);
	}
	
	bool RenderBGRH(CRGBYUVImage *pImage, int nB, int nG, int nR, int nH, int nScale, int nOffset);
	bool DumpBGRH2BMPFile(char *pFileName, int nB, int nG, int nR, int nH, int nScale, int nOffset)
	{
		CRGBYUVImage Image;
		if (!RenderBGRH(&Image, nB, nG, nR, nH, nScale, nOffset))return false;

		return Image.SaveToBMPFile(pFileName);
	}

	bool BGRH2BGRImage(CRGBYUVImage *pImage);

	bool HFilterChannel(int nChannel, int nMask[], int nWinW, int nScale);
	bool VFilterChannel(int nChannel, int nMask[], int nWinW, int nScale);
	bool HVFilterChannel(int nChannel, int nHMask[], int nHWinW, int nHScale, int nVMask[], int nVWinW, int nVScale);

	bool HMidFilterChannel(int nChannel, int nWinW=1);
	bool VMidFilterChannel(int nChannel, int nWinW=1);
};

#endif

