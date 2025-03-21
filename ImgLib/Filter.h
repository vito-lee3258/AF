#ifndef __FILTER_H_
#define __FILTER_H_

#include "BasicType.h"
#include "BasicImageArray.h"
#include "RGBYUVImage.h"
#include "GrayImage.h"
#include "BayerRAWImage.h"
#include "MultipleChannelImage.h"

class CFilterParameterList
{
protected:
	typedef struct tagParamListItem
	{
		bool bFlag;
		char sName[64];
		int *pVal;
		int nMin;
		int nMax;
		char sHint[256];
		tagParamListItem *pNext;
		tagParamListItem()
		{
			bFlag = false;
			pVal = NULL;
			nMin = nMax = 0;
			pNext = NULL;
		}
	}TParamListItem;

	int m_nCount;
	TParamListItem *m_pFirstItem;
	TParamListItem *m_pLastItem;

	void Clear();
	void ClearFlag();
	bool CheckFlag();
	bool SetParam(char *pName, int Val);
	bool FindNameValue(char *pStr, char *pName, int &Val);
public:
	CFilterParameterList()
	{
		m_nCount = 0;
		m_pFirstItem = m_pLastItem = NULL;
	}
	~CFilterParameterList()
	{
		Clear();
	}

	bool Add(char *pName, int *pVal, int nMin, int nMax, char *pHint = NULL);
	bool AddVector(char *pVectorName, int *pVal, int nDim, int nMin, int nMax, char *pHint = NULL);
	bool AddMatrixRow(char *pMatrixName, int *pVal, int nRow, int nCol, int nMin, int nMax, char *pHint = NULL);

	bool SaveToFile(FILE *fp);
	bool LoadFromFile(FILE *fp);
};

class CFilter
{
protected:
	char m_sFilterName[64];
	CFilterParameterList m_ParamList;

	virtual bool InitParamList()=0;
	virtual bool UpdateInternalParam() { return true; };
public:
	virtual bool Initialize(char *pFilterName)
	{
		strcpy_s(m_sFilterName, pFilterName);
		return InitParamList();
	}
	void ModifyName(char *pStr)
	{
		if (pStr != NULL)
		{
			strcat_s(m_sFilterName, pStr);
		}
	}
	bool MatchFilterName(char *pName)
	{
		if (strcmp(m_sFilterName, pName) == 0)return true;

		return false;
	}

	char * GetFilterName() { return m_sFilterName; }

	virtual bool LoadParameterFromStream(FILE *fp);
	virtual bool SaveParameterToStream(FILE *fp);

	bool LoadParameterFile(char *pFileName);
	bool SaveParameterFile(char *pFileName);
};

class CMultipleFilterProcessFlow : public CFilter
{
protected:
	typedef struct tagFilterListItem
	{
		CFilter *pFilter;
		bool bFlag;
		tagFilterListItem *pNext;

		tagFilterListItem()
		{
			pFilter = NULL;
			bFlag = false;
			pNext = NULL;
		}
	}TFilterListItem;

	int m_nCount;
	TFilterListItem *m_pFirstItem;
	TFilterListItem *m_pLastItem;

	void ClearFilterList();
	void ClearFlag();
	bool CheckFlag();
	bool AddFilter(CFilter *pFilter, char *pNewName=NULL);

	virtual bool CreateFilterList() = 0;
public:
	CMultipleFilterProcessFlow()
	{
		m_nCount = 0;
		m_pFirstItem = m_pLastItem = NULL;
	}
	~CMultipleFilterProcessFlow()
	{
		ClearFilterList();
	}

	virtual bool Initialize(char *pFilterName)
	{
		strcpy_s(m_sFilterName, pFilterName);
		if (!CreateFilterList())return false;
		return InitParamList();
	}

	virtual bool LoadParameterFromStream(FILE *fp);
	virtual bool SaveParameterToStream(FILE *fp);
};

#endif

