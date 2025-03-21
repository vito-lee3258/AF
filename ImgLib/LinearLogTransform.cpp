#include <math.h>
#include "LinearLogTransform.h"

bool CLinearLogTransform::Initialize(int nLog2Bit, int nLinear2LogBit, int nLog2LinearBit)
{
	int i;
	double fx, fy;
	m_nLog2 = (1 << nLog2Bit);
	m_nLog2Bit = nLog2Bit;
	m_nLinear2LogBit = nLinear2LogBit;
	m_nLog2LinearBit = nLog2LinearBit;

	m_nYList[0] = 0;
	for (i = 1; i < 32; i++)
	{
		fx = (double) i / 32.0;
		fy = log(fx+1.0) / log(2.0);
		m_nYList[i] = (int)(fy*m_nLog2+0.5);
	}
	m_nYList[32] = m_nLog2;

	m_nXList[0] = (1 << (nLog2LinearBit - 1));
	for (i = 1; i < 32; i++)
	{
		fy = (double) i / 32.0;
		fx = pow(2.0, fy);
		m_nXList[i] = (int)(fx*m_nXList[0] + 0.5);
	}
	m_nXList[32] = (1 << nLog2LinearBit);

	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, "LogTransfrom.csv", "wt");
//	if ( err== 0)return false;
	if (fp == NULL)return false;

	for (i = 0; i < 33; i++)
	{
		fprintf_s(fp, "%d,%d\n", m_nYList[i], m_nXList[i]);
	}

	fclose(fp);
	return true;
}

int CLinearLogTransform::Linear2Log(int nX)
{
	if (nX < 0)return 0;
	if (nX >= (1 << m_nLinear2LogBit) - 1)return (m_nLinear2LogBit*m_nLog2);
	
	int i, nY;
	int C = m_nLinear2LogBit - 1;
	int M = (1 << C);
	int nShift = (m_nLinear2LogBit - 6);
	int nMask = (1 << nShift) - 1;
	int nHalf = 1 << (nShift - 1);

	nX+=1;
	while ((nX&M) == 0)
	{
		nX <<= 1;
		C--;
	}

	nX -= M;
	
	i = nX >> nShift;
	nX = (nX&nMask);

	nY = m_nYList[i] + (((m_nYList[i + 1] - m_nYList[i])*nX + nHalf) >> nShift);
	nY += (C<<m_nLog2Bit);

	return nY;
}

int CLinearLogTransform::Log2Linear(int nY)
{
	if (nY < 0)return 1;
	
	int i, nX;
	int M = (1 << m_nLog2Bit) - 1;
	int C = nY >> m_nLog2Bit;
	nY = nY&M;

	int nShift = (m_nLog2Bit - 5);
	int nMask = (1 << nShift) - 1;
	int nHalf = 1 << (nShift - 1);
	
	i = (nY >> nShift);
	nY = nY&nMask;
	
	nX = m_nXList[i] + (((m_nXList[i + 1] - m_nXList[i])*nY+nHalf) >> nShift);

	if (C >= m_nLog2LinearBit)
	{
		C -= m_nLog2LinearBit-1;
		nX <<= C;
	}
	else if(C<m_nLog2LinearBit-1)
	{
		C=m_nLog2LinearBit - 1 - C;
		nX >>= C;
	}

	return nX;
}