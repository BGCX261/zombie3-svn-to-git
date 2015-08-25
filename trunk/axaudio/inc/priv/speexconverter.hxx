#ifndef __SPEEX_CONVERTER_HXX__
#define __SPEEX_CONVERTER_HXX__

#include <speex/speex.h>
#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>
#include "config.hxx"

const tInt32 TWO_BYTES=2;

class FileLog;

class SpeexConverter
{
public:
	SpeexConverter();

	~SpeexConverter();

	tInt32 Code(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out);

	tInt32 Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out);

	void CancelEcho(tByte* ref, const tByte* echo, tByte* outframe);

	void SpeexEchoPlayback(const tByte* frame);
	void SpeexEchoCapture(const tByte* recordframe,short* outframe);
	int SpeexPreprocessRun(const short* recordframe);

	void* GetDecoder(void) const;	// for initialize jitter inside player.

private:
////////////////////////////////////////////////////////////////
// Privete Methods
////////////////////////////////////////////////////////////////
	tBool Init();
	tBool Uninit();

	SpeexBits _bits;
	SpeexBits _bits2;

	void* _enc_state; 
	void* _dec_state;

	SpeexEchoState* _eco_state;
	SpeexPreprocessState* _den_state;

	//union
	//{
	//	char ca[FRAME_SIZE*2];
	//	short sa[FRAME_SIZE];
	//} _buf,_buf2;
};

#endif //__SPEEX_CONVERTER_HXX__