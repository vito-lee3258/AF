#ifndef __BILATERAL_UVDNS_FILTER_BASIC_H_
#define __BILATERAL_UVDNS_FILTER_BASIC_H_

#include "DNSUVFilter.h"

class CBilateralUVDNSFilter_Basic : public CDNSUVFilter
{
protected:

	int m_nDitheringE[2];
	int m_nMask2D[17][17];
	int m_nYUVWin[17][17][3];
	void BuildMask(int nWinW);
	void RotateYUVLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch);
	void DNSUVWindow(int UV[]);
	void DNSUVLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nPitch);
	virtual bool DNSUVImage(CRGBYUVImage *pYUVImage);

	virtual bool InitParamList() { return true; }
public:
	CBilateralUVDNSFilter_Basic()
	{
		Initialize("CBilateralUVDNSFilter_Basic");
	}
};

#endif
