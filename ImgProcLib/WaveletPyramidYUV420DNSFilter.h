#ifndef __WAVELET_PYRAMID_YUV420DNS_FILTER_H_
#define __WAVELET_PYRAMID_YUV420DNS_FILTER_H_

#include "../ImgLib/Filter.h"

class CYDNSFilter : public CFilter
{
protected:
	int m_nMAXS;
	int m_nShift;
	int m_nNoiseYLUT[256];
	int m_nAddBackNoiseY;

	virtual void BuildNoiseLUT(int nNoise, int nCurve[], int nLUT[]);

public:
	bool m_bEdgeAdaptiveEnable;

	virtual bool ProcessImage(CMultipleChannelImage *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[], int nShift)=0;
	virtual bool ProcessImage(CYUV420Image *pYUV420Image, int nNoiseY, int nAddBackY, int nNoiseYCurve[])=0;
};

class CUVDNSFilter : public CFilter
{
public:
	virtual bool ProcessImage(CMultipleChannelImage *pYUVImage, int nYThre, int nNoiseUV, int nWinW, int nShift)=0;
};

class CWavelePyramidYUV420DNSFilter: public CMultipleFilterProcessFlow
{
protected:
	CYDNSFilter *m_pYDNSFilter;
	CUVDNSFilter *m_pUVDNSFilter;
	
	int m_nNoiseY;
	int m_nNoiseYCurve[9];
	int m_nDNSUVYThre;
	int m_nNoiseUV;
	
	void fWT8x8(short *pBlock);
	void iWT8x8_Level2(short *pBlock);
	void iWT8x8_Level1(short *pBlock);
	void iWT8x8_Level0(short *pBlock);

	void FWTBlockLine(short *pBlockLine, int nBlockWidth, int nDim=3);
	void YUVH2BlockLine(short *pYUVHLine, short *pBlockLine, int nImageWidth, int nInPitch, int nDim=3);
	void Block2YUVHLine(short *pBlockLine, short *pYUVHLine, int nImageWidth, int nOutPitch, int nDim=3);
	bool FWTYUVHImage(CMultipleChannelImage *pYUVHImage, CMultipleChannelImage *pBlockImage);

	void IWTBlockLine_Level2(short *pBlockLine, int nBlockWidth, int nDim = 3);
	void IWTBlockLine_Level1(short *pBlockLine, int nBlockWidth, int nDim = 3);
	void IWTBlockLine_Level0(short *pBlockLine, int nBlockWidth, int nDim = 3);

	void IWTBlockImage_Level2(CMultipleChannelImage *pBlockImage);
	void IWTBlockImage_Level1(CMultipleChannelImage *pBlockImage);
	void IWTBlockImage_Level0(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVHImage);

	bool ExtractYUVImage_Level3(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);
	bool ExtractYUVImage_Level2(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);
	bool ExtractYUVImage_Level1(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);

	void ReplaceYUVImage_Level3(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);
	void ReplaceYUVImage_Level2(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);
	void ReplaceYUVImage_Level1(CMultipleChannelImage *pBlockImage, CMultipleChannelImage *pYUVImage);

	virtual bool DNSYImage(CMultipleChannelImage *pYUVImage, int nNoiseY, int nAddBack, int nNoiseYCurve[], int nShift)
	{
		if (m_pYDNSFilter != NULL)
		{
			m_pYDNSFilter->m_bEdgeAdaptiveEnable = (m_bEdgeAdaptiveEnableLevel[nShift] == 1);
			if (!m_pYDNSFilter->ProcessImage(pYUVImage, nNoiseY, nAddBack, nNoiseYCurve, nShift))return false;
		}
		return true;
	}
	virtual bool DNSUVImage(CMultipleChannelImage *pYUVImage, int nYThre, int nNoiseUV, int nWinW, int nShift)
	{
		if (m_pUVDNSFilter != NULL)
		{
			if (!m_pUVDNSFilter->ProcessImage(pYUVImage, nYThre, nNoiseUV, nWinW, nShift))return false;
		}
		return true;
	}
	
