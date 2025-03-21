#ifndef __DUAL_CAMERA_FAST_AF_FILTER_H__
#define __DUAL_CAMERA_FAST_AF_FILTER_H__

#include "../ImgLib/Filter.h"
#include "../MathLib/Matrix.h"
#include "../MathLib/Vector.h"
#include "../MathLib/DualCameraGeometry.h"
#include "../ImgProcLib/HomographMappingFilter.h"
#include "../ImgProcLib/GaussianPyramid.h"


class CDualCameraFastAFContrastFilter :public CFilter
{
private:

public:
	int m_nContrastValue;
	int m_nWinWidth;
	int m_nWinHeight;

	double m_fDiffX;
	double m_fDiffY;
	double m_fSobel;
	double m_fTV1DHor;
	double m_fTV1DVer;
	double m_fHorSD1D;
	double m_fVerSD1D;
	double m_fMeanVal;
	
	CDualCameraFastAFContrastFilter()
	{
		Initialize("CDualCameraFastAFContrastFilter");

	}
	virtual bool InitParamList()
	{
		return true;
	}
	virtual bool UpdateInternalParam()
	{
		return true;
	}

	void Statistics(CGrayImage *pImage);
};

class CDualCameraFastAFMatchFilter :public CMultipleFilterProcessFlow
{
private:
	int m_bDumpImages;
	int m_nInitMatchLevel;
	
	int m_nWinFilter[16];
	int m_nWinFilterSize;
	int m_nFilterSumShift;
	int m_nPeakPercentTh;
	int m_nLocalMaxBoxX;
	int m_nLocalMaxBoxY;
	int m_nInitOffset[2];
	int m_nSearchRange[8][2];
	
	int m_nCorrTh[8];
	int m_nWinHalfWidth;

public:
	float m_fScaleToMainRef;
	int m_nDACScaleList[9];
	float m_fScaleList[9];
	float m_fRotationAngle[3];

	//int m_nDACList[16];
	//int m_nDispList[16];


public:
	CGrayImage m_MainYPyramid[8];
	CGrayImage m_SecondYPyramid[8];
	int m_nFocusXY[2];
	int m_nMatchXY[2];
	float m_fImageCenterMain[2];
	float m_fImageCenterSecond[2];
	//float m_nFullSizeMainCenter[2]

	CDualCameraFastAFMatchFilter()
	{
		Initialize("CDualCameraFastAFMatchFilter");
	}

	bool BuildMainPyramid(CYUV420Image *pMainYUVImage, int nFocusPos[2], int nFocusWin);
	bool BuildSecondPyramid(CYUV420Image *pSecondYUVImage, float fScale, float fAngleX, float fAngleY, float fAngleZ);
	bool FindMatch();
	double SearchMaxCorrelation(CGrayImage *pImage0, CGrayImage *pImage1, int &nOffsetX, int &nOffsetY, int nRangeX, int nRangeY, bool &bFoundMultiple);
	double ComputeCorrelation(CGrayImage *pInImage0, CGrayImage *pInImage1, int nOffsetX, int nOffsetY);

protected:
	CHomographMappingFilter m_HomographMappingFilter;

	virtual bool CreateFilterList()
	{
		AddFilter(&m_HomographMappingFilter);
		return true;
	}
	virtual bool InitParamList()
	{
		m_ParamList.Add("bDumpImages", &m_bDumpImages, 0, 1);
		m_bDumpImages = 1;
		m_ParamList.Add("nInitMatchLevel", &m_nInitMatchLevel, 0, 7);
		m_nInitMatchLevel = 2;
		m_ParamList.Add("nWinFilterSize", &m_nWinFilterSize, 1, 15);
		m_nWinFilterSize = 5;
		m_ParamList.Add("nFilterSumShift", &m_nFilterSumShift, 1, 8);
		m_nFilterSumShift = 4;

		m_ParamList.AddVector("nWinFilter", m_nWinFilter, 16, 0, 65536);
		m_nWinFilter[0] = 1; m_nWinFilter[1] = 4; m_nWinFilter[2] = 6; m_nWinFilter[3] = 4; m_nWinFilter[4] = 1;

		m_ParamList.Add("nPeakPercentTh", &m_nPeakPercentTh, 0, 100);
		m_nPeakPercentTh = 85;


		m_ParamList.Add("nLocalMaxBoxX", &m_nLocalMaxBoxX, 0, 100);
		m_nLocalMaxBoxX = 3;
		m_ParamList.Add("nLocalMaxBoxY", &m_nLocalMaxBoxY, 0, 100);
		m_nLocalMaxBoxY = 3;

		m_ParamList.AddVector("nInitOffset", m_nInitOffset, 2,-65535, 65535,"init offset on the match level");
		m_nInitOffset[0] = 0; m_nInitOffset[1] = 0;

		for (int i = 0; i < 8; i++)
		{
			m_ParamList.AddMatrixRow("nSearchRange", m_nSearchRange[i], i, 2, 0, 1000);
			m_nSearchRange[i][0] = 5; m_nSearchRange[i][1] = 5;
		}
		m_nSearchRange[0][0] = 200;
		m_nSearchRange[0][1] = 100;

		m_ParamList.AddVector("nCorrTh", m_nCorrTh, 8, 0, 100);
		
		for (int i = 0; i < 8; i++)
			m_nCorrTh[i] = 85;

		m_fScaleToMainRef = 1.0f;

		//m_ParamList.AddVector("nDACList", m_nDACList, 16, -1, 1024);
		//for (int i = 0; i < 16; i++)m_nDACList[i] = -1;
		//m_ParamList.AddVector("nDispList", m_nDispList, 16, -16384, 16383, "Disparity (x0.1 pixels)");
		//for (int i = 0; i < 16; i++)m_nDispList[i] = -16384;

		return true;
	}
	virtual bool UpdateInternalParam()
	{
		return true;
	}
	void DownSampleImage(CGrayImage *pImage);
	void FilterLine(unsigned char *pInLine, unsigned short *pOutLine, int nWidth, int pFilter[], int nFilterSize);
	bool ExtractYImage(CYUV420Image *pInImage, CGrayImage *pOutImage);
	bool ExtendImage(CGrayImage *pInImage, CGrayImage *pOutImage, int nS);
};

