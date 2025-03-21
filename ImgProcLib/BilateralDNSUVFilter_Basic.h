#ifndef __BILATERAL_DNSUVFILTER_BASIC_H_
#define __BILATERAL_DNSUVFILTER_BASIC_H_

#include "WaveletPyramidYUV420DNSFilter.h"

class CBilateralDNSUVFilter_Basic : public CUVDNSFilter
{
protected:
	int m_nMAXS;
	int m_nShift;
	int m_nNoiseY;
	int m_nNoiseUV;
	int m_nInvNoiseY;
	int m_nInvNoiseUV;
	int m_nWinW;

	static int m_nMask1[9];
	static int m_nMask2[25];
	int m_nMask4[81];
	int m_nMask8[289];

	void BuildMask(int *pMask, int nWinW);
	void RotateYUVLine(short *pInLine, short *pOutLine, int nWidth, int nInPitch, int nOutPitch);

	int m_pYUVWinBuffer[17*17*3];
	
	int *m_pYUVWin[17];
	int *m_pMask;
	void DNSUVWindow(int UV[]);
	void DNSUVLine(short *pInLines[], short *pOutLine, int nWidth, int nInPitch, int nOutPitch);
	bool DNSUVImage(CMultipleChannelImage *pYUVImage);

	virtual bool InitParamList()
	{
		return true;
	}
public:
	CBilateralDNSUVFilter_Basic()
	{
		BuildMask(m_nMask4, 4);
		BuildMask(m_nMask8, 8);
		Initialize("CBilateralDNSUVFilter_Basic");
	}
	virtual bool ProcessImage(CMultipleChannelImage *pYUVImage, int nYThre, int nNoiseUV, int nWinW, int nShift)
	{
		m_nMAXS = pYUVImage->m_nMAXS;
		m_nShift = nShift;
		m_nNoiseY = (nYThre*nYThre+4)>>3;
		m_nNoiseUV = (nNoiseUV*nNoiseUV + 4) >> 3;
		m_nWinW = nWinW;

		if (m_nNoiseY == 0 || m_nNoiseUV == 0)return true;

		if (m_nNoiseY > 65535)m_nNoiseY = 65535;
		if (m_nNoiseUV > 65535)m_nNoiseUV = 65535;
		m_nInvNoiseY = (1 << 24) / m_nNoiseY;
		m_nInvNoiseUV = (1 << 24) / m_nNoiseUV;

		return DNSUVImage(pYUVImage);
	}
};

#endif
