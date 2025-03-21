#include "CDualCameraFastAFFilter.h"


void PlotRect(CGrayImage *pImage, int nX, int nY, int nWinSize, char *filename)
{
	int x, y;
	CGrayImage tmp;
	tmp.Copy(pImage);
	for (x = -nWinSize; x <= nWinSize; x++)
	{
		*(tmp.GetPixelAt(x + nX, nY - nWinSize)) = 255;

		*(tmp.GetPixelAt(x + nX, nY + nWinSize) ) = 255;

	}


	for (y = -nWinSize; y <= nWinSize; y++)
	{
		*(tmp.GetPixelAt(nX - nWinSize, y+nY) ) = 255;

		*(tmp.GetPixelAt(nX + nWinSize, y + nY) ) = 255;
	}
	tmp.SaveToBMPFile(filename);
}


void CDualCameraFastAFContrastFilter::Statistics(CGrayImage *pImage)
{
	
	int w = pImage->GetWidth();
	int h = pImage->GetHeight();

	int nSX = 0, nSY=0;
	int nEX = w, nEY = h;


	//nSX = MAX2(0, MIN2(nSX, nWidth - 1));
	//nSY = MAX2(0, MIN2(nSY, nHeight - 1));

	//int nEX = MIN2(nWidth, nSX + nWidth);
	//int nEY = MIN2(nHeight, nSY + nHeight);

	int nWidth = nEX - nSX;
	int nHeight = nEY - nSY;

	__int64 sumXDiff = 0, sumYDiff = 0;

	int nContrast = 0;

	int *vstat = new int[nWidth];
	int *hstat = new int[nHeight];

	memset(vstat, 0, nWidth*sizeof(vstat[0]));
	memset(hstat, 0, nHeight*sizeof(hstat[0]));

	int x, y;

	for (y = nSY; y < nEY; y++)
	{
		BYTE *pLine[3] = { pImage->GetImageLine(y - 1), pImage->GetImageLine(y), pImage->GetImageLine(y + 1) };
		for (x = nSX; x < nEX; x++)
		{
			int MeanHor0, MeanHor2, MeanVer0, MeanVer2;

			int ix[3] = { MAX2(0,x - 1), x, MIN2(w - 1,x + 1) };

			vstat[x - nSX] += (int)pLine[1][ix[1]];
			hstat[y - nSY] += (int)pLine[1][ix[1]];
			sumXDiff += DIFF((int)pLine[1][1], (int)pLine[1][2]);
			sumYDiff += DIFF((int)pLine[1][1], (int)pLine[2][1]);

			MeanHor0 = (int)pLine[0][0] + 2 * (int)pLine[0][1] + (int)pLine[0][2];//up horizontal line
			MeanHor2 = (int)pLine[2][0] + 2 * (int)pLine[2][1] + (int)pLine[2][2];//down horizontal line

			MeanVer0 = (int)pLine[0][0] + 2 * (int)pLine[1][0] + (int)pLine[2][0];//left vertical col
			MeanVer2 = (int)pLine[0][2] + 2 * (int)pLine[1][2] + (int)pLine[2][2];//right vertical col

			int diff = ABS(MeanHor0 - MeanHor2) + ABS(MeanVer0 - MeanVer2);
			nContrast += diff * 256 / (1 + (int)pLine[1][1]);
		}
	}

	m_fSobel = (double)nContrast;
	m_fDiffX = (double)sumXDiff;
	m_fDiffY = (double)sumYDiff;


	__int64 sumXVal = 0, sumYVal = 0;
	__int64 sumXSqr = 0, sumYSqr = 0;

	m_fTV1DHor = m_fTV1DVer = 0;

	for (x = 0; x < nWidth; x++)
	{
		sumXVal += vstat[x];
		sumXSqr += vstat[x] * vstat[x];
		if (x > 0)
			m_fTV1DHor += DIFF(vstat[x], vstat[x - 1]);
	}

	for (y = 0; y < nHeight; y++)
	{
		sumYVal += hstat[y];
		sumYSqr += hstat[y] * hstat[y];
		if (y > 0)
			m_fTV1DVer += DIFF(hstat[y], hstat[y - 1]);
	}

	m_fHorSD1D = sqrt((double)(sumXSqr*nWidth - sumXVal*sumXVal) / (nWidth*nWidth*nHeight*nHeight));
	m_fMeanVal = sumXVal / (nWidth*nHeight);
	m_fTV1DHor /= nHeight;

	m_fVerSD1D = sqrt((double)(sumYSqr*nHeight - sumYVal*sumYVal) / (nWidth*nWidth*nHeight*nHeight));
	m_fTV1DVer /= nWidth;

	delete[]vstat;
	delete[]hstat;
}

