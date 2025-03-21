#ifndef __GAUSSIAN_PYRAMID_H_
#define __GAUSSIAN_PYRAMID_H_

#include "Pyramid.h"

class CGaussianPyramid: public CPyramid
{
protected:
	virtual void RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nDim, int nInPitch, int nOutPitch);
	virtual void RotateLine(short *pInLine, short *pOutLine, int nWidth, int nDim, int nInPitch, int nOutPitch);

	int m_nDownScaleDitheringE[8];
	int m_nUpScaleDitheringE[8];

	int m_nWin[5][5][8];
	int m_nVBuf[8][5];
	virtual __inline void DownScaleVFilter(int nX, int nDim)
	{
		int i;

		for (i = 0; i < nDim; i++)
		{
			m_nVBuf[i][nX] = (m_nWin[0][nX][i] + m_nWin[4][nX][i]);
			m_nVBuf[i][nX]+= (m_nWin[1][nX][i] + m_nWin[3][nX][i])*4;
			m_nVBuf[i][nX]+= m_nWin[2][nX][i]*6;
			m_nVBuf[i][nX]/= 4;
		}
	}
	virtual __inline int DownScaleHFilter(int ch)
	{
		int Out = (m_nVBuf[ch][0] + m_nVBuf[ch][4]);
		Out += (m_nVBuf[ch][1] + m_nVBuf[ch][3]) * 4;
		Out += m_nVBuf[ch][2] * 6;
		
		if (m_bDitheringEnable)
		{
			Out += m_nDownScaleDitheringE[ch];
			m_nDownScaleDitheringE[ch] = (Out & 63);
			Out >>= 6;
		}
		else
		{
			Out = (Out + 32) / 64;
		}

		return Out;
	}

	virtual void DownScaleWindow(int Out[], int nDim);
	virtual void StartDownScaleLine(int nDim);
	virtual void DownScaleLine(BYTE *pInLines[], BYTE *pOutLine, int nInWidth, int nDim, int nInPitch, int nOutPitch);
	virtual void DownScaleLine(short *pInLines[], short *pOutLine, int nInWidth, int nDim, int nPitch);

	__inline void UpScaleVFilter(int nX, int bYFlag, int nDim)
	{
		int i;

		if (bYFlag == 0)
		{
			for (i = 0; i < nDim; i++)
			{
				m_nVBuf[i][nX] = (m_nWin[0][nX][i] + m_nWin[1][nX][i] * 6 + m_nWin[2][nX][i])/4;
			}
		}
		else
		{
			for (i = 0; i < nDim; i++)
			{
				m_nVBuf[i][nX] = (m_nWin[1][nX][i] + m_nWin[2][nX][i]);
			}
		}
	}
	virtual __inline int UpScaleHFilter(int bXFlag, int ch)
	{
		int Out;

		if (bXFlag == 0)
		{
			Out = (m_nVBuf[ch][0] + m_nVBuf[ch][1] * 6 + m_nVBuf[ch][2])/4;
		}
		else
		{
			Out = (m_nVBuf[ch][1] + m_nVBuf[ch][2]);
		}

		if (m_bDitheringEnable)
		{
			Out += m_nUpScaleDitheringE[ch];
			m_nUpScaleDitheringE[ch] = (Out & 3);
			Out >>= 2;
		}
		else
		{
			Out /= 4;
		}

		return Out;
	}

	virtual void UpScaleWindow(int bXFlag, int bYFlag, int Out[], int nDim);
	virtual void StartUpScaleLine(int bYFlag, int nDim);
	virtual void UpScaleLine(BYTE *pInLines[], BYTE *pOutLine, int bYFlag, int nWidth, int nDim, int nInPitch, int nOutPitch);
	virtual void UpScaleLine(short *pInLines[], short *pOutLine, int bYFlag, int nWidth, int nDim, int nPitch);

	virtual bool ExtendImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage, int nS);
	virtual bool ExtendImage(CGrayImage *pInImage, CGrayImage *pOutImage, int nS);
	virtual bool ExtendImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, int nS);
	virtual bool ExtendImage(CYUV420Image *pInImage, CYUV420Image *pOutImage, int nS);

public:
	bool m_bDitheringEnable;

	virtual bool DownScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage);
	virtual bool DownScaleImage(CGrayImage *pInImge, CGrayImage *pOutImage);
	virtual bool DownScaleImage(CYUV420Image *pInImage, CRGBYUVImage *pOutImage);
	virtual bool UpScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage);
	virtual bool UpScaleImage(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool UpScaleImage(CRGBYUVImage *pInImage, CYUV420Image *pOutImage);

	virtual bool DownScaleImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage);
	virtual bool UpScaleImage(CMultipleChannelImage *pInImage, CMultipleChannelImage *pOutImage);
};

#endif
