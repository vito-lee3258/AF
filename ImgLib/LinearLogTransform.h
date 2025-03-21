#ifndef __LINEAR_LOG_TRANSFORM_H_
#define __LINEAR_LOG_TRANSFORM_H_

#include "BasicType.h"

class CLinearLogTransform
{
protected:
	int m_nLog2;
	int m_nLog2Bit;
	int m_nLinear2LogBit;
	int m_nLog2LinearBit;

	int m_nYList[33];
	int m_nXList[33];
public:

	bool Initialize(int nLog2Bit, int nLinear2LogBit, int nLog2LinearBit);

	int Linear2Log(int nX);
	int Log2Linear(int nY);

	int ComputeLogRation(int nRatio, int nShift)
	{
		while (nRatio > (1 << m_nLinear2LogBit))
		{
			nRatio >>= 1;
			nShift--;
		}
		return Linear2Log(nRatio - 1) - nShift*m_nLog2;
	}
	int ComputeLinearRatio(int nRatio, int nShift) 
	{
		return Log2Linear(nRatio + nShift*m_nLog2);
	}
};

#endif

