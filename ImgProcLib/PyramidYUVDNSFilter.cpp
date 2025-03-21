#include "PyramidYUVDNSFilter.h"

bool CPyramidYUVDNSFilter::BuildPyramid(CRGBYUVImage *pInImage)
{
	CRGBYUVImage LPyramid[2], TempImage;

	if (!DownScaleImage(pInImage, LPyramid))return false;
	if (!UpScaleImage(LPyramid, &TempImage))return false;
	if (!SubtractEdge(pInImage, &TempImage, m_EdgePyramid))return false;

	if (!DownScaleImage(LPyramid, LPyramid + 1))return false;
	if (!UpScaleImage(LPyramid + 1, &TempImage))return false;
	if (!SubtractEdge(LPyramid, &TempImage, m_EdgePyramid + 1))return false;

	if (!DownScaleImage(LPyramid + 1, &m_LImage))return false;
	if (!UpScaleImage(&m_LImage, &TempImage))return false;
	if (!SubtractEdge(LPyramid + 1, &TempImage, m_EdgePyramid + 2))return false;

	return true;
}

bool CPyramidYUVDNSFilter::BuildPyramid(CYUV420Image *pInImage)
{
	CRGBYUVImage LPyramid[2], TempYUVImage;
	CYUV420Image TempYUV420Image;

	if (!DownScaleImage(pInImage, LPyramid))return false;
	if (!UpScaleImage(LPyramid, &TempYUV420Image))return false;
	if (!SubtractEdge(pInImage, &TempYUV420Image, m_EdgePyramid))return false;

	if (!DownScaleImage(LPyramid, LPyramid + 1))return false;
	if (!UpScaleImage(LPyramid + 1, &TempYUVImage))return false;
	if (!SubtractEdge(LPyramid, &TempYUVImage, m_EdgePyramid + 1))return false;

	if (!DownScaleImage(LPyramid + 1, &m_LImage))return false;
	if (!UpScaleImage(&m_LImage, &TempYUVImage))return false;
	if (!SubtractEdge(LPyramid + 1, &TempYUVImage, m_EdgePyramid + 2))return false;

	return true;
}

bool CPyramidYUVDNSFilter::DNSYUVImage_Level(CRGBYUVImage *pYUVImage, int nLevel)
{
//	printf("Level%d ", nLevel);
	if (m_nDNSYWinWLevel[nLevel] > 1)
	{
		int nDNSYNoise = (m_nNoiseY*m_nNoiseYLevel[nLevel] + 8) >> 4;
//		printf("DNSYNoise=%d DNSYWinW=%d NoiseYAddBack=%d ", nDNSYNoise, m_nDNSYWinWLevel[nLevel], m_nAddBackNoiseYLevel[nLevel]);
		if (!m_DNSYFilter.ProcessImage(pYUVImage, m_nDNSYWinWLevel[nLevel], nDNSYNoise, m_nAddBackNoiseYLevel[nLevel]))return false;
	}

	int nDNSUVNoise[3];
	nDNSUVNoise[0] = m_nDNSUVYThre;
	nDNSUVNoise[1] = nDNSUVNoise[2] = (m_nNoiseUV*m_nNoiseUVLevel[nLevel] + 8) >> 4;
//	printf("DNSUVNoise=[%d,%d,%d] DNSUVWinW=%d", nDNSUVNoise[0], nDNSUVNoise[1], nDNSUVNoise[2], m_nDNSUVWinWLevel[nLevel]);
	if (!m_DNSUVFilter.ProcessImage(pYUVImage, m_nDNSUVWinWLevel[nLevel], nDNSUVNoise))return false;
//	printf("\n");

	return true;
}

