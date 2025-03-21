#include "Filter.h"

void CFilterParameterList::Clear()
{
	while (m_pFirstItem != NULL)
	{
		TParamListItem *pNext = m_pFirstItem->pNext;
		delete m_pFirstItem;

		m_pFirstItem = pNext;
	}
	m_pLastItem = NULL;
	m_nCount = 0; 
}

void CFilterParameterList::ClearFlag()
{
	TParamListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		pItem->bFlag = false;
		pItem = pItem->pNext;
	}
}

bool CFilterParameterList::CheckFlag()
{
	bool bFlag=true;

	TParamListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!pItem->bFlag)
		{
			printf("Missing to load %s!!!\n", pItem->sName);
		}
		bFlag = bFlag && (pItem->bFlag);

		pItem = pItem->pNext;
	}
	return bFlag;
}

bool CFilterParameterList::Add(char *pName, int *pVal, int nMin, int nMax, char *pHint)
{
	TParamListItem *pItem = new TParamListItem;

	if (pItem == NULL)return false;

	strcpy_s(pItem->sName, pName);
	pItem->pVal = pVal;
	pItem->nMin = nMin;
	pItem->nMax = nMax;
	if (pHint != NULL)
	{
		strcpy_s(pItem->sHint, pHint);
	}
	else
	{
		pItem->sHint[0] = 0;
	}

	if (m_nCount == 0)
	{
		m_pFirstItem = m_pLastItem = pItem;
	}
	else
	{
		m_pLastItem->pNext = pItem;
		m_pLastItem = pItem;
	}
	m_nCount++;

	return true;
}

bool CFilterParameterList::AddVector(char *pVectorName, int *pVal, int nDim, int nMin, int nMax, char *pHint)
{
	int i;
	char pName[64];
	sprintf_s(pName, "%s_0", pVectorName);
	if (!Add(pName, pVal, nMin, nMax, pHint))return false;

	for (i = 1; i < nDim; i++)
	{
		sprintf_s(pName, "%s_%d", pVectorName, i);
		if (!Add(pName, pVal + i, nMin, nMax))return false;
	}

	return true;
}

bool CFilterParameterList::AddMatrixRow(char *pMatrixName, int *pVal, int nRow, int nCol, int nMin, int nMax, char *pHint)
{
	char pName[64];

	sprintf_s(pName, "%s_%d", pMatrixName, nRow);
	if (nRow == 0)
	{
		if(!AddVector(pName, pVal, nCol, nMin, nMax, pHint))return false;
	}
	else
	{
		if (!AddVector(pName, pVal, nCol, nMin, nMax))return false;
	}

	return true;
}

bool CFilterParameterList::FindNameValue(char *pStr, char *pName, int &Val)
{
	unsigned int i;

	size_t len = strlen(pStr);
	for (i = 0; i<len; i++)
	{
		if (pStr[i] == '=')
		{
			if (sscanf_s(pStr + i + 1, "%d", &Val) != 1)return false;
			memcpy(pName, pStr, i);
			pName[i] = '\0';

			return true;
		}
	}

	return false;
}

bool CFilterParameterList::SetParam(char *pName, int Val)
{
	TParamListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (strcmp(pItem->sName, pName) == 0)
		{
			if (pItem->bFlag)
			{
				printf("%s has been loaded more than once!!!\n", pName);
				return false;
			}
			if (Val<pItem->nMin || Val>pItem->nMax)
			{
				printf("%s value %d is out of range [%d,%d]!!!\n", pName, Val, pItem->nMin, pItem->nMax);
				return false;
			}

			pItem->bFlag = true;
			*(pItem->pVal) = Val;
			return true;
		}
		pItem = pItem->pNext;
	}
	printf("Can not find parameter name %s!!!\n", pName);
	return false;
}

bool CFilterParameterList::SaveToFile(FILE *fp)
{
	if (fp == NULL)return false;

	TParamListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		fprintf(fp, "%s=%d;\t\tRange=[%d,%d] %s\n", pItem->sName, *(pItem->pVal), pItem->nMin, pItem->nMax, pItem->sHint);
		pItem = pItem->pNext;
	}

	return true;
}