bool CDualCameraFastAFFilter::Estimate()
{
	return true;
}

int CDualCameraFastAFFilter::DSIP2DAC(float disp)
{
	int dac = -1;

	int mind = m_nDACDisparityList[0];
	int maxd = -1;
	int numd = 0;

	int i;
	for (i = 0; i < 16; i++)
	{
		if (m_nDACDisparityList[i] >= 0)numd++;
		else break;
	}

	maxd = m_nDACDisparityList[numd - 1];

	float maxdisp = m_nDisparityList[numd-1];
	float mindisp = m_nDisparityList[0];
	
	disp *= 10;

	if (disp > maxdisp)
		disp = maxdisp;
	if (disp < mindisp)
		disp = mindisp;

	for (i = 0; i < numd - 1; i++)
	{
		float prod = (disp - m_nDisparityList[i]) * (disp - m_nDisparityList[i + 1]);
		if (prod <= 0)
		{
			float w = (disp - m_nDisparityList[i]) / (m_nDisparityList[i + 1] - m_nDisparityList[i]);
			float fdac = w * m_nDACDisparityList[i + 1] + (1 - w)*m_nDACDisparityList[i];

			dac = int(fdac + 0.5);
			if (dac < mind)dac = mind;
			if (dac > maxd)dac = maxd;

			break;
		}
	}

	dac = (dac*m_nDisparityDACStep + m_nCurrentDAC*(16 - m_nDisparityDACStep) + 8) / 16;

	int nd = dac / 5 * 5;
	if (dac - nd >= 3) dac = nd + 5;
	else dac = nd;

	return dac;
}

