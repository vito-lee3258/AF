#include <stdio.h>
//#include "../ImgLib/Filter.h"
#include "CDualCameraFastAFFilter.h"


bool ExtractFileExt(char *pFileFullName, char *pExt)
{
	unsigned int l = strlen(pFileFullName);
	if (l<5)return false;
	if (pFileFullName[l - 4] != '.')return false;

	strcpy_s(pExt, 4, pFileFullName + l - 3);
	return true;
}

bool ExtractFileName(char *pFileFullName, char *pFileName)
{
	unsigned int l = strlen(pFileFullName);
	if (l<5)return false;
	if (pFileFullName[l - 4] != '.')return false;

	strncpy_s(pFileName, 256, pFileFullName, l - 4);
	pFileName[l - 4] = '\0';
	return true;
}

int main(int argc, char **argv)
{

	int i;
	char main_image[256], second_image[256], main_ext[4], second_ext[4];
	char para_filename[256],filename[256];
	CDualCameraFastAFFilter DualCameraFastAFFilter;

	int nMainWidth = 1280;
	int nMainHeight = 960;
	int nSecondWidth = 320;
	int nSecondHeight=240;
	int bMainUVOrder = 1;
	int bSecondUVOrder = 1;
	int nFocusPos[2] = {0,0};
	int nFocusWin=128;

	bool bSingleMode = false;

	//float fSecondScale = 1.0f;
	int nDAC;
	float fAngleX = 0.0f;
	float fAngleY = 0.0f;
	float fAngleZ = 0.0f;
	int daclist[9];
	float fScale2Ref;
	float scale_list[9];
	float fPI = atan2(0, -1);

	if (argc < 4)return -1;

	ExtractFileName(argv[1], main_image);
	ExtractFileExt(argv[1], main_ext);
	_strlwr_s(main_ext, 4);

	ExtractFileName(argv[2], second_image);
	ExtractFileExt(argv[2], second_ext);
	_strlwr_s(second_ext, 4);

	sprintf_s(para_filename, "control_param_FastAF_%s.txt", argv[3]);
	

	if (!DualCameraFastAFFilter.LoadParameterFile(para_filename))
	{
		printf("Can not load parameter file from %s!!!\n", para_filename);
		DualCameraFastAFFilter.SaveParameterFile("default_param_FastAF.txt");
		return -1;
	}


	for (i = 4; i < argc; i++)
	{
	if (strcmp(argv[i], "-MainSize") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "[%d,%d]", &nMainWidth, &nMainHeight);
			printf("MainSize=[%d,%d]\n", nMainWidth, nMainHeight);
			i++;
		}
		else if (strcmp(argv[i], "-SecondSize") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "[%d,%d]", &nSecondWidth, &nSecondHeight);
			printf("SecondSize=[%d,%d]\n", nSecondWidth, nSecondHeight);
			i++;
		}
		else if (strcmp(argv[i], "-MainUVOrder") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "%d", &bMainUVOrder);
			i++;
		}
		else if (strcmp(argv[i], "-SecondUVOrder") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "%d", &bSecondUVOrder);
			i++;
		}
		else if (strcmp(argv[i], "-FocusXY") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "[%d,%d]", nFocusPos, nFocusPos + 1);
			//bRefocusMode = true;
			i++;
		}
		else if (strcmp(argv[i], "-FocusWin") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "%d", &nFocusWin);
			i++;
		}
		else if (strcmp(argv[i], "-nDAC") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "%d", &nDAC);
			i++;
		}
		/*else if (strcmp(argv[i], "-fSecondScale") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "%f", &fSecondScale);
			i++;
		}*/
		else if (strcmp(argv[i], "-Cali") == 0 && i + 1 < argc)
		{
			FILE *fpc = NULL;
			fopen_s(&fpc, argv[i + 1], "rt");
			if (fpc != NULL)
			{ 
				fscanf_s(fpc, "AngleX=%e\n", &fAngleX);
				fscanf_s(fpc, "AngleY=%e\n", &fAngleY);
				fscanf_s(fpc, "AngleZ=%e\n", &fAngleZ);
				fscanf_s(fpc, "DAC=[%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", daclist, daclist + 1, daclist + 2, daclist + 3, daclist + 4, daclist + 5, daclist + 6, daclist + 7, daclist + 8);
				fscanf_s(fpc, "Scale=[%f,%f,%f,%f,%f,%f,%f,%f,%f]\n", scale_list, scale_list + 1, scale_list + 2, scale_list + 3, scale_list + 4, scale_list + 5, scale_list + 6, scale_list + 7, scale_list + 8);
				fscanf_s(fpc, "ScaleRef=%f\n", &fScale2Ref);
				fclose(fpc);

				printf("AngleX=%f AngleY=%f AngleZ=%f (degrees)\n", fAngleX*180.0 / fPI, fAngleY*180.0 / fPI, fAngleZ*180.0 / fPI);

				DualCameraFastAFFilter.m_MatchFilter.m_fRotationAngle[0] = fAngleX;
				DualCameraFastAFFilter.m_MatchFilter.m_fRotationAngle[1] = fAngleY;
				DualCameraFastAFFilter.m_MatchFilter.m_fRotationAngle[2] = fAngleZ;

				DualCameraFastAFFilter.m_MatchFilter.m_fScaleToMainRef = fScale2Ref;
				for (int j = 0; j < 9; j++)
				{
					DualCameraFastAFFilter.m_MatchFilter.m_fScaleList[j] = scale_list[j];
					DualCameraFastAFFilter.m_MatchFilter.m_nDACScaleList[j] = daclist[j];
				}
			}
			i++;
		}
		/*else if (strcmp(argv[i], "-Angle") == 0 && i + 1 < argc)
		{
			sscanf_s(argv[i + 1], "[%f,%f,%f]", &fAngleX, &fAngleY, &fAngleZ);
			printf("AngleX=%f AngleY=%f AngleZ=%f (degrees)\n", fAngleX, fAngleY, fAngleZ);

			fAngleX = fAngleX*fPI / 180.0;
			fAngleY = fAngleY*fPI / 180.0;
			fAngleZ = fAngleZ*fPI / 180.0;

			i++;
		}*/
		else if (strcmp(argv[i], "-SingleStepMode") == 0)
		{
			bSingleMode = true;
		}
	}
	
	CYUV420Image MainYUVImage, SecondYUVImage;
	CRGBYUVImage RGBImage;
	if (strcmp(main_ext, "bmp") == 0)
	{
		if (!RGBImage.LoadFromBMPFile(argv[1]))
		{
			printf("Can not load main image from %s!!!\n", argv[1]);
			return -1;
		}
		RGBImage.RGB2YCbCr();
		if (!MainYUVImage.YUV444ToYUV420(&RGBImage))
		{
			printf("YUV444toYUV420 Fail!!!\n");
			return -1;
		}
	}
	else if (strcmp(main_ext, "yuv") == 0)
	{
		if (!MainYUVImage.LoadYUV420File(argv[1], nMainWidth, nMainHeight, bMainUVOrder))
		{
			printf("Can not load main image from %s!!!\n", argv[1]);
			return -1;
		}
		MainYUVImage.YUV420ToYUV444(&RGBImage);
		RGBImage.YCbCr2RGB();
		sprintf_s(filename, 256, "%s.bmp", main_image);
		RGBImage.SaveToBMPFile(filename);
	}
	else
	{
		printf("Unknown image format: %s!!!\n", argv[1]);
		return -1;
	}
	nMainWidth = MainYUVImage.GetWidth();
	nMainHeight = MainYUVImage.GetHeight();


	if (strcmp(second_ext, "bmp") == 0)
	{
		if (!RGBImage.LoadFromBMPFile(argv[2]))
		{
			printf("Can not load second image from %s!!!\n", argv[2]);
			return -1;
		}
		RGBImage.RGB2YCbCr();
		if (!SecondYUVImage.YUV444ToYUV420(&RGBImage))
		{
			printf("YUV444toYUV420 Fail!!!\n");
			return -1;
		}
	}
	else if (strcmp(second_ext, "yuv") == 0)
	{
		if (!SecondYUVImage.LoadYUV420File(argv[2], nSecondWidth, nSecondHeight, bSecondUVOrder))
		{
			printf("Can not load second image from %s!!!\n", argv[2]);
			return -1;
		}
		SecondYUVImage.YUV420ToYUV444(&RGBImage);
		RGBImage.YCbCr2RGB();
		sprintf_s(filename, 256, "%s.bmp", second_image);
		RGBImage.SaveToBMPFile(filename);
	}
	else
	{
		printf("Unknown image format: %s!!!\n", argv[2]);
		return -1;
	}
	nSecondWidth = SecondYUVImage.GetWidth();
	nSecondHeight = SecondYUVImage.GetHeight();



	//if (!DualCameraFastAFFilter.ProcessImagePair(&MainYUVImage, &SecondYUVImage, nFocusPos, nFocusWin, nDAC))// , fAngleX, fAngleY, fAngleZ))


	while (1)
	{
	
		int t1 = GetTickCount();
		if(!DualCameraFastAFFilter.ProcessImagePair(&MainYUVImage, &SecondYUVImage, nFocusPos, nFocusWin, nDAC))// , fAngleX, fAngleY, fAngleZ))
		{
			printf("Processing Error\n");
			return -1;
		}
		int t2 = GetTickCount();
		printf("******** Time cost: %dms *********\n", (int)(t2 - t1));
		if (bSingleMode)break;

		if (DualCameraFastAFFilter.m_bDisparityStable)
		{
			for (int n = 0; n < 50; n++)
			{
				printf("Contrast value:%f \n", DualCameraFastAFFilter.m_fContrast[n]);
				printf("DAC value:%d \n", DualCameraFastAFFilter.m_fDacValue[n]);
			}
			break;
		}
		else
		{
			nDAC = DualCameraFastAFFilter.m_nNextDAC;
			char newfile[256];
			sprintf_s(newfile, "%03d.%s", nDAC, main_ext);

			if (strcmp(main_ext, "bmp") == 0)
			{
				if (!RGBImage.LoadFromBMPFile(newfile))
				{
					printf("Can not load main image from %s!!!\n", argv[1]);
					return -1;
				}
				RGBImage.RGB2YCbCr();
				if (!MainYUVImage.YUV444ToYUV420(&RGBImage))
				{
					printf("YUV444toYUV420 Fail!!!\n");
					return -1;
				}
			}
			else if (strcmp(main_ext, "yuv") == 0)
			{
				if (!MainYUVImage.LoadYUV420File(newfile, nMainWidth, nMainHeight, bMainUVOrder))
				{
					printf("Can not load main image from %s!!!\n", argv[1]);
					return -1;
				}
				MainYUVImage.YUV420ToYUV444(&RGBImage);
				RGBImage.YCbCr2RGB();
				sprintf_s(filename, 256, "%s.bmp", main_image);
				//RGBImage.SaveToBMPFile(filename);
			}
		}
	
	}


	/*int t1 = GetTickCount();

	if (!DualCameraFastAFFilter.m_MatchFilter.BuildMainPyramid(&MainYUVImage, nFocusPos, nFocusWin))
	{
		printf("Build main pyramid Fail\n");
		return -1;
	}
	printf("MainPyramid=%d\n", (int)GetTickCount() - t1);

	if(!DualCameraFastAFFilter.m_MatchFilter.BuildSecondPyramid(&SecondYUVImage, fSecondScale, fAngleX, fAngleY, fAngleZ))
	{
		printf("Build second pyramid Fail\n");
		return -1;
	}
	printf("SecondPyramid=%d\n", (int)GetTickCount() - t1);


	bool bFoundMatch = DualCameraFastAFFilter.m_MatchFilter.FindMatch();
	printf("Match=%d\n", (int)GetTickCount() - t1);

	if (!bFoundMatch)
	{
		printf("Block not found\n");
	}
	else
	{
		printf("[%d %d]<->[%d %d]\n", DualCameraFastAFFilter.m_MatchFilter.m_nFocusXY[0], 
			DualCameraFastAFFilter.m_MatchFilter.m_nFocusXY[1], 
			DualCameraFastAFFilter.m_MatchFilter.m_nMatchXY[0], 
			DualCameraFastAFFilter.m_MatchFilter.m_nMatchXY[1]);

		printf("[dx,dy]=[%f %f]", (DualCameraFastAFFilter.m_MatchFilter.m_nFocusXY[0] - DualCameraFastAFFilter.m_MatchFilter.m_nMatchXY[0] - DualCameraFastAFFilter.m_MatchFilter.m_fImageCenterMain[0]+ DualCameraFastAFFilter.m_MatchFilter.m_fImageCenterSecond[0]), (DualCameraFastAFFilter.m_MatchFilter.m_nFocusXY[1] - DualCameraFastAFFilter.m_MatchFilter.m_nMatchXY[1] - DualCameraFastAFFilter.m_MatchFilter.m_fImageCenterMain[1] + DualCameraFastAFFilter.m_MatchFilter.m_fImageCenterSecond[1]));
	}
	
	int t2 = GetTickCount();
	printf("t2-t1=%d\n", t2 - t1);*/

	return 0;
}