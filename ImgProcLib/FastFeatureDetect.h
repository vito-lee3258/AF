#ifndef __FAST_FEATURE_DETECT_H_
#define __FAST_FEATURE_DETECT_H_

#include "../ImgLib/Filter.h"

typedef struct tagFeaturePoint
{
	int nX;
	int nY;
	int nScore;
}TFeaturePoint;

class CFastFeatureDetection: public CFilter
{
protected:
	void RotateLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nInPitch, int nOutPitch);

	int m_nYWin[7][7];
	int InitDetectWindow();
	void InitDetectLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nInPitch);

	int m_nTotalNum;
	int m_nHist[256];

	int NonMaxSupressionWindow();
	void NonMaxSupressionLine(BYTE *pInLines[], BYTE *pOutLine, int nWidth, int nInPitch);
	bool InitDetect(CRGBYUVImage *pInImage, CGrayImage *pOutImage, int nChannel);
	bool NonMaxSupression(CGrayImage *pImage);

	virtual bool InitParamList()
	{
		m_ParamList.Add("nPtThre1", &m_nPtThre1, 2, 3, "Feature Point Detect Threshold1");
		m_nPtThre1 = 3;
		m_ParamList.Add("nPtThre2", &m_nPtThre2, 8, 15, "Feature Point Detect Threshold1");
		m_nPtThre2 = 12;

		m_ParamList.Add("nThre", &m_nThre, 0, 255, "Threshold for absolute difference detection");
		m_ParamList.Add("nRatio", &m_nRatio, 0, 255, "Threshold for relative difference detection");
		m_nThre = 4;
		m_nRatio = 16;

		return true;
	}
public:
	int m_nPtThre1;
	int m_nPtThre2;
	int m_nThre;	//8
	int m_nRatio;	//25

	CFastFeatureDetection()
	{
		Initialize("CFastFeatureDetection");
	}

	bool ProcessImage(CRGBYUVImage *pInImage, int nChannel, int nMaxNum, TFeaturePoint pPtList[], int &nNum);
	bool MarkFeaturePoint(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, int nChannel, TFeaturePoint pPtList[], int nNum);
};

#endif