//class CDualCameraFastAFControlFilter :public CMultipleFilterProcessFlow
//{
//	int m_nCurrentDAC;
//	int m_nControlStatus;
//	int m_nDACList[16];
//	int m_nDisparityList[16];
//	int m_nNextDAC;
//	int m_nScaleList[16];
//	int m_nReferenceDAC;
//	
//private:
//
//public:
//	CDualCameraFastAFControlFilter()
//	{
//		Initialize("CDualCameraFastAFControlFilter");
//	}
//protected:
//	virtual bool CreateFilterList()
//	{
//		return true;
//	}
//	virtual bool InitParamList()
//	{
//		return true;
//	}
//	virtual bool UpdateInternalParam()
//	{
//		return true;
//	}
//
//};


class CDualCameraFastAFFilter :public CMultipleFilterProcessFlow
{
private:
	int m_nCurrentDAC;
	//bool m_bControlStatus;
	
	//bool m_b
	int m_nDACDisparityList[16];
	int m_nDisparityList[16];


	int m_nDACScaleList[16];
	int m_nScaleList[16];
	int m_nReferenceDAC;
	int m_nReferenceDisparity;
	int m_bNeedMatch;

	int m_nDisparityDACStep;

public:	

	int m_nFrameCount = 0;
	double m_fContrast[50];
	int m_fDacValue[50];

	bool m_bDisparityStable;
	int m_nNextDAC;
	enum AF_STATUS
	{
		//AF_DONE,
		AF_NOT_COMPLETE=0,
		AF_SUCCESS=1,
		AF_FAIL=2
	
	};
	CDualCameraFastAFMatchFilter m_MatchFilter;
	CDualCameraFastAFContrastFilter m_ContrastStatFilter;
	//CDualCameraFastAFControlFilter m_ControlFilter;


	bool Estimate();

	CDualCameraFastAFFilter()
	{
		Initialize("CDualCameraFastAFFlow");
	}

	bool ProcessImagePair(CYUV420Image *pMainYUVImage, CYUV420Image *pSecondYUVImage, int nFocusPos[2], int nFocusWin, int nDAC);// , float fAngleX, float fAngleY, float fAngleZ);

	int DSIP2DAC(float disp);

	__inline float interp(int nList[9], float fList[9], int nVal)
	{
		float a;
		if (nVal < nList[0])nVal = nList[0];
		if (nVal > nList[8])nVal = nList[8];

		for (int i = 0; i < 8; i++)
		{
			float prod = (nVal - nList[i])*(nVal - nList[i + 1]);
			if (prod <= 0)
			{
				float w = ((float)(nVal - nList[i])) / (nList[i + 1] - nList[i]);
				a = w*fList[i + 1] + (1 - w)*fList[i];
			}
		}
		return a;
	}

protected:
	virtual bool CreateFilterList()
	{
		AddFilter(&m_MatchFilter);
		AddFilter(&m_ContrastStatFilter);
		return true;
	}
	virtual bool InitParamList()
	{

		m_ParamList.AddVector("nDAC_DisparityList", m_nDACDisparityList, 16, -1, 1024);
		for (int i = 0; i < 16; i++)m_nDACDisparityList[i] = -1;
		m_ParamList.AddVector("nDisparityList", m_nDisparityList, 16, -16384, 16383, "Disparity (x0.1 pixels)");
		for (int i = 0; i < 16; i++)m_nDisparityList[i] = -16384;
		m_ParamList.AddVector("nDAC_ScaleList", m_nDACScaleList, 16, -1, 1024);
		for (int i = 0; i < 16; i++)m_nDACScaleList[i] = -1;
		m_ParamList.AddVector("nScaleList", m_nScaleList, 16, -1, 16383, "Scale (x0.0001)");
		for (int i = 0; i < 16; i++)m_nScaleList[i] = 10000;
		m_ParamList.Add("nDisparityDACStep", &m_nDisparityDACStep, 0, 16);
		m_nDisparityDACStep = 8;

		m_bNeedMatch = true;
		return true;
	}
	virtual bool UpdateInternalParam()
	{
		return true;
	}	
};


#endif
