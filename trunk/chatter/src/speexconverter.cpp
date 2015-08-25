#include <iostream>

#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"

#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#include "priv/speexconverter.hxx"

using namespace std;

SpeexConverter::SpeexConverter(tInt32 quality, FileLog* filelog )
:	_bits(),_bits2(),_enc_state(NULL),_dec_state(NULL),_eco_state(NULL),_den_state(NULL),
	_encframe_size(0),_decframe_size(0),_skip_group_delay(0),_quality(quality),
	//_encframe_buf(NULL),_decframe_buf(NULL),
	_filelog(filelog)
{
}
SpeexConverter::~SpeexConverter()
{
	UnInit();
}

tInt32	SpeexConverter::Init()
{
	tInt32		sample_rate;
	tInt32		tmp;
	float		ftmp;

	_enc_state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));

	if (_enc_state == NULL)
	{
		LOGERROR("SpeexConverter::Init:Failed to create a encoder.");

		return -1;
	}

	_dec_state = speex_decoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));

	if (_dec_state == NULL)
	{
		LOGERROR("SpeexConverter::Init:Failed to create a decoder.");

		return -2;
	}

	tmp=1;
	speex_decoder_ctl(_dec_state, SPEEX_SET_ENH, &tmp);
	tmp=0;
	//tmp=1;		//Here failed to set it to 1.
	speex_encoder_ctl(_enc_state, SPEEX_SET_VBR, &tmp);
	tmp=8;
	speex_encoder_ctl(_enc_state, SPEEX_SET_QUALITY, &tmp);
	tmp=1;
	speex_encoder_ctl(_enc_state, SPEEX_SET_COMPLEXITY, &tmp);

	//for test
	//speex_encoder_ctl(_enc_state,SPEEX_GET_VAD,&tmp);
	//speex_encoder_ctl(_enc_state,SPEEX_GET_DTX,&tmp);

	tmp = SAMPLE_RATE;
	speex_encoder_ctl(_enc_state, SPEEX_SET_SAMPLING_RATE, &tmp);
	speex_decoder_ctl(_dec_state, SPEEX_SET_SAMPLING_RATE, &tmp);

	 /* Turn this off if you want to measure SNR (on by default) */
	tmp=1;
	speex_encoder_ctl(_enc_state, SPEEX_SET_HIGHPASS, &tmp);
	speex_decoder_ctl(_dec_state, SPEEX_SET_HIGHPASS, &tmp);

	speex_encoder_ctl(_enc_state, SPEEX_GET_LOOKAHEAD, &_skip_group_delay);
	speex_decoder_ctl(_dec_state, SPEEX_GET_LOOKAHEAD, &tmp);
	_skip_group_delay += tmp;

	/**
	 * Get the frame size of the encoder and decoder
	 */
	speex_encoder_ctl(_enc_state,SPEEX_GET_FRAME_SIZE,(void*)&_encframe_size); 
	speex_decoder_ctl(_dec_state,SPEEX_GET_FRAME_SIZE,(void*)&_decframe_size);

	//_encframe_buf = new tInt16[_encframe_size];
	//_decframe_buf = new tInt16[_decframe_size];

	/**
	 * Initialization of echo and preprocess
	 */
	_eco_state = speex_echo_state_init(_encframe_size, TAIL);
	_den_state = speex_preprocess_state_init(_encframe_size, SAMPLE_RATE);
	if (_eco_state == NULL)
	{
		LOGERROR("SpeexConverter::Init:Failed to create a echo state.");
		return -1;
	}
	if (_den_state == NULL)
	{
		LOGERROR("SpeexConverter::Init:Failed to create a preprocessor.");
		return -1;
	}
	sample_rate  = SAMPLE_RATE;
	speex_echo_ctl(_eco_state, SPEEX_ECHO_SET_SAMPLING_RATE, &sample_rate);
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_ECHO_STATE, _eco_state);

	/**
	 * Initialization of denoise
	 */
	tmp=1;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DENOISE, &tmp);
	tmp=0;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_AGC, &tmp);
	tmp=8000;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_AGC_LEVEL, &tmp);
	tmp=0;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB, &tmp);
	ftmp=.0;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &ftmp);
	ftmp=.0;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &ftmp);

	/**
	 * Initialization of the bits struct
	 */
	speex_bits_init(&_bits);
	speex_bits_init(&_bits2);

	return 1;
}

