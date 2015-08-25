#include <memory.h>	//memcpy
#include "databuffer.hxx"

DataBuffer::DataBuffer(int dwBlockSize,int dwBlockNum)
:m_apBuffer(0),
 m_dwBlockSize(dwBlockSize),
 m_dwBlockNum(dwBlockNum),
 m_dwFreeBlock(dwBlockNum),
 m_dwCurRead(0),
 m_dwCurWrite(0),
 m_dwCurWritePos(0)/*,
 m_CurReadCSec()*/
{
	m_apBuffer = new unsigned char*[dwBlockNum];

	for (int i=0;i<dwBlockNum;i++)
	{
		m_apBuffer[i] = new unsigned char[dwBlockSize];
	}
}
DataBuffer::~DataBuffer()
{
	if (m_apBuffer)
	{
		for (int i=0;i<m_dwBlockNum;i++)
		{
			if (m_apBuffer[i]) delete[] m_apBuffer[i];
		}
		delete[] m_apBuffer;
	}
}

int DataBuffer::WriteData(unsigned char* data,int dwSize)
{
	if (m_dwFreeBlock==0 || dwSize==0)
		return 0;

	if (m_dwBlockSize-m_dwCurWritePos > dwSize)
	{
		memcpy(m_apBuffer[m_dwCurWrite]+m_dwCurWritePos,data,dwSize);
		m_dwCurWritePos += dwSize;
		return dwSize;
	}
	else
	{
		int dwFirstSize = m_dwBlockSize-m_dwCurWritePos;
		int dwSecondSize = dwSize - dwFirstSize;

		memcpy(m_apBuffer[m_dwCurWrite]+m_dwCurWritePos,data,dwFirstSize);

		m_dwCurWrite++;
		m_dwCurWrite %= m_dwBlockNum;
		m_dwCurWritePos = 0;
		m_dwFreeBlock--;

		if (dwSecondSize>0)
			return dwFirstSize+WriteData(data+dwFirstSize,dwSecondSize);
		else 
			return dwFirstSize;
	}
}
int DataBuffer::ReadBlock(unsigned char* buf,int dwBufSize)
{	
	if (m_dwFreeBlock==m_dwBlockNum)
		return 0;

	//_ASSERT(dwBufSize==m_dwBlockSize);

	int dwReadSize = dwBufSize<m_dwBlockSize ? dwBufSize : m_dwBlockSize;

	memcpy(buf,m_apBuffer[m_dwCurRead],dwReadSize);
	m_dwCurRead++;
	m_dwCurRead %= m_dwBlockNum;
	m_dwFreeBlock++;

	return dwReadSize;
}
//int DataBuffer::WriteAndJump(unsigned char* data,int dwSize)
//{
//	if (m_dwFreeBlock==0 || dwSize==0)
//		return 0;
//
//	int dwFirstSize = m_dwBlockSize-m_dwCurWritePos;
//	int dwSecondSize = dwSize - dwFirstSize;
//
//	memcpy(m_apBuffer[m_dwCurWrite],data,dwSize);
//
//	m_dwFreeBlock--;
//	m_dwCurWrite++;
//	m_dwCurWrite %= m_dwBlockNum;
//	m_dwCurWritePos = 0;
//
//	return dwSize;
//}
//int DataBuffer::ReadAndJump(unsigned char* buf,int dwBufSize)
//{
//	if (m_dwFreeBlock==m_dwBlockNum)
//		return 0;
//
//	//_ASSERT(dwBufSize==m_dwBlockSize);
//
//	int dwReadSize = dwBufSize<m_dwBlockSize ? dwBufSize : m_dwBlockSize;
//
//	memcpy(buf,m_apBuffer[m_dwCurRead],dwReadSize);
//	m_dwCurRead++;
//	m_dwCurRead %= m_dwBlockNum;
//	m_dwFreeBlock++;
//
//	return dwReadSize;
//}
bool DataBuffer::ReadyForBufferMore() const
{
	return (m_dwFreeBlock>0);
}
void DataBuffer::Empty()
{
	m_dwCurRead = 0;
	m_dwCurWrite = 0;
	m_dwCurWritePos = 0;
}