#ifndef __ZOOMIN_FILTER_H_
#define __ZOOMIN_FILTER_H_

#include "../ImgLib/Filter.h"

class CZoomInFilter : public CFilter
{
protected:
	double m_fCenterX;
	double m_fCenterY;
	
	double m_fZoomRect_Left;
	double m_fZoomRect_Top;
	double m_fZoomRect_Width;
	double m_fZoomRect_Height;

	double m_fStartX;
	double m_fStartY;

	virtual bool ZoomIn(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage) = 0;
public:
	
	bool ProcessImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, double fZoomInScale)
	{
		int nWidth = pInImage->GetWidth();
		int nHeight = pInImage->GetHeight();
		
		if (fZoomInScale < 1)fZoomInScale = 1;
		if (pOutImage == pInImage)return false;
		if (!pOutImage->Create(nWidth, nHeight))return false;

		m_fCenterX = (double)(nWidth - 1) / 2.0;
		m_fCenterY = (double)(nHeight - 1) / 2.0;

		m_fZoomRect_Width = (double)nWidth / fZoomInScale;
		m_fZoomRect_Height = (double)nHeight / fZoomInScale;

		m_fZoomRect_Left = m_fCenterX - m_fZoomRect_Width / 2.0;
		m_fZoomRect_Top = m_fCenterY - m_fZoomRect_Height / 2.0;

		m_fStartX = m_fZoomRect_Left + (m_fZoomRect_Width / nWidth) / 2.0;
		m_fStartY = m_fZoomRect_Top + (m_fZoomRect_Height / nHeight) / 2.0;

		return ZoomIn(pInImage, pOutImage);
	}
};

class CBicubicZoomInFilter : public CZoomInFilter
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

	void VZoomInLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, double fdY);
	void HZoomInLine(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	virtual bool ZoomIn(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage);

	virtual bool InitParamList()
	{
		return true;
	}
public:
	CBicubicZoomInFilter()
	{
		Initialize("CBicubicZoomInFilter");
	}
};

#endif
