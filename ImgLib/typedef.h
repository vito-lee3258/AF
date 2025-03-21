#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_


#ifndef _WIN32
#include <stdio.h>

typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef long long      __int64;
typedef int            errno_t;
typedef long           LONG;
typedef unsigned long  DWORD;
#define __int32        int

#define BI_RGB         0L

__inline errno_t fopen_s(FILE ** streamptr,
                const char * filename,
                const char * mode)
		{
				*streamptr = fopen(filename, mode);
				return 0;
		}
		
__inline errno_t fread_s(void* _buffer,
                size_t bufSize,
				size_t eleSize,
				size_t eleCount,
				FILE*  stream)
		{
				fread(_buffer, eleSize, eleCount, stream);
				return 0;
		}

/*
__inline int sprintf_s(
	char*       _Buffer,
	size_t      _BufferCount,
	const char* _Format,
	const char*  str)
{
	sprintf(_Buffer, _Format, str);
	return 0;
}

__inline int sprintf_s(
	char*       _Buffer,
	size_t      _BufferCount,
	const char* _Format,
	int& i)
{
	sprintf(_Buffer, _Format, i);
	return 0;
}

__inline int sprintf_s(
	char*       _Buffer,
	const char* _Format,
	const char* str)
{
	sprintf(_Buffer, _Format, str);
	return 0;
}

__inline int sprintf_s(
	char*       _Buffer,
	const char* _Format,
	const char* str,
	int i)
{
	sprintf(_Buffer, _Format, str, i);
	return 0;
}
*/



#define sprintf_s sprintf	
#define fscanf_s(fp, format, str, num) fscanf(fp, format, str) 
#define strcpy_s  strcpy		
#define strcat_s  strcat
#define sscanf_s  sscanf
#define fprintf_s fprintf
#define _countof(a)   (sizeof(a)/sizeof(*(a)))

#pragma pack(push) 
#pragma pack(2)
typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

#pragma pack()

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

#pragma pack(pop)

#endif	
#endif