	bool DNSLevel3(CMultipleChannelImage *pBlockImage);
	bool DNSLevel2(CMultipleChannelImage *pBlockImage);
	bool DNSLevel1(CMultipleChannelImage *pBlockImage);
	bool DNSLevel0(CMultipleChannelImage *pYUVHImage);

	bool YUV420ToYUVHImage(CYUV420Image *pInImage, CMultipleChannelImage *pOutImage);
	void YUVHToYUV420Image(CMultipleChannelImage *pInImage, CYUV420Image *pOutImage);

	virtual bool CreateFilterList()
	{
		if (m_pYDNSFilter != NULL)AddFilter(m_pYDNSFilter);
		if (m_pUVDNSFilter != NULL)AddFilter(m_pUVDNSFilter);

		return true;
	}
	virtual bool InitParamList()
	{
		m_ParamList.AddVector("nNoiseYList", m_nNoiseYList, 9, 0, 1023, "YDNS NoiseY at Different Gain");
		m_nNoiseYList[0] = 10;
		m_nNoiseYList[1] = 15;
		m_nNoiseYList[2] = 20;
		m_nNoiseYList[3] = 30;
		m_nNoiseYList[4] = 40;
		m_nNoiseYList[5] = 60;
		m_nNoiseYList[6] = 80;
		m_nNoiseYList[7] = 120;
		m_nNoiseYList[8] = 160;

		m_ParamList.AddVector("bEdgeAdaptiveEnableLevel", m_bEdgeAdaptiveEnableLevel, 5, 0, 1, "Enable Edge Adpative at Each Pyramid Level");
		m_bEdgeAdaptiveEnableLevel[0] = 1;
		m_bEdgeAdaptiveEnableLevel[1] = 1;
		m_bEdgeAdaptiveEnableLevel[2] = 1;
		m_bEdgeAdaptiveEnableLevel[3] = 1;
		m_bEdgeAdaptiveEnableLevel[4] = 1;

		m_ParamList.AddVector("nAddBackNoiseYLevel", m_nAddBackNoiseYLevel, 5, 0, 16, "Add Back Noise Y at Each Pyramid Level");
		m_nAddBackNoiseYLevel[0] = 0;
		m_nAddBackNoiseYLevel[1] = 4;
		m_nAddBackNoiseYLevel[2] = 6;
		m_nAddBackNoiseYLevel[3] = 8;
		m_nAddBackNoiseYLevel[4] = 12;

		m_ParamList.AddVector("nNoiseYCurveGainThre", m_nNoiseYCurveGainThre, 2, 1, 16, "Gain Threshold for NoiseYCurve");
		m_nNoiseYCurveGainThre[0] = 2;
		m_nNoiseYCurveGainThre[1] = 4;

		m_ParamList.AddVector("nNoiseYCurveL", m_nNoiseYCurveL, 9, 1, 16, "Noise Relative changes with Y in Low Gain");
		m_nNoiseYCurveL[0] = 8;
		m_nNoiseYCurveL[1] = 12;
		m_nNoiseYCurveL[2] = 16;
		m_nNoiseYCurveL[3] = 16;
		m_nNoiseYCurveL[4] = 16;
		m_nNoiseYCurveL[5] = 12;
		m_nNoiseYCurveL[6] = 8;
		m_nNoiseYCurveL[7] = 6;
		m_nNoiseYCurveL[8] = 4;

		m_ParamList.AddVector("nNoiseYCurveH", m_nNoiseYCurveH, 9, 1, 16, "Noise Relative changes with Y in High Gain");
		m_nNoiseYCurveH[0] = 16;
		m_nNoiseYCurveH[1] = 16;
		m_nNoiseYCurveH[2] = 16;
		m_nNoiseYCurveH[3] = 16;
		m_nNoiseYCurveH[4] = 16;
		m_nNoiseYCurveH[5] = 16;
		m_nNoiseYCurveH[6] = 16;
		m_nNoiseYCurveH[7] = 12;
		m_nNoiseYCurveH[8] = 8;

		m_ParamList.AddVector("nDNSUVYThreList", m_nDNSUVYThreList, 9, 0, 1023, "UVDNS Y Threshold at Differernt Gain");
		m_nDNSUVYThreList[0] = 16;
		m_nDNSUVYThreList[1] = 24;
		m_nDNSUVYThreList[2] = 32;
		m_nDNSUVYThreList[3] = 48;
		m_nDNSUVYThreList[4] = 64;
		m_nDNSUVYThreList[5] = 96;
		m_nDNSUVYThreList[6] = 128;
		m_nDNSUVYThreList[7] = 192;
		m_nDNSUVYThreList[8] = 256;

		m_ParamList.AddVector("nNoiseUVList", m_nNoiseUVList, 9, 0, 1023, "UVDNS NoiseUV at Differernt Gain");
		m_nNoiseUVList[0] = 24;
		m_nNoiseUVList[1] = 32;
		m_nNoiseUVList[2] = 48;
		m_nNoiseUVList[3] = 64;
		m_nNoiseUVList[4] = 96;
		m_nNoiseUVList[5] = 128;
		m_nNoiseUVList[6] = 192;
		m_nNoiseUVList[7] = 256;
		m_nNoiseUVList[8] = 384;

		m_nNoiseYLevel[0] = 16;
		m_nNoiseYLevel[1] = 23;
		m_nNoiseYLevel[2] = 32;
		m_nNoiseYLevel[3] = 45;
		m_nNoiseYLevel[4] = 64;

		m_nYThreLevel[0] = 16;
		m_nYThreLevel[1] = 32;
		m_nYThreLevel[2] = 64;
		m_nYThreLevel[3] = 128;

		m_nNoiseUVLevel[0] = 16;
		m_nNoiseUVLevel[1] = 23;
		m_nNoiseUVLevel[2] = 32;
		m_nNoiseUVLevel[3] = 45;

		m_nDNSUVWinW[0] = 1;
		m_nDNSUVWinW[1] = 2;
		m_nDNSUVWinW[2] = 4;
		m_nDNSUVWinW[3] = 8;

		return true;
	}
public:
	int m_nNoiseYList[9];
	int m_bEdgeAdaptiveEnableLevel[5];
	int m_nAddBackNoiseYLevel[5];
	int m_nNoiseYCurveL[9];
	int m_nNoiseYCurveH[9];
	int m_nNoiseYCurveGainThre[2];