tInt32	SpeexConverter::Code(tByte* src, tUInt32 len, tByte* out, tUInt32 out_buffer_len,void*arg)
{
	tInt32			nvBytes		= 0;
	tInt16*			in_short	= (tInt16*)src;
	tUInt32			len_short	= len/2;
	tUInt32			coded_shorts= 0;
	tUInt32			in_offset	= 0;
	tUInt32			out_offset	= 0;

	//tUInt32			skip_group_delay = _skip_group_delay;

	tUInt32 tmp = 0;
	
	while (in_offset < len)
	{
		// if less than a frame, abandon it.
		if (len - in_offset < (tUInt32)_encframe_size*2)
			break;

		// if out buffer is over.
		if (out_buffer_len-out_offset < sizeof(tUInt32))
			break;

		out_offset += sizeof(tUInt32);

		speex_bits_reset(&_bits);

		tmp = speex_encode_int(_enc_state, (spx_int16_t*)(src+in_offset), &_bits);
		//memcpy(_buf.ca,src+in_offset,_encframe_size*2);
		//tmp = speex_encode_int(_enc_state, _buf.sa, &_bits);

		nvBytes = speex_bits_write(&_bits, (char*)(out+out_offset), out_buffer_len-out_offset);
		*(tUInt32*)(out+out_offset-4) = nvBytes;

		in_offset += _encframe_size*2;
		out_offset += nvBytes;
	}

	return out_offset;
}

tInt32	SpeexConverter::Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 out_buffer_len,void*arg)
{
	tUInt32 nvByte		= 0;
	tUInt32	in_offset	= 0;
	tUInt32	out_offset	= 0;

	tInt32 tmp = 0;

	while (in_offset < len)
	{
		nvByte = *(tUInt32*)(src+in_offset);
		if(nvByte > len-in_offset-sizeof(tUInt32))
		{
			LOGWARNING("SpeexConverter::Decode:Illegal nvByte "<<nvByte);
			break;
		}

		in_offset += sizeof(tUInt32);

		//speex_bits_reset(&_bits2);
		//speex_bits_set_bit_buffer(&_bits2, (void*)(src+in_offset), nvByte);
		speex_bits_read_from(&_bits2, (char*)(src+in_offset), nvByte);

		tmp = speex_decode_int(_dec_state, &_bits2, (tInt16*)(out+out_offset));
		if (tmp != 0)
		{
			LOGWARNING("SpeexConverter::Decode:Speex_decode_int failed with return "<<tmp);
			break;
		}

		//tmp = speex_decode_int(_dec_state, &_bits2, _buf2.sa);
		//memcpy(out+out_offset,(void*)_buf2.ca,(_decframe_size)*2);

		in_offset += nvByte;

		out_offset += _decframe_size*2;
	}

	return out_offset;
}

tInt32 SpeexConverter::Denoise(tByte* src, tUInt32 len)
{
	tUInt32 offset = 0;
	tInt32 tmp = 0;

	while (offset < len)
	{
		if (len - offset < (tUInt32)_decframe_size*2)
		{
			LOGWARNING("SpeexConverter::Denoise:Last less than a frame size "<<_decframe_size*2);
			break;
		}

		speex_preprocess_run(_den_state, (tInt16*)src);

		offset += _encframe_size * 2;
	}

	return offset;
}

tInt32 SpeexConverter::CancelEcho(tByte* ref, tByte* echo, tUInt32 len) 
{
	tUInt32 offset = 0;
	tInt32 tmp = 0;

	LOGDEBUG("SpeexConverter::CancelEcho ref address start:"<<(tUInt32)ref<<",echo adress start:"<<(tUInt32)echo);

	while (offset < len)
	{
		if (len - offset < (tUInt32)_encframe_size*2)
		{
			LOGWARNING("SpeexConverter::CancelEcho:	last less than a frame size "<<_decframe_size*2);
			break;
		}

		//memset(_encframe_buf, 0x00, _encframe_size*2);

		speex_echo_cancellation(_eco_state, (tInt16*)(ref+offset), (tInt16*)(echo+offset),_encframe_buf);
		speex_preprocess_run(_den_state, _encframe_buf);

		memcpy(ref+offset, _encframe_buf, _encframe_size*2);

		offset += _encframe_size*2;
	}

	//LOGDEBUG("SpeexConverter::CancelEcho output is:");
	//for (tUInt32 i=0;i<offset;i+=4)
	//{
	//	LOGDEBUG("CancelEcho: \tdata:"<<*(tInt32*)(((tByte*)_encframe_buf)+i));
	//}

	LOGDEBUG("SpeexConverter::CancelEcho done");

	return offset;
}

tInt32	SpeexConverter::UnInit()
{
	return 0;
}