#ifndef __BYTE_IMAGE_ARRAY_H_
#define __BYTE_IMAGE_ARRAY_H_

#include "BasicImageArray.h"

class CByteImageArray : public CBasicImageArray_BYTE
{
protected:
	void RotateLine(BYTE *pInLine, BYTE *pOutLine, int nDim, int nWidth, int nInPitch, int nOutPitch);

	void HFilterLine(BYTE *pInLine, short *pOutLine, int nWidth, int nMask[], int nWinW, int nScale, int nInPitch, int nOutPitch);
	void VFilterLine(short *pInLines[], BYTE *pOutLine, int nWidth, int nMask[], int nWinW, int nScale, int nOffset, int nInPitch, int nOutPitch);

	void Filter2DLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nMask2D[], int nWinW, int nWinH, int nScale, int nOffset, int nInPitch);
public:
	bool HVFilterChannel(int nChannel, int nHMask[], int nHWinW, int nHScale, int nVMask[], int nVWinW, int nVScale, int nOffset = 0);
	bool Filter2DChannel(int nChannel, int nMask2D[], int nWinW, int nWinH, int nScale, int nOffset = 0);
};

#endif

