#ifndef __WAVELET_PYRAMID_YUV420_DNSFILTER_BASIC_H_
#define __WAVELET_PYRAMID_YUV420_DNSFILTER_BASIC_H_

#include "NLM_YDNSFilter_Basic.h"
#include "BilateralDNSUVFilter_Basic.h"
#include "WaveletPyramidYUV420DNSFilter.h"

class CWaveletPyramidYUV420DNSFilter_Basic : public CWavelePyramidYUV420DNSFilter
{
public:
	CWaveletPyramidYUV420DNSFilter_Basic()
	{
		m_pYDNSFilter = new CNLM_YDNSFilter_Basic;
		m_pUVDNSFilter = new CBilateralDNSUVFilter_Basic;
		Initialize("CWaveletPyramidYUV420DNSFilter_Basic");
	}
};

class CWaveletPyramidYUV420DNSFilter_V2 : public CWavelePyramidYUV420DNSFilter
{
public:
	CWaveletPyramidYUV420DNSFilter_V2()
	{
		m_pYDNSFilter = new CNLM_YDNSFilter_ABS;
		m_pUVDNSFilter = new CBilateralDNSUVFilter_Basic;
		Initialize("CWaveletPyramidYUV420DNSFilter_V2");
	}
};

#endif
