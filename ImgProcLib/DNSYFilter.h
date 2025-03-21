#ifndef __DNS_Y_FILTER_H_
#define __DNS_Y_FILTER_H_

#include "../ImgLib/Filter.h"

class CDNSYFilter : public CFilter
{
protected:
	int m_nWinW;
	int m_nAddBackNoise;
	int m_nNoiseLUT[256];

	bool ExtractYChannel(CRGBYUVImage *pYUVImage, CGrayImage *pYImage);
	void ReplaceYChannel(CGrayImage *pYImage, CRGBYUVImage *pYUVImage);
	bool ExtractYChannel(CYUV422Image *pYUVImage, CGrayImage *pYImage);
	void ReplaceYChannel(CGrayImage *pYImage, CYUV422Image *pYUVImage);
	bool ExtractYChannel(CYUV420Image *pYUVImage, CGrayImage *pYImage);
	void ReplaceYChannel(CGrayImage *pYImage, CYUV420Image *pYUVImage);

	virtual bool DNSYImage(CGrayImage *pYImage) = 0;

	virtual bool InitAdvancedParamList() { return true; }
	virtual bool InitParamList()
	{
		m_ParamList.AddVector("nNoiseYCurve", m_nNoiseYCurve, 9, 1, 256, "Noise Relative changes with Y");

		m_nNoiseYCurve[0] = 8;
		m_nNoiseYCurve[1] = 12;
		m_nNoiseYCurve[2] = 16;
		m_nNoiseYCurve[3] = 16;
		m_nNoiseYCurve[4] = 16;
		m_nNoiseYCurve[5] = 16;
		m_nNoiseYCurve[6] = 12;
		m_nNoiseYCurve[7] = 8;
		m_nNoiseYCurve[8] = 6;

		return InitAdvancedParamList();
	}
public:
	int m_nNoiseYCurve[9];

	virtual void EstimateNoiseLevel(int nNoiseY)
	{
		int Y, iY, dY, nGain;

		for (Y = 0; Y < 256; Y++)
		{
			iY = (Y >> 5);
			dY = (Y & 31);
			nGain = m_nNoiseYCurve[iY];
			nGain += ((m_nNoiseYCurve[iY + 1] - m_nNoiseYCurve[iY])*dY + 16) / 32;
			m_nNoiseLUT[Y] = (nNoiseY*nGain + 8) >> 4;
			m_nNoiseLUT[Y] = (m_nNoiseLUT[Y] * m_nNoiseLUT[Y] + 8) >> 4;
		}
	}
	virtual bool ProcessImage(CGrayImage *pYImage, int nWinW, int nNoiseY, int nAddBack)
	{
		if (nWinW < 1)nWinW = 1;	if (nWinW > 4)nWinW = 4;
		m_nWinW = nWinW;
		m_nAddBackNoise = nAddBack;

		if (nNoiseY == 0 || nAddBack == 16)return true;
		EstimateNoiseLevel(nNoiseY);
		if (!DNSYImage(pYImage))return false;

		return true;
	}
	virtual bool ProcessImage(CRGBYUVImage *pYUVImage, int nWinW, int nNoiseY, int nAddBack)
	{
		CGrayImage YImage;

		if (nWinW < 1)nWinW = 1;	if (nWinW > 4)nWinW = 4;
		m_nWinW = nWinW;
		m_nAddBackNoise = nAddBack;

		if (nNoiseY == 0 || nAddBack == 16)return true;
		EstimateNoiseLevel(nNoiseY);
		if (!ExtractYChannel(pYUVImage, &YImage))return false;
		if (!DNSYImage(&YImage))return false;
		ReplaceYChannel(&YImage, pYUVImage);

		return true;
	}
	virtual bool ProcessImage(CYUV422Image *pYUVImage, int nWinW, int nNoiseY, int nAddBack)
	{
		CGrayImage YImage;

		if (nWinW < 1)nWinW = 1;	if (nWinW > 4)nWinW = 4;
		m_nWinW = nWinW;
		m_nAddBackNoise = nAddBack;

		if (nNoiseY == 0 || nAddBack == 16)return true;
		EstimateNoiseLevel(nNoiseY);
		if (!ExtractYChannel(pYUVImage, &YImage))return false;
		if (!DNSYImage(&YImage))return false;
		ReplaceYChannel(&YImage, pYUVImage);

		return true;
	}
	virtual bool ProcessImage(CYUV420Image *pYUVImage, int nWinW, int nNoiseY, int nAddBack)
	{
		CGrayImage YImage;

		if (nWinW < 1)nWinW = 1;	if (nWinW > 4)nWinW = 4;
		m_nWinW = nWinW;
		m_nAddBackNoise = nAddBack;

		if (nNoiseY == 0 || nAddBack == 16)return true;
		EstimateNoiseLevel(nNoiseY);
		if (!ExtractYChannel(pYUVImage, &YImage))return false;
		if (!DNSYImage(&YImage))return false;
		ReplaceYChannel(&YImage, pYUVImage);

		return true;
	}
};

#endif

