#ifndef __PYRAMID_YUVDNS_FILTER_H_
#define __PYRAMID_YUVDNS_FILTER_H_

#include "GaussianPyramid.h"
#include "NLM_DNSYFilter_Basic.h"
#include "BilateralUVDNSFilter_Basic.h"

class CPyramidYUVDNSFilter : public CMultipleFilterProcessFlow, public CGaussianPyramid
{
protected:
	int m_nNoiseY;
	int m_nNoiseUV;

	CRGBYUVImage m_LImage;
	CMultipleChannelImage m_EdgePyramid[3];

	CNLM_DNSYFilter_Basic m_DNSYFilter;
	CBilateralUVDNSFilter_Basic m_DNSUVFilter;

	bool BuildPyramid(CRGBYUVImage *pInImage);
	bool BuildPyramid(CYUV420Image *pInImage);
	bool DNSYUVImage_Level(CRGBYUVImage *pYUVImage, int nLevel);

	virtual bool CreateFilterList()
	{
		AddFilter(&m_DNSYFilter);
		AddFilter(&m_DNSUVFilter);

		return true;
	}
	virtual bool InitParamList()
	{
		m_ParamList.AddVector("nDNSYWindWLevel", m_nDNSYWinWLevel, 4, 1, 4, "DNSYFilter Window Size at Each Pyramid Level");
		m_nDNSYWinWLevel[0] = 2;
		m_nDNSYWinWLevel[1] = 2;
		m_nDNSYWinWLevel[2] = 3;
		m_nDNSYWinWLevel[3] = 4;

		m_ParamList.AddVector("nNoiseYLevel", m_nNoiseYLevel, 4, 1, 16, "Noise Y at Each Pyarmid Level");
		m_nNoiseYLevel[0] = 16;
		m_nNoiseYLevel[1] = 8;
		m_nNoiseYLevel[2] = 4;
		m_nNoiseYLevel[3] = 2;

		m_ParamList.AddVector("nAddBackNoiseYLevel", m_nAddBackNoiseYLevel, 4, 0, 15, "Add Back Noise Y at Each Pyramid Level");
		m_nAddBackNoiseYLevel[0] = 1;
		m_nAddBackNoiseYLevel[1] = 2;
		m_nAddBackNoiseYLevel[2] = 4;
		m_nAddBackNoiseYLevel[3] = 8;

		m_ParamList.AddVector("nNoiseYList", m_nNoiseYList, 9, 0, 255, "Noise Y at Different Gain");
		m_nNoiseYList[0] = 10;
		m_nNoiseYList[1] = 15;
		m_nNoiseYList[2] = 20;
		m_nNoiseYList[3] = 30;
		m_nNoiseYList[4] = 40;
		m_nNoiseYList[5] = 60;
		m_nNoiseYList[6] = 80;
		m_nNoiseYList[7] = 120;
		m_nNoiseYList[8] = 160;

		m_ParamList.AddVector("m_nDNSUVWinWLevel", m_nDNSUVWinWLevel, 4, 1, 8, "DNSUVFilter Window Size at Each Pyramid Level");
		m_nDNSUVWinWLevel[3] = 8;
		m_nDNSUVWinWLevel[2] = 6;
		m_nDNSUVWinWLevel[1] = 4;
		m_nDNSUVWinWLevel[0] = 3;

		m_ParamList.AddVector("nNoiseUVLevel", m_nNoiseUVLevel, 4, 1, 16, "Noise UV at Each Pyramid Level");
		m_nNoiseUVLevel[0] = 16;
		m_nNoiseUVLevel[1] = 12;
		m_nNoiseUVLevel[2] = 8;
		m_nNoiseUVLevel[3] = 6;

		m_ParamList.Add("nDNSUVYThre", &m_nDNSUVYThre, 0, 255, "Y Threshold for DNSUV");
		m_nDNSUVYThre = 16;

		m_ParamList.AddVector("nNoiseUVList", m_nNoiseUVList, 9, 0, 1023, "Noise UV at Different Gain");
		m_nNoiseUVList[0] = 24;
		m_nNoiseUVList[1] = 32;
		m_nNoiseUVList[2] = 48;
		m_nNoiseUVList[3] = 64;
		m_nNoiseUVList[4] = 96;
		m_nNoiseUVList[5] = 128;
		m_nNoiseUVList[6] = 192;
		m_nNoiseUVList[7] = 256;
		m_nNoiseUVList[8] = 384;

		return true;
	}
public:
	int m_nDNSYWinWLevel[4];
	int m_nNoiseYLevel[4];
	int m_nNoiseYList[9];
	int m_nAddBackNoiseYLevel[4];

