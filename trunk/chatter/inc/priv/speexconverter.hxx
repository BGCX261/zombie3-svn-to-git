#ifndef __SPEEX_CONVERTER_HXX__
#define __SPEEX_CONVERTER_HXX__

#include "converter.hxx"
#include "speex/speex.h"
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#define FRAME_SIZE 160

class FileLog;

class SpeexConverter	:	public Converter
{
public:
	SpeexConverter(tInt32 quality = 5, FileLog* filelog = NULL);

	~SpeexConverter();

	/**
	 * Normally called in converter factory
	 */
	virtual tInt32 Init();

	/**
	 * Compress the source voice data
	 * @src	:	the voice data to compress
	 * @len	:	the length of the voice data to compresse
	 * @out	:	the buffer to store the compressed data
	 * @len_out	:	the length of the out buffer
	 * @arg	:	pointer to additional arguments,here it should be NULL.
	 * @return	:	the length of the compressed data	
	 */
	virtual tInt32	Code(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out,void*arg = NULL);

	/**
	 * Decompress compressed voice data, please refer to Code(...) for detail.
	 */
	virtual tInt32	Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out,void*arg = NULL);

	virtual tInt32 Denoise(tByte* src, tUInt32 len);

	/**
	 * Remove the echo from the voice stream, by cancel out the input stream and the echo.
	 */
	virtual tInt32 CancelEcho(tByte* ref, tByte* echo,tUInt32 len);
	/**
	 * Clean it Up
	 */
	virtual tInt32	UnInit();

private:
	SpeexBits	_bits;
	SpeexBits	_bits2;

	void*		_enc_state; 
	void*		_dec_state;

	SpeexEchoState*			_eco_state;
	SpeexPreprocessState*	_den_state;

	tInt32		_encframe_size;
	tInt32		_decframe_size;
	tInt32		_skip_group_delay;
	tInt32		_quality;

	tInt16		_encframe_buf[160];
	tInt16		_decframe_buf[160];

	FileLog*	_filelog;

	union
	{
		char ca[FRAME_SIZE*2];
		short sa[FRAME_SIZE];
	} _buf,_buf2;
};

#endif //__SPEEX_CONVERTER_HXX__