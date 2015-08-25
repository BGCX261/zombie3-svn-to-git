#ifndef __ACMCONVERTER_HXX__
#define __ACMCONVERTER_HXX__


/**
 * to invalidate the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>

#include "config.hxx"

//#include "converter.hxx"
// WORD wFormatTag = WAVE_FORMAT_ADPCM;
// WORD wFormatTag = WAVE_FORMAT_IMA_ADPCM;
 //WORD wFormatTag = WAVE_FORMAT_GSM610;
// WORD wFormatTag = WAVE_FORMAT_ALAW;
// WORD wFormatTag = WAVE_FORMAT_MULAW;
// WORD wFormatTag = 0x32; // MSN
// WORD wFormatTag = WAVE_FORMAT_DSPGROUP_TRUESPEECH
//#define DEST_FORMAT_TAG		WAVE_FORMAT_DSPGROUP_TRUESPEECH


// Locate a driver that supports a given format and return its ID
//寻找一个支持给定格式的驱动并返回其ID
typedef struct 
{
	HACMDRIVERID hadid;//指向HACMDRIVERID的句柄
	WORD wFormatTag;//32位无符号整型 格式标签
} FIND_DRIVER_INFO;//结构FIND_DRIVER_INFO包含2个元素:HACMDRIVERID hadid 和WORD wFormatTag

BOOL CALLBACK find_format_enum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport);
BOOL CALLBACK find_driver_enum(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);

class FileLog;
class ACMConverter
{
public:
	ACMConverter(tUInt32 dest_format_tag, FileLog* filelog = NULL);

	~ACMConverter();

	/**
	 * Compress the source voice data directly or alterlatively indrectly to PCM then to dest format.
	 * if succeeded, return the length of the result data of dest format, or a Interger no bigger than zero
	 * but here it's from PCM to GSM.
	 */
	virtual tInt32 Code(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out/*,void*arg = NULL*/);

	/**
	 * Decompress compressed voice data, please refer to Code for detail.
	 * it's from GSM to PCM
	 */
	virtual tInt32 Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out/*,void*arg = NULL*/);



private:
	/**
	 * Normally called in constructor
	 */
	tInt32 Init();
	/**
	 * Clean it Up,Normally called in Destructor
	 */
	tInt32 UnInit();

	tInt32 DirectCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);
	tInt32 DirectDecode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);

	tInt32 IndirectCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);
	tInt32 IndirectDecode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);

	tInt32 Src2PCMCode(tByte* src, tUInt32 len);
	tInt32 PCM2DestCode(tByte* out, tUInt32 len_out);
	tInt32 Src2DestCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);

	tInt32 PCM2SrcCode(tByte* out, tUInt32 len_out);
	tInt32 Dest2PCMCode(tByte* src, tUInt32 len);
	tInt32 Dest2SrcCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out);

	//void SetSrcData(tByte* data, tUInt32 len);
	//void SetCompressedData(tByte* data, tUInt32 len);


	HACMDRIVERID find_driver(WORD wFormatTag);
	WAVEFORMATEX* get_driver_format(HACMDRIVERID hadid, WORD wFormatTag);

	HACMDRIVER		_desthad;
	HACMDRIVER		_pcmhad;		// Actually it's null in the sample code,null means using the first matched.

	tUInt32			_dest_format_tag;

	//WAVEFORMATEX	wfsrc;
	//WAVEFORMATEX*	pwfpcm;
	//WAVEFORMATEX*	pwfdest; 

	HACMSTREAM _hstr_src_pcm;
	HACMSTREAM _hstr_pcm_dest;
	HACMSTREAM _hstr_src_dest;

	HACMSTREAM _hstr_pcm_src;
	HACMSTREAM _hstr_dest_pcm;
	HACMSTREAM _hstr_dest_src;

	ACMSTREAMHEADER _strhdr_code;
	ACMSTREAMHEADER _strhdr_decode;

	//tByte* src;
	//tUInt32 len;

	//tByte* src;
	//tUInt32 len;

	tByte* _buffer_pcm;
	tUInt32 _buffer_pcm_size;
	tUInt32 _pcm_length_used;

	tByte* _buffer_pcm2;
	tUInt32 _buffer_pcm_size2;
	tUInt32 _pcm_length_used2;


	FileLog* _filelog;
};

#endif //__ACMCONVERTER_HXX__