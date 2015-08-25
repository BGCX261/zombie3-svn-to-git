#include <iostream>

#include "config.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "priv/acmconverter.hxx"

using namespace std;

// callback function for format enumeration
//用来枚举格式的回调函数
BOOL CALLBACK find_format_enum(HACMDRIVERID hadid, LPACMFORMATDETAILS pafd, DWORD dwInstance, DWORD fdwSupport)//LPACMFORMATDETAILS pafd指向ACM格式详情的指针
{
	FIND_DRIVER_INFO* pdi = (FIND_DRIVER_INFO*) dwInstance;//FIND_DRIVER_INFO类型的指针 指向驱动信息
	if (pafd->dwFormatTag == pdi->wFormatTag) 
	{
		//如果详情中的格式标签==驱动信息指针pdi中的格式标签
		// found it如果能找到
		pdi->hadid = hadid;
		return FALSE; // stop enumerating停止枚举
	}
	return TRUE; // continue enumerating继续枚举
}


// callback function for driver enumeration
BOOL CALLBACK find_driver_enum(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
	FIND_DRIVER_INFO* pdi = (FIND_DRIVER_INFO*) dwInstance;

	// open the driver
	HACMDRIVER had = NULL;//HACMDRIVER类型的变量 had 指向ACM驱动的句柄
	MMRESULT mmr = acmDriverOpen(&had, hadid, 0);
	if (mmr) 
	{
		// some error错误
		return FALSE; // stop enumerating停止枚举
	}
	// enumerate the formats it supports枚举它支持的格式
	DWORD dwSize = 0;
	mmr = acmMetrics((HACMOBJ)had, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
	if (dwSize < sizeof(WAVEFORMATEX)) dwSize = sizeof(WAVEFORMATEX); // for MS-PCM
	WAVEFORMATEX* pwf = (WAVEFORMATEX*) malloc(dwSize);
	memset(pwf, 0, dwSize);
	pwf->cbSize = LOWORD(dwSize) - sizeof(WAVEFORMATEX);
	pwf->wFormatTag = pdi->wFormatTag;
	ACMFORMATDETAILS fd;
	memset(&fd, 0, sizeof(fd));
	fd.cbStruct = sizeof(fd);
	fd.pwfx = pwf;
	fd.cbwfx = dwSize;
	fd.dwFormatTag = pdi->wFormatTag;
	mmr = acmFormatEnum(had, &fd, find_format_enum, (DWORD_PTR)(VOID*)pdi, 0);  
	free(pwf);
	acmDriverClose(had, 0);
	if (pdi->hadid || mmr) {
		// found it or some error找到了 或 有错误
		return FALSE; // stop enumerating停止枚举
	}
	return TRUE; // continue enumeration继续枚举
}


ACMConverter::ACMConverter(tUInt32 dest_format_tag, FileLog* filelog )
:	_desthad(NULL),_pcmhad(NULL),
	_dest_format_tag(dest_format_tag),
	//wfsrc(),pwfpcm(NULL),pwfdest(NULL),
	_hstr_src_pcm(NULL),_hstr_pcm_dest(NULL),_hstr_src_dest(NULL),
	_hstr_pcm_src(NULL),_hstr_dest_pcm(NULL),_hstr_dest_src(NULL),
	_strhdr_code(),_strhdr_decode(),
	//src(NULL),len(0),
	//src(NULL),len(0),
	_buffer_pcm(NULL),_buffer_pcm_size(0),_pcm_length_used(0),
	_buffer_pcm2(NULL),_buffer_pcm_size2(0),_pcm_length_used2(0),
	_filelog(filelog)
{
	memset(&_strhdr_code, 0, sizeof(_strhdr_code));
	_strhdr_code.cbStruct = sizeof(_strhdr_code);

	memset(&_strhdr_decode, 0, sizeof(_strhdr_decode));
	_strhdr_decode.cbStruct = sizeof(_strhdr_decode);
}
ACMConverter::~ACMConverter()
{
	UnInit();
}
tInt32 ACMConverter::Init()
{
	MMRESULT mmr;
	HACMDRIVERID	hadid;
	WAVEFORMATEX	wfsrc;
	WAVEFORMATEX*	pwfpcm;
	WAVEFORMATEX*	pwfdest;

	LOGDEBUG("ACMConverter::Init:Start initialization ...");

	hadid = find_driver(_dest_format_tag);

	if (hadid == NULL) 
	{
		LOGERROR("ACMConverter::Init:Failed to find a driver for format tag "<<_dest_format_tag);

		return -1;
	}

	LOGINFO("Driver found (hadid: "<<hadid);

	// show some information about the driver
	ACMDRIVERDETAILS dd;
	dd.cbStruct = sizeof(ACMDRIVERDETAILS);

	mmr = acmDriverDetails(hadid, &dd, 0);

	LOGINFO("ACMConverter::Init     Short name		:  "<<dd.szShortName);
	LOGINFO("ACMConverter::Init     Long name		:  "<<dd.szLongName);
	LOGINFO("ACMConverter::Init     Copyright		:  "<<dd.szCopyright);
	LOGINFO("ACMConverter::Init     Licensing		:  "<<dd.szLicensing);
	LOGINFO("ACMConverter::Init     Features		:  "<<dd.szFeatures);

	// get the details of the format
	// Note: this is just the first of one or more possible formats for the given tag
	pwfdest = get_driver_format(hadid, _dest_format_tag);

	if (pwfdest == NULL) 
	{
		LOGERROR("ACMConverter::Init:Error getting dest format info\n");

		return -2;
	}

	LOGINFO("ACMConverter::Init:Got Driver format: "<<pwfdest->wBitsPerSample<<" samples per second :"<<pwfdest->nSamplesPerSec);

	// get a PCM format tag the driver supports
	// Note: we just pick the first supported PCM format which might not really
	// be the best choice.
	pwfpcm = get_driver_format(hadid, WAVE_FORMAT_PCM);

	if (pwfpcm == NULL) 
	{
		LOGWARNING("ACMConverter::Init:Failed to get PCM format info\n");
	}
	else
	{
		LOGINFO("ACMConverter::Init:Got PCM format: "<<pwfpcm->wBitsPerSample<<" bits, "<<pwfpcm->nSamplesPerSec<<" samples per second\n");
	}

	// src format, perhaps the src format can be reached by call back argument of waveIn.
	memset(&wfsrc, 0, sizeof(wfsrc));
	wfsrc.cbSize			= 0;
	wfsrc.wFormatTag		= WAVE_FORMAT_PCM; // pcm
	wfsrc.nChannels			= CHANNEL_NB; 
	wfsrc.nSamplesPerSec	= SAMPLE_RATE; // 
	wfsrc.wBitsPerSample	= BITS_PER_SAMPLE;
	wfsrc.nBlockAlign		= wfsrc.nChannels * wfsrc.wBitsPerSample >> 3;	// >>3 actually divided by sizeof(tByte)
	wfsrc.nAvgBytesPerSec	= wfsrc.nSamplesPerSec * wfsrc.nBlockAlign;

	// open the conversion stream
	// Note the use of the ACM_STREAMOPENF_NONREALTIME flag. Without this
	// some software compressors will report error 512 - not possible
	mmr = acmStreamOpen(&_hstr_src_pcm,
			NULL,			// Any driver
			&wfsrc,			// source format
			pwfpcm,			// PCM format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		LOGERROR("ACMConverter::Init:Failed to open a stream to do src format to PCM format conversion,mmr="<<(DWORD)mmr);

		return -3;
	}

	// for decode
	mmr = acmStreamOpen(&_hstr_pcm_src,
			NULL,			// Any driver
			pwfpcm,			// PCM format
			&wfsrc,			// source format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		LOGWARNING("ACMConverter::Init:Failed to open a stream to do PCM format to src format conversion,mmr="<<(DWORD)mmr);

		return -4;
	}


	// open the driver.
	mmr = acmDriverOpen(&_desthad, hadid, 0);

	if (mmr) 
	{
		LOGERROR("ACMConverter::Init:Failed to open driver "<<hadid);

		return -5;
	}

	mmr = acmStreamOpen(&_hstr_pcm_dest,
			_desthad,		// driver handle
			pwfpcm,			// PCM format
			pwfdest,		// destination format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		//ACMERR_NOTPOSSIBLE MMSYSERR_INVALHANDLE
		LOGWARNING("ACMConverter::Init:Failed to open a stream to do PCM format to dest format conversion,mmr="<<(DWORD)mmr);

		return -6;
	}

	// for decode
	mmr = acmStreamOpen(&_hstr_dest_pcm,
			_desthad,		// driver handle
			pwfdest,		// destination format
			pwfpcm,			// PCM format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		LOGWARNING("ACMConverter::Init:Failed to open a stream to do dest format to PCM format conversion,mmr="<<(DWORD)mmr);

		return -7;
	}

	// open the conversion stream from src to dest format, check if it feasible
	mmr = acmStreamOpen(&_hstr_src_dest,
			_desthad,		// driver handle
			&wfsrc,			// source format
			pwfdest,		// destination format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		LOGWARNING("ACMConverter::Init:Failed to open a stream to do src format to dest format conversion,mmr="<<(DWORD)mmr);

		_hstr_src_dest = NULL;
	}

	// open the conversion stream from src to dest format, check if it feasible
	mmr = acmStreamOpen(&_hstr_dest_src,
			_desthad,		// driver handle
			pwfdest,		// destination format
			&wfsrc,			// source format
			NULL,			// no filter
			NULL,			// no callback
			0,				// instance data (not used)
			ACM_STREAMOPENF_NONREALTIME); // flags  
	if (mmr) 
	{
		LOGWARNING("ACMConverter::Init:Failed to open a stream to do dest format to src format conversion,mmr="<<(DWORD)mmr);

		_hstr_dest_src = NULL;
	}

	// Just make them large enough
	_buffer_pcm_size = _buffer_pcm_size2 = static_cast<tUInt32>((wfsrc.nSamplesPerSec*wfsrc.wBitsPerSample>>3)*DURATION*(3/2));

	_buffer_pcm		= new tByte[_buffer_pcm_size];
	_buffer_pcm2	= new tByte[_buffer_pcm_size2];


	LOGDEBUG("ACMConverter::Init:Successfully Initialized");

	return 1;
}
tInt32 ACMConverter::UnInit()
{
	// close the stream and driver关闭流和驱动
	if(_hstr_src_pcm)	acmStreamClose(_hstr_src_pcm, 0);
	if(_hstr_pcm_dest)	acmStreamClose(_hstr_pcm_dest, 0);
	if(_hstr_src_dest)	acmStreamClose(_hstr_src_dest, 0);

	if(_hstr_pcm_src)	acmStreamClose(_hstr_pcm_src, 0);
	if(_hstr_dest_pcm)	acmStreamClose(_hstr_dest_pcm, 0);
	if(_hstr_dest_src)	acmStreamClose(_hstr_dest_src, 0);

	if(_desthad) acmDriverClose(_desthad, 0);

	if (_buffer_pcm)	delete _buffer_pcm;
	if (_buffer_pcm2)	delete _buffer_pcm2;

	return 1;
}

tInt32 ACMConverter::Code(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out,void*arg)
{
	//SetSrcData(src,len);

	if (_hstr_src_dest != NULL)
	{
		return DirectCode(src,len,out, len_out);
	}
	else
	{
		return IndirectCode(src,len,out, len_out);
	}

}

tInt32 ACMConverter::Decode(tByte* src, tUInt32 len, tByte* out, tUInt32 len_out,void*arg)
{
	//SetCompressedData(src,len);

	if (_hstr_dest_src != NULL)
	{
		return DirectDecode(src,len,out, len_out);
	}
	else
	{
		return IndirectDecode(src,len,out, len_out);
	}
}



tInt32 ACMConverter::DirectCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	return Src2DestCode(src,len,out, len_out);
}
tInt32 ACMConverter::DirectDecode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	return 0;
}

