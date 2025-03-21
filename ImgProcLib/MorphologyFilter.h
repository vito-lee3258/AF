#ifndef __MORPHOLOGY_FILTER_H_
#define __MORPHOLOGY_FILTER_H_

#include "../ImgLib/Filter.h"

class CMorphologyFilter
{
protected:
	void RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch);
	void FillLine(BYTE *pOutLine, BYTE nVal, int nWidth, int nPitch);

	void HErosion3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void VErosion3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void HDilation3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void VDilation3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void HMin3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void VMin3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void HMax3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);
	void VMax3Line(BYTE *pInLine, BYTE *pOutLine, int nWidth);

	void Erosion3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth);
	void Min3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth);
	void Dilation3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth);
	void Max3x3Line(BYTE *pInLines[], BYTE *pOutLine, int nWidth);

public:
	virtual bool BinarizeImageGT(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre);
	virtual bool BinarizeImageLT(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre);
	virtual bool BinarizeImageRange(CGrayImage *pInImage, CGrayImage *pOutImage, int nThre1, int nThre2);
	virtual bool InverseImage(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool AND2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage);
	virtual bool OR2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage);
	virtual bool XOR2Image(CGrayImage *pInImage1, CGrayImage *pInImage2, CGrayImage *pOutImage);

	virtual bool Erosion3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Dilation3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Min3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Max3x3_Square(CGrayImage *pInImage, CGrayImage *pOutImage);

	virtual bool Erosion3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Dilation3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Min3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage);
	virtual bool Max3x3_Diamond(CGrayImage *pInImage, CGrayImage *pOutImage);

	virtual bool Erosion5x5Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 1 1 1 0
		1 1 1 1 1
		1 1 1 1 1
		1 1 1 1 1
		0 1 1 1 0
		*/

		if (!Erosion3x3_Square(pInImage, pOutImage))return false;
		if (!Erosion3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Erosion7x7Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 0 0
		0 1 1 1 1 1 0
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		0 1 1 1 1 1 0
		0 0 1 1 1 0 0
		*/

		if (!Erosion3x3_Square(pInImage, pOutImage))return false;
		if (!Erosion3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Erosion3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Erosion9x9Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 1 1 0 0
		0 1 1 1 1 1 1 1 0
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		0 1 1 1 1 1 1 1 0
		0 0 1 1 1 1 1 0 0
		*/

		if (!Erosion3x3_Square(pInImage, pOutImage))return false;
		if (!Erosion3x3_Square(pOutImage, pOutImage))return false;
		if (!Erosion3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Erosion3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Min5x5Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 1 1 1 0
		1 1 1 1 1
		1 1 1 1 1
		1 1 1 1 1
		0 1 1 1 0
		*/

		if (!Min3x3_Square(pInImage, pOutImage))return false;
		if (!Min3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Min7x7Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 0 0
		0 1 1 1 1 1 0
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		0 1 1 1 1 1 0
		0 0 1 1 1 0 0
		*/

		if (!Min3x3_Square(pInImage, pOutImage))return false;
		if (!Min3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Min3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Min9x9Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 1 1 0 0
		0 1 1 1 1 1 1 1 0
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		0 1 1 1 1 1 1 1 0
		0 0 1 1 1 1 1 0 0
		*/

		if (!Min3x3_Square(pInImage, pOutImage))return false;
		if (!Min3x3_Square(pOutImage, pOutImage))return false;
		if (!Min3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Min3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}

	virtual bool Dilation5x5Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 1 1 1 0
		1 1 1 1 1
		1 1 1 1 1
		1 1 1 1 1
		0 1 1 1 0
		*/

		if (!Dilation3x3_Square(pInImage, pOutImage))return false;
		if (!Dilation3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Dilation7x7Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 0 0
		0 1 1 1 1 1 0
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		0 1 1 1 1 1 0
		0 0 1 1 1 0 0
		*/

		if (!Dilation3x3_Square(pInImage, pOutImage))return false;
		if (!Dilation3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Dilation3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Dilation9x9Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 1 1 0 0
		0 1 1 1 1 1 1 1 0
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		0 1 1 1 1 1 1 1 0
		0 0 1 1 1 1 1 0 0
		*/

		if (!Dilation3x3_Square(pInImage, pOutImage))return false;
		if (!Dilation3x3_Square(pOutImage, pOutImage))return false;
		if (!Dilation3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Dilation3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}

	virtual bool Max5x5Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 1 1 1 0
		1 1 1 1 1
		1 1 1 1 1
		1 1 1 1 1
		0 1 1 1 0
		*/

		if (!Max3x3_Square(pInImage, pOutImage))return false;
		if (!Max3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Max7x7Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 0 0
		0 1 1 1 1 1 0
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		1 1 1 1 1 1 1
		0 1 1 1 1 1 0
		0 0 1 1 1 0 0
		*/

		if (!Max3x3_Square(pInImage, pOutImage))return false;
		if (!Max3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Max3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
	virtual bool Max9x9Image(CGrayImage *pInImage, CGrayImage *pOutImage)
	{
		/*
		Mask
		0 0 1 1 1 1 1 0 0
		0 1 1 1 1 1 1 1 0
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		1 1 1 1 1 1 1 1 1
		0 1 1 1 1 1 1 1 0
		0 0 1 1 1 1 1 0 0
		*/

		if (!Max3x3_Square(pInImage, pOutImage))return false;
		if (!Max3x3_Square(pOutImage, pOutImage))return false;
		if (!Max3x3_Diamond(pOutImage, pOutImage))return false;
		if (!Max3x3_Diamond(pOutImage, pOutImage))return false;

		return true;
	}
};

#endif
