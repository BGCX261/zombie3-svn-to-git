#pragma once

#include "mytypes.h"

#if defined(WIN32)

#if defined(AXAUDIO_EXPORTS)

#ifdef EXPORT_AXAUDIO
#undef EXPORT_AXAUDIO
#endif

/* DLL export */
#define EXPORT_AXAUDIO __declspec(dllexport)
#else
/* EXE import */
#define EXPORT_AXAUDIO __declspec(dllimport)
#endif	//AXAUDIO_EXPORTS

#else
#define EXPORT_AXAUDIO 
#endif	//WIN32

class WaveAXAudio;
class DAXAudio;
class DDuplexAudio;
class FileLog;

class EXPORT_AXAUDIO AXAudio
{
public:
	///
	// arg1 is for DAXAudio hWnd parameter,SetCooperativeLevel needs this.or you can
	// only have 8-bits sound which is of lower quality with 'hiss...' background.
	AXAudio(void* arg1,FileLog* filelog=NULL);
	~AXAudio();

	tBool Start();
	void Stop();

	void RegisterFrom(string host,tUInt32 port);
	void UnregisterFrom(string host,tUInt32 port);

	tUInt32 GetBlockSize(void) const;
	tUInt32 WriteSound(string host,tUInt32 port, tByte* data, tUInt32 size);
	tUInt32 ReadSound(tByte* buf, tUInt32 bufsize);

	void SetOutVolume(tUInt32 val);
	tUInt32 GetOutVolume(void);
	void SetInVolume(tUInt32 val);
	tUInt32 GetInVolume(void);

	string GetWarning(void);

private:
	AXAudio(const AXAudio& model);

	DDuplexAudio* _impl;
};