bool CDualCameraFastAFFilter::ProcessImagePair(CYUV420Image *pMainYUVImage, CYUV420Image *pSecondYUVImage, int nFocusPos[2], int nFocusWin, int nDAC)//, float fAngleX, float fAngleY, float fAngleZ)
{
	m_nCurrentDAC = nDAC;

	bool bBuildMainPyramid = true;
	
	if (!m_MatchFilter.BuildMainPyramid(pMainYUVImage, nFocusPos, nFocusWin))
	{
		printf("Build main pyramid Fail\n");
		bBuildMainPyramid = false;
	}
	
	float fScaleAF2Ref= interp(m_MatchFilter.m_nDACScaleList, m_MatchFilter.m_fScaleList, nDAC);
	float fScaleRef2AF = 1.0f / fScaleAF2Ref;
	printf("%f,%f\n", fScaleRef2AF, m_MatchFilter.m_fScaleToMainRef);
	float fAngleX = m_MatchFilter.m_fRotationAngle[0];
	float fAngleY = m_MatchFilter.m_fRotationAngle[1];
	float fAngleZ = m_MatchFilter.m_fRotationAngle[2];

	bool bBuildSecondPyramid = true;
	//if (!m_MatchFilter.BuildSecondPyramid(pSecondYUVImage, fSecondScale, fAngleX, fAngleY, fAngleZ))
	if (!m_MatchFilter.BuildSecondPyramid(pSecondYUVImage, fScaleRef2AF, fAngleX, fAngleY, fAngleZ))
	{
		printf("Build second pyramid Fail\n");
		bBuildSecondPyramid = false;
	}

	m_ContrastStatFilter.Statistics(m_MatchFilter.m_MainYPyramid);
	printf("--WindowStat--\n");
	printf("m_fDiffX=%f\nm_fDiffY=%f\nm_fSobel=%f\nm_fTV1DHor=%f\nm_fTV1DVer=%f\nm_fHorSD1D=%f\nm_fVerSD1D=%f\nm_fMeanVal=%f\n", m_ContrastStatFilter.m_fDiffX, m_ContrastStatFilter.m_fDiffY, m_ContrastStatFilter.m_fSobel, m_ContrastStatFilter.m_fTV1DHor, m_ContrastStatFilter.m_fTV1DVer, m_ContrastStatFilter.m_fHorSD1D, m_ContrastStatFilter.m_fVerSD1D, m_ContrastStatFilter.m_fMeanVal);
	printf("--------------\n");

	m_fContrast[m_nFrameCount] = m_ContrastStatFilter.m_fSobel;

	bool bFoundMatch;

	if (bBuildSecondPyramid && bBuildMainPyramid && m_bNeedMatch)
	{
		printf("--BlockMatch--\n");
		bFoundMatch = m_MatchFilter.FindMatch();
		printf("--------------\n");
	}
	else
		bFoundMatch = false;

	/*if (!bFoundMatch)
	{
		printf("Block not found\n");
	}
	else*/
	if(bFoundMatch)
	{		
		printf("Block Center: [%d %d]<->[%d %d]\n", m_MatchFilter.m_nFocusXY[0],
			m_MatchFilter.m_nFocusXY[1],
			m_MatchFilter.m_nMatchXY[0],
			m_MatchFilter.m_nMatchXY[1]);

		//float dx = (m_MatchFilter.m_nFocusXY[0] - m_MatchFilter.m_nMatchXY[0] - m_MatchFilter.m_fImageCenterMain[0] + m_MatchFilter.m_fImageCenterSecond[0]);
		//float dy = (m_MatchFilter.m_nFocusXY[1] - m_MatchFilter.m_nMatchXY[1] - m_MatchFilter.m_fImageCenterMain[1] + m_MatchFilter.m_fImageCenterSecond[1]);
	
		float dx = ((m_MatchFilter.m_nFocusXY[0] - m_MatchFilter.m_fImageCenterMain[0])*fScaleAF2Ref - m_MatchFilter.m_nMatchXY[0] + m_MatchFilter.m_fImageCenterSecond[0]);
		float dy = ((m_MatchFilter.m_nFocusXY[1] - m_MatchFilter.m_fImageCenterMain[1])*fScaleAF2Ref - m_MatchFilter.m_nMatchXY[1] + m_MatchFilter.m_fImageCenterSecond[1]);

		//printf("Remove Center:[dx,dy]=[%f %f]", (m_MatchFilter.m_nFocusXY[0] - m_MatchFilter.m_nMatchXY[0] - m_MatchFilter.m_fImageCenterMain[0] + m_MatchFilter.m_fImageCenterSecond[0]), (m_MatchFilter.m_nFocusXY[1] - m_MatchFilter.m_nMatchXY[1] - m_MatchFilter.m_fImageCenterMain[1] + m_MatchFilter.m_fImageCenterSecond[1]));
		printf("Remove Center:[dx,dy]=[%f %f]\n", dx,dy);

		PlotRect(m_MatchFilter.m_SecondYPyramid, m_MatchFilter.m_nMatchXY[0], m_MatchFilter.m_nMatchXY[1],nFocusWin,"dump\\MatchedBlock.bmp");

		float disp = dy;
		
		int dac = DSIP2DAC(disp);

		printf("**************DAC_=%d**********************\n", dac);

		m_nNextDAC = dac;
		m_fDacValue[m_nFrameCount] = dac;

		m_nFrameCount++;
		m_nNextDAC = m_nCurrentDAC + 20;
		//m_bDisparityStable = (m_nNextDAC == m_nCurrentDAC);
		if (m_nFrameCount > 5)
		{
			int num = 0;
			float maxContrast = -1;
			for (int k = 0; k < m_nFrameCount; k++)
			{
				if (m_fContrast[k] > maxContrast)
				{
					maxContrast = m_fContrast[k];
					num = k;
				}
			}
			m_bDisparityStable = true;
			printf("max contrast frame:%d \n", num);
		}

	}
	
	if (!bFoundMatch)
	{
		m_bDisparityStable = false;
	}
	
	return bFoundMatch;
	
}