bool CFilterParameterList::LoadFromFile(FILE *fp)
{
	int i;
	char str[1024], name[64];
	int val;

	if (fp == NULL)return false;

	ClearFlag();
	for (i = 0; i<m_nCount; i++)
	{
		fgets(str, 1024, fp);
		if (!FindNameValue(str, name, val))
		{
			printf("FindNameValue from %s failed!!!\n", str);

			CheckFlag();
			return false;
		}

		if (!SetParam(name, val))
		{
			CheckFlag();
			return false;
		}
	}

	return CheckFlag();
}

bool CFilter::LoadParameterFromStream(FILE *fp)
{
	char sFilterName[64];

	fscanf_s(fp, "\n%s\n", sFilterName, _countof(sFilterName));
	if (strcmp(m_sFilterName, sFilterName)!=0)
	{
		printf("Filter %s find non-matched name %s!!!\n", m_sFilterName, sFilterName);
		return false;
	}

	if (!m_ParamList.LoadFromFile(fp))return false;

	return UpdateInternalParam();
}

bool CFilter::LoadParameterFile(char *pFileName)
{
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, pFileName, "rt");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	if (!LoadParameterFromStream(fp))
	{
		printf("Filter %s loading parameter fail!!!\n", m_sFilterName);
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool CFilter::SaveParameterToStream(FILE *fp)
{
	fprintf_s(fp, "\n%s\n", m_sFilterName);
	if (!m_ParamList.SaveToFile(fp))return false;

	return true;
}

bool CFilter::SaveParameterFile(char *pFileName)
{
	FILE *fp = NULL;
	errno_t err = fopen_s(&fp, pFileName, "wt");
	if (fp == NULL)
	{
		printf("Open file %s fail!!! errcode=%d\n", pFileName, err);
		return false;
	}

	if (!SaveParameterToStream(fp))
	{
		printf("Filter %s saving parameters fail!!!\n", m_sFilterName);
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

void CMultipleFilterProcessFlow::ClearFilterList()
{
	while (m_pFirstItem != NULL)
	{
		TFilterListItem *pItem = m_pFirstItem->pNext;
		delete m_pFirstItem;
		m_pFirstItem = pItem;
	}
	m_pLastItem = NULL;
	m_nCount = 0; 
}

bool CMultipleFilterProcessFlow::AddFilter(CFilter *pFilter, char *pNewName)
{
	if (pFilter == NULL)return false;

	pFilter->ModifyName(pNewName);
	
	TFilterListItem *pItem = new TFilterListItem;
	if (pItem == NULL)return false;

	pItem->pFilter = pFilter;
	pItem->bFlag = false;

	if (m_nCount == 0)
	{
		m_pFirstItem = m_pLastItem = pItem;
	}
	else
	{
		m_pLastItem->pNext = pItem;
		m_pLastItem = pItem;
	}
	m_nCount++;

	return true;
}

void CMultipleFilterProcessFlow::ClearFlag()
{
	TFilterListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		pItem->bFlag = false;
		pItem = pItem->pNext;
	}
}

bool CMultipleFilterProcessFlow::CheckFlag()
{
	bool bFlag = true;
	TFilterListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!pItem->bFlag)
		{
			printf("Filter %s is missing to load parameters!!!\n", pItem->pFilter->GetFilterName());
		}
		bFlag = (bFlag && (pItem->bFlag));
		pItem = pItem->pNext;
	}

	return bFlag;
}

bool CMultipleFilterProcessFlow::SaveParameterToStream(FILE *fp)
{
	fprintf_s(fp, "\n%s\n", m_sFilterName);
	if (!m_ParamList.SaveToFile(fp))
	{
		printf("Filter %s saving parameters fail!!!\n", m_sFilterName);
		return false;
	}

	TFilterListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!pItem->pFilter->SaveParameterToStream(fp))return false;

		pItem = pItem->pNext;
	}

	return true;
}

bool CMultipleFilterProcessFlow::LoadParameterFromStream(FILE *fp)
{
	char sFilterName[64];

	fscanf_s(fp, "\n%s\n", sFilterName, _countof(sFilterName));
	if (strcmp(m_sFilterName, sFilterName)!=0)
	{
		printf("Filter %s find non-matched name %s!!!\n", m_sFilterName, sFilterName);
		return false;
	}

	if (!m_ParamList.LoadFromFile(fp))return false;
	if (!UpdateInternalParam())return false;

	ClearFlag();

	TFilterListItem *pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!pItem->pFilter->LoadParameterFromStream(fp))
		{
			CheckFlag();
			return false;
		}
		pItem->bFlag = true;
		pItem = pItem->pNext;
	}

	return CheckFlag();
}