	int m_nDNSUVYThreList[9];
	int m_nNoiseUVList[9];

	int m_nNoiseYLevel[5];
	int m_nYThreLevel[4];
	int m_nNoiseUVLevel[4];
	int m_nDNSUVWinW[4];

	CWavelePyramidYUV420DNSFilter()
	{
		m_pYDNSFilter = NULL;
		m_pUVDNSFilter = NULL;
	}
	~CWavelePyramidYUV420DNSFilter()
	{
		if (m_pYDNSFilter != NULL)delete m_pYDNSFilter;
		if (m_pUVDNSFilter != NULL)delete m_pUVDNSFilter;
	}

	void EstimateNoiseLevel(TCameraInfo *pInfo)
	{
		int i;
		int nGain = (pInfo->nCameraGain*pInfo->nDigiGain + (1 << (pInfo->nDigiGainShift - 1))) >> (pInfo->nDigiGainShift);
		m_nNoiseY = m_nNoiseYList[0];
		m_nDNSUVYThre = m_nDNSUVYThreList[0];
		m_nNoiseUV = m_nNoiseUVList[0];

		if (nGain > (1 << (pInfo->nCameraGainShift)))
		{
			for (i = 1; i < 9; i++)
			{
				if (nGain < (1 << (pInfo->nCameraGainShift + i)))
				{
					int dG = nGain - (1 << (pInfo->nCameraGainShift + i - 1));
					int half = (1 << (pInfo->nCameraGainShift + i - 2));

					m_nNoiseY += ((m_nNoiseYList[i] - m_nNoiseYList[i - 1])*dG + half) >> (pInfo->nCameraGainShift + i - 1);
					m_nDNSUVYThre += ((m_nDNSUVYThreList[i] - m_nDNSUVYThreList[i - 1])*dG + half) >> (pInfo->nCameraGainShift + i - 1);
					m_nNoiseUV += ((m_nNoiseUVList[i] - m_nNoiseUVList[i - 1])*dG + half) >> (pInfo->nCameraGainShift + i - 1);
					break;
				}
				m_nNoiseY = m_nNoiseYList[i];
				m_nDNSUVYThre = m_nDNSUVYThreList[i];
				m_nNoiseUV = m_nNoiseUVList[i];
			}
		}
//		printf("Gain=%d DNSY: NoiseY=%d DNSUV: YThre=%d NoiseUV=%d\n", nGain, m_nNoiseY, m_nDNSUVYThre, m_nNoiseUV);

		if (nGain < (m_nNoiseYCurveGainThre[0] << pInfo->nCameraGainShift))
		{
			for (i = 0; i < 9; i++)
			{
				m_nNoiseYCurve[i] = m_nNoiseYCurveL[i];
			}
		}
		else if (nGain < (m_nNoiseYCurveGainThre[1] << pInfo->nCameraGainShift))
		{
			int dG = nGain - (m_nNoiseYCurveGainThre[0] << pInfo->nCameraGainShift);
			for (i = 0; i < 9; i++)
			{
				m_nNoiseYCurve[i] = m_nNoiseYCurveL[i];
				m_nNoiseYCurve[i] += (((m_nNoiseYCurveH[i] - m_nNoiseYCurveL[i])*dG) / (m_nNoiseYCurveGainThre[1] - m_nNoiseYCurveGainThre[0])) >> pInfo->nCameraGainShift;
			}
		}
		else
		{
			for (i = 0; i < 9; i++)
			{
				m_nNoiseYCurve[i] = m_nNoiseYCurveH[i];
			}
		}
	}