bool CDualCameraFastAFMatchFilter::BuildMainPyramid(CYUV420Image *pYUVImage, int nFocusPos[2], int nFocusWin)
{
	m_nFocusXY[0] = nFocusPos[0];
	m_nFocusXY[1] = nFocusPos[1];

	m_nWinHalfWidth = nFocusWin;

	m_fImageCenterMain[0] = pYUVImage->GetWidth() / 2.0f;
	m_fImageCenterMain[1] = pYUVImage->GetHeight() / 2.0f;
	

	//m_nFullSizeMainCenter[0] = pYUVImage->GetWidth() / 2.0f;
	//m_nFullSizeMainCenter[1] = pYUVImage->GetHeight() / 2.0f;

	int nWidth = pYUVImage->GetWidth();
	int nHeight = pYUVImage->GetHeight();

	int nXRange[2], nYRange[2];

	int W[8];
	int nHalfSize = m_nWinFilterSize / 2;
	W[m_nInitMatchLevel] = nFocusWin >> m_nInitMatchLevel;
	

	int i;	
	for (i = m_nInitMatchLevel - 1; i >= 0; i--)
	{
		W[i] = 2 * W[i + 1] + nHalfSize;
	}


	nXRange[0] = W[0];
	nXRange[1] = nWidth - W[0];//less, not equal
	nYRange[0] = W[0];
	nYRange[1] = nHeight - W[0];

	if (nXRange[1] < nXRange[0] || nYRange[1] < nYRange[0])
	{
		printf("Focus window size conflicts to match level\n");
		return false;
	}

	if (nFocusPos[0] < nXRange[0])nFocusPos[0] = nXRange[0];
	if (nFocusPos[0] >= nXRange[1])nFocusPos[0] = nXRange[1]-1;
	if (nFocusPos[1] < nYRange[0])nFocusPos[1] = nYRange[0];
	if (nFocusPos[1] >= nYRange[1])nFocusPos[1] = nYRange[1] - 1;
	printf("Real FocusPos = [%d %d]\n", nFocusPos[0], nFocusPos[1]);

	CGrayImage InBlock;
	InBlock.Create(2 * W[0] + 1, 2 * W[0] + 1);

	int y, x, y1, x1;
	for (y = 0; y < 2 * W[0] + 1; y++)
	{
		unsigned char *pSrcLine = pYUVImage->GetYLine(nFocusPos[1] + y - W[0]);
		unsigned char *pDstLine = InBlock.GetImageLine(y);
		for (x = 0; x < 2 * W[0] + 1; x++)
		{
			pDstLine[x] = pSrcLine[nFocusPos[0] + x - W[0]];
		}
	}
	

	for (i = 0; i <= m_nInitMatchLevel; i++)
	{
	/*	char filename[256];
		sprintf_s(filename, "dump\\Inblock_%d.bmp", i);
		InBlock.SaveToBMPFile(filename);*/
		m_MainYPyramid[i].Create(nFocusWin * 2 / (1 << i) + 1, nFocusWin * 2 / (1 << i) + 1);
		
		int nStartY = InBlock.GetHeight() - (nFocusWin * 2 / (1 << i) + 1);
		nStartY >>= 1;
		int nStartX = InBlock.GetWidth() - (nFocusWin * 2 / (1 << i) + 1);
		nStartX >>= 1;
		for (y = 0; y < nFocusWin * 2 / (1 << i) + 1; y++)
		{
			memcpy(m_MainYPyramid[i].GetImageLine(y), InBlock.GetImageLine(nStartY + y) + nStartX, nFocusWin * 2 / (1 << i) + 1);
		}
		DownSampleImage(&InBlock);

		if (m_bDumpImages)
		{
			char filename[256];
			sprintf_s(filename, "dump\\Main_Pyramid_%d.bmp", i);
			m_MainYPyramid[i].SaveToBMPFile(filename);
		}

	}
	return true;
}

bool CDualCameraFastAFMatchFilter::ExtractYImage(CYUV420Image *pInImage, CGrayImage *pOutImage)
{
	int nWidth = pInImage->GetWidth();
	int nHeight = pInImage->GetHeight();

	pOutImage->Create(nWidth, nHeight);
	memcpy(pOutImage->GetImageData(), pInImage->GetYImage(), nWidth*nHeight);
	return true;
}

