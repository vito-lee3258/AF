#ifndef __DOWN_SCALE_FILTER_H_
#define __DOWN_SCALE_FILTER_H_

#include "../ImgLib/Filter.h"

class CDownScaleFilter : public CFilter
{
protected:
	double m_fXScale;
	double m_fYScale;

	virtual bool DownScaleImage(BYTE *pInImage, BYTE *pOutImage, int nDim, int nInWidth, int nInHeight, int nOutWidth, int nOutHeight) = 0;
public:

	bool ProcessImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, double fScale, int S=0)
	{
//		if (fScale < 1.0)fScale = 1.0;
		if (pOutImage == pInImage)return false;

		int nInWidth = pInImage->GetWidth();
		int nInHeight = pInImage->GetHeight();
		int nOutWidth = (int) (nInWidth/fScale+0.5);
		int nOutHeight = (int)(nInHeight/fScale+0.5);

		m_fXScale = (double)nInWidth / nOutWidth;
		m_fYScale = (double)nInHeight / nOutHeight;

		nOutWidth = (nOutWidth >> S) << S;
		nOutHeight = (nOutHeight >> S) << S;
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;

		return DownScaleImage(pInImage->GetImageData(), pOutImage->GetImageData(), 3, nInWidth, nInHeight, nOutWidth, nOutHeight);
	}

	bool ProcessImage(CGrayImage *pInImage, CGrayImage *pOutImage, double fScale, int S = 0)
	{
//		if (fScale < 1.0)fScale = 1.0;
		if (pOutImage == pInImage)return false;

		int nInWidth = pInImage->GetWidth();
		int nInHeight = pInImage->GetHeight();
		int nOutWidth = (int)(nInWidth / fScale + 0.5);
		int nOutHeight = (int)(nInHeight / fScale + 0.5);

		m_fXScale = (double)nInWidth / nOutWidth;
		m_fYScale = (double)nInHeight / nOutHeight;

		nOutWidth = (nOutWidth >> S) << S;
		nOutHeight = (nOutHeight >> S) << S;
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;

		return DownScaleImage(pInImage->GetImageData(), pOutImage->GetImageData(), 1, nInWidth, nInHeight, nOutWidth, nOutHeight);
	}

	bool ProcessImage(CYUV420Image *pInImage, CYUV420Image *pOutImage, double fScale, int S = 0)
	{
//		if (fScale < 1.0)fScale = 1.0;
		if (pOutImage == pInImage)return false;

		int nInWidth = pInImage->GetWidth();
		int nInHeight = pInImage->GetHeight();
		int nOutWidth = (int)(nInWidth / fScale + 0.5);
		int nOutHeight = (int)(nInHeight / fScale + 0.5);

		nOutWidth = (nOutWidth >> 1) << 1;
		nOutHeight = (nOutHeight >> 1) << 1;
		m_fXScale = (double)nInWidth / nOutWidth;
		m_fYScale = (double)nInHeight / nOutHeight;

		nOutWidth = (nOutWidth >> S) << S;
		nOutHeight = (nOutHeight >> S) << S;
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;

		if (!DownScaleImage(pInImage->GetYImage(), pOutImage->GetYImage(), 1, nInWidth, nInHeight, nOutWidth, nOutHeight))return false;

		nInWidth >>= 1;
		nInHeight >>= 1;
		nOutWidth >>= 1;
		nOutHeight >>= 1;

		if (!DownScaleImage(pInImage->GetUVImage(), pOutImage->GetUVImage(), 2, nInWidth, nInHeight, nOutWidth, nOutHeight))return false;
		
		return true;
	}
};

class CBicubicDownScaleFilter : public CDownScaleFilter
{
protected:
	__inline double Interpolate(int In[], double f)
	{
		double fa[4];
		double f2 = f*f;
		double f3 = f2*f;

		fa[0] = In[1];
		fa[1] = (-2 * In[0] - 3 * In[1] + 6 * In[2] - In[3]) / 6.0;
		fa[2] = (In[0] - 2 * In[1] + In[2]) / 2.0;
		fa[3] = (-In[0] + 3 * In[1] - 3 * In[2] + In[3]) / 6.0;

		return fa[3] * f3 + fa[2] * f2 + fa[1] * f + fa[0];
	}

	void VDownScaleLine(BYTE *pInLines[], BYTE *pOutLine, int nDim,  int nWidth, double fdY);
	void HDownScaleLine(BYTE *pInLine, BYTE *pOutLine, int nDim, int nInWidth, int nOutWidth);

	virtual bool DownScaleImage(BYTE *pInImage, BYTE *pOutImage, int nDim, int nInWidth, int nInHeight, int nOutWidth, int nOutHeight);

	virtual bool InitParamList()
	{
		return true;
	}
public:
	CBicubicDownScaleFilter()
	{
		Initialize("CBicubicDownScaleFilter");
	}
};

#endif