	int m_nDNSUVWinWLevel[4];
	int m_nNoiseUVLevel[4];
	int m_nDNSUVYThre;
	int m_nNoiseUVList[9];

	CPyramidYUVDNSFilter()
	{
		Initialize("CPyramidYUVDNSFilter");
	}

	void EstimateNoiseLevel(TCameraInfo *pInfo)
	{
		int i;
		int nGain = (pInfo->nCameraGain*pInfo->nDigiGain + (1 << (pInfo->nDigiGainShift - 1))) >> (pInfo->nDigiGainShift);
		m_nNoiseY = m_nNoiseYList[0];
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
					m_nNoiseUV += ((m_nNoiseUVList[i] - m_nNoiseUVList[i - 1])*dG + half) >> (pInfo->nCameraGainShift + i - 1);
					break;
				}
				m_nNoiseY = m_nNoiseYList[i];
				m_nNoiseUV = m_nNoiseUVList[i];
			}
		}
//		printf("Gain=%d NoiseY=%d NoiseUV=%d\n", nGain, m_nNoiseY, m_nNoiseUV);
	}

	bool ProcessImage(CRGBYUVImage *pImage, TCameraInfo *pInfo)
	{
		EstimateNoiseLevel(pInfo);

		if (!BuildPyramid(pImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 3))return false;
		if (!UpScaleImage(&m_LImage, pImage))return false;
		if (!AddBackEdge(pImage, m_EdgePyramid + 2, &m_LImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 2))return false;
		if (!UpScaleImage(&m_LImage, pImage))return false;
		if (!AddBackEdge(pImage, m_EdgePyramid + 1, &m_LImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 1))return false;
		if (!UpScaleImage(&m_LImage, pImage))return false;
		if (!AddBackEdge(pImage, m_EdgePyramid , pImage))return false;

		if (!DNSYUVImage_Level(pImage, 0))return false;

		return true;
	}

	bool ProcessImage(CYUV420Image *pImage, TCameraInfo *pInfo)
	{
		CRGBYUVImage TempYUVImage;
		EstimateNoiseLevel(pInfo);

		if (!BuildPyramid(pImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 3))return false;
		if (!UpScaleImage(&m_LImage, &TempYUVImage))return false;
		if (!AddBackEdge(&TempYUVImage, m_EdgePyramid + 2, &m_LImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 2))return false;
		if (!UpScaleImage(&m_LImage, &TempYUVImage))return false;
		if (!AddBackEdge(&TempYUVImage, m_EdgePyramid + 1, &m_LImage))return false;

		if (!DNSYUVImage_Level(&m_LImage, 1))return false;
		if (!UpScaleImage(&m_LImage, pImage))return false;
		if (!AddBackEdge(pImage, m_EdgePyramid, pImage))return false;

		if (m_nDNSYWinWLevel[0] > 1)
		{
			int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[0] + 8) >> 4;
//			printf("Level0: DNSYNoise=%d DNSYWinW=%d NoiseYAddBack=%d\n", nDNSYNoise, m_nDNSYWinWLevel[0], m_nAddBackNoiseYLevel[0]);
			if (!m_DNSYFilter.ProcessImage(pImage, m_nDNSYWinWLevel[0], nDNSYNoise, m_nAddBackNoiseYLevel[0]))return false;
		}

		return true;
	}
};

#endif