bool CDualCameraFastAFMatchFilter::BuildSecondPyramid(CYUV420Image *pYUVImage, float fScale, float fAngleX, float fAngleY, float fAngleZ)
{


	int i;
	//CMatrix K(3, 3);
	CMatrix K1(3, 3);
	CMatrix K2(3, 3);
	CMatrix Rx(3, 3);
	CMatrix Ry(3, 3);
	CMatrix Rz(3, 3);
	CMatrix R;
	CMatrix T1(3, 3);
	CMatrix T2(3, 3);
	CMatrix T(3, 3);
	CMatrix H;
	CGrayImage TempImage1, TempImage2;

	//K.MakeUnit();
	K1.MakeUnit();
	int nInWidth = pYUVImage->GetWidth();
	int nInHeight = pYUVImage->GetHeight();
	int nScaleWidth = (int)(nInWidth * fScale* m_fScaleToMainRef + 0.5);
	int nScaleHeight = (int)(nInHeight * fScale*m_fScaleToMainRef + 0.5);

	int nScaleWidth1= nInWidth * m_fScaleToMainRef;
	int nScaleHeight1 = nInHeight * m_fScaleToMainRef;
	K1[0][0] = K1[1][1] = m_fScaleToMainRef;
	
	/*K[0][0] = (float)nScaleWidth / nInWidth;
	K[1][1] = (float)nScaleHeight / nInHeight;*/

	Angle2Rx(fAngleX, Rx);
	Angle2Ry(fAngleY, Ry);
	Angle2Rz(fAngleZ, Rz);
	R = Rz*Ry*Rx;
	printf("Rotation:\n");
	R.PRINTF("%12.8f\t");

	CMatrix R0(3, 3);
	CMatrix R1(3, 3);
	
	R0.MakeZero();
	R0[0][1] = -1;
	R0[1][0] = 1;
	R0[2][2] = 1;

	R1.MakeZero();
	R1[0][1] = 1;
	R1[1][0] = -1;
	R1[2][2] = 1;

	T1.MakeUnit();
	T1[0][2] = (float)(-nScaleWidth1 / 2);
	T1[1][2] = (float)(-nScaleHeight1 / 2);
	T1 = R0*T1;


	T2.MakeUnit();
	T2[0][2] = (float)(nScaleWidth / 2);
	T2[1][2] = (float)(nScaleHeight / 2);

	T2 = T2*R1;
		
	K2.MakeUnit();
	K2[0][0] = (float)nScaleWidth / nInWidth / m_fScaleToMainRef;
	K2[1][1] = (float)nScaleHeight / nInHeight / m_fScaleToMainRef;


	printf("K1:\n");
	K1.PRINTF("%12.8f\t");
	printf("T1:\n");
	T1.PRINTF("%12.8f\t");
	printf("R:\n");
	R.PRINTF("%12.8f\t");
	printf("K2:\n");
	K2.PRINTF("%12.8f\t");
	printf("T2:\n");
	T2.PRINTF("%12.8f\t");

	H = T2*K2*R*T1*K1;


	H /= H[2][2];
	printf("H:\n");
	H.PRINTF("%12.8f\t");

	H.Inverse();

	//CGrayImage TempImage1;

	int t1 = GetTickCount();
	if (!ExtractYImage(pYUVImage, &TempImage1))return false;
	if (!TempImage2.Create(nScaleWidth, nScaleHeight))return false;
	m_HomographMappingFilter.ProcessImage(&TempImage1, &TempImage2, H);
	printf("SecondCorrection=%d\n", (int)GetTickCount() - t1);

	ExtendImage(&TempImage2, m_SecondYPyramid, m_nInitMatchLevel);

	if (m_bDumpImages)
		TempImage2.SaveToBMPFile("dump\\Second_correct.bmp");

	if(m_bDumpImages)
		m_SecondYPyramid[0].SaveToBMPFile("dump\\Second_Pyramid_0.bmp");
	
	CGaussianPyramid gaussianPyramid;
	for (int i = 1; i <= m_nInitMatchLevel; i++)
	{
		if (!gaussianPyramid.DownScaleImage(m_SecondYPyramid + i - 1, m_SecondYPyramid + i))return false;

		if (m_bDumpImages)
		{
			char filename[256];
			sprintf_s(filename, "dump\\Second_Pyramid_%d.bmp", i);
			m_SecondYPyramid[i].SaveToBMPFile(filename);
		}
	}

	m_fImageCenterSecond[0] = TempImage2.GetWidth() / 2.0f;
	m_fImageCenterSecond[1] = TempImage2.GetHeight() / 2.0f;
	
	return true;
}


