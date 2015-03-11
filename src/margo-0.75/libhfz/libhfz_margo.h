#ifndef __LIBHFZ_INCLUDED__
#define __LIBHFZ_INCLUDED__

#pragma once

#include <stdint.h>
//#include "/home/diezel/tech/zlib-1.2.7/zlib.h"
#include "zlib/zlib.h"


#define LIBHFZ_STATUS_OK				0
#define LIBHFZ_ERROR_OPEN_FAILED		-1
#define LIBHFZ_ERROR_WRITE_FAILED		-2
#define LIBHFZ_ERROR_READ_FAILED		-3
#define LIBHFZ_ERROR_ALLOC_FAILED		-4
#define LIBHFZ_ERROR_INVALID_PARAM		-5
#define LIBHFZ_ERROR_INVALID_HANDLE		-6
#define LIBHFZ_ERROR_INVALID_FORMAT		-7
#define LIBHFZ_ERROR_INVALID_PRECIS		-8
#define LIBHFZ_ERROR_INVALID_MAP_SIZE	-9
#define LIBHFZ_ERROR_INVALID_TILE_SIZE	-10
#define LIBHFZ_ERROR_INVALID_EXT_HEADER		-11
#define LIBHFZ_ERROR_WRITE_HEADER_FAILED	-12
#define LIBHFZ_ERROR_WRITE_EXTHEAD_FAILED	-13
#define LIBHFZ_USER_ABORT	-14

#define LIBHFZ_BYTEORDER_LITTLEENDIAN	0 // default
#define LIBHFZ_BYTEORDER_INTEL			0
#define LIBHFZ_BYTEORDER_WINDOWS		0
#define LIBHFZ_BYTEORDER_BIGENDIAN		1
#define LIBHFZ_BYTEORDER_MOTOROLA		1
#define LIBHFZ_BYTEORDER_UNIX			1

typedef enum { LIBHFZ_FORMAT_HF2, LIBHFZ_FORMAT_HF2_GZ } hfzFormat;

// extended header block
typedef struct {
	char BlockType[4];
	char BlockName[16];
	uint32_t BlockLength;
	void* pBlockData;
} hfzExtHeaderBlock;

// header block
typedef struct {
	unsigned short FileVersionNo;
	uint32_t nx;
	uint32_t ny;
	unsigned short TileSize;
	float HorizScale;
	float Precis;
	uint32_t ExtHeaderLength;
	uint32_t nExtHeaderBlocks;
	hfzExtHeaderBlock* pExtHeaderBlocks;
} hfzHeader;

// file handle struct
typedef struct {
	hfzFormat Format;
	//struct _iobuf *raw;
	//gzFile gz;
	void* pIoStream;
} hfzFile;

// function handle definitions
//typedef long (__stdcall* LIBHFZ_PROG_CALLBACK)(float Progress, void* lpCallbackParam);
//typedef void* (__stdcall* LIBHFZ_CUSTOM_MALLOC)(long nBytes);
//typedef void* (__stdcall* LIBHFZ_CUSTOM_MEMCPY)(void* pDest, const void* pSrc, long nBytes);
//typedef void (__stdcall* LIBHFZ_CUSTOM_FREE)(void* pMem);
//typedef void* (__stdcall* LIBHFZ_CUSTOM_FOPEN)(const char* lpFileName, const char* lpAttrib);
//typedef long (__stdcall* LIBHFZ_CUSTOM_FWRITE)(void* pIoStream, const void* pData, long nBytes);
//typedef long (__stdcall* LIBHFZ_CUSTOM_FREAD)(void* pIoStream, void* pData, long nBytes);
//typedef long (__stdcall* LIBHFZ_CUSTOM_FCLOSE)(void* pIoStream);
//
// high-level
//long hfzSave(const char* lpFileName, hfzFormat Format, hfzHeader& fh, float* pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam);
//long hfzLoad(const char* lpFileName, hfzHeader& fh, float * pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam); // user must allocate map mem
//long hfzLoadEx(const char* lpFileName, hfzHeader& fh, float ** h_pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam); // allocates map mem (user must free)
int32_t hfzSave(const char* lpFileName, hfzFormat Format, hfzHeader& fh, float* pData);
int32_t hfzLoad(const char* lpFileName, hfzHeader& fh, float * pData); // user must allocate map mem
int32_t hfzLoadEx(const char* lpFileName, hfzHeader& fh, float ** h_pData); // allocates map mem (user must free)


// medium-level
int32_t hfzReadHeader(hfzFile* fs, hfzHeader& fh);
int32_t hfzReadHeader2(const char* lpFileName, hfzHeader& fh);
int32_t hfzReadTile(hfzFile* fs, hfzHeader& fh, uint32_t TileX, uint32_t TileY, float* pMapData);
int32_t hfzReadTile2(hfzFile* fs, hfzHeader& fh, uint32_t TileX, uint32_t TileY, float* pTileData);

int32_t hfzWriteHeader(hfzFile* fs, hfzHeader& fh);
int32_t hfzWriteTile(hfzFile* fs, hfzHeader& fh, uint32_t TileX, uint32_t TileY, float* pMapData);
int32_t hfzWriteTile2(hfzFile* fs, hfzHeader& fh, uint32_t TileX, uint32_t TileY, float* pTileData);

// low-level
hfzFile* hfzOpen(const char* lpFileName, hfzFormat FormatID, const char* lpMode);
void hfzClose(hfzFile* fs);
int32_t hfzWrite(hfzFile* fs, const void* pData, int32_t len);
int32_t hfzRead(hfzFile* fs, void* pData, int32_t len);

void* hfzMalloc(int32_t nBytes);
void* hfzMemcpy(void* pDest, const void* pSrc, int32_t nBytes);
void hfzFree(void* pData);

// helpers
const char* hfzGetErrorStr(int32_t ErrorCode);

int32_t hfzHeader_Init(hfzHeader &fh, uint32_t nx, uint32_t ny, unsigned short TileSize, float Precis, float HorizScale, uint32_t nExtHeaderBlocks);
void hfzHeader_Reset(hfzHeader &fh); // clears mem in ext header buf and restores defaults
int32_t hfzHeader_EncodeExtHeaderBuf(hfzHeader &fh, char** ppBuf); 
int32_t hfzHeader_DecodeExtHeaderBuf(hfzHeader &fh, char* pBuf); 

int32_t hfzExtHeaderBlock_Init(hfzExtHeaderBlock* pBlock, const char* lpBlockType, const char* lpBlockName, uint32_t BlockDataLength, void* pBlockData);
int32_t hfzExtHeaderBlock_InitEx(hfzHeader& fh, uint32_t BlockID, const char* lpBlockType, const char* lpBlockName, uint32_t BlockDataLength, void* pBlockData);
void hfzExtHeaderBlock_Reset(hfzExtHeaderBlock* pBlock);

int32_t hfzSetLocalByteOrder(int32_t ByteOrder = LIBHFZ_BYTEORDER_LITTLEENDIAN);
int32_t hfzByteSwap(void* pData, uint32_t DataSize);

#endif