tInt32 ACMConverter::IndirectCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	tInt32 ret = 0;

	ret = Src2PCMCode(src,len);

	if (ret <= 0) { return ret;}

	return PCM2DestCode(out, len_out);
}
tInt32 ACMConverter::IndirectDecode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	tInt32 ret = 0;

	ret = Dest2PCMCode(src,len);

	if (ret <= 0) { return ret;}

	return PCM2SrcCode(out, len_out);
}

tInt32 ACMConverter::Src2PCMCode(tByte* src, tUInt32 len)
{
	_pcm_length_used = 0; 

	if (_hstr_src_pcm == NULL) return 0;

	// fill in the conversion info
	_strhdr_code.pbSrc			= src;
	_strhdr_code.cbSrcLength	= len;
	_strhdr_code.pbDst			= _buffer_pcm;
	_strhdr_code.cbDstLength	= _buffer_pcm_size;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_src_pcm, &_strhdr_code, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::Src2PCMCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}


	LOGDEBUG("ACMConverter::Src2PCMCode:Converting to PCM format started.");

	mmr = acmStreamConvert(_hstr_src_pcm, &_strhdr_code, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::Src2PCMCode:Failed to do PCM to driver format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	_pcm_length_used = _strhdr_code.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_src_pcm,&_strhdr_code,0);

	LOGDEBUG("ACMConverter::Src2PCMCode:Converted OK\n src size:"<<len<<" pcm size:"<<_pcm_length_used<<"\ncompression rate:"<<((double)_pcm_length_used/(double)len));	

	return _pcm_length_used;
}
tInt32 ACMConverter::PCM2DestCode(tByte* out, tUInt32 len_out)
{
	tInt32 dest_length_used = 0;

	if (_hstr_pcm_dest == NULL) return 0;

	// fill in the conversion info
	_strhdr_code.pbSrc			= _buffer_pcm;
	_strhdr_code.cbSrcLength	= _pcm_length_used;
	_strhdr_code.pbDst			= out;
	_strhdr_code.cbDstLength	= len_out;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_pcm_dest, &_strhdr_code, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::PCM2DestCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}


	LOGDEBUG("ACMConverter::PCM2DestCode:Converting to final format started.");

	mmr = acmStreamConvert(_hstr_pcm_dest, &_strhdr_code, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::PCM2DestCode:Failed to do PCM to dest format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	dest_length_used = _strhdr_code.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_pcm_dest,&_strhdr_code,0);

	LOGDEBUG("ACMConverter::PCM2DestCode:Converted OK\n pcm size:"<<_pcm_length_used<<" dest size:"<<dest_length_used<<"\ncompression rate:"<<((double)dest_length_used/(double)_pcm_length_used));		


	return dest_length_used;
}
tInt32 ACMConverter::Src2DestCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	tInt32 dest_length_used = 0; // abandon the data in buffer, preparing for convert.

	if (_hstr_src_dest == NULL) return 0;

	_strhdr_code.pbSrc			= src;
	_strhdr_code.cbSrcLength	= len;
	_strhdr_code.pbDst			= out;
	_strhdr_code.cbDstLength	= len_out;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_src_dest, &_strhdr_code, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::DirectCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}


	LOGDEBUG("ACMConverter::DirectCode:Converting to dest format started.");

	mmr = acmStreamConvert(_hstr_src_dest, &_strhdr_code, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::DirectCode:Failed to do PCM to dest format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	dest_length_used = _strhdr_code.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_src_dest,&_strhdr_code,0);

	LOGDEBUG("ACMConverter::DirectCode:Converted OK\n src size:"<<len<<" dest size:"<<dest_length_used<<"\ncompression rate:"<<((double)dest_length_used/(double)len));		


	return dest_length_used;
}
tInt32 ACMConverter::PCM2SrcCode(tByte* out, tUInt32 len_out)
{
	tInt32 src_length_used = 0; // abandon the data in buffer, preparing for convert.

	if (_hstr_pcm_src == NULL) return 0;

	_strhdr_decode.pbSrc		= _buffer_pcm2; // the source data to convert
	_strhdr_decode.cbSrcLength	= _pcm_length_used2;
	_strhdr_decode.pbDst		= out;
	_strhdr_decode.cbDstLength	= len_out;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_pcm_src, &_strhdr_decode, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::PCM2SrcCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}

	// convert the data
	LOGDEBUG("ACMConverter::PCM2SrcCode:Converting from PCM to src started.");

	mmr = acmStreamConvert(_hstr_pcm_src, &_strhdr_decode, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::PCM2SrcCode:Failed to do PCM to src format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	src_length_used = _strhdr_decode.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_pcm_src,&_strhdr_decode,0);

	LOGDEBUG("ACMConverter::PCM2SrcCode:Converted OK\n src size:"<<src_length_used<<" PCM size:"<<_pcm_length_used2<<"\ndecompression rate:"<<((double)_pcm_length_used2/(double)src_length_used));	


	return src_length_used;
}
tInt32 ACMConverter::Dest2PCMCode(tByte* src, tUInt32 len)
{
	_pcm_length_used2 = 0;

	if (_hstr_dest_pcm == NULL) return 0;

	_strhdr_decode.pbSrc		= src; // the source data to convert
	_strhdr_decode.cbSrcLength	= len;
	_strhdr_decode.pbDst		= _buffer_pcm2;
	_strhdr_decode.cbDstLength	= _buffer_pcm_size2;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_dest_pcm, &_strhdr_decode, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::Dest2PCMCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}

	// convert the data
	LOGDEBUG("ACMConverter::Dest2PCMCode:Converting from dest to PCM started.");

	mmr = acmStreamConvert(_hstr_dest_pcm, &_strhdr_decode, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::Dest2PCMCode:Failed to do dest to PCM format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	_pcm_length_used2 = _strhdr_decode.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_dest_pcm,&_strhdr_decode,0);

	LOGDEBUG("ACMConverter::Dest2PCMCode:Converted OK\n dest size:"<<len<<" PCM size:"<<_pcm_length_used2<<"\ndecompression rate:"<<((double)len/(double)_pcm_length_used2));
	

	return _pcm_length_used2;
}
tInt32 ACMConverter::Dest2SrcCode(tByte* src, tUInt32 len,tByte* out, tUInt32 len_out)
{
	tInt32 src_length_used = 0;

	if (_hstr_dest_src == NULL) return 0;

	_strhdr_decode.pbSrc		= src; // the source data to convert
	_strhdr_decode.cbSrcLength	= len;
	_strhdr_decode.pbDst		= out;
	_strhdr_decode.cbDstLength	= len_out;

	// prep the header
	MMRESULT mmr = acmStreamPrepareHeader(_hstr_dest_src, &_strhdr_decode, 0);

	if (mmr)
	{
		LOGERROR("ACMConverter::Dest2SrcCode:Failed to prepare header,mmr="<<(DWORD)mmr);

		return -1;
	}

	// convert the data
	LOGDEBUG("ACMConverter::Dest2SrcCode:Converting to src format started.");

	mmr = acmStreamConvert(_hstr_dest_src, &_strhdr_decode, 0);

	if (mmr) 
	{
		LOGWARNING("ACMConverter::Dest2SrcCode:Failed to do dest to src format conversion,mmr="<<(DWORD)mmr);

		return -2;
	}

	src_length_used = _strhdr_decode.cbDstLengthUsed;

	acmStreamUnprepareHeader(_hstr_dest_src,&_strhdr_decode,0);

	LOGDEBUG("ACMConverter::Dest2SrcCode:Converted OK\n src size:"<<src_length_used<<" dest size:"<<len<<"\ncompression rate:"<<((double)len/(double)src_length_used));		

	
	return src_length_used;
}
//void ACMConverter::SetSrcData(tByte* data, tUInt32 len)
//{
//	src = data;
//
//	len = len;
//}
//void ACMConverter::SetCompressedData(tByte* data, tUInt32 len)
//{
//	src = data;
//
//	len = len;
//}
// locate the first driver that supports a given format tag
HACMDRIVERID ACMConverter::find_driver(WORD wFormatTag)
{
	MMRESULT		 mmr;
	FIND_DRIVER_INFO fdi;

	fdi.hadid		= NULL;
	fdi.wFormatTag	= wFormatTag;

	mmr	= acmDriverEnum(find_driver_enum, (DWORD)(VOID*)&fdi, 0);
	if (mmr) return NULL;

	return fdi.hadid;
}

