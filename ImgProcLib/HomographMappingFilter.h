#ifndef __HOMOGRAPH_MAPPING_FILTER_H_
#define __HOMOGRAPH_MAPPING_FILTER_H_

#include "../ImgLib/Filter.h"
#include "../mathlib/Matrix.h"

class CHomographMappingFilter : public CFilter
{
protected:

	__inline float CubicInterpolate(int In[], float f)
	{
		float fa[4];
		float f2 = f*f;
		float f3 = f2*f;

		fa[0] = (float) In[1];
		fa[1] = (float) ((-2 * In[0] - 3 * In[1] + 6 * In[2] - In[3]) / 6.0);
		fa[2] = (float) ((In[0] - 2 * In[1] + In[2]) / 2.0);
		fa[3] = (float) ((-In[0] + 3 * In[1] - 3 * In[2] + In[3]) / 6.0);

		return fa[3] * f3 + fa[2] * f2 + fa[1] * f + fa[0];
	}
	__inline float LinearInterpolate(int In[], float f)
	{
		float f1 = (float)In[0];
		float f2 = (float)In[1];
		
		return (float) (f1*(1.0 - f) + f2*f);
	}
	void BicubicMappingImage(BYTE *pInImage, int nInWidth, int nInHeight, BYTE *pOutImage, int nOutWidth, int nOutHeight, int nDim, CMatrix &H);
	void BilinearMappingImage(BYTE *pInImage, int nInWidth, int nInHeight, BYTE *pOutImage, int nOutWidth, int nOutHeight, int nDim, CMatrix &H);

	virtual bool InitParamList()
	{
		m_ParamList.Add("bBicubicEnable", &m_bBicubicEnable, 0, 1);
		m_bBicubicEnable = 1;
		
		return true;
	}
public:
	int m_bBicubicEnable;

	CHomographMappingFilter()
	{
		Initialize("CHomographMappingFilter");
	}

	void ProcessImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, CMatrix &H);
	void ProcessImage(CGrayImage *pInImage, CGrayImage *pOutImage, CMatrix &H);
	void ProcessImage(CYUV420Image *pInImage, CYUV420Image *pOutImage, CMatrix &H);
};

#endif
