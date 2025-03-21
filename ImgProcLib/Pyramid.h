#ifndef __PYRAMID_H_
#define __PYRAMID_H_

#include "../ImgLib/Filter.h"

class CPyramid
{
public:
	virtual bool DownScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage)=0;
	virtual bool DownScaleImage(CGrayImage *pInImge, CGrayImage *pOutImage)=0;
	virtual bool DownScaleImage(CYUV420Image *pInImage, CRGBYUVImage *pOutImage) = 0;
	virtual bool UpScaleImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage)=0;
	virtual bool UpScaleImage(CGrayImage *pInImage, CGrayImage *pOutImage)=0;
	virtual bool UpScaleImage(CRGBYUVImage *pInImage, CYUV420Image *pOutImage) = 0;

	virtual bool SubtractEdge(CRGBYUVImage *pInImage0, CRGBYUVImage *pInImage1, CMultipleChannelImage *pOutImage);
	virtual bool AddBackEdge(CRGBYUVImage *pInImage, CMultipleChannelImage *pEdgeImage, CRGBYUVImage *pOutImage);

	virtual bool SubtractEdge(CGrayImage *pInImage0, CGrayImage *pInImage1, CMultipleChannelImage *pOutImage);
	virtual bool AddBackEdge(CGrayImage *pInImage, CMultipleChannelImage *pEdgeImage, CGrayImage *pOutImage);

	virtual bool SubtractEdge(CYUV420Image *pInImage0, CYUV420Image *pInImage1, CMultipleChannelImage *pOutImage);
	virtual bool AddBackEdge(CYUV420Image *pInImage, CMultipleChannelImage *pEdgeImage, CYUV420Image *pOutImage);
};

#endif

