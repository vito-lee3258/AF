#include "HomographMappingFilter.h"

void CHomographMappingFilter::BicubicMappingImage(BYTE *pInImage, int nInWidth, int nInHeight, BYTE *pOutImage, int nOutWidth, int nOutHeight, int nDim, CMatrix &H)
{
	int i, j, k, x1, y1, x2, y2, x3, y3, x4, y4, nOut[3], nInWin[3][4][4], nInY[3][4];
	int nInPitch = nInWidth*nDim;
	float fx, fy, fY[3], fX[3];

	BYTE *pOut = pOutImage;
	fY[0] = H[0][2];
	fY[1] = H[1][2];
	fY[2] = H[2][2];
	for (y1 = 0; y1 < nOutHeight; y1++)
	{
		fX[0] = fY[0];
		fX[1] = fY[1];
		fX[2] = fY[2];
		for (x1 = 0; x1 < nOutWidth; x1++)
		{
			//			HomographMap((float)x1, (float)y1, H, fx, fy);
			fx = fX[0] / fX[2];
			fy = fX[1] / fX[2];

			fX[0] += H[0][0];
			fX[1] += H[1][0];
			fX[2] += H[2][0];

			x2 = (int)fx;
			y2 = (int)fy;
			fx -= x2;
			fy -= y2;

			for (i = 0, y3 = y2 - 1; i < 4; i++, y3++)
			{
				y4 = MIN2(nInHeight - 1, MAX2(0, y3));
				BYTE *pInLine = pInImage + y4*nInPitch;
				for (j = 0, x3 = x2 - 1; j < 4; j++, x3++)
				{
					x4 = MIN2(nInWidth - 1, MAX2(0, x3));
					BYTE *pIn = pInLine + x4*nDim;
					for (k = 0; k < nDim; k++)
					{
						nInWin[k][i][j] = pIn[k];
					}
				}

				for (k = 0; k < nDim; k++)
				{
					nInY[k][i] = (int)(CubicInterpolate(nInWin[k][i], fx)*16.0);
				}
			}

			for (k = 0; k < nDim; k++)
			{
				nOut[k] = (int)(CubicInterpolate(nInY[k], fy) / 16.0);

				if (nOut[k] < 0)nOut[k] = 0;	if (nOut[k] > 255)nOut[k] = 255;

				*(pOut++) = (BYTE)nOut[k];
			}
		}
		fY[0] += H[0][1];
		fY[1] += H[1][1];
		fY[2] += H[2][1];
	}
}

void CHomographMappingFilter::BilinearMappingImage(BYTE *pInImage, int nInWidth, int nInHeight, BYTE *pOutImage, int nOutWidth, int nOutHeight, int nDim, CMatrix &H)
{
	int i, j, k, x1, y1, x2, y2, x3, y3, x4, y4, nOut[3], nInWin[3][2][2], nInY[3][2];
	int nInPitch = nInWidth*nDim;
	float fx, fy, fY[3], fX[3];

	BYTE *pOut = pOutImage;
	fY[0] = H[0][2];
	fY[1] = H[1][2];
	fY[2] = H[2][2];
	for (y1 = 0; y1 < nOutHeight; y1++)
	{
		fX[0] = fY[0];
		fX[1] = fY[1];
		fX[2] = fY[2];
		for (x1 = 0; x1 < nOutWidth; x1++)
		{
			//			HomographMap((float)x1, (float)y1, H, fx, fy);
			fx = fX[0] / fX[2];
			fy = fX[1] / fX[2];

			fX[0] += H[0][0];
			fX[1] += H[1][0];
			fX[2] += H[2][0];

			x2 = (int)fx;
			y2 = (int)fy;
			fx -= x2;
			fy -= y2;

			for (i = 0, y3 = y2; i < 2; i++, y3++)
			{
				y4 = MIN2(nInHeight - 1, MAX2(0, y3));
				BYTE *pInLine = pInImage + y4*nInPitch;
				for (j = 0, x3 = x2; j < 2; j++, x3++)
				{
					x4 = MIN2(nInWidth - 1, MAX2(0, x3));
					BYTE *pIn = pInLine + x4*nDim;
					for (k = 0; k < nDim; k++)
					{
						nInWin[k][i][j] = pIn[k];
					}
				}

				for (k = 0; k < nDim; k++)
				{
					nInY[k][i] = (int)(LinearInterpolate(nInWin[k][i], fx)*16.0);
				}
			}

			for (k = 0; k < nDim; k++)
			{
				nOut[k] = (int)(LinearInterpolate(nInY[k], fy) / 16.0);

				if (nOut[k] < 0)nOut[k] = 0;	if (nOut[k] > 255)nOut[k] = 255;

				*(pOut++) = (BYTE)nOut[k];
			}
		}
		fY[0] += H[0][1];
		fY[1] += H[1][1];
		fY[2] += H[2][1];
	}
}

void CHomographMappingFilter::ProcessImage(CRGBYUVImage *pInImage, CRGBYUVImage *pOutImage, CMatrix &H)
{
	if (m_bBicubicEnable)
	{
		BicubicMappingImage(pInImage->GetImageData(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetImageData(), pOutImage->GetWidth(), pOutImage->GetHeight(), 3, H);
	}
	else
	{
		BilinearMappingImage(pInImage->GetImageData(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetImageData(), pOutImage->GetWidth(), pOutImage->GetHeight(), 3, H);
	}
}

void CHomographMappingFilter::ProcessImage(CGrayImage *pInImage, CGrayImage *pOutImage, CMatrix &H)
{
	if (m_bBicubicEnable)
	{
		BicubicMappingImage(pInImage->GetImageData(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetImageData(), pOutImage->GetWidth(), pOutImage->GetHeight(), 1, H);
	}
	else
	{
		BilinearMappingImage(pInImage->GetImageData(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetImageData(), pOutImage->GetWidth(), pOutImage->GetHeight(), 1, H);
	}
}

void CHomographMappingFilter::ProcessImage(CYUV420Image *pInImage, CYUV420Image *pOutImage, CMatrix &H)
{
	if (m_bBicubicEnable)
	{
		BicubicMappingImage(pInImage->GetYImage(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetYImage(), pOutImage->GetWidth(), pOutImage->GetHeight(), 1, H);
		BicubicMappingImage(pInImage->GetUVImage(), pInImage->GetWidth() / 2, pInImage->GetHeight() / 2, pOutImage->GetUVImage(), pOutImage->GetWidth() / 2, pOutImage->GetHeight() / 2, 2, H);
	}
	else
	{
		BilinearMappingImage(pInImage->GetYImage(), pInImage->GetWidth(), pInImage->GetHeight(), pOutImage->GetYImage(), pOutImage->GetWidth(), pOutImage->GetHeight(), 1, H);
		BilinearMappingImage(pInImage->GetUVImage(), pInImage->GetWidth() / 2, pInImage->GetHeight() / 2, pOutImage->GetUVImage(), pOutImage->GetWidth() / 2, pOutImage->GetHeight() / 2, 2, H);
	}
}
