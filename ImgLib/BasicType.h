#ifndef __BASIC_TYPE_H_
#define __BASIC_TYPE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>


#define double float

#define MAX2(x, y) (((x)>(y))?(x):(y))
#define MIN2(x, y) (((x)<(y))?(x):(y))
#define ABS(x) (((x)<0)?(-(x)):(x))
#define DIFF(x, y) (((x)>(y))?((x)-(y)):((y)-(x)))
#define DIFF2(x, y) (((x)-(y))*((x)-(y)))

typedef struct tagCameraInfo
{
	int nCameraExposure;
	int nCameraGain;
	int nCameraGainShift;
	int nAWBGain[4];
	int nAWBGainShift;
	int nDigiGain;
	int nDigiGainShift;
	int nBlackLevel;

	tagCameraInfo()
	{
		nCameraExposure = 1;
		nCameraGain = 16;
		nCameraGainShift = 4;
		nAWBGain[0] = nAWBGain[1] = nAWBGain[2] = nAWBGain[3] = 512;
		nAWBGainShift = 9;
		nDigiGain = 128;
		nDigiGainShift = 7;
		nBlackLevel = 64;
	}
}TCameraInfo;

typedef struct tagStatRect
{
	int nLeft;
	int nTop;
	int nRight;
	int nBottom;

	tagStatRect()
	{
		nLeft = nTop = nRight = nBottom = 0;
	}
}TStatRect;

typedef struct tagPixelPos
{
	int nX;
	int nY;
}TPixelPos;

#endif

