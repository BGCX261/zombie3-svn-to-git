#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>

#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "axaudioexception.hxx"
#include "priv/speexconverter.hxx"

using namespace std;

#pragma comment(lib,"libspeex")
#pragma comment(lib,"libspeexdsp")

SpeexConverter::SpeexConverter()
:	_bits(),_bits2(),
	_enc_state(NULL),_dec_state(NULL),
	_eco_state(NULL),_den_state(NULL)
{
	if (!Init())
		throw AXAudioException("Failed to init SpeexConverter!");
}
SpeexConverter::~SpeexConverter()
{
	Uninit();
}
tInt32	SpeexConverter::Code(tByte* src, tUInt32 len, tByte* out, tUInt32 out_buffer_len)
{
	tInt32 nvBytes = 0;

	speex_bits_reset(&_bits);

	// DTX,no need to be transmitted.
	if (0==speex_encode_int(_enc_state, (short*)src, &_bits))
		return 0;

	nvBytes = speex_bits_write(&_bits, (char*)(out), out_buffer_len);

	return nvBytes;
}

tInt32	SpeexConverter::Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 out_buffer_len)
{
	tInt32 tmp = 0;

	speex_bits_read_from(&_bits2, reinterpret_cast<char*>(src), len);

	tmp = speex_decode_int(_dec_state, &_bits2, (tInt16*)(out));

	if (tmp != 0)
		return 0;

	return FRAME_SIZE*TWO_BYTES;
}
void SpeexConverter::CancelEcho(tByte* ref, const tByte* echo, tByte* outframe) 
{
	tUInt32 offset = 0;
	tInt32 tmp = 0;

	speex_echo_cancellation(_eco_state, (tInt16*)(ref), (tInt16*)(echo),(tInt16*)outframe);

	//speex_preprocess_run(_den_state, (tInt16*)outframe);
}

void SpeexConverter::SpeexEchoPlayback(const tByte* frame)
{
	speex_echo_playback(_eco_state,reinterpret_cast<const spx_int16_t *>(frame));
}
void SpeexConverter::SpeexEchoCapture(const tByte* recordframe,short* outframe)
{
	speex_echo_capture(_eco_state,reinterpret_cast<const spx_int16_t *>(recordframe),outframe);
}
int SpeexConverter::SpeexPreprocessRun(const short* recordframe)
{
	return speex_preprocess_run(_den_state, (tInt16*)recordframe);
}
void* SpeexConverter::GetDecoder(void) const
{
	return _dec_state;
}
tBool SpeexConverter::Init()
{
	int mode;
	tInt32 tmp = 0;
	float ftmp = 0.0;

	speex_bits_init(&_bits);
	speex_bits_init(&_bits2);

	mode = SPEEX_MODEID_NB;

	if (FRAME_SIZE==320)
		mode = SPEEX_MODEID_WB;

	_enc_state = speex_encoder_init(speex_lib_get_mode(mode));
	if (_enc_state == NULL)
		return FALSE;

	_dec_state = speex_decoder_init(speex_lib_get_mode(mode));

	if (_dec_state == NULL)
		return FALSE;

	tmp=1;
	speex_decoder_ctl(_dec_state, SPEEX_SET_ENH, &tmp);
	tmp=1;
	speex_encoder_ctl(_enc_state, SPEEX_SET_VBR, &tmp);
	tmp=SPEEX_QUALITY;
	speex_encoder_ctl(_enc_state, SPEEX_SET_QUALITY, &tmp);
	tmp=2;
	speex_encoder_ctl(_enc_state, SPEEX_SET_COMPLEXITY, &tmp);
	tmp=0;
	speex_encoder_ctl(_enc_state, SPEEX_SET_DTX, &tmp);

	tmp = SAMPLE_RATE;
	speex_encoder_ctl(_enc_state, SPEEX_SET_SAMPLING_RATE, &tmp);
	speex_decoder_ctl(_dec_state, SPEEX_SET_SAMPLING_RATE, &tmp);

	int test_encframe_size=0;
	speex_encoder_ctl(_enc_state,SPEEX_GET_FRAME_SIZE,(void*)&test_encframe_size); 
	//speex_decoder_ctl(_dec_state,SPEEX_GET_FRAME_SIZE,(void*)&test_encframe_size);

	/**
	 * Initialization of echo
	 */
	_eco_state = speex_echo_state_init(FRAME_SIZE, 10*FRAME_SIZE);
	_den_state = speex_preprocess_state_init(FRAME_SIZE, SAMPLE_RATE);
	
	tmp = SAMPLE_RATE;
	speex_echo_ctl(_eco_state, SPEEX_ECHO_SET_SAMPLING_RATE, &tmp);
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_ECHO_STATE, _eco_state);

	tmp=1;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DENOISE, &tmp);
	tmp=1;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_AGC, &tmp);
	ftmp=SPEEX_AGC_LEVEL;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_AGC_LEVEL, &ftmp);
	tmp=1;
	speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_VAD, &tmp);
	//// Not enabled in current version
	//tmp=1;
	//speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB, &tmp);
	//ftmp=0.4;
	//speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB_DECAY, &ftmp);
	//ftmp=0.3;
	//speex_preprocess_ctl(_den_state, SPEEX_PREPROCESS_SET_DEREVERB_LEVEL, &ftmp);

	return 1;
}

tBool SpeexConverter::Uninit()
{
	//speex_jitter_destroy(&_jitter);

	speex_preprocess_state_destroy(_den_state);
	speex_echo_state_destroy(_eco_state);
	
	speex_encoder_destroy(_enc_state);
	speex_decoder_destroy(_dec_state);

	speex_bits_reset(&_bits);
	speex_bits_reset(&_bits2);
	speex_bits_destroy(&_bits);
	speex_bits_destroy(&_bits2);

	return TRUE;
}