#ifndef __NLM_DNSYFILTER_BASIC_H_
#define __NLM_DNSYFILTER_BASIC_H_

#include "DNSYFilter.h"

class CNLM_DNSYFilter_Basic: public CDNSYFilter
{
protected:
	void RotateYLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch);

	int m_nDitheringE;
	int m_nYWin[9][9];
	int DNSYWindow();
	void DNSYLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch);
	virtual bool DNSYImage(CGrayImage *pYImage);

	virtual bool InitAdvancedParamList()
	{
		m_ParamList.Add("bEdgeAdaptiveEnable", &m_bEdgeAdaptiveEnable, 0, 1, "Enable Edge Adaptive Option");
		m_bEdgeAdaptiveEnable = 1;

		return true;
	}
public:
	int m_bEdgeAdaptiveEnable;

	CNLM_DNSYFilter_Basic()
	{
		Initialize("CNLM_DNSYFilter_Basic");
	}
};

#endif

