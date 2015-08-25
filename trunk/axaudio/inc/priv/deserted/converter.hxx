#ifndef __CONVERTER_HXX__
#define __CONVERTER_HXX__

#include "mytypes.h"

class Converter
{
public:
	/*
	Converter();
	~Converter();*/

	/**
	 * Basically to check if this type of converter available
	 * under current enviroment, Yes return zero, Normally called by 
	 * converter factory to check.
	 */
	virtual tInt32 Init() = 0;

	virtual tInt32 Code(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out, void*arg=NULL) = 0;

	virtual tInt32 Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out,void*arg=NULL) = 0;

	virtual tInt32 Denoise(tByte* src, tUInt32 len) = 0;

	virtual tInt32 CancelEcho(tByte* ref, tByte* echo,tUInt32 len) = 0;

	/**
	 * Clean it Up
	 */
	virtual tInt32 UnInit() = 0;
};

#endif //__CONVERTER_HXX__