#pragma once

#include "common.h"

class EXPORT_COMMON DataBuffer
{
public:
	DataBuffer(int dwBlockSize,int dwBlockNum);
	~DataBuffer();

	int WriteData(unsigned char* data,int dwSize);
	int ReadBlock(unsigned char* buf,int dwBufSize);

	//int WriteAndJump(unsigned char* data,int dwSize);
	//int ReadAndJump(unsigned char* data,int dwSize);

	bool ReadyForBufferMore() const;
	void Empty();

private:
	unsigned char** m_apBuffer;
	int m_dwBlockSize;
	int m_dwBlockNum;
	int m_dwFreeBlock;

	int m_dwCurRead;
	int m_dwCurWrite;
	int m_dwCurWritePos;

	//CriticalSection m_CurReadCSec;
};