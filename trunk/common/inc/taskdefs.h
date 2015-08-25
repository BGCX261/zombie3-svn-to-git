#ifndef __TASK_DEFS_H__
#define __TASK_DEFS_H__
//
//#include <stdio.h>	// FILE
#include "mytypes.h"

typedef struct tagTransFile
{
	string name;
	tUInt32 id;		// normally it's the task id, that's also the transaction id inside the message
	FILE fhandle;	// handle to the open file
	tUInt32 size_total;
	tUInt32 size_transfered;
	tagTransFile* next;
} tTransFile;
//typedef struct tagEndPoints
//{
//	string	pip;
//	tUInt32	pport;
//	string	iip;
//	tUInt32 iport;
//}tEndPoints;

#endif	//__TASK_DEFS_H__