bool CDualCameraFastAFMatchFilter::ExtendImage(CGrayImage *pInImage, CGrayImage *pOutImage, int nS)
{
	int x, y;
	BYTE Y;
	int nMask = (1 << nS) - 1;
	int nInWidth = pInImage->GetWidth();
	int nInHeight = pInImage->GetHeight();
	int nOutWidth = nInWidth + (((1 << nS) - (nInWidth&nMask))&nMask);
	int nOutHeight = nInHeight + (((1 << nS) - (nInHeight&nMask))&nMask);

	if (nS < 0)return false;
	if (nInWidth == nOutWidth&&nInHeight == nOutHeight)
	{
		return pOutImage->Copy(pInImage);
	}

	if (pOutImage->GetWidth() != nOutWidth || pOutImage->GetHeight() != nOutHeight)
	{
		if (!pOutImage->Create(nOutWidth, nOutHeight))return false;
	}

	for (y = 0; y < nInHeight; y++)
	{
		BYTE *pInY = pInImage->GetImageLine(y);
		BYTE *pOutY = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			Y = *(pInY++);
			*(pOutY++) = Y;
		}
		for (; x < nOutWidth; x++)
		{
			*(pOutY++) = Y;
		}
	}
	for (; y < nOutHeight; y++)
	{
		BYTE *pInY = pInImage->GetImageLine(nInHeight - 1);
		BYTE *pOutY = pOutImage->GetImageLine(y);
		for (x = 0; x < nInWidth; x++)
		{
			Y = *(pInY++);
			*(pOutY++) = Y;
		}
		for (; x < nOutWidth; x++)
		{
			*(pOutY++) = Y;
		}
	}
	return true;
}

void CDualCameraFastAFMatchFilter::FilterLine(unsigned char *pInLine, unsigned short *pOutLine, int nWidth, int pFilter[], int nFilterSize)
{
	int x, dx;
	for (x = 0; x < nWidth + 1 - nFilterSize; x++)
	{
		int sum = 0;
		for (dx = 0; dx <nFilterSize; dx++)
		{
			sum += ((int)pInLine[x + dx]) * pFilter[dx];
		}
		pOutLine[x] = sum;
	}
}


void CDualCameraFastAFMatchFilter::DownSampleImage(CGrayImage *pImage)
{
	//blur,sampling,crop boundary
	int nInWidth = pImage->GetWidth();
	int nInHeight = pImage->GetHeight();

	int nOutWidth = (nInWidth - 1- (m_nWinFilterSize - 1)) / 2 + 1 ;
	int nOutHeight = (nInHeight - 1- (m_nWinFilterSize - 1)) / 2 + 1;

	CGrayImage OutImage;
	OutImage.Create(nOutWidth, nOutHeight);

	int y, x;	
	unsigned short **pLines = new unsigned short *[m_nWinFilterSize];
	for (y = 0; y < m_nWinFilterSize; y++)
	{
		pLines[y] = new unsigned short[nInWidth - m_nWinFilterSize + 1];
		FilterLine(pImage->GetImageLine(y), pLines[y], nInWidth, m_nWinFilter, m_nWinFilterSize);
	}

	int dy;
	for (y = 0; y < nOutHeight; y++)
	{
		unsigned char *pOutLine = OutImage.GetImageLine(y);
		for (x = 0; x < nOutWidth; x++)
		{
			int sum = 0;
			int bx = 2 * x;
			for (dy = 0; dy < m_nWinFilterSize; dy++)
			{
				sum += ((m_nWinFilter[dy]*pLines[dy][bx])>> m_nFilterSumShift);
			}
			sum >>= m_nFilterSumShift;
			pOutLine[x] = sum;
		}

		for (dy = 0; dy < m_nWinFilterSize - 2; dy++)
		{
			memcpy(pLines[dy], pLines[dy + 2], (nInWidth - m_nWinFilterSize + 1)*sizeof(unsigned short));
		}
		FilterLine(pImage->GetImageLine(2 * y + m_nWinFilterSize), pLines[m_nWinFilterSize - 2], nInWidth, m_nWinFilter, m_nWinFilterSize);
		FilterLine(pImage->GetImageLine(2 * y + m_nWinFilterSize+1), pLines[m_nWinFilterSize - 1], nInWidth, m_nWinFilter, m_nWinFilterSize);

	}

	
	
	for (y = 0; y < m_nWinFilterSize; y++)
	{
		delete[]pLines[y];
	}
	delete[]pLines;

	pImage->Copy(&OutImage);

}

