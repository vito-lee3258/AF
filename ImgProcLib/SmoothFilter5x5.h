#ifndef __SMOOTH_FILTER5X5_H_
#define __SMOOTH_FILTER5X5_H_

#include "../ImgLib/Filter.h"

class CSmoothFilter5x5
{
protected:
	virtual void HSmoothFilterLine(BYTE *pInLine, WORD *pOutLine, int nWidth, int nPitch, int nDim);
	virtual void VSmoothFilterLine(WORD *pInLines[], BYTE *pOutLine, int nE[], int nWidth, int nPitch, int nDim, bool bDitheringEnable);

public:
	virtual bool SmoothYUVImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, bool bDitheringEnable = true);
	virtual bool SmoothYImage(CGrayImage *pInImage, CGrayImage *pOutImage, bool bDitheringEnable = true);
};

#endif