// get a description of the first format supported for a given tag
WAVEFORMATEX* ACMConverter::get_driver_format(HACMDRIVERID hadid, WORD wFormatTag)
{
	// open the driver打开驱动
	HACMDRIVER	had	= NULL;
	MMRESULT	mmr = acmDriverOpen(&had, hadid, 0);
	if (mmr) 
	{
		return NULL;
	}

	// allocate a structure for the info
	//为信息分配一个结构
	DWORD dwSize = 0;
	mmr = acmMetrics((HACMOBJ)had, ACM_METRIC_MAX_SIZE_FORMAT, &dwSize);
	if (dwSize < sizeof(WAVEFORMATEX)) dwSize = sizeof(WAVEFORMATEX); // for MS-PCM
	WAVEFORMATEX* pwf = (WAVEFORMATEX*) malloc(dwSize);
	memset(pwf, 0, dwSize);
	pwf->cbSize = LOWORD(dwSize) - sizeof(WAVEFORMATEX);
	pwf->wFormatTag = wFormatTag;

	ACMFORMATDETAILS fd;
	memset(&fd, 0x00, sizeof(fd));
	fd.cbStruct		= sizeof(fd);
	fd.pwfx			= pwf;
	fd.cbwfx		= dwSize;
	fd.dwFormatTag	= wFormatTag;

	// set up a struct to control the enumeration
	FIND_DRIVER_INFO fdi;
	fdi.hadid		= NULL;
	fdi.wFormatTag	= wFormatTag;

	mmr = acmFormatEnum(had, &fd, find_format_enum, (DWORD)(VOID*)&fdi, 0); 

	acmDriverClose(had, 0);

	if ((fdi.hadid == NULL) || mmr) 
	{
		free(pwf);

		return NULL;
	}

	return pwf;
}