double CDualCameraFastAFMatchFilter::ComputeCorrelation(CGrayImage *pInImage0, CGrayImage *pInImage1, int nOffsetX, int nOffsetY)
{
	int x, y, X, Y;
	int nPixelCount;
	double fX, fY, fXY, fX2, fY2;
	int nWidth0 = pInImage0->GetWidth();
	int nHeight0 = pInImage0->GetHeight();
	int nWidth1 = pInImage1->GetWidth();
	int nHeight1 = pInImage1->GetHeight();
	//int nDim = pInImage0->GetDim();

	nPixelCount = 0;
	fX = fY = fXY = fX2 = fY2 = 0;
	int SY = MAX2(0, -nOffsetY);
	int EY = MIN2(nHeight0, nHeight1 - nOffsetY);
	int SX = MAX2(0, -nOffsetX);
	int EX = MIN2(nWidth0, nWidth1 - nOffsetX);
	nPixelCount = (EY - SY)*(EX - SX);
	if (nPixelCount <= 256)return -1000.0;

	for (y = SY; y < EY; y++)
	{
		BYTE *pIn0 = pInImage0->GetImageLine(y) + SX;
		BYTE *pIn1 = pInImage1->GetImageLine(y + nOffsetY) + (SX + nOffsetX);
		for (x = SX; x < EX; x++)
		{
			X = *pIn0;	pIn0++;// = nDim;
			Y = *pIn1;	pIn1++;// = nDim;

			fX += X;
			fY += Y;
			fX2 += X*X;
			fY2 += Y*Y;
			fXY += X*Y;
		}
	}

	fX /= nPixelCount;
	fY /= nPixelCount;
	fX2 /= nPixelCount;	fX2 -= fX*fX;
	fY2 /= nPixelCount;	fY2 -= fY*fY;
	fXY /= nPixelCount;	fXY -= fX*fY;

	fX2 = sqrt(fX2);
	fY2 = sqrt(fY2);

	return fXY / (fX2*fY2);
}

double CDualCameraFastAFMatchFilter::SearchMaxCorrelation(CGrayImage *pImage0, CGrayImage *pImage1, int &nOffsetX, int &nOffsetY, int nRangeX, int nRangeY, bool &bFoundMultiple)
{
	int x, y, nBestX, nBestY;
	double E, maxE;

	double *E_Array = new double[(2 * nRangeX + 1)*(2 * nRangeY + 1)];
	memset(E_Array, 0, sizeof(double)*(2 * nRangeX + 1)*(2 * nRangeY + 1));

	maxE = ComputeCorrelation(pImage0, pImage1, nOffsetX, nOffsetY);
	nBestY = nOffsetY;
	nBestX = nOffsetX;
	for (y = -nRangeY; y <= nRangeY; y++)
	{
		for (x = -nRangeX; x <= nRangeX; x++)
		{
			//if (y == 0 && x == 0)continue;
			E = ComputeCorrelation(pImage0, pImage1, nOffsetX + x, nOffsetY + y);
			if (E > maxE)
			{
				nBestY = nOffsetY + y;
				nBestX = nOffsetX + x;
				maxE = E;
			}
			E_Array[(y + nRangeY)*(2 * nRangeX + 1) + x + nRangeX] = E;
		}
	}

	//search whether mulitplepeak
	bool bMultiplePeak = false;
	int dx, dy;

	for (y = -nRangeY; y <= nRangeY; y++)
	{
		for (x = -nRangeX; x <= nRangeX; x++)
		{
			if (nBestY == nOffsetY + y && nBestX == nOffsetX + x) continue;//maxE itself is not in consideration
			double cE = E_Array[(y + nRangeY)*(2 * nRangeX + 1) + x + nRangeX];
			if (cE < maxE*m_nPeakPercentTh / 100)continue;//not a distinct high value

			bool isLocalMaxima = true;
			for (dy = -m_nLocalMaxBoxY; dy <= m_nLocalMaxBoxY; dy++)
			{
				if (y + dy < -nRangeY || y + dy > nRangeY)continue;//skip possible boundary
				for (dx = -m_nLocalMaxBoxX; dx <= m_nLocalMaxBoxX; dx++)
				{
					if (x + dx < -nRangeX || x + dx > nRangeX)continue;//skip possible boundary

					if (E_Array[(y + dy + nRangeY)*(2 * nRangeX + 1) + x + dx + nRangeX] > cE)
					{
						isLocalMaxima = false;
						break;
					}
				}
				if (!isLocalMaxima)break;
			}

			if (isLocalMaxima)
			{
				bMultiplePeak = true;
				break;
			}
		}
		if (bMultiplePeak)	break;
	}

	bFoundMultiple = bMultiplePeak;

	nOffsetY = nBestY;
	nOffsetX = nBestX;

	delete[]E_Array;
	return maxE;
}

bool CDualCameraFastAFMatchFilter::FindMatch()
{
	int i;
	bool bSuccess = true;
	bool bFoundMultiPeak = false;
	
	/*m_fImageCenterMain[0] = m_MainYPyramid[0].GetWidth() / 2.0f;
	m_fImageCenterMain[1] = m_MainYPyramid[0].GetHeight() / 2.0f;
	m_fImageCenterSecond[0] = m_SecondYPyramid[0].GetWidth() / 2.0f;
	m_fImageCenterSecond[1] = m_SecondYPyramid[0].GetHeight() / 2.0f;*/


	//int nOffsetX = m_nInitOffset[0] + (m_nFocusXY[0] >> m_nInitMatchLevel);
	//int nOffsetY = m_nInitOffset[1] + (m_nFocusXY[1] >> m_nInitMatchLevel);
	//int nOffsetX = ((m_nFocusXY[0] - m_fImageCenterMain[0]) + m_SecondYPyramid[m_nInitMatchLevel].GetWidth() / 2.0f) / (1 << m_nInitMatchLevel) + m_nInitOffset[0];
	//int nOffsetY = ((m_nFocusXY[1] - m_fImageCenterMain[1]) + m_SecondYPyramid[m_nInitMatchLevel].GetHeight() / 2.0f) / (1 << m_nInitMatchLevel) + m_nInitOffset[1];

	int nOffsetX = (m_nFocusXY[0] - m_fImageCenterMain[0] + m_fImageCenterSecond[0]) / (1 << m_nInitMatchLevel) + m_nInitOffset[0] - (m_MainYPyramid[m_nInitMatchLevel].GetWidth() / 2);
	int nOffsetY = (m_nFocusXY[1] - m_fImageCenterMain[1] + m_fImageCenterSecond[1]) / (1 << m_nInitMatchLevel)  + m_nInitOffset[1] - (m_MainYPyramid[m_nInitMatchLevel].GetHeight() / 2);

	for (i = m_nInitMatchLevel; i >= 0; i--)
	{
		if (i != m_nInitMatchLevel)
		{
			nOffsetX *= 2;
			nOffsetY *= 2;
		}

		printf("Offset=[%d,%d]->", nOffsetX, nOffsetY);
		float fmax = SearchMaxCorrelation(&m_MainYPyramid[i], &m_SecondYPyramid[i], nOffsetX, nOffsetY, m_nSearchRange[i][0], m_nSearchRange[i][1], bFoundMultiPeak);
		printf("[%d,%d], fmax=%f, %s\n", nOffsetX, nOffsetY, fmax, bFoundMultiPeak ? "Multi" : "Single");
		//if (bFoundMultiPeak || fmax*100 < m_nCorrTh[i])
		if (fmax * 100 < m_nCorrTh[i])
		{
			printf("fmax=%f, false\n", fmax);
			bSuccess = false;
			break;
		}
	}

	m_nMatchXY[0] = nOffsetX + m_nWinHalfWidth;
	m_nMatchXY[1] = nOffsetY + m_nWinHalfWidth;

	return bSuccess;
	//return true;
}