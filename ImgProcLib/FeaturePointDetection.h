#ifndef __FEATURE_POINT_DETECTION_H_
#define __FEATURE_POINT_DETECTION_H_

#include "../ImgLib/Filter.h"

typedef struct tagFeaturePoint
{
	float fX;
	float fY;
	float fScore;
	int nFlag;
}TFeaturePoint;

class CFeaturePointDetection : public CFilter
{
protected:
	void RotateBLine(BYTE *pInLine, BYTE *pOutLine, int nWidth, int nPitch);
	void RotateSLine(float *pInLine, float *pOutLine, int nWidth, int nPitch);

	int m_nYWin[7][7];
	bool FastDetectCorner();
	float ComputeHarrisCorner();
	void ComputeCornernessLine(BYTE *pInLines[], float *pOutLine, int nWidth, int nPitch);
	void ComputeCornernessLine(BYTE *pInLines[], float *pOutLine, int nWidth, int nLeft, int nRight, int nPitch);
	bool ComputeCornernessImage(CGrayImage *pInImage, CBasicImageArray_FLOAT *pOutImage);
	bool ComputeCornernessImage(CGrayImage *pInImage, CBasicImageArray_FLOAT *pOutImage, int nLeft, int nTop, int nRight, int nBottom);

	float m_fHarris_K;
	float m_fYWin[7][7];
	bool NonMaximumWindow(float &fX, float &fY, float &max);
	void NonMaximumLine(float *pInLines[], int nY, int nWidth, int nPitch, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum);
	bool NonMaximumImage(CBasicImageArray_FLOAT *pImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum);

	virtual bool InitParamList()
	{
		m_ParamList.Add("nPtThre1", &m_nPtThre1, 2, 3, "Feature Point Detect Threshold1");
		m_nPtThre1 = 2;
		m_ParamList.Add("nPtThre2", &m_nPtThre2, 8, 15, "Feature Point Detect Threshold1");
		m_nPtThre2 = 8;

		m_ParamList.Add("nThre", &m_nThre, 0, 255, "Threshold for absolute difference detection");
		m_ParamList.Add("nRatio", &m_nRatio, 0, 255, "Threshold for relative difference detection");
		m_nThre = 4;
		m_nRatio = 8;

		m_ParamList.Add("nHarris_K", &m_nHarris_K, 1, 10);
		m_nHarris_K = 5;

		m_ParamList.Add("nHarrisThre", &m_nHarrisThre, 1, 65536);
		m_nHarrisThre = 10000;

		return true;
	}
	virtual bool UpdateInternalParam()
	{
		m_fHarris_K = (float) (m_nHarris_K / 100.0);
		return true;
	}
public:
	int m_nRatio;
	int m_nThre;
	int m_nPtThre1;
	int m_nPtThre2;
	int m_nHarris_K;
	int m_nHarrisThre;

	CFeaturePointDetection()
	{
		Initialize("CFeaturePointDetection");
	}

	bool MarkFeaturePoint(char *pFileName, CGrayImage *pYImage, TFeaturePoint pPtList[], int nPtNum, int nS, int nC = 0);
	bool MarkFeaturePoint(char *pFileName, CGrayImage *pYImage, TFeaturePoint pPtList[], int nPtNum, int nS, int nLeft, int nTop, int nRight, int nBottom, int nC = 0);

	bool DetectFeaturePoint(CGrayImage *pInYImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum);
	bool DetectFeaturePoint(CGrayImage *pInYImage, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum, int nLeft, int nTop, int nRight, int nBottom);
	bool UpdateFeaturePoint(CGrayImage *pYImage, TFeaturePoint *pPtList, int nPtNum);

	bool PyramidDetectPoint(CGrayImage *pPyramid, int nDetectLevel, TFeaturePoint *pPtList, int &nPtNum, int nMaxNum);
};

#endif
