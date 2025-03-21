#ifndef __BAYER_RAW_IMAGE_H_
#define __BAYER_RAW_IMAGE_H_

#include "BasicImageArray.h"
#include "MultipleChannelImage.h"

class CBayerRAWImage : public CBasicImageArray_WORD
{
protected:
	int m_nCFAPattern;	
	/*
		0:	BG
			GR
		1:	GB
			RG
		2:	GR
			BG
		3:	RG
			GB
	*/
public:
	int m_nMAXS;	//max signal level
	int m_nBLK;		//black level
	int m_nBit;		//bit width

	CBayerRAWImage()
	{
		m_nMAXS = 4095;
		m_nBLK = 0;
		m_nBit = 12;
		m_nCFAPattern = 0;
	}
	void SetCFAPattern(int nCFAPattern)
	{
		m_nCFAPattern = (nCFAPattern & 3);
	}
	int GetCFAPattern()
	{
		return m_nCFAPattern;
	}
	bool Create(int nWidth, int nHeight, int nBits)
	{
		m_nBit = (nBits>16) ? (16) : (nBits);
		m_nMAXS = (1 << m_nBit) - 1;
		m_nBLK = 0;
		m_nCFAPattern = 0;

		return SetImageSize((nWidth>>1)*2, (nHeight>>1)*2, 1);
	}
	void CopyParameter(CBayerRAWImage *pImage)
	{
		m_nMAXS = pImage->m_nMAXS;
		m_nBLK = pImage->m_nBLK;
		m_nCFAPattern = pImage->GetCFAPattern();
		m_nBit = pImage->m_nBit;
	}
	bool Copy(CBayerRAWImage *pImage)
	{
		if (!Create(pImage->GetWidth(), pImage->GetHeight(), pImage->m_nBit))return false;
		CopyParameter(pImage);
		memcpy(m_pData, pImage->GetImageData(), m_nWidth*m_nHeight * sizeof(WORD));

		return true;
	}

	bool ExtractBayerRAWFromBGRImage(CRGBYUVImage *pBGRImage, int nCFAPattern, int nBit);

	bool SaveToBMPFile(char *pFileName, bool bColor = false);
	bool LoadFromBMPFile(char *pFileName, int nCFAPattern=0, int nBit=12);

	bool CheckRAWFileInfo(char *pFileName, int &nFileSize, bool &bRAW8File, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bMSBAlign, bool &bByteOrder);

	bool SaveToRAW16File(char *pFileName, bool bMSBAlign, bool bByteOrder, bool bWithInfo);
	bool LoadFromRAW16File(char *pFileName, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bMSBAlign, bool &bByteOrder, bool &bWithInfo);

	bool SaveToRAW8File(char *pFileName, bool bWithInfo);
	bool LoadFromRAW8File(char *pFileName, int &nWidth, int &nHeight, int &nBit, int &nCFAPattern, int &nBLK, bool &bWithInfo);

	virtual unsigned short *GetImageLine(int nY)
	{
		if (nY < 0)nY = ((-nY) & 1);	if (nY >= m_nHeight)nY = m_nHeight - 2 + ((nY - m_nHeight) & 1);
		return m_pData + nY*m_nWidth*m_nDim;
	}
	virtual unsigned short *GetPixelAt(int nX, int nY)
	{
		if (nX < 0)nX = ((-nX) & 1); if (nX >= m_nWidth)nX = m_nWidth - 2 + ((nX - m_nWidth) & 1);
		if (nY < 0)nY = ((-nY) & 1); if (nY >= m_nHeight)nY = m_nHeight - 2 + ((nY - m_nHeight) & 1);
		return m_pData + (nY*m_nWidth + nX)*m_nDim;
	}

	virtual bool HFlip()
	{
		if (!CBasicImageArray_WORD::HFlip())return false;

		m_nCFAPattern ^= 1;

		return true;
	}
	virtual bool VFlip()
	{
		if (!CBasicImageArray_WORD::VFlip())return false;

		m_nCFAPattern ^= 2;

		return true;
	}
	virtual bool Trans()
	{
		if(!CBasicImageArray_WORD::Trans())return  false;

		m_nCFAPattern = ((m_nCFAPattern >> 1)&1) | ((m_nCFAPattern & 1) << 1);

		return true;
	}
	virtual bool Crop(int nLeft_Margin, int nTop_Margin, int nRight_Margin, int nBottom_Margin)
	{
		int x, y;

		x = (nLeft_Margin & 1);
		y = (nTop_Margin & 1);
		nRight_Margin = ((nRight_Margin >> 1) << 1) | x;
		nBottom_Margin = ((nBottom_Margin >> 1) << 1) | y;
		if (!CBasicImageArray_WORD::Crop(nLeft_Margin, nTop_Margin, nRight_Margin, nBottom_Margin))return false;

		m_nCFAPattern ^= (y << 1) | x;
		printf("New CFAPattern=%d\n", m_nCFAPattern);

		return true;
	}

	bool Bayer2BGGRImage(CMultipleChannelImage *pBGGRImage);
	bool BGGR2BayerImage(CMultipleChannelImage *pBGGRImage, int nCFAPattern=0);
	bool Bayer2BGRHImage(CMultipleChannelImage *pBGRHImage);
	bool BGRH2BayerImage(CMultipleChannelImage *pBGRHImage, int nCFAPattern=0);

	bool Normalize(int nNewBit);
};

#endif
