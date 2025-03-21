#ifndef __NLM_YHDNS_FILTER_BASIC_H_
#define __NLM_YHDNS_FILTER_BASIC_H_

#include "WaveletPyramidYUV420DNSFilter.h"

class CNLM_YDNSFilter_Basic : public CYDNSFilter
{
protected:
	void RotateSLine(short *pInLine, short *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch);
	void RotateYLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nOutPitch = 7);

	int m_nYWinBuf[49];
	int *m_nYWin[7];
	int DNSYWindow();
	void DNSYLine(short *pInLines[], short *pOutLine, int nWidth, int nInPitch, int nOutPitch);
	bool DNSYImage_Line(CMultipleChannelImage *pYUVImage);

	void DNSYLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch = 7);
	bool DNSYImage_Line(CYUV420Image *pYUVImage);

	void HSumLine(WORD *pInLine, unsigned __int32 *pOutLine, int nWidth, int nInPitch);
	void VSumLine(unsigned __int32 *pInLines[], WORD *pOutLine, int nWidth, int nOutPitch);
	bool Avg3x3Image(CBasicImageArray_WORD *pInImage, int nChannel=0);
	bool MeanVarYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage);
	bool MeanVarYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage);

	void DiffLine(BYTE *pSource, BYTE *pTarget, WORD *pDiff, int nOffsetX, int nWidth, int nOutPitch);
	void DiffLine(short *pSource, short *pTarget, WORD *pDiff, int nOffsetX, int nWidth, int nInPitch, int nOutPitch);
	bool DiffYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage, int nOffsetX, int nOffsetY);
	bool DiffYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage, int nOffsetX, int nOffsetY);

	bool InitDNSYImage(CYUV420Image *pYUVImage, CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	bool InitDNSYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	void AccumulateSumYW(CBasicImageArray_WORD *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	bool DNSYImage_New(CYUV420Image *pYUVImage);
	bool DNSYImage_New(CMultipleChannelImage *pYUVImage);

	bool DNSYImage(CMultipleChannelImage *pYUVImage)
	{
		if (m_bUseLineBuffer)
		{
			return DNSYImage_Line(pYUVImage);
		}
		else
		{
			return DNSYImage_New(pYUVImage);
		}
	}
	bool DNSYImage(CYUV420Image *pYUVImage)
	{
		if (m_bUseLineBuffer)
		{
			return DNSYImage_Line(pYUVImage);
		}
		else
		{
			return DNSYImage_New(pYUVImage);
		}
	}

	virtual bool InitParamList()
	{
		return true;
	}
public:
	bool m_bUseLineBuffer;
	CNLM_YDNSFilter_Basic()
	{
		m_bUseLineBuffer = false;
		Initialize("CNLM_YHDNSFilter_Basic");
	}

	virtual bool ProcessImage(CMultipleChannelImage *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[], int nShift)
	{
		m_nMAXS = pYUVImage->m_nMAXS;
		m_nShift = nShift;
		m_nAddBackNoiseY = nAddBack;
		BuildNoiseLUT(nNoiseY, nNoiseYCurve, m_nNoiseYLUT);

		return DNSYImage(pYUVImage);
	}
	virtual bool ProcessImage(CYUV420Image *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[])
	{
		m_nMAXS = 255;
		m_nShift = 0;
		m_nAddBackNoiseY = nAddBack;
		BuildNoiseLUT(nNoiseY, nNoiseYCurve, m_nNoiseYLUT);

		return DNSYImage(pYUVImage);
	}
};

class CNLM_YDNSFilter_ABS : public CYDNSFilter
{
protected:
	int m_nMask;

	virtual void BuildNoiseLUT(int nNoise, int nCurve[], int nLUT[]);

	void HSumLine(BYTE *pInLine, WORD *pOutLine, int nWidth, int nInPitch);
	void VSumLine(WORD *pInLines[], BYTE *pOutLine, int nWidth, int nOutPitch);
	bool Avg3x3Image(CBasicImageArray_BYTE *pInImage, int nChannel = 0);
	bool MeanVarYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pDiffImage);
	bool MeanVarYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pDiffImage);

	void DiffLine(BYTE *pSource, BYTE *pTarget, BYTE *pDiff, int nOffsetX, int nWidth, int nOutPitch);
	void DiffLine(short *pSource, short *pTarget, BYTE *pDiff, int nOffsetX, int nWidth, int nInPitch, int nOutPitch);
	bool DiffYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pDiffImage, int nOffsetX, int nOffsetY);
	bool DiffYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pDiffImage, int nOffsetX, int nOffsetY);

	bool InitDNSYImage(CYUV420Image *pYUVImage, CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	bool InitDNSYImage(CMultipleChannelImage *pYUVImage, CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	void AccumulateSumYW3(CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	void AccumulateSumYW4(CBasicImageArray_BYTE *pDiffImage, CBasicImageArray_UINT32 *pSumYWImage);
	bool DNSYImage(CYUV420Image *pYUVImage);
	bool DNSYImage(CMultipleChannelImage *pYUVImage);

	virtual bool InitParamList()
	{
		return true;
	}
public:
	CNLM_YDNSFilter_ABS()
	{
		Initialize("CNLM_YDNSFilter_ABS");
	}
	virtual bool ProcessImage(CMultipleChannelImage *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[], int nShift)
	{
		m_nMAXS = pYUVImage->m_nMAXS;
		m_nShift = nShift;
		m_nMask = (1 << m_nShift) - 1;
		m_nAddBackNoiseY = nAddBack;
		printf("nNoiseY=%d nShift=%d\n", nNoiseY, m_nShift);
		BuildNoiseLUT(nNoiseY, nNoiseYCurve, m_nNoiseYLUT);

		return DNSYImage(pYUVImage);
	}
	virtual bool ProcessImage(CYUV420Image *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[])
	{
		m_nMAXS = 255;
		m_nShift = 0;
		m_nMask = 0;
		m_nAddBackNoiseY = nAddBack;
		printf("nNoiseY=%d nShift=%d\n", nNoiseY, m_nShift);
		BuildNoiseLUT(nNoiseY, nNoiseYCurve, m_nNoiseYLUT);

		return DNSYImage(pYUVImage);
	}
};

#endif
