#ifndef __DNSUVFILTER_H_
#define __DNSUVFILTER_H_

#include "../ImgLib/Filter.h"

class CDNSUVFilter : public CFilter
{
protected:
	int m_nWinW;
	int m_nNoiseYUV[3];

	bool ExtractYUVSubImage(CYUV422Image *pYUV422Image, CRGBYUVImage *pYUVImage);
	bool ExtractYUVSubImage(CYUV420Image *pYUV420Image, CRGBYUVImage *pYUVImage);
	void ReplaceUVChannel(CRGBYUVImage *pYUVImage, CYUV422Image *pYUV422Image);
	void ReplaceUVChannel(CRGBYUVImage *pYUVImage, CYUV420Image *pYUV420Image);

	virtual bool DNSUVImage(CRGBYUVImage *pYUVImage) = 0;
public:

	virtual bool ProcessImage(CRGBYUVImage *pYUVImage, int nWinW, int nNoiseYUV[])
	{
		int i;
		
		if (nWinW < 1)nWinW = 1;	if (nWinW > 8)nWinW = 8;
		m_nWinW = nWinW;
		for (i = 0; i < 3; i++)
		{
//			m_nNoiseYUV[i] = (nNoiseYUV[i] * nNoiseYUV[i] + 8) >> 4;
			m_nNoiseYUV[i] = (nNoiseYUV[i] * nNoiseYUV[i] + 4) >> 3;
		}
		if (m_nNoiseYUV[0] > 0 && m_nNoiseYUV[1] > 0 && m_nNoiseYUV[2] > 0)
		{
			if (!DNSUVImage(pYUVImage))return false;
		}

		return true;
	}
	virtual bool ProcessImage(CYUV422Image *pYUV422Image, int nWinW, int nNoiseYUV[])
	{
		int i;
		CRGBYUVImage YUVImage;

		if (nWinW < 1)nWinW = 1;	if (nWinW > 8)nWinW = 8;
		m_nWinW = nWinW;
		for (i = 0; i < 3; i++)
		{
			m_nNoiseYUV[i] = (nNoiseYUV[i] * nNoiseYUV[i] + 8) >> 4;
		}

		if (!ExtractYUVSubImage(pYUV422Image, &YUVImage))return false;
		if (m_nNoiseYUV[0] > 0 && m_nNoiseYUV[1] > 0 && m_nNoiseYUV[2] > 0)
		{
			if (!DNSUVImage(&YUVImage))return false;
		}
		ReplaceUVChannel(&YUVImage, pYUV422Image);

		return true;
	}
	virtual bool ProcessImage(CYUV420Image *pYUV420Image, int nWinW, int nNoiseYUV[])
	{
		int i;
		CRGBYUVImage YUVImage;

		if (nWinW < 1)nWinW = 1;	if (nWinW > 8)nWinW = 8;
		m_nWinW = nWinW;
		for (i = 0; i < 3; i++)
		{
			m_nNoiseYUV[i] = (nNoiseYUV[i] * nNoiseYUV[i] + 8) >> 4;
		}

		if (!ExtractYUVSubImage(pYUV420Image, &YUVImage))return false;
		if (m_nNoiseYUV[0] > 0 && m_nNoiseYUV[1] > 0 && m_nNoiseYUV[2] > 0)
		{
			if (!DNSUVImage(&YUVImage))return false;
		}
		ReplaceUVChannel(&YUVImage, pYUV420Image);

		return true;
	}
};

#endif
