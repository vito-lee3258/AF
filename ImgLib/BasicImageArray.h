#ifndef __BASIC_IMAGE_ARRAY_H_
#define __BASIC_IMAGE_ARRAY_H_

#include "BasicType.h"

template <class T> class CBasicImageArray
{
protected:
	int m_nWidth;
	int m_nHeight;
	int m_nDim;
	T *m_pData;

	__inline T Avg3(T In0, T In1, T In2)
	{
		return (In0 + In1 * 2 + In2) / 4;
	}
	__inline T Max3(T In0, T In1, T In2)
	{
		if (In0 < In1)
		{
			if (In2 < In1)
			{
				return In1;
			}
			else
			{
				return In2;
			}
		}
		else
		{
			if (In2 < In0)
			{
				return In0;
			}
			else
			{
				return In2;
			}
		}
	}
	__inline T Min3(T In0, T In1, T In2)
	{
		if (In0 < In1)
		{
			if (In2 < In0)
			{
				return In2;
			}
			else
			{
				return In0;
			}
		}
		else
		{
			if (In2 < In1)
			{
				return In2;
			}
			else
			{
				return In1;
			}
		}
	}
	__inline T Mid3(T In0, T In1, T In2)
	{
		if (In0 < In1)
		{
			if (In2 < In0)
			{
				return In0;
			}
			else if (In1 < In2)
			{
				return In1;
			}
			else
			{
				return In2;
			}
		}
		else
		{
			if (In2 < In1)
			{
				return In1;
			}
			else if (In0 < In2)
			{
				return In0;
			}
			else
			{
				return In2;
			}
		}
	}

	void RotateLine(T *pInLine, T *pOutLine, int nChannel, int nPitch = 3)
	{
		int x;

		for (x = 0; x < m_nWidth; x++)
		{
			pOutLine[0] = pInLine[nChannel];
			pInLine += m_nDim;
			pOutLine += nPitch;
		}
	}

	void HAvgLine(T *pLine, int ch)
	{
		int x; 
		T In[3];
		T *pIn = pLine;
		T *pOut = pLine;

		In[0] = In[1] = pIn[ch];	pIn += m_nDim;

		for (x = 0; x < m_nWidth - 1; x++)
		{
			In[2] = pIn[ch];	pIn += m_nDim;

			pOut[ch] = Avg3(In[0], In[1], In[2]);
			pOut += m_nDim;

			In[0] = In[1];
			In[1] = In[2];
		}
		{
			pOut[ch] = Avg3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}
	
	void HMidLine(T *pLine, int ch)
	{
		int x;
		T In[3];
		T *pIn = pLine;
		T *pOut = pLine;
		In[0] = In[1] = pIn[ch];	pIn += m_nDim;

		for (x = 0; x < m_nWidth - 1; x++)
		{
			In[2] = pIn[ch];	pIn += m_nDim;

			pOut[ch] = Mid3(In[0], In[1], In[2]);
			pOut += m_nDim;

			In[0] = In[1];
			In[1] = In[2];
		}
		{
			pOut[ch] = Mid3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void HMaxLine(T *pLine, int ch)
	{
		int x;
		T In[3];
		T *pIn = pLine;
		T *pOut = pLine;
		In[0] = In[1] = pIn[ch];	pIn += m_nDim;

		for (x = 0; x < m_nWidth - 1; x++)
		{
			In[2] = pIn[ch];	pIn += m_nDim;

			pOut[ch] = Max3(In[0], In[1], In[2]);
			pOut += m_nDim;

			In[0] = In[1];
			In[1] = In[2];
		}
		{
			pOut[ch] = Max3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void HMinLine(T *pLine, int ch)
	{
		int x;
		T In[3];
		T *pIn = pLine;
		T *pOut = pLine;
		In[0] = In[1] = pIn[ch];	pIn += m_nDim;

		for (x = 0; x < m_nWidth - 1; x++)
		{
			In[2] = pIn[ch];	pIn += m_nDim;

			pOut[ch] = Min3(In[0], In[1], In[2]);
			pOut += m_nDim;

			In[0] = In[1];
			In[1] = In[2];
		}
		{
			pOut[ch] = Min3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void VAvgLine(T *pInLines[], T *pOutLine, int ch)
	{
		int i, x;
		T In[3];
		T *pIn[3];
		T *pOut = pOutLine;

		for (i = 0; i < 3; i++)
		{
			pIn[i] = pInLines[i];
		}

		for (x = 0; x < m_nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				In[i] = pIn[i][0];
				pIn[i] += 3;
			}
			pOut[ch] = Avg3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void VMidLine(T *pInLines[], T *pOutLine, int ch)
	{
		int i, x;
		T In[3];
		T *pIn[3];
		T *pOut = pOutLine;

		for (i = 0; i < 3; i++)
		{
			pIn[i] = pInLines[i];
		}

		for (x = 0; x < m_nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				In[i] = pIn[i][0];
				pIn[i] += 3;
			}
			pOut[ch] = Mid3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void VMaxLine(T *pInLines[], T *pOutLine, int ch)
	{
		int i, x;
		T In[3];
		T *pIn[3];
		T *pOut = pOutLine;

		for (i = 0; i < 3; i++)
		{
			pIn[i] = pInLines[i];
		}

		for (x = 0; x < m_nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				In[i] = pIn[i][0];
				pIn[i] += 3;
			}
			pOut[ch] = Max3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

	void VMinLine(T *pInLines[], T *pOutLine, int ch)
	{
		int i, x;
		T In[3];
		T *pIn[3];
		T *pOut = pOutLine;

		for (i = 0; i < 3; i++)
		{
			pIn[i] = pInLines[i];
		}

		for (x = 0; x < m_nWidth; x++)
		{
			for (i = 0; i < 3; i++)
			{
				In[i] = pIn[i][0];
				pIn[i] += 3;
			}
			pOut[ch] = Min3(In[0], In[1], In[2]);
			pOut += m_nDim;
		}
	}

public:
	CBasicImageArray()
	{
		m_nWidth = m_nHeight = m_nDim = 0;
		m_pData = NULL;
	}
	~CBasicImageArray()
	{
		if (m_pData != NULL)delete[] m_pData;
	}
	void ClearImage()
	{
		if (m_pData != NULL)delete[] m_pData;
		m_nWidth = m_nHeight = m_nDim = 0;
		m_pData = NULL;
	}
	bool SetImageSize(int W, int H, int D)
	{
		ClearImage();
		m_pData = new T[W*H*D];
		if (m_pData == NULL)return false;

		for (int i = 0; i < W*H*D; i++)
		{
			m_pData[i] = 0;
		}

		m_nWidth = W;
		m_nHeight = H;
		m_nDim = D;
		return true;
	}
	void FillValue(T val)
	{
		int i, x, y;

		T *pD = m_pData;
		if (pD == NULL)return;

		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth; x++)
			{
				for (i = 0; i < m_nDim; i++)
				{
					*(pD++) = val;
				}
			}
		}
	}
	void FillVector(T vec[])
	{
		int i, x, y;

		T *pD = m_pData;
		if (pD == NULL)return;

		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth; x++)
			{
				for (i = 0; i < m_nDim; i++)
				{
					*(pD++) = vec[i];
				}
			}
		}
	}
	
	T * GetImageData() { return m_pData; };
	int GetWidth() { return m_nWidth; };
	int GetHeight() { return m_nHeight; };
	int GetDim() { return m_nDim; };
	T * GetImageLine(int nY)
	{
		if (nY < 0)nY = 0;	if (nY >= m_nHeight)nY = m_nHeight - 1;
		return m_pData + nY*m_nWidth*m_nDim;
	}
	__inline T * GetPixelAt(int nX, int nY)
	{
		if (nX < 0)nX = 0; if (nX >= m_nWidth)nX = m_nWidth - 1;
		if (nY < 0)nY = 0; if (nY >= m_nHeight)nY = m_nHeight - 1;
		return m_pData + (nY*m_nWidth + nX)*m_nDim;
	}
	bool Dump2TextFile(char *pFileName)
	{
		int i, x, y;
		FILE *fp = NULL;
		
		errno_t err=fopen_s(&fp, pFileName, "wt");

		if (fp == NULL)
		{
			printf("Open file %s fail!!! err=%d\n", pFileName, err);
			return false;
		}

		T *pData = m_pData;
		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth; x++)
			{
				for (i = 0; i < m_nDim; i++)
				{
					fprintf(fp, "%x", pData[i]);
				}
				pData += m_nDim;
			}
		}

		fclose(fp);
		return true;
	}

	virtual bool HFlip()
	{
		int i, x, y;
		T g;

		if (m_pData == NULL)return false;

		for (y = 0; y<m_nHeight; y++)
		{
			T *ptr1 = GetImageLine(y);
			T *ptr2 = ptr1 + (m_nWidth - 1)*m_nDim;
			for (x = 0; x<m_nWidth / 2; x++)
			{
				for (i = 0; i<m_nDim; i++)
				{
					g = ptr1[i];
					ptr1[i] = ptr2[i];
					ptr2[i] = g;
				}
				ptr1 += m_nDim;
				ptr2 -= m_nDim;
			}
		}

		return true;
	}

	virtual bool VFlip()
	{
		int i, x, y;
		T g;

		if (m_pData == NULL)return false;

		for (y = 0; y<m_nHeight / 2; y++)
		{
			T *ptr1 = GetImageLine(y);
			T *ptr2 = GetImageLine(m_nHeight - 1 - y);
			for (x = 0; x<m_nWidth; x++)
			{
				for (i = 0; i<m_nDim; i++)
				{
					g = ptr1[i];
					ptr1[i] = ptr2[i];
					ptr2[i] = g;
				}
				ptr1 += m_nDim;
				ptr2 += m_nDim;
			}
		}

		return true;
	}

	virtual bool Trans()
	{
		if (m_pData == NULL)return false;

		int i, x, y;
		int nW = m_nHeight;
		int nH = m_nWidth;
		int nD = m_nDim;

		T *pBuffer = new T[nW*nH*nD];
		if (pBuffer == NULL)return false;

		T *pOut = pBuffer;
		for (y = 0; y<nH; y++)
		{
			for (x = 0; x<nW; x++)
			{
				T *pIn = GetPixelAt(y, x);
				for (i = 0; i<nD; i++)
				{
					*(pOut++) = pIn[i];
				}
			}
		}

		delete[] m_pData;
		m_nWidth = nW;
		m_nHeight = nH;
		m_nDim = nD;
		m_pData = pBuffer;

		return true;
	}

	virtual bool Rotate90(CBasicImageArray<T> *pOutImage = NULL, bool bOnlyY = false)
	{
		int i, x, y;
		int nW = m_nWidth;
		int nH = m_nHeight;
		int nD = m_nDim;
		int nOutD = bOnlyY ? 1 : nD;
		T *pOut, *pIn;

		if (pOutImage != NULL)
		{
			if (!pOutImage->SetImageSize(nH, nW, nOutD))return false;
			pOut = pOutImage->GetImageData();
		}
		else
		{
			pOut = new T[nH*nW*nOutD];
			if (pOut == NULL)return false;
		}
		pIn = m_pData;

		for (y = 0; y < nH; y++)
		{
			for (x = 0; x < nW; x++)
			{
				for (i = 0; i < nOutD; i++)
				{
					pOut[(x*nH + nH - 1 - y)*nOutD + i] = pIn[i];
				}
				pIn += nD;
			}
		}

		if (pOutImage == NULL)
		{
			delete[] m_pData;
			m_pData = pOut;
			m_nWidth = nH;
			m_nHeight = nW;
			m_nDim = nOutD;
		}
		return true;
	}

	virtual bool Rotate270(CBasicImageArray<T> *pOutImage = NULL, bool bOnlyY = false)
	{
		int i, x, y;
		int nW = m_nWidth;
		int nH = m_nHeight;
		int nD = m_nDim;
		int nOutD = bOnlyY ? 1 : nD;
		T *pOut, *pIn;

		if (pOutImage != NULL)
		{
			if (!pOutImage->SetImageSize(nH, nW, nOutD))return false;
			pOut = pOutImage->GetImageData();
		}
		else
		{
			pOut = new T[nH*nW*nOutD];
			if (pOut == NULL)return false;
		}
		pIn = m_pData;

		for (y = 0; y < nH; y++)
		{
			for (x = 0; x < nW; x++)
			{
				for (i = 0; i < nOutD; i++)
				{
					pOut[((nW - 1 - x)*nH + y)*nOutD + i] = pIn[i];
				}
				pIn += nD;
			}
		}

		if (pOutImage == NULL)
		{
			delete[] m_pData;
			m_pData = pOut;
			m_nWidth = nH;
			m_nHeight = nW;
			m_nDim = nOutD;
		}
		return true;
	}

	virtual bool Rotate180(CBasicImageArray<T> *pOutImage = NULL, bool bOnlyY = false)
	{
		int i, x, y;
		int nW = m_nWidth;
		int nH = m_nHeight;
		int nD = m_nDim;
		int nOutD = bOnlyY ? 1 : nD;
		T *pOut, *pIn;

		if (pOutImage != NULL)
		{
			if (!pOutImage->SetImageSize(nW, nH, nOutD))return false;
			pOut = pOutImage->GetImageData();
		}
		else
		{
			pOut = new T[nW*nH*nOutD];
			if (pOut == NULL)return false;
		}
		pIn = m_pData;

		for (y = 0; y < nH; y++)
		{
			for (x = 0; x < nW; x++)
			{
				for (i = 0; i < nOutD; i++)
				{
					pOut[((nH - 1 - y)*nW + nW - 1 - x)*nOutD + i] = pIn[i];
				}
				pIn += nD;
			}
		}

		if (pOutImage == NULL)
		{
			delete[] m_pData;
			m_pData = pOut;
			m_nDim = nOutD;
		}
		return true;
	}

	virtual bool Crop(int nLeft_Margin, int nTop_Margin, int nRight_Margin, int nBottom_Margin)
	{
		if (m_pData == NULL)return false;

		if (nLeft_Margin<0)nLeft_Margin = 0;		if (nLeft_Margin >= m_nWidth)nLeft_Margin = m_nWidth - 1;
		if (nRight_Margin<0)nRight_Margin = 0;		if (nRight_Margin >= m_nWidth)nRight_Margin = m_nWidth - 1;
		if (nTop_Margin<0)nTop_Margin = 0;			if (nTop_Margin >= m_nHeight)nTop_Margin = m_nHeight - 1;
		if (nBottom_Margin<0)nBottom_Margin = 0;	if (nBottom_Margin >= m_nHeight)nBottom_Margin = m_nHeight - 1;

		int i, x, y;
		int nLeft = nLeft_Margin;
		int nTop = nTop_Margin;
		int nW = m_nWidth - nLeft_Margin - nRight_Margin;
		int nH = m_nHeight - nTop_Margin - nBottom_Margin;
		int nD = m_nDim;

		if ((nW<1) || (nH<1))return false;

		if (nW == m_nWidth&&nH == m_nHeight)return true;

		//T *pBuffer = new T[nW*nH*nD];
		//if (pBuffer == NULL)return false;
		//T *pOut = pBuffer;

		T *pOut = m_pData;
		for (y = 0; y<nH; y++)
		{
			T *pIn = GetImageLine(y + nTop) + nLeft*nD;
			for (x = 0; x<nW; x++)
			{
				for (i = 0; i<nD; i++)
				{
					*(pOut++) = *(pIn++);
				}
			}
		}

//		delete[] m_pData;
		m_nWidth = nW;
		m_nHeight = nH;
		m_nDim = nD;
//		m_pData = pBuffer;

		return true;
	}

	bool HAvgFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		for (y = 0; y < m_nHeight; y++)
		{
			T *pLine = GetImageLine(y);
			HAvgLine(pLine, ch);
		}

		return true;
	}

	bool HMidFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		for (y = 0; y < m_nHeight; y++)
		{
			T *pLine = GetImageLine(y);
			HMidLine(pLine, ch);
		}

		return true;
	}

	bool HMaxFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		for (y = 0; y < m_nHeight; y++)
		{
			T *pLine = GetImageLine(y);
			HMaxLine(pLine, ch);
		}

		return true;
	}

	bool HMinFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		for (y = 0; y < m_nHeight; y++)
		{
			T *pLine = GetImageLine(y);
			HMinLine(pLine, ch);
		}

		return true;
	}

	bool VAvgFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL || m_nHeight < 3)return false;

		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		T *pInLines[3];
		T *pBuffer = new T[m_nWidth * 3];
		if (pBuffer == NULL)return false;

		pInLines[0] = pInLines[1] = pBuffer;
		pInLines[2] = pInLines[1] + 1;

		T *pInLine = GetImageData();
		T *pOutLine = pInLine;

		RotateLine(pInLine, pInLines[1], ch);
		pInLine += m_nWidth*m_nDim;

		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VAvgLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] += 1;
		}

		for (y = 1; y < m_nHeight - 1; y++)
		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VAvgLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			T *pTemp = pInLines[0];
			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] = pTemp;
		}

		{
			pInLines[2] = pInLines[1];

			VAvgLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;
		}

		delete[] pBuffer;
		return true;
	}

	bool VMidFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL || m_nHeight < 3)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		T *pInLines[3];
		T *pBuffer = new T[m_nWidth * 3];
		if (pBuffer == NULL)return false;

		pInLines[0] = pInLines[1] = pBuffer;
		pInLines[2] = pInLines[1] + 1;

		T *pInLine = GetImageData();
		T *pOutLine = pInLine;

		RotateLine(pInLine, pInLines[1], ch);
		pInLine += m_nWidth*m_nDim;

		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMidLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] += 1;
		}

		for (y = 1; y < m_nHeight - 1; y++)
		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMidLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			T *pTemp = pInLines[0];
			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] = pTemp;
		}

		{
			pInLines[2] = pInLines[1];

			VMidLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;
		}

		delete[] pBuffer;
		return true;
	}

	bool VMaxFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL || m_nHeight < 3)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		T *pInLines[3];
		T *pBuffer = new T[m_nWidth * 3];
		if (pBuffer == NULL)return false;

		pInLines[0] = pInLines[1] = pBuffer;
		pInLines[2] = pInLines[1] + 1;

		T *pInLine = GetImageData();
		T *pOutLine = pInLine;

		RotateLine(pInLine, pInLines[1], ch);
		pInLine += m_nWidth*m_nDim;

		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMaxLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] += 1;
		}

		for (y = 1; y < m_nHeight - 1; y++)
		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMaxLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			T *pTemp = pInLines[0];
			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] = pTemp;
		}

		{
			pInLines[2] = pInLines[1];

			VMaxLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;
		}

		delete[] pBuffer;
		return true;
	}

	bool VMinFilterChannel(int ch)
	{
		int y;

		if (m_pData == NULL || m_nHeight < 3)return false;
		if (ch < 0)ch = 0;	if (ch >= m_nDim)ch = m_nDim - 1;

		T *pInLines[3];
		T *pBuffer = new T[m_nWidth * 3];
		if (pBuffer == NULL)return false;

		pInLines[0] = pInLines[1] = pBuffer;
		pInLines[2] = pInLines[1] + 1;

		T *pInLine = GetImageData();
		T *pOutLine = pInLine;

		RotateLine(pInLine, pInLines[1], ch);
		pInLine += m_nWidth*m_nDim;

		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMinLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] += 1;
		}

		for (y = 1; y < m_nHeight - 1; y++)
		{
			RotateLine(pInLine, pInLines[2], ch);
			pInLine += m_nWidth*m_nDim;

			VMinLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;

			T *pTemp = pInLines[0];
			pInLines[0] = pInLines[1];
			pInLines[1] = pInLines[2];
			pInLines[2] = pTemp;
		}

		{
			pInLines[2] = pInLines[1];

			VMinLine(pInLines, pOutLine, ch);
			pOutLine += m_nWidth*m_nDim;
		}

		delete[] pBuffer;
		return true;
	}
};

typedef CBasicImageArray<unsigned char> CBasicImageArray_BYTE;
typedef CBasicImageArray<unsigned short> CBasicImageArray_WORD;
typedef CBasicImageArray<short> CBasicImageArray_SHORT;
typedef CBasicImageArray<__int32> CBasicImageArray_INT32;
typedef CBasicImageArray<unsigned __int32> CBasicImageArray_UINT32;
typedef CBasicImageArray<__int64> CBasicImageArray_INT64;
typedef CBasicImageArray<unsigned __int64> CBasicImageArray_UINT64;
typedef CBasicImageArray<float> CBasicImageArray_FLOAT;

#endif