	DWORD m_Time;

	virtual bool ProcessImage(CYUV420Image *pYUV420Image, TCameraInfo *pInfo)
	{
		CMultipleChannelImage YUVHImage, BlockImage;
		
		EstimateNoiseLevel(pInfo);

		if (!YUV420ToYUVHImage(pYUV420Image, &YUVHImage))return false;
		if (!FWTYUVHImage(&YUVHImage, &BlockImage))return false;

		if (!DNSLevel3(&BlockImage))return false;
		IWTBlockImage_Level2(&BlockImage);

		if (!DNSLevel2(&BlockImage))return false;
		IWTBlockImage_Level1(&BlockImage);

		if (!DNSLevel1(&BlockImage))return false;
		IWTBlockImage_Level0(&BlockImage, &YUVHImage);

		if (!DNSLevel0(&YUVHImage))return false;
		YUVHToYUV420Image(&YUVHImage, pYUV420Image);

		if (m_pYDNSFilter != NULL&&m_nAddBackNoiseYLevel[0]<16)
		{
			m_pYDNSFilter->m_bEdgeAdaptiveEnable = (m_bEdgeAdaptiveEnableLevel[0] == 1);
			DWORD t0 = GetTickCount();
			if (!m_pYDNSFilter->ProcessImage(pYUV420Image, m_nNoiseY, m_nAddBackNoiseYLevel[0], m_nNoiseYCurve))return false;
			DWORD t1 = GetTickCount();
			m_Time = t1 - t0;
		}

		return true;
	}
};

